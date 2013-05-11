#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>

int serverport = 8080;
char *videofile = "video.webm";
char *stylefile = "style.css";
char *scriptfile = "script.js";
char *indexfile = "index.html";

#define ERR(...) \
        fprintf(stderr, __VA_ARGS__);

#define LOG(...) \
        printf(__VA_ARGS__);

static void doexit(evutil_socket_t sig, short events, void *ptr) {
	LOG("\nGot Signal!\n");

	struct event_base *base = (struct event_base *) ptr;

        event_base_loopexit(base, NULL);
}

void sendchunkedfile(char *filename, char *type, struct evhttp_request *req, void *arg) {
        int fd = -1, chunked = 1;
        struct evbuffer *buffer = NULL;
        struct stat st;
        struct evhttp_connection *client = NULL;
        char *addr = NULL;
        int port;

        if ((client = evhttp_request_get_connection(req)) != NULL) {
                evhttp_connection_get_peer(client, &addr,(u_short *) &port);

                LOG("%s:%d -> /%s\n", addr, port, filename);
        }

        struct evkeyvalq *headers = evhttp_request_get_output_headers(req);

        if (evhttp_add_header(headers, "Content-Type", type) < 0) {
                ERR("Unable to set header: Content-Type\n");
                return;
        }

	buffer = evbuffer_new();

        if (stat(filename, &st) != -1) {
		if ((fd = open(filename, O_RDONLY)) != -1) {
			if (st.st_size > 0) {
				if (evbuffer_add_file(buffer, fd, 0, st.st_size) < 0) {
					ERR("Unable to read file! (%s)\n", filename);
				}
			} else {
				// File is empty
				close(fd);
				chunked = 0;
			}
		} else {
			ERR("Unable to open file! (%s: %s)\n", filename, strerror(errno));
			chunked = 0;
		}
	} else {
                ERR("Unable to open file! (%s)\n", filename);
		chunked = 0;
        }

	if (chunked > 0) {
		evhttp_send_reply_start(req, HTTP_OK, "OK");
		evhttp_send_reply_chunk(req, buffer);
		evhttp_send_reply_end(req);
	}

	else {
		evhttp_send_reply(req, HTTP_OK, "OK", 0);
	}

        evbuffer_free(buffer);
}

void handleResponseVideo(struct evhttp_request *req, void *arg) {
	sendchunkedfile(videofile, "video/webm", req, arg);
}

void handleResponseStyle(struct evhttp_request *req, void *arg) {
        sendchunkedfile(stylefile, "text/css", req, arg);
}

void handleResponseScript(struct evhttp_request *req, void *arg) {
        sendchunkedfile(scriptfile, "application/javascript", req, arg);
}

void handleResponse(struct evhttp_request *req, void *arg) {
	sendchunkedfile(indexfile, "text/html", req, arg);
}

int main(int argc, char **argv) {
	int c;
	struct event_base *base = NULL;
	struct event *sig = NULL;
	struct evhttp *http = NULL;

	while ((c = getopt(argc, argv, "iscvp:")) != -1) {
		switch (c) {
			case 'i':
				indexfile = argv[optind];
				break;
			case 's':
				stylefile = argv[optind];
				break;
			case 'c':
				scriptfile = argv[optind];
				break;
			case 'v':
				videofile = argv[optind];
				break;
			case 'p':
				serverport = atoi(optarg);
				break;
			default:
				LOG("Usage: %s [-i ./index.html] [-s ./style.css] [-c ./script.js] [-v ./video.webm] [-p 8080]\n", argv[0]);
				return 1;
		}
	}

        signal(SIGPIPE, SIG_IGN);

	base = event_base_new();
	sig = evsignal_new(base, SIGINT, doexit, base);

	if (event_add(sig, NULL) < 0) {
		ERR("Unable to set signal event!\n");
		return 1;
	}

	http = evhttp_new(base);

	evhttp_bind_socket(http, "0.0.0.0", serverport);

        if (evhttp_set_cb(http, "/", handleResponse, base) < 0) {
                ERR("Unable to set request callback!\n");
                return 1;
        }

        if (evhttp_set_cb(http, "/style.css", handleResponseStyle, base) < 0) {
                ERR("Unable to set request callback!\n");
                return 1;
        }

        if (evhttp_set_cb(http, "/script.js", handleResponseScript, base) < 0) {
                ERR("Unable to set request callback!\n");
                return 1;
        }


	if (evhttp_set_cb(http, "/video.webm", handleResponseVideo, base) < 0) {
		ERR("Unable to set request callback!\n");
		return 1;
	}

	LOG("Listening 0.0.0.0:%d\n", serverport);

        event_base_dispatch(base);

        event_free(sig);
        evhttp_free(http);
        event_base_free(base);

        return 0;
}

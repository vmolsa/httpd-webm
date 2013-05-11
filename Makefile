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

#define FILENAME "./example.webm"

#define PORT 8080

#define ERR(...) \
        fprintf(stderr, __VA_ARGS__);

#define LOG(...) \
        printf(__VA_ARGS__);

static void doexit(evutil_socket_t sig, short events, void *ptr) {
	LOG("\nGot Signal!\n");

	struct event_base *base = (struct event_base *) ptr;

        event_base_loopexit(base, NULL);
}

void handleResponse(struct evhttp_request *req, void *arg) {
	int fd = -1;
	struct evbuffer *buffer = NULL;
	struct stat st;
	struct evhttp_connection *client = NULL;
	char *addr = NULL;
	int port;

	if ((client = evhttp_request_get_connection(req)) != NULL) {
		evhttp_connection_get_peer(client, &addr,(u_short *) &port);

		LOG("%s:%d\n", addr, port);
	} else {
		LOG("Got New Client!\n");
	}

	struct evkeyvalq *headers = evhttp_request_get_output_headers(req);

	if (evhttp_add_header(headers, "Content-Type", "video/webm") < 0) {
		ERR("Unable to set header: Content-Type\n");
		return;
	}

	evhttp_send_reply_start(req, HTTP_OK, "OK");

	if (stat(FILENAME, &st) == -1) {
		ERR("Unable to stat file!\n");
		return;
	}

	if ((fd = open(FILENAME, O_RDONLY)) < 0) {
		ERR("Unable to open movie file! (%s)\n", strerror(errno));
		return;
	}

	buffer = evbuffer_new();

	if (evbuffer_add_file(buffer, fd, 0, st.st_size) < 0) {
		 ERR("Unable to read movie file!\n");
	}

	evhttp_send_reply_chunk(req, buffer);
	evhttp_send_reply_end(req);
	evbuffer_free(buffer);
}

int main(void) {
	struct event_base *base = NULL;
	struct event *sig = NULL;
	struct evhttp *http = NULL;

        signal(SIGPIPE, SIG_IGN);

	base = event_base_new();
	sig = evsignal_new(base, SIGINT, doexit, base);

	if (event_add(sig, NULL) < 0) {
		ERR("Unable to set signal event!\n");
		return 1;
	}

	http = evhttp_new(base);

	evhttp_bind_socket(http, "0.0.0.0", PORT);

	if (evhttp_set_cb(http, "/", handleResponse, base) < 0) {
		ERR("Unable to set request callback!\n");
		return 1;
	}

        event_base_dispatch(base);

        event_free(sig);
        evhttp_free(http);
        event_base_free(base);

        return 0;
}

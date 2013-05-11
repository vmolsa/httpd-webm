httpd-webm
==========

Small HTTP server for streaming webm video file to browser

### Build (ubuntu):

You have to install libevent2 and gcc before you can build this

    sudo apt-get install libevent-dev build-essential

Then just type make in httpd-webm folder
    
    make
    
### WEBM:

Place your webm file to same folder as the httpd-webm file is and rename it to

    example.webm

### Run:

    ./httpd-webm
    
### Open:

    http://localhost:8080

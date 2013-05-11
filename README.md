httpd-webm
==========

Small HTTP server for streaming webm video file to browser

### Requirements (ubuntu):

You have to install libevent2 and gcc before you can build this

    sudo apt-get install libevent-dev build-essential

### Build:
    
    git clone https://github.com/vmolsa/httpd-webm.git
    cd httpd-webm
    make
    
### Webm:

Place your webm file to same folder as the httpd-webm file is and rename it to

    example.webm

### Run:

    ./httpd-webm
    
### Open:

    http://localhost:8080

### License:

[http://opensource.org/licenses/BSD-3-Clause] (BSD-3 Clause License) 

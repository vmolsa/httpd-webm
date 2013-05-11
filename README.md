httpd-webm
==========

Small HTTP server for streaming webm video file to browser

### Requirements (ubuntu):

You have to install libevent2 and gcc before you can build this

    sudo apt-get install libevent-dev build-essential git

### Build:
    
    git clone https://github.com/vmolsa/httpd-webm.git
    cd httpd-webm
    make

### Run:

    ./httpd-webm -v /path/to/your-video.webm
    
### Usage:
    
    Usage: ./httpd-webm [-i ./index.html] [-s ./style.css] [-c ./script.js] [-v ./video.webm] [-p 8080]
    
### Open:

    http://localhost:8080

### License:

[BSD-3 Clause License](http://opensource.org/licenses/BSD-3-Clause) 

Kendall Weihe
CS 371 Spring 2016
Programming Assignment 1

Program description:
  this is a simple HTTP server that handles GET requests
  the server is capable of handling html, txt, jpg, and gif file types
  the server returns the following headers as an example:
  HTTP/1.1 200 OK
  Content-Length: 1986
  Connection: close
  Content-Type: text/html; charset=iso-8859-1

Usage:
  to run the server
      run make clean
      run make
      execute the program with the port number as arg1 and the root path to the HTML directory as arg2
      to load content use web browser or telnet
      
      

Limitations:
    none

Code for TCP socket creation and connection was found from the following source: http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/textcode.html
    The HandleTCPClient.c and TCPEchoServer-Fork.c files were edited and renamed

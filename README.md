# Chat-lab1
Linux online chat room one to one

In this project, you will be writing two C programs that use inter-process communication to implement a multi-user chat
service. For this lab, we will assume that there will be one server process and a single chat client. There are binary
reference versions of these programs located in /home/comp330/lab2 if you would like to experiment or test against my
versions.

Server Program
The server program has two separate components, a relay server and a monitor. The relay server's responsibility is to
wait until there is input from the client, then it will write the message it receives to the monitor process. The monitor
program acts as a local version of the chat client and can be used to participate in the chat.
Each part of the server operation will be handled by separate processes. The server will call fork(), with the parent
acting as the relay server and the child process as the monitor. The parent and child processes must communicate using
pipes, as demonstrated in the sample chat program on Moodle.
Our initial version of the chat program uses a turn-taking approach to communication. When a client connects, we
expect the client to send the rst message to the monitor. Once the message has been relayed to the monitor, input can
be read from the monitor process and relayed back to the client.

Monitor Process
The monitor assumes that the pipe was initialized before the fork and takes two le descriptors as parameters, one for
reading and another for writing. The monitor will also read from the keyboard and write to the screen (STDIN FILENO
and STDOUT FILENO are the built-in le descriptors for each).
The monitor should drop into a loop that should continue until no more input is read, the user has generated an
end-of-le (EOF, or Ctrl-D), or the server has closed the connection. EOF can be detected when a call to read() returns
zero bytes.
For the monitor, during each iteration of the loop you will rst issue a blocking read from the read le descriptor
from the pipe to the relay server. After data has been read, it will be written to standard out. Next, a prompt is printed
to standard output (">"), a message is read from standard input and nally written to the relay server (on the write le
descriptor).

Relay Server
The relay server will use traditional pipes to communicate with the monitor process, but will use a TCP/IP socket to
communicate with the client program.
Upon startup, the server should create a TCP/IP socket, bind it to all of the available TCP/IP interfaces. The port
number may default to a given value, but the user should be able to override the TCP port number with a "-p" option,
see the getopt section below. The port number must be converted to network byte order before calling bind(). You
just need to call htons() on the port number.
Once the socket has been created and bound to an address/port, you should call listen(), then call accept().
When accept returns with a positive value, it means that the server has made a connection to the client and the returned
value is a valid le descriptor that can be used to communicate to the client.
Again, we will use a loop to handle the chatting. The relay server will rst attempt to read() from the remote
client, then write the message to the monitor. Next, a message is read from the monitor and relayed to the client. This
continues indenitely until either the monitor or client sends an EOF.
1
Make sure that your children call exit() and that your parent uses wait() to reap the children. If you don't, you
will soon learn about zombie processes the hard way.
The reference version of the chat program consists of three functions: main(), server(), and monitor() and is
about 200 lines of code. There is a skeleton le that you may use in /home/comp330/lab2.

Client Program
The client program is very similar to the monitor program, except that the client uses a TCP/IP socket to connect to
the server, which only requires one (bidirectional) le descriptor to communicate to the server.
The client must create a new TCP/IP socket(), and then call connect() to attach it to the server's socket. The
socket must bind to the localhost address, and use the same default TCP port as specied in the server. To resolve
the localhost name into an IP address, you should use the gethostbyname() function. The TCP port number should
also be able to be specied by using the same "-p" 
agas the server.
From there, the code should be nearly identical to the monitor process. The reference implementation of the client
contains only a main() function, and is about 90 lines of code.
getopt
To simplify processing command line arguments, we will use the getopt() function. Both programs should support two
options, a "-h" option to print a usage message, and a "-p" option which takes an integer argument to specify the
port number. The manual page (man 3 getopt) contains an example of how to use getopt(). You can use atoi()
to convert the option string to an integer value for the port number.

Sample Output
Server
brian@greenwich $ ./server -h
usage: ./server [-h] [-p port #]
-h - this help message
-p # - the port to use when connecting to the server
brian@greenwich $ ./server -p 5000
hello
> howdy!
^D
brian@greenwich $

Client
brian@greenwich $ ./client -p 5000
connected to server...
> hello
howdy!
> hanging up
brian@greenwich

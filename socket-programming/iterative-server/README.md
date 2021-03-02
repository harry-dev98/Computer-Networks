## Libraries
- socket.h
- sys/types.h
- netinet/in.h (sockaddr_in)
- arpa/inet.h (inet_pton)

## Commands:

` int socket(int domain, int type, int protocol) `
- socket command creates a file descriptor which is endpoint for
- all the communications, fd is returnd

- @domain:
    - domain is a protocol family [all adresses are described here]
    - for eg. AF_UNIX: local comm, AF_INET: ipv4 ip, etc
- @type:
    - specifies what type of communication
    - for eg, SOCK_STREAM: duplex connections, SOCK_DGRAM: connectionless connections
- @protocol:
    - what protocol to be used with socket
    - for eg, generally 0 is used as generally single protocol supports a particular type
- return int:
    - 0 for success
    - -1 for error to read error refer `man socket`

` int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen); `

` int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen); `
- setsockopt/getsockopt helps to set/get the value for particualr option name and 
- at particular level

- @sockfd:
    - socket file descriptor
- @level:
    - level is the level where the option resides, generally its API level [SOL_SOCKET]
    - other levels: TCP, etc.
- @optname:
    - optname and any specified options are passed uninterpreted to the
    - appropriate protocol module for interpretation.
    - eg SO_REUSEADDR SO_REUSEPORT
- @optval:
    - for set its value for the option, for get its null and value if allocated
- @optlen:
    - sizeof optval
- return int:
    - 0: success, -1: error

` int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); `
- on creation of socket, a family of addresses is attached to socket, bind
- allocates address to the socket.
- binding address to a socket is neccessary to start communications
- @sockfd:
    - socket fd
- @addr:
    - struct containing family and address of following format(generally diff for all add fam): [lookhere](https://www.tutorialspoint.com/unix_sockets/socket_structures.htm)

        
        ```cpp
            struct sockaddr_in {
                short int            sin_family;
                unsigned short int   sin_port;
                struct in_addr       sin_addr;
                unsigned char        sin_zero[8];
            };
        ```
- @addrlen:
    - size of address
- return int:
    - 0: sucess, -1: error

` int listen(int sockfd, int backlog) `
- sets socket in listening state i.e looking for connections
- @sockfd:
    - socket fd
- @backlog:
    - #of waiting clients in the queue, while a connection is being processed.
- return int:
    - 0: success, -1: error

`  int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); `
- accepts incomming connection(hence connection based sockets), it take the
- first connection in the queue(this queue is handled by tcp) and binds 
- the connection to a new socket which is for obvious reasons not 
- listening and is unique to a client at an instant. Also is a blocking call
- i.e. waits till there is some connection
- @sockfd:
    - socket fd
- @addr:
    - address of peer, initally is null on succesfull call execution
    - struct is populated with peer address
- @addrlen:
    - pointer to length of addr struct, on execution of call its value is changed
    - to actual size of addr (peer addr)
- return int:
    - fd on success (natural no.) and -1 on error


` ssize_t send(int sockfd, const void *buf, size_t len, int flags); `
` ssize_t recv(int sockfd, void *buf, size_t len, int flags); `

` ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen); `
` ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen); `


` ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags); `
` ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags); `

- to send/recieve messages
- @sockfd:
    - socket fd
- @buff:
    - pointer to buffer to send/ address of buff to recieve
- @len:
    - size of buffer
- @flag:
    - argument that make send/recv different from write/read use `man send` or `man recv` to know more
- @dest_addr:
    - address of reciever/sender
- @addrlen:
    - len of addres struct
- @msg:
    - struct msghdr. to learn more shoot command in ya terminal `man send` or `man recv`

`htos` - converts short into to network byte order

`inet_pton` - converts string of ip to sockaddr struct (internally converts to network byte order)

# OS Project 2 Gropu 21
## Setup
1. Install dependencies:
    ```bash
    sudo apt install build-essential git
    ```
1. Clone this repository and enter into the directory:
    ```bash
    git clone https://github.com/coding-ray/OS-project-2
    cd OS-project-2
    ```

## Build the Server and Client Porgrams
1. Remove old files if necessary:
    ```bash
    make remove
    ```
1. Build the programs:
    ```bash
    make
    ```

## Usage
* Note that the server and the client should be run on the same machine, the same user. Otherwise, the firewall may block the connection.
1. Host the server on *localhost* first:
    ```bash
    # Format
    ./server [port]

    # Example
    ./server 6666
    ```
    * Note: `port` is `6666` by default.
1. Run the client later:
    ```bash
    # Format
    ./client <username> localhost <port>

    # Example
    ./client userA localhost 6666
    ```
    * Note: the port must be the same as port where the server is hosted.


## Demo
1. Set up sessions
![Imgur](https://i.imgur.com/57HQ5py.png)
1. userA sent the message "Hi, I am A." in the green rectangle. And it was then broadcasted to all the clients in the red rectangles.
![Imgur](https://i.imgur.com/wCbvsU4.png)
1. userB sent the message "Hi, this is B." in the green rectangle. And it was then broadcasted to all the clients in the red rectangles.
![Imgur](https://i.imgur.com/qG8q9QB.png)
1. userA quit the chat room. This event was logged in the server.
![Imgur](https://i.imgur.com/RcwudZl.png)

## References
* [TCP Server-Client implementation in C - GeeksforGeeks](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/)
* [Socket Programming in C/C++ - GeeksforGeeks](https://www.geeksforgeeks.org/socket-programming-cc/)
* [Socket Programming in C/C++: Handling multiple clients on server without multi threading - GeeksforGeeks](https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/)
* [eugeneli/Multithreaded-Chat-in-C: Multithreaded chat server/client written in C - Github](https://github.com/eugeneli/Multithreaded-Chat-in-C)
* [Socket 多工方式 - tsnien.idv.tw](http://www.tsnien.idv.tw/Internet_WebBook/chap8/8-8%20Socket%20%E5%A4%9A%E5%B7%A5%E6%96%B9%E5%BC%8F.html)
* [Handling multiple clients on server with multithreading using Socket Programming in C/C++ - GeeksforGeeks](https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/?ref=rp)
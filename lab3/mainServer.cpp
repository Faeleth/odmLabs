// Server side C program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

#define PORT 8080
#define BS 1024

int new_socket;
char buffer[BS] = {0};
mutex mtx;

void read_thread()
{
    ssize_t valread;

    while (1)
    {
        valread = read(new_socket, buffer,
                       BS - 1); // subtract 1 for the null
                                // terminator at the end
        mtx.lock();
        cout << "\033[;42m" << buffer << "\033[0m\n";
        mtx.unlock();

        // Czyszczenie bufora
        for (int i = 0; i < BS; i++)
            buffer[i] = 0;
    }
}

int main(int argc, char const *argv[])
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char hello[BS] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             &addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    thread watek(read_thread);
    while (1)
    {
        std::this_thread::sleep_for(1000ms);
        mtx.lock();
        cout << "> ";
        scanf("%s\n", hello);
        mtx.unlock();
        send(new_socket, hello, strlen(hello), 0);

        for (int i = 0; i < BS; i++)
            hello[i] = 0;
    }

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    close(server_fd);
    return 0;
}

// Client side C program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
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

int client_fd;
char buffer[BS] = {0};
mutex mtx;

void read_thread()
{
    ssize_t valread;

    while (1)
    {
        valread = read(client_fd, buffer,
                       BS - 1); // subtract 1 for the null
                                // terminator at the end
        mtx.lock();
        cout << "\033[;42m" << buffer << "\033[0m\n";
        mtx.unlock();

        for (int i = 0; i < BS; i++)
            buffer[i] = 0;
    }
}

int main(int argc, char const *argv[])
{
    int status;
    struct sockaddr_in serv_addr;
    char hello[BS] = {0};

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr,
                          sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    thread watek(read_thread);

    while (1)
    {
        std::this_thread::sleep_for(1000ms);
        mtx.lock();
        cout << "> ";
        scanf("%s\n", hello);
        mtx.unlock();
        send(client_fd, hello, strlen(hello), 0);

        for (int i = 0; i < BS; i++)
            hello[i] = 0;
    }

    // closing the connected socket
    close(client_fd);
    return 0;
}

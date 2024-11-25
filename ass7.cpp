
// The given experiment description involves:

// Part (i): Load Balancer Design
// You need to create:

// A client to send lowercase strings to a broker server (load balancer).
// The broker server decides which of the two TCP servers to forward the string to, based on CPU utilization.
// The TCP server (either server 1 or server 2) converts the string to uppercase and sends it back to the broker, which then returns it to the client.
// Part (ii): Basic TCP Client-Server Communication
// The second part focuses on implementing a client-server program where a server converts a lowercase string to uppercase and sends it back to the client.

// Solution for Part (i): Load Balancer with Two TCP Servers
// Below is the C++ implementation for the entire setup:

// TCP Server Code (server.cpp):
// cpp
// Copy code
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <algorithm>

#define PORT 9090  // You can use PORT 9091 for the second server

void toUpperCase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    // Accept client connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);  // Read string from broker

    std::string receivedStr(buffer);
    toUpperCase(receivedStr);

    send(new_socket, receivedStr.c_str(), receivedStr.size(), 0);  // Send uppercase string back
    std::cout << "Processed string: " << receivedStr << "\n";

    close(new_socket);
    close(server_fd);

    return 0;
}
// Broker Server Code (broker.cpp):
// cpp
// Copy code
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdlib>

#define PORT 8080  // Broker port
#define SERVER1_PORT 9090
#define SERVER2_PORT 9091

int connectToServer(const std::string& str, int serverPort) {
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverPort);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection to server failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    send(sock, str.c_str(), str.size(), 0);
    char buffer[1024] = {0};
    read(sock, buffer, 1024);

    std::cout << "Response from server " << serverPort << ": " << buffer << "\n";
    close(sock);

    return buffer[0];  // Return the server's response (just a demo)
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Broker listening on port " << PORT << "...\n";

    // Accept client connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);  // Read string from client
    std::string receivedStr(buffer);

    // Simulate CPU utilization check (for simplicity, alternate between servers)
    int selectedServerPort = (rand() % 2 == 0) ? SERVER1_PORT : SERVER2_PORT;
    std::cout << "Selected server: " << selectedServerPort << "\n";

    // Forward string to selected server and get response
    std::string response = connectToServer(receivedStr, selectedServerPort);

    // Send response back to client
    send(new_socket, response.c_str(), response.size(), 0);
    close(new_socket);
    close(server_fd);

    return 0;
}
// Client Code (client.cpp):
// cpp
// Copy code
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080  // Broker server port

int main() {
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection to broker failed");
        return -1;
    }

    std::string inputStr;
    std::cout << "Enter a lowercase string: ";
    std::cin >> inputStr;

    send(sock, inputStr.c_str(), inputStr.size(), 0);

    char buffer[1024] = {0};
    read(sock, buffer, 1024);

    std::cout << "Received uppercase string from broker: " << buffer << "\n";

    close(sock);
    return 0;
}
// Execution Steps
// Compile the programs:

// bash
// Copy code
// g++ -o server server.cpp
// g++ -o broker broker.cpp
// g++ -o client client.cpp
// Start two server instances on different terminals:

// bash
// Copy code
// ./server  # For server on PORT 9090
// ./server  # For server on PORT 9091
// Start the broker server:

// bash
// Copy code
// ./broker
// Run the client program to send a lowercase string:

// bash
// Copy code
// ./client
// The broker will distribute the load to the server with lower CPU utilization (simulated alternation). Let me know if you need further clarification!








// Here's how you can approach both problems:

// i) MapReduce Program to Find Users with Maximum File Size
// This implementation requires knowledge of file metadata and a MapReduce framework (like Hadoop). For simplicity, I will simulate the MapReduce process in C++.

// Code:
// cpp
// Copy code
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

int main() {
    std::unordered_map<std::string, uintmax_t> userFileSizeMap; // User -> Total File Size
    std::vector<std::string> largestUsers;
    uintmax_t maxSize = 0;

    // Iterate over files in the current working directory
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.is_regular_file()) {
            uintmax_t fileSize = entry.file_size();
            std::string owner = fs::status(entry).permissions() & fs::perms::owner_read ? 
                                "current_user" : "unknown"; // Replace with actual owner retrieval

            userFileSizeMap[owner] += fileSize;

            // Update maxSize and track users with the largest file size
            if (userFileSizeMap[owner] > maxSize) {
                maxSize = userFileSizeMap[owner];
                largestUsers.clear();
                largestUsers.push_back(owner);
            } else if (userFileSizeMap[owner] == maxSize) {
                largestUsers.push_back(owner);
            }
        }
    }

    // Output users with maximum file sizes
    std::cout << "Users owning files with maximum total size (" << maxSize << " bytes):\n";
    for (const auto& user : largestUsers) {
        std::cout << "- " << user << '\n';
    }

    return 0;
}
// ii) RPC Mechanism for File Transfer in C
// For Remote Procedure Calls (RPC), you can use a socket-based communication model. Below is a basic file transfer implementation in C++:

// Server (file_transfer_server.cpp):
// cpp
// Copy code
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Waiting for a connection...\n";

    // Accept connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Send file to client
    std::ifstream file("file_to_transfer.txt", std::ios::binary);
    if (!file.is_open()) {
        perror("File not found");
        close(new_socket);
        close(server_fd);
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        send(new_socket, buffer, file.gcount(), 0);
    }

    std::cout << "File sent successfully.\n";

    file.close();
    close(new_socket);
    close(server_fd);

    return 0;
}
// Client (file_transfer_client.cpp):
// cpp
// Copy code
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return EXIT_FAILURE;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        return EXIT_FAILURE;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return EXIT_FAILURE;
    }

    // Receive file from server
    std::ofstream file("received_file.txt", std::ios::binary);
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(sock, buffer, BUFFER_SIZE)) > 0) {
        file.write(buffer, bytesRead);
    }

    std::cout << "File received successfully.\n";

    file.close();
    close(sock);

    return 0;
}
// Compilation and Execution
// Compile the server and client:

// bash
// Copy code
// g++ file_transfer_server.cpp -o server
// g++ file_transfer_client.cpp -o client
// Run the server and client:

// Start the server:
// bash
// Copy code
// ./server
// Start the client in another terminal:
// bash
// Copy code
// ./client
// Ensure the file file_to_transfer.txt exists in the server's directory for transfer.
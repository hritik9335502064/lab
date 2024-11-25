// Task (i): Distributed Application for Factorial Calculation
// In this task, a client sends an integer to a server. The server calculates the factorial of the number and sends the result back to the client. This application will use sockets for communication.

// Server Code (factorial_server.cpp)
// cpp
// Copy code
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080

long long factorial(int n) {
    long long result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        int num;
        read(new_socket, &num, sizeof(num));
        long long result = factorial(num);

        send(new_socket, &result, sizeof(result), 0);
        std::cout << "Processed factorial for number " << num << "\n";

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
// Client Code (factorial_client.cpp)
// cpp
// Copy code
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    int num;
    std::cout << "Enter an integer to calculate factorial: ";
    std::cin >> num;

    send(sock, &num, sizeof(num), 0);

    long long result;
    read(sock, &result, sizeof(result));

    std::cout << "Factorial of " << num << " is: " << result << "\n";

    close(sock);
    return 0;
}
// Task (ii): MapReduce Program to Find Users with Maximum File Size
// This task involves finding the user(s) who own the largest file(s) in the current working directory. We'll simulate a MapReduce program in C++ by splitting the work into mapping, reducing, and reporting phases.

// Code (map_reduce_file_size.cpp)
// cpp
// Copy code
#include <iostream>
#include <filesystem>
#include <map>
#include <vector>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>

namespace fs = std::filesystem;

struct FileInfo {
    std::string owner;
    std::uintmax_t size;
};

// Function to get the owner of a file
std::string getOwner(const fs::path& path) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) != 0) return "unknown";

    struct passwd* pw = getpwuid(fileStat.st_uid);
    return (pw) ? pw->pw_name : "unknown";
}

int main() {
    std::vector<FileInfo> fileInfos;

    // Map phase: Collect file info
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.is_regular_file()) {
            FileInfo info;
            info.owner = getOwner(entry.path());
            info.size = entry.file_size();
            fileInfos.push_back(info);
        }
    }

    // Reduce phase: Find max file size
    std::uintmax_t maxSize = 0;
    for (const auto& file : fileInfos) {
        maxSize = std::max(maxSize, file.size);
    }

    // Report phase: Find owners of max-size files
    std::map<std::string, std::vector<std::string>> ownerToFile;
    for (const auto& file : fileInfos) {
        if (file.size == maxSize) {
            ownerToFile[file.owner].push_back(file.owner);
        }
    }

    // Print results
    std::cout << "Max File Size: " << maxSize << " bytes\n";
    for (const auto& [owner, files] : ownerToFile) {
        std::cout << "Owner: " << owner << " owns file(s) of maximum size\n";
    }

    return 0;
}
// How It Works
// Mapping Phase:

// Traverse the current directory to collect file information, including size and ownership.
// Reducing Phase:

// Determine the maximum file size.
// Reporting Phase:

// Identify users owning the file(s) with the maximum size.
// Let me know if you'd like further clarification or refinements!
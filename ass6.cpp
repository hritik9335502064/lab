// The tasks involve implementing three different client-server applications using C++. Below are the solutions:

// Task (i): Concurrent Echo Client-Server Application
// This involves creating a server that echoes back whatever data it receives from multiple clients simultaneously. This requires multithreading on the server side.

// Server Code (echo_server.cpp)
// cpp
// Copy code
#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    while (true) {
        int bytesRead = read(clientSocket, buffer, 1024);
        if (bytesRead <= 0) break;
        std::cout << "Client: " << buffer << std::endl;
        send(clientSocket, buffer, bytesRead, 0);
        memset(buffer, 0, sizeof(buffer));
    }
    close(clientSocket);
}

int main() {
    int server_fd, clientSocket;
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
        if ((clientSocket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        std::cout << "New client connected!\n";
        std::thread(handleClient, clientSocket).detach();
    }

    close(server_fd);
    return 0;
}
// Client Code (echo_client.cpp)
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

    std::string message;
    char buffer[1024] = {0};

    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);
        send(sock, message.c_str(), message.size(), 0);

        if (message == "exit") break;

        int bytesRead = read(sock, buffer, 1024);
        std::cout << "Server: " << buffer << std::endl;
        memset(buffer, 0, sizeof(buffer));
    }

    close(sock);
    return 0;
}
// Task (ii): Master Table Update
// This task requires maintaining a master table of connected clients. The server updates the table with client information and sends it back.

// Server Code (master_table_server.cpp)
// cpp
// Copy code
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8081

struct Node {
    int nodeNumber;
    std::string ipAddress;
    int portNumber;
};

std::vector<Node> masterTable;

void addNodeToTable(int nodeNumber, const std::string& ipAddress, int portNumber) {
    masterTable.push_back({nodeNumber, ipAddress, portNumber});
}

std::string getTableAsString() {
    std::string table = "Node No.\tIP Address\tPort No.\n";
    for (const auto& node : masterTable) {
        table += std::to_string(node.nodeNumber) + "\t" + node.ipAddress + "\t" + std::to_string(node.portNumber) + "\n";
    }
    return table;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

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

    std::cout << "Server is listening on port " << PORT << "...\n";

    int nodeCount = 1;
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);

        std::string clientIP = inet_ntoa(address.sin_addr);
        int clientPort = ntohs(address.sin_port);
        addNodeToTable(nodeCount++, clientIP, clientPort);

        std::string table = getTableAsString();
        send(new_socket, table.c_str(), table.size(), 0);

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
// Task (iii): Date-Time Server
// The server sends the current date, time, and CPU load to connected clients.

// Server Code (datetime_server.cpp)
// cpp
// Copy code
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/sysinfo.h>

#define PORT 8082

std::string getDateTime() {
    time_t now = time(0);
    tm* localtm = localtime(&now);
    return asctime(localtm);
}

double getCPULoad() {
    struct sysinfo sysInfo;
    sysinfo(&sysInfo);
    return (sysInfo.loads[0] / 65536.0) * 100;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

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

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        std::string dateTime = "Date & Time: " + getDateTime();
        std::string cpuLoad = "CPU Load: " + std::to_string(getCPULoad()) + "%";
        std::string response = dateTime + cpuLoad;

        send(new_socket, response.c_str(), response.size(), 0);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
// Let me know which part you'd like further elaboration on!








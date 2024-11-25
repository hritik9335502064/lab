
// Here are the requested implementations:

// Task (i): UDP Client Program
// This program demonstrates a UDP client that uses sendto() to send a string to a server.

// UDP Client in C
// c
// Copy code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    char *message = "Hello from UDP client!";
    struct sockaddr_in server_addr;

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with actual server IP if needed

    // Send message to server
    int n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("Send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Message sent to server: %s\n", message);

    // Receive response from server
    socklen_t len = sizeof(server_addr);
    n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &len);
    if (n < 0) {
        perror("Receive failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("Server response: %s\n", buffer);

    // Close the socket
    close(sockfd);
    return 0;
}
// Task (ii): Distributed Application Using MPI
// This example demonstrates how to design a distributed application using the Message Passing Interface (MPI). The program distributes computation of the square of integers across multiple processes.

// MPI Program in C++
// cpp
// Copy code
#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get the total number of processes

    const int data_size = 10; // Number of elements to process
    std::vector<int> data;
    std::vector<int> results;

    if (rank == 0) {
        // Master process initializes the data
        data.resize(data_size);
        for (int i = 0; i < data_size; i++) {
            data[i] = i + 1;
        }
        results.resize(data_size);
        std::cout << "Master process broadcasting data to all processes.\n";
    }

    // Scatter the data to all processes
    int chunk_size = data_size / size;
    std::vector<int> local_data(chunk_size);
    MPI_Scatter(data.data(), chunk_size, MPI_INT, local_data.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process performs computation on its chunk
    std::vector<int> local_results(chunk_size);
    for (int i = 0; i < chunk_size; i++) {
        local_results[i] = std::pow(local_data[i], 2); // Compute square
    }

    // Gather results from all processes
    MPI_Gather(local_results.data(), chunk_size, MPI_INT, results.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Master process collects results and displays them
        std::cout << "Results (Squares of integers): ";
        for (int i = 0; i < data_size; i++) {
            std::cout << results[i] << " ";
        }
        std::cout << "\n";
    }

    MPI_Finalize();
    return 0;
}
// How to Compile and Run
// For Task (i): UDP Client
// Save the program as udp_client.c.
// Compile using:
// bash
// Copy code
// gcc -o udp_client udp_client.c
// Make sure you have a UDP server running on the specified IP and port (e.g., 127.0.0.1:8080).
// Run the client:
// bash
// Copy code
// ./udp_client
// For Task (ii): MPI Application
// Save the program as mpi_example.cpp.
// Compile using:
// bash
// Copy code
// mpic++ -o mpi_example mpi_example.cpp
// Run using multiple processes (e.g., 4 processes):
// bash
// Copy code
// mpirun -np 4 ./mpi_example
// These programs implement distributed computation using both UDP sockets and MPI for practical applications in distributed systems. Let me know if you need assistance with additional concepts!











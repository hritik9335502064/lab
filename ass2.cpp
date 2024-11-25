// Task (i): Determine CPU Load of Node B from Node A
// This program calculates the CPU load of a remote node by sending a request from node A to node B. Node B computes its CPU usage and sends it back to Node A.

// Node A: CPU Load Client (C Program)
// c
// Copy code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Node B's IP address

    // Connect to server (Node B)
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send request for CPU load
    char *message = "GET_CPU_LOAD";
    send(sockfd, message, strlen(message), 0);

    // Receive response
    int n = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("CPU Load of Node B: %s%%\n", buffer);
    } else {
        printf("Failed to receive data from Node B\n");
    }

    close(sockfd);
    return 0;
}
// Node B: CPU Load Server (C Program)
// c
// Copy code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to calculate CPU load
float get_cpu_load() {
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0) {
        perror("sysinfo");
        return -1;
    }
    float load = sys_info.loads[0] / 65536.0; // Convert load to percentage
    return load * 100;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    listen(server_fd, 5);
    printf("Server listening on port %d...\n", PORT);

    socklen_t len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
    if (client_fd < 0) {
        perror("Connection failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Handle client request
    read(client_fd, buffer, BUFFER_SIZE);
    if (strcmp(buffer, "GET_CPU_LOAD") == 0) {
        float load = get_cpu_load();
        snprintf(buffer, BUFFER_SIZE, "%.2f", load);
        send(client_fd, buffer, strlen(buffer), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
// Task (ii): Server Using Shared Memory and Semaphore
// This program demonstrates a server that increments a counter in shared memory using a semaphore to ensure synchronization.

// Server (C Program)
// c
// Copy code
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/shared_mem"
#define SEM_NAME "/sem_counter"
#define MEM_SIZE sizeof(int)

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("Shared memory creation failed");
        exit(EXIT_FAILURE);
    }

    ftruncate(shm_fd, MEM_SIZE);

    int *counter = (int *)mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("Memory mapping failed");
        exit(EXIT_FAILURE);
    }

    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Semaphore creation failed");
        exit(EXIT_FAILURE);
    }

    // Increment counter
    for (int i = 0; i < 10; i++) {
        sem_wait(sem);
        (*counter)++;
        printf("Server incremented counter: %d\n", *counter);
        sem_post(sem);
        sleep(1);
    }

    // Cleanup
    sem_close(sem);
    sem_unlink(SEM_NAME);
    munmap(counter, MEM_SIZE);
    shm_unlink(SHM_NAME);

    return 0;
}
// Task (iii): Client Reading Shared Memory
// This program demonstrates how the client reads the counter value from shared memory.

// Client (C Program)
// c
// Copy code
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/shared_mem"
#define SEM_NAME "/sem_counter"
#define MEM_SIZE sizeof(int)

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd < 0) {
        perror("Shared memory opening failed");
        exit(EXIT_FAILURE);
    }

    int *counter = (int *)mmap(0, MEM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("Memory mapping failed");
        exit(EXIT_FAILURE);
    }

    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("Semaphore opening failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++) {
        sem_wait(sem);
        printf("Client read counter value: %d\n", *counter);
        sem_post(sem);
        sleep(1);
    }

    sem_close(sem);
    munmap(counter, MEM_SIZE);

    return 0;
}
// How to Compile and Run
// Compile both server and client:

// bash
// Copy code
// gcc server.c -o server -lrt -pthread
// gcc client.c -o client -lrt -pthread
// Run the server first:

// bash
// Copy code
// ./server
// Run the client in another terminal:

// bash
// Copy code
// ./client
// These programs demonstrate CPU load measurement, shared memory, and semaphore usage in distributed systems. Let me know if you need further assistance!








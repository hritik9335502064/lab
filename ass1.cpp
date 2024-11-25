// Here are implementations for your problems in C++ using pipes for inter-process communication.

// Task (i): Process Communication with String Manipulation Using Pipes
// This program creates two processes where the first process sends a string to the second. The second process concatenates the string and sends it back to the first process for printing.

// Code
// cpp
// Copy code
#include <iostream>
#include <unistd.h>
#include <cstring>

using namespace std;

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid;

    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    // Fork process
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process (Second Process)
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2

        char receivedString[100];
        read(pipe1[0], receivedString, sizeof(receivedString));

        // Concatenate received string with another string manually
        const char* additionalString = " concatenated string";
        int i = 0, j = 0;
        while (receivedString[i] != '\0') i++; // Find the end of received string
        while (additionalString[j] != '\0') {
            receivedString[i++] = additionalString[j++];
        }
        receivedString[i] = '\0';

        write(pipe2[1], receivedString, strlen(receivedString) + 1);
        close(pipe1[0]);
        close(pipe2[1]);
    } else {
        // Parent process (First Process)
        close(pipe1[0]); // Close read end of pipe1
        close(pipe2[1]); // Close write end of pipe2

        const char* inputString = "Initial string";
        write(pipe1[1], inputString, strlen(inputString) + 1);

        char resultString[100];
        read(pipe2[0], resultString, sizeof(resultString));
        cout << "Result received from child: " << resultString << endl;

        close(pipe1[1]);
        close(pipe2[0]);
    }

    return 0;
}
// Task (ii): Process Communication with Matrix Addition Using Pipes
// This program sends two matrices from the parent process to the child process. The child computes the sum and sends it back to the parent for printing.

// Code
// cpp
// Copy code
#include <iostream>
#include <unistd.h>
#include <cstring>

using namespace std;

#define SIZE 2 // Matrix size (2x2)

void printMatrix(int matrix[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid;

    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    // Fork process
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2

        int matrixA[SIZE][SIZE], matrixB[SIZE][SIZE], result[SIZE][SIZE];
        read(pipe1[0], matrixA, sizeof(matrixA));
        read(pipe1[0], matrixB, sizeof(matrixB));

        // Compute matrix sum
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                result[i][j] = matrixA[i][j] + matrixB[i][j];
            }
        }

        write(pipe2[1], result, sizeof(result));
        close(pipe1[0]);
        close(pipe2[1]);
    } else {
        // Parent process
        close(pipe1[0]); // Close read end of pipe1
        close(pipe2[1]); // Close write end of pipe2

        int matrixA[SIZE][SIZE] = {{1, 2}, {3, 4}};
        int matrixB[SIZE][SIZE] = {{5, 6}, {7, 8}};
        int result[SIZE][SIZE];

        // Send matrices to child
        write(pipe1[1], matrixA, sizeof(matrixA));
        write(pipe1[1], matrixB, sizeof(matrixB));

        // Receive result from child
        read(pipe2[0], result, sizeof(result));

        cout << "Matrix A:" << endl;
        printMatrix(matrixA);

        cout << "Matrix B:" << endl;
        printMatrix(matrixB);

        cout << "Sum of Matrices:" << endl;
        printMatrix(result);

        close(pipe1[1]);
        close(pipe2[0]);
    }

    return 0;
}
// How to Compile and Run
// Compile the programs:

// bash
// Copy code
// g++ string_pipe.cpp -o string_pipe
// g++ matrix_pipe.cpp -o matrix_pipe
// Run the programs:

// bash
// Copy code
// ./string_pipe
// ./matrix_pipe
// Explanation
// Task (i):

// A pipe is used to transfer a string from the parent to the child process.
// The child manually concatenates another string without using any library functions and sends it back to the parent.
// Task (ii):

// Two matrices are sent from the parent process to the child process via a pipe.
// The child computes the sum of these matrices and sends the result back to the parent for printing.
// Let me know if you need further clarifications!







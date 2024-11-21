/*
 ============================================================================
 Name        : Client_exam.c
 Author      : Giovanni Giancaspero
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#if defined WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "protocol.h"

// Error handler function
void errorhandler(const char *errorMessage) {
    fprintf(stderr, "%s", errorMessage);
}

// Clears Winsock if running on Windows
void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

// Removes newline characters from a string
void removeNewLine(char* str) {
    // Find the length of the string
    size_t len = strlen(str);

    // If the string is not empty and ends with '\n', remove the '\n'
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0'; // Remove the newline character
    } else {
        // If the string is too long and there's no newline, clear the buffer
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
    }

    // Check for multiple '\n' characters if input is multiline
    while (strlen(str) > 0 && str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0'; // Remove any additional '\n'
    }
}

// Validates the input string for correct command format
int control_string(char* selection) {
    int length = strlen(selection);

    // Check that the length is not 1 or 3-4 characters
    if (length != 1 && length != 3 && length != 4) {
        return 1;  // Error: Invalid input
    }

    if (length == 1 && selection[0] != 'q') {
        return 1;  // Error: Invalid single character input
    }

    // Verify that the first character is one of 'n', 'a', 'm', 's', or 'q'
    if (selection[0] != 'n' && selection[0] != 'a' &&
        selection[0] != 'm' && selection[0] != 's' && selection[0] != 'q') {
        return 1;  // Error: Invalid command character
    }

    // Check that the second character is a space (if it exists)
    if (length > 1 && !isspace(selection[1])) {
        return 1;  // Error: Missing space after the command
    }

    // Handles length of 3 or 4 for numeric value validation
    if (length == 4 || length == 3) {
        int num_value;

        // Length 4: Verify that selection[2] and selection[3] are digits
        if (length == 4 && isdigit(selection[2]) && isdigit(selection[3])) {
            num_value = (selection[2] - '0') * 10 + (selection[3] - '0');
            if (num_value < 10 || num_value > 32) {
                return 1;  // Error: Number out of range
            }
        }
        // Length 3: Verify that selection[2] is a digit
        else if (length == 3 && isdigit(selection[2])) {
            num_value = selection[2] - '0';
            if (num_value < 6 || num_value > 9) {
                return 1;  // Error: Number out of range
            }
        } else {
            return 1;  // Error: Invalid numeric format
        }
    }

    return 0;  // Valid input
}

int main(void) {
#if defined WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        errorhandler("WSAStartup() failed.\n");
        return -1;
    }
#endif

    // Create the client socket
    clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0) {
        errorhandler("Socket creation failed.\n");
        clearwinsock();
        return -1;
    }

    //struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(CLIENT_IP);
    sad.sin_port = htons(PROTOPORT);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        errorhandler("Connection to the server failed.\n");
        closesocket(clientSocket);
        clearwinsock();
        return -1;
    }

    // CONNECTION TO THE SERVER AND CREATION OF THE PASSWORD FORMAT
    printf("Connection to the server established.\n");
    fflush(stdout);

    // Main loop for user input
    while (1) {
        printf("Insert the letter for the type of password you want to generate, followed by a blank space and the password length (from 8 to 32).\n");
        printf("n:\t numeric\na:\t alphabetic\nm:\t mixed\ns:\t secured\nq:\t close the connection\n");
        fflush(stdout);

        char choice[5] = " ";
        int control = 1;
        while (control == 1) {
            fgets(choice, 5, stdin);
            removeNewLine(choice);
            control = control_string(choice);
            if (control == 1) {
                printf("Wrong input, PLEASE TRY AGAIN!\n");
                fflush(stdout);
            } else {
                control = 0;
            }
        }

        // Handle quit command
        if (choice[0] == 'q' || choice[0] == 'Q') {
            printf("Connection to the server closed.\n");
            fflush(stdout);
            closesocket(clientSocket);
            clearwinsock();
            return 0;
        } else {
            send(clientSocket, choice, strlen(choice), 0);
        }

        // RECEIVING THE PASSWORD FROM THE SERVER
        char buff[BUFFERSIZE];
        int bytesReceived = recv(clientSocket, buff, BUFFERSIZE - 1, 0);
        if (bytesReceived <= 0) {
            errorhandler("Error receiving the first string.\n");
            closesocket(clientSocket);
            continue;
        }

        buff[bytesReceived] = '\0'; // Null-terminate the received string
        printf("Password received\nNew password:\t%s.\n", buff);
        fflush(stdout);
    }
}


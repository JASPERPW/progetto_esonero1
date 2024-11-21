/*
 ============================================================================
 Name        : Server_exam.c
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


void errorhandler(const char *errorMessage) {
    fprintf(stderr, "%s", errorMessage);
}

void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

void generate_numeric(char* pass, int length) {
    // Generate a random password
    for (int i = 0; i < length; i++) {
        pass[i] = '0' + (rand() % 10);  // casual number from 0 to 9
    }

    // add the end character to the string
    pass[length] = '\0';
}

void generate_alpha(char* pass, int length) {
	const char charset_lowercase[] = "abcdefghijklmnopqrstuvwxyz";
	const int charset_lowercase_size = sizeof(charset_lowercase) - 1;
    // Generate password with lowercase letters only
    for (int i = 0; i < length; i++) {
        pass[i] = charset_lowercase[rand() % charset_lowercase_size];
    }

    // add the end character to the string
    pass[length] = '\0';
}

void generate_mixed(char* pass, int length){
	const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	const int charset_size = sizeof(charset) - 1;
	// Generate random alphanumeric password
	    for (int i = 0; i < length; i++) {
	        pass[i] = charset[rand() % charset_size];
	    }

	    // add the end character to the string
	    pass[length] = '\0';
}

void generate_secure(char* pass, int length) {
	const char charset_secure[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}|;:',.<>?/~`";
	const int charset_secure_size = sizeof(charset_secure) - 1;
    // Generate password with random characters from complete charset
    for (int i = 0; i < length; i++) {
        pass[i] = charset_secure[rand() % charset_secure_size];
    }

    // add the end character to the string
    pass[length] = '\0';
}

int main(void) {
#if defined WIN32
    WSADATA wsaData;
    // Initialize Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        errorhandler("WSAStartup() failed.\n");
        return -1;
    }
#endif

    // Create a TCP socket
    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) {
        errorhandler("Socket creation failed.\n");
        clearwinsock();
        return -1;
    }

    // Configure the socket address structure
    //struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;              // Use IPv4
    sad.sin_addr.s_addr = inet_addr(SERVER_IP); // Bind to localhost
    sad.sin_port = htons(PROTOPORT);       // Set the port number

    // Bind the socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        errorhandler("Bind() failed.\n");
        closesocket(serverSocket);
        clearwinsock();
        return -1;
    }

    // Put the server socket into listening mode
    if (listen(serverSocket, QLEN) < 0) {
        errorhandler("Listen() failed.\n");
        closesocket(serverSocket);
        clearwinsock();
        return -1;
    }

    printf("Server listening on port %d...\n", PROTOPORT);
    fflush(stdout);

    struct sockaddr_in cad;  // Structure for the client address
    int clientSocket;
    int clientLen = sizeof(cad);

    // Main loop to accept client connections
    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&cad, &clientLen);
        if (clientSocket < 0) {
            // Log the error and refuse connection
        	printf("Queue full, Connection from %s refused.\n", inet_ntoa(cad.sin_addr));
        	fflush(stdout);
            continue; // go to the start of the cicle to accept new connections
        }

        // Log the new connection
        printf("New connection from %s:%d\n", inet_ntoa(cad.sin_addr), ntohs(cad.sin_port));
        fflush(stdout);

        // Handle client commands
        int controlp = 1;
        while (controlp == 1) {
            char choice[BUFFERSIZE] = "";
            // Receive a command from the client
            int bytesReceived = recv(clientSocket, choice, BUFFERSIZE - 1, 0);
            if (bytesReceived <= 0) {
                printf("Error receiving data or connection closed by client.\n");
                fflush(stdout);
                closesocket(clientSocket);
                break; // Exit the loop if the connection is closed or an error occurs
            }
            choice[bytesReceived] = '\0'; // Null-terminate the received string

            // Determine the length of the string to generate
            int string_lenght;
            if (strlen(choice) == 3) {
                string_lenght = choice[2] - '0';
            } else {
                char x[3] = { choice[2], choice[3], '\0' };
                string_lenght = atoi(x);
            }

            char password[string_lenght + 1]; // Buffer to store the generated password

            // Process the client's command
            switch (choice[0]) {
                case 'n': // Numeric password
                    generate_numeric(password, string_lenght);
                    break;
                case 'a': // Alphabetic password
                    generate_alpha(password, string_lenght);
                    break;
                case 'm': // Mixed password
                    generate_mixed(password, string_lenght);
                    break;
                case 's': // Secure password
                    generate_secure(password, string_lenght);
                    break;
                default: // Invalid command
                    printf("Invalid command received.\n");
                    fflush(stdout);
                    break;
            }

            // Send the generated password back to the client if still connected
            if (controlp == 1) {
                send(clientSocket, password, strlen(password), 0);
            }
        }
    }

    return EXIT_SUCCESS;
}


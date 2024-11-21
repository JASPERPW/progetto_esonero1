/*
 * protocol.h
 *
 *  Created on: 21 nov 2024
 *      Author: Giovanni Giancaspero
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

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
#include <stdbool.h>
#include <ctype.h>

int clientSocket;
struct sockaddr_in sad;

#define CLIENT_IP "127.0.0.1"
#define PROTOPORT 57015 // client Port
#define BUFFERSIZE 512 // buffer dimension

void errorhandler(const char *errorMessage);
void clearwinsock();
void rimuoviNewLine(char* str);
int control_string(char* selection);

#endif /* PROTOCOL_H_ */

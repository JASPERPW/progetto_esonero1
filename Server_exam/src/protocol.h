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
#include <ctype.h>

#define SERVER_IP "127.0.0.1"
#define PROTOPORT 57015 // Server Port
#define BUFFERSIZE 512 // Buffer dimension
#define QLEN 5 // maximum users number accepted


int serverSocket;
struct sockaddr_in sad;

void errorhandler(const char *errorMessage);
void clearwinsock();
void generate_numeric(char* pass, int length);
void generate_alpha(char* pass, int length);
void generate_mixed(char* pass, int length);
void generate_secure(char* pass, int length);

#endif /* PROTOCOL_H_ */

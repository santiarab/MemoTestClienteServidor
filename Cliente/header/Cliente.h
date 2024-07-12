#ifndef CLIENTE_H
#define CLIENTE_H

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <csignal>

using namespace std;

void verificarParamEntrada(int argc, char *argv[], int &puerto, string &nickname, string &servidor);
int conectandoAlServidor(int puerto, string ip);
void mandarPeticiones(int socketComunicacion);
void imprimirTableroCompleto(char tablero[][4]);
void recibirMatrix(int socketId, char tablero[][4]);
void enviarCordenadas(int socketId);
void juegoMemotest(int socketId);
bool corroborarCordenadas(string valor, char tablero[][4]);
vector<string> split(string str, char pattern);
void recibir(int socketId, char &signal);
bool esIp(string servidor);
bool esNumero(const string &str);
void mostrarAyuda();

#endif
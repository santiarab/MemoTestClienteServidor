#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <queue>
#include <string>
#include <random>
#include <vector>
#include <algorithm>
#include <iomanip>

#define NROCARTAS 8

using namespace std;

extern char tableroCompleto[4][4];
extern char tableroCompletando[4][4];
extern char tableroErroneo[4][4];

typedef struct
{
    int puntos;
    int socketComunicacion;
} clientes;

void verificarParamEntrada(int argc, char *argv[], int &puerto, int &jugadores);
int crearServidor(int puerto, int jugadores);
void escucharClientes(int socketEscucha, int jugadores);
bool elegirValor(string arg1, string arg2);
void crearTablero();
bool existeUnValor(char valor);
vector<string> split(string str, char pattern);
void enviarMatrix(clientes cliente, char tablero[4][4]);
bool recivirCordenadas(clientes &cliente, int &cantCartas);
void mostrarTablero(char tablero[4][4]);
void copiarMatriz(char matrizOrigen[4][4], char matrizDestino[4][4]);
void enviarMensaje(int socketId, char msj);
vector<clientes> colocoEnVector(queue<clientes> colaClientes);
int cmpPuntos(const clientes &arg1, const clientes &arg2);
int mayorValor(vector<clientes> vecCliente);
void mostrarAyuda();

#endif
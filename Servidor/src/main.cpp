#include "Servidor.h"

char tableroCompleto[4][4] = {{0, 0, 0, 0},
                              {0, 0, 0, 0},
                              {0, 0, 0, 0},
                              {0, 0, 0, 0}};
char tableroCompletando[4][4] = {{0, 0, 0, 0},
                                 {0, 0, 0, 0},
                                 {0, 0, 0, 0},
                                 {0, 0, 0, 0}};
char tableroErroneo[4][4] = {{0, 0, 0, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}};

int main(int argc, char *argv[])
{
    int puerto = -1;
    int jugadores = -1;
    int socketEscucha;
    try
    {
        verificarParamEntrada(argc, argv, puerto, jugadores);
        socketEscucha = crearServidor(puerto, jugadores);
        escucharClientes(socketEscucha, jugadores);
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        return 0;
    }
    cout << "Se cerro completamente" << endl;
    close(socketEscucha);
    return 0;
}
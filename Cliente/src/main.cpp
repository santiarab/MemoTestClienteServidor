#include "Cliente.h"

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN); //  Ignorar SIGPIPE
    int puerto = -1;
    string nickname;
    string servidor;
    int socketComunicacion = 0;

    // Compruebo si se envio correctamente los parametros, si no envio una excepcion
    try
    {
        verificarParamEntrada(argc, argv, puerto, nickname, servidor);
        socketComunicacion = conectandoAlServidor(puerto, servidor);
        juegoMemotest(socketComunicacion);
    }
    catch (const runtime_error &ex)
    {
        system("clear");
        cerr << "Excepción: " << ex.what() << endl;
    }
    close(socketComunicacion);
    return 0;
}

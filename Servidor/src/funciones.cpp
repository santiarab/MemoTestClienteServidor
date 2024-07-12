#include "Servidor.h"

void verificarParamEntrada(int argc, char *argv[], int &puerto, int &jugadores)
{
    if ((strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        mostrarAyuda();
        exit(1);
    }
    if (argc != 5)
    {
        throw runtime_error(" -p <puerto> -j <jugadores>");
    }
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--puerto") == 0) && i + 1 < argc)
        {
            if ((puerto = atoi(argv[i + 1])) <= 0)
            {
                ostringstream mensaje;
                mensaje << "Opción desconocida o falta de argumento válido para " << argv[i + 1];
                throw runtime_error(mensaje.str());
            }
            i++;
        }
        else if ((strcmp(argv[i], "-j") == 0 || strcmp(argv[i], "--jugadores") == 0) && i + 1 < argc)
        {
            if ((jugadores = atoi(argv[i + 1])) <= 0)
            {
                ostringstream mensaje;
                mensaje << "Opción desconocida o falta de argumento válido para " << argv[i + 1];
                throw runtime_error(mensaje.str());
            }
            i++;
        }
        else
        {
            ostringstream mensaje;
            mensaje << "Opción desconocida o falta de argumento válido para " << argv[i];
            throw runtime_error(mensaje.str());
        }
    }
    if (puerto == -1 || jugadores == -1)
    {
        throw runtime_error(" -p <puerto> -n <nickname> -s <servidor>");
    }
}

int crearServidor(int puerto, int jugadores)
{
    struct sockaddr_in serverConfig;

    memset(&serverConfig, '0', sizeof(serverConfig));
    serverConfig.sin_family = AF_INET; // IPV4: 127.0.0.1
    serverConfig.sin_addr.s_addr = htonl(INADDR_ANY);
    serverConfig.sin_port = htons(puerto);
    int socketEscucha = socket(AF_INET, SOCK_STREAM, 0);
    if (socketEscucha == -1)
        throw runtime_error("Error al crear el socket");
    if (bind(socketEscucha, (struct sockaddr *)&serverConfig, sizeof(serverConfig)) == -1)
        throw runtime_error("Error al crear el socket");
    if (listen(socketEscucha, jugadores) == -1)
        throw runtime_error("Error al crear el socket");
    return socketEscucha;
}
void escucharClientes(int socketEscucha, int jugadores)
{
    queue<clientes> colaClientes;
    int numeroDeCartasOcultas = NROCARTAS;
    int maxValor;
    int CantidadDeJugadores = jugadores;
    crearTablero();
    while (jugadores > 0)
    {
        cout << "Esperando a " << jugadores << " jugadores" << endl;
        int socketComunicacion = accept(socketEscucha, (struct sockaddr *)NULL, NULL);
        clientes cliente;
        cliente.socketComunicacion = socketComunicacion;
        cliente.puntos = 0;
        colaClientes.push(cliente);
        jugadores--;
    }

    while (numeroDeCartasOcultas > 0 && CantidadDeJugadores > 1 && !colaClientes.empty())
    {
        cout << "Tablero Completando" << endl;
        mostrarTablero(tableroCompletando);
        // mostrarTablero(tableroCompleto); Descomentar para agilizar las pruebas
        clientes cliente = colaClientes.front();
        colaClientes.pop();
        char signal = 'a'; // mando la senal para que se prepare
        cout << "Le toca al cliente: " << cliente.socketComunicacion << endl;
        try
        {
            enviarMensaje(cliente.socketComunicacion, signal);
            enviarMatrix(cliente, tableroCompletando);
            if (recivirCordenadas(cliente, numeroDeCartasOcultas))
            {
                signal = 'c'; // Se recibio una cordenada correcta
                enviarMensaje(cliente.socketComunicacion, signal);
                enviarMatrix(cliente, tableroCompletando);
            }
            else
            {
                signal = 'i'; // Se recibio una cordenada incorrecta
                enviarMensaje(cliente.socketComunicacion, signal);
                enviarMatrix(cliente, tableroErroneo);
            }
        }
        catch (const exception &e)
        {
            cerr << "Error: " << e.what() << '\n';
            cout << "Cerro el cliente: " << cliente.socketComunicacion << endl;
            close(cliente.socketComunicacion);
            sleep(1);
            CantidadDeJugadores--;
            continue;
        }
        cout << "Numero de cartas ocultas: " << numeroDeCartasOcultas << endl;
        colaClientes.push(cliente);
        sleep(1);
        system("clear");
    }
    if (CantidadDeJugadores == 1)
    {
        sleep(1);
        cout << "Se termino el juego" << endl;
        vector<clientes> vecCliente = colocoEnVector(colaClientes);
        maxValor = mayorValor(vecCliente);               // Busco el valor mas grande
        for (int i = 0; i < (int)vecCliente.size(); i++) // Empiezo a
        {
            clientes cliente = vecCliente[i];
            // Como queda 1 jugador nadie gano!!
            enviarMensaje(cliente.socketComunicacion, 'b'); // Significa que termino y se va a cerrar el socket
            close(cliente.socketComunicacion);
        }
    }
    else
    {

        sleep(1);
        cout << "Se termino el juego" << endl;
        vector<clientes> vecCliente = colocoEnVector(colaClientes);
        maxValor = mayorValor(vecCliente);               // Busco el valor mas grande
        for (int i = 0; i < (int)vecCliente.size(); i++) // Empiezo a
        {
            cout << "El cliente: " << vecCliente[i].socketComunicacion << " Saco: " << vecCliente[i].puntos << " Puntos" << endl;
            clientes cliente = vecCliente[i];
            enviarMensaje(cliente.socketComunicacion, 'p'); // Mando un mensaje que se termino, y va a decidir si gano o no
            if (cliente.puntos == maxValor)
            {
                enviarMensaje(cliente.socketComunicacion, 'g'); // Significa que gano
            }
            else
                enviarMensaje(cliente.socketComunicacion, 'l'); // Significa que perdio

            enviarMensaje(cliente.socketComunicacion, 'b'); // Significa que termino y se va a cerrar el socket
            close(cliente.socketComunicacion);
        }
    }
}

vector<string> split(string str, char pattern)
{

    int posInit = 0;
    int posFound = 0;
    string splitted;
    vector<string> results;

    while (posFound >= 0)
    {
        posFound = str.find(pattern, posInit);
        splitted = str.substr(posInit, posFound - posInit);
        posInit = posFound + 1;
        results.push_back(splitted);
    }
    return results;
}
bool elegirValor(string cor1, string cor2)
{
    vector<string> cor1Split = split(cor1, '-');
    vector<string> cor2Split = split(cor2, '-');
    int x1, x2, y1, y2;

    x1 = stoi(cor1Split[0]);
    x2 = stoi(cor1Split[1]);
    y1 = stoi(cor2Split[0]);
    y2 = stoi(cor2Split[1]);

    if (tableroCompleto[x1][x2] == tableroCompleto[y1][y2])
    {
        tableroCompletando[x1][x2] = tableroCompleto[x1][x2];
        tableroCompletando[y1][y2] = tableroCompleto[y1][y2];
        return true;
    }
    copiarMatriz(tableroCompletando, tableroErroneo);
    tableroErroneo[x1][x2] = tableroCompleto[x1][x2];
    tableroErroneo[y1][y2] = tableroCompleto[y1][y2];
    return false;
}

bool existeUnValor(char valor)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (tableroCompleto[i][j] == 0)
                return false;
            if (tableroCompleto[i][j] == valor)
                return true;
        }
    }
    return false;
}

void crearTablero()
{
    vector<char> letras;
    srand(time(0));
    for (int i = 0; i < 8; i++)
    {
        char elemento;
        do
        {
            elemento = 'A' + rand() % 26;

        } while ((find(letras.begin(), letras.end(), elemento)) != letras.end());
        letras.push_back(elemento);
        letras.push_back(elemento);
    }
    random_shuffle(letras.begin(), letras.end());

    int index = 0;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tableroCompleto[i][j] = letras[index];
            ++index;
        }
    }
}

void enviarMatrix(clientes cliente, char tablero[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        ssize_t info = send(cliente.socketComunicacion, tablero[i], sizeof(tablero[i]), 0);
        if (info == -1)
        {
            throw runtime_error("Error al enviar la matrix");
        }
    }
}

void mostrarTablero(char tablero[4][4])
{
    int filas = 4;
    int columnas = 4;
    // Imprimir los números de las columnas
    printf("%*s", 3, ""); // Espacio para el número de fila
    for (int j = 0; j < columnas; ++j)
    {
        printf("%*d", 3, j);
    }
    printf("\n");

    // Imprimir las filas con los números de las filas
    for (int i = 0; i < filas; ++i)
    {
        printf("%*d", 3, i); // Número de fila
        for (int j = 0; j < columnas; ++j)
        {
            if (tablero[i][j] != 0)
                printf("%*c", 3, tablero[i][j]);
            else
                printf("%*c", 3, '-');
        }
        printf("\n");
    }
}
void copiarMatriz(char matrizOrigen[4][4], char matrizDestino[4][4])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            matrizDestino[i][j] = matrizOrigen[i][j];
        }
    }
}
bool recivirCordenadas(clientes &cliente, int &cantCartas)
{
    char received_data1[120], received_data2[120];
    if (recv(cliente.socketComunicacion, received_data1, sizeof(received_data1), 0) < 0)
    {
        throw runtime_error("Error al recibir el info");
    }
    cout << "Se envio el primer valor" << endl;
    if (recv(cliente.socketComunicacion, received_data2, sizeof(received_data2), 0) < 0)
    {
        throw runtime_error("Error al recibir el info");
    }
    cout << "Se envio el Segundo valor" << endl;
    cout << "Recibio los infos" << endl;
    if (elegirValor(received_data1, received_data2))
    {
        cliente.puntos += 1;
        cantCartas--;
        return true;
    }
    return false;
}
void enviarMensaje(int socketId, char msj)
{
    char signal;
    ssize_t info = send(socketId, &msj, sizeof(msj), 0);
    if (info == -1)
    {
        throw runtime_error("Error al enviar el mensaje");
    }
    if (recv(socketId, &signal, sizeof(signal), 0) < 0 || signal != 'r')
        throw runtime_error("Error en el cliente");
}
vector<clientes> colocoEnVector(queue<clientes> colaClientes)
{
    vector<clientes> vecCliente;
    while (!colaClientes.empty())
    {
        vecCliente.push_back(colaClientes.front());
        colaClientes.pop();
    }
    return vecCliente;
}
int cmpPuntos(const clientes &arg1, const clientes &arg2)
{
    return arg1.puntos - arg2.puntos;
}
int mayorValor(vector<clientes> vecCliente)
{
    int maxValor = 0;
    for (clientes cliente : vecCliente)
    {
        if (cliente.puntos > maxValor)
            maxValor = cliente.puntos;
    }
    return maxValor;
}
void mostrarAyuda()
{
    cout << "  -p / --puerto     Número de puerto (Requerido)\n";
    cout << "  -j / --jugadores  Cantidad de jugadores a esperar para iniciar la sala (Requerido)\n\n";
}
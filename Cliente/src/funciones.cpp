#include "Cliente.h"

void verificarParamEntrada(int argc, char *argv[], int &puerto, string &nickname, string &servidor)
{
    if ((strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        mostrarAyuda();
        exit(1);
    }
    if (argc != 7)
    {
        throw runtime_error(" -p <puerto> -n <nickname> -s <servidor>");
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
        else if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--nickname") == 0) && i + 1 < argc)
        {
            nickname = argv[i + 1];
            i++;
        }
        else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--servidor") == 0) && i + 1 < argc)
        {
            servidor = argv[i + 1];
            i++;
        }
        else
        {
            ostringstream mensaje;
            mensaje << "Opción desconocida o falta de argumento válido para " << argv[i];
            throw runtime_error(mensaje.str());
        }
    }

    if (puerto == -1 || nickname.empty() || servidor.empty())
    {
        throw runtime_error(" -p <puerto> -n <nickname> -s <servidor>");
    }
}

bool esNumero(const string &str)
{
    // Verifica si la cadena es un número
    for (char const &c : str)
    {
        if (isdigit(c) == 0)
            return false;
    }
    return true;
}

bool esIp(string servidor)
{
    stringstream ss(servidor);
    string segmento;
    int partes = 0;

    // Dividir la cadena por los puntos
    while (getline(ss, segmento, '.'))
    {
        if (!esNumero(segmento))
            return false;

        int numero = stoi(segmento);
        if (numero < 0 || numero > 255)
            return false;

        partes++;
    }

    // Verificar si la IP tiene exactamente 4 partes
    return (partes == 4);
}
int conectandoAlServidor(int puerto, string servidor)
{
    int intentos = 5;
    int con;
    if (esIp(servidor))
    {
        struct sockaddr_in socketConfig;
        memset(&socketConfig, '0', sizeof(socketConfig));
        socketConfig.sin_family = AF_INET;
        socketConfig.sin_port = htons(puerto);
        if (inet_pton(AF_INET, servidor.c_str(), &socketConfig.sin_addr) == 0)
            throw runtime_error("Direccion IP invalida");
        int socketId = socket(AF_INET, SOCK_STREAM, 0);
        while ((con = connect(socketId,
                              (struct sockaddr *)&socketConfig, sizeof(socketConfig))) < 0 &&
               intentos > 0)
        {
            intentos--;
            cout << "Intentando Conectarse..." << endl;
            sleep(2);
            system("clear");
        }
        if (con < 0)
            throw runtime_error("Error al intentar conectarse");
        system("clear");
        return socketId;
    }
    else
    {
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;     // Permitir IPv4 o IPv6
        hints.ai_socktype = SOCK_STREAM; // Tipo de socket TCP

        int status = getaddrinfo(servidor.c_str(), to_string(puerto).c_str(), &hints, &res);
        if (status != 0)
        {
            throw runtime_error("Error al crear el socket de escucha");
        }

        int socketId = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (socketId == -1)
        {
            freeaddrinfo(res); // Liberar la memoria alocada por getaddrinfo
            throw runtime_error("Error al crear el socket de escucha");
        }

        while ((con = connect(socketId, res->ai_addr, res->ai_addrlen)) < 0 && intentos > 0)
        {
            intentos--;
            cout << "Intentando Conectarse..." << endl;
            sleep(2);
            system("clear");
        }
        if (con < 0)
        {
            close(socketId);
            freeaddrinfo(res); // Liberar la memoria alocada por getaddrinfo
            throw runtime_error("Error al intentar conectarse");
        }
        system("clear");
        return socketId;
    }
}
void mandarPeticiones(int socketId)
{
    char buffer[2000];
    int bytesRecibidos = 0;
    while ((bytesRecibidos = read(socketId, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRecibidos] = 0;
        printf("%s\n", buffer);
    }
}

void imprimirTableroCompleto(char tablero[][4])
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

void recibirMatrix(int socketId, char tablero[][4])
{
    char received_data[4];

    for (int i = 0; i < 4; ++i)
    {
        if (recv(socketId, received_data, sizeof(received_data), 0) < 0)
        {
            throw runtime_error("Error al recivir la matrix");
        }
        for (int j = 0; j < 4; j++)
        {
            tablero[i][j] = received_data[j];
        }
    }
}
void enviarCordenadas(int socketId, char tablero[4][4])
{
    char valor[120], valor2[120];
    valor2[0] = 0;
    for (int i = 0; i < 2; i++)
    {
        do
        {
            do
            {
                cout << "Por favor, ingrese el " << i + 1 << "° valor: ";
                cin >> valor;
            } while (!corroborarCordenadas(valor, tablero));

            if (strcmp(valor, valor2) != 0)
            {
                ssize_t v = 0;
                v = send(socketId, valor, strlen(valor) + 1, 0);
                if (v < 0)
                {
                    throw runtime_error("Error al enviar datos!");
                }
            }
        } while (i == 1 && strcmp(valor, valor2) == 0);
        strcpy(valor2, valor);
    }
    system("clear");
}

void juegoMemotest(int socketId)
{
    char signal;
    char tablero[4][4];
    char tableroErroneo[4][4];
    int marcador = 0;
    while (true)
    {
        signal = 0;
        try
        {
            recibir(socketId, signal);
            switch (signal)
            {
            case 'a':

                system("clear");
                recibirMatrix(socketId, tablero);
                imprimirTableroCompleto(tablero);
                cout << "Tu puntaje es: " << marcador << endl;
                enviarCordenadas(socketId, tablero);
                signal = 0;
                recibir(socketId, signal);
                if (signal == 'c') // C indica que se indico cordenadas
                {
                    system("clear");
                    cout << "Correcto!!!" << endl;
                    marcador++;
                    recibirMatrix(socketId, tablero);
                    imprimirTableroCompleto(tablero);
                    cout << "Tu puntaje es: " << marcador << endl;
                    cout << "Esperando a los otro/s jugador/es..." << endl;
                }
                else if (signal == 'i') // I indica que se indico incordenadas
                {
                    cout << "Incorrecto!!!" << endl;
                    recibirMatrix(socketId, tableroErroneo);
                    imprimirTableroCompleto(tableroErroneo);
                    sleep(3);
                    system("clear"); // Borro el tablero incorrecto y muestro el tablero anterior
                    imprimirTableroCompleto(tablero);
                    cout << "Tu puntaje es: " << marcador << endl;
                    cout << "Esperando a los otro/s jugador/es..." << endl;
                }
                break;
            case 'p': // Se termino el juego y se va a decidir si gano o no

                system("clear");
                recibir(socketId, signal);
                if (signal == 'g') // significa que gano
                    cout << "Ganaste!!!" << endl;
                else if (signal == 'l') // significa que perdio
                    cout << "Perdiste!!!" << endl;
                break;
            case 'b': // Significa que termino

                cout << "termino" << endl;
                return;
                break;

            default:
                throw runtime_error("Se cayo el servidor!");
                break;
            }
        }
        catch (const exception &e)
        {
            throw runtime_error("Se cayo el Servidor!");
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

bool corroborarCordenadas(string valor, char tablero[4][4])
{
    // Define el patrón de la expresión regular
    regex patron("[0-3]-[0-3]");
    string valorAux = valor;
    // Usa regex_match para verificar si la cadena cumple con el patrón

    if (regex_match(valorAux, patron))
    {
        vector<string> cor = split(valorAux, '-');
        int x1 = stoi(cor[0]);
        int x2 = stoi(cor[1]);
        if (tablero[x1][x2] == 0)
            return true;
    }
    return false;
}

void recibir(int socketId, char &signal)
{
    char recibido = 'r';
    if (recv(socketId, &signal, sizeof(signal), 0) < 0)
        throw runtime_error("Error en el servidor");
    send(socketId, &recibido, sizeof(recibido), 0); // Envio un acknowledgment/reconocimiento al servidor
}
void mostrarAyuda()
{
    cout << "  -n / --nickname   Nickname del usuario (Requerido)\n";
    cout << "  -p / --puerto     Número de puerto (Requerido)\n";
    cout << "  -s / --servidor   Dirección IP o nombre del servidor (Requerido)\n";
}
#include <iostream>
#include <winsock2.h>
#include <clocale>
#include <windows.h>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define BUFLEN 1024
#define PORT 9637

//Сервер

bool err_check = false;
int mode = 0;

struct Command {
    std::string name;
    std::vector<std::string> params;
};

Command ParseCommand(const std::string& commandStr) {
    Command command;

    int number;

    std::istringstream ss(commandStr);
    std::string word;
    ss >> command.name;


    while (ss >> word) {
        if (word == ":")
        {
            break;
        }
        command.name += " " + word;
    }

    if (command.name == "clear display") {
        err_check = false;
        mode = 1;
    }
    else if (command.name == "draw pixel") {
        mode = 2;
        err_check = false;
    }
    else if (command.name == "draw line") {
        mode = 5;
        err_check = false;
    }
    else if (command.name == "draw rectangle")
    {
        mode = 10;
        err_check = false;
    }
    else if (command.name == "fill rectangle") {
        mode = 15;
        err_check = false;
    }
    else if (command.name == "draw ellipse") {
        mode = 20;
        err_check = false;
    }
    else if (command.name == "fill ellipse")
    {
        mode = 25;
        err_check = false;
    }
    else {
        err_check = true;
    }


    if (!err_check)
    {
        while (ss >> word) {
            switch (mode)
            {
                //clear display
                ///other
            case 1: case 4: case 9: case 14: case 19: case 24: case 29:
                command.params.push_back(word);
                try {
                    number = std::stoi(word);
                }
                catch (const std::invalid_argument& e) {
                    err_check = false;
                    return command;
                }
                err_check = true;
                return command;
                ///
                ///numbers
            case 2: case 3: case 5: case 6: case 7: case 8: case 10: case 11: case 12: case 13:
            case 15: case 16: case 17: case 18: case 20: case 21: case 22: case 23:
            case 25: case 26: case 27: case 28:
                command.params.push_back(word);
                try {
                    number = std::stoi(word);
                }
                catch (const std::invalid_argument& e) {
                    err_check = true;
                    return command;
                }
                err_check = false;
                mode++;
                break;
                ///
            }
        }
        err_check = true;
        return command;
    }
    else {
        return command;
    }
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "ru");
    system("title UDP сервер");
    system("color 3");
    sockaddr_in server, client;

    const char* myMessage = "Ошибка! Такой команды нет!"; //err_check == true
    const char* myMessage2 = "Команда успешно сработала"; //err_check == false
    const char* myHelp;


    WSADATA wsa;
    cout << "Устанавливается Winsock" << endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Ошибка. Код ошибки: " << WSAGetLastError();
        exit(0);
    }
    cout << "Установленно соединение" << endl;


    SOCKET server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        cout << "Не удалось создать сокет. Код ошибки: " << WSAGetLastError();
    }
    cout << "Сокет был создан" << endl;


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);


    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        cout << "Bind не удался. Код ошибки: " << WSAGetLastError();
        exit(EXIT_FAILURE);
    }
    cout << "Bind установлен" << endl;
    system("cls");
    while (true)
    {
        cout << "Ожидаем меседж.." << endl << endl;
        fflush(stdout);
        char message[BUFLEN] = {};


        int message_len;
        int slen = sizeof(sockaddr_in);
        if (message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen) == SOCKET_ERROR)
        {
            cout << "recvfrom() получил ошибку. Код ошибки: " << WSAGetLastError();
            exit(0);
        }

        cout << "Получен пакет от " << inet_ntoa(client.sin_addr) << ": " << ntohs(client.sin_port) << endl;
        cout << "Сообщение: " << message << endl << endl;
        if (strstr(message, "/help") != nullptr)
        {
            myHelp = "Список доступных команд: \nclear display : <цвет> - очищает экран и заполняет его указанным цветом \ndraw pixel : <х0> <y0> <цвет> - устанавливает пиксель соответственного цвета в коориданатах х0, у0 \ndraw rectangle : <х0> <y0> <w> <h> <цвет> - рисует прямоугольник соответственного цвета, от координат х0, у0 до координат х1 у1 \nfill rectangle : <х0> <y0> <w> <h> <цвет> - рисует заполненный прямоугольник соответственного цвета, от координат х0, у0 до координат х1 у1 \ndraw ellipse : <х0> <y0> <радиус х> <радиус у> <цвет> - рисует эллипс соответственного цвета, от координат х0, у0 до координат х1 у1 \nfill ellipse : <х0> <y0> <радиус х> <радиус у> <цвет> - рисует заполненный эллипс соответственного цвета, от координат х0, у0 до координат х1 у1 \n";
            if (!err_check)
            {
                strcpy(message, myHelp);

                if (sendto(server_socket, message, strlen(message), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
                {
                    cout << "sendto() получил ошибку. Код ошибки: " << WSAGetLastError();
                    return 3;
                }
            }

        }
        else {
            std::istringstream commandStream(message);
            std::string commandLine;

            while (std::getline(commandStream, commandLine, '\n')) {
                Command command = ParseCommand(commandLine);

                std::string firstWord, secondWord;
                std::istringstream nameStream(command.name);
                nameStream >> firstWord >> secondWord;


                if (!err_check) {
                    std::cout << "Command: " << command.name << std::endl;
                    std::cout << "Parameters:";
                    for (const std::string& param : command.params) {
                        std::cout << " " << param;
                    }
                    std::cout << std::endl;
                }
            }

            if (err_check)
            {
                strcpy(message, myMessage);

                if (sendto(server_socket, message, strlen(message), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
                {
                    cout << "sendto() получил ошибку. Код ошибки: " << WSAGetLastError();
                    return 3;
                }
            }

            if (!err_check)
            {
                strcpy(message, myMessage2);

                if (sendto(server_socket, message, strlen(message), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
                {
                    cout << "sendto() получил ошибку. Код ошибки: " << WSAGetLastError();
                    return 3;
                }
            }
        }
    }

    closesocket(server_socket);
    WSACleanup();
}

//Ниже приведен код для "Пользователя". Чтобы полноценно проверить работоспособность
//Кода и выполнения команд, необходимо будет запустить код (которые находится выше)
//Вместе с кодом, который будет находится в заккоментированной области.
//Код в закомментированной области следует скопировать и вставить в новый проект VS.
//После чего, открыть 2 кода (Клиент и Сервер) и запустить сначало Сервер а после Клиент

//Если же у вас все работает - значит вы сделали все верно!

//Клиент:
/*

#include <iostream>
#include <winsock2.h>
#include <locale.h>
#include <windows.h>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define SERVER "127.0.0.1"
#define BUFLEN 1024
#define PORT 9637

//Клиент

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "ru");
    system("title UDP клиент");
    system("color 2");


    WSADATA ws;
    cout << "Устанавливается Winsock" << endl;
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        cout << "Ошибка. Код ошибки: " << WSAGetLastError();
        return 1;
    }
    cout << "Установленно соединение" << endl;


    sockaddr_in server;
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        cout << "Не удалось создать сокет. Код ошибки: " << WSAGetLastError();
        return 2;
    }


    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);

    system("cls");
    while (true)
    {
        char message[BUFLEN];
        cout << "Введите Команду (для того, чтобы узнать список команд, введите /help): ";
        cin.getline(message, BUFLEN);


        if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            cout << "sendto() получил ошибку. Код ошибки: " << WSAGetLastError();
            return 3;
        }


        char answer[BUFLEN] = {};

        int slen = sizeof(sockaddr_in);
        int answer_length;
        if (answer_length = recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &slen) == SOCKET_ERROR)
        {
            cout << "recvfrom() получил ошибку. Код ошибки: " << WSAGetLastError();
            exit(0);
        }

        cout << endl << answer << endl << endl;
    }

    closesocket(client_socket);
    WSACleanup();
}

*/
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

#define BUFLEN 2048
#define PORT 9637

//Сервер

bool err_check = false;
int mode = 0;

int mas_param[6];
string mas_param2[3];

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

    int j = 0;
    int i = 0;
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
    else if (command.name == "draw circle")
    {
        mode = 30;
        err_check = false;
    }
    else if (command.name == "fill circle")
    {
        mode = 34;
        err_check = false;
    }
    else if (command.name == "draw rounded rectangle")
    {
        mode = 38;
        err_check = false;
    }
    else if (command.name == "fill rounded rectangle")
    {
        mode = 44;
        err_check = false;
    }
    else if (command.name == "draw text")
    {
        mode = 50;
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
            case 1: case 4: case 9: case 14: case 19: case 24: case 29: case 33:
            case 37: case 43: case 49: case 52: case 55:
                command.params.push_back(word);
                try {
                    number = std::stoi(word);
                }
                catch (const std::invalid_argument& e) {
                    err_check = false;
                    return command;
                }
                err_check = true;
                mas_param2[j] = word;
                j++;
                if (mode != 52)
                {
                    return command;
                }
                ///
                ///numbers
            case 2: case 3: case 5: case 6: case 7: case 8: case 10: case 11: case 12: case 13:
            case 15: case 16: case 17: case 18: case 20: case 21: case 22: case 23:
            case 25: case 26: case 27: case 28: case 30: case 31: case 32: case 34:
            case 35: case 36: case 38: case 39: case 40: case 41: case 42: case 44:
            case 45: case 46: case 47: case 48: case 50: case 51: case 53: case 54:
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
                mas_param[i] = number;
                i++;
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
            myHelp = "Список доступных команд: \nclear display : <цвет> - очищает экран и заполняет его указанным цветом \ndraw pixel : <х0> <y0> <цвет> - устанавливает пиксель соответственного цвета в коориданатах х0, у0 \ndraw rectangle : <х0> <y0> <w> <h> <цвет> - рисует прямоугольник соответственного цвета, от координат х0, у0 до координат х1 у1 \nfill rectangle : <х0> <y0> <w> <h> <цвет> - рисует заполненный прямоугольник соответственного цвета, от координат х0, у0 до координат х1 у1 \ndraw ellipse : <х0> <y0> <радиус х> <радиус у> <цвет> - рисует эллипс соответственного цвета, от координат х0, у0 до координат х1 у1 \nfill ellipse : <х0> <y0> <радиус х> <радиус у> <цвет> - рисует заполненный эллипс соответственного цвета, от координат х0, у0 до координат х1 у1 \n draw circle : <x0> <y0> <radius> <color> - рисует круг с указанными радиусом, начальными координатами и цветом.\nfill circle : <x0> <y0> <color> рисует заполненный круг с указанными радиусом, начальными координатами и цветом.\n draw rounded rectangle : <x0> <y0> <w> <h> <radius> <color> - рисует прямоугольник с закругленным углами с заданными координатами, длиной, шириной и цветом.\n fill rounded rectangle : <x0> <y0> <w> <h> <radius> <color> - рисует заполненный прямоугольник с округленными углами в указанных начальных координатах, длиной, шириной, радиусом и цветом.\ndraw text : пишет текст в указанных координатах, цветом, шрифтом, длиной и самим текстом\n";
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

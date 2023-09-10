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

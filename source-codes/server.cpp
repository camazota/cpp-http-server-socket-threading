#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include "threadpool.h"

#pragma comment(lib, "ws2_32.lib")

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string redirect(const std::string &path)
{
    std::string contentType, response, filepath;
    if (path.ends_with(".css"))
    {
        contentType = "text/css";
        filepath = "./static" + path;
    }
    else
    {
        contentType = "text/html; charset=utf-8";
        filepath = "./templates";
        if (path == "/" || path == "")
        {
            filepath += "/index.html";
        }
        else
        {
            filepath += path + ".html";
        }
    }
    std::string body = readFile(filepath);
    if (body.empty())
    {
        std::string errorPage = readFile("./templates/404.html");
        if (errorPage.empty())
        {
            errorPage = "<html><body><h1>404 Not Found </h1></body></html>";
        }
        std::ostringstream ss;
        ss << "HTTP/1.1 404 Not Found\r\n"
           << "Content-Type: text/html; charset=utf-8\r\n"
           << "Content-Length: " << errorPage.size() << "\r\n\r\n"
           << errorPage;
        return ss.str();
    }
    std::ostringstream ss;
    ss << "HTTP/1.1 200 OK\r\n"
       << "Content-Type: " << contentType << "\r\n"
       << "Content-Length: " << body.size() << "\r\n\r\n"
       << body;
    return ss.str();
}

void handleClient(SOCKET clientSocket, sockaddr_in clientAddr)
{
    char buffer[1024];
    int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytes > 0)
    {
        buffer[bytes] = '\0';

        char ip[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);

        std::cout << "IPv4-Address: " << ip << " - " << bytes << " bytes received" << std::endl;

        std::string request(buffer);
        std::istringstream iss(request);
        std::string method, path, version;
        iss >> method >> path >> version;

        std::string response = redirect(path);
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    closesocket(clientSocket);
}

int runserver(const char *ipv4, unsigned short port)
{
    WSADATA wsaData;

    if (WSAStartup((unsigned short)514, &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return -1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed - Err: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ipv4, &serverAddr.sin_addr);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed - Err: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed - Err: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server is running on -> " << ipv4 << ":" << port << std::endl;

    size_t cores = std::thread::hardware_concurrency();
    if (cores == 0)
        cores = 8;
    size_t threadCount = cores * 2;
    threadpool pool(threadCount);

    while (true)
    {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Client socket accept failed - Err: " << WSAGetLastError() << std::endl;
            continue;
        }
        pool.enqueue([clientSocket, clientAddr]()
                     { handleClient(clientSocket, clientAddr); });
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

int main()
{
    runserver("127.0.0.1", (unsigned short)8080);
    return 0;
}
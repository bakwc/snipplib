#pragma once

#include <string>
#include <winsock2.h>

using namespace std;

namespace snipplib {

struct THttpAddr;
inline string FetchUrl(const string& url);  // <--- Use it!
inline THttpAddr GetHostAction(string url);

struct THttpAddr {
    string Host;
    unsigned short Port;
    string Action;
};

inline string FetchUrl(const string& url) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cout << "WSAStartup failed.\n";
        return string();
    }

    THttpAddr addr = GetHostAction(url);

    SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    struct hostent *host;
    host = gethostbyname(addr.Host.c_str());
    SOCKADDR_IN SockAddr;
    SockAddr.sin_port=htons(addr.Port);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
        return string();
    }

    string request = "GET " + addr.Action + " HTTP/1.1\r\n";
    request += "Host: " + addr.Host + "\r\n";
    request += "Connection: close\r\n\r\n";

    send(Socket, request.c_str(), request.length(), 0);
    char buffer[10000];
    int nDataLength;
    nDataLength = recv(Socket,buffer,10000,0);
    closesocket(Socket);
    WSACleanup();
    return string(buffer, nDataLength);
}

inline THttpAddr GetHostAction(string url) {
    THttpAddr addr;
    if (url.find("http://") == 0) {
        url = url.substr(7);
    }
    size_t dot = url.find('/');
    if (dot == string::npos) {
        addr.Host = url;
        addr.Action = "/";
        addr.Port = 80;
    } else {
        string hostPort = url.substr(0, dot);
        size_t portPos = hostPort.find(':');
        addr.Action = url.substr(dot);
        if (portPos != string::npos) {
            addr.Host = hostPort.substr(0, portPos);
            addr.Port = stoi(hostPort.substr(portPos + 1));
        } else {
            addr.Port = 80;
            addr.Host = hostPort;
        }
    }
    return addr;
}

}

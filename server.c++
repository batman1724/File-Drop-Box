#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <vector>
using namespace std;
#define PORTNO 10000
struct sockaddr_in server, client;
struct TextFile
{
    int sockfd;
    char clientName[50];
    char fileName[50];
    char pathname[100];
    char fileContents[10000];
    char password[50];
};
struct USER
{
    char USERNAME[50];
    char PASSWORD[50];
};
struct USER *createLOGIN()
{
    struct USER *info = (struct USER *)malloc(sizeof(USER));
    return info;
}
struct TextFile *createTextInfo()
{
    struct TextFile *info = (struct TextFile *)malloc(sizeof(struct TextFile));
    // info->clientName = (char*)malloc(50*sizeof(char));
    // info->fileName = (char*)malloc(50*sizeof(char));
    // info->pathname = (char*)malloc(100*sizeof(char));
    return info;
}
map<string, string> database;
void poplateHashMap()
{
    FILE *username_file = fopen("usernames.txt", "r");
    FILE *password_file = fopen("password.txt", "r");
    char ch = fgetc(username_file);
    string username = "";
    string password = "";
    vector<string> username_vector;
    vector<string> password_vector;
    // FOR USERNAMES
    while (ch != EOF)
    {
        if (ch != '\n')
            username = username + ch;
        else
        {
            username_vector.push_back(username);
            username = "";
        }
        ch = fgetc(username_file);
    }
    // FOR PASSWORDS
    ch = fgetc(password_file);
    while (ch != EOF)
    {
        if (ch != '\n')
            password = password + ch;
        else
        {
            password_vector.push_back(password);
            password = "";
        }
        ch = fgetc(password_file);
    }
    map<string, string>::iterator it;
    for (int i = 0; i < username_vector.size(); i++)
    {
        database.insert({username_vector[i], password_vector[i]});
    }
}
struct USER *clientAuthentication(int newsockfd)
{
    /*
        ~~~
            Authenticate client details here
            1) read client username and password
            2) authenticate it with existing database
            3) if user exists and entered right info, allow connection to continue
            4)	else return error message and terminate client connection to server.
            4) could also ask for username and password again x number of times, after which
                message "number of attempts execeeded" could be displayed.

        ~~~
    */
    while (1)
    {
        struct USER *details = createLOGIN();
        char message[50];
        read(newsockfd, details->USERNAME, sizeof(details->USERNAME));
        read(newsockfd, details->PASSWORD, sizeof(details->PASSWORD));
        string username = details->USERNAME;
        string password = details->PASSWORD;
        if (database.find(username) != database.end())
        {
            if (!database[username].compare(password))
            {
                strcpy(message, "****USER-AUTHORIZED****");
                write(newsockfd, message, sizeof(message));
                return details;
            }
            else
            {
                strcpy(message, "****INNCORRECT-PASSWORD****\n");
            }
        }
        else
            strcpy(message, "****INNCORRECT-USERNAME****\n");
        // cout << "password = " << database[username] << endl;
        //  sending authentication message back to client
        write(newsockfd, message, sizeof(message));
    }
}
void display(struct TextFile *info);
struct TextFile *recieveFile(int newsockfd, struct USER *details)
{
    struct TextFile *info = createTextInfo();
    FILE *fp = fopen("input.txt", "r");
    // read(newsockfd, info->clientName, sizeof(info->clientName));
    // read(newsockfd, info->password, sizeof(info->password));
    strcpy(info->clientName, details->USERNAME);
    strcpy(info->password, details->PASSWORD);
    read(newsockfd, info->fileName, sizeof(info->fileName));
    read(newsockfd, info->pathname, sizeof(info->pathname));
    read(newsockfd, info->fileContents, sizeof(info->fileContents));
    info->sockfd = newsockfd;
    display(info);
    return info;
}
void display(struct TextFile *info)
{
    printf("info recieved\n");
    printf("~client name -> %s\n", info->clientName);
    printf("~password -> %s\n", info->password);
    printf("~fileName -> %s\n", info->fileName);
    printf("~pathnameee -> %s\n", info->pathname);
    printf("file contents = %s\n", info->fileContents);
}
void *clientFunctions(int newsockfd)
{
    /*
        ~~~
            1)	authenticate user
            2)	recieve file info and popluate struct TextFile
            3)
        ~~~
    */
    struct USER *details = clientAuthentication(newsockfd);
    recieveFile(newsockfd, details);
    // display(info);
}

int getSocket()
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORTNO);

    bind(socketfd, (struct sockaddr *)&server, sizeof(server));
    listen(socketfd, 5);
    return socketfd;
}
int getConnection(int sockfd)
{
    int clilen = sizeof(clilen);
    int newsockfd = accept(sockfd, (struct sockaddr *)&client, (socklen_t *)&clilen);
    std::thread clientThread(&clientFunctions, newsockfd);
    clientThread.join();
}
int main()
{
    poplateHashMap();
    int sockfd = getSocket();
    while (1)
    {
        getConnection(sockfd); // get connection and execute client functions here only
    }
}
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <cctype>
void error_die(const char * msg);

int startup(u_short *port);

void *accept_request(void * from_client);

void execute_cgi(int client,const std::string &path,
                 const std::string &method,const char *query_string);

void bad_request(int client);

void cannot_execute(int client);

void serve_file(int client,const std::string& path);

void headers(int client);

void cat(int client,std::fstream &file);

void not_found(int client);

void unimplemented(int client);

int get_line(int sock,std::string &buf,int size);


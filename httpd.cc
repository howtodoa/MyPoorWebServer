#include"httpd.h"

void error_die(const char * msg)
{
     perror(msg);
     exit(1);
}
int startup(u_short *port)
{
    int httpd=0,option;
    struct sockaddr_in name;
    //创建套接字
    httpd=socket(PF_INET,SOCK_STREAM,0);
    if(httpd==-1) error_die("socket");
    
    //设置套接字属性，端口复用
    socklen_t optlen;
    optlen=sizeof(option);
    option=1;
    setsockopt(httpd,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<void *>(&option),optlen);

    //初始化服务端ip端口信息
    memset(&name,0,sizeof(name));
    name.sin_family=AF_INET;
    name.sin_port=htons(*port);
    name.sin_addr.s_addr=htonl(INADDR_ANY);

    //将端口ip和服务端套接字绑定
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
    {
        perror("bind");
    }
    else 
    {
    //传入的端口绑定失败，动态绑定系统分配的端口
    socklen_t  namelen = sizeof(name);
    if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
        error_die("getsockname");

    // 将获取到的端口号存储在传入的 port 变量中
    *port = ntohs(name.sin_port);
    }
    //监听，最多等待10个链接
    if (listen(httpd, 10) < 0) error_die("listen");
    return (httpd);
}

void *accept_request(void * from_client)
{
      int client=*reinterpret_cast<int *>(from_client);
      std::string buf;
      int numchars;
      std::string method;
      std::string url;
      std::string path;
      char *query_string=nullptr;
      struct stat st;
      int cgi = 0;
      //按行分隔接受到的数据
      numchars=get_line(client,buf,1024);
    int i=0,j=0;
    while(!std::isspace(static_cast<int>(buf[j])))
    {
        method[i++]=buf[j++];
    }
    method[i]='\0';
    if(strcasecmp(method.c_str(),"get")&&strcasecmp(method.c_str(),"post"))
    {
        unimplemented(client);
        return static_cast<void *>(0);
    }
    i=0;
    while(std::isspace(static_cast<int >(buf[j]))) j++;//跳过空格
    while(!std::isspace(static_cast<int >(buf[j]))&&j<1024)//获取url
    {
        url[i++]=buf[j++];
    }
    //url[i]='\0';
    std::cout<<url<<std::endl;
      if(strcasecmp(method.c_str(),"get")==0)
    {
        query_string=&url[0];
        while(*query_string!='?'&&*query_string!='\0')
        query_string++;
        if(*query_string=='?')
        {
            cgi=1;
            *query_string='\0';
            query_string++;
        }
    }
    path+="httpdocs";
    std::cout<<url.size()<<std::endl;
    path+=url;
   
   std::cout<<url<<std::endl;
    std::cout<<path.c_str()<<std::endl;
    if(path.back()!='/')
    {
    
        path+="/test.html";
    }

    if(!std::filesystem::exists(path))
    {
        not_found(client);
    }
    else
    {
        cgi=1;
    if (!cgi)
	   serve_file(client, path);
	  else
	   execute_cgi(client, path, method, query_string);
    }
    close(client);
	 //printf("connection close....client: %d \n",client);
	 return NULL;
}

void execute_cgi(int client,const std::string &path,
                 const std::string &method,const char *query_string)
{ 
    std::string buf;
    int cgi_output[2];
    int cgi_input[2];

    pid_t pid;
    int status;
    int i;
    int c;
    int numchars;
    int Content_length=-1;
    buf[0]='A';
    buf[1]='\0';
    if(method.c_str()=="GET"|method.c_str()=="get")
    {
         while ((numchars > 0) && strcmp("\n", buf.c_str()))
		 {
			 numchars = get_line(client, buf, 1024);
		 }
    }
    else
    {
         numchars = get_line(client, buf, 1024);
		  while ((numchars > 0) && strcmp("\n", buf.c_str()))
		  {
				buf[15] = '\0';
			   if (strcasecmp(buf.c_str(), "Content-Length:") == 0)
					Content_length = atoi(&(buf[16]));

			   numchars = get_line(client, buf, 1024);
		  }

		  if (Content_length == -1) {
		   bad_request(client);
		   return;
		   }
    }
    std::ostringstream response;
    response<<"HTTP/1.0 200 OK\r\n";
    
    if(pipe(cgi_output)<0)
    {
        cannot_execute(client);
        return;
    }
    if(pid=fork()<0)
    {
        cannot_execute(client);
        return;
    }
    if(pid==0)//子进程：运行cgi脚本
    {
         char meth_env[255];
		  char query_env[255];
		  char length_env[255];

		  dup2(cgi_output[1], 1);
		  dup2(cgi_input[0], 0);


		  close(cgi_output[0]);//关闭了cgi_output中的读通道
		  close(cgi_input[1]);//关闭了cgi_input中的写通道


		  sprintf(meth_env, "REQUEST_METHOD=%s", method.c_str());
		  putenv(meth_env);

		  if (strcasecmp(method.c_str(), "GET") == 0) {
		  //存储QUERY_STRING
		   sprintf(query_env, "QUERY_STRING=%s", query_string);
		   putenv(query_env);
		  }
		  else {   /* POST */
			//存储CONTENT_LENGTH
		   sprintf(length_env, "CONTENT_LENGTH=%d", Content_length);
		   putenv(length_env);
		  }


		  execl(path.c_str(), path.c_str(), NULL);//执行CGI脚本
		  exit(0);
    }
    else
    {
		  close(cgi_output[1]);
		  close(cgi_input[0]);
		  if (strcasecmp(method.c_str(), "POST") == 0)

			 for (i = 0; i < Content_length; i++) 
			   {

				recv(client, &c, 1, 0);

				write(cgi_input[1], &c, 1);
			   }



		//读取cgi脚本返回数据

		while (read(cgi_output[0], &c, 1) > 0)
			//发送给浏览器
		{			
			send(client, &c, 1, 0);
		}

		//运行结束关闭
		close(cgi_output[0]);
		close(cgi_input[1]);


		  waitpid(pid, &status, 0);
    }
}

void bad_request(int client)
{
    std::string buf;
    buf+="HTTP/1.0 400 BAD REQUEST\r\n";
    buf+="Content-type: text/html\r\n";
    buf+="\r\n";
    buf+="<P>Your browser sent a bad request, ";
    buf+="such as a POST without a Content-Length.\r\n";
    send(client,buf.c_str(),buf.length(),0);
} 
void cannot_execute(int client)
{
    std::string buf;
    buf+="HTTP/1.0 500 Internal Server Error\r\n";
    buf+="Content-type: text/html\r\n";
    buf+="\r\n";
    buf+="<P>Error prohibited CGI execution.\r\n";
} 
void serve_file(int client,const std::string& path)
{
    std::fstream file(path);
    if(!file.is_open())
    {
        not_found(client);
    }
    else
    {
        headers(client);
        cat(client,file);
    }
    file.close();
}
void headers(int client)
{
    std::ostringstream response;
    response<<"HTTP/1.0 200 OK\r\n";
    response<<"SERVICE:TJQ's http/0.1.0\r\n";
    response<<"Content-Type: text/html\r\n";
    response<<"\r\n";
    std::string s=response.str();
    send(client,s.c_str(),s.length(),0);
} 
void cat(int client,std::fstream &file)
{
    const int bufsize=1024;
    char buf[bufsize];
    while(file.read(buf,bufsize))
    {
        send(client,buf,file.gcount(),0);       
    }
    send(client,buf,file.gcount(),0); 
    file.close();
}
void not_found(int client)
{
    std::ostringstream response;
    response<<"HTTP/1.0 404 NOT FOUND\r\n";
    response<<"SERVICE:TJQ's http/0.1.0\r\n";
    response<<"Content-Type: text/html\r\n";
    response<<"\r\n";

    response<<"<html><title> 404 not found</title>\r\n";
    response<<"<body>your request source is not found\r\n";
    response<<"</body></html>\r\n";//html解释器会忽略中间出现的换行符和空格
    std::string s=response.str();
    send(client,s.c_str(),s.length(),0);
}
void unimplemented(int client)
{
    std::ostringstream response;
    //http响应头
    response<<"HTTP/1.0 501 Method Not Implemented\r\n";
    response<<"Server: YourServerName\r\n";
    response<<"Content-Type: text/html\r\n";
    response<<"\r\n";
    //http响应体
    response<<"<HTML><HEAD><TITLE>Method Not Implemented</TITLE></HEAD>\r\n";
    response<<"<BODY><P>HTTP request method not supported!!!.</P></BODY></HTML>\r\n";
    std::string resstr;
    resstr=response.str();
    send(client,resstr.c_str(),resstr.length(),0);
}
int get_line(int sock,std::string &buf,int size)
{
    int i=0;
    char c='\0';
    int n;
    //处理\r\n和\r处理成\n，统一报文协议格式
    while((i<size-1)&&(c!='\n'))
    {
        n=recv(sock,&c,1,0); //一次读取一个字符串
        if(n>0)
        {
            if (c == '\r')
			{
             n=recv(sock,&c,1,MSG_PEEK);//预读下一个字符
             if(n>0&&(c=='\n')) recv(sock,&c,1,0);
             else c='\n';
            }
            buf[i++]=c;
        }
        else c='\n';
    }
    buf[i]='\0';
    return i;
}
int main()
{
    int server_sock=-1;
    u_short port=6379;
    int client_sock=-1;
    struct sockaddr_in client_name;
    socklen_t client_name_len=sizeof(client_name);
    server_sock=startup(&port);
    std::cout<<"http server_sock is"<<server_sock<<std::endl;
    std::cout<<"http running on port"<<port<<std::endl;
    while(1)
    {
        client_sock=accept(server_sock,reinterpret_cast<sockaddr*>(&client_name),&client_name_len);
        std::cout<<"New connection.... ip is"<<inet_ntoa(client_name.sin_addr)<<"port:"<<ntohs(client_name.sin_port)<<std::endl;
	    std::thread newthread;
        if(client_sock==-1) error_die("accept");
        try
        {
		newthread = std::thread(accept_request, reinterpret_cast<void*>(&client_sock));
		newthread.detach();
        //已经有异常的情况下使用throw抛出
       }
        catch(const std::system_error &e)
        {
            std::cerr<<"caught system_error"<<e.what()<<" code:"<<e.code()<<std::endl;
        }
        catch(const std::exception &e)
        {
            std::cerr<<"caught exception"<<e.what()<<std::endl;
        }
    } 
    close(server_sock);
    return 0;
}

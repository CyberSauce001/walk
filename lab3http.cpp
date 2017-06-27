//modified by:Alexander Nguyen	
//date:6/12/2017
//purpose:To modify the program and fix error
//
//This program originated from the website: coding.debuntu.org
//Author: chantra
//Date: Sat 07/19/2008 - 19:23
//Usage:
//   $ gcc lab3prog.c -Wall -olab3prog
//   $ ./lab3prog
//   USAGE: prog host [page]
//          host: the website hostname. ex: coding.debuntu.org
//          page: the page to retrieve. ex: index.html, default: /
//
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
//added unistd
#include <unistd.h>
extern char *message;
int create_tcp_socket();
char *get_ip(char *host);
char *build_get_query(char *host, char *page);

#define HOST "www.cs.csub.edu"
#define PAGE  "/~anguyen/3350/test.txt"
#define PORT 80
#define USERAGENT "HTMLGET 1.0"

int newmain(int argc, char **argv, char *&message)
{
  struct sockaddr_in *remote;
  int sock;
  int tmpres;
  char *ip;
  char *get;
  char buf[BUFSIZ+1];
  char *host;
  char *page;

if(argc == 1){
    exit(2);
  }  
  host = argv[1];
    if(argc > 2){
        page = argv[2];
    }else {
      //Fixed warning: deprecated conversion from string constant to char*
      //page = Page; casted a char to make the string constant as a char since it is define above as const
    page = (char*)PAGE;
  }
  sock = create_tcp_socket();
  ip = get_ip(host);
  //fprintf(stderr, "IP is %s\n", ip);
  remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
  remote->sin_family = AF_INET;
  tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
  if ( tmpres < 0)  
  {
    perror("Can't set remote->sin_addr.s_addr");
    exit(1);
  }
  else if (tmpres == 0)
  {
    fprintf(stderr, "%s is not a valid IP address\n", ip);
    exit(1);
  }
  remote->sin_port = htons(PORT);

  if (connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0) {
    perror("Could not connect");
    exit(1);
  }
  get = build_get_query(host, page);
//  fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);

  //Send the query to the server
  //Fixed warning: comparison between signed and unsigned integer expression
  //int sent = 0; change int to unsigned to be able to compare expression
  unsigned int sent = 0;
  while (sent < strlen(get))
  {
    tmpres = send(sock, get+sent, strlen(get)-sent, 0);
    if (tmpres == -1) {
      perror("send command, Can't send query");
      exit(1);
    }
    sent += tmpres;
  }
  //now it is time to receive the page
  memset(buf, 0, sizeof(buf));
  int htmlstart = 0;
  char* htmlcontent;
  while ((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0) {
    if (htmlstart == 0)
    {
      /* Under certain conditions this will not work.
      * If the \r\n\r\n part is splitted into two messages
      * it will fail to detect the beginning of HTML content
      */
      htmlcontent = strstr(buf, "\r\n\r\n");
      if (htmlcontent != NULL) {
        htmlstart = 1;
        htmlcontent += 4;
      }
    } else {
      htmlcontent = buf;
    }
    if (htmlstart) {
	//Fixed warning: format not a string literal and no format arguments
      //fprintf(stdout, htmlcontent); added "&s" in order to read in the constant string
      fprintf(stdout, "%s", htmlcontent);
      int i = 0;
      while (htmlcontent + i != '\0' && htmlcontent+i != '\n') {
     	i++;

      }
      	  htmlcontent[i] = '\0';
	  strcpy(message,htmlcontent);
    }

    memset(buf, 0, tmpres);
  }
  if (tmpres < 0)
  {
    perror("Error receiving data");
  }
  free(get);
  free(remote);
  free(ip);
  close(sock);
  return 0;
}


int create_tcp_socket()
{
  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("Can't create TCP socket");
    exit(1);
  }
  return sock;
}

char *get_ip(char *host)
{
  struct hostent *hent;
  //ip address format  123.123.123.123
  int iplen = 15;
  char *ip = (char *)malloc(iplen+1);
  memset(ip, 0, iplen+1);
  if ((hent = gethostbyname(host)) == NULL)
  {
    herror("Can't get IP host by name");
    exit(1);
  }
  //Fixed warning:Can't resolve host with inet_ntop: No space left on device
  //I added iplen+1 to have the iplen extend to 16 since malloc shows for (15)+1, but 
  //the inet did not account for it and doesn't have length of 15 in the buffer at all, 
  //so by changing it to iplen+1 it will be able to have correct length in its buffer
  if (inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen+1) == NULL)
  {
    perror("Can't resolve host with inet_ntop");
    exit(1);
  }
  return ip;
}

char *build_get_query(char *host, char *page)
{
  char *query;
  char *getpage = page;
  //Fixed warning: deprecated conversion from string constant to char*
  //char *tpl = "......" change char to constant char to fixed the string constant
  const char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
  if (getpage[0] == '/') {
    getpage = getpage + 1;
//    fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
  }
  // -5 is to consider the %s %s %s in tpl and the ending \0
  query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
  sprintf(query, tpl, getpage, host, USERAGENT);
  return query;
}


/* stock_client.cpp
 * Name- Ravindu Tharanga Perera Kankaniyage Don
 * User Name- rkankan@g.clemson.edu
 * Course Number- CPSC-8810 MSCS Ready: Module 6 ,summer 2024
 * Project 2: Stock Market: implementing with pipes and sockets
 * 7/30/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sstream>  
#include <iostream>
#include <iomanip>
#include "List.h"
#include "Stock.h"
using namespace std;

/* define a port number to connect to ....*/
#define PORT 7777 // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; // connector's address information 

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info 
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;    // host byte order 
    their_addr.sin_port = htons(PORT);  // short, network byte order 
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);   // zero the rest of the struct 

    if (connect(sockfd,(struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }
    //from MarketParentChild.cpp
    List<Stock> ownedStocks;
    double money = 10000.0;
    cout << "-------------------------\n" << endl;

    cout << "Client: I have $" << fixed << setprecision(2) << money << " and the following stocks:" << endl;
    cout << "-------------------" << endl;
    ownedStocks.display();
    cout << "-------------------" << endl;

    string sym;
    cout << "Client: What stock should client buy (provide valid ticker symbol)? ";
    cin >> sym;

    int quantity;
    cout << "Client: How many should client buy? (must be integer value) ";
    cin >> quantity;
    cout << "\n" << endl;

    ostringstream send_string;
    send_string << sym << " " << quantity << " " << fixed << setprecision(2) << money;

    if (send(sockfd, send_string.str().c_str(), send_string.str().length(), 0) == -1)
        perror("client send");

    
    if ((numbytes = recv(sockfd,  buf, MAXDATASIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';

    int purchasedShares;
    double price;
    char ticker[10], name[100];
    sscanf(buf, "%d@%lf:%[^:]:%[^:]", &purchasedShares, &price, ticker, name);

    if (purchasedShares > 0) {
        Stock newStock(name, ticker, purchasedShares, price);
        ownedStocks.insert(newStock);
        money -= purchasedShares * price;

        cout << "\nClient: client now has $" << fixed << setprecision(2) << money <<" and the following stocks:" <<endl;
        cout << "-------------------" << endl;
        ownedStocks.display();
        cout << "-------------------" << endl;
    } else {
        cout << "\nClient: client now has $" << fixed << setprecision(2) << money <<" and the following stocks:" <<endl;
        cout << "-------------------" << endl;
        ownedStocks.display();
        cout << "-------------------" << endl;
    }

    close(sockfd);
    return 0;



}







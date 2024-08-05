
/* stock_server.cpp
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "List.h"
#include "Stock.h"
#include "Market.h"
using namespace std;




#define MYPORT 7777	// the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 80	 // max size of message between client & server

void sigchld_handler(int s) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void)
{
	int sockfd, new_fd;  			// listen on sock_fd, new connection on new_fd
	struct sockaddr_in my_addr;	// my address information
	struct sockaddr_in their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char buf[MAXDATASIZE];


	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	
	my_addr.sin_family = AF_INET;		 // host byte order
	my_addr.sin_port = htons(MYPORT);	 // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

	if (::bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {

		perror("bind");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	Market market;
    double money = 0.0;  // Persistent money variable

    cout << "I am the Market server and I have : $" << fixed << setprecision(2) << money <<" and the following stocks:"<< endl;
    cout<<"----------------------"<<endl;
    market.readStockFile();
    market.showStocks();

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
            perror("accept");
            continue;
        }
        if (!fork()) { 
            close(sockfd); // child doesn't need the listener

            // char buf[MAXDATASIZE];
            int numbytes;
            if ((numbytes = recv(new_fd,  buf, MAXDATASIZE, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            buf[numbytes] = '\0';

            // Parse the received string
            char* token;
            string sym;
            int quantity = 0;
            double clientMoney= 0.0;

            // Extract symbol
            token = strtok(buf, " ");
            if (token != nullptr) {
                sym = string(token);
            }

            // Extract quantity
            token = strtok(nullptr, " ");
            if (token != nullptr) {
                quantity = stoi(token);
            }

            // Extract client money
            token = strtok(nullptr, " ");
            if (token != nullptr) {
                clientMoney = stod(token);
            }

            cout << "Market server: Market received request for " << quantity << " shares of " 
                 << sym << " for up to $" << fixed << setprecision(2) << clientMoney << endl;

            // Process the stock purchase
            Stock* stock = market.getStockBySymbol(sym);

            if (stock == nullptr) {
                cout << "Market server: Market does not contain " << sym << endl;
                string response = "0@0.00:NULL:No such stock";
                send(new_fd, response.c_str(), response.length() + 1, 0);
            } else {
                int availableShares = min(quantity, stock->getQuantity());
                int purchasedShares = min(static_cast<int>(clientMoney / stock->getPrice()), availableShares);
                double totalCost = purchasedShares * stock->getPrice();

                stock->setQuantity(stock->getQuantity() - purchasedShares);
                money += totalCost;

                cout << "Market server: Confirming " << purchasedShares << " shares @ $" 
                     << fixed << setprecision(3) << stock->getPrice() << " of " 
                     << sym << ": " << stock->getName() << " for $" 
                     << fixed << setprecision(2) << totalCost << endl;

                ostringstream oss;
                oss << purchasedShares << "@" << fixed << setprecision(2) << stock->getPrice() 
                    << ":" << sym << ":" << stock->getName();
                string confirmationStr = oss.str();
                send(new_fd, confirmationStr.c_str(), confirmationStr.length() + 1, 0);
            }

            cout << "I am the Market server and I now have : $" << fixed << setprecision(2) << money <<" and the following stocks:" <<endl;
            cout << "-------------------" << endl;
            market.showStocks();
            cout << "-------------------" << endl;

            market.writeStockFile();

            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this

        // Display stocks at the bottom 
        cout << "I am the Market server and I now have : $" << fixed << setprecision(2) << money <<" and the following stocks:" <<endl;
        cout << "-------------------" << endl;
        market.showStocks();
        cout << "-------------------" << endl;
    }

    return 0;

}



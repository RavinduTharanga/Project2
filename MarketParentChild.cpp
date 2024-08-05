
/* MarketParentChild.cpp
 * Name- Ravindu Tharanga Perera Kankaniyage Don
 * User Name- rkankan@g.clemson.edu
 * Course Number- CPSC-8810 MSCS Ready: Module 6 ,summer 2024
 * Project 2: Stock Market: implementing with pipes and sockets
 * 7/30/2024
 */



#include <sstream>
#include <cstring>
#include <limits>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "List.h"
#include "Stock.h"
#include "Market.h"
using namespace std;

int main(){
    //declare variables needed to create the pipes and fork off the child 
    int fd_top[2];
    int fd_bottom[2];
    pid_t pid;
    char line[80];

    // create two pipes and check for errors
    if (pipe(fd_top) == -1 || pipe(fd_bottom) == -1) {
        cerr << "Pipe creation failed" << endl;
        exit(1);
    }

    // fork off child process
    // check for errors based on return value
    pid = fork();
    if (pid < 0) {
        cerr << "Fork failed" << endl;
        exit(1);
    }

    /*  
	 * block of code for parent 
	 */

    if (pid > 0) { 
        // declare variables 
        //Market object using default parameters
        //money variable set to 0.0
		//any other variables needed ...
        Market market;
        double money = 0.0;

        // close unused ends of the pipes
        close(fd_top[1]);
        close(fd_bottom[0]);

        //display current balance, read in the stockfile, display list of stocks
        cout << "I am the parent and I have : $" << fixed << setprecision(2) << money <<" and the following stocks:"<< endl;
        cout<<"----------------------"<<endl;
        market.readStockFile();
        market.showStocks();

        // wait for child process to send purchase request through the pipe

        bzero(line, sizeof(line));
        read(fd_top[0], line, sizeof(line));

		// parse out parameters
        char* token;
        string sym;
        int quantity = 0;
        double clientMoney = 0.0;

        // Extract symbol
        token = strtok(line, " ");
        if (token != nullptr) {
            sym = string(token);
        }

        // Extract quantity
        token = strtok(nullptr, " ");
        if (token != nullptr) {
            try {
                quantity = stoi(token);
            } catch (const invalid_argument& e) {
                cerr  << e.what() << endl;
                exit(1);
            } catch (const out_of_range& e) {
                cerr << e.what() << endl;
                exit(1);
            }
        }

        // Extract client money
        token = strtok(nullptr, " ");
        if (token != nullptr) {
            try {
                clientMoney = stod(token);
            } catch (const invalid_argument& e) {
                cerr <<  e.what() << endl;
                exit(1);
            } catch (const out_of_range& e) {
                cerr << e.what() << endl;
                exit(1);
            }
        }

		// echo the child's request to the screen
        cout << "Parent: Market received request for " << quantity << " shares of " 
             << sym << " for up to $" << fixed << setprecision(2) << clientMoney << endl;

        // try to find stock by symbol (use Market class methods); report error
        Stock* stock = market.getStockBySymbol(sym);
    
        // if not found, send "0@0.00:NULL:No such stock" to child and exit

        if (stock == nullptr) {
        cout << "Parent: Market does not contain " << sym << endl;
        string response = "0@0.00:NULL:No such stock";
        write(fd_bottom[1], response.c_str(), response.length() + 1);
        exit(0);
         }

        // if found, use stored price, quantity and cost to check for value client request
        int availableShares = min(quantity, stock->getQuantity());
        // double cost = availableShares * stock->getPrice();
        int purchasedShares = min(static_cast<int>(clientMoney / stock->getPrice()), availableShares);

        double totalCost = purchasedShares * stock->getPrice();

        // remove stocks from market
        stock->setQuantity(stock->getQuantity() - purchasedShares);
            money += totalCost;

        // update money
        money += purchasedShares * stock->getPrice();

        // display confirmation to screen
        double initialCost = availableShares * stock->getPrice();

        if (initialCost > clientMoney) {
        cout << "Cost is $ " << fixed << setprecision(2) << initialCost 
            << " but client has only $" << clientMoney 
            << " ; selling " << purchasedShares << endl;
        }
        cout << "Parent: Confirming " << purchasedShares << " shares @ $" 
         << fixed << setprecision(3) << stock->getPrice() << " of " 
         << sym << ": " << stock->getName() << " for $" 
         << fixed << setprecision(2) << totalCost << endl;

        // construct string with purchase confirmation
     
        ostringstream oss;
        oss << purchasedShares << "@" << fixed << setprecision(2) << stock->getPrice() 
        << ":" << sym << ":" << stock->getName();

        // send purchase request to child via pipe
        string confirmationStr = oss.str();
        write(fd_bottom[1], confirmationStr.c_str(), confirmationStr.length() + 1);
    
        // wait for child to exit; display error if appropriate
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            cout << "Child process exited with status " << WEXITSTATUS(status) << endl;
        }

        //then display the updated balance and stocklist
              
        cout << "I am the parent and I now have : $" << fixed << setprecision(2) << totalCost <<" and the following stocks:" <<endl;
        cout << "-------------------" << endl;
        market.showStocks();
        cout << "-------------------" << endl;

        // and  write out the updated stockfile
        market.writeStockFile();

        // and then parent exits
        exit(0);
    }
    
    /*  
	 * block of code for child 
	 */
    else {  
        // Wait 1 second to allow parent display to occur first
        sleep(1);

        // Close unused ends of the pipes
        // child maintains a list of stocks ; the list is initially empty
		// and a money balance, initialized to $10,000
		// and other variables as needed
        close(fd_top[0]);
        close(fd_bottom[1]);

        // Declare variables:
        List<Stock> ownedStocks;
        double money = 10000.0;
       

		// Child displays initial account balance and list of stocks owned
        cout << "-------------------------\n" << endl;
        cout << "Child: I have $" << fixed << setprecision(2) << money << " and the following stocks:" << endl;
        cout << "-------------------" << endl;
        ownedStocks.display();
        cout << "-------------------" << endl;

        string sym;
        cout << "Child: What stock should client buy (provide valid ticker symbol)? ";
        cin >> sym;

		// prompt the user to enter a stock symbol and a quantity
        int quantity;
        cout << "Child: How many should client buy? (must be integer value) ";
        cin >> quantity;
        cout<< "\n"<<endl;

        // build a C++ string that contains sym, quantity and money 
        ostringstream send_string(ostringstream::ate);
        send_string << sym << " " << quantity << " " << fixed << setprecision(2) << money;

        // Create a character buffer for the C-style string
        char buffer[80];
        // use the string functions str() to turn your C++ string
		// object into a string, and then apply c_str() to make
          strncpy(buffer, send_string.str().c_str(), sizeof(buffer) - 1);
		// it a null-terminated string, suitable for sending through the pipe
         buffer[sizeof(buffer) - 1] = '\0'; 
        

        // Send the purchase request string to the parent/server through the pipe
        write(fd_top[1], buffer, strlen(buffer) + 1);
        
        // Get the response from the parent/server through the other pipe
        char response[80];
        read(fd_bottom[0], response, sizeof(response));
        // Parse out the elements of the returned string
        
        int purchasedShares;
        double price;
        char ticker[10], name[100]; 
        sscanf(response, "%d@%lf:%[^:]:%[^:]", &purchasedShares, &price, ticker, name);

        // Make a new stock object based on the return string
        Stock newStock(string(name), string(ticker), purchasedShares, price);


        if (purchasedShares > 0) {
            // Make a new stock object based on the return string
            Stock newStock(name, ticker, purchasedShares, price);
            // Insert it into the list of stocks owned
            ownedStocks.insert(newStock);
            // Update the account balance
            money -= purchasedShares * price;

            // Display the updated balance and stock list
            cout << "\nChild: client now has $" << fixed << setprecision(2) << money <<" and the following stocks:" <<endl;
            cout << "-------------------" << endl;
            ownedStocks.display();
            cout << "-------------------" << endl;
        } else {
            cout << "\nChild: client now has $" << fixed << setprecision(2) << money <<" and the following stocks:" <<endl;
            cout << "-------------------" << endl;
            ownedStocks.display();
            cout << "-------------------" << endl;
        }

        // and then exit

        exit(0);


    }
}

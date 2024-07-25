// #include <sstream>
// #include <cstring>
// #include <limits>

// #include <unistd.h>
// #include <sys/wait.h>
// #include <string.h>
// #include <iostream>
// #include <iomanip>
// #include <sstream>
// #include <string.h>
// #include "List.h"
// #include "Stock.h"
// #include "Market.h"
// using namespace std;

// int main(){
//     //declare variables needed to create the pipes and fork off the child 
//     int fd_top[2];
//     int fd_bottom[2];
//     pid_t pid;
//     char line[80];

//     // create two pipes and check for errors
//     if (pipe(fd_top) == -1 || pipe(fd_bottom) == -1) {
//         cerr << "Pipe creation failed" << endl;
//         exit(1);
//     }

//     // fork off child process
//     // check for errors based on return value
//     pid = fork();
//     if (pid < 0) {
//         cerr << "Fork failed" << endl;
//         exit(1);
//     }

//     if (pid > 0) {  // Parent process
//         // declare variables 
//         Market market;
//         double money = 0.0;

//         // close unused ends of the pipes
//         close(fd_top[1]);
//         close(fd_bottom[0]);

//         //display current balance, read in the stockfile, display list of stocks
//         cout << "I am the parent and I have : $" << fixed << setprecision(2) << money <<" and the following stocks:"<< endl;
//         cout<<"----------------------"<<endl;
//         market.readStockFile();
//         market.showStocks();


//         char line[80];
//         bzero(line, sizeof(line));  // Set the buffer to all zeros

//         // Read the purchase request from the child through the pipe
//         read(fd_top[0], line, sizeof(line));

//         // Display the received message
//         //cout << "parent received: " << line << endl;

//         // wait for child process to send purchase request through the pipe
//         read(fd_top[0], line, sizeof(line));

//         // parse out parameters
//         char* token;
//         string sym;
//         int quantity;
//         double clientMoney;
//         sscanf(line, "%s %d %lf", &sym[0], &quantity, &clientMoney);



//     // Extract symbol
//         token = strtok(line, " ");
//         if (token != nullptr) {
//             sym = string(token);
//         }

//     // Extract quantity
//     token = strtok(nullptr, " ");
//     if (token != nullptr) {
//         try {
//             quantity = std::stoi(token);
//         } catch (const std::invalid_argument& e) {
//             cerr << "Invalid quantity format: " << e.what() << endl;
//             exit(1);
//         } catch (const std::out_of_range& e) {
//             cerr << "Quantity out of range: " << e.what() << endl;
//             exit(1);
//         }
//     }

//     // Extract client money
//     token = strtok(nullptr, " ");
//     if (token != nullptr) {
//         try {
//             clientMoney = std::stod(token);
//         } catch (const std::invalid_argument& e) {
//             cerr << "Invalid money format: " << e.what() << endl;
//             exit(1);
//         } catch (const std::out_of_range& e) {
//             cerr << "Money amount out of range: " << e.what() << endl;
//             exit(1);
//         }
//     }

//     // Echo the client's request
//     cout << "Parent: Market received request for " << quantity << " shares of " 
//          << sym << " for up to $" << fixed << setprecision(2) << clientMoney << endl;

//         // echo the child's request to the screen
//        // cout << "Received request: " << line << endl;

//         // try to find stock by symbol (use Market class methods); report error
//         Stock* stock = market.getStockBySymbol(sym);
    
//         if (stock == nullptr) {
//             // if not found, send "0@0.00:NULL:No such stock" to child and exit
//             string response = "0@0.00:NULL:No such stock";
//             write(fd_bottom[1], response.c_str(), response.length() + 1);
//             exit(0);
//         }

//         // if found, use stored price, quantity and cost to check for value client request
//         int availableShares = min(quantity, stock->getQuantity());
//         double cost = availableShares * stock->getPrice();
//         int purchasedShares = min(static_cast<int>(clientMoney / stock->getPrice()), availableShares);

//         // remove stocks from market
//         stock->setQuantity(stock->getQuantity() - purchasedShares);

//         // update money
//         money += purchasedShares * stock->getPrice();

//         // display confirmation to screen
//         cout << "Sold " << purchasedShares << " shares of " << sym << " at $" << stock->getPrice() << " each." << endl;

//         // construct string with purchase confirmation
//         stringstream ss;
//         ss << purchasedShares << "@" << stock->getPrice() << ":" << sym << ":" << stock->getName();
//         string confirmation = ss.str();

//         // send purchase request to child via pipe
//         write(fd_bottom[1], confirmation.c_str(), confirmation.length() + 1);
    
//         // wait for child to exit; display error if appropriate
//         int status;
//         waitpid(pid, &status, 0);
//         if (WIFEXITED(status)) {
//             cout << "Child process exited with status " << WEXITSTATUS(status) << endl;
//         }

//         //then display the updated balance and stocklist
//         cout << "Updated balance: $" << fixed << setprecision(2) << money << endl;
//         market.showStocks();

//         // and  write out the updated stockfile
//         market.writeStockFile();

//         // and then parent exits
//         exit(0);
//     }
    
//     else {  // Child process
//     // Wait 1 second to allow parent display to occur first
//     sleep(1);

//     // Close unused ends of the pipes
//     close(fd_top[0]);
//     close(fd_bottom[1]);

//     // Declare variables:
//     List<Stock> ownedStocks;
//     double money = 10000.0;

//     // Child displays initial account balance and list of stocks owned
//     cout << "Child: I have $" << fixed << setprecision(2) << money << " and the following stocks:" << endl;
//     cout << "-------------------\n" << endl;
//     ownedStocks.display();
//     cout << "-------------------" << endl;

//     string sym;
//     cout << "Child: What stock should client buy (provide valid ticker symbol)? ";
//     cin >> sym;

//     // Prompt the user to enter the quantity
//     int quantity;
//     cout << "Child: How many should client buy? (must be integer value) ";
//     cin >> quantity;


//     // Build the string using ostringstream
//     // std::ostringstream send_string(std::ostringstream::ate);
//     // send_string << sym << " " << quantity << " " << fixed << setprecision(2) << money;
//     std::ostringstream send_string(std::ostringstream::ate);
//     send_string << sym << " " << quantity << " " << fixed << setprecision(2) << money;

//     // Create a character buffer for the C-style string
//     char buffer[80];
    
//     // Copy the C-stringified version of send_string into the buffer
//     // strncpy(buffer, send_string.str().c_str(), sizeof(buffer) - 1);
//     // buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination
//     strncpy(buffer, send_string.str().c_str(), sizeof(buffer) - 1);
//     buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination
//     // Send the purchase request string to the parent/server through the pipe
//     // write(fd_top[1], buffer, strlen(buffer) + 1);
//         write(fd_top[1], buffer, strlen(buffer) + 1);

    
//     // Get the response from the parent/server through the other pipe
//     char response[80];
//     read(fd_bottom[0], response, sizeof(response));
//     // Parse out the elements of the returned string
//     int purchasedShares;
//     double price;
//     string ticker, name;
//     try {
//         sscanf(response, "%d@%lf:%[^:]:%[^:]", &purchasedShares, &price, &ticker[0], &name[0]);
//     } catch (const exception& e) {
//         cerr << "Error parsing response: " << e.what() << endl;
//         exit(1);
//     }

//     // If the return value from the parent is successful
//     if (purchasedShares > 0) {
//         // Make a new stock object based on the return string
//         Stock newStock(name, ticker, purchasedShares, price);
//         // Insert it into the list of stocks owned
//         ownedStocks.insert(newStock);
//         // Update the account balance
//         money -= purchasedShares * price;
//     }

//     // Display updated balance and stock list 
//     // cout << "Updated balance: $" << fixed << setprecision(2) << money << endl;
//     // cout << "Updated owned stocks:" << endl;
//     // ownedStocks.display();

//     // Exit
//     exit(0);
// }
// }


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

    if (pid > 0) {  // Parent process
        // declare variables 
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

        // Read the purchase request from the child through the pipe
        bzero(line, sizeof(line));
        read(fd_top[0], line, sizeof(line));

        // Parse the received string
        char* token;
        string sym;
        int quantity;
        double clientMoney;

        // Extract symbol
        token = strtok(line, " ");
        if (token != nullptr) {
            sym = string(token);
        }

        // Extract quantity
        token = strtok(nullptr, " ");
        if (token != nullptr) {
            try {
                quantity = std::stoi(token);
            } catch (const std::invalid_argument& e) {
                cerr << "Invalid quantity format: " << e.what() << endl;
                exit(1);
            } catch (const std::out_of_range& e) {
                cerr << "Quantity out of range: " << e.what() << endl;
                exit(1);
            }
        }

        // Extract client money
        token = strtok(nullptr, " ");
        if (token != nullptr) {
            try {
                clientMoney = std::stod(token);
            } catch (const std::invalid_argument& e) {
                cerr << "Invalid money format: " << e.what() << endl;
                exit(1);
            } catch (const std::out_of_range& e) {
                cerr << "Money amount out of range: " << e.what() << endl;
                exit(1);
            }
        }

        // Echo the client's request
        cout << "Parent: Market received request for " << quantity << " shares of " 
             << sym << " for up to $" << fixed << setprecision(2) << clientMoney << endl;

        // try to find stock by symbol (use Market class methods); report error
        Stock* stock = market.getStockBySymbol(sym);
    
        if (stock == nullptr) {
            // if not found, send "0@0.00:NULL:No such stock" to child and exit
            string response = "0@0.00:NULL:No such stock";
            write(fd_bottom[1], response.c_str(), response.length() + 1);
            exit(0);
        }

        // if found, use stored price, quantity and cost to check for value client request
        int availableShares = min(quantity, stock->getQuantity());
        double cost = availableShares * stock->getPrice();
        int purchasedShares = min(static_cast<int>(clientMoney / stock->getPrice()), availableShares);

        // remove stocks from market
        stock->setQuantity(stock->getQuantity() - purchasedShares);

        // update money
        money += purchasedShares * stock->getPrice();

        // display confirmation to screen
        cout << "Sold " << purchasedShares << " shares of " << sym << " at $" << stock->getPrice() << " each." << endl;

        // construct string with purchase confirmation
        stringstream ss;
        ss << purchasedShares << "@" << stock->getPrice() << ":" << sym << ":" << stock->getName();
        string confirmation = ss.str();

        // send purchase request to child via pipe
        write(fd_bottom[1], confirmation.c_str(), confirmation.length() + 1);
    
        // wait for child to exit; display error if appropriate
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            cout << "Child process exited with status " << WEXITSTATUS(status) << endl;
        }

        //then display the updated balance and stocklist
        cout << "Updated balance: $" << fixed << setprecision(2) << money << endl;
        market.showStocks();

        // and  write out the updated stockfile
        market.writeStockFile();

        // and then parent exits
        exit(0);
    }
    
    else {  // Child process
        // Wait 1 second to allow parent display to occur first
        sleep(1);

        // Close unused ends of the pipes
        close(fd_top[0]);
        close(fd_bottom[1]);

        // Declare variables:
        List<Stock> ownedStocks;
        double money = 10000.0;

        // Child displays initial account balance and list of stocks owned
        cout << "Child: I have $" << fixed << setprecision(2) << money << " and the following stocks:" << endl;
        cout << "-------------------\n" << endl;
        ownedStocks.display();
        cout << "-------------------" << endl;

        string sym;
        cout << "Child: What stock should client buy (provide valid ticker symbol)? ";
        cin >> sym;

        // Prompt the user to enter the quantity
        int quantity;
        cout << "Child: How many should client buy? (must be integer value) ";
        cin >> quantity;

        // Build the string using ostringstream
        std::ostringstream send_string(std::ostringstream::ate);
        send_string << sym << " " << quantity << " " << fixed << setprecision(2) << money;

        // Create a character buffer for the C-style string
        char buffer[80];
        
        // Copy the C-stringified version of send_string into the buffer
        strncpy(buffer, send_string.str().c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination

        // Send the purchase request string to the parent/server through the pipe
        write(fd_top[1], buffer, strlen(buffer) + 1);
        
        // Get the response from the parent/server through the other pipe
        char response[80];
        read(fd_bottom[0], response, sizeof(response));
        // Parse out the elements of the returned string
        int purchasedShares;
        double price;
        string ticker, name;
        try {
            sscanf(response, "%d@%lf:%[^:]:%[^:]", &purchasedShares, &price, &ticker[0], &name[0]);
        } catch (const exception& e) {
            cerr << "Error parsing response: " << e.what() << endl;
            exit(1);
        }

        // If the return value from the parent is successful
        if (purchasedShares > 0) {
            // Make a new stock object based on the return string
            Stock newStock(name, ticker, purchasedShares, price);
            // Insert it into the list of stocks owned
            ownedStocks.insert(newStock);
            // Update the account balance
            money -= purchasedShares * price;
        }

        // Exit
        exit(0);
    }
}

#ifndef STOCK_H
#define STOCK_H
#include <string>
#include <iostream>


/*
 * class Stock encapsulates a stock position
 * including name, ticker, price, and numshares 
 * Provides methods used by the stock_server to 
 * to support buying and selling of shares.
 */
using namespace std;

class Stock {

friend ostream& operator<<(ostream& os, const Stock& st);
friend ostream& operator<<(ostream& os, const Stock* st);

public:
	Stock():name("null"), ticker("nul"), quantity(0),price(0.0){};//changed order
	Stock(string n, string sym, int num, double pr);
	Stock(const Stock& s);

	const string getKey() const;
	const string getName() const;
	const string getTicker() const;
	double getPrice() const;
	int getQuantity() const;

	void setName(string s);
	void setTicker(string s);
	void setPrice(double p);
	void setQuantity(int s);


	// relational operators
	bool operator>(const Stock other) const;
	bool operator<(const Stock other) const;
	bool operator==(const string key) const;
	bool operator==(const Stock other) const;
	bool operator>=(const Stock other) const;
	bool operator<=(const Stock other) const;

private:
	string name; //changed order
	string ticker;
	int quantity;
	double price;
};

#endif

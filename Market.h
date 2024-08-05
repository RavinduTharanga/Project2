#ifndef MARKET_H
#define MARKET_H
#include <string>
#include <iostream>
#include <random>
#include "StringList.h"
#include "HashMap.h"
#include "Stock.h"

using namespace std;

/*
 * class Market encapsulates a simulated stock market
 * using a list of Stock objects with ticker symbol (string)
 * as a key value
 */  

const int HASHMAP_SIZE = 101;

class Market {


public:
	Market();
	Market(string mname);
	Market(string mname, int dayNum);

	void readStockFile();
	void readStockFile(int dayNum);

	void writeStockFile();
	void writeStockFile(int dayNum);

	void showStocks() const;

	void step();

	int nextDay();
	int getDay() const;

	void addStock(Stock *s);
	void removeStock(string sym);

	Stock * getStockBySymbol(string sym);

private:
	string name;				// name of this stock market
	int currentDay;			// current day number for this market
	HashMap * stockMap; 		//list of stocks in this market
	StringList  *keyList;
	


};

#endif


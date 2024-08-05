I changed these files because of warnings that occurred when I compiled. Here are the files I changed:

Stock.h

    change the order of the class members

    private:
        string name; //changed order
        string ticker;
        int quantity;
        double price;

Market.h
    change the order of the class members

    private:
        string name;				// name of this stock market
        int currentDay;			// current day number for this market
        HashMap * stockMap; 		//list of stocks in this market
        StringList  *keyList;
	


StringList.h

    comment the following line
        	
    Node<string> * cursor;

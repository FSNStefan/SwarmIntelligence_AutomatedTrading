#ifndef CSVREADER_H
#define CSVREADER_H
class CSVReader{
private:
    vector<string> symbol;
    vector<vector<int>> decisions;
    int population_size, training_percent;
    double initial_money;
    vector<double> prices;

public:
    vector<double> getPrices(){
        return prices;
    }

    vector<string> getSymbol(){
        return symbol;
    }

    vector<vector<int>> getDecisions(){
        return decisions;
    }

    int getTrainingPercent(){
        return training_percent;
    }

    int getPopulationSize(){
        return population_size;
    }

    double * getInitialMoney(){
        return &initial_money;
    }

    void clear(){
        symbol.clear();
        decisions.clear();
        prices.clear();
    }

    void getInitialData()
    {
        ifstream inFile;
        inFile.open(".\\setting\\pso.ini");
        string buffer, value;
        int temp;

        getline(inFile,buffer, ',');
        getline(inFile,buffer);
        population_size = stoi(buffer);
        getline(inFile,buffer, ',');
        getline(inFile,buffer);
        training_percent = stoi(buffer);
        inFile.close();

        inFile.open(".\\setting\\stock.ini");
        getline(inFile,buffer, ',');
        getline(inFile,buffer);
        initial_money = stod(buffer);
        while(getline(inFile,buffer, ','))
        {
            getline(inFile,value);
            temp = stoi(value);
            if(temp == 1)
            {
                symbol.push_back(buffer);
            }
        }
        inFile.close();

    }

    void getSignalAllData(string name)
    {
        ifstream inFile;

        std::string fname;
        std::string csv = ".csv";
        std::string buffer;
        fname = ".\\csv\\evaluated\\All\\"+name;
        fname = fname+csv;
        inFile.open(fname);
        while(getline(inFile,buffer, ','))
        {
            vector<int> buffersignal(9);
            getline(inFile,buffer, ',');
            prices.push_back(stod(buffer));
            getline(inFile,buffer, ',');
            buffersignal.at(0) = stoi(buffer);
            getline(inFile,buffer, ',');
            buffersignal.at(1) = stoi(buffer);
            getline(inFile,buffer, ',');
            buffersignal.at(2) = stoi(buffer);
            getline(inFile,buffer, ',');
            buffersignal.at(3) = stoi(buffer);
            getline(inFile,buffer, ',');
            buffersignal.at(4) = stoi(buffer);
            getline(inFile,buffer, ',');
            buffersignal.at(5) = stoi(buffer);
            getline(inFile,buffer, ',');
            buffersignal.at(6) = stoi(buffer);
            getline(inFile,buffer, ',');
            buffersignal.at(7) = stoi(buffer);
            getline(inFile,buffer);
            buffersignal.at(8) = stoi(buffer);
            decisions.push_back(buffersignal);
        }
    }

};

#endif // CSVREADER_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <string>
#include <ctime>


using namespace std;


class Signalize
{
    private:

    std::vector<string> bdate, sdate;
    std::vector<double> bopen, sopen;
    std::vector<double> bhigh, shigh;
    std::vector<double> blow, slow;
    std::vector<double> bclose, sclose;
    std::vector<string> symbol;
    std::vector<vector <short int>>result;
    std::size_t overall = 0; //Number of Finalcial Data Row
    int seperate_factor;

    void clear1()
    {
        bopen.clear();
        bclose.clear();
        bhigh.clear();
        blow.clear();
        bdate.clear();
    }
    void clear2()
    {
        sopen.clear();
        sclose.clear();
        shigh.clear();
        slow.clear();
        sdate.clear();
    }

    void getInitialData()
    {
        ifstream inFile;
        string buffer,value;
        int temp;
        inFile.open(".\\setting\\stock.ini");

        getline(inFile,buffer);
        while(getline(inFile,buffer, ','))
        {
            getline(inFile,value);
            temp = stoi(value);
            if(temp == 1)
                symbol.push_back(buffer);
        }
    }

    void getFinancialData(string name)
    {
        ifstream inFile;
        std::string fname;
        std::string csv = ".csv";
        std::string buffer;
        unsigned j;
        fname = ".\\csv\\All\\"+name;
        fname = fname+csv;
        inFile.open(fname);
        while(getline(inFile,buffer, ','))
        {
            bdate.push_back(buffer);
            getline(inFile,buffer,',');
            bopen.push_back(stod(buffer));
            getline(inFile,buffer,',');
            bhigh.push_back(stod(buffer));
            getline(inFile,buffer,',');
            blow.push_back(stod(buffer));
            getline(inFile,buffer,',');
            bclose.push_back(stod(buffer));
            getline(inFile,buffer);
            overall++;
        }

        for(j = 0;j < overall;j++)
        {
            sopen.push_back(bopen.at(bopen.size()-1));
            bopen.pop_back();
            sclose.push_back(bclose.at(bclose.size()-1));
            bclose.pop_back();
            sdate.push_back(bdate.at(bdate.size()-1));
            bdate.pop_back();
            shigh.push_back(bhigh.at(bhigh.size()-1));
            bhigh.pop_back();
            slow.push_back(blow.at(blow.size()-1));
            blow.pop_back();
        }
        clear1();
    }
    void process(string name)
    {
        //Calculating Function
        vector<double> ma = MA(sclose, 50, 1); // scolse is vector of Closing Price, 100 is Moving Average Period, 1  is type of Moving Average (1 = Simple, 2 = Exponential)
        vector<double> ema20 = MA(sclose, 20, 2);
        vector<double> ema50 = MA(sclose, 50, 2);
        vector<double> ema100 = MA(sclose, 100, 2);
        vector<double> rsiOb = RSI(sclose, 14); // 14 is RSI Period

        vector<double> commo = Commodities(sclose, shigh, slow, 20); // 20 is period
        vector<double> k = Stochastic(sclose,shigh,slow,1);

        vector<double> willr = WilliamPercentRange(sclose, shigh, slow); //I will modify it soon.
        vector<short int> macd = macdCrossOver(sclose); //I will modify it soon.

        //Evaluating Function
        vector<short int> sma = Crossover(sclose, ma);
        vector<short int> ema2050 = Crossover(ema20, ema50);
        vector<short int> ema20100 = Crossover(ema20, ema100);
        vector<short int> ema50100 = Crossover(ema50, ema100);


        vector<short int> rsi = OverBoughtOverSold(rsiOb, 70, 30); // 70 is Upperbound, 30 is lowerbound
        vector<short int> sto = OverBoughtOverSold(k,70,30);
        vector<short int> cci = OverBoughtOverSold(commo, 200, -200);
        vector<short int> wpr = OverBoughtOverSold(willr, -20, -80);


        ostringstream ss;
        ss << time;
        std::string fname;
        std::string csv = ".csv";
        ofstream myfile;
        fname = ".\\csv\\evaluated\\All\\"+name;
        fname = fname+csv;
        myfile.open(fname);
        int i = (int)sclose.size()-seperate_factor;
        if(i < 0)
            i = 0;
        if(seperate_factor == 0 || i == (int)sclose.size())
            i = 0;
        for(i;i < (int)sclose.size();i++)
        {
            myfile << sdate.at(i);
            myfile << ',';
            myfile << sclose.at(i);
            myfile << ',';
            myfile << sma.at(i);
            myfile << ',';
            myfile << ema2050.at(i);
            myfile << ',';
            myfile << ema50100.at(i);
            myfile << ',';
            myfile << ema20100.at(i);
            myfile << ',';
            myfile << macd.at(i);
            myfile << ',';
            myfile << rsi.at(i);
            myfile << ',';
            myfile << sto.at(i);
            myfile << ',';
            myfile << cci.at(i);
            myfile << ',';
            myfile << wpr.at(i);
            myfile << '\n';
        }
        myfile.close();
    }


    vector<double> MA(vector<double> cprice, unsigned period, int type)
    {
        std::vector<double>average(overall);
        double buffer = 0;
        unsigned period2 = period;
        period--;
        if(type == 1) //Simple
        {
            for(unsigned k=0; k<cprice.size();k++)
            {
                if(k < period)
                    continue;
                else
                {
                    for(unsigned l = k-period; l < k+1;l++)
                        buffer += cprice.at(l);
                    average.at(k) = buffer/period2;
                    buffer = 0;
                }
            }
        }
        if(type == 2) //Exponential
        {
            double mult = 2/((double)period2 + 1); // <<<<<<<<<<<<<<
            for(unsigned k=0; k<cprice.size();k++)
            {
                if(k < period)
                    continue;
                else
                {
                    if(k == period)
                    {
                        for(unsigned l = k-period; l < k+1;l++)
                            buffer += cprice.at(l);
                        average.at(k) = buffer/period2;
                        buffer = 0;
                    }
                    else
                        average.at(k) = ((cprice.at(k)-average.at(k-1))*mult)+average.at(k-1);
                }
            }
        }
        /*if(type == 3) //Volume-Weighted
        {
            double sumv = 0;
            for(unsigned k=0; k<cprice.size();k++)
            {
                if(k < period)
                    continue;
                else
                {
                    for(unsigned l = k-period; l < k+1;l++)
                    {
                        buffer += cprice.at(l)*svolume.at(l);
                        sumv += svolume.at(l);
                    }
                    average.at(k) = buffer/sumv;
                    buffer = 0;
                    sumv = 0;
                }
            }
        }*/
        return average;
    }
    vector<double> RSI(vector<double> cprice, unsigned period)
    {
        unsigned period2 = period;
        period -= 1;
        std::vector<double> RSI (overall);
        double rs = 0 ,gain = 0, loss = 0;
        unsigned k, l;
        for(k=0; k<cprice.size();k++)
        {
            if(k < period)
                continue;
            else
            {
                for(l = k-period; l < k+1;l++)
                {
                    if(l == k-period)
                        continue;
                    else
                    {
                        if(cprice.at(l) > cprice.at(l-1))
                            gain += cprice.at(l);
                        else if(cprice.at(l) < cprice.at(l-1))
                            loss += cprice.at(l);
                    }
                }
                gain = gain/(double)period2;
                loss = loss/(double)period2;
                rs = gain/loss;
                RSI.at(k) = 100-(100/(1+rs));
            }
        }
        return RSI;
    }
    vector<double> Stochastic(vector<double> cprice, vector<double> chigh, vector<double> clow, int type)
    {
        unsigned period = 20;
        period -= 1;
        vector<double> hbuffer, lbuffer,k,d;
        vector<double> buffer(overall);
        double max,min,compute1,compute2;
        for(unsigned j=0; j<cprice.size();j++)
        {
            if(j < period)
            {
                continue;
            }
            else
            {
                for(unsigned l = j-period; l < j+1;l++)
                {
                    hbuffer.push_back(chigh.at(l));
                    lbuffer.push_back(clow.at(l));
                }

                max = findMax(hbuffer);
                min = findMin(lbuffer);
                compute1 = cprice.at(j) - min;
                compute2 = max - min;
                buffer.at(j) = (compute1/compute2)*100;
                hbuffer.clear();
                lbuffer.clear();
            }
        }
        k = MA(buffer,5,1);
        d = MA(k, 5, 1);
        if(type == 1)
            return k;
        if(type == 2)
            return d;
    }
    vector<double> Boillinger(vector<double> cprice, unsigned period, int type)
    {
        period -= 1;
        vector<double> buffer, middle, upper, lower;
        double std;
        unsigned j, k;
        for(unsigned i=0; i<cprice.size();i++)
        {
            if(i < period)
            {
                continue;
            }
            else
            {
                for(j = i; j > i-period;j--)
                    buffer.push_back(cprice.at(j-1));
                std = stdDev(buffer);
                middle = MA(cprice, 20, 1);
                for(k = 0;k < middle.size();k++)
                {
                    upper.push_back(middle.at(k)+std);
                    lower.push_back(middle.at(k)-std);
                }
            }
        }
        if(type == 1)
            return upper;
        if(type == 2)
            return lower;
    }
    vector<double> Commodities(vector<double> cprice, vector<double> chigh, vector<double> clow, unsigned period)
    {
        vector<double> cci(overall), sma, tp, tsma;
        double buffer, tbuffer, md, compute, total;
        unsigned k,l;


        unsigned period2 = period;
        period -= 1;

        for(k=0; k<cprice.size();k++)
        {
            tbuffer = (chigh.at(k) + clow.at(k) + cprice.at(k))/3;
            tp.push_back(tbuffer);
        }
        tsma = MA(tp, period2, 1);
        for(k=0; k<cprice.size();k++)
        {

            if(k < period)
                continue;
            else
            {

                for(l = k-period; l < k+1;l++)
                {
                    total= tp.at(l)-tsma.at(k);
                    buffer += fabs(total);
                }
                md = buffer/period2;
                compute = (tp.at(k) - tsma.at(k))/(0.015*md);
                cci.at(k) = compute;
                buffer = 0;

            }
        }
        return cci;
    }
    vector<double> WilliamPercentRange(vector<double> cprice, vector<double> chigh, vector<double> clow)
    {
        unsigned j,l,period = 14;
        period -= 1;
        vector<double> wpr(overall);
        vector<double> hbuffer,lbuffer;
        double max, min, compute, close;
        for(j=0; j<cprice.size();j++)
        {
            if(j < period)
            {
                continue;
            }
            else
            {
                for(l = j; l > j-period;l--)
                {
                    hbuffer.push_back(chigh.at(l-1));
                    lbuffer.push_back(clow.at(l-1));
                }
                close = cprice.at(j);
                max = findMax(hbuffer);
                min = findMin(lbuffer);
                compute = (max-close)/(max-min)*-100;
                wpr.at(j) = compute;
                hbuffer.clear();
                lbuffer.clear();
            }

        }
        return wpr;
    }
    vector<double> AroonOsc(vector<double> chigh, vector<double> clow) //Can't Use
    {
        unsigned period = 25;
        period--;
        vector<double> aro(overall),arup(overall),ardow(overall);
        int hbuffer = 0, lbuffer = 0;
        unsigned k,l;
        for(k=0; k<chigh.size();k++)
        {
            if(k < period)
                continue;
            else
            {
                for(l = k-period; l < k+1;l++)
                {
                    if(l == k-period)
                        continue;
                    if(chigh.at(l-1) < chigh.at(l))
                    {
                        hbuffer = 0;
                        continue;
                    }
                    else
                        hbuffer++;
                }
                for(l = k-period; l < k+1;l++)
                {
                    if(l == k-period)
                        continue;
                    if(clow.at(l-1) > clow.at(l))
                    {
                        lbuffer = 0;
                        continue;
                    }
                    else
                        lbuffer++;
                }
                arup.at(k) =(25-hbuffer)*4;
                ardow.at(k) =(25-lbuffer)*4;
                aro.at(k) = arup.at(k)-ardow.at(k);
                lbuffer = 0;
                hbuffer = 0;
            }
        }
        return aro;
    }
    /*vector<double> RVI(vector<double> oprice, vector<double> cprice, vector<double> chigh, vector<double> clow, int type)
    {
        unsigned k;
        vector<double> line, single, rvi(overall);
        for(k=0; k<cprice.size();k++)
        {
            rvi.at(k) = (cprice.at(k) - oprice.at(k))/(chigh.at(k) - clow.at(k));
        }
        line = MA(rvi, 10, 1);
        single = MA(line, 4, 3);

        if(type == 1) //Overbouth Oversold
        {
            return line;
        }
        if(type == 2)
        {
            return single;
        }
    }*/

    void printValue(vector<double> init, vector<double> result)
    {
        printf("\n\n::Result::\n\n");
        printf("Initial\tResult\n");
        for(unsigned m = 0;m < init.size();m++)
        {   std::cout << m+1 << "\t";
            std::cout << init.at(m);
            if(m > 0)
            {
                if(init.at(m) > init.at(m-1))
                    cout << " ^";
                else cout << " v";
            }
            std::cout << '\t';
            std::cout << result.at(m) << '\n';
        }
        std::cout << '\n';
    }
    vector<short int> macdCrossOver(vector<double> cprice)
    {
        vector<double> macdLine(overall);
        vector<double> ema1 = MA(cprice, 12, 2);
        vector<double> ema2 = MA(cprice, 26, 2);
        for(unsigned i = 0;i < cprice.size();i++)
        {
            if(ema1.at(i) == 0 || ema2.at(i) == 0)
                continue;
            macdLine.at(i) = ema1.at(i)-ema2.at(i);
        }
        vector<double> macdSignal = MA(macdLine, 9, 2);
        vector<short int> compute;
        compute = Crossover(macdLine,macdSignal);
        return compute;
    }
    vector<short int> Crossover(vector<double> value1, vector<double> value2)
    {
        unsigned k;
        vector<short int> compute(overall);
        vector<double> buffer(overall), evaluated(overall);
        double change;
        for(k = 0;k<value1.size();k++)
        {
            if(value1.at(k) == 0 || value2.at(k) == 0)
                continue;
            else buffer.at(k) = value1.at(k)-value2.at(k);
        }
        for(k = 0;k<buffer.size();k++)
        {
            if(buffer.at(k) == 0)
                continue;
            if(buffer.at(k-1) == 0)
                continue;
            else
            {
                change = (buffer.at(k)-buffer.at(k-1))/buffer.at(k-1);
                evaluated.at(k) = change*100;
            }
        }
        for(k = 0;k<evaluated.size();k++)
        {
            if(evaluated.at(k) == (double)0.0 || evaluated.at(k-1) == (double)0.0)
                continue;
            else
            {
                if(evaluated.at(k) > -0 && evaluated.at(k-1) < -0.0)
                {
                    compute.at(k) = 1;
                    continue;
                }
                if(evaluated.at(k) < -0.0 && evaluated.at(k-1) > -0.0)
                {
                    compute.at(k) = -1;
                    continue;
                }
                else {
                    compute.at(k) = 0;
                    continue;
                }
            }
        }
        return compute;
    }
    vector<short int> OverBoughtOverSold(vector<double> value, int upper, int lower)
    {
        unsigned k;
        vector<short int> compute(overall);
        double val;
        int count = 0, stat = 0;
        for(k = 0;k < value.size();k++)
        {
            val = value.at(k);
            if(val == 0)
                continue;
            else
            {
                if(val > upper && stat == 0)
                {
                    compute.at(k) = -1;
                    stat = 1;
                    continue;
                }
                if(val < lower && stat == 0)
                {
                    compute.at(k) = 1;
                    stat = 1;
                    continue;
                }
                else
                {
                    count = 0;
                    stat = 0;
                }
            }
        }
        return compute;
    }
    vector<short int> Sto(vector<double> cprice, vector<double> chigh, vector<double> clow)
    {
        vector<double> k = Stochastic(cprice, chigh, clow, 1);
        vector<double> d = Stochastic(cprice, chigh, clow, 2);
        unsigned i;
        vector<short int> compute(overall);
        vector<double> buffer(overall), evaluated(overall);
        double change;
        for(i = 0;i<k.size();i++)
        {
            if(k.at(i) == 0 || d.at(i) == 0)
                continue;
            else buffer.at(i) = k.at(i)-d.at(i);
        }
        for(i = 0;i<buffer.size();i++)
        {
            if(buffer.at(i) == 0)
                continue;
            if(buffer.at(i-1) == 0)
                continue;
            else
            {
                change = (buffer.at(i)-buffer.at(i-1))/buffer.at(i-1);
                evaluated.at(i) = change*100;
            }
        }
        for(i = 0;i<evaluated.size();i++)
        {
            if(evaluated.at(i) == (double)0.0 || evaluated.at(i-1) == (double)0.0)
                continue;
            else
            {
                if(k.at(i) >= (double)80.0 && d.at(i) >= (double)80.0)
                    if(evaluated.at(i) > -0 && evaluated.at(i-1) < -0.0)
                    {
                        compute.at(i) = 1;
                        continue;
                    }
                if(k.at(i) <= (double)20.0 && d.at(i) <= (double)20.0)
                    if(evaluated.at(i) < -0.0 && evaluated.at(i-1) > -0.0)
                    {
                        compute.at(i) = -1;
                        continue;
                    }
                else {
                    compute.at(i) = 0;
                    continue;
                }
            }
        }
        return compute;
    }

    double findMax(vector<double> price)
    {
        double max = *max_element(price.begin(), price.end());
        return max;
    }
    double findMin(vector<double> price)
    {
        double min = *min_element(price.begin(), price.end());
        return min;
    }
    double stdDev(vector<double> data)
    {
        double sum = 0.0, mean, std = 0.0;
        unsigned i;
        unsigned total;
        for(i = 0; i < data.size(); ++i)
        {
            sum += data.at(i);
        }
        total = data.size();
        mean = sum/total;
        for(i = 0; i < data.size(); ++i)
            std += pow(data.at(i) - mean, 2);
        return sqrt(std / total);
    }
    public:

    void setSeperateFactor(int set)
    {
        seperate_factor = set;
    }

    void initialize()
    {
        clock_t begin = clock();
        getInitialData();
        cout << "Start\n";
        unsigned i;

        for(i = 0;i < symbol.size();i++)
        {
            getFinancialData(symbol.at(i));
            //cout << symbol.at(i);
            process(symbol.at(i));
            //cout << " pass" << endl;
            overall = 0;
            clear1();
            clear2();
        }
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout << "Overall Time: " << elapsed_secs << "\n";
        printf("Complete");
        getchar();
        system("cls");
    }

};

#ifndef SHARPERATIO_H
#define SHARPERATIO_H
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
#include <math.h>
using namespace std;

class SharpeRatio {
    private:
        vector<double> reop;
        double sharpe_ratio = 0, rfr = 0.75;

        void calculate_sharpe_ratio(){
            double sum=0;
            double avg=0;
            double sumsd=0;
            double sd=0;
            for(int i=0;i<reop.size();i++)
            {
                sum+=reop.at(i);
            }
            avg=sum/reop.size();
            for(int i=0;i<reop.size();i++)
            {
                sumsd+=pow(reop.at(i)-avg,2);
            }
            sd=sqrt(sumsd/reop.size());
            sharpe_ratio=(reop.back()-rfr)/sd;
        }

    public:
        double getsharperatio()
        {
            if(sharpe_ratio == 0)
                calculate_sharpe_ratio();
            return sharpe_ratio;
        }

        void addReturn(double x)
        {
                reop.push_back(x);
        }

};

#endif // SHARPERATIO_H


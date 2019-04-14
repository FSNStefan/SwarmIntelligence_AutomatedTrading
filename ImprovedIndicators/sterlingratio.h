#ifndef STERLINGRATIO_H
#define STERLINGRATIO_H
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

class SterlingRatio {
    private:
        double cev=0,maxev=0,drawdown=0,countmaxdrawdown=0,maxdrawdown=0,summaxdrawdown=0,avgmaxdrawdown=0,car=0,sterlingratio=0;

    public:
        double getMaxDrawdown()
        {
            calculate_avg_max_drawdown();
            return avgmaxdrawdown;
        }

        void calculate_sum_max_drawdown(double current_portfolio){
            cev=current_portfolio; //calculate equity
            if(cev>maxev){
                maxev=cev;  //calculate maximum equity
            }
            drawdown=(cev/maxev-1)*100;
            if(drawdown<maxdrawdown){
                maxdrawdown=drawdown;
                countmaxdrawdown++;
                summaxdrawdown+=maxdrawdown;
            }
        }

        void calculate_avg_max_drawdown(){
            avgmaxdrawdown=summaxdrawdown/countmaxdrawdown;
        }

        double getSterlingRatio(double profit)
        {
            if(avgmaxdrawdown == 0)
                calculate_avg_max_drawdown();
            sterlingratio=profit/fabs((-1*(avgmaxdrawdown*0.9)));
            return sterlingratio;
        }

        void clear()
        {
            cev=0;
            maxev=0;
            drawdown=0;
            countmaxdrawdown=0;
            maxdrawdown=0;
            summaxdrawdown=0;
            avgmaxdrawdown=0;
            car=0;
            sterlingratio=0;
        }
        };

#endif // STERLINGRATIO_H


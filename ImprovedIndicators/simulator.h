#ifndef SIMULATOR_H
#define SIMULATOR_H
#include<iostream>
#include<vector>
#include<random>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <string>
#include <ctime>
#include <math.h>
#include <sterlingratio.h>
#include <sharperatio.h>

using namespace std;

class Simulator{
private:
    vector<double> * weights; //genes
    vector<vector<int>> * decisions;
    vector<double> * prices;
    int volume, training_index;
    double cash, portfolio, year, month, profit, drange;
    double * initial_money;
    int buyc = 0,sellc = 0, sumbuy = 0;
    double sum_weight, decision_result;
    char option_of_obj; //'1' for profit, '2' for Sterling Ratio, '3' for Sharpe Ratio
    SterlingRatio sterling_ratio;
    SharpeRatio sharpe_ratio;

    void calculate_return_sharpe(){
        double retn = *initial_money;
        retn = (portfolio - retn)/retn;
        sharpe_ratio.addReturn(retn);
    }

    void calculate_obj(){
        if(option_of_obj == '2'){
            sterling_ratio.calculate_sum_max_drawdown(portfolio);
        }
        else if(option_of_obj == '3'){
            calculate_return_sharpe();
        }
    }


    void calculate_current_portfolio(int i){
        portfolio = cash + (volume*prices->at(i));
    }

    void sell(int i){
        double n = (volume/sumbuy)*prices->at(i);
        cash = cash + (n - (n*0.002));
        if(sumbuy == 1)
              volume=0;
        else
            volume = volume - (volume/sumbuy);
        sumbuy--;
        calculate_current_portfolio(i);
        calculate_obj();
        sellc++;
    }

    void buy(int i){
        double vol = (cash/4)/(prices->at(i));
        int n = (int)vol;
        cash = cash - (1.002*n*prices->at(i));
        volume = volume + n;
        buyc++;
        sumbuy++;
        calculate_current_portfolio(i);
        calculate_obj();
    }

    void hold(int i){
        calculate_current_portfolio(i);
        calculate_obj();
    }

    void calculate_decision_equation(int i){
        int signal; int j;
        sum_weight = 0;
        decision_result = 0;
        vector<int> * equation = &decisions->at(i);

        for(j = 0; j<equation->size(); j++){
            signal = equation->at(j);
            sum_weight = sum_weight + weights->at(j);
            decision_result = decision_result + weights->at(j)*signal;
        }
    }

    void make_decision(int i){
        if(decision_result/sum_weight<(-1*drange) && volume != 0)
        {
            sell(i);
        }
        else if(decision_result/sum_weight>drange)
        {
            buy(i);
        }
        else
        {
            hold(i);
        }
    }

    void simulate(int index, char where_to_begin){
        unsigned i;
        cash = *initial_money;
        portfolio = cash;
        volume = 0; buyc = 0; sellc = 0;

        if(where_to_begin == '1'){
            for(i = 0; i<index; i++){
                calculate_decision_equation(i);
                make_decision(i);
            }
        }
        else{
            for(i = index; i<decisions->size(); i++){
                calculate_decision_equation(i);
                make_decision(i);
            }
        }

        double temp = *initial_money;
        profit = (portfolio - temp)/temp;
    }

public:
    void setOption(char ch){
        option_of_obj = ch;
    }

    vector<vector<int>> * getDecisions(){
        return decisions;
    }

    void setWeights(vector<double> * vd){
        weights = vd;
    }

    void setTrainingIndex(int t){
        training_index = t;
    }

    void setPrices(vector<double> * ps){
        prices = ps;
    }

    void setInitialMoney(double * initial){
        initial_money = initial;
    }

    void setDecisionRange(double i){
        drange = i;
    }

    int getBuyCounts(){
        return buyc;
    }

    int getSellCounts(){
        return sellc;
    }

    double getProfit(){
        return profit;
    }

    double getPortfolio(){
        return portfolio;
    }

    vector<double> * getWeights(){
        return weights;
    }

    void setDecisions(vector<vector<int>> * decides){
        decisions = decides;
    }

    void train(){
        simulate(training_index, '1');
    }

    void test(){
        simulate(training_index, '2');
    }

    double getSharpeRatio(){
        return sharpe_ratio.getsharperatio();
    }

    double getMaximumDrawdown(){
        return sterling_ratio.getMaxDrawdown();
    }

    double getSterlingRatio(){
        return sterling_ratio.getSterlingRatio(profit);
    }
};

#endif // SIMULATOR_H


#ifndef ANTIBODY_HEADER
#define ANTIBODY_HEADER
#include<iostream>
#include<vector>
#include<random>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <string>
#include <ctime>
#include <math.h>
#include <simulator.h>

using namespace std;

std::default_random_engine gen;
std::uniform_real_distribution<double> distrib(0.0,1.0);

class Antibody{
private:
    vector<double> weights; //genes
    vector<double> * prices;
    int volume, training_index;
    double cash, portfolio, year, month, profit;
    double * initial_money;
    std::size_t y,m,fy,fm;
    vector<double> r1, r2;
    vector<vector<int>> * decisions;
    int buyc = 0, sellc = 0;
    char option; //'1' for profit, '2' for Sterling Ratio, '3' for Sharpe Ratio
    Simulator sim;
    double Pm; //Pm = hypermutation rate

    void randij(int * i, int * j){
        *i = 1; *j = 1;
        while(i==j || j<i){
            double f = distrib(gen);
            int r = (int) (f*(weights.size()-1));
            *i = r;
            int t = *i;
            f = distrib(gen);
            r = t + (int) (f*(weights.size()-1-t));
            *j = r;
        }
    }

public:
    void setOption(char ch){
        option = ch;
    }

    void initialize(int dimension, vector<vector<int>> * decides){
        int i; double f;
        decisions = decides;
        sim.setDecisions(decisions);
        sim.setOption(option);
        sim.setInitialMoney(initial_money);
        sim.setPrices(prices);
        sim.setTrainingIndex(training_index);
        for(i = 0; i<dimension; i++){
            f = distrib(gen);
            weights.push_back(f);
        }
        sim.setWeights(&weights);
    }

    void somatic_hypermutate(){
        int i, j;
        double f = distrib(gen);
        if(f<Pm){
            randij(&i, &j);
            double temp = weights.at(i);
            weights.at(i) = weights.at(j);
            weights.at(j) = temp;
        }
    }

    double getSterlingRatio(){
        return sim.getSterlingRatio();
    }

    double getSharpeRatio(){
        return sim.getSharpeRatio();
    }

    double getPm(){
        return Pm;
    }

    void setTrainingIndex(int i){
        training_index = i;
    }

    void setInitialMoney(double * im){
        initial_money = im;
    }

    void setPrices(vector<double> * ps){
        prices = ps;
    }
    void setDecisionRange(double i){
        sim.setDecisionRange(i);
    }

    double run(){
        sim.train();
        buyc = sim.getBuyCounts();
        sellc = sim.getSellCounts();
        profit = sim.getProfit();
        if(option == '1'){
            return profit;
        }
        else if(option == '2'){
            return sim.getSterlingRatio();
        }
        else{
            return sim.getSharpeRatio();
        }
    }

    void calculatePm(double best_affinity){
        Pm = (best_affinity-profit)/profit;
    }

    double getProfit(){
        return profit;
    }
    double getPort()
    {
        return portfolio;
    }
    int getBuycout()
    {
        return buyc;
    }
    int getSellcout()
    {
        return sellc;
    }
    vector<double> getWeights(){
        return weights;
    }
    void setWeights(vector<double> * ws){
        weights = *ws;
    }

};
#endif


#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER
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

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0,1.0);

class Particle{
private:
    vector<double> weights; //positions
    vector<double> old_weights;
    vector<double> * gbest;
    vector<double> * pbest;
    vector<double> * prices;
    vector<double> velocities;
    int volume, training_index;
    double c1, c2, w, cash, portfolio, profit;
    double * initial_money;
    std::size_t y,m,fy,fm;
    vector<double> r1, r2;
    vector<vector<int>> * decisions;
    char option; //'1' for profit, '2' for Sterling Ratio, '3' for Sharpe Ratio
    int buyc = 0,sellc = 0;
    Simulator sim;


    void update_velocities(){
        double f;
        unsigned i; r1.clear(); r2.clear();
        for(i = 0;i<velocities.size(); i++){
            f = distribution(generator);
            r1.push_back(f);
            f = distribution(generator);
            r2.push_back(f);
            velocities.at(i) = (w * velocities.at(i)) + (c1 * r1.at(i) * (pbest->at(i) - weights.at(i)))
                    +(c2 * r2.at(i) * (gbest->at(i)-weights.at(i)));
        }
    }

    void update_positions(){
        unsigned i;
        for(i=0;i<weights.size();i++){
            weights.at(i) = weights.at(i) + velocities.at(i);
            if(weights.at(i) <= 0.0)
                weights.at(i) = 0.0;
            else if(weights.at(i) >= 1.0)
                weights.at(i) = 1.0;
        }
    }

public:
    void initialize(int dimension, vector<vector<int>> * decides){
        int i; double j, f;
        decisions = decides;
        sim.setDecisions(decisions);
        sim.setInitialMoney(initial_money);
        sim.setPrices(prices);
        sim.setOption(option);
        sim.setTrainingIndex(training_index);
        for(i = 0; i<dimension; i++){
            j = 0.0;
            velocities.push_back(j);
            f = distribution(generator);
            weights.push_back(f);
        }
        sim.setWeights(&weights);
    }

    void setTrainingIndex(int i){
        training_index = i;
    }

    vector<double> * getWeights(){
        old_weights.clear();
        unsigned i; vector<double> * vf;
        for(i=0;i<weights.size(); i++){
            old_weights.push_back(weights.at(i));
        }
        vf = &old_weights;
        return vf;
    }

    void setCoefficients(double c_1, double c_2, double inertia){
        c1 = c_1;
        c2 = c_2;
        w = inertia;
    }

    void setInitialMoney(double * im){
        initial_money = im;
    }

    double getSterlingRatio(){
        return sim.getSterlingRatio();
    }

    double getSharpeRatio(){
        return sim.getSharpeRatio();
    }

    double getMaxDrawdown(){
        return sim.getMaximumDrawdown();
    }

    void setPrices(vector<double> *ps){
        prices = ps;
    }

    void setGBest(vector<double> * Pg){
        gbest = Pg;
    }

    void setPBest(vector<double> * Pi){
        pbest = Pi;
    }

    void setOption(char ch){
        option = ch;
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
        else
            return sim.getSharpeRatio();
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
    void update(){
        update_velocities();
        update_positions();
    }
};
#endif

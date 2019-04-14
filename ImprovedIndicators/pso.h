#ifndef PSO_H
#define PSO_H
#include <iostream>
#include <math.h>
#include <vector>
#include <particle.h>

using namespace std;

class Swarm{
private:
    vector<vector<int>> decisions;
    vector<Particle * > particles;
    vector<double> prices;
    vector<double> obj_vals;
    vector<int> index_to_pbests;
    vector<string> * symbol;
    int index_to_gbest, MAXCALLS, calls, training_percent, population_size;
    double * initial_money;
    double w, w1, w2, c1, c2, c1f, c1i, c2f, c2i, drange;
    int type = 0;
    char option; //'1' for profit, '2' for Sterling Ratio, '3' for Sharpe Ratio
    int buy, sell;

    void select_gbest(){
        unsigned i; Particle * p;
        double q, val;
        p = particles.at(0);
        if(option == '1')
            q = p->getProfit();
        else if(option == '3')
            q = p->getSharpeRatio();
        else
            q = p->getSterlingRatio();
        index_to_gbest = 0;
        for(i=1; i<index_to_pbests.size(); i++){
            p = particles.at(i);
            if(option == '1')
                val = p->getProfit();
            else if(option == '3')
                val = p->getSharpeRatio();
            else
                val = p->getSterlingRatio();
            if(val>=q){
                index_to_gbest = i;
            }
        }
    }

    int select_pbests(int i, int j, int k){
        if(obj_vals.at(i)>obj_vals.at(j)){
            if(obj_vals.at(i)>obj_vals.at(k)){
                return i;
            }
            else
                return k;
        }
        else{
            if(obj_vals.at(j)>obj_vals.at(k)){
                return j;
            }
            else
                return k;
        }
    }

public:
    void setSymbols(vector<string> * syms){
        symbol = syms;
    }

    void setDecisions(vector<vector<int>> ds){
        decisions = ds;
    }
    void setDecisionRange(double i){
        drange = i;
    }

    void setOption(char ch){
        option = ch;
    }

    void setPrices(vector<double> ps){
        prices = ps;
    }

    void setTrainingPercentage(int t){
            training_percent = t;
    }

    void setPopulationSize(int n){
        population_size = n;
    }

    void setCoefficients(double c_1_1, double c_1_2, double c_2_1, double c_2_2, double inertia_1, double inertia_2){
        c1i = c_1_1;
        c1f = c_1_2;
        c2i = c_2_1;
        c2f = c_2_2;
        w1 = inertia_1;
        w2 = inertia_2;
        w = w1;
        c1 = c1i;
        c2 = c2i;
    }


    void setInitialMoney(double * im){
        initial_money = im;
    }


    void setMaxObjCalls(int i){
        MAXCALLS = i;
    }

    void initilize(){
        int i, t;
        t = (training_percent * decisions.size())/100;
        for(i = 0; i<population_size; i++){
            Particle * p = new Particle();
            p->setOption(option);
            p->setTrainingIndex(t);
            p->setPrices(&prices);
            p->setInitialMoney(initial_money);
            p->setCoefficients(c1,c2,w);
            p->initialize(10, &decisions);
            p->setDecisionRange(drange);
            particles.push_back(p);
        }
    }

    void getResults(int stock){
        size_t ns = (size_t)stock;
        Particle * p;
        p = particles.at(index_to_gbest);
        vector<double> * vf;
        vf = p->getWeights();
        double train = p->getProfit(); //Not change!!!
        double ratio = 0;
        if(option == '3')
            ratio = p->getSharpeRatio();
        else if(option == '2')
            ratio = p->getSterlingRatio();
        buy = p->getBuycout();
        sell = p->getSellcout();
        //Don't forget to show the results of Sterling and Sharpe Ratio
        cout << "Objective function value : " << train << endl;
        cout << "Objective Ratio : " << ratio << endl;
        cout << "Buy = " << buy << " times \t Sell = " << sell << " times" <<endl;
        std::string fname;
        ofstream myfile;
        fname = ".\\csv\\result\\pso.csv";
        myfile.open(fname, ofstream::out | ofstream::app);
        myfile << symbol->at(ns);
        myfile << ',';
        myfile << "Train";
        myfile << ',';
        myfile << train;
        myfile << ',';
        myfile << population_size;
        myfile << ',';
        myfile << drange;
        myfile << ',';
        if(option == '1')
            myfile << "Profit";
        else if(option == '3')
            myfile << "Sharpe";
        else
            myfile << "Sterling";
        myfile << ',';
        myfile << ratio;
        myfile << ',';
        myfile << buy;
        myfile << ',';
        myfile << sell;
        myfile << '\n';
        myfile.close();
    }

    vector<double> * getBestWeights(){
        Particle * p;
        p = particles.at(index_to_gbest);
        return p->getWeights();
    }

    void run(){
        calls = 0;
        while(calls < MAXCALLS){
            //Simulate and evaluate
            index_to_pbests.clear();
            obj_vals.clear();
            unsigned i; double temp; Particle * p;
            for(i=0; i<particles.size();i++){
                p = particles.at(i);
                temp = p->run();
                calls++;
                obj_vals.push_back(temp);
            }

            //Select Pi and Pg
            for(i=0;i<obj_vals.size();i++){
                int index;
                if(i==0)
                    index = select_pbests(obj_vals.size()-1, i, i+1);
                else if(i==obj_vals.size()-1)
                    index = select_pbests(i-1,i,0);
                else
                    index = select_pbests(i-1, i, i+1);
                index_to_pbests.push_back(index);
            }
            select_gbest();

            //Update acceleration coefficients
            w = (w1-w2) * ((MAXCALLS - calls)/MAXCALLS) + w2;
            c1 = (c1f - c1i) * (calls/MAXCALLS) + c1i;
            c2 = (c2f - c2i) * (calls/MAXCALLS) + c2i;

            //Prepare for the next iteration
            vector<double> * weights;
            vector<double> * gbest_weights;
            p = particles.at(index_to_gbest);
            gbest_weights = p->getWeights();
            for(i=0; i<particles.size(); i++){
                p = particles.at(index_to_pbests.at(i));
                weights = p->getWeights();
                p = particles.at(i);
                p->setPBest(weights);
                p->setGBest(gbest_weights);
                p->setCoefficients(c1, c2, w);

                //Update velocity and position of each particle.
                p->update();

            }
        }
        cout << "Finished" << endl;
    }
};

#endif // PSO_H

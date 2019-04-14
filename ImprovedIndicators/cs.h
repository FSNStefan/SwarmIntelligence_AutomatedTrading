#ifndef CS_H
#define CS_H
#include<iostream>
#include <math.h>
#include <vector>
#include<antibody.h>

using namespace std;


class ClonalSelection{
private:
    vector<vector<int>> decisions; //signals from indicators
    vector<double> weights_of_the_best; //store the best weights
    vector<Antibody * > antibodies; Antibody best_one;
    vector<Antibody> clones; //memory vector for storing clones during each iteration
    vector<int> orders; //tagging for sorting
    vector<double> prices;
    vector<double> obj_vals;
    vector<string> * symbol;
    int index_to_best, MAXCALLS, calls, training_percent, population_size;
    double * initial_money;
    char option; //'1' for profit, '2' for Sterling Ratio, '3' for Sharpe Ratio
    double init, beta, k, drange;
    int buy, sell, pb, pd;
    //pb = percent of antibodies with best affinity
    //pd = percent of antibodies to be replaced
    //beta = multiplying factor used to calculate number of clones

    int calculateNC(){
        return (int) (beta * population_size);
    }

    void quickSort(int first, int last){
        int pivotElement;
        if(first<last){
            pivotElement = pivot(first, last);
            quickSort(first, pivotElement-1);
            quickSort(pivotElement+1, last);
        }
    }

    int pivot(int first, int last){
        int p = first;
        double pivotElement = obj_vals.at(first);
        for(int i = first+1; i<= last; i++){
            if(obj_vals.at(i) <= pivotElement){
                p++;
                swap(i, p);
            }
        }
        swap(p, first);
        return p;
    }

    void swap(int a, int b){
        int temp = a;
        orders.at(a) = b;
        orders.at(b) = temp;
    }

public:
    void setTrainingPercentage(int t){
            training_percent = t;
    }

    void setDecisions(vector<vector<int>> decs){
        decisions = decs;
    }

    void setDecisionRange(double i){
        drange = i;
    }

    void setSymbols(vector<string> * syms){
        symbol = syms;
    }

    void setPopulationSize(int n){
        population_size = n;
    }

    void setOption(char ch){
        option = ch;
    }

    void setInitialMoney(double * im){
        initial_money = im;
    }

    void setMaxObjCalls(int i){
        MAXCALLS = i;
    }

    void setPrices(vector<double> ps){
        prices = ps;
    }

    void setParameters(int percent_to_be_cloned, int percent_to_be_replaced, double multiplying_factor){
        pb = percent_to_be_cloned;
        pd = percent_to_be_replaced;
        beta = multiplying_factor;
    }

    void initilize(){
        int i, t;
        t = (training_percent * decisions.size())/100;
        for(i = 0; i<population_size; i++){
            Antibody * a = new Antibody();
            a->setOption(option);
            a->setTrainingIndex(t);
            a->setPrices(&prices);
            a->setInitialMoney(initial_money);
            a->initialize(10, &decisions);
            a->setDecisionRange(drange);
            antibodies.push_back(a);
        }
    }

    void getResults(int stock){
        size_t ns = (size_t)stock;
        double obj = best_one.getProfit();
        double ratio = 0;
        if(option == '3')
            ratio = best_one.getSharpeRatio();
        else if(option == '2')
            ratio = best_one.getSterlingRatio();
        buy = best_one.getBuycout();
        sell = best_one.getSellcout();
        cout << "Objective function value : " << obj << endl;
        cout << "Objective Ratio : " << ratio << endl;
        cout << "Buy = " << buy << " times \t Sell = " << sell << " times" <<endl;
        std::string fname;
        ofstream myfile;
        fname = ".\\csv\\result\\cs.csv";
        myfile.open(fname, ofstream::out | ofstream::app);
        myfile << symbol->at(ns);
        myfile << ',';
        myfile << "Train";
        myfile << ',';
        myfile << obj;
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

    void run(){
        calls = 0; index_to_best = 0;
        Antibody * ab;
        Antibody * a;
        vector<Antibody> tempa;
        vector<Antibody> tempb;
        vector<int> temp_orders;
        while(calls < MAXCALLS){
            //Simulate and evaluate
            unsigned i, j; double temp;

            for(i=0; i<antibodies.size();i++){
                a = antibodies.at(i);
                temp = a->run();
                calls++;
                obj_vals.push_back(temp);
            }

            //Select n best antibodies

            k = (int) (pb * population_size)/100; // Now k is the number of antibodies to be selected

            for(i=0; i<obj_vals.size();i++){
                orders.push_back(i);
                temp_orders.push_back(i);
            }
            quickSort(0, obj_vals.size()-1); //Sort from min to max
            index_to_best = obj_vals.size() - 1;
            for(i=index_to_best; i>obj_vals.size()-k; i--){
                //Reverse loop to get max to min
                a = antibodies.at(orders.at(i));
                Antibody tempo = *a;
                tempa.push_back(tempo);
            }

            //Clone n best affinity for Nc
            k = calculateNC(); // Now k is the number of clones for each selected antibody
            double best_aff;
            for(i=0; i<tempa.size(); i++){
                ab = &tempa.at(i);
                a = antibodies.at(index_to_best);
                if(option == '1')
                    best_aff = a->getProfit();
                else if(option == '3')
                    best_aff = a->getSharpeRatio();
                else
                    best_aff = a->getSterlingRatio();
                ab->calculatePm(best_aff);
                for(j=0; j<k; j++){
                    clones.push_back(*ab);
                }
            }

            //Hypermutate the clones
            for(i=0; i<clones.size(); i++){
                a = &clones.at(i);
                a->somatic_hypermutate();
                obj_vals.push_back(a->run());
                calls++;
            }
            orders.clear();
            for(i=0; i<obj_vals.size();i++){
                orders.push_back(i);
            }
            quickSort(0, obj_vals.size()-1);

            //Clonal deletion
            k = (int) (pd * population_size)/100; // Now k is the number of antibody to be replaced
            for(i = 0; i<population_size - k + 1; i++){
                //Take the best (population - k) pre-mutated antibodies to our memory
                a = antibodies.at(temp_orders.at(population_size - i - 1));
                tempb.push_back(*a);
            }
            index_to_best = 0;
            for(i = 0; i<k; i++){
                //Insert the new mutant antibodies to our memory
                tempb.push_back(clones.at(orders.at(k - i)));
            }
            antibodies.clear(); //Wipe out everything from our old memory
            vector<vector <double> > temp_weights;
            for(i = 0; i<population_size; i++){
                //Propose our new memory vector
                Antibody abc = tempb.at(i);
                a = &abc;
                temp_weights.push_back(a->getWeights());
                antibodies.push_back(a);
            }
            for(i=0; i<population_size; i++){
                a = antibodies.at(i);
                a->setWeights(&temp_weights.at(i));
            }

            tempb.clear();
            tempa.clear();
            obj_vals.clear();
            clones.clear();
            orders.clear();
            temp_orders.clear();
        }
        a = antibodies.at(0);
        best_one = *a;
        weights_of_the_best = best_one.getWeights();
        cout << "Finished" << endl;
    }

    vector<double> * getBestWeights(){
        return &weights_of_the_best;
    }
};

#endif // CS_H

#include <iostream>
#include <pso.h>
#include <ctime>
#include <cs.h>
#include <simulator.h>
#include <csvreader.h>
#include <signalize.h>

using namespace std;

void test(Simulator * sim, vector<double> * best_weights, double * initial_money, size_t i, vector<string> * sym, int training_percent, string algo, char option, int pop, double range);

int main()
{
    Signalize signal;
    signal.setSeperateFactor(500);
    signal.initialize();

    CSVReader csv;
    vector<double> * best_weights;
    clock_t begin, end;
    double elapsed_secs;

    cout << "------------------------------------------------------------" << endl
         << "************ PSO : ***************" << endl;

    unsigned i, j;
    double * initial_money;
    double drange;
    csv.getInitialData();
    int training = csv.getTrainingPercent();
    vector<string> symbol = csv.getSymbol();
    int population;
    vector<vector<int>> decisions;
    initial_money = csv.getInitialMoney();
    vector<double> prices;
    int size = symbol.size();
    begin = clock();
    for(j = 0;j < 2;j++)
    {
        if(j == 0)
            population = 20;
        if(j == 1)
            population = 25;
        /*for(k = 0;k < 3;k++)
        {
            if(k == 0)
                drange = 0.1;
            if(k == 1)
                drange = 0.3;
            if(k == 2)
                drange = 0.5;
            for(i = 0;i < size;i++)
            {
                Swarm pso;
                cout << "Decision : " << drange << "\t" << "Population : " << population << endl;
                cout << "\n-----------------------------------" << endl;
                cout << "Stock #" << i << "\t" << symbol.at(i) << endl;
                double w1 = 0.9, w2 = 0.4, c1f = 2.5, c1i = 0.5, c2f = 0.5, c2i = 2.5;
                pso.setCoefficients(c1i, c1f, c2i, c2f, w1, w2);
                pso.setSymbols(&symbol);
                csv.getSignalAllData(symbol.at(i));
                decisions = csv.getDecisions();
                prices = csv.getPrices();
                pso.setPopulationSize(population);
                pso.setPrices(prices);
                pso.setOption('1');
                pso.setTrainingPercentage(training);
                pso.setDecisions(decisions);
                pso.setInitialMoney(initial_money);
                pso.setDecisionRange(drange);
                pso.initilize();
                pso.setMaxObjCalls(150000);
                pso.run(); //
                pso.getResults(i);

                Simulator sim;
                sim.setPrices(&prices);
                best_weights = pso.getBestWeights();
                sim.setDecisions(&decisions);
                sim.setDecisionRange(drange);
                test(&sim, best_weights, initial_money, i, &symbol, training, "pso", '1', population, drange);
                csv.clear();
            }
        }*/


        cout << "------------------------------------------------------------" << endl << "************ Clonal Selection : ***************" << endl;
        for(i = 0; i<size;i++)
        {
            ClonalSelection cs;
            cout << "\n-----------------------------------" << endl;
            cout << "Stock # " << i << endl;
            cs.setParameters(100,20,1);
            cs.setSymbols(&symbol);
            csv.getSignalAllData(symbol.at(i));
            decisions = csv.getDecisions();
            prices = csv.getPrices();
            cs.setPrices(prices);
            cs.setOption('1');
            cs.setDecisions(decisions);
            cs.setTrainingPercentage(90);
            cs.setPopulationSize(population);
            cs.setInitialMoney(initial_money);
            cs.setDecisionRange(0.1);
            cs.initilize();
            cs.setMaxObjCalls(150000);
            cs.run();
            cs.getResults(i);

            Simulator sim;
            sim.setPrices(&prices);
            best_weights = cs.getBestWeights();
            sim.setDecisions(&decisions);
            sim.setDecisionRange(0.1);
            test(&sim, best_weights, initial_money, i, &symbol, training, "cs", '1', population, drange);
            csv.clear();
        }
        system("cls");
    }
    end = clock();
    elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Elapsed time : " << elapsed_secs << endl;
    cout << "Average time used per quote : " << elapsed_secs/(int)size << endl;
    getchar();
    return 0;
}

void test(Simulator * sim, vector<double> * best_weights, double * initial_money, size_t i, vector<string> * sym, int training_percent, string algo, char option, int pop, double range){
    int t = (training_percent * sim->getDecisions()->size())/100;
    sim->setInitialMoney(initial_money);
    sim->setOption(option);
    sim->setWeights(best_weights);
    sim->setTrainingIndex(t);
    sim->test();
    double obj = sim->getProfit();
    double ratio = 0;
    if(option == '2')
        ratio = sim->getSharpeRatio();
    if(option == '3')
        ratio = sim->getSterlingRatio();
    cout << "Objective function value :" << sim->getProfit() << endl;
    cout << "Objective Ratio : " << ratio << endl;
    cout << "Final portfolio value :" << sim->getPortfolio() << endl;
    cout << "Initial portfolio value :" << *initial_money << endl;
    int buy = sim->getBuyCounts();
    int sell = sim->getSellCounts();
    cout << "Buy = " << buy << " times \t Sell = " << sell << " times" <<endl;
    cout << "------------------------------------------------" << endl;

    std::string fname;
    ofstream myfile;
    fname = ".\\csv\\result\\" + algo + ".csv";
    myfile.open(fname, ofstream::out | ofstream::app);
    myfile << sym->at(i);
    myfile << ',';
    myfile << "Test";
    myfile << ',';
    myfile << obj;
    myfile << ',';
    myfile << pop;
    myfile << ',';
    myfile << range;
    myfile << ',';
    if(option == '1')
        myfile << "Profit";
    if(option == '2')
        myfile << "Sharpe";
    if(option == '3')
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

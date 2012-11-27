#include "ResultsTest.h"
#include "Results.h"
#include <cstdlib>
#include <iostream>

void ResultsTest::easy_usage(){
    QHash<QString,double> param_comb;
    for(unsigned int x=0;x<10;x++){
        param_comb["x"] = x;
        for(unsigned int y=0;y<10;y++){
            param_comb["y"] = y;
            for(unsigned int z=0;z<10;z++){
                param_comb["z"] = z;
                QHash<QString,double> results;
                results["dek"] = 100*x + 10*y + z;
                Results::getInstance().add_values(param_comb, results);
            }
        }
    }

    for(unsigned int x=0;x<10;x++){
        param_comb["x"] = x;
        for(unsigned int y=0;y<10;y++){
            param_comb["y"] = y;
            for(unsigned int z=0;z<10;z++){
                param_comb["z"] = z;
                double expected = 100*x + 10*y + z;
                QCOMPARE(Results::getInstance().value(param_comb,"dek"), expected);
            }
        }
    }
}

inline uint qHash(const QHash<QString,double> hash)
{
    uint h=0;
    QString string;
    foreach(string, hash.keys())
        h += qHash(string) + hash[string];
    return h;
}

void ResultsTest::random_access(){
    QHash< QHash<QString,double>, QHash<QString,double> > mock;
    QStringList parameters; parameters << "wurst" << "bla" << "blub";
    QStringList results; results << "r1" << "hein" << "3r_2";
    std::cout << "Creating 100 entries for testing";
    for(unsigned int i=0; i<100; i++){
        QHash<QString,double> param_comb;
        foreach(QString p, parameters){
            param_comb[p] = rand() % 10;
        }

        QHash<QString,double> result_values;
        foreach(QString r, results){
            result_values[r] = rand();
        }
        mock[param_comb] = result_values;
        Results::getInstance().add_values(param_comb, result_values);
        if(i%10 == 0) std::cout << ".";
    }

    std::cout << std::endl << "Testing...";
    QHash<QString,double> param_comb;
    foreach(param_comb,mock.keys()){
        foreach(QString r,results){
            QCOMPARE(Results::getInstance().value(param_comb,r), mock[param_comb][r]);
        }
    }
}


QTEST_MAIN(ResultsTest)
//#include "ResultsTest.moc"

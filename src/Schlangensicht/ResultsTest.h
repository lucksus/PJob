#ifndef RESULTSTEST_H
#define RESULTSTEST_H
#include <QtTest/QtTest>

class ResultsTest : public QObject{
Q_OBJECT
private slots:
    void easy_usage();
    void random_access();
};

#endif // RESULTSTEST_H

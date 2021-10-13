#ifndef SIMPLETEST_H
#define SIMPLETEST_H

#include <QObject>
#include <QtTest/QtTest>
#include "mainwindow.h"

class SimpleTest : public QObject
{
    Q_OBJECT

public:
    explicit SimpleTest(QObject *parent = nullptr);

private:
    MainWindow w;

private slots:
    void testCase1();
    void testCase2();
    void testCase3();
    void testCase4();
    void testCase5();
    void testCase6();
    void testCase7();
    void testCase8();
};

#endif // SIMPLETEST_H

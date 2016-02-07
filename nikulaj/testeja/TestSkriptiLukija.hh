#ifndef TESTSKRIPTILUKIJA
#define TESTSKRIPTILUKIJA


// Qt
#include <QtTest/QtTest>


#include "SkriptiLukija.hh"


class TestSkriptiLukija : public QObject
{
    Q_OBJECT
public:
        TestSkriptiLukija();

private slots:
    void testaaTaistelijanLukeminen();
    void testaaVarusteenLukeminen();
    void testaaKentanLukeminen();

private:
    joanka::SkriptiLukija lukija_;
};


#endif // TESTSKRIPTILUKIJA

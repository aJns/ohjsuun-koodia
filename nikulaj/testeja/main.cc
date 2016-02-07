#include <QTest>
#include <QGuiApplication>


/* #include "TestTaistelija.hh" */
#include "TestSkriptiLukija.hh"
/* #include "TestObjektiTehdas.hh" */
/* #include "TestLogiikka.hh" */


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    /* TestTaistelija taistelijaTesti; */
    TestSkriptiLukija skriptiTesti;
    /* TestObjektiTehdas tehdasTesti; */
    /* TestLogiikka logiikkaTesti; */


    /* QTest::qExec(&taistelijaTesti); */
    QTest::qExec(&skriptiTesti);
    /* QTest::qExec(&tehdasTesti); */
    /* QTest::qExec(&logiikkaTesti); */

    return 0;
}

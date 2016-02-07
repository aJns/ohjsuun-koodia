#ifndef TEKOALY_HH
#define TEKOALY_HH


// std
#include <string>

// Qt
#include <QList>
#include <QMap>

// joanka
#include "objekti.hh"
#include "Komento.hh"


namespace joanka {
    class Logiikka;
    typedef QMap<QString, Objekti*> pelaajaMappi;
    typedef QMap<Objekti*, int> toimintapisteMappi;
    typedef QMap<Objekti*, Objekti*> kohdeMappi;
    class Tekoaly {
    public:
        Tekoaly(Objekti* pelaaja);
        virtual ~Tekoaly();

        // Palauttaa tosi jos on vielä laskettavaa, ja false jos ei ole
        bool laskeSiirtoja(QList<Objekti*>& objektilista, pelaajaMappi& pelaajat,
                toimintapisteMappi& toimintapisteet, Logiikka& logiikka);
        // Poistaa kaikki viitteet tekoalyssa kyseiseen objektiin
        // Tulee kutsua kun objekti poistetaan logiikassa
        void poistaObjekti(Objekti* objekti);
        void alusta();

    private:
        Objekti* pelaaja_;
        QList<Objekti*> viholliset_;
        kohdeMappi kohteet_;

        int laskuri;
    };
} /* joanka  */ 


#endif // TEKOALY_HH

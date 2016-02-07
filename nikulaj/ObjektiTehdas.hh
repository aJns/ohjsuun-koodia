#ifndef OBJEKTITEHDAS_HH
#define OBJEKTITEHDAS_HH

// std
#include <memory>

// Qt
#include <QList>

// joanka
#include "objekti.hh"
#include "KenttaLaatta.hh"
#include "SkriptiLukija.hh"


namespace joanka {
    class ObjektiTehdas {
    public:
        static ObjektiTehdas& instanssi();

        Objekti* luoObjekti(ObjektiTyyppi tyyppi);
        QList<Objekti*> generoiKentta();

        struct KentanKoko {
            int leveys = 15;
            int korkeus = 8;
        };

        void asetaKentanKoko(int leveys, int korkeus);
        KentanKoko kentanKoko();

    private:
        ObjektiTehdas();
        ObjektiTehdas(ObjektiTehdas const&) = delete;
        void operator = (ObjektiTehdas const&) = delete;
        SkriptiLukija lukija_;
        Objekti* luoTaistelija(stringMap tiedot);
        Objekti* luoVaruste(stringMap tiedot);

        KentanKoko kentanKoko_;
    }; 
}


#endif // OBJEKTITEHDAS_HH

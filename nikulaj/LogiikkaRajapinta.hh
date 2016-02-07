#ifndef PELILOGIIKKARAJAPINTA_HH
#define PELILOGIIKKARAJAPINTA_HH


// Qt
#include <QList>
#include <QMap>
#include <QString>

// joanka
#include "objekti.hh"
#include "Vakiot.hh"


namespace joanka {
    const QString PELAAJA_TUNNUS = "pelaaja";
    const QString VASTUSTAJA_TUNNUS = "vastustaja";

    enum ViestiTaso {
        TURNAUKSEN_TILA,
        TAISTELUN_TILA,
        KRIITTINEN,
        NORMAALI
    };
    struct KayttoliittymaViesti {
        KayttoliittymaViesti();
        KayttoliittymaViesti(QString viesti, ViestiTaso taso=NORMAALI);
        QString viesti;
        ViestiTaso taso;
    };

    class LogiikkaRajapinta {
    public:
        virtual const QList<Objekti*>& peliObjektit() const = 0;
        virtual const QMap<QString, Objekti*>& pelaajat() const = 0;
        virtual NAKYMA nykyinenNakyma() const = 0;
        virtual bool objektiValittu() const = 0;
        virtual const Objekti* valittuObjekti() const = 0;
        virtual void lisaaUusiObjekti(ObjektiTyyppi tyyppi, QString omistaja="") = 0;
        virtual void vaihdaOmistajaa(Objekti* objekti, QString uusiOmistaja) = 0;
        virtual int voitetutTaistelut() const = 0;
        virtual int havitytTaistelut() const = 0;
        virtual int valuuttaaJaljella() const = 0;
        virtual void muutaValuuttaa(int maara) = 0;
        virtual KayttoliittymaViesti lueKayttoliittymaViesti() = 0;
    };
}


#endif // PELILOGIIKKARAJAPINTA_HH

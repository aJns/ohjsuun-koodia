#ifndef PELILOGIIKKA
#define PELILOGIIKKA


// std
#include <memory>

// Qt
#include <QList>
#include <QMap>
#include <QString>

// joanka
#include "objekti.hh"
#include "Taistelija.hh"
#include "LogiikkaRajapinta.hh"
#include "KayttoliittymaTapahtuma.hh"
#include "Tekoaly.hh"


namespace joanka {
    class Logiikka : public LogiikkaRajapinta {

    public :
        Logiikka();
        virtual ~Logiikka();

        // Logiikkafunktiot
        void paivitaLogiikka();
        int laskeToimintapisteet(KayttoliittymaTapahtuma& tapahtuma, Objekti* kohde);
        void lisaaKomento(Komento* komento);
        void poistaKuolleet();

        // Getterit 
        const QList<Objekti*>& peliObjektit() const override;
        bool objektiValittu() const override;
        const Objekti* valittuObjekti() const override;
        const QMap<QString, Objekti*>& pelaajat() const override;
        NAKYMA nykyinenNakyma() const override;
        int voitetutTaistelut() const override;
        int havitytTaistelut() const override;
        int valuuttaaJaljella() const override;
        void muutaValuuttaa(int maara) override;

        // Kayttoliittyman syotteet
        void lisaaKayttoliittymaTapahtuma(KayttoliittymaTapahtuma tapahtuma);
        void lisaaUusiObjekti(ObjektiTyyppi tyyppi, QString omistaja="") override;
        void vaihdaOmistajaa(Objekti* objekti, QString uusiOmistaja) override;

        // Kayttoliittyman viestintajarjestelma
        KayttoliittymaViesti lueKayttoliittymaViesti() override;
        static void lisaaKayttoliittymaViesti(QString viesti, ViestiTaso=NORMAALI);

    private:
        enum LogiikanTila {
            VALIKOSSA,
            MANAGEROINNISSA,
            PELAAJAVUORO,
            TIETOKONEVUORO,
            TAISTELUKESKEN,
            TAISTELULOPPU,
        };

        NAKYMA nykyinenNakyma_;
        LogiikanTila tila_;
        QList<Objekti*> objektiLista_;
        QList<Objekti*> kuolleetObjektit_;
        Objekti* valittuObjekti_;
        QList<KayttoliittymaTapahtuma> tapahtumaLista_;
        QList<Komento*> komentoLista_;
        QMap<QString, Objekti*> pelaajat_;
        QMap<Objekti*, int> toimintapisteet_;
        std::unique_ptr<Tekoaly> tekoaly_;
        static QList<KayttoliittymaViesti> kayttoliittymaViestit_;
        bool pelaajaVoitti_;
        int havitytTaistelut_;
        int voitetutTaistelut_;

        int valuuttaaJaljella_;

        void alustaVuoro();
        bool alustaTaistelu();
        void lopetaTaistelu();
        void alustaTurnaus();
        void kasitteleNakymaSiirtyma(KayttoliittymaTapahtuma tapahtuma);
        void tarkistaTaistelunTilanne();
        void kasitteleTapahtumat();
        void kasitteleKomennot();
        
        struct Koordinaatti {
            Koordinaatti(int x, int y) : x(x), y(y) {}
            int x;
            int y;
        };

        QList<Koordinaatti> kentanKoordinaatit_;
    };
}


#endif // PELILOGIIKKA


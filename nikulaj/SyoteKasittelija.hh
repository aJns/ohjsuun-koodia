#ifndef SYOTEKASITTELIJA_HH
#define SYOTEKASITTELIJA_HH

// std
#include <memory>

// Qt
#include <QGraphicsSceneMouseEvent>

// joanka
#include "SyoteKasittelijaRajapinta.hh"
#include "Logiikka.hh"
#include "objekti.hh"


namespace joanka {
    class SyoteKasittelija : public SyoteKasittelijaRajapinta {
    public:
        SyoteKasittelija(Logiikka* logiikka);

        void lisaaTapahtuma(QGraphicsSceneMouseEvent& tapahtuma) override;
        void poistaKuolleet() override;

        const int VALINTA_ETAISYYS = 1;

    public slots:
        void siirrymanagerointiin() override;
        void siirryasetuksiin() override;
        void siirryvalikkoon() override;
        void siirrypelinakymaan() override;

        void luovutavuoro() override;

    private:
        Logiikka* logiikka_;

        Objekti* laskeKohde(QPoint tapahtumapaikka, ObjektiTyyppi
                kohdetyyppi=TAISTELIJA);
    };
}


#endif // SYOTEKASITTELIJA_HH

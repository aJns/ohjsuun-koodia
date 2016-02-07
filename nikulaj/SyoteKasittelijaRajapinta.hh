#ifndef SYOTEKASITTELIJARAJAPINTA_HH
#define SYOTEKASITTELIJARAJAPINTA_HH

// Qt
#include <QGraphicsSceneMouseEvent>
#include <QObject>

#include "paaikkuna.hh"


namespace joanka {
    class SyoteKasittelijaRajapinta : public QObject {
        Q_OBJECT
    public:
        virtual void lisaaTapahtuma(QGraphicsSceneMouseEvent& tapahtuma) = 0;
        virtual void poistaKuolleet() = 0;

    public slots:
        virtual void siirrymanagerointiin() = 0;
        virtual void siirryasetuksiin() = 0;
        virtual void siirryvalikkoon() = 0;
        virtual void siirrypelinakymaan() = 0;

        virtual void luovutavuoro() = 0;
    };
}


#endif // SYOTEKASITTELIJARAJAPINTA_HH

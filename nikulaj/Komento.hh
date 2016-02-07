#ifndef KOMENTO_HH
#define KOMENTO_HH


// joanka
#include "objekti.hh"
#include "KayttoliittymaTapahtuma.hh"


namespace joanka {
    class Komento
    {
    public:
        Komento(Objekti* toteuttaja, Objekti* kohde, int x=0, int y=0);
        Komento(Objekti* toteuttaja, KayttoliittymaTapahtuma& kayttoliittymaTapahtuma);

        bool toteuta(QMap<Objekti*, int>& toimintapisteMappi);
        int toimintapisteHinta() const;
    
    private:
        Objekti* const toteuttaja_;
        Objekti* taistelijaKohde_;
        Objekti* kenttalaattaKohde_;
        const int x_;
        const int y_;
        int toimintapisteHinta_;
        bool jarkevaKomento_;

        int laskeToimintapisteHinta();
    };
} /* joanka  */ 


#endif // KOMENTO_HH

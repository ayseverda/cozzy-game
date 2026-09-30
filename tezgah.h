#pragma once

#include "raylib.h"
#include "oyun_verileri.h"

struct TezgahGorselleri
{
    Texture2D arkaPlan;
    Texture2D tavsanSiparis;
    Texture2D tavsanMutlu;
    Texture2D balon;
    Texture2D havucluKek;
    Texture2D capybara;
    Texture2D capybaraMutlu;
    Texture2D kirpi;
    Texture2D kirpiMutlu;
    Texture2D kurbaga;
    Texture2D kurbagaMutlu;
    Texture2D granola;
    Texture2D elmaliTurta;
    Texture2D sandvic;
};

struct TezgahDurumu
{
    int tavsanKaresi;
    float animasyonSayaci;
    float mesajSuresi;
};

void TezgahiBaslat(TezgahDurumu& tezgah);
void TezgahiGuncelle(TezgahDurumu& tezgah, OyunDurumu& oyun,
                     Sahne& aktifSahne, float gecenSure);
void TezgahiCiz(const TezgahDurumu& tezgah, const OyunDurumu& oyun,
                const TezgahGorselleri& gorseller, int buyutme);


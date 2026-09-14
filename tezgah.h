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


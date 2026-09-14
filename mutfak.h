#pragma once

#include "raylib.h"
#include "oyun_verileri.h"

struct MutfakGorselleri
{
    Texture2D arkaPlan;
    Texture2D bugday;
    Texture2D havuc;
    Texture2D cilek;
    Texture2D salatalik;
    Texture2D elma;
    Texture2D havucluKek;
};

struct MutfakDurumu
{
    int imlec;
    UrunTuru secim1;
    UrunTuru secim2;
    float parlamaSuresi;
    float mesajSuresi;
    bool tarifBasarili;
};

void MutfagiBaslat(MutfakDurumu& mutfak);
void MutfagiGuncelle(MutfakDurumu& mutfak, OyunDurumu& oyun,
                     Sahne& aktifSahne, float gecenSure);
void MutfagiCiz(const MutfakDurumu& mutfak, const OyunDurumu& oyun,
                const MutfakGorselleri& gorseller, int buyutme);


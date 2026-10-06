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
    Texture2D granola;
    Texture2D elmaliTurta;
    Texture2D sandvic;
    Texture2D secimCercevesi;
    Texture2D puff;
    Texture2D kareler;
};

struct MutfakDurumu
{
    int imlec;
    UrunTuru secim1;
    UrunTuru secim2;
    float parlamaSuresi;
    float mesajSuresi;
    bool tarifBasarili;
    YemekTuru hazirlananYemek;
};

void MutfagiBaslat(MutfakDurumu& mutfak);
void MutfagiGuncelle(MutfakDurumu& mutfak, OyunDurumu& oyun,
                     Sahne& aktifSahne, float gecenSure);
void MutfagiCiz(const MutfakDurumu& mutfak, const OyunDurumu& oyun,
                const MutfakGorselleri& gorseller, int buyutme);


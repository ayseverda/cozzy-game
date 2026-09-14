#pragma once

#include "raylib.h"
#include "oyun_verileri.h"

enum BitkiDurumu
{
    BOS,
    BUYUYOR,
    HAZIR
};

struct Bitki
{
    Vector2 konum;
    BitkiDurumu durum;
    float buyumeSuresi;
};

struct BahceGorselleri
{
    Texture2D arkaPlan;
    Texture2D karakterOn;
    Texture2D karakterArka;
    Texture2D karakterSag;
    Texture2D karakterSol;
    Texture2D bugday;
    Texture2D havuc;
    Texture2D cilek;
    Texture2D salatalik;
    Texture2D elma;
};

struct BahceDurumu
{
    Vector2 karakterKonumu;
    Yon yon;
    int animasyonKaresi;
    float animasyonSayaci;
    bool engelleriGoster;

    Bitki bugdaylar[6];
    Bitki havuclar[6];
    Bitki cilekler[6];
    Bitki salataliklar[6];
    Bitki elmalar[3];
};

void BahceyiBaslat(BahceDurumu& bahce);
void BahceyiGuncelle(BahceDurumu& bahce, OyunDurumu& oyun,
                     Sahne& aktifSahne, float gecenSure);
void BahceyiCiz(const BahceDurumu& bahce,
                const BahceGorselleri& gorseller, int buyutme);

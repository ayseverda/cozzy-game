#include "bahce.h"

namespace
{
constexpr float TAM_BUYUME_SURESI = 20.0f;
constexpr float KARAKTER_GENISLIGI = 48.0f;
constexpr float KARAKTER_YUKSEKLIGI = 64.0f;
constexpr int ENVANTER_KAPASITESI = 20;

const Rectangle ENGELLER[] = {
    {55, 36, 83, 61}, {184, 36, 84, 61},
    {55, 141, 83, 60}, {184, 141, 84, 60},
    {0, 224, 135, 8}, {185, 224, 135, 8}
};

bool EngelVarMi(Rectangle ayaklar)
{
    for (const Rectangle& engel : ENGELLER)
        if (CheckCollisionRecs(ayaklar, engel)) return true;
    return false;
}

void BitkileriGuncelle(Bitki* bitkiler, int sayi, float dt)
{
    for (int i = 0; i < sayi; ++i)
    {
        if (bitkiler[i].durum != BUYUYOR) continue;
        bitkiler[i].buyumeSuresi += dt;
        if (bitkiler[i].buyumeSuresi >= TAM_BUYUME_SURESI)
        {
            bitkiler[i].durum = HAZIR;
            bitkiler[i].buyumeSuresi = 0.0f;
        }
    }
}

bool BitkiTopla(Bitki* bitkiler, int sayi, bool elmaMi)
{
    for (int i = sayi - 1; i >= 0; --i)
    {
        if (bitkiler[i].durum != HAZIR) continue;
        bitkiler[i].durum = elmaMi ? BUYUYOR : BOS;
        bitkiler[i].buyumeSuresi = 0.0f;
        return true;
    }
    return false;
}

bool BitkiEk(Bitki* bitkiler, int sayi)
{
    for (int i = 0; i < sayi; ++i)
    {
        if (bitkiler[i].durum != BOS) continue;
        bitkiler[i].durum = BUYUYOR;
        bitkiler[i].buyumeSuresi = 0.0f;
        return true;
    }
    return false;
}

void FilizCiz(Vector2 konum, float sure, int s)
{
    const Color koyu = {45, 130, 66, 255};
    const Color acik = {91, 190, 92, 255};
    const Color tohum = {205, 151, 78, 255};
    int x = static_cast<int>(konum.x), y = static_cast<int>(konum.y);

    if (sure < 7.0f)
        DrawRectangle((x + 7) * s, (y + 12) * s, 2 * s, 2 * s, tohum);
    else if (sure < 14.0f)
    {
        DrawRectangle((x + 7) * s, (y + 8) * s, 2 * s, 6 * s, koyu);
        DrawRectangle((x + 4) * s, (y + 8) * s, 4 * s, 2 * s, acik);
        DrawRectangle((x + 9) * s, (y + 6) * s, 3 * s, 2 * s, acik);
    }
    else
    {
        DrawRectangle((x + 7) * s, (y + 5) * s, 2 * s, 9 * s, koyu);
        DrawRectangle((x + 3) * s, (y + 6) * s, 5 * s, 3 * s, acik);
        DrawRectangle((x + 9) * s, (y + 3) * s, 5 * s, 3 * s, acik);
    }
}

void UrunCiz(Texture2D doku, Vector2 konum, int s)
{
    DrawTextureEx(doku, {konum.x * s, konum.y * s}, 0, (float)s, WHITE);
}

void BitkileriCiz(Texture2D doku, const Bitki* bitkiler, int sayi, int s)
{
    for (int i = 0; i < sayi; ++i)
    {
        if (bitkiler[i].durum == HAZIR) UrunCiz(doku, bitkiler[i].konum, s);
        else if (bitkiler[i].durum == BUYUYOR)
            FilizCiz(bitkiler[i].konum, bitkiler[i].buyumeSuresi, s);
    }
}

void EkranaKapla(Texture2D doku, int s)
{
    DrawTexturePro(doku, {0, 0, (float)doku.width, (float)doku.height},
                   {0, 0, 320.0f * s, 240.0f * s}, {0, 0}, 0, WHITE);
}
}

void BahceyiBaslat(BahceDurumu& b)
{
    b.karakterKonumu = {136, 172};
    b.yon = ARKA;
    b.animasyonKaresi = 0;
    b.animasyonSayaci = 0;
    b.engelleriGoster = false;

    const Vector2 solUst[6] = {{64,48},{88,48},{112,48},{64,76},{88,76},{112,76}};
    const Vector2 sagUst[6] = {{192,48},{216,48},{240,48},{192,76},{216,76},{240,76}};
    const Vector2 solAlt[6] = {{64,151},{88,151},{112,151},{64,178},{88,178},{112,178}};
    const Vector2 sagAlt[6] = {{192,151},{216,151},{240,151},{192,178},{216,178},{240,178}};
    for (int i = 0; i < 6; ++i)
    {
        b.bugdaylar[i] = {solUst[i], HAZIR, 0};
        b.havuclar[i] = {sagUst[i], HAZIR, 0};
        b.cilekler[i] = {solAlt[i], HAZIR, 0};
        b.salataliklar[i] = {sagAlt[i], HAZIR, 0};
    }
    // 12x12 elmalarin tamami agacin yaprakli bolgesinde.
    b.elmalar[0] = {{9,7}, HAZIR, 0};
    b.elmalar[1] = {{38,12}, HAZIR, 0};
    b.elmalar[2] = {{21,27}, HAZIR, 0};
}

void BahceyiGuncelle(BahceDurumu& b, OyunDurumu& oyun,
                     Sahne& sahne, float dt)
{
    BitkileriGuncelle(b.bugdaylar, 6, dt);
    BitkileriGuncelle(b.havuclar, 6, dt);
    BitkileriGuncelle(b.cilekler, 6, dt);
    BitkileriGuncelle(b.salataliklar, 6, dt);
    BitkileriGuncelle(b.elmalar, 3, dt);

    Vector2 yeni = b.karakterKonumu;
    bool yuruyor = false;
    const float hiz = 55.0f;
    if (IsKeyDown(KEY_RIGHT)) { yeni.x += hiz*dt; b.yon=SAG; yuruyor=true; }
    else if (IsKeyDown(KEY_LEFT)) { yeni.x -= hiz*dt; b.yon=SOL; yuruyor=true; }
    else if (IsKeyDown(KEY_UP)) { yeni.y -= hiz*dt; b.yon=ARKA; yuruyor=true; }
    else if (IsKeyDown(KEY_DOWN)) { yeni.y += hiz*dt; b.yon=ON; yuruyor=true; }

    if (yeni.x < 0) yeni.x = 0;
    if (yeni.y < 0) yeni.y = 0;
    if (yeni.x > 320-KARAKTER_GENISLIGI) yeni.x=320-KARAKTER_GENISLIGI;
    if (yeni.y > 240-KARAKTER_YUKSEKLIGI) yeni.y=240-KARAKTER_YUKSEKLIGI;
    Rectangle ayaklar = {yeni.x+16, yeni.y+51, 16, 11};
    if (!EngelVarMi(ayaklar)) b.karakterKonumu = yeni;

    if (yuruyor)
    {
        b.animasyonSayaci += dt;
        if (b.animasyonSayaci >= 0.13f)
        {
            b.animasyonSayaci = 0;
            b.animasyonKaresi = (b.animasyonKaresi + 1) % 4;
        }
    }
    else { b.animasyonKaresi=0; b.animasyonSayaci=0; }

    Vector2 merkez = {b.karakterKonumu.x+24, b.karakterKonumu.y+57};

    // Bahcenin sag kenarindan mutfaga gec.
    if (IsKeyDown(KEY_RIGHT) && b.karakterKonumu.x >= 271.0f)
    {
        sahne = MUTFAK_SAHNESI;
        return;
    }

    if (merkez.x >= 135 && merkez.x <= 185 && b.karakterKonumu.y >= 169 && IsKeyDown(KEY_DOWN))
    {
        sahne = TEZGAH_SAHNESI;
        return;
    }
    UrunTuru tur = URUN_YOK;
    Bitki* alan = nullptr;
    int sayi = 0;
    if (CheckCollisionPointRec(merkez,{43,24,107,85})) { tur=BUGDAY; alan=b.bugdaylar; sayi=6; }
    else if (CheckCollisionPointRec(merkez,{172,24,108,85})) { tur=HAVUC; alan=b.havuclar; sayi=6; }
    else if (CheckCollisionPointRec(merkez,{43,129,107,84})) { tur=CILEK; alan=b.cilekler; sayi=6; }
    else if (CheckCollisionPointRec(merkez,{172,129,108,84})) { tur=SALATALIK; alan=b.salataliklar; sayi=6; }
    else if (CheckCollisionPointRec(merkez,{0,0,67,82})) { tur=ELMA; alan=b.elmalar; sayi=3; }

    if (tur != URUN_YOK && IsKeyPressed(KEY_E) && ToplamEnvanter(oyun) < ENVANTER_KAPASITESI)
    {
        if (BitkiTopla(alan, sayi, tur == ELMA))
        {
            if (tur==BUGDAY) oyun.bugday++;
            if (tur==HAVUC) oyun.havuc++;
            if (tur==CILEK) oyun.cilek++;
            if (tur==SALATALIK) oyun.salatalik++;
            if (tur==ELMA) oyun.elma++;
        }
    }
    if (tur != URUN_YOK && tur != ELMA && IsKeyPressed(KEY_F)) BitkiEk(alan, sayi);
    if (IsKeyPressed(KEY_F1)) b.engelleriGoster = !b.engelleriGoster;
}

void BahceyiCiz(const BahceDurumu& b, const BahceGorselleri& g, int s)
{
    EkranaKapla(g.arkaPlan, s);
    BitkileriCiz(g.bugday,b.bugdaylar,6,s);
    BitkileriCiz(g.havuc,b.havuclar,6,s);
    BitkileriCiz(g.cilek,b.cilekler,6,s);
    BitkileriCiz(g.salatalik,b.salataliklar,6,s);
    for (int i=0;i<3;++i)
    {
        if (b.elmalar[i].durum==HAZIR) UrunCiz(g.elma,b.elmalar[i].konum,s);
        else if (b.elmalar[i].durum==BUYUYOR && b.elmalar[i].buyumeSuresi>=14)
            DrawCircle((int)((b.elmalar[i].konum.x+6)*s),(int)((b.elmalar[i].konum.y+6)*s),2*s,{255,116,133,255});
    }

    Texture2D karakter = g.karakterOn;
    if (b.yon==ARKA) karakter=g.karakterArka;
    else if (b.yon==SAG) karakter=g.karakterSag;
    else if (b.yon==SOL) karakter=g.karakterSol;
    float kareW=karakter.width/4.0f;
    DrawTexturePro(karakter,{b.animasyonKaresi*kareW,0,kareW,(float)karakter.height},
                   {b.karakterKonumu.x*s,b.karakterKonumu.y*s,48.0f*s,64.0f*s},{0,0},0,WHITE);

    if (b.engelleriGoster)
        for (const Rectangle& r:ENGELLER) DrawRectangleLines((int)(r.x*s),(int)(r.y*s),(int)(r.width*s),(int)(r.height*s),RED);
}

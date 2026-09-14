#pragma once

enum Sahne
{
    BAHCE_SAHNESI,
    TEZGAH_SAHNESI,
    MUTFAK_SAHNESI
};

enum Yon
{
    ON,
    ARKA,
    SAG,
    SOL
};

enum UrunTuru
{
    URUN_YOK,
    BUGDAY,
    HAVUC,
    CILEK,
    SALATALIK,
    ELMA
};

struct OyunDurumu
{
    int bugday = 0;
    int havuc = 0;
    int cilek = 0;
    int salatalik = 0;
    int elma = 0;
    int havucluKek = 0;

    bool siparisAlindi = false;
    bool siparisTamamlandi = false;
    bool envanterAcik = false;
};

inline int ToplamEnvanter(const OyunDurumu& oyun)
{
    return oyun.bugday + oyun.havuc + oyun.cilek +
           oyun.salatalik + oyun.elma + oyun.havucluKek;
}


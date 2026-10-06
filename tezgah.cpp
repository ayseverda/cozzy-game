#include "tezgah.h"

namespace
{
YemekTuru SiparisYemegi(MusteriTuru m)
{
    if(m==CAPYBARA)return GRANOLA;
    if(m==KIRPI)return ELMALI_TURTA;
    if(m==KURBAGA)return SANDVIC;
    return HAVUCLU_KEK;
}

MusteriTuru SonrakiMusteri(MusteriTuru musteri)
{
    switch(musteri)
    {
        case TAVSAN: return CAPYBARA;
        case CAPYBARA: return KURBAGA;
        case KURBAGA: return KIRPI;
        case KIRPI: return TAVSAN;
    }
    return TAVSAN;
}

Texture2D YemekDokusu(const TezgahGorselleri& g,YemekTuru y)
{
    if(y==GRANOLA)return g.granola;
    if(y==ELMALI_TURTA)return g.elmaliTurta;
    if(y==SANDVIC)return g.sandvic;
    return g.havucluKek;
}

int YemekAdedi(const OyunDurumu& o,YemekTuru y)
{
    if(y==GRANOLA)return o.granola;
    if(y==ELMALI_TURTA)return o.elmaliTurta;
    if(y==SANDVIC)return o.sandvic;
    return o.havucluKek;
}

void YemegiAzalt(OyunDurumu& o,YemekTuru y)
{
    if(y==GRANOLA)o.granola--;
    else if(y==ELMALI_TURTA)o.elmaliTurta--;
    else if(y==SANDVIC)o.sandvic--;
    else o.havucluKek--;
}

const char* YemekAdi(YemekTuru y)
{
    if(y==GRANOLA)return "GRANOLA";
    if(y==ELMALI_TURTA)return "ELMALI TURTA";
    if(y==SANDVIC)return "SANDVIC";
    return "HAVUCLU KEK";
}

Texture2D MusteriDokusu(const TezgahGorselleri& g,MusteriTuru m,bool mutlu)
{
    if(m==CAPYBARA)return mutlu?g.capybaraMutlu:g.capybara;
    if(m==KIRPI)return mutlu?g.kirpiMutlu:g.kirpi;
    if(m==KURBAGA)return mutlu?g.kurbagaMutlu:g.kurbaga;
    return mutlu?g.tavsanMutlu:g.tavsanSiparis;
}

void DokuCiz(
    Texture2D doku,
    float x,
    float y,
    float genislik,
    float yukseklik,
    int buyutme
)
{
    Rectangle kaynak = {
        0,
        0,
        static_cast<float>(doku.width),
        static_cast<float>(doku.height)
    };

    Rectangle hedef = {
        x * buyutme,
        y * buyutme,
        genislik * buyutme,
        yukseklik * buyutme
    };

    DrawTexturePro(
        doku,
        kaynak,
        hedef,
        Vector2{0, 0},
        0,
        WHITE
    );
}

void PastaDolabiniCiz(
    const OyunDurumu& oyun,
    const TezgahGorselleri& gorseller,
    int buyutme
)
{
    // Şimdilik dolaba yalnızca hazırlanmış
    // havuçlu kek yerleştiriliyor.
    const Texture2D yemekler[4]={gorseller.havucluKek,gorseller.granola,
                                 gorseller.elmaliTurta,gorseller.sandvic};
    const int adetler[4]={oyun.havucluKek,oyun.granola,oyun.elmaliTurta,oyun.sandvic};
    const Vector2 yerler[4]={{252,97},{278,91},{252,127},{281,127}};
    for(int i=0;i<4;++i)
    {
        float boyut=(i==1)?30.0f:24.0f;
        if(adetler[i]>0)DokuCiz(yemekler[i],yerler[i].x,yerler[i].y,boyut,boyut,buyutme);
    }

    // Ürünlerin camın arkasında görünmesi
    // için hafif şeffaf cam katmanı.
    Color camRengi = {
        190,
        235,
        245,
        25
    };

    // Üst raf camı
    DrawRectangle(
        250 * buyutme,
        98 * buyutme,
        57 * buyutme,
        22 * buyutme,
        camRengi
    );

    // Alt raf camı
    DrawRectangle(
        250 * buyutme,
        122 * buyutme,
        57 * buyutme,
        31 * buyutme,
        camRengi
    );

    // Cam üzerindeki küçük parlama çizgileri
    DrawLine(
        253 * buyutme,
        100 * buyutme,
        271 * buyutme,
        100 * buyutme,
        Fade(WHITE, 0.45f)
    );

    DrawLine(
        253 * buyutme,
        124 * buyutme,
        267 * buyutme,
        124 * buyutme,
        Fade(WHITE, 0.35f)
    );
}
}

void TezgahiBaslat(
    TezgahDurumu& tezgah
)
{
    tezgah.tavsanKaresi = 0;
    tezgah.animasyonSayaci = 0.0f;
    tezgah.mesajSuresi = 0.0f;
}

void TezgahiGuncelle(
    TezgahDurumu& tezgah,
    OyunDurumu& oyun,
  Sahne& aktifSahne,
    float gecenSure
)
{
    // Tavşan animasyonu
    tezgah.animasyonSayaci += gecenSure;

    if (tezgah.animasyonSayaci >= 0.20f)
    {
        tezgah.animasyonSayaci = 0.0f;
        tezgah.tavsanKaresi++;
    }

    if (tezgah.mesajSuresi > 0.0f)
    {
        tezgah.mesajSuresi -= gecenSure;
    }
    if(oyun.siparisTamamlandi && tezgah.mesajSuresi<=0.0f)
    {
        oyun.musteri=SonrakiMusteri(oyun.musteri);
        oyun.siparisAlindi=false;
        oyun.siparisTamamlandi=false;
        tezgah.tavsanKaresi=0;
        tezgah.animasyonSayaci=0.0f;
    }

    // Tezgâhtan sola gidince bahçe
    if (IsKeyPressed(KEY_LEFT))
    {
        aktifSahne = BAHCE_SAHNESI;
        return;
    }

    // Tezgâhtan sağa gidince mutfak
    if (IsKeyPressed(KEY_RIGHT))
    {
        aktifSahne = MUTFAK_SAHNESI;
        return;
    }

    // Sipariş alma ve teslim etme
    if (IsKeyPressed(KEY_E))
    {
        // Tavşanın siparişini ilk kez al
        if (!oyun.siparisAlindi)
        {
            oyun.siparisAlindi = true;
            tezgah.mesajSuresi = 2.0f;
        }

        // Kek hazırsa tavşana teslim et
        else if (!oyun.siparisTamamlandi &&
                 YemekAdedi(oyun,SiparisYemegi(oyun.musteri))>0)
        {
            YemegiAzalt(oyun,SiparisYemegi(oyun.musteri));

            oyun.siparisTamamlandi = true;

            tezgah.mesajSuresi = 2.0f;
            tezgah.tavsanKaresi = 0;
            tezgah.animasyonSayaci = 0.0f;
        }

        // Sipariş var ama kek henüz hazırlanmamış
        else
        {
            tezgah.mesajSuresi = 2.0f;
        }
    }
}

void TezgahiCiz(
    const TezgahDurumu& tezgah,
    const OyunDurumu& oyun,
    const TezgahGorselleri& gorseller,
    int buyutme
)
{
    // ---------------------------------
    // TEZGÂH ARKA PLANI
    // ---------------------------------

    DokuCiz(
        gorseller.arkaPlan,
        0,
        0,
        320,
        240,
        buyutme
    );

    // Arka planın ardından ürünleri çiziyoruz.
    // Böylece ürünler dolabın içinde görünüyor.
    PastaDolabiniCiz(
        oyun,
        gorseller,
        buyutme
    );

    // ---------------------------------
    // TAVŞAN
    // ---------------------------------

    Texture2D aktifTavsan;

    aktifTavsan = MusteriDokusu(gorseller,oyun.musteri,oyun.siparisTamamlandi);

    int kareSayisi = aktifTavsan.width / 64;

    if (kareSayisi < 1)
    {
        kareSayisi = 1;
    }

    int aktifKare =
        tezgah.tavsanKaresi %
        kareSayisi;

    Rectangle tavsanKaynak = {static_cast<float>(aktifKare * 64),0,64,80};

    Rectangle tavsanHedef;
    if(oyun.musteri==CAPYBARA)
        tavsanHedef={107.0f*buyutme,22.0f*buyutme,106.0f*buyutme,132.0f*buyutme};
    else
        tavsanHedef={123.0f*buyutme,60.0f*buyutme,74.0f*buyutme,92.0f*buyutme};

    DrawTexturePro(
        aktifTavsan,
        tavsanKaynak,
        tavsanHedef,
        Vector2{0, 0},
        0,
        WHITE
    );
    // ---------------------------------
    // SİPARİŞ BALONU
    // ---------------------------------

    if (!oyun.siparisTamamlandi)
    {
        DokuCiz(
            gorseller.balon,
            163,
            38,
            100,
            66,
            buyutme
        );

        // Kek balonun görünen beyaz
        // bölümünün tam ortasında.
        bool granolaSiparisi=SiparisYemegi(oyun.musteri)==GRANOLA;
        float ikonBoyutu=granolaSiparisi?27.0f:22.0f;
        float ikonMerkezY=granolaSiparisi?57.0f:61.0f;
        DokuCiz(YemekDokusu(gorseller,SiparisYemegi(oyun.musteri)),
                213.0f-ikonBoyutu/2.0f,ikonMerkezY-ikonBoyutu/2.0f,
                ikonBoyutu,ikonBoyutu,buyutme);
    }

    // ---------------------------------
    // MESAJ KUTUSU
    // ---------------------------------

    if (tezgah.mesajSuresi > 0.0f)
    {
       const char* yazi = TextFormat("%s ISTIYOR!",YemekAdi(SiparisYemegi(oyun.musteri)));

        if (!oyun.siparisAlindi)
        {
            yazi = "E ILE SIPARISI AL";
        }
        else if (oyun.siparisTamamlandi)
        {
            yazi = "AFIYET OLSUN!";
        }
        else if (YemekAdedi(oyun,SiparisYemegi(oyun.musteri)) > 0)
        {
            yazi = TextFormat("E ILE VER: %s",YemekAdi(SiparisYemegi(oyun.musteri)));
        }

        DrawRectangle(
            76 * buyutme,
            203 * buyutme,
            168 * buyutme,
            22 * buyutme,
            Fade(BLACK, 0.72f)
        );

        DrawText(
            yazi,
            86 * buyutme,
            210 * buyutme,
            7 * buyutme,
            WHITE
        );
    }

    // ---------------------------------
    // ALT KONTROL YAZISI
    // ---------------------------------

    DrawText(
        "SOL: BAHCE   E: SIPARIS/TESLIM   SAG: MUTFAK   I: CANTA",
        12 * buyutme,
        230 * buyutme,
        5 * buyutme,
        DARKBROWN
    );
}

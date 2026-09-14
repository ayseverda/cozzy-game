#include "tezgah.h"

namespace
{
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
    if (oyun.havucluKek > 0)
    {
        DokuCiz(
            gorseller.havucluKek,
            268,
            109,
            20,
            20,
            buyutme
        );
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
        99 * buyutme,
        57 * buyutme,
        43 * buyutme,
        camRengi
    );

    // Alt raf camı
    DrawRectangle(
        250 * buyutme,
        149 * buyutme,
        57 * buyutme,
        29 * buyutme,
        camRengi
    );

    // Cam üzerindeki küçük parlama çizgileri
    DrawLine(
        254 * buyutme,
        102 * buyutme,
        271 * buyutme,
        102 * buyutme,
        Fade(WHITE, 0.45f)
    );

    DrawLine(
        254 * buyutme,
        152 * buyutme,
        267 * buyutme,
        152 * buyutme,
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
        else if (
            !oyun.siparisTamamlandi &&
            oyun.havucluKek > 0
        )
        {
            oyun.havucluKek--;

            oyun.siparisTamamlandi = true;

            tezgah.mesajSuresi = 3.0f;
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

    if (oyun.siparisTamamlandi)
    {
        aktifTavsan =
            gorseller.tavsanMutlu;
    }
    else
    {
        aktifTavsan =
            gorseller.tavsanSiparis;
    }

    int kareSayisi =
        aktifTavsan.width / 64;

    if (kareSayisi < 1)
    {
        kareSayisi = 1;
    }

    int aktifKare =
        tezgah.tavsanKaresi %
        kareSayisi;

    Rectangle tavsanKaynak = {
        static_cast<float>(
            aktifKare * 64
        ),
        0,
        64,
        80
    };

    Rectangle tavsanHedef = {
        128.0f * buyutme,
        72.0f * buyutme,
        64.0f * buyutme,
        80.0f * buyutme
    };

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
            194,
            18,
            96,
            64,
            buyutme
        );

        // Kek balonun görünen beyaz
        // bölümünün tam ortasında.
        DokuCiz(
            gorseller.havucluKek,
            231,
            30,
            20,
            20,
            buyutme
        );
    }

    // ---------------------------------
    // MESAJ KUTUSU
    // ---------------------------------

    if (tezgah.mesajSuresi > 0.0f)
    {
       const char* yazi =
            "HAVUCLU KEKI GETIR!";

        if (!oyun.siparisAlindi)
        {
            yazi = "E ILE SIPARISI AL";
        }
        else if (oyun.siparisTamamlandi)
        {
            yazi = "TESKKURLER! COK GUZEL!";
        }
        else if (oyun.havucluKek > 0)
        {
            yazi = "E ILE KEKI VER";
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
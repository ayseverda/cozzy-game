#include "mutfak.h"
#include <cmath>

namespace
{
const UrunTuru URUNLER[5] = {BUGDAY,HAVUC,CILEK,SALATALIK,ELMA};

int Adet(const OyunDurumu& o, UrunTuru u)
{
    if(u==BUGDAY)return o.bugday; if(u==HAVUC)return o.havuc;
    if(u==CILEK)return o.cilek; if(u==SALATALIK)return o.salatalik;
    if(u==ELMA)return o.elma; return 0;
}
void Azalt(OyunDurumu& o,UrunTuru u)
{
    if(u==BUGDAY)o.bugday--; else if(u==HAVUC)o.havuc--;
    else if(u==CILEK)o.cilek--; else if(u==SALATALIK)o.salatalik--;
    else if(u==ELMA)o.elma--;
}
Texture2D Doku(const MutfakGorselleri& g,UrunTuru u)
{
    if(u==BUGDAY)return g.bugday; if(u==HAVUC)return g.havuc;
    if(u==CILEK)return g.cilek; if(u==SALATALIK)return g.salatalik;
    return g.elma;
}
Texture2D YemekDokusu(const MutfakGorselleri& g,YemekTuru y)
{
    if(y==GRANOLA)return g.granola;
    if(y==ELMALI_TURTA)return g.elmaliTurta;
    if(y==SANDVIC)return g.sandvic;
    return g.havucluKek;
}
bool Eslesiyor(UrunTuru a,UrunTuru b,UrunTuru x,UrunTuru y)
{
    return (a==x&&b==y)||(a==y&&b==x);
}
void DokuCiz(Texture2D t,float x,float y,float w,float h,int s,Color renk=WHITE)
{
    DrawTexturePro(t,{0,0,(float)t.width,(float)t.height},{x*s,y*s,w*s,h*s},{0,0},0,renk);
}
}

void MutfagiBaslat(MutfakDurumu& m)
{
    m.imlec=0; m.secim1=URUN_YOK; m.secim2=URUN_YOK;
    m.parlamaSuresi=0; m.mesajSuresi=0; m.tarifBasarili=false;
    m.hazirlananYemek=HAVUCLU_KEK;
}

void MutfagiGuncelle(MutfakDurumu& m,OyunDurumu& o,Sahne& sahne,float dt)
{
    if(m.parlamaSuresi>0)m.parlamaSuresi-=dt;
    if(m.mesajSuresi>0)m.mesajSuresi-=dt;
    if(IsKeyPressed(KEY_B)){sahne=TEZGAH_SAHNESI;return;}
    // Ilk malzemedeyken sola basmak mutfaktan cikip tezgaha goturur.
    if(IsKeyPressed(KEY_LEFT))
    {
        if(m.imlec==0){sahne=TEZGAH_SAHNESI;return;}
        m.imlec--;
    }
    if(IsKeyPressed(KEY_RIGHT))m.imlec=(m.imlec+1)%5;
    if(IsKeyPressed(KEY_Q)){m.secim1=URUN_YOK;m.secim2=URUN_YOK;}

    if(IsKeyPressed(KEY_E))
    {
        UrunTuru u=URUNLER[m.imlec];
        if(Adet(o,u)>0 && u!=m.secim1 && u!=m.secim2)
        {
            if(m.secim1==URUN_YOK)m.secim1=u;
            else if(m.secim2==URUN_YOK)m.secim2=u;
        }
    }
    if(IsKeyPressed(KEY_SPACE) && m.secim1!=URUN_YOK && m.secim2!=URUN_YOK)
    {
        bool tarif=false;
        YemekTuru yemek=HAVUCLU_KEK;
        if(Eslesiyor(m.secim1,m.secim2,BUGDAY,HAVUC))
        { tarif=true; yemek=HAVUCLU_KEK; }
        else if(Eslesiyor(m.secim1,m.secim2,BUGDAY,CILEK))
        { tarif=true; yemek=GRANOLA; }
        else if(Eslesiyor(m.secim1,m.secim2,BUGDAY,ELMA))
        { tarif=true; yemek=ELMALI_TURTA; }
        else if(Eslesiyor(m.secim1,m.secim2,BUGDAY,SALATALIK))
        { tarif=true; yemek=SANDVIC; }
        m.tarifBasarili=tarif; m.mesajSuresi=2.0f;
        m.hazirlananYemek=yemek;
        if(tarif)
        {
            Azalt(o,m.secim1); Azalt(o,m.secim2);
            if(yemek==HAVUCLU_KEK)o.havucluKek++;
            else if(yemek==GRANOLA)o.granola++;
            else if(yemek==ELMALI_TURTA)o.elmaliTurta++;
            else o.sandvic++;
            m.parlamaSuresi=2.4f;
        }
        m.secim1=URUN_YOK; m.secim2=URUN_YOK;
    }
}

void MutfagiCiz(const MutfakDurumu& m,const OyunDurumu& o,const MutfakGorselleri& g,int s)
{
    DokuCiz(g.arkaPlan,0,0,320,240,s);
    // Her urunun masanin ustunde kendine ait, hafif daginik konumu.
    // Kase ortada bos birakiliyor.
    const Vector2 yerler[5] = {
        {84, 144},   // bugday: sol arka
        {108, 156},  // havuc: sol orta
        {204, 140},  // cilek: sag arka
        {228, 155},  // salatalik: sag orta
        {199, 164}   // elma: kasenin sag-alt tarafi
    };
    for(int i=0;i<5;++i)
    {
        UrunTuru u=URUNLER[i];
        Color renk=Adet(o,u)>0?WHITE:Fade(GRAY,0.55f);
        const float x=yerler[i].x;
        const float y=yerler[i].y;
        Texture2D doku=Doku(g,u);
        // Masada kucuk gorunsunlar; adetler envanter ekraninda gorulecek.
        float boyut=(u==ELMA)?18.0f:20.0f;
        DokuCiz(doku,x,y,boyut,boyut,s,renk);
    }
    // Elle cizilen koseler yerine yeni piksel cerceve dokusu.
    DokuCiz(g.secimCercevesi,yerler[m.imlec].x-6,yerler[m.imlec].y-6,32,32,s);

    // kare.png atlasinin 5. kare cercevesi ve 2. arti hucreleri.
    const Rectangle kareCercevesi={132,100,66,66};
    const Rectangle artiIsareti={102,184,52,50};
    DrawTexturePro(g.kareler,kareCercevesi,{119*s,6*s,32*s,32*s},{0,0},0,WHITE);
    DrawTexturePro(g.kareler,kareCercevesi,{169*s,6*s,32*s,32*s},{0,0},0,WHITE);
    DrawTexturePro(g.kareler,artiIsareti,{154*s,15*s,11*s,11*s},{0,0},0,WHITE);
    if(m.secim1!=URUN_YOK)DokuCiz(Doku(g,m.secim1),127,14,16,16,s);
    if(m.secim2!=URUN_YOK)DokuCiz(Doku(g,m.secim2),177,14,16,16,s);

    if(m.parlamaSuresi>0)
    {
        // Sprite sayfasindaki butun efekt kareleri hizlica, sabit boyutta oynar.
        const float animasyonToplami=2.4f;
        float gecenSure=animasyonToplami-m.parlamaSuresi;
        if(gecenSure<0.68f)
        {
            const Rectangle kareler[]={
                {0,0,80,84},       // ilk pariltilar
                {176,0,80,88},     // donen yildizlar
                {0,88,82,80},      // pembe girdap
                {88,88,82,80},     // isik halkasi
                {184,96,70,62},    // ilk puff
                {96,176,68,70}     // son puff
            };
            const float kareSuresi=0.68f/6.0f;
            int kareNo=(int)(gecenSure/kareSuresi);
            if(kareNo>5)kareNo=5;
            const float boyut=48.0f;
            Rectangle hedef={(160-boyut/2.0f)*s,(154-boyut/2.0f)*s,boyut*s,boyut*s};
            DrawTexturePro(g.puff,kareler[kareNo],hedef,{0,0},0,WHITE);
        }
        else
        {
            float yemekBoyutu=m.hazirlananYemek==GRANOLA?32.0f:24.0f;
            DokuCiz(YemekDokusu(g,m.hazirlananYemek),
                    160-yemekBoyutu/2.0f,157-yemekBoyutu/2.0f,
                    yemekBoyutu,yemekBoyutu,s);
        }
    }
    if(m.mesajSuresi>0)
    {
        DrawRectangle(93*s,202*s,134*s,23*s,Fade(BLACK,0.72f));
        const char* sonuc="TARIF HAZIR!";
        if(m.tarifBasarili && m.hazirlananYemek==HAVUCLU_KEK)sonuc="HAVUCLU KEK HAZIR!";
        else if(m.tarifBasarili && m.hazirlananYemek==GRANOLA)sonuc="GRANOLA HAZIR!";
        else if(m.tarifBasarili && m.hazirlananYemek==ELMALI_TURTA)sonuc="ELMALI TURTA HAZIR!";
        else if(m.tarifBasarili)sonuc="SANDVIC HAZIR!";
        else sonuc="BU TARIF OLMADI";
        DrawText(sonuc,105*s,209*s,7*s,WHITE);
    }

}

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
void DokuCiz(Texture2D t,float x,float y,float w,float h,int s,Color renk=WHITE)
{
    DrawTexturePro(t,{0,0,(float)t.width,(float)t.height},{x*s,y*s,w*s,h*s},{0,0},0,renk);
}
}

void MutfagiBaslat(MutfakDurumu& m)
{
    m.imlec=0; m.secim1=URUN_YOK; m.secim2=URUN_YOK;
    m.parlamaSuresi=0; m.mesajSuresi=0; m.tarifBasarili=false;
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
        bool kek=(m.secim1==BUGDAY&&m.secim2==HAVUC)||(m.secim1==HAVUC&&m.secim2==BUGDAY);
        m.tarifBasarili=kek; m.mesajSuresi=2.0f;
        if(kek)
        {
            Azalt(o,m.secim1); Azalt(o,m.secim2); o.havucluKek++;
            m.parlamaSuresi=1.0f;
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
    // Urunu kapatmayan, yumusak yanip sonen piksel-kose secim cercevesi.
    float imlecBoyutu=(URUNLER[m.imlec]==ELMA)?14.0f:16.0f;
    int sx=(int)((yerler[m.imlec].x-4)*s);
    int sy=(int)((yerler[m.imlec].y-4)*s);
    int sw=(int)((imlecBoyutu+8)*s);
    int kose=5*s;
    int kalinlik=2*s;
    float nabiz=0.65f+0.35f*(sinf((float)GetTime()*6.0f)+1.0f)/2.0f;
    Color secimRengi=Fade(Color{255,92,151,255},nabiz);
    DrawRectangle(sx,sy,kose,kalinlik,secimRengi);
    DrawRectangle(sx,sy,kalinlik,kose,secimRengi);
    DrawRectangle(sx+sw-kose,sy,kose,kalinlik,secimRengi);
    DrawRectangle(sx+sw-kalinlik,sy,kalinlik,kose,secimRengi);
    DrawRectangle(sx,sy+sw-kalinlik,kose,kalinlik,secimRengi);
    DrawRectangle(sx,sy+sw-kose,kalinlik,kose,secimRengi);
    DrawRectangle(sx+sw-kose,sy+sw-kalinlik,kose,kalinlik,secimRengi);
    DrawRectangle(sx+sw-kalinlik,sy+sw-kose,kalinlik,kose,secimRengi);

    // Secilen iki urun icin buyuk gri cubuk yerine iki kucuk pastel kutu.
    Color kutu={255,247,225,235};
    Color cerceve={105,75,62,255};
    DrawRectangle(118*s,8*s,34*s,28*s,kutu);
    DrawRectangleLines(118*s,8*s,34*s,28*s,cerceve);
    DrawRectangle(168*s,8*s,34*s,28*s,kutu);
    DrawRectangleLines(168*s,8*s,34*s,28*s,cerceve);
    if(m.secim1!=URUN_YOK)DokuCiz(Doku(g,m.secim1),127,14,16,16,s);
    DrawText("+",157*s,16*s,9*s,cerceve);
    if(m.secim2!=URUN_YOK)DokuCiz(Doku(g,m.secim2),177,14,16,16,s);

    if(m.parlamaSuresi>0)
    {
        float a=(sinf(GetTime()*25)+1)*0.5f;
        // Parlama ve sonuc kasenin tam uzerinde belirir.
        DrawCircle(160*s,165*s,(18+8*a)*s,Fade(WHITE,0.75f));
        DokuCiz(g.havucluKek,148,151,24,24,s);
    }
    if(m.mesajSuresi>0)
    {
        DrawRectangle(93*s,202*s,134*s,23*s,Fade(BLACK,0.72f));
        DrawText(m.tarifBasarili?"HAVUCLU KEK HAZIR!":"BU TARIF OLMADI",105*s,209*s,7*s,WHITE);
    }
    DrawText("SAG/SOL: SEC  E: EKLE  SPACE: YAP  Q: TEMIZLE",22*s,224*s,5*s,DARKBROWN);
    DrawText("ILK MALZEMEDE SOL: TEZGAHA DON",69*s,232*s,5*s,DARKBROWN);
}

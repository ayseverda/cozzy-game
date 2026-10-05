#include "raylib.h"
#include "bahce.h"
#include "tezgah.h"
#include "mutfak.h"

namespace
{
struct OyunSesleri
{
    Sound meyveToplama;
    Sound yemekHazir;
    Sound musteriMutlu;
    Sound sayfaCevirme;
};

void SesleriYukle(OyunSesleri& s)
{
    s.meyveToplama=LoadSound("assets/sesler/pop_hizli.wav");
    s.yemekHazir=LoadSound("assets/sesler/yemek.wav");
    s.musteriMutlu=LoadSound("assets/sesler/hihi_yeni.wav");
    s.sayfaCevirme=LoadSound("assets/sesler/page.wav");

    TraceLog(LOG_INFO,"Ses dosyalari: toplama=%s, yemek=%s, teslim=%s",
             IsSoundValid(s.meyveToplama)?"OK":"YUKLENEMEDI",
             IsSoundValid(s.yemekHazir)?"OK":"YUKLENEMEDI",
             IsSoundValid(s.musteriMutlu)?"OK":"YUKLENEMEDI");
}

void SesleriKapat(OyunSesleri& s)
{
    UnloadSound(s.meyveToplama);
    UnloadSound(s.yemekHazir);
    UnloadSound(s.musteriMutlu);
    UnloadSound(s.sayfaCevirme);
}

int HazirYemekSayisi(const OyunDurumu& o)
{
    return o.havucluKek + o.granola + o.elmaliTurta + o.sandvic;
}

void Filtrele(Texture2D& t){SetTextureFilter(t,TEXTURE_FILTER_POINT);}
void DokuCiz(Texture2D t,float x,float y,float w,float h,int s)
{
    DrawTexturePro(t,{0,0,(float)t.width,(float)t.height},{x*s,y*s,w*s,h*s},{0,0},0,WHITE);
}

void FotoCercevesiCiz(Texture2D foto,float x,float y,float w,float h,int s)
{
    DrawRectangle((int)((x+2)*s),(int)((y+2)*s),(int)(w*s),(int)(h*s),Fade(DARKBROWN,0.45f));
    DrawRectangle((int)(x*s),(int)(y*s),(int)(w*s),(int)(h*s),RAYWHITE);

    float oranHedef=w/h;
    float oranFoto=(float)foto.width/foto.height;
    Rectangle kaynak={0,0,(float)foto.width,(float)foto.height};
    if(oranFoto>oranHedef)
    {
        kaynak.width=foto.height*oranHedef;
        kaynak.x=(foto.width-kaynak.width)/2.0f;
    }
    else
    {
        kaynak.height=foto.width/oranHedef;
        kaynak.y=(foto.height-kaynak.height)/2.0f;
    }
    Rectangle hedef={(x+2)*s,(y+2)*s,(w-4)*s,(h-4)*s};
    DrawTexturePro(foto,kaynak,hedef,{0,0},0,WHITE);
}

void KitapOklariniCiz(int s,bool oncekiVar,bool sonrakiVar)
{
    Color solRenk=oncekiVar?DARKBROWN:Fade(DARKBROWN,0.18f);
    Color sagRenk=sonrakiVar?DARKBROWN:Fade(DARKBROWN,0.18f);
    DrawTriangle({39.0f*s,120.0f*s},{51.0f*s,111.0f*s},{51.0f*s,129.0f*s},solRenk);
    DrawTriangle({281.0f*s,120.0f*s},{269.0f*s,111.0f*s},{269.0f*s,129.0f*s},sagRenk);
}

int MusteriAniSayfasi(MusteriTuru musteri)
{
    if(musteri==TAVSAN)return 1;
    if(musteri==CAPYBARA)return 2;
    if(musteri==KIRPI)return 3;
    return 4;
}

void PuanGoster(Texture2D rakamlar,int puan,int s)
{
    int rakamDizisi[10];
    int rakamSayisi=0;
    if(puan==0)rakamDizisi[rakamSayisi++]=0;
    else
    {
        int kalan=puan;
        while(kalan>0 && rakamSayisi<10)
        {
            rakamDizisi[rakamSayisi++]=kalan%10;
            kalan/=10;
        }
    }
    for(int i=rakamSayisi-1,konum=0;i>=0;--i,++konum)
    {
        int x=(8+konum*8)*s;
        int digit=rakamDizisi[i];
        Rectangle kaynak;
        if(digit<8)kaynak={(float)(digit*16),0,16,16};
        else kaynak={(float)((digit-8)*16),16,16,16};
        Rectangle hedef={(float)x,12.0f*s,8.0f*s,12.0f*s};
        DrawTexturePro(rakamlar,kaynak,hedef,{0,0},0,WHITE);
    }
}

void TarifKitabiCiz(Texture2D acikKitap,Font font,const MutfakGorselleri& g,
                   int sayfa,int s)
{
    const char* adlar[4]={"HAVUCLU KEK","GRANOLA","ELMALI TURTA","SANDVIC"};
    const char* malzemeler[4]={"BUGDAY + HAVUC","BUGDAY + CILEK",
                               "BUGDAY + ELMA","BUGDAY + SALATALIK"};
    Texture2D malzeme2[4]={g.havuc,g.cilek,g.elma,g.salatalik};
    Texture2D yemekler[4]={g.havucluKek,g.granola,g.elmaliTurta,g.sandvic};
    DrawRectangle(0,0,320*s,240*s,Fade(BLACK,0.58f));
    DokuCiz(acikKitap,32,40,256,160,s);

    const char* tarif=adlar[sayfa];
    Vector2 tarifBoyut=MeasureTextEx(font,tarif,12*s,1.0f*s);
    DrawTextEx(font,tarif,{110.0f*s-tarifBoyut.x/2.0f,70.0f*s},12*s,1.0f*s,DARKBROWN);
    const char* malzemeBasligi="MALZEMELER";
    Vector2 malzemeBaslikBoyut=MeasureTextEx(font,malzemeBasligi,8*s,1.0f*s);
    DrawTextEx(font,malzemeBasligi,{110.0f*s-malzemeBaslikBoyut.x/2.0f,96.0f*s},8*s,1.0f*s,DARKBROWN);
    DokuCiz(g.bugday,84,113,22,22,s);
    DrawTextEx(font,"+",{118.0f*s,118.0f*s},11*s,1.0f*s,DARKBROWN);
    DokuCiz(malzeme2[sayfa],135,113,22,22,s);
    const char* malzemeAdlari[4]={"HAVUC","CILEK","ELMA","SALATALIK"};
    const char* tekIlkMalzeme="BUGDAY";
    Vector2 ilkAdBoyut=MeasureTextEx(font,tekIlkMalzeme,6*s,0.5f*s);
    Vector2 ikinciAdBoyut=MeasureTextEx(font,malzemeAdlari[sayfa],6*s,0.5f*s);
    DrawTextEx(font,tekIlkMalzeme,{(96.0f*s-ilkAdBoyut.x/2.0f),142.0f*s},6*s,0.5f*s,DARKBROWN);
    DrawTextEx(font,malzemeAdlari[sayfa],{(146.0f*s-ikinciAdBoyut.x/2.0f),142.0f*s},6*s,0.5f*s,DARKBROWN);

    const char* sonucBasligi="SONUC";
    Vector2 sonucBaslikBoyut=MeasureTextEx(font,sonucBasligi,8*s,1.0f*s);
    DrawTextEx(font,sonucBasligi,{208.0f*s-sonucBaslikBoyut.x/2.0f,88.0f*s},8*s,1.0f*s,DARKBROWN);
    const float yemekBoyutu=sayfa==1?48.0f:34.0f;
    const float yemekY=sayfa==1?92.0f:105.0f;
    DokuCiz(yemekler[sayfa],208.0f-yemekBoyutu/2.0f,yemekY,
            yemekBoyutu,yemekBoyutu,s);
    const char* afiyet="AFIYET OLSUN!";
    Vector2 afiyetBoyut=MeasureTextEx(font,afiyet,6*s,1.0f*s);
    DrawTextEx(font,afiyet,{208.0f*s-afiyetBoyut.x/2.0f,145.0f*s},6*s,1.0f*s,DARKBROWN);
    KitapOklariniCiz(s,sayfa>0,sayfa<3);
    DrawTextEx(font,"J / ESC: KAPAT",{107.0f*s,195.0f*s},7*s,1.0f*s,DARKBROWN);
}

void KilitAcmaBildirimiCiz(Texture2D bildirim,Font font,float kalanSure,int s)
{
    const float girisSuresi=0.38f;
    const float tutmaSuresi=1.75f;
    const float cikisSuresi=0.47f;
    const float toplamSure=girisSuresi+tutmaSuresi+cikisSuresi;
    const float gecerSure=toplamSure-kalanSure;
    const float baslangicY=-150.0f;
    const float durmaY=-24.0f;
    const float goruntuW=190.0f;
    const float goruntuH=142.5f;
    float y=durmaY;
    if(gecerSure<girisSuresi)
    {
        float t=gecerSure/girisSuresi;
        float yumusat=1.0f-(1.0f-t)*(1.0f-t)*(1.0f-t);
        y=baslangicY+(durmaY-baslangicY)*yumusat;
    }
    else if(gecerSure>girisSuresi+tutmaSuresi)
    {
        float t=(gecerSure-girisSuresi-tutmaSuresi)/cikisSuresi;
        y=durmaY+(baslangicY-durmaY)*t*t*t;
    }

    float x=(320.0f-goruntuW)/2.0f;
    DokuCiz(bildirim,x,y,goruntuW,goruntuH,s);
    const char* mesaj="Yeni bir sayfaniz var!";
    const float boyut=36.0f;
    Vector2 yaziBoyutu=MeasureTextEx(font,mesaj,boyut,1.0f);
    Vector2 konum={(320.0f*s-yaziBoyutu.x)/2.0f,
                   (y+goruntuH*109.0f/240.0f)*s};
    DrawTextEx(font,mesaj,{konum.x+0.8f,konum.y},boyut,1.0f,DARKBROWN);
    DrawTextEx(font,mesaj,konum,boyut,1.0f,DARKBROWN);
}

void EnvanteriCiz(const OyunDurumu& o,Texture2D panel,
                  Texture2D bugday,Texture2D havuc,Texture2D cilek,
                  Texture2D salatalik,Texture2D elma,Texture2D kek,
                  Texture2D granola,Texture2D turta,Texture2D sandvic,int s)
{
    DokuCiz(panel,0,0,320,240,s);
    Texture2D dokular[9]={bugday,havuc,cilek,salatalik,elma,kek,granola,turta,sandvic};
    int adetler[9]={o.bugday,o.havuc,o.cilek,o.salatalik,o.elma,o.havucluKek,
                    o.granola,o.elmaliTurta,o.sandvic};
    // PNG icindeki gercek slot merkezleri.
    const float merkezX[5]={79,119,159,199,239};
    for(int i=0;i<9;++i)
    {
        float cx=i<5?merkezX[i]:merkezX[i-5];
        float cy=i<5?99.0f:131.0f;
        float w=(i==6)?21.0f:((i>=5)?18.0f:(float)dokular[i].width);
        float h=(i==6)?21.0f:((i>=5)?18.0f:(float)dokular[i].height);
        float x=cx-w/2.0f, y=cy-h/2.0f;
        DokuCiz(dokular[i],x,y,w,h,s);
        DrawText(TextFormat("%d",adetler[i]),
                 (int)((cx+5)*s),(int)((cy+5)*s),5*s,DARKBROWN);
    }
    DrawText(TextFormat("CANTA %d/20",ToplamEnvanter(o)),137*s,171*s,6*s,DARKBROWN);
    DrawText("I: KAPAT",139*s,181*s,6*s,DARKBROWN);
}
}

int main()
{
    const int S=3;
    InitWindow(320*S,240*S,"Cozzy");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); // ESC kitap penceresini kapatsin; oyunu sonlandirmasin.

    InitAudioDevice();
    OyunSesleri sesler={0};
    const bool sesHazir=IsAudioDeviceReady();
    if(sesHazir)
    {
        SetMasterVolume(0.55f);
        SesleriYukle(sesler);
    }

    BahceGorselleri bg;
    bg.arkaPlan=LoadTexture("assets/bahce.png");
    bg.karakterOn=LoadTexture("assets/karakter_on.png");
    bg.karakterArka=LoadTexture("assets/karakter_arka.png");
    bg.karakterSag=LoadTexture("assets/karakter_sag.png");
    bg.karakterSol=LoadTexture("assets/karakter_sol.png");
    bg.bugday=LoadTexture("assets/bugday.png");
    bg.havuc=LoadTexture("assets/carrot.png");
    bg.cilek=LoadTexture("assets/cilek.png");
    bg.salatalik=LoadTexture("assets/cucumber.png");
    bg.elma=LoadTexture("assets/apple.png");

    TezgahGorselleri tg;
    tg.arkaPlan=LoadTexture("assets/tezgah.png");
    tg.tavsanSiparis=LoadTexture("assets/bunny_siparis.png");
    tg.tavsanMutlu=LoadTexture("assets/bunny_happy.png");
    tg.balon=LoadTexture("assets/balon.png");
    tg.havucluKek=LoadTexture("assets/carrot_cake.png");
    tg.capybara=LoadTexture("assets/capybara_siparis.png");
    tg.capybaraMutlu=LoadTexture("assets/capybara_happy.png");
    tg.kirpi=LoadTexture("assets/kirpi_siparis.png");
    tg.kirpiMutlu=LoadTexture("assets/kirpi_happy.png");
    tg.kurbaga=LoadTexture("assets/kurba_siparis.png");
    tg.kurbagaMutlu=LoadTexture("assets/kurba_happy.png");
    tg.granola=LoadTexture("assets/granola.png");
    tg.elmaliTurta=LoadTexture("assets/turta.png");
    tg.sandvic=LoadTexture("assets/salatalik_sandivic.png");

    MutfakGorselleri mg;
    mg.arkaPlan=LoadTexture("assets/mutfak.png");
    mg.bugday=bg.bugday; mg.havuc=bg.havuc; mg.cilek=bg.cilek;
    mg.salatalik=bg.salatalik; mg.elma=bg.elma; mg.havucluKek=tg.havucluKek;
    mg.granola=tg.granola; mg.elmaliTurta=tg.elmaliTurta; mg.sandvic=tg.sandvic;
    Texture2D envanter=LoadTexture("assets/envanter.png");
    Texture2D kitapTezgah=LoadTexture("assets/kitap_tezgah.png");
    Texture2D acikKitap=LoadTexture("assets/open_book.png");






    Texture2D rakamlar=LoadTexture("assets/Numbers.png");
    SetTextureFilter(rakamlar,TEXTURE_FILTER_POINT);
    Texture2D bildirimGorseli=LoadTexture("assets/Bildirim.png");
    SetTextureFilter(bildirimGorseli,TEXTURE_FILTER_POINT);
    const char* bildirimMesaji="Yeni bir sayfaniz var!";
    int bildirimKodSayisi=0;
    Font bildirimFontu=LoadFontEx("assets/Patrick_Hand/PatrickHand-Regular.ttf",48,nullptr,0);
    SetTextureFilter(bildirimFontu.texture,TEXTURE_FILTER_BILINEAR);
    const char* defterFotoYollari[10]={
        "assets/bahce-karakter.png","assets/karakter-mutfak.png",
        "assets/bahce-tavsan.png","assets/selfie-bunny.png",
        "assets/bahce-capybara.png","assets/selfie-capy.png",
        "assets/bahce-kirpi.png","assets/selfie-kirpi.png",
        "assets/bahce-kurba.png","assets/selfie-kurba.png"};
    Texture2D defterFotograflari[10];
    for(int i=0;i<10;++i)
    {
        defterFotograflari[i]=LoadTexture(defterFotoYollari[i]);
        SetTextureFilter(defterFotograflari[i],TEXTURE_FILTER_BILINEAR);
    }

    Texture2D filtrelenecek[]={bg.arkaPlan,bg.karakterOn,bg.karakterArka,bg.karakterSag,
        bg.karakterSol,bg.bugday,bg.havuc,bg.cilek,bg.salatalik,bg.elma,
        tg.arkaPlan,tg.tavsanSiparis,tg.tavsanMutlu,tg.balon,tg.havucluKek,
        tg.capybara,tg.capybaraMutlu,tg.kirpi,tg.kirpiMutlu,tg.kurbaga,tg.kurbagaMutlu,
        tg.granola,tg.elmaliTurta,tg.sandvic,
        mg.arkaPlan,envanter,kitapTezgah,acikKitap};
    for(Texture2D& t:filtrelenecek)Filtrele(t);

    OyunDurumu oyun;
    BahceDurumu bahce; BahceyiBaslat(bahce);
    TezgahDurumu tezgah; TezgahiBaslat(tezgah);
    MutfakDurumu mutfak; MutfagiBaslat(mutfak);
    Sahne sahne=TEZGAH_SAHNESI;
    bool kitapAcik=false;
    bool tarifKitabiAcik=false;
    int kitapSayfasi=0;
    int tarifKitabiSayfasi=0;
    bool aniSayfasiAcik[5]={true,false,false,false,false};
    float kilitBildirimiSuresi=0.0f;

    while(!WindowShouldClose())
    {
        float dt=GetFrameTime();
        // Sesleri oyunda denemek icin: F3 toplama, F4 yemek, F5 teslim.
        if(sesHazir)
        {
            if(IsKeyPressed(KEY_F3) && IsSoundValid(sesler.meyveToplama))PlaySound(sesler.meyveToplama);
            if(IsKeyPressed(KEY_F4) && IsSoundValid(sesler.yemekHazir))PlaySound(sesler.yemekHazir);
            if(IsKeyPressed(KEY_F5) && IsSoundValid(sesler.musteriMutlu))PlaySound(sesler.musteriMutlu);
        }
        const int oncekiEnvanter=ToplamEnvanter(oyun);
        const int oncekiHazirYemek=HazirYemekSayisi(oyun);
        const bool oncekiSiparisTamamlandi=oyun.siparisTamamlandi;
        bool yeniSayfaAcildi=false;

        if(IsKeyPressed(KEY_J) && !oyun.envanterAcik)
        {
            if(sahne==MUTFAK_SAHNESI)tarifKitabiAcik=!tarifKitabiAcik;
            else kitapAcik=!kitapAcik;
        }
        if(kitapAcik && IsKeyPressed(KEY_ESCAPE))
            kitapAcik=false;
        if(tarifKitabiAcik && IsKeyPressed(KEY_ESCAPE))tarifKitabiAcik=false;
        if(kilitBildirimiSuresi>0.0f)kilitBildirimiSuresi-=dt;
        int sonAcikSayfa=0;
        for(int i=0;i<5;++i)if(aniSayfasiAcik[i])sonAcikSayfa=i;
        int oncekiKitapSayfasi=kitapSayfasi;
        if(kitapAcik && IsKeyPressed(KEY_RIGHT) && kitapSayfasi<sonAcikSayfa)kitapSayfasi++;
        if(kitapAcik && IsKeyPressed(KEY_LEFT) && kitapSayfasi>0)kitapSayfasi--;
        if(kitapAcik && GetMouseWheelMove()<0 && kitapSayfasi<sonAcikSayfa)kitapSayfasi++;
        if(kitapAcik && GetMouseWheelMove()>0 && kitapSayfasi>0)kitapSayfasi--;
        if(kitapAcik && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 fare=GetMousePosition();
            float fareX=fare.x/S, fareY=fare.y/S;
            if(fareY>=50 && fareY<=190 && fareX>=25 && fareX<160 && kitapSayfasi>0)
                kitapSayfasi--;
            else if(fareY>=50 && fareY<=190 && fareX>=160 && fareX<=295 && kitapSayfasi<sonAcikSayfa)
                kitapSayfasi++;
        }
        if(kitapSayfasi!=oncekiKitapSayfasi && sesHazir && IsSoundValid(sesler.sayfaCevirme))
            PlaySound(sesler.sayfaCevirme);

        if(tarifKitabiAcik)
        {
            int oncekiTarifSayfasi=tarifKitabiSayfasi;
            if(IsKeyPressed(KEY_RIGHT) && tarifKitabiSayfasi<3)tarifKitabiSayfasi++;
            if(IsKeyPressed(KEY_LEFT) && tarifKitabiSayfasi>0)tarifKitabiSayfasi--;
            if(GetMouseWheelMove()<0 && tarifKitabiSayfasi<3)tarifKitabiSayfasi++;
            if(GetMouseWheelMove()>0 && tarifKitabiSayfasi>0)tarifKitabiSayfasi--;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 fare=GetMousePosition();
                float fareX=fare.x/S,fareY=fare.y/S;
                if(fareY>=50 && fareY<=190 && fareX>=25 && fareX<160 && tarifKitabiSayfasi>0)
                    tarifKitabiSayfasi--;
                else if(fareY>=50 && fareY<=190 && fareX>=160 && fareX<=295 && tarifKitabiSayfasi<3)
                    tarifKitabiSayfasi++;
            }
            if(tarifKitabiSayfasi!=oncekiTarifSayfasi && sesHazir && IsSoundValid(sesler.sayfaCevirme))
                PlaySound(sesler.sayfaCevirme);
        }

        if(!kitapAcik && IsKeyPressed(KEY_I))
        {
            oyun.envanterAcik=!oyun.envanterAcik;
        }
        if(!oyun.envanterAcik && !kitapAcik && !tarifKitabiAcik)
        {
            if(sahne==BAHCE_SAHNESI)BahceyiGuncelle(bahce,oyun,sahne,dt);
            else if(sahne==TEZGAH_SAHNESI)TezgahiGuncelle(tezgah,oyun,sahne,dt);
            else MutfagiGuncelle(mutfak,oyun,sahne,dt);

            if(!oncekiSiparisTamamlandi && oyun.siparisTamamlandi)
            {
                oyun.sevgiBagi+=10;
                int yeniAniSayfasi=MusteriAniSayfasi(oyun.musteri);
                if(!aniSayfasiAcik[yeniAniSayfasi])
                {
                    aniSayfasiAcik[yeniAniSayfasi]=true;
                    kilitBildirimiSuresi=2.60f;
                    yeniSayfaAcildi=true;
                }
            }

            if(sesHazir)
            {
                if(oncekiSiparisTamamlandi==false && oyun.siparisTamamlandi &&
                   IsSoundValid(sesler.musteriMutlu))
                    PlaySound(sesler.musteriMutlu);
                else if(HazirYemekSayisi(oyun)>oncekiHazirYemek &&
                        IsSoundValid(sesler.yemekHazir))
                    PlaySound(sesler.yemekHazir);
                else if(sahne==BAHCE_SAHNESI && ToplamEnvanter(oyun)>oncekiEnvanter &&
                        IsSoundValid(sesler.meyveToplama))
                    PlaySound(sesler.meyveToplama);
            }
        }

        BeginDrawing(); ClearBackground(BLACK);
        if(sahne==BAHCE_SAHNESI)BahceyiCiz(bahce,bg,S);
        else if(sahne==TEZGAH_SAHNESI)TezgahiCiz(tezgah,oyun,tg,S);
        else MutfagiCiz(mutfak,oyun,mg,S);
        if(oyun.envanterAcik)EnvanteriCiz(oyun,envanter,bg.bugday,bg.havuc,bg.cilek,bg.salatalik,bg.elma,tg.havucluKek,tg.granola,tg.elmaliTurta,tg.sandvic,S);
        if(!kitapAcik && sahne==TEZGAH_SAHNESI)
            DokuCiz(kitapTezgah,78,134,64,48,S);
        if(!tarifKitabiAcik && sahne==MUTFAK_SAHNESI)
        {
            DokuCiz(kitapTezgah,57,158,42,31.5f,S);
            DrawTextEx(bildirimFontu,"J: TARIFLER",{52.0f*S,190.0f*S},6*S,1*S,DARKBROWN);
        }
        if(kitapAcik)
        {
            DrawRectangle(0,0,320*S,240*S,Fade(BLACK,0.45f));
            DokuCiz(acikKitap,32,40,256,160,S);
            int solFoto=kitapSayfasi*2;
            int sagFoto=solFoto+1;
            FotoCercevesiCiz(defterFotograflari[solFoto],80,74,66,50,S);
            FotoCercevesiCiz(defterFotograflari[sagFoto],174,74,66,50,S);
            KitapOklariniCiz(S,kitapSayfasi>0,kitapSayfasi<sonAcikSayfa);
        }
        if(tarifKitabiAcik)
            TarifKitabiCiz(acikKitap,bildirimFontu,mg,tarifKitabiSayfasi,S);
        PuanGoster(rakamlar,oyun.sevgiBagi,S);
        if(kilitBildirimiSuresi>0.0f)
            KilitAcmaBildirimiCiz(bildirimGorseli,bildirimFontu,kilitBildirimiSuresi,S);
        EndDrawing();
    }

    UnloadTexture(bg.arkaPlan); UnloadTexture(bg.karakterOn); UnloadTexture(bg.karakterArka);
    UnloadTexture(bg.karakterSag); UnloadTexture(bg.karakterSol); UnloadTexture(bg.bugday);
    UnloadTexture(bg.havuc); UnloadTexture(bg.cilek); UnloadTexture(bg.salatalik); UnloadTexture(bg.elma);
    UnloadTexture(tg.arkaPlan); UnloadTexture(tg.tavsanSiparis); UnloadTexture(tg.tavsanMutlu);
    UnloadTexture(tg.balon); UnloadTexture(tg.havucluKek); UnloadTexture(mg.arkaPlan); UnloadTexture(envanter);
    UnloadTexture(tg.capybara); UnloadTexture(tg.capybaraMutlu);
    UnloadTexture(tg.kirpi); UnloadTexture(tg.kirpiMutlu);
    UnloadTexture(tg.kurbaga); UnloadTexture(tg.kurbagaMutlu);
    UnloadTexture(tg.granola); UnloadTexture(tg.elmaliTurta); UnloadTexture(tg.sandvic);
    UnloadTexture(kitapTezgah); UnloadTexture(acikKitap);
    UnloadTexture(rakamlar);
    UnloadTexture(bildirimGorseli);
    UnloadFont(bildirimFontu);
    for(int i=0;i<10;++i)UnloadTexture(defterFotograflari[i]);
    if(sesHazir)
    {
        SesleriKapat(sesler);
        CloseAudioDevice();
    }
    CloseWindow(); return 0;
}

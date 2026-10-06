#include "raylib.h"
#include "bahce.h"
#include "tezgah.h"
#include "mutfak.h"
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

namespace
{
const float DEFTER_ZOOM=1.20f;
const float DEFTER_ICERIK_ZOOM=1.05f;
const float DEFTER_SATIR_YUKSEKLIGI=6.1f*DEFTER_ICERIK_ZOOM;
const int DEFTER_SAYFA_SAYISI=25;
const bool DEFTER_ONIZLEME=false;
struct DefterHikayesi
{
    std::string baslik;
    std::string sol;
    std::string sag;
};

const char* DEFTER_FOTO_YOLLARI[DEFTER_SAYFA_SAYISI][2]={
    {"assets/karakter1.png","assets/karakter2.png"},
    {"assets/tavsan1.png","assets/tavsan2.png"},
    {"assets/capy1.png","assets/capy2.png"},
    {"assets/kurba1.png","assets/kurba2.png"},
    {"assets/kirpi1.png","assets/kirpi2.png"},
    {"assets/tavsan3.png",nullptr},
    {"assets/capy3.png",nullptr},
    {"assets/kurba3.png",nullptr},
    {"assets/kirpi3.png",nullptr},
    {"assets/tavsan4.png",nullptr},
    {"assets/capy4.png",nullptr},
    {"assets/kurba4.png",nullptr},
    {"assets/kirpi4.png",nullptr},
    {"assets/tavsan5.png",nullptr},
    {"assets/capy5.png",nullptr},
    {"assets/kurba5.png",nullptr},
    {"assets/kirpi5.png",nullptr},
    {"assets/tavsan6.png",nullptr},
    {"assets/capy6.png",nullptr},
    {"assets/kurba6.png",nullptr},
    {"assets/kirpi6.png",nullptr},
    {"assets/tavsan7.png",nullptr},
    {"assets/capy7.png",nullptr},
    {"assets/kurba7.png",nullptr},
    {"assets/kirpi7.png",nullptr}
};

bool DefterHikayeleriniYukle(DefterHikayesi hikayeler[DEFTER_SAYFA_SAYISI])
{
    std::ifstream dosya("assets/hikayeler.txt",std::ios::binary);
    if(!dosya)return false;

    DefterHikayesi* aktifSayfa=nullptr;
    std::string* aktifAlan=nullptr;
    std::string satir;
    bool ilkSatir=true;
    while(std::getline(dosya,satir))
    {
        if(ilkSatir && satir.size()>=3 &&
           (unsigned char)satir[0]==0xEF &&
           (unsigned char)satir[1]==0xBB &&
           (unsigned char)satir[2]==0xBF)
            satir.erase(0,3);
        ilkSatir=false;
        if(!satir.empty() && satir.back()=='\r')satir.pop_back();

        if(satir.rfind("[PAGE ",0)==0)
        {
            const int sayfa=std::atoi(satir.c_str()+6);
            aktifSayfa=(sayfa>=0 && sayfa<DEFTER_SAYFA_SAYISI)
                ?&hikayeler[sayfa]:nullptr;
            aktifAlan=nullptr;
            continue;
        }
        if(!aktifSayfa)continue;
        if(!satir.empty() && satir[0]=='#')continue;
        if(satir.empty())
        {
            if(aktifAlan && !aktifAlan->empty() && aktifAlan->back()!='\n')
                *aktifAlan+='\n';
            continue;
        }
        if(satir.rfind("TITLE:",0)==0)
        {
            aktifSayfa->baslik=satir.substr(6);
            if(!aktifSayfa->baslik.empty() && aktifSayfa->baslik[0]==' ')
                aktifSayfa->baslik.erase(0,1);
            aktifAlan=nullptr;
        }
        else if(satir=="LEFT:")aktifAlan=&aktifSayfa->sol;
        else if(satir=="RIGHT:")aktifAlan=&aktifSayfa->sag;
        else if(aktifAlan)
        {
            if(!aktifAlan->empty())*aktifAlan+='\n';
            *aktifAlan+=satir;
        }
    }
    return true;
}

const char* ANA_KARAKTER_NOTLARI[2]={
    "Bazen insan, nereye gitmek istediğini bilmeden yola çıkar. Birkaç eşyam, az param ve cesaretimle geldim. Bahçe yabani otlarla doluydu; evin çatısı akıyordu.",
    "Ve nedense... İlk kez hiçbir yere yetişmem gerekmiyormuş gibi hissettim. O gün yerleştim. Ne kadar kalırım bilmiyorum. Şimdilik burası benim."
};

struct OyunSesleri
{
    Sound meyveToplama;
    Sound ekme;
    Sound yemekHazir;
    Sound siparisTeslim[4];
    Sound sayfaCevirme;
    Sound musteriKonusma[4];
    Music yurume;
};

void SesleriYukle(OyunSesleri& s)
{
    s.meyveToplama=LoadSound("assets/sesler/pop_hizli.wav");
    s.ekme={0};
    Wave ekmeDalgasi=LoadWave("assets/sesler/ekme.wav");
    if(IsWaveValid(ekmeDalgasi))
    {
        WaveCrop(&ekmeDalgasi,0,(int)(ekmeDalgasi.sampleRate*0.38f));
        s.ekme=LoadSoundFromWave(ekmeDalgasi);
        UnloadWave(ekmeDalgasi);
    }
    s.yemekHazir=LoadSound("assets/sesler/yemek.wav");
    s.siparisTeslim[TAVSAN]=LoadSound("assets/sesler/tavsan-thanku.wav");
    s.siparisTeslim[CAPYBARA]=LoadSound("assets/sesler/capy-thanku.wav");
    s.siparisTeslim[KIRPI]=LoadSound("assets/sesler/kirpi-thanku.wav");
    s.siparisTeslim[KURBAGA]=LoadSound("assets/sesler/kurba-thanku.wav");
    s.sayfaCevirme=LoadSound("assets/sesler/page.wav");
    s.musteriKonusma[TAVSAN]=LoadSound("assets/sesler/tavsan_talk_kisa.wav");
    s.musteriKonusma[CAPYBARA]=LoadSound("assets/sesler/capy_talk_kisa.wav");
    s.musteriKonusma[KIRPI]=LoadSound("assets/sesler/kirpi_talk_kisa.wav");
    s.musteriKonusma[KURBAGA]=LoadSound("assets/sesler/frog_talk_kisa.wav");
    s.yurume=LoadMusicStream("assets/sesler/walk_kisik.wav");
    s.yurume.looping=true;
    SetMusicVolume(s.yurume,0.28f);

    TraceLog(LOG_INFO,"Ses dosyalari: toplama=%s, yemek=%s, teslim=%s",
             IsSoundValid(s.meyveToplama)?"OK":"YUKLENEMEDI",
             IsSoundValid(s.yemekHazir)?"OK":"YUKLENEMEDI",
             IsSoundValid(s.siparisTeslim[TAVSAN])?"OK":"YUKLENEMEDI");
}

void SesleriKapat(OyunSesleri& s)
{
    UnloadSound(s.meyveToplama);
    if(IsSoundValid(s.ekme))UnloadSound(s.ekme);
    UnloadSound(s.yemekHazir);
    for(Sound& teslim:s.siparisTeslim)UnloadSound(teslim);
    UnloadSound(s.sayfaCevirme);
    for(Sound& konusma:s.musteriKonusma)UnloadSound(konusma);
    UnloadMusicStream(s.yurume);
}

void KonusmalariDurdur(const OyunSesleri& s)
{
    for(const Sound& konusma:s.musteriKonusma)
        if(IsSoundValid(konusma))StopSound(konusma);
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
    const float cercevePayi=2.0f*DEFTER_ICERIK_ZOOM;
    DrawRectangle((int)((x+cercevePayi)*s),(int)((y+cercevePayi)*s),(int)(w*s),(int)(h*s),Fade(DARKBROWN,0.45f));
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
    Rectangle hedef={(x+cercevePayi)*s,(y+cercevePayi)*s,
                     (w-2*cercevePayi)*s,(h-2*cercevePayi)*s};
    DrawTexturePro(foto,kaynak,hedef,{0,0},0,WHITE);
}

std::string MetniSatirlaraBol(const char* metin,Font font,float boyut,
                              float aralik,float enCokGenislik)
{
    const std::string kaynak(metin);
    std::vector<std::string> satirlar;
    size_t paragrafBaslangici=0;
    while(paragrafBaslangici<=kaynak.size())
    {
        size_t paragrafSonu=kaynak.find('\n',paragrafBaslangici);
        if(paragrafSonu==std::string::npos)paragrafSonu=kaynak.size();
        const std::string paragraf=kaynak.substr(paragrafBaslangici,
                                                 paragrafSonu-paragrafBaslangici);
        std::string satir;
        size_t baslangic=0;
        while(baslangic<paragraf.size())
        {
            size_t bitis=paragraf.find(' ',baslangic);
            if(bitis==std::string::npos)bitis=paragraf.size();
            const std::string kelime=paragraf.substr(baslangic,bitis-baslangic);
            const std::string aday=satir.empty()?kelime:satir+" "+kelime;
            if(!satir.empty() && MeasureTextEx(font,aday.c_str(),boyut,aralik).x>enCokGenislik)
            {
                satirlar.push_back(satir);
                satir=kelime;
            }
            else satir=aday;
            baslangic=bitis+1;
        }
        satirlar.push_back(satir);
        if(paragrafSonu==kaynak.size())break;
        paragrafBaslangici=paragrafSonu+1;
    }

    std::string sonuc;
    for(size_t i=0;i<satirlar.size();++i)
    {
        if(i>0)sonuc+='\n';
        sonuc+=satirlar[i];
    }
    return sonuc;
}

int DefterNotuCiz(Font font,const char* metin,float x,float y,float w,int s,
                  bool kalin=false)
{
    const float boyut=6.5f*DEFTER_ICERIK_ZOOM*s;
    const float aralik=0.4f*DEFTER_ICERIK_ZOOM*s;
    const std::string satirli=MetniSatirlaraBol(metin,font,boyut,aralik,w*s);
    int satirSayisi=1;
    for(char karakter:satirli)if(karakter=='\n')satirSayisi++;
    size_t baslangic=0;
    int satir=0;
    while(baslangic<=satirli.size())
    {
        size_t bitis=satirli.find('\n',baslangic);
        if(bitis==std::string::npos)bitis=satirli.size();
        const std::string parca=satirli.substr(baslangic,bitis-baslangic);
        const Vector2 konum={x*s,(y+satir*DEFTER_SATIR_YUKSEKLIGI)*s};
        DrawTextEx(font,parca.c_str(),konum,boyut,aralik,DARKBROWN);
        if(kalin)DrawTextEx(font,parca.c_str(),{konum.x+0.65f,konum.y},
                            boyut,aralik,DARKBROWN);
        if(bitis==satirli.size())break;
        baslangic=bitis+1;
        satir++;
    }
    return satirSayisi;
}

void AnaKarakterNotlariniCiz(Font font,int s)
{
    const float xSol=78.0f,xSag=171.0f,w=69.0f,y=122.0f;
    const float satirYuksekligi=DEFTER_SATIR_YUKSEKLIGI;
    int solVurguSatirlari=DefterNotuCiz(font,
        "Bazen insan, nereye gitmek istediğini bilmeden yola çıkar.",
        xSol,y,w,s,true);
    DefterNotuCiz(font,
        "Birkaç eşyam, az param ve cesaretimle geldim. Bahçe yabani otlarla doluydu; evin çatısı akıyordu.",
        xSol,y+solVurguSatirlari*satirYuksekligi,w,s);

    int sagGirisSatirlari=DefterNotuCiz(font,"Ve nedense...",xSag,y,w,s);
    int sagVurguSatirlari=DefterNotuCiz(font,
        "İlk kez hiçbir yere yetişmem gerekmiyormuş gibi hissettim.",
        xSag,y+sagGirisSatirlari*satirYuksekligi,w,s,true);
    DefterNotuCiz(font,
        "O gün yerleştim. Ne kadar kalırım bilmiyorum. Şimdilik burası benim.",
        xSag,y+(sagGirisSatirlari+sagVurguSatirlari)*satirYuksekligi,w,s);
}

void KitapOklariniCiz(int s,bool oncekiVar,bool sonrakiVar)
{
    Color solRenk=oncekiVar?DARKBROWN:Fade(DARKBROWN,0.18f);
    Color sagRenk=sonrakiVar?DARKBROWN:Fade(DARKBROWN,0.18f);
    DrawTriangle({39.0f*s,120.0f*s},{51.0f*s,111.0f*s},{51.0f*s,129.0f*s},solRenk);
    DrawTriangle({281.0f*s,120.0f*s},{269.0f*s,111.0f*s},{269.0f*s,129.0f*s},sagRenk);
}

int AcikDefterSayfasi(int puan)
{
    const int sayfa=puan/10;
    return sayfa<DEFTER_SAYFA_SAYISI?sayfa:DEFTER_SAYFA_SAYISI-1;
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

void DefterBagPuaniCiz(Texture2D rakamlar,Font font,int sayfa,int s)
{
    const int puan=sayfa*10;

    int rakamlarDizisi[10];
    int rakamSayisi=0;
    if(puan==0)rakamlarDizisi[rakamSayisi++]=0;
    else
    {
        int kalan=puan;
        while(kalan>0 && rakamSayisi<10)
        {
            rakamlarDizisi[rakamSayisi++]=kalan%10;
            kalan/=10;
        }
    }
    const float genislik=6.0f*s,aralik=1.0f*s;
    float x=250.0f*s-rakamSayisi*(genislik+aralik)+aralik;
    for(int i=rakamSayisi-1;i>=0;--i,x+=genislik+aralik)
    {
        const int rakam=rakamlarDizisi[i];
        Rectangle kaynak=rakam<8
            ?Rectangle{(float)(rakam*16),0,16,16}
            :Rectangle{(float)((rakam-8)*16),16,16,16};
        Rectangle hedef={x,157.0f*s,genislik,9.0f*s};
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
    mg.secimCercevesi=LoadTexture("assets/cerceve.png");
    mg.puff=LoadTexture("assets/puff.png");
    mg.kareler=LoadTexture("assets/kare.png");
    Texture2D envanter=LoadTexture("assets/envanter.png");
    Texture2D kitapTezgah=LoadTexture("assets/kitap_tezgah.png");
    Texture2D acikKitap=LoadTexture("assets/open_book.png");






    Texture2D rakamlar=LoadTexture("assets/Numbers.png");
    SetTextureFilter(rakamlar,TEXTURE_FILTER_POINT);
    Texture2D bildirimGorseli=LoadTexture("assets/Bildirim.png");
    SetTextureFilter(bildirimGorseli,TEXTURE_FILTER_POINT);
    const char* bildirimMesaji="Yeni bir sayfaniz var!";
    int bildirimKodSayisi=0;
    DefterHikayesi defterHikayeleri[DEFTER_SAYFA_SAYISI]={};
    if(!DefterHikayeleriniYukle(defterHikayeleri))
        TraceLog(LOG_WARNING,"assets/hikayeler.txt okunamadi");
    std::string fontKarakterleri="Yeni bir sayfaniz var!";
    fontKarakterleri+=" HAVUCLU KEK GRANOLA ELMALI TURTA SANDVIC BUGDAY HAVUC CILEK ELMA SALATALIK";
    fontKarakterleri+=" MALZEMELER SONUC AFIYET OLSUN! J / ESC: KAPAT BAĞ";
    for(const DefterHikayesi& hikaye:defterHikayeleri)
    {
        fontKarakterleri+=' ';
        fontKarakterleri+=hikaye.baslik;
        fontKarakterleri+=' ';
        fontKarakterleri+=hikaye.sol;
        fontKarakterleri+=' ';
        fontKarakterleri+=hikaye.sag;
    }
    for(const char* notu:ANA_KARAKTER_NOTLARI)
    {
        fontKarakterleri+=' ';
        fontKarakterleri+=notu;
    }
    int fontKodSayisi=0;
    int* fontKodlari=LoadCodepoints(fontKarakterleri.c_str(),&fontKodSayisi);
    Font bildirimFontu=LoadFontEx("assets/Patrick_Hand/PatrickHand-Regular.ttf",48,
                                   fontKodlari,fontKodSayisi);
    UnloadCodepoints(fontKodlari);
    SetTextureFilter(bildirimFontu.texture,TEXTURE_FILTER_BILINEAR);
    Texture2D defterFotograflari[DEFTER_SAYFA_SAYISI][2];
    for(int i=0;i<DEFTER_SAYFA_SAYISI;++i)
    for(int j=0;j<2;++j)
    {
        defterFotograflari[i][j]={0};
        if(DEFTER_FOTO_YOLLARI[i][j])
        {
            defterFotograflari[i][j]=LoadTexture(DEFTER_FOTO_YOLLARI[i][j]);
            SetTextureFilter(defterFotograflari[i][j],TEXTURE_FILTER_BILINEAR);
        }
    }

    Texture2D filtrelenecek[]={bg.arkaPlan,bg.karakterOn,bg.karakterArka,bg.karakterSag,
        bg.karakterSol,bg.bugday,bg.havuc,bg.cilek,bg.salatalik,bg.elma,
        tg.arkaPlan,tg.tavsanSiparis,tg.tavsanMutlu,tg.balon,tg.havucluKek,
        tg.capybara,tg.capybaraMutlu,tg.kirpi,tg.kirpiMutlu,tg.kurbaga,tg.kurbagaMutlu,
        tg.granola,tg.elmaliTurta,tg.sandvic,
        mg.arkaPlan,mg.secimCercevesi,mg.puff,mg.kareler,envanter,kitapTezgah,acikKitap};
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
    float kilitBildirimiSuresi=0.0f;
    bool ilkMusteriKonusmaBekliyor=true;

    while(!WindowShouldClose())
    {
        float dt=GetFrameTime();
        // Sesleri oyunda denemek icin: F3 toplama, F4 yemek, F5 teslim.
        if(sesHazir)
        {
            if(IsKeyPressed(KEY_F3) && IsSoundValid(sesler.meyveToplama))PlaySound(sesler.meyveToplama);
            if(IsKeyPressed(KEY_F4) && IsSoundValid(sesler.yemekHazir))PlaySound(sesler.yemekHazir);
            if(IsKeyPressed(KEY_F5) && IsSoundValid(sesler.siparisTeslim[oyun.musteri]))
                PlaySound(sesler.siparisTeslim[oyun.musteri]);
        }
        const int oncekiEnvanter=ToplamEnvanter(oyun);
        const int oncekiHazirYemek=HazirYemekSayisi(oyun);
        const MusteriTuru oncekiMusteri=oyun.musteri;
        const bool oncekiSiparisTamamlandi=oyun.siparisTamamlandi;
        const Sahne oncekiSahne=sahne;
        const bool oncekiKitapAcik=kitapAcik;
        const bool oncekiTarifKitabiAcik=tarifKitabiAcik;
        const bool oncekiEnvanterAcik=oyun.envanterAcik;
        const UrunTuru oncekiMalzeme1=mutfak.secim1;
        const UrunTuru oncekiMalzeme2=mutfak.secim2;
        bool bahcedeHareketEdiyor=false;
        bool bitkiEkildi=false;
        bool malzemeSecildi=false;
        const Vector2 oncekiBahceKonumu=bahce.karakterKonumu;

        if(IsKeyPressed(KEY_J) && !oyun.envanterAcik)
        {
            if(sahne==MUTFAK_SAHNESI)tarifKitabiAcik=!tarifKitabiAcik;
            else kitapAcik=!kitapAcik;
        }
        if(kitapAcik && IsKeyPressed(KEY_ESCAPE))
            kitapAcik=false;
        if(tarifKitabiAcik && IsKeyPressed(KEY_ESCAPE))tarifKitabiAcik=false;
        if(kilitBildirimiSuresi>0.0f)kilitBildirimiSuresi-=dt;
        int sonAcikSayfa=DEFTER_ONIZLEME
            ?DEFTER_SAYFA_SAYISI-1:AcikDefterSayfasi(oyun.sevgiBagi);
        int oncekiKitapSayfasi=kitapSayfasi;
        if(kitapAcik && IsKeyPressed(KEY_RIGHT) && kitapSayfasi<sonAcikSayfa)kitapSayfasi++;
        if(kitapAcik && IsKeyPressed(KEY_LEFT) && kitapSayfasi>0)kitapSayfasi--;
        if(kitapAcik && GetMouseWheelMove()<0 && kitapSayfasi<sonAcikSayfa)kitapSayfasi++;
        if(kitapAcik && GetMouseWheelMove()>0 && kitapSayfasi>0)kitapSayfasi--;
        if(kitapAcik && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 fare=GetMousePosition();
            float fareX=fare.x/S, fareY=fare.y/S;
                fareX=(fareX-160.0f)/DEFTER_ZOOM+160.0f;
                fareY=(fareY-120.0f)/DEFTER_ZOOM+120.0f;
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
            if(sahne==BAHCE_SAHNESI)
            {
                BahceyiGuncelle(bahce,oyun,sahne,dt);
                bitkiEkildi=bahce.ekimYapildi;
                bahcedeHareketEdiyor=sahne==BAHCE_SAHNESI &&
                    (bahce.karakterKonumu.x!=oncekiBahceKonumu.x ||
                     bahce.karakterKonumu.y!=oncekiBahceKonumu.y);
            }
            else if(sahne==TEZGAH_SAHNESI)TezgahiGuncelle(tezgah,oyun,sahne,dt);
            else
            {
                MutfagiGuncelle(mutfak,oyun,sahne,dt);
                malzemeSecildi=mutfak.secim1!=oncekiMalzeme1 ||
                               mutfak.secim2!=oncekiMalzeme2;
            }

            if(!oncekiSiparisTamamlandi && oyun.siparisTamamlandi)
            {
                int oncekiAcikAniSayfasi=AcikDefterSayfasi(oyun.sevgiBagi);
                oyun.sevgiBagi+=10;
                int yeniAcikAniSayfasi=AcikDefterSayfasi(oyun.sevgiBagi);
                if(yeniAcikAniSayfasi>oncekiAcikAniSayfasi)
                {
                    kilitBildirimiSuresi=2.60f;
                }
            }

            if(sesHazir)
            {
                if(ilkMusteriKonusmaBekliyor || oyun.musteri!=oncekiMusteri)
                {
                    Sound konusma=sesler.musteriKonusma[oyun.musteri];
                    if(IsSoundValid(konusma))PlaySound(konusma);
                    ilkMusteriKonusmaBekliyor=false;
                }
                else if(!oncekiSiparisTamamlandi && oyun.siparisTamamlandi)
                {
                    KonusmalariDurdur(sesler);
                    Sound teslimSesi=sesler.siparisTeslim[oyun.musteri];
                    if(IsSoundValid(teslimSesi))PlaySound(teslimSesi);
                }
                else if(HazirYemekSayisi(oyun)>oncekiHazirYemek &&
                        IsSoundValid(sesler.yemekHazir))
                    PlaySound(sesler.yemekHazir);
                else if(bitkiEkildi && IsSoundValid(sesler.ekme))
                    PlaySound(sesler.ekme);
                else if(malzemeSecildi && IsSoundValid(sesler.meyveToplama))
                    PlaySound(sesler.meyveToplama);
                else if(sahne==BAHCE_SAHNESI && ToplamEnvanter(oyun)>oncekiEnvanter &&
                        IsSoundValid(sesler.meyveToplama))
                    PlaySound(sesler.meyveToplama);
            }
        }

        if(sesHazir && sesler.yurume.ctxData!=nullptr)
        {
            if(bahcedeHareketEdiyor)
            {
                if(!IsMusicStreamPlaying(sesler.yurume))PlayMusicStream(sesler.yurume);
                UpdateMusicStream(sesler.yurume);
            }
            else if(IsMusicStreamPlaying(sesler.yurume))StopMusicStream(sesler.yurume);
        }

        const bool ekranDegisti=sahne!=oncekiSahne || kitapAcik!=oncekiKitapAcik ||
            tarifKitabiAcik!=oncekiTarifKitabiAcik || oyun.envanterAcik!=oncekiEnvanterAcik;
        if(sesHazir && (ekranDegisti ||
           (!oncekiSiparisTamamlandi && oyun.siparisTamamlandi)))
            KonusmalariDurdur(sesler);

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
            Camera2D kitapKamerasi={{160.0f*S,120.0f*S},{160.0f*S,120.0f*S},0,DEFTER_ZOOM};
            BeginMode2D(kitapKamerasi);
            DokuCiz(acikKitap,26,31,268,178,S);
            if(kitapSayfasi==0)
            {
                FotoCercevesiCiz(defterFotograflari[kitapSayfasi][0],78,66,69,52.5f,S);
                FotoCercevesiCiz(defterFotograflari[kitapSayfasi][1],172,66,69,52.5f,S);
                AnaKarakterNotlariniCiz(bildirimFontu,S);
            }
            else if(kitapSayfasi<=4)
            {
                FotoCercevesiCiz(defterFotograflari[kitapSayfasi][0],78,66,69,52.5f,S);
                FotoCercevesiCiz(defterFotograflari[kitapSayfasi][1],172,66,69,52.5f,S);
                int vurguSatirlari=DefterNotuCiz(bildirimFontu,defterHikayeleri[kitapSayfasi].baslik.c_str(),
                                                  78,121,69,S,true);
                DefterNotuCiz(bildirimFontu,defterHikayeleri[kitapSayfasi].sol.c_str(),
                    78,121+vurguSatirlari*DEFTER_SATIR_YUKSEKLIGI,69,S);
                DefterNotuCiz(bildirimFontu,defterHikayeleri[kitapSayfasi].sag.c_str(),172,121,66,S);
            }
            else
            {
                FotoCercevesiCiz(defterFotograflari[kitapSayfasi][0],78,66,69,52.5f,S);
                int vurguSatirlari=DefterNotuCiz(bildirimFontu,defterHikayeleri[kitapSayfasi].baslik.c_str(),
                                                  78,121,69,S,true);
                DefterNotuCiz(bildirimFontu,defterHikayeleri[kitapSayfasi].sol.c_str(),
                              78,121+vurguSatirlari*DEFTER_SATIR_YUKSEKLIGI,69,S);
                DefterNotuCiz(bildirimFontu,defterHikayeleri[kitapSayfasi].sag.c_str(),
                              171,67,77,S);
            }
            DefterBagPuaniCiz(rakamlar,bildirimFontu,kitapSayfasi,S);
            KitapOklariniCiz(S,kitapSayfasi>0,kitapSayfasi<sonAcikSayfa);
            EndMode2D();
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
    UnloadTexture(tg.balon); UnloadTexture(tg.havucluKek); UnloadTexture(mg.arkaPlan);
    UnloadTexture(mg.secimCercevesi); UnloadTexture(mg.puff); UnloadTexture(mg.kareler);
    UnloadTexture(envanter);
    UnloadTexture(tg.capybara); UnloadTexture(tg.capybaraMutlu);
    UnloadTexture(tg.kirpi); UnloadTexture(tg.kirpiMutlu);
    UnloadTexture(tg.kurbaga); UnloadTexture(tg.kurbagaMutlu);
    UnloadTexture(tg.granola); UnloadTexture(tg.elmaliTurta); UnloadTexture(tg.sandvic);
    UnloadTexture(kitapTezgah); UnloadTexture(acikKitap);
    UnloadTexture(rakamlar);
    UnloadTexture(bildirimGorseli);
    UnloadFont(bildirimFontu);
    for(int i=0;i<DEFTER_SAYFA_SAYISI;++i)
    for(int j=0;j<2;++j)
        if(DEFTER_FOTO_YOLLARI[i][j])UnloadTexture(defterFotograflari[i][j]);
    if(sesHazir)
    {
        SesleriKapat(sesler);
        CloseAudioDevice();
    }
    CloseWindow(); return 0;
}

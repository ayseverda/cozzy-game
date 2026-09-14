#include "raylib.h"
#include "bahce.h"
#include "tezgah.h"
#include "mutfak.h"

namespace
{
void Filtrele(Texture2D& t){SetTextureFilter(t,TEXTURE_FILTER_POINT);}
void DokuCiz(Texture2D t,float x,float y,float w,float h,int s)
{
    DrawTexturePro(t,{0,0,(float)t.width,(float)t.height},{x*s,y*s,w*s,h*s},{0,0},0,WHITE);
}

void EnvanteriCiz(const OyunDurumu& o,Texture2D panel,
                  Texture2D bugday,Texture2D havuc,Texture2D cilek,
                  Texture2D salatalik,Texture2D elma,Texture2D kek,int s)
{
    DokuCiz(panel,0,0,320,240,s);
    Texture2D dokular[6]={bugday,havuc,cilek,salatalik,elma,kek};
    int adetler[6]={o.bugday,o.havuc,o.cilek,o.salatalik,o.elma,o.havucluKek};
    // PNG icindeki gercek slot merkezleri.
    const float merkezX[5]={79,119,159,199,239};
    for(int i=0;i<6;++i)
    {
        float cx=i<5?merkezX[i]:merkezX[0];
        float cy=i<5?99.0f:131.0f;
        float w=(i==5)?18.0f:(float)dokular[i].width;
        float h=(i==5)?18.0f:(float)dokular[i].height;
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

    MutfakGorselleri mg;
    mg.arkaPlan=LoadTexture("assets/mutfak.png");
    mg.bugday=bg.bugday; mg.havuc=bg.havuc; mg.cilek=bg.cilek;
    mg.salatalik=bg.salatalik; mg.elma=bg.elma; mg.havucluKek=tg.havucluKek;
    Texture2D envanter=LoadTexture("assets/envanter.png");

    Texture2D filtrelenecek[]={bg.arkaPlan,bg.karakterOn,bg.karakterArka,bg.karakterSag,
        bg.karakterSol,bg.bugday,bg.havuc,bg.cilek,bg.salatalik,bg.elma,
        tg.arkaPlan,tg.tavsanSiparis,tg.tavsanMutlu,tg.balon,tg.havucluKek,
        mg.arkaPlan,envanter};
    for(Texture2D& t:filtrelenecek)Filtrele(t);

    OyunDurumu oyun;
    BahceDurumu bahce; BahceyiBaslat(bahce);
    TezgahDurumu tezgah; TezgahiBaslat(tezgah);
    MutfakDurumu mutfak; MutfagiBaslat(mutfak);
    Sahne sahne=TEZGAH_SAHNESI;

    while(!WindowShouldClose())
    {
        float dt=GetFrameTime();
        if(IsKeyPressed(KEY_I))oyun.envanterAcik=!oyun.envanterAcik;
        if(!oyun.envanterAcik)
        {
            if(sahne==BAHCE_SAHNESI)BahceyiGuncelle(bahce,oyun,sahne,dt);
            else if(sahne==TEZGAH_SAHNESI)TezgahiGuncelle(tezgah,oyun,sahne,dt);
            else MutfagiGuncelle(mutfak,oyun,sahne,dt);
        }

        BeginDrawing(); ClearBackground(BLACK);
        if(sahne==BAHCE_SAHNESI)BahceyiCiz(bahce,bg,S);
        else if(sahne==TEZGAH_SAHNESI)TezgahiCiz(tezgah,oyun,tg,S);
        else MutfagiCiz(mutfak,oyun,mg,S);
        if(oyun.envanterAcik)EnvanteriCiz(oyun,envanter,bg.bugday,bg.havuc,bg.cilek,bg.salatalik,bg.elma,tg.havucluKek,S);
        EndDrawing();
    }

    UnloadTexture(bg.arkaPlan); UnloadTexture(bg.karakterOn); UnloadTexture(bg.karakterArka);
    UnloadTexture(bg.karakterSag); UnloadTexture(bg.karakterSol); UnloadTexture(bg.bugday);
    UnloadTexture(bg.havuc); UnloadTexture(bg.cilek); UnloadTexture(bg.salatalik); UnloadTexture(bg.elma);
    UnloadTexture(tg.arkaPlan); UnloadTexture(tg.tavsanSiparis); UnloadTexture(tg.tavsanMutlu);
    UnloadTexture(tg.balon); UnloadTexture(tg.havucluKek); UnloadTexture(mg.arkaPlan); UnloadTexture(envanter);
    CloseWindow(); return 0;
}

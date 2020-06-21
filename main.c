#include <stdio.h>
#include <stdlib.h>


typedef struct kapcs{
    int szam;
    int dij;
    struct kapcs* kov;
}kapcs;

typedef struct muhold{
    int szam;
    int fb;     /*fb = felső becslés arra, hogy milyen áron lehet elérni ezt a műholdat*/
    char bef;   /*befejezettség*/
    struct muhold* beallito;
    struct muhold* kov;
    kapcs* KHEAD;
}muhold;

muhold* muholdkereso(muhold* MHEAD,int n)   /*visszaadja a keresett számú műhold pointerét*/
{
    muhold* mp=MHEAD;
    while (mp!=NULL && mp->szam!=n) mp=mp->kov;
    return mp;
}

kapcs* ujkapcs(int masik, int percdij)      /*létrehoz egy a megadott adatokkal kitöltött kapcsolatot*/
{
    kapcs* uj=(kapcs*)malloc(sizeof(kapcs));
    uj->dij=percdij;
    uj->kov=NULL;
    uj->szam=masik;
    return uj;
}

muhold* ujmuhold(int szama)                 /*létrehoz egy kapcsolatstrázsákkal rendelkező új műholdat*/
{
    muhold* ujM=(muhold*)malloc(sizeof(muhold));
    ujM->szam=szama;
    ujM->fb=-1;
    ujM->bef='N';
    ujM->beallito=NULL;
    ujM->kov=NULL;
    ujM->KHEAD=ujkapcs(0,0);
    ujM->KHEAD->kov=ujkapcs(0,0);
    return ujM;
}

void K_rend_beszur(muhold *mp, kapcs* uj)   /* egy műhold kapcsolatai közé rendezetten beszúr egy új kapcsolatot*/
{
    kapcs* aktkapcs, *fogo;
    for(aktkapcs=mp->KHEAD;aktkapcs->kov->szam<uj->szam && aktkapcs->kov->szam!=0;aktkapcs=aktkapcs->kov);
    fogo=aktkapcs->kov;
    aktkapcs->kov=uj;
    uj->kov=fogo;
}

muhold* M_Lista_generator(int n)            /*visszaad egy n-hosszú, kapcsolatstrázsás műholdlistát*/
{
    muhold *MHEAD, *mp;
    int i;
    if (n==0)return NULL;
    MHEAD=mp=ujmuhold(1);      /*az első műhold*/
    for (i=2;i<=n;i++)
    {
        mp->kov=ujmuhold(i);      /*A többi műhold*/
        mp=mp->kov;
    }
    return MHEAD;
}

kapcs* K_hely(muhold* mp, int n)            /*visszaadja az adott számú kapcsolatot rendezett listában megelőző kapcsolat pointerét*/
{
    kapcs *elozo;
    for(elozo=mp->KHEAD;elozo->kov->szam!=0 && elozo->kov->szam<n;elozo=elozo->kov);
    return elozo;
}

void K1_free(muhold* mp)                    /* felszabadítja egy műhold legfelső hasznos kapcsolatát*/
{
    kapcs* fogo=mp->KHEAD->kov->kov;
    if(fogo!=NULL)
    {
        free(mp->KHEAD->kov);
        mp->KHEAD->kov=fogo;
    }
}

void M1_free(muhold** MHEAD)                /*felszabadítja a legelő műholdat egy nemüres, kapcsolattalan műholdláncban*/
{
    free((*MHEAD)->KHEAD->kov);
    (*MHEAD)->KHEAD->kov=NULL;
    free((*MHEAD)->KHEAD);
    (*MHEAD)->KHEAD=NULL;
    muhold* fogo=(*MHEAD)->kov;
    free(*MHEAD);
    *MHEAD=fogo;
}

void K_free(muhold* MHEAD)                  /*felszabadítja az összes műhold összes kapcsolatát*/
{
    muhold* mp;

    for(mp=MHEAD;mp!=NULL;mp=mp->kov)
    {
        while(mp->KHEAD->kov->kov!=NULL)
        {
            K1_free(mp);
        }
    }
}

void M_free(muhold** MHEAD)                 /* felszabadítja a kapcsolattalan műholdlista kapcsolatstrázsáit és műholdjait*/
{
    while(*MHEAD!=NULL)
    {
        M1_free(MHEAD);
    }
}

void szetkapcs(muhold* MHEAD, int m1, int m2)   /*2 megadott műhold közötti kapcsolatot kitöröl*/
{
    kapcs* aktkapcs, *fogo;
    muhold* mp1=muholdkereso(MHEAD, m1);
    muhold* mp2=muholdkereso(MHEAD, m2);
                                        /*a következő for loop ellépeget a kitörölni kívánt kapcsolat előtti kapcsolatig*/
    aktkapcs=K_hely(mp1, m2);
    if(aktkapcs->kov->szam==m2)
    {
    fogo=aktkapcs->kov->kov;
    free(aktkapcs->kov);
    aktkapcs->kov=fogo;
    }
                                        /*ugyanezt végigcsinálja a kitörölni kívánt kapcsolat másik végén*/
    aktkapcs=K_hely(mp2, m1);
    if(aktkapcs->kov->szam==m1)
    {
    fogo=aktkapcs->kov->kov;
    free(aktkapcs->kov);
    aktkapcs->kov=fogo;
    }
}

void osszekapcs(muhold* MHEAD, int m1, int m2, int ujdij)       /*2 megadott műhold között ujdij díjú kapcsolatot hoz létre*/
{                                                               /* vagy ha már van kapcsolatuk, annak a díját frissíti*/
    muhold *mp1 = muholdkereso(MHEAD, m1);
    muhold *mp2 = muholdkereso(MHEAD, m2);
    kapcs *aktK, *elozo;

    elozo=K_hely(mp1,m2);
    aktK=elozo->kov;
    if(aktK->szam!=m2)                                      /*a kapcsolatok között nincs meg az új kapcsolat*/
        K_rend_beszur(mp1,ujkapcs(m2,ujdij));
    else                                                    /*itt megvan, csak az árat kell frissíteni*/
        aktK->dij=ujdij;

    elozo=K_hely(mp2,m1);                                   /*itt ugyanez lejátszódik a frissülő*/
    aktK=elozo->kov;                                        /*kapcsolat másik végén lévő műholdon is*/
    if(aktK->szam!=m1)
        K_rend_beszur(mp2,ujkapcs(m1,ujdij));
    else
        aktK->dij=ujdij;
}

void kapcs_frissit(muhold* MHEAD, int m1, int m2, int ujdij)    /*újraárazza vagy törli 2 műhold kapcsolatát*/
{
    if(ujdij==0) szetkapcs(MHEAD, m1, m2);
    else osszekapcs(MHEAD, m1, m2, ujdij);
}

void javito1(muhold* MHEAD, int n, kapcs* el)       /*#n műhold "el" pointerrel megadott kapcsolatán keresztül javít*/
{
    muhold* m1=muholdkereso(MHEAD, n);
    muhold* m2=muholdkereso(MHEAD, el->szam);
    if(m1->fb==-1)return;                       /*eléretlen műholdból nem javítunk*/
    if(m1->fb+el->dij < m2->fb || m2->fb==-1)
    {
        m2->fb = m1->fb + el->dij;
        m2->beallito = m1;
    }
}

void javito(muhold* MHEAD, int n)                   /*#n műhold kapcsolatai mentén javítja a felső becsléseket*/
{
    muhold* m=muholdkereso(MHEAD, n);
    kapcs* kp;
    for(kp = m->KHEAD->kov;kp->kov!=NULL;kp=kp->kov)
    {
        javito1(MHEAD, n, kp);
    }
}

int vege(muhold* MHEAD, int hivott)                 /* 0 = tart a keresés*/
{                                                   /* 1 = megvan a legolcsóbb hívási útvonal*/
    int van_elert=0, hivott_bef=0;                  /* 2 = nem lehet hívást létrehozni*/
    muhold* mp;
    for(mp=MHEAD;mp!=NULL;mp=mp->kov)
    {
        if(mp->bef=='N' && mp->fb!=-1)            /*van még tovább*/
        van_elert=1;
        if(mp->szam==hivott && mp->bef=='I')
        hivott_bef=1;
    }
    if(hivott_bef==1) return 1;
    if(van_elert==1) return 0;
    else return 2;
}

muhold* LKFBEBM(muhold* MHEAD)              /*legkisebb felső becslésű elért, befejezetlen műhold*/
{
    muhold* mp, *legjobb=NULL;
    int lkfb;
    for(mp=MHEAD;mp!=NULL;mp=mp->kov)   /*ez a loop megkeres egy elért és befejezetlen műholdat*/
    {
        if(mp->bef=='N' && mp->fb!=-1)
        {
            legjobb=mp;
            lkfb=mp->fb;
        }
    }
    for(mp=MHEAD;mp!=NULL;mp=mp->kov)       /*ez a loop megkeresi a legkisebb felső becslésű elért és befejezetlen műholdat*/
    {
        if(mp->bef=='N' && mp->fb!=-1 && mp->fb<lkfb)
        {
            legjobb=mp;
            lkfb=mp->fb;
        }
    }
    return legjobb;
}
                                    /*megkeresi a legkisebb felső becslésű, elért, befejezetlen műholdat,*/
void Dijkstra(muhold* MHEAD)     /*ezt befejezetté teszi és élmenti javítást végez minden szomszédjába*/
{
    muhold* LKFBMP=NULL;        /*legkisebb felső becslésű befejezetlen műhold pointere*/
    LKFBMP=LKFBEBM(MHEAD);
    if(LKFBMP==NULL)return;/*ha nincs elért műhold, nem lehet mit csinálnia*/
    /*------------------------------ezen a ponton megvan az a műhold, ami elértté válik*/
    javito(MHEAD, LKFBMP->szam);/*élmenti javításokat végzünk az összes LKFBM-ból kifutó él mentén*/
    LKFBMP->bef='I';            /*befejezett lett*/
    return;
}

muhold* rendszer_epit(int* hivo, int* hivott)
{
    int n, egyikM, masikM, dij;
    muhold* MHEAD, *mp;
    FILE* fp;
    *hivo=*hivott=0;
    if(!(fp=fopen("rendszer.txt","r")))
    {
        printf("\n\nHiba: nem sikerült megnyitni a \"rendszer.txt\"-t");
        return NULL;
    }
    if((fscanf(fp, "%d",&n))!=1)
    {
        printf("Hiba: nincs műholdszám");
        fclose(fp);
        return NULL;
    }
    MHEAD=M_Lista_generator(n);
    while(fscanf(fp, "%d %d %d",&egyikM,&masikM,&dij)==3)
    {
        osszekapcs(MHEAD, egyikM, masikM, dij);
    }
    fclose(fp);

    if(!(fp=fopen("hivas.txt","r")))
    {
        printf("\n\nHiba: nem sikerült megnyitni a \"hivas.txt\"-t");
        return MHEAD;
    }
    if((fscanf(fp,"%d %d",hivott,hivo))!=2)
    {
        printf("\n\nHiba: nem lehetett beolvasni a hívó feleket");
        fclose(fp);
        return MHEAD;
    }
    while(fscanf(fp,"%d %d %d",&egyikM, &masikM, &dij)==3)
    {
        kapcs_frissit(MHEAD, egyikM, masikM, dij);
    }
    fclose(fp);
    mp=muholdkereso(MHEAD, *hivo);
    mp->beallito=NULL;
    mp->bef='N';
    mp->fb=0;
    return MHEAD;
}

void utvonal_kiir(muhold* mp)
{
    printf("\n\nTeljes Díj: %d\nÚtvonal: ", mp->fb);
    if(mp==NULL)return;
    while(mp!=NULL)
    {
        printf(" %d ",mp->szam);
        mp=mp->beallito;
    }
}

int main()
{
    int hivo, hivott, i=0;
    muhold* head=rendszer_epit(&hivo, &hivott);
    while(vege(head, hivott)==0)                 /*fut az algoritmus*/
    {
        Dijkstra(head);
        i++;
    }
    if(vege(head, hivott)==1) utvonal_kiir(muholdkereso(head, hivott));   /* ha megvan az útvonal, kiírja*/
    else printf("\nA hívó felek nem tudnak beszélni");           /* ha nem lehet összekapcsolni a hívó feleket*/
    printf("\nDijkstra iterációk száma: %d\n", i);
    K_free(head);
    M_free(&head);
    return 0;
}

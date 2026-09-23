#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>

#define MAX_AUTO_PER_STAZIONE 512

//struct per salvare il percorso
typedef struct tappe{
    struct tappe *next;
    struct stazione *riferimento;
}tappe;

//struct per una stazione che contiene un parco auto
typedef struct stazione{
    int distanza;
    int max_autonomia;
    //int n_auto;        // n_auto è max 512
    //int *parco_auto;   // array dinamico;   attenzione:  int parco_auto[MAX_AUTO_PER_STAZIONE]
    int parco_auto[MAX_AUTO_PER_STAZIONE];
    //int pos_lib;
    struct stazione *next;
    struct stazione *prev;
}stazione;
typedef stazione *staz;
typedef stazione **stazion;




staz cerca_stazione(staz head, int distanza);                                           



staz creaStazione(int distanza, int n_auto, int autonomie[]);
bool aggiungiStazione(stazione **head, int distanza, int n_auto, int autonomie[],FILE*);

bool demolisciStazione(stazione **head, int distanza_stazione,FILE*);

bool aggiungiAuto(stazione *station, int autonomia_auto_da_aggiungere,FILE*);

bool esisteAutoConAutonomia(stazione *station, int autonomia);
bool rottamaAuto(stazione **head, int distanza_stazione, int autonomia_auto_da_rottamare,FILE*);





int max_auto(int a[]);

void pianificaAvanti(stazione *a, int staz1, int staz2,FILE*);
stazione *ricerca(stazione *partenza, stazione *arrivo);
tappe *in_testa(tappe *percorso, stazione *s);
stazione* cercaS(stazione *a, int s);



bool pianifica_indietro(stazione*,int,int,FILE*);

void aggiornaTappe(tappe*);
void stampaP(tappe *t,FILE *f);

//per testare
int main(){
    stazione *head = NULL;
    char comando[20];
    int distanza, n_auto, autonomia;
    int autonomie[512],j=0;
    FILE *out, *in;
    in=studin;
    out=stdout;
    while(fscanf(in,"%s", comando) != EOF){
        j++;
        if(strcmp(comando, "aggiungi-stazione") == 0){
            if(fscanf(in,"%d %d", &distanza, &n_auto) != EOF){

                for(int i = 0; i < n_auto; i++){
                    if(fscanf(in,"%d", &autonomie[i]) != EOF);
                }
                aggiungiStazione(&head, distanza, n_auto, autonomie,out);
            }

        }else if(strcmp(comando, "demolisci-stazione") == 0){
            if(fscanf(in,"%d", &distanza) != EOF){
                demolisciStazione(&head, distanza,out);
            }

        }else if(strcmp(comando, "aggiungi-auto") == 0){
            if(fscanf(in,"%d %d", &distanza, &autonomia) != EOF){
                staz stazione_esistente = cerca_stazione(head, distanza);
                if(stazione_esistente == NULL){
                    fprintf(out,"non aggiunta\n");
                }else{
                    aggiungiAuto(stazione_esistente, autonomia,out);
                }
            }

        }else if(strcmp(comando, "rottama-auto") == 0){
            if(fscanf(in,"%d %d", &distanza, &autonomia) != EOF){
                rottamaAuto(&head, distanza, autonomia,out);
            }

        }else if(strcmp(comando, "pianifica-percorso") == 0){
            int partenza, arrivo;
            if(fscanf(in,"%d %d", &partenza, &arrivo) != EOF){
                if(partenza<arrivo)
                   pianificaAvanti(head, partenza, arrivo,out);
                else if(partenza>arrivo)
                   pianifica_indietro(head,partenza,arrivo,out);
                else
                fprintf(out,"%d\n",partenza);
                /*if(partenza<arrivo)
                    pianificaAvanti(head, partenza, arrivo);
                else if(partenza>arrivo)
                    fprintf(out,"to do percorso decrescente\n");
                    pianifica_indietro(head,partenza,arrivo,out);
                else
                    printf("%d\n",partenza);  */


            }
        }
        //printf("%d ",j);
    }
    fclose(in);
    fclose(out);
    return 0;
}






staz cerca_stazione(staz head, int distanza) {         //serve per il main, ma potrei modificarlo(però non cambia nulla modificarlo)
    staz cur = head;  
    while (cur != NULL) {
        if (cur->distanza == distanza) {
            return cur; // Stazione trovata
        }
        cur = cur->next;
    }
    return NULL; // Stazione non trovata
}


// VERSIONE CORRETTA DEI COMANDI

staz creaStazione(int distanza, int n_auto, int autonomie[]){
    staz station  = (stazione *)malloc(sizeof(stazione));
    station->distanza = distanza;
    int max=0;
    for (int i = 0; i < MAX_AUTO_PER_STAZIONE; i++) {
        if(i<n_auto){
            station->parco_auto[i] = autonomie[i];
            if(autonomie[i]>max)
                max=autonomie[i];
        }else
            station->parco_auto[i]=-1;
        
    }
    station->max_autonomia = max;
    station->next = NULL;
    station->prev = NULL;
    return station;
}




// Funzione per aggiungere una stazione alla lista
bool aggiungiStazione(stazione **head, int distanza, int n_auto, int autonomie[],FILE*out){
    // Verifica se esiste già una stazione alla stessa distanza
    stazione *cur = *head;
    while (cur != NULL) {
        if (cur->distanza == distanza) {
            fprintf(out,"non aggiunta\n");
            return false;
        }
        cur = cur->next;
    }

    // Se non esiste una stazione alla stessa distanza, aggiungi la nuova stazione
    stazione *nuova_stazione = creaStazione(distanza, n_auto, autonomie);
    // Se l'autostrada è vuota o la nuova stazione è prima di tutte le stazioni esistenti
    // Inserisce la nuova stazione in ordine crescente rispetto alla distanza
    if (*head == NULL || (*head)->distanza > distanza) {
        nuova_stazione->next = *head;
        if (*head != NULL) {
            (*head)->prev = nuova_stazione;
        }
        *head = nuova_stazione;
    } else {
        cur = *head;
        while (cur->next != NULL && cur->next->distanza < distanza) {
            cur = cur->next;
        }
        nuova_stazione->next = cur->next;
        nuova_stazione->prev = cur;
        if (cur->next != NULL) {
            cur->next->prev = nuova_stazione;
        }
        cur->next = nuova_stazione;
    }

    fprintf(out,"aggiunta\n");
    return true;
}






// Funzione per demolire una stazione
bool demolisciStazione(stazione **head, int distanza_stazione,FILE*out) {
    // Cerca la stazione corrispondente
    stazione *station = *head;
    stazione *station_precedente = NULL;

    while (station != NULL) {
        if (station->distanza == distanza_stazione) {
            // Rimuovi la stazione dalla lista
            if (station_precedente != NULL) {
                station_precedente->next = station->next;
            } else {
                // Se stiamo rimuovendo la prima stazione
                *head = station->next;
            }
            if(station->next)
                station->next->prev=station_precedente;
            // Libera la memoria della stazione

            free(station);

            fprintf(out,"demolita\n");
            return true;
        }

        station_precedente = station;
        station = station->next;
    }

    // Stazione non trovata
    fprintf(out,"non demolita\n");
    return false;
}





// Funzione per aggiungere un'auto alla stazione
bool aggiungiAuto(stazione *station, int autonomia_auto_da_aggiungere,FILE*out) {
    if (station == NULL) {
        fprintf(out,"non aggiunta\n");
        return false;
    }
    int i;
    // Verifica se l'autonomia è già presente nel parco auto
    for (i = 0; i < MAX_AUTO_PER_STAZIONE&&station->parco_auto[i]!=-1; i++);
    station->parco_auto[i]=autonomia_auto_da_aggiungere;
    if(autonomia_auto_da_aggiungere>station->max_autonomia)
        station->max_autonomia=autonomia_auto_da_aggiungere;
    fprintf(out,"aggiunta\n");
    return true;
}





// Funzione per rottamare un'auto dalla stazione
bool rottamaAuto(stazione **head, int distanza_stazione, int autonomia_auto_da_rottamare,FILE*out) {
    // Cerca la stazione corrispondente
    stazione *station = *head;
    int i;
    while (station != NULL) {
        if (station->distanza == distanza_stazione) {
            for(i=0;i<MAX_AUTO_PER_STAZIONE&&station->parco_auto[i]!=autonomia_auto_da_rottamare;i++);
            if(i<MAX_AUTO_PER_STAZIONE){
                station->parco_auto[i]=-1;
                if(station->max_autonomia==autonomia_auto_da_rottamare)
                    station->max_autonomia=max_auto(station->parco_auto);
                fprintf(out,"rottamata\n");
                return true;
            }else
                fprintf(out,"non rottamata\n");
            return false;
        }
        station = station->next;
    }

    // Stazione non trovata
    fprintf(out,"non rottamata\n");
    return false;
}

int max_auto(int a[]){
    int i,max=0;
    for(i=0;i<MAX_AUTO_PER_STAZIONE;i++){
        if(a[i]>max)
            max=a[i];

    }
    return max;
}






void pianificaAvanti(stazione *a, int staz1, int staz2,FILE*out){
    stazione *partenza = NULL;
    stazione *arrivo = NULL;
    stazione *cur = NULL;
    tappe *percorso = NULL, *canc=NULL,*tmp=NULL;
    partenza = cercaS(a, staz1); //vado a recuperare il riferimento alle due stazioni, partenza e arrivo
    arrivo = cercaS(a, staz2);
    percorso = in_testa(percorso, arrivo);  //metto già la destinazione come tappa ultima
    cur = ricerca(partenza, arrivo); //calcolo la stazione più piccola che può raggiungere la destinazione
    if(cur&&cur->distanza==partenza->distanza) //caso 1 : valuto che questa stazione sia uguale alla partenza, se si la aggiungo al percorso e stampo
        percorso = in_testa(percorso, cur);
    while(cur&&cur->distanza!=partenza->distanza){ //se non sono nel caso 1, cerco le altre tappe
        percorso = in_testa(percorso, cur); //aggiungo la tappa
        cur = ricerca(partenza, cur);  //ricerca ritorna la stazione più piccola >= alla partenza che può raggiungere la destinazione corrente
    }
    if((cur)&&(cur->distanza==partenza->distanza))
        percorso=in_testa(percorso,cur);
    tmp=percorso;
    if(percorso->riferimento->distanza==partenza->distanza){ //se l'ultima tappa che può raggiungere la destinazione è proprio la partenza, stampo il percorso
        while(percorso != NULL){      //WARNING   risolvo
            fprintf(out,"%d", percorso->riferimento->distanza);
            if(percorso->next)
                fprintf(out," ");
            else
                fprintf(out,"\n");
            percorso = percorso->next;
        }
    }else
        fprintf(out,"nessun percorso\n");
    while(tmp!=NULL){
        canc=tmp;
        tmp=tmp->next;
        free(canc);
    }
    
}

void stampaPercorso(tappe *percorso,FILE *f){
    if(percorso!=NULL){
        stampaP(percorso->next,f);
        fprintf(f,"%d\n",percorso->riferimento->distanza);
    }
}
void stampaP(tappe *t,FILE *f){
    if(t){
        stampaP(t->next,f);
        fprintf(f,"%d ",t->riferimento->distanza);
    }
}



stazione *ricerca(stazione *partenza, stazione *arrivo){
    stazione *cur;
    //trovo la stazione più piccola che può raggiungere la destinazione
    for(cur = partenza; (cur->distanza+cur->max_autonomia<arrivo->distanza)&&(cur->distanza<arrivo->distanza);cur=cur->next);
    if(cur->distanza != arrivo->distanza && cur->distanza+cur->max_autonomia>=arrivo->distanza)
        return cur;
    else
        return NULL;
}
tappe* in_testa(tappe *percorso, stazione *s){
    tappe *n = NULL;
    n = malloc(sizeof(tappe));
    n->riferimento=s;
    n->next=percorso;
    return n;
}

stazione* cercaS(stazione *a, int s){
    while(a->distanza<s)
        a=a->next;
    if(a->distanza==s)
        return a;               // return s; è errore di warning per via del cast
    return NULL;
}







//manca pianificaIndietro

bool pianifica_indietro(stazione *autos,int s1, int s2,FILE *out){
stazione *partenza=NULL, *arrivo=NULL, *par=NULL, *cur=NULL, *pre=NULL, *p=NULL,*h;
tappe *percorso=NULL,*tmp,*canc;
int i=0,fine=0;
partenza=cercaS(autos,s1);
arrivo=cercaS(autos,s2);

percorso=in_testa(percorso,partenza);
i++;


for(par=partenza;fine==0;){
    if(par->distanza-par->max_autonomia<=arrivo->distanza){
        fine=1;
        i++;
        percorso=in_testa(percorso,arrivo);
    }else{
        for(cur=arrivo;par->distanza-par->max_autonomia>cur->distanza&&cur->distanza<par->distanza;cur=cur->next);
        if(cur->distanza!=par->distanza&&par->distanza-par->max_autonomia<=cur->distanza){
            for(p=cur;p->distanza<par->distanza;p=p->next){
                if(p->distanza-p->max_autonomia<cur->distanza-cur->max_autonomia)
                cur=p;
            }
            par=cur;
            i++;
            percorso=in_testa(percorso,par);
        }else{
            fine=-1;
        }
    }
}
if(fine==-1){
    fprintf(out,"nessun percorso\n");
}else{
    aggiornaTappe(percorso);
    stampaP(percorso->next,out);
    fprintf(out,"%d\n",percorso->riferimento->distanza);
    

}

   //libera memoria
    while(percorso){
        canc=percorso;
        percorso=percorso->next;
        free(canc);
    }
}
//i>2
void aggiornaTappe(tappe *percorso){ 
    tappe *f=NULL,*c=NULL, *l=NULL;
    stazione *cur=NULL, *min=NULL;
    if(percorso->next&&percorso->next->next){
        f=percorso;
        c=percorso->next;
        l=percorso->next->next;
        for(cur=f->riferimento,min=NULL;cur->distanza<c->riferimento->distanza&&min==NULL;cur=cur->next){
            //printf("%d ",cur->distanza);
            if(cur->distanza-cur->max_autonomia<=f->riferimento->distanza&&l->riferimento->distanza-l->riferimento->max_autonomia<=cur->distanza)
            min=cur;
        }
        if(min)
        c->riferimento=min;
        aggiornaTappe(c);
    }
}

/*
for(par=partenza;par->distanza!=arrivo->distanza;){
   i++;
if(par->distanza-par->max_autonomia<=arrivo->distanza)
   par=arrivo;
else{
      for(cur=par->prev,pre=NULL;cur->distanza>=par->distanza-par->max_autonomia;pre=cur,cur=cur->prev);
      if(pre==NULL){
         printf("nessun percorso\n");
        return false;
        }else
         par=pre;
    }
percorso=in_testa(percorso,par);
}
printf("%d\n",i);
return true;
//ottimizzazione
*/



/*

int i=0,fine=0;
partenza=cercaS(autos,s1);
arrivo=cercaS(autos,s2);
// percorso=in_testa(percorso,partenza);
for(par=partenza;!fine;){
    i++;
    if(par->distanza-par->max_autonomia<=arrivo->distanza){
       par=arrivo;
       fine=1;
}else{
      for(cur=arrivo;cur->distanza<par->distanza&&(par->distanza-par->max_autonomia)>cur->distanza;cur=cur->next);
      if((par->distanza-par->max_autonomia)>cur->distanza){
        par=cur;
      }else{
            printf("nessun percorso\n");
            return false;
      }
}


*/

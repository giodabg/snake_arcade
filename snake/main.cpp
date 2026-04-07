#include <iostream>
#include <string>
#include <stdlib.h>       // atoi
#include <windows.h>
#include "Griglia.h"
#include "Cella.h"
#include "Tastiera.h"
#include "Snake.h"

using namespace std;

// ============================================================
//  Costanti di gioco
// ============================================================
const int NUM_RIGHE = 20;
const int NUM_COLONNE = 40;
const int ORIGINE_X = 4;
const int ORIGINE_Y = 2;

const char SIMBOLO_ASTERISCO = '*';
const char SIMBOLO_VUOTO = ' ';

const int RITARDO_MS = 50;   // millisecondi tra un frame e il successivo

// Funzione: creaGriglia
// Crea una griglia con bordo e sfondo blu, pronta per essere usata
// dai giochi utilizzando i parametri numRig e numCol 
// per dimensionare grandezza della griglia.
// La posizione iniziale della griglia è definita da ORIGINE_X e ORIGINE_Y.
// Riempie tutta la griglia con spazi su sfondo blu e
// esegue il primo rendering completo per disegnare sfondo e bordo.
// Restituisce la griglia pronta all'uso.
Griglia creaGriglia(int numRig, int numCol)
{
    // ── Impostazione code page UTF-8 per i caratteri del bordo
    SetConsoleOutputCP(850);

    // ── Creazione della griglia con bordo e sfondo blu ────────
    Griglia g = Griglia(numRig, numCol,
        ORIGINE_X, ORIGINE_Y,
        true, FG_BIANCO);

    // Riempie tutta la griglia con spazi su sfondo blu
    g.riempiTutto(SIMBOLO_VUOTO, FG_BIANCO, BG_BLU_I);

    // Primo rendering completo per disegnare sfondo e bordo
    g.renderCompleto();

	// Restituisce la griglia pronta all'uso
	return g;
}

// ============================================================
//  Funzione: scriviAlCentro
//  Visualizza al centro dello schermo il messaggio presente nel parametro.
//  i parametri cp e cs specificano i colori da applicare a ogni carattere.
// ============================================================
void scriviAlCentro(Griglia& g, string msg, WORD cp, WORD cs)
{
    // Riga centrale per il messaggio
    int rigaMsg = NUM_RIGHE / 2;

    // Lunghezza della stringa
    int lunghezza = msg.length();

    // calcola la colonna di inizio affinché la stringa risulti
    //  centrata rispetto alle colonne totali della griglia
    int colMsg = (NUM_COLONNE - lunghezza) / 2;

    // Visualizzazione del testo
    g.scriviStringa(rigaMsg, colMsg, msg, FG_BIANCO, BG_BLU_I);
    g.renderDifferenziale();
}

// ============================================================
// main prova Griglia: sposta un asterisco da sinistra a destra finché esce dallo schermo,
// poi mostra "GAME OVER" al centro e attende un tasto prima di uscire
// ============================================================
int mainGriglia()
{
    Tastiera tastiera;

	// creazione griglia con impostazioni iniziali
    Griglia griglia = creaGriglia(NUM_RIGHE, NUM_COLONNE);

    // ── Posizione iniziale: centro della griglia ──────────────
    int rigaAsterisco = NUM_RIGHE / 2;
    int colonnaAsterisco = NUM_COLONNE / 2;

    // ── Game loop: sposta l'asterisco verso destra ─────────────
    bool fuoriSchermo = false;

    while (fuoriSchermo == false)
    {
        // Disegna l'asterisco giallo su sfondo blu nella posizione corrente
        griglia.impostaCella(rigaAsterisco, colonnaAsterisco,
            SIMBOLO_ASTERISCO,
            FG_GIALLO_I,
            BG_BLU_I);
        griglia.renderDifferenziale();

        // Pausa per rendere visibile il movimento
        Sleep(RITARDO_MS);

        // Cancella la cella corrente (sfondo blu, spazio)
        griglia.impostaCella(rigaAsterisco, colonnaAsterisco,
            SIMBOLO_VUOTO,
            FG_BIANCO,
            BG_BLU_I);

        // Avanza di una colonna verso destra
        colonnaAsterisco = colonnaAsterisco + 1;

        // Controlla se l'asterisco è uscito dalla griglia
        if (griglia.isInterna(rigaAsterisco, colonnaAsterisco) == false)
            fuoriSchermo = true;
    }

    // ── L'asterisco è uscito: mostra GAME OVER ────────────────
	scriviAlCentro(griglia, "GAME OVER", FG_BIANCO, BG_BLU_I);

    // ── Attende pressione di un tasto prima di uscire ─────────────────────
    tastiera.leggiBloccante();

    return 0;
}

int mainSnake()
{
    Tastiera tastiera;

    // creazione griglia con impostazioni iniziali
    Griglia griglia = creaGriglia(NUM_RIGHE, NUM_COLONNE);

    // Lettura nome — stringa libera
    char nome[MAX_LUNGHEZZA_TESTO];
    tastiera.leggiStringa(griglia, "Come ti chiami?", nome, BG_BLU_I);

    // Lettura velocità — intero con default 50 e range [50, 100]
    int ritardo = tastiera.leggiInt(griglia,
        "Tempo in mill. tra due frame (50-200):",
        50,
        50, 200,
        BG_BLU_I);

    // se il ritardo è troppo piccolo, si imposta al minimo per evitare che il gioco sia ingiocabile
    if (ritardo < RITARDO_MS)
        ritardo = RITARDO_MS;
    
    // Pulizia della griglia prima di iniziare la partita
    griglia.riempiTutto(' ', FG_BIANCO, BG_BLU_I);
    griglia.renderCompleto();


    // ── Posizione iniziale: centro della griglia ──────────────
    int rigaSnake = NUM_RIGHE / 2;
    int colonnaSnake = NUM_COLONNE / 2;
    // Snake al centro, angolo 90°, velocità 0.4 misurata in celle/frame
    Snake snake(colonnaSnake, rigaSnake, 0.4f, 90.0f,
        '@', FG_GIALLO, BG_BLU_I);

    // ── Game loop: sposta lo snake ─────────────
    bool partitaInCorso = true;
    while (partitaInCorso == true)
    {
        // ── mostra nome giocatore e velocità ────────────────
        griglia.scriviStringa(2, 2, nome, FG_BIANCO, BG_BLU_I);
        griglia.scriviStringa(3, 2, to_string(ritardo), FG_BIANCO, BG_BLU_I);
        griglia.renderDifferenziale();

		// Aggiorna la posizione dello snake in base alla sua velocità e direzione
		// usando la griglia per verificare collisioni e limiti
        snake.muovi(griglia);

		// Disegna lo snake nella nuova posizione
        snake.disegna(griglia, BG_BLU_I);
        griglia.renderDifferenziale();

        // Pausa per rendere visibile il movimento
        Sleep(ritardo);

		int tasto = tastiera.leggi(); // lettura non bloccante: restituisce -1 se non c'è tasto in attesa
        if (tastiera.isFreccia(tasto) == true)
        {
            if (tastiera.uguale(TASTO_SU))  
                snake.impostaAngolo(270);
            if (tastiera.uguale(TASTO_GIU))
                snake.impostaAngolo(90);
            if (tastiera.uguale(TASTO_SINISTRA))
                snake.impostaAngolo(180);
            if (tastiera.uguale(TASTO_DESTRA))
                snake.impostaAngolo(0);
        }
        if (tastiera.uguale(TASTO_ESC))
            partitaInCorso = false;
    }

    // ── Fine del gioco: mostra GAME OVER ────────────────
    scriviAlCentro(griglia, "GAME OVER", FG_BIANCO, BG_BLU_I);

    // ── Attende pressione di un tasto prima di uscire ─────────────────────
    tastiera.leggiBloccante();

    return 0;
}

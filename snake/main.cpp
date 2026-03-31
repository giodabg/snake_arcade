#include <iostream>
#include <string>
#include <windows.h>
#include "Griglia.h"
#include "Cella.h"

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

const int VELOCITA_MS = 100;   // millisecondi tra un frame e il successivo

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
//  Funzione: calcolaCentroStringa
//  Restituisce la colonna di inizio affinché la stringa risulti
//  centrata rispetto alle colonne totali della griglia.
// ============================================================
int calcolaCentroStringa(int colonneGriglia, int lunghezzaTesto)
{
    return (colonneGriglia - lunghezzaTesto) / 2;
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

    int colMsg = calcolaCentroStringa(NUM_COLONNE, lunghezza);

    // Visualizzazione del testo
    g.scriviStringa(rigaMsg, colMsg,
        msg, FG_BIANCO, BG_BLU_I);
    g.renderDifferenziale();

}

// ============================================================
// Funzione: attendiTasto
// Visualizza sullo schermo il messaggio "Premi un tasto per uscire ..."
// e termina quando l'utente preme un tasto seguito da INVIO
// ============================================================
void attendiTasto() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD  posCursore;
    posCursore.X = ORIGINE_X;
    posCursore.Y = ORIGINE_Y + NUM_RIGHE + 3;
    SetConsoleCursorPosition(hConsole, posCursore);

    SetConsoleTextAttribute(hConsole,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << "  Premi un tasto per uscire..." << endl;

    // Consuma l'eventuale invio residuo nel buffer
    cin.sync();
    cin.get();
}

// ============================================================
//  main
// ============================================================
int main()
{
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
        Sleep(VELOCITA_MS);

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
    attendiTasto();

    return 0;
}

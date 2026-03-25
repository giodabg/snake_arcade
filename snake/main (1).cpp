#include <iostream>
#include <windows.h>
#include "Griglia.h"
#include "Cella.h"

// ============================================================
//  Costanti di gioco
// ============================================================
#define NUM_RIGHE   20
#define NUM_COLONNE 40
#define ORIGINE_X    4
#define ORIGINE_Y    2

#define COLORE_SFONDO_BLU  (BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define COLORE_ASTERISCO   (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COLORE_GAME_OVER   (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define COLORE_DEFAULT     (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

#define SIMBOLO_ASTERISCO  '*'
#define SIMBOLO_VUOTO      ' '

#define VELOCITA_MS        100   // millisecondi tra un frame e il successivo


// ============================================================
//  Funzione: scriviStringa
//  Scrive una stringa carattere per carattere sulla griglia,
//  a partire dalla colonna 'colStart', sulla riga 'r'.
//  Usata per visualizzare "GAME OVER" al centro della griglia.
// ============================================================
void scriviStringa(Griglia& g, int r, int colStart,
                   const char* testo, WORD cp, WORD cs)
{
    int i = 0;
    while (testo[i] != '\0')
    {
        if (g.isInterna(r, colStart + i) == true)
            g.impostaCella(r, colStart + i, testo[i], cp, cs);
        i = i + 1;
    }
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
//  main
// ============================================================
int main()
{
    // ── Impostazione code page UTF-8 per i caratteri del bordo
    SetConsoleOutputCP(850);

    // ── Creazione della griglia con bordo e sfondo blu ────────
    Griglia griglia(NUM_RIGHE, NUM_COLONNE,
                    ORIGINE_X, ORIGINE_Y,
                    true, COLORE_DEFAULT);

    // Riempie tutta la griglia con spazi su sfondo blu
    griglia.riempiTutto(SIMBOLO_VUOTO, COLORE_DEFAULT, COLORE_SFONDO_BLU);

    // Primo rendering completo per disegnare sfondo e bordo
    griglia.renderCompleto();

    // ── Posizione iniziale: centro della griglia ──────────────
    int rigaAsterisco    = NUM_RIGHE   / 2;
    int colonnaAsterisco = NUM_COLONNE / 2;

    // ── Game loop: sposta l'asterisco verso destra ─────────────
    bool fuoriSchermo = false;

    while (fuoriSchermo == false)
    {
        // Disegna l'asterisco giallo su sfondo blu nella posizione corrente
        griglia.impostaCella(rigaAsterisco, colonnaAsterisco,
                             SIMBOLO_ASTERISCO,
                             COLORE_ASTERISCO,
                             COLORE_SFONDO_BLU);

        griglia.renderDifferenziale();

        // Pausa per rendere visibile il movimento
        Sleep(VELOCITA_MS);

        // Cancella la cella corrente (sfondo blu, spazio)
        griglia.impostaCella(rigaAsterisco, colonnaAsterisco,
                             SIMBOLO_VUOTO,
                             COLORE_DEFAULT,
                             COLORE_SFONDO_BLU);

        // Avanza di una colonna verso destra
        colonnaAsterisco = colonnaAsterisco + 1;

        // Controlla se l'asterisco è uscito dalla griglia
        if (griglia.isInterna(rigaAsterisco, colonnaAsterisco) == false)
            fuoriSchermo = true;
    }

    // ── L'asterisco è uscito: mostra GAME OVER ────────────────

    // Riga centrale per il messaggio
    int rigaMsg    = NUM_RIGHE / 2;
    const char* msg = "GAME OVER";

    // Lunghezza manuale della stringa (strlen non richiesto)
    int lunghezza = 0;
    while (msg[lunghezza] != '\0')
        lunghezza = lunghezza + 1;

    int colMsg = calcolaCentroStringa(NUM_COLONNE, lunghezza);

    // Riga superiore decorativa  ---------
    scriviStringa(griglia, rigaMsg - 2, colMsg - 2,
                  "---------",
                  COLORE_GAME_OVER, COLORE_SFONDO_BLU);

    // Riga del testo  | GAME OVER |
    griglia.impostaCella(rigaMsg, colMsg - 2,
                         '|', COLORE_GAME_OVER, COLORE_SFONDO_BLU);

    scriviStringa(griglia, rigaMsg, colMsg,
                  msg, COLORE_GAME_OVER, COLORE_SFONDO_BLU);

    griglia.impostaCella(rigaMsg, colMsg + lunghezza + 1,
                         '|', COLORE_GAME_OVER, COLORE_SFONDO_BLU);

    // Riga inferiore decorativa  ---------
    scriviStringa(griglia, rigaMsg + 2, colMsg - 2,
                  "---------",
                  COLORE_GAME_OVER, COLORE_SFONDO_BLU);

    griglia.renderDifferenziale();

    // ── Attende un tasto prima di uscire ─────────────────────
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD  posCursore;
    posCursore.X = ORIGINE_X;
    posCursore.Y = ORIGINE_Y + NUM_RIGHE + 3;
    SetConsoleCursorPosition(hConsole, posCursore);

    SetConsoleTextAttribute(hConsole,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "  Premi un tasto per uscire..." << std::endl;

    // Consuma l'eventuale invio residuo nel buffer
    std::cin.sync();
    std::cin.get();

    return 0;
}

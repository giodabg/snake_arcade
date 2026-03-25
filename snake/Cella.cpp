#include "Cella.h"
#include <iostream>

using namespace std;

// ============================================================
//  Colori di default usati in tutto il file
// ============================================================
#define COLORE_DEFAULT (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define SFONDO_DEFAULT 0


// ── Costruttori ──────────────────────────────────────────────

Cella::Cella()
{
    forma = ' ';
    colorePrimeroPiano = COLORE_DEFAULT;
    coloreSfondo = SFONDO_DEFAULT;
    visibile = true;
    riga = 0;
    colonna = 0;
}

Cella::Cella(int r, int c, char f, WORD cp, WORD cs, bool vis)
{
    riga = r;
    colonna = c;
    forma = f;
    colorePrimeroPiano = cp;
    coloreSfondo = cs;
    visibile = vis;
}


// ── Setter ───────────────────────────────────────────────────

void Cella::impostaContenuto(char f, WORD cp, WORD cs)
{
    forma = f;
    colorePrimeroPiano = cp;
    coloreSfondo = cs;
}

void Cella::impostaForma(char f)
{
    forma = f;
}

void Cella::impostaColori(WORD cp, WORD cs)
{
    colorePrimeroPiano = cp;
    coloreSfondo = cs;
}

void Cella::impostaVisibilita(bool vis)
{
    visibile = vis;
}

void Cella::impostaPosizione(int r, int c)
{
    riga = r;
    colonna = c;
}

void Cella::svuota()
{
    forma = ' ';
    colorePrimeroPiano = COLORE_DEFAULT;
    coloreSfondo = SFONDO_DEFAULT;
    visibile = true;
}


// ── Getter ───────────────────────────────────────────────────

char Cella::getForma()
{
    return forma;
}

WORD Cella::getAttributoColore()
{
    // Combina i due attributi WinAPI in un unico WORD a 16 bit:
    // i 4 bit bassi codificano il colore del carattere,
    // i 4 bit alti codificano il colore di sfondo.
    return colorePrimeroPiano | coloreSfondo;
}

bool Cella::isVisibile()
{
    return visibile;
}

int Cella::getRiga()
{
    return riga;
}

int Cella::getColonna()
{
    return colonna;
}


// ── Rendering ────────────────────────────────────────────────

void Cella::visualizza(HANDLE hConsole)
{
    // Se la cella non è visibile si stampa uno spazio
    // con i colori di default senza alterare il contesto grafico.
    if (visibile == false)
    {
        SetConsoleTextAttribute(hConsole, COLORE_DEFAULT);
        cout << ' ';
    }
    else
    {
        SetConsoleTextAttribute(hConsole, getAttributoColore());
        cout << forma;
    }

    // Ripristina il colore di default dopo ogni cella per non
    // "inquinare" le stampe successive (es. messaggi di stato).
    SetConsoleTextAttribute(hConsole, COLORE_DEFAULT);
}
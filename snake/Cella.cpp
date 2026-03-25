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
    this->forma = ' ';
    this->colorePrimeroPiano = COLORE_DEFAULT;
    this->coloreSfondo = SFONDO_DEFAULT;
    this->visibile = true;
    this->riga = 0;
    this->colonna = 0;
}

Cella::Cella(int r, int c, char f, WORD cp, WORD cs, bool vis)
{
    this->riga = r;
    this->colonna = c;
    this->forma = f;
    this->colorePrimeroPiano = cp;
    this->coloreSfondo = cs;
    this->visibile = vis;
}


// ── Setter ───────────────────────────────────────────────────

void Cella::impostaContenuto(char f, WORD cp, WORD cs)
{
    this->forma = f;
    this->colorePrimeroPiano = cp;
    this->coloreSfondo = cs;
}

void Cella::impostaForma(char f)
{
    this->forma = f;
}

void Cella::impostaColori(WORD cp, WORD cs)
{
    this->colorePrimeroPiano = cp;
    this->coloreSfondo = cs;
}

void Cella::impostaVisibilita(bool vis)
{
    this->visibile = vis;
}

void Cella::impostaPosizione(int r, int c)
{
    this->riga = r;
    this->colonna = c;
}

void Cella::svuota()
{
    this->forma = ' ';
    this->colorePrimeroPiano = COLORE_DEFAULT;
    this->coloreSfondo = SFONDO_DEFAULT;
    this->visibile = true;
}


// ── Getter ───────────────────────────────────────────────────

char Cella::getForma()
{
    return this->forma;
}

WORD Cella::getAttributoColore()
{
    // Combina i due attributi WinAPI in un unico WORD a 16 bit:
    // i 4 bit bassi codificano il colore del carattere,
    // i 4 bit alti codificano il colore di sfondo.
    return this->colorePrimeroPiano | this->coloreSfondo;
}

bool Cella::isVisibile()
{
    return this->visibile;
}

int Cella::getRiga()
{
    return this->riga;
}

int Cella::getColonna()
{
    return this->colonna;
}


// ── Rendering ────────────────────────────────────────────────

void Cella::visualizza(HANDLE hConsole)
{
    // Se la cella non è this->visibile si stampa uno spazio
    // con i colori di default senza alterare il contesto grafico.
    if (this->visibile == false)
    {
        SetConsoleTextAttribute(hConsole, COLORE_DEFAULT);
        cout << ' ';
    }
    else
    {
        SetConsoleTextAttribute(hConsole, getAttributoColore());
        cout << this->forma;
    }

    // Ripristina il colore di default dopo ogni cella per non
    // "inquinare" le stampe successive (es. messaggi di stato).
    SetConsoleTextAttribute(hConsole, COLORE_DEFAULT);
}
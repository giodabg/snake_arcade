#include "Snake.h"
#include <math.h>

#define PI 3.14159265f
#define COLORE_DEFAULT (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)


// ── Costruttori ──────────────────────────────────────────────

Snake::Snake()
{
    this->x = 0.0f;
    this->y = 0.0f;
    this->dx = 0.0f;
    this->dy = 0.0f;
    this->velocita = 0.0f;
    this->forma = 'O';
    this->coloreForma = COLORE_DEFAULT;
    this->coloreSfondo = 0;
    this->rigaPrec = 0;
    this->colonnaPrec = 0;
}

Snake::Snake(float startX, float startY,
    float vel, float angoloDeg,
    char f, WORD cp, WORD cs)
{
    this->x = startX;
    this->y = startY;
    this->velocita = vel;
    this->forma = f;
    this->coloreForma = cp;
    this->coloreSfondo = cs;
    this->rigaPrec = (int)(y + 0.5f);
    this->colonnaPrec = (int)(x + 0.5f);

    impostaAngolo(angoloDeg);
}


// ── Movimento ────────────────────────────────────────────────

void Snake::aggiorna()
{
    // Salva posizione intera corrente prima di muoversi
    rigaPrec = (int)(this->y + 0.5f);
    colonnaPrec = (int)(this->x + 0.5f);

    this->x = this->x + this->dx;
    this->y = this->y + this->dy;
}

void Snake::impostaAngolo(float angoloDeg)
{
    // Converte gradi in radianti
    float rad = angoloDeg * PI / 180.0f;

    // dx ridotto del RAPPORTO_CELLA per compensare la forma
    // rettangolare dei caratteri (più alti che larghi).
    // Senza questa correzione un angolo di 45° apparirebbe
    // molto più ripido di quanto dovrebbe essere visivamente.
    this->dx = cos(rad) * this->velocita * RAPPORTO_CELLA;
    this->dy = sin(rad) * this->velocita;
}

void Snake::rimbalzaX()
{
    this->dx = - this->dx;
}

void Snake::rimbalzaY()
{
    this->dy = - this->dy;
}

void Snake::accelera(float delta)
{
    // Recupera l'angolo corrente dalla direzione di dx/dy,
    // aumenta il modulo e ricalcola i componenti.
    // atan2 restituisce l'angolo in radianti da dy e dx.
    // Nota: dx è già scalato per RAPPORTO_CELLA, quindi
    // per ricavare l'angolo reale lo si divide prima.
    float dxReale = this->dx / RAPPORTO_CELLA;
    float angolo = atan2(this->dy, dxReale);

    this->velocita = this->velocita + delta;

    this->dx = cos(angolo) * this->velocita * RAPPORTO_CELLA;
    this->dy = sin(angolo) * this->velocita;
}


// ── Disegno sulla griglia ─────────────────────────────────────

void Snake::disegna(Griglia& g, WORD sfondoCs)
{
    int rigaCorrente = (int)(this->y + 0.5f);
    int colonnaCorrente = (int)(this->x + 0.5f);

    // Cancella la cella del frame precedente solo se l'oggetto
    // si è effettivamente spostato su una cella diversa
    if (this->rigaPrec != rigaCorrente || this->colonnaPrec != colonnaCorrente)
    {
        if (g.isInterna(this->rigaPrec, this->colonnaPrec) == true)
            g.impostaCella(this->rigaPrec, this->colonnaPrec,
                ' ', COLORE_DEFAULT, sfondoCs);
    }

    // Disegna nella nuova posizione
    if (g.isInterna(rigaCorrente, colonnaCorrente) == true)
        g.impostaCella(rigaCorrente, colonnaCorrente,
            this->forma, this->coloreForma, this->coloreSfondo);
}


// ── Getter ────────────────────────────────────────────────────

float Snake::getX() { return this->x; }
float Snake::getY() { return this->y; }
int   Snake::getRiga() { return (int)(this->y + 0.5f); }
int   Snake::getColonna() { return (int)(this->x + 0.5f); }
float Snake::getVelocita() { return this->velocita; }


// ── Setter ────────────────────────────────────────────────────

void Snake::impostaPosizione(float nuovaX, float nuovaY)
{
    this->x = nuovaX;
    this->y = nuovaY;
}

void Snake::impostaVelocita(float vel)
{
    float dxReale = this->dx / RAPPORTO_CELLA;
    float angolo = atan2(this->dy, dxReale);

    this->velocita = vel;

    this->dx = cos(angolo) * this->velocita * RAPPORTO_CELLA;
    this->dy = sin(angolo) * this->velocita;
}

void Snake::impostaAspetto(char f, WORD cp, WORD cs)
{
    this->forma = f;
    this->coloreForma = cp;
    this->coloreSfondo = cs;
}
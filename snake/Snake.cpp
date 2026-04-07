#include "Snake.h"
#include <math.h>
#include <iostream>

#define PI 3.14159265f
#define COLORE_DEFAULT (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)


// ── Costruttori ──────────────────────────────────────────────

Snake::Snake()
{
    this->x = 0.0f;
    this->y = 0.0f;
    this->spostX = 0.0f;
    this->spostY = 0.0f;
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
	this->rigaPrec = (int)(this->y + 0.5f);    // Arrotonda alla cella più vicina
	this->colonnaPrec = (int)(this->x + 0.5f); // Arrotonda alla cella più vicina

    impostaAngolo(angoloDeg);
}


// ── Movimento ────────────────────────────────────────────────

void Snake::muovi(Griglia g)
{
    float nuovaY = this->y + this->spostY;
    float nuovaX = this->x + this->spostX;
    if (nuovaX >= 0 && nuovaX <= g.getColonne()-1 &&
        nuovaY >= 0 && nuovaY <= g.getRighe()-1)
    {
        // Salva posizione intera corrente prima di muoversi
        rigaPrec = (int)(this->y + 0.5f);     // Arrotonda alla cella più vicina
		colonnaPrec = (int)(this->x + 0.5f);  // Arrotonda alla cella più vicina
         
        this->x = this->x + this->spostX;
        this->y = this->y + this->spostY;
    }
    else {
		// se ci si trova fuori dallo schermo, si corregge la posizione e si rimbalza
        if (nuovaY < 0) {
            this->y = 0;
            rimbalzaY();
        }	
        else if (nuovaY > g.getRighe()-1) {
            this->y = g.getRighe()-1;
            rimbalzaY();
        }
        if (nuovaX < 0) {
            this->x = 0;
            rimbalzaX();
        }
        if (nuovaX > g.getColonne()-1) {
            this->x = g.getColonne()-1;
            rimbalzaX();
        }
    }
}

void Snake::impostaAngolo(float angoloDeg)
{
    // Converte gradi in radianti
    float rad = angoloDeg * PI / 180.0f;

    this->spostX = cos(rad) * this->velocita;
    this->spostY = sin(rad) * this->velocita;
}

void Snake::rimbalzaX()
{
    this->spostX = -this->spostX;
}

void Snake::rimbalzaY()
{
    this->spostY = -this->spostY;
}

void Snake::accelera(float delta)
{
    // Recupera l'angolo corrente dalla direzione di dx/dy,
    // aumenta il modulo e ricalcola i componenti.
    // atan2 restituisce l'angolo in radianti da dy e dx.
    float dxReale = this->spostX;
    float angolo = atan2(this->spostY, dxReale);

    this->velocita = this->velocita + delta;

    this->spostX = cos(angolo) * this->velocita;
    this->spostY = sin(angolo) * this->velocita;
}


// ── Disegno sulla griglia ─────────────────────────────────────

void Snake::disegna(Griglia& g, WORD sfondoCs)
{
	int rigaCorrente = (int)(this->y + 0.5f);     // Arrotonda alla cella più vicina
	int colonnaCorrente = (int)(this->x + 0.5f);  // Arrotonda alla cella più vicina

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
int   Snake::getRiga() { return (int)(this->y + 0.5f); }    // Arrotonda alla cella più vicina
int   Snake::getColonna() { return (int)(this->x + 0.5f); } // Arrotonda alla cella più vicina
float Snake::getVelocita() { return this->velocita; }


// ── Setter ────────────────────────────────────────────────────

void Snake::impostaPosizione(Griglia& g, float nuovaX, float nuovaY)
{
    if (nuovaX >= 0 && nuovaX <= g.getColonne() - 1 &&
        nuovaY >= 0 && nuovaY <= g.getRighe() - 1) {
        this->x = nuovaX;
        this->y = nuovaY;
    }
}

void Snake::impostaVelocita(float vel)
{
    float dxReale = this->spostX;
    float angolo = atan2(this->spostY, dxReale);

    this->velocita = vel;

    this->spostX = cos(angolo) * this->velocita;
    this->spostY = sin(angolo) * this->velocita;
}

void Snake::impostaAspetto(char f, WORD cp, WORD cs)
{
    this->forma = f;
    this->coloreForma = cp;
    this->coloreSfondo = cs;
}
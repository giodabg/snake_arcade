#pragma once
#include "Griglia.h"
#include <math.h>

// ============================================================
//  OGGETTO MOBILE SNAKE
//  Rappresenta qualsiasi elemento di gioco che si sposta sulla
//  griglia con posizione reale (float) e velocità direzionale.
//  Esempi: pallina Pong, asteroide, proiettile, fantasma, ...
//
//  La posizione float viene arrotondata a intero solo nel
//  momento del disegno sulla griglia, ottenendo un movimento
//  fluido a qualsiasi angolo pur rimanendo su celle discrete.
// ============================================================

// Rapporto altezza/larghezza di un carattere della console Windows.
// I caratteri sono alti circa il doppio rispetto alla larghezza:
// dimezzare dx compensa la distorsione e rende il moto a 45°
// visivamente corretto (angolo reale = angolo percepito).
const float RAPPORTO_CELLA = 0.5f;

class Snake {
private:

    // ── Posizione reale (float) ───────────────────────────────

    float x;
    // Colonna reale dell'oggetto in coordinate float.
    // Permette spostamenti sub-cella: l'oggetto avanza
    // anche quando il passo è inferiore a 1 cella per frame.

    float y;
    // Riga reale dell'oggetto in coordinate float.

    // ── Velocità direzionale ─────────────────────────────────

    float dx;
    // Componente orizzontale della velocità (colonne per frame).
    // Già corretta per il RAPPORTO_CELLA: va inizializzata
    // tramite impostaAngolo() per ottenere la correzione automatica.

    float dy;
    // Componente verticale della velocità (righe per frame).

    float velocita;
    // Modulo della velocità in celle per frame.
    // Separato da dx/dy per poter variare la rapidità senza
    // alterare la direzione (es. accelerazione progressiva nel Pong).

    // ── Aspetto visivo ────────────────────────────────────────

    char forma;
    // Carattere usato per disegnare l'oggetto sulla griglia.

    WORD coloreForma;
    // Colore WinAPI del carattere (primo piano).

    WORD coloreSfondo;
    // Colore WinAPI dello sfondo della cella occupata.

    // ── Posizione precedente (per la cancellazione) ───────────

    int rigaPrec;
    // Riga intera occupata nel frame precedente.
    // Serve a svuotare la cella prima di disegnare quella nuova,
    // senza dover svuotare l'intera griglia ad ogni frame.

    int colonnaPrec;
    // Colonna intera occupata nel frame precedente.

public:

    // ── Costruttori ──────────────────────────────────────────

    // Costruttore di default: oggetto in (0,0), fermo, simbolo 'O'.
    Snake();

    // Costruttore completo.
    // Parametri:
    //   startX, startY : posizione iniziale float
    //   vel            : modulo della velocità (celle/frame)
    //   angoloDeg      : direzione in gradi (0° = destra, 90° = giù)
    //   f              : carattere da visualizzare
    //   cp             : colore primo piano WinAPI
    //   cs             : colore sfondo WinAPI
    Snake(float startX, float startY,
        float vel, float angoloDeg,
        char f, WORD cp, WORD cs);

    // ── Movimento ────────────────────────────────────────────

    // Aggiorna x e y aggiungendo dx e dy.
    // Da chiamare una volta per frame nel game loop.
    void aggiorna();

    // Imposta la direzione di movimento a partire da un angolo
    // in gradi. Calcola automaticamente dx e dy applicando il
    // RAPPORTO_CELLA su dx per correggere la distorsione dei caratteri.
    // angoloDeg: 0° = destra, 90° = basso, 180° = sinistra, 270° = alto
    void impostaAngolo(float angoloDeg);

    // Inverte la componente orizzontale (rimbalzo su parete verticale).
    void rimbalzaX();

    // Inverte la componente verticale (rimbalzo su parete orizzontale).
    void rimbalzaY();

    // Aumenta il modulo della velocità di 'delta' celle/frame,
    // ricalcolando dx e dy mantenendo la direzione corrente.
    // Usata per l'accelerazione progressiva stile Pong.
    void accelera(float delta);

    // ── Disegno sulla griglia ─────────────────────────────────

    // Cancella la cella occupata nel frame precedente e disegna
    // l'oggetto nella nuova posizione arrotondata.
    // Il parametro sfondoCs è il colore sfondo usato per la
    // cella di "cancellazione" (deve coincidere con lo sfondo
    // della griglia per non lasciare tracce).
    void disegna(Griglia& g, WORD sfondoCs);

    // ── Getter ────────────────────────────────────────────────

    // Restituisce la colonna reale (float) corrente.
    float getX();

    // Restituisce la riga reale (float) corrente.
    float getY();

    // Restituisce la riga intera arrotondata (posizione sulla griglia).
    int getRiga();

    // Restituisce la colonna intera arrotondata (posizione sulla griglia).
    int getColonna();

    // Restituisce il modulo della velocità corrente.
    float getVelocita();

    // ── Setter ────────────────────────────────────────────────

    // Imposta direttamente posizione e velocità (es. reset pallina).
    void impostaPosizione(float nuovaX, float nuovaY);

    // Imposta il modulo della velocità ricalcolando dx e dy.
    void impostaVelocita(float vel);

    // Imposta il carattere e i colori dell'oggetto.
    void impostaAspetto(char f, WORD cp, WORD cs);
};

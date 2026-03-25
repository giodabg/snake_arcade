#pragma once
#include <windows.h>

// ============================================================
//  CELLA — unità atomica della griglia
//  Ogni cella occupa una posizione (riga, colonna) nella griglia
//  e visualizza un singolo carattere con colore di primo piano
//  e di sfondo, impostati dal componente di gioco che la occupa.
// ============================================================

class Cella {
private:

    // ── Contenuto visivo ─────────────────────────────────────

    char forma;
    // Carattere che rappresenta l'elemento di gioco ospitato
    // (es. 'O' = Snake, '*' = mela, '#' = muro, ' ' = vuoto).

    WORD colorePrimeroPiano;
    // Colore del carattere, espresso come costante WinAPI
    // (es. FOREGROUND_GREEN | FOREGROUND_INTENSITY).

    WORD coloreSfondo;
    // Colore di sfondo della cella, espresso come costante WinAPI
    // (es. BACKGROUND_BLUE). Combinato con colorePrimeroPiano
    // forma l'attributo WORD passato a SetConsoleTextAttribute().

    bool visibile;
    // Flag che indica se la cella deve essere renderizzata.
    // Se false, la cella viene stampata come spazio neutro,
    // consentendo effetti di lampeggio o mascheratura.

    int riga;
    // Coordinata di riga all'interno della griglia (0-based).
    // Usata dalla Griglia per posizionare il cursore console.

    int colonna;
    // Coordinata di colonna all'interno della griglia (0-based).

public:

    // ── Costruttori ──────────────────────────────────────────

    // Costruttore di default: cella vuota (spazio), bianca su nero,
    // posizionata in (0,0) e visibile.
    Cella();

    // Costruttore completo: inizializza tutti gli attributi.
    // Parametri:
    //   r   : riga nella griglia
    //   c   : colonna nella griglia
    //   f   : carattere da visualizzare
    //   cp  : colore primo piano (costante WinAPI)
    //   cs  : colore sfondo     (costante WinAPI)
    //   vis : visibilità iniziale
    Cella(int r, int c, char f, WORD cp, WORD cs, bool vis);

    // ── Setter — usati dai componenti di gioco ───────────────

    // Imposta il carattere e i colori in un'unica chiamata.
    // I componenti di gioco (Snake, Pacman, …) chiamano questo
    // metodo per disegnare il proprio elemento sulla cella.
    void impostaContenuto(char f, WORD cp, WORD cs);

    // Sovrascrive solo il carattere, mantenendo i colori attuali.
    void impostaForma(char f);

    // Sovrascrive solo i colori, mantenendo il carattere attuale.
    void impostaColori(WORD cp, WORD cs);

    // Abilita o disabilita la visualizzazione della cella.
    void impostaVisibilita(bool vis);

    // Imposta la posizione della cella nella griglia.
    // Chiamato dalla Griglia durante l'inizializzazione della matrice.
    void impostaPosizione(int r, int c);

    // Ripristina la cella allo stato vuoto:
    // spazio, colori default (bianco su nero), visibile = true.
    void svuota();

    // ── Getter ───────────────────────────────────────────────

    // Restituisce il carattere contenuto nella cella.
    char getForma();

    // Restituisce l'attributo colore WinAPI combinato
    // (primo piano | sfondo), pronto per SetConsoleTextAttribute().
    WORD getAttributoColore();

    // Restituisce true se la cella è visibile.
    bool isVisibile();

    // Restituisce la riga della cella nella griglia.
    int getRiga();

    // Restituisce la colonna della cella nella griglia.
    int getColonna();

    // ── Rendering ────────────────────────────────────────────

    // Stampa il singolo carattere della cella sulla console,
    // applicando i colori tramite SetConsoleTextAttribute().
    // NON sposta il cursore: è compito della Griglia farlo prima.
    // Se visibile == false, stampa uno spazio con colori default.
    void visualizza(HANDLE hConsole);
};
#pragma once
#include "Cella.h"
#include <windows.h>
#include <string>

using namespace std;

// ============================================================
//  Costanti dimensionali della griglia
//  Definite qui per dimensionare la matrice statica.
//  I giochi che richiedono griglie diverse ridefiniscono
//  queste costanti prima di includere questo header.
// ============================================================

const int MAX_RIGHE = 80;
const int MAX_COLONNE = 60;

// ============================================================
//  Colori FOREGROUND (primo piano — colore del carattere)
// ============================================================

const WORD FG_NERO = 0;
const WORD FG_BLU = FOREGROUND_BLUE;
const WORD FG_VERDE = FOREGROUND_GREEN;
const WORD FG_CIANO = FOREGROUND_BLUE | FOREGROUND_GREEN;
const WORD FG_ROSSO = FOREGROUND_RED;
const WORD FG_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE;
const WORD FG_GIALLO = FOREGROUND_RED | FOREGROUND_GREEN;
const WORD FG_BIANCO = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

// Varianti intense (luminose)
const WORD FG_GRIGIO = FOREGROUND_INTENSITY;
const WORD FG_BLU_I = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD FG_VERDE_I = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD FG_CIANO_I = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD FG_ROSSO_I = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD FG_MAGENTA_I = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD FG_GIALLO_I = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD FG_BIANCO_I = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

// ============================================================
//  Colori BACKGROUND (sfondo della cella)
// ============================================================

const WORD BG_NERO = 0;
const WORD BG_BLU = BACKGROUND_BLUE;
const WORD BG_VERDE = BACKGROUND_GREEN;
const WORD BG_CIANO = BACKGROUND_BLUE | BACKGROUND_GREEN;
const WORD BG_ROSSO = BACKGROUND_RED;
const WORD BG_MAGENTA = BACKGROUND_RED | BACKGROUND_BLUE;
const WORD BG_GIALLO = BACKGROUND_RED | BACKGROUND_GREEN;
const WORD BG_BIANCO = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;

// Varianti intense (luminose)
const WORD BG_GRIGIO = BACKGROUND_INTENSITY;
const WORD BG_BLU_I = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
const WORD BG_VERDE_I = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
const WORD BG_CIANO_I = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
const WORD BG_ROSSO_I = BACKGROUND_RED | BACKGROUND_INTENSITY;
const WORD BG_MAGENTA_I = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
const WORD BG_GIALLO_I = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
const WORD BG_BIANCO_I = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;

// ============================================================
//  Alias semantici — usati nei giochi per chiarezza
// ============================================================

const WORD FG_ARANCIONE = FG_ROSSO_I;      // arancione non esiste in WinAPI:
// rosso intenso è la migliore approssimazione
const WORD FG_ORO = FG_GIALLO;        // giallo scuro → effetto oro
const WORD FG_DEFAULT = FG_BIANCO;        // colore testo di default
const WORD BG_DEFAULT = BG_NERO;          // sfondo di default


// ============================================================
//  GRIGLIA — display testuale unico per tutti i giochi
//
//  Gestisce una matrice righe × colonne di oggetti Cella.
//  I componenti di gioco (Snake, Pacman, …) scrivono sulle celle
//  tramite impostaCella(); la Griglia si occupa esclusivamente
//  di renderizzare la scena sulla console Windows.
//
//  Politica di coordinate:
//    - (0,0) è in alto a sinistra
//    - riga cresce verso il basso, colonna verso destra
//    - l'origine della griglia sullo schermo è definita da
//      (origineX, origineY) in caratteri dalla posizione (0,0)
//      della finestra console
// ============================================================

class Griglia {
private:

    // ── Dimensioni effettive ──────────────────────────────────

    int righe;
    // Numero di righe effettivamente usate dalla partita corrente.
    // Deve essere <= MAX_RIGHE.

    int colonne;
    // Numero di colonne effettivamente usate dalla partita corrente.
    // Deve essere <= MAX_COLONNE.

    // ── Matrice di celle ─────────────────────────────────────

    Cella celle[MAX_RIGHE][MAX_COLONNE];
    // Matrice statica di oggetti Cella dimensionata con le costanti
    // MAX_RIGHE e MAX_COLONNE. Solo le posizioni [0..righe-1][0..colonne-1]
    // vengono utilizzate e renderizzate durante il gioco.

    // ── Maschera di aggiornamento ────────────────────────────

    bool cellaModificata[MAX_RIGHE][MAX_COLONNE];
    // Maschera booleana parallela alla matrice celle[][].
    // Un elemento è true se la cella corrispondente è stata
    // modificata dall'ultimo rendering. Usata dal rendering
    // differenziale per ridisegnare solo le celle cambiate,
    // eliminando lo sfarfallio tipico della console Windows.

    // ── Bordo decorativo ─────────────────────────────────────

    bool bordoAbilitato;
    // Se true, attorno alla griglia viene disegnato un bordo
    // con i caratteri box-drawing Unicode (╔ ═ ╗ ║ ╚ ╝).

    WORD coloreBordo;
    // Colore WinAPI applicato al bordo decorativo
    // (es. FOREGROUND_CYAN | FOREGROUND_INTENSITY).

    // ── Posizione della griglia sullo schermo ────────────────

    int origineX;
    // Colonna (in caratteri) della console da cui inizia la griglia.
    // Permette di centrare la griglia o affiancare più griglie.

    int origineY;
    // Riga (in caratteri) della console da cui inizia la griglia.

    // ── Handle e stato della console Windows ─────────────────

    HANDLE hConsole;
    // Handle alla console ottenuto con GetStdHandle(STD_OUTPUT_HANDLE).
    // Usato da tutte le operazioni di colore e posizionamento cursore.

    CONSOLE_SCREEN_BUFFER_INFO infoConsole;
    // Struttura WinAPI che memorizza lo stato originale della console
    // (colori, dimensioni buffer) per ripristinarlo alla chiusura.

    bool primoRender;
    // Flag che distingue il primo rendering completo
    // (pulizia totale dello schermo) dai successivi aggiornamenti
    // differenziali (solo celle modificate).

	// ── Utilità interne ────────────────────────────────────────
	// Inizializza tutta la matrice con celle vuote e azzera la maschera di aggiornamento.
	void inizializzaCelle();

public:

    // ── Costruttori / Distruttore ─────────────────────────────
    // Costruttore di default
    // Imposta tutti gli attributi ai valori di default ossia (30x30) in popsizione
    // (10, 10) con bordo biano
    // Inizializza tutte le celle come vuote, azzera la maschera
    // cellaModificata e ottiene l'handle della console.
    Griglia();

    // Costruttore principale.
    // Parametri:
    //   r           : righe effettive della griglia  (<= MAX_RIGHE)
    //   c           : colonne effettive               (<= MAX_COLONNE)
    //   ox, oy      : origine della griglia nella console (in caratteri)
    //   bordo       : true per disegnare il bordo decorativo
    //   colBordo    : colore WinAPI del bordo
    // Inizializza tutte le celle come vuote, azzera la maschera
    // cellaModificata e ottiene l'handle della console.
    Griglia(int r, int c, int ox, int oy, bool bordo, WORD colBordo);

    // Distruttore: ripristina colore e visibilità del cursore
    // della console allo stato precedente all'avvio del gioco.
    ~Griglia();

    // ── Accesso alle celle — usato dai componenti di gioco ────

    // Restituisce un riferimento diretto alla cella in (r, c).
    // I componenti di gioco chiamano getCella(r,c).impostaContenuto(…)
    // per disegnare i propri elementi.
    // Non esegue controllo dei limiti: il chiamante deve verificare
    // con isInterna() prima di accedere.
    Cella& getCella(int r, int c);

    // Scorciatoia: imposta forma e colori di una singola cella
    // e la marca come modificata per il prossimo rendering.
    void impostaCella(int r, int c, char forma, WORD cp, WORD cs);

    // Svuota una singola cella (spazio, colori default)
    // e la marca come modificata per il prossimo rendering.
    void svuotaCella(int r, int c);

    // ── Operazioni sull'intera griglia ────────────────────────

    // Svuota tutte le celle portandole allo stato vuoto
    // e azzera la maschera cellaModificata.
    // Da chiamare a inizio partita o al cambio di livello.
    void svuotaTutto();

    // Riempie tutte le celle con lo stesso carattere e colori.
    // Utile per inizializzare sfondi o pattern di debug.
    void riempiTutto(char forma, WORD cp, WORD cs);

    // ── Rendering ─────────────────────────────────────────────

    // Rendering completo: ridisegna il bordo e tutte le celle,
    // poi azzera la maschera cellaModificata.
    // Da chiamare al primo frame o dopo un resize della console.
    void renderCompleto();

    // Rendering differenziale: ridisegna solo le celle marcate
    // come modificate dall'ultima chiamata, riducendo lo sfarfallio.
    // Da chiamare ad ogni ciclo del game loop.
    void renderDifferenziale();

    // Disegna il bordo decorativo attorno alla griglia usando
    // i box-drawing characters Unicode (╔ ═ ╗ ║ ╚ ╝).
    // Chiamato internamente da renderCompleto() se bordoAbilitato == true.
    void disegnaBordo();

    // ── Utilità console ───────────────────────────────────────

    // Sposta il cursore della console alla posizione assoluta
    // corrispondente alla cella (r, c), tenendo conto di origineX,
    // origineY e dell'eventuale bordo (+1 offset se presente).
    void posizionaCursore(int r, int c);

    // Nasconde il cursore lampeggiante della console per ridurre
    // lo sfarfallio durante il ciclo di rendering del game loop.
    void nascondiCursore();

    // Mostra il cursore della console: da chiamare alla fine
    // del gioco o durante le fasi di input testuale.
    void mostraCursore();

    // Pulisce l'intera finestra console e riposiziona il cursore
    // in (0,0). Chiamato internamente prima del primo render.
    void pulisciSchermo();

    // Scrive una stringa carattere per carattere sulla griglia,
    // a partire dalla colonna 'colStart', sulla riga 'r'.
    // i parametri cp e cs specificano i colori da applicare a ogni carattere.
    void scriviStringa(int r, int colStart,
        string testo, WORD cp, WORD cs);

    // ── Getter ────────────────────────────────────────────────

    // Restituisce il numero di righe effettive della griglia.
    int getRighe();

    // Restituisce il numero di colonne effettive della griglia.
    int getColonne();

    // Restituisce true se la posizione (r, c) è interna alla griglia.
    // I componenti di gioco usano questo metodo per rilevare
    // le collisioni con i bordi prima di accedere a una cella.
    bool isInterna(int r, int c);
};


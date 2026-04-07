#pragma once
#include <windows.h>
#include "Griglia.h"

// ============================================================
//  Costanti per i tasti speciali
//  I tasti freccia e funzione non producono un char diretto:
//  _getch() restituisce prima 0 o 224, poi il codice esteso.
//  Questi alias rendono il codice di gioco leggibile.
// ============================================================

const int TASTO_NESSUNO = 0;   // nessun tasto premuto (non bloccante)
const int TASTO_INVIO = 13;   // Enter
const int TASTO_BACKSPACE = 8;   // Backspace
const int TASTO_SPAZIO = 32;   // Barra spaziatrice
const int TASTO_ESC = 27;   // Escape

// Frecce — codici estesi preceduti da 224
const int TASTO_SU = 72;
const int TASTO_GIU = 80;
const int TASTO_SINISTRA = 75;
const int TASTO_DESTRA = 77;

// Tasti funzione — codici estesi preceduti da 0
const int TASTO_F1 = 59;
const int TASTO_F2 = 60;
const int TASTO_F3 = 61;
const int TASTO_F4 = 62;
const int TASTO_F5 = 63;
const int TASTO_F6 = 64;
const int TASTO_F7 = 65;
const int TASTO_F8 = 66;
const int TASTO_F9 = 67;
const int TASTO_F10 = 68;

// Lunghezza massima per la lettura di stringhe bloccante
const int MAX_LUNGHEZZA_TESTO = 64;

// ============================================================
//  Costanti di layout per la visualizzazione dell'input
//  Definiscono dove appaiono domanda e risposta sulla griglia.
// ============================================================
const int INPUT_RIGA_DOMANDA = 2;   // riga del testo della domanda
const int INPUT_RIGA_RISPOSTA = 4;   // riga dove l'utente digita
const int INPUT_COLONNA_START = 2;   // colonna di inizio

// ============================================================
//  TASTIERA
//  Interfaccia tra l'utente e gli oggetti di gioco.
//
//  Offre due modalità:
//
//  NON BLOCCANTE — usata nel game loop (Snake, Pong, ...):
//    leggi() controlla se c'è un tasto in attesa nel buffer
//    senza fermare il programma. Se non c'è nulla restituisce
//    TASTO_NESSUNO. Il game loop prosegue normalmente.
//
//  BLOCCANTE — usata nei menu e nell'inserimento dati:
//    leggiBloccante() sospende il programma fino alla pressione
//    di un tasto. leggiTesto() accumula i caratteri digitati
//    fino alla pressione di Invio (con supporto Backspace).
// ============================================================

class Tastiera {
private:

    // ── Stato dell'ultimo tasto letto ─────────────────────────

    int ultimoTasto;
    // Codice dell'ultimo tasto letto (char o costante TASTO_*).
    // Aggiornato da leggi() e leggiBloccante().
    // Permette ai componenti di gioco di consultare l'ultimo
    // input senza rileggere il buffer.

    bool tastoSpeciale;
    // true se l'ultimo tasto letto era un tasto speciale
    // (freccia, F1-F10) che richiede la lettura del codice esteso.
    // false se era un carattere ASCII normale.

    // ── Buffer testo ─────────────────────────────────────────

    char bufferTesto[MAX_LUNGHEZZA_TESTO];
    // Accumula i caratteri digitati durante leggiTesto().
    // Viene azzerato all'inizio di ogni nuova lettura.

    int lunghezzaTesto;
    // Numero di caratteri validi attualmente in bufferTesto.

    // ── Handle console input ─────────────────────────────────

    HANDLE hInput;
    // Handle al buffer di input della console, ottenuto con
    // GetStdHandle(STD_INPUT_HANDLE).
    // Usato da _kbhit() e dalle WinAPI di lettura raw.

    // ── Helper privati ────────────────────────────────────────

    // Scrive una stringa sulla griglia cella per cella.
    // Usato internamente da leggiStringa e leggiInt per
    // visualizzare la domanda e i caratteri digitati.
    void scriviStringa(Griglia& g, int riga, int col,
        const char* testo, WORD cp, WORD cs);

    // Sovrascrive con spazi una riga intera della griglia.
    // Usato per pulire domanda e risposta precedente.
    void pulisciRiga(Griglia& g, int riga, WORD cs);

    // Logica comune di lettura carattere per carattere
    // con eco sulla griglia fino alla pressione di Invio.
    // Riempie 'destinazione' e restituisce la lunghezza.
    // Estratta per evitare duplicazione tra leggiStringa e leggiInt.
    int leggiLoop(Griglia& g, char* destinazione, WORD sfondo);

public:

    // ── Costruttore / Distruttore ────────────────────────────

    // Costruttore: ottiene l'handle di input e azzera lo stato.
    Tastiera();

    // Distruttore: ripristina la modalità input della console.
    ~Tastiera();

    // ── Input NON BLOCCANTE — per il game loop ───────────────

    // Controlla se c'è un tasto in attesa nel buffer di input
    // SENZA fermare il programma.
    // Restituisce il codice del tasto se disponibile,
    // oppure TASTO_NESSUNO se il buffer è vuoto.
    // Gestisce automaticamente i codici estesi (frecce, F1-F10):
    // in quel caso legge il secondo byte e aggiorna tastoSpeciale.
    // Uso tipico: Snake legge la freccia senza bloccare il movimento.
    int leggi();

    // Restituisce true se nel buffer di input c'è almeno un tasto
    // in attesa, senza consumarlo.
    // Utile per decidere se chiamare leggi() nel game loop.
    bool tastoPremuto();

    // Svuota il buffer di input eliminando tutti i tasti accumulati.
    // Da chiamare all'inizio di un nuovo livello o dopo un menu,
    // per evitare che input residui influenzino il gioco.
    void svuotaBuffer();

    // ── Input BLOCCANTE — per menu e schermate statiche ──────

    // Sospende il programma fino alla pressione di un tasto.
    // Restituisce il codice del tasto premuto (char o TASTO_*).
    // Gestisce i codici estesi come leggi().
    // Uso tipico: "Premi un tasto per continuare..."
    int leggiBloccante();

    // Legge una sequenza di caratteri fino alla pressione di Invio,
    // con supporto al Backspace per correggere l'inserimento.
    // I caratteri digitati vengono copiati in 'destinazione'
    // (dimensione minima MAX_LUNGHEZZA_TESTO).
    // Restituisce il numero di caratteri inseriti.
    // Parametri:
    //   destinazione  : buffer esterno dove copiare il testo
    //   eco           : se true, stampa i caratteri durante la digitazione
    //                   (false per password o input silenzioso)
    // Uso tipico: inserimento nome giocatore, risposta quiz.
    int leggiTesto(char* destinazione, bool eco);


    // ── Input con visualizzazione su griglia ─────────────────

    // Mostra 'domanda' sulla griglia, legge una stringa di testo
    // con eco in tempo reale fino a Invio e la copia in 'dest'.
    // Restituisce il numero di caratteri inseriti.
    // Parametri:
    //   g       : griglia su cui visualizzare domanda e risposta
    //   domanda : testo della domanda da mostrare
    //   dest    : buffer in cui scrivere la risposta (min MAX_LUNGHEZZA_TESTO)
    //   sfondo  : colore di sfondo della griglia (per la pulizia delle righe)
    int leggiStringa(Griglia& g, const char* domanda,
        char* dest, WORD sfondo);

    // Mostra 'domanda' sulla griglia, legge una stringa numerica
    // con eco, la converte in intero e applica il clamp [min, max].
    // Se l'utente non inserisce nulla o inserisce un non-numero
    // restituisce il valore di default 'valDefault'.
    // Parametri:
    //   g          : griglia su cui visualizzare domanda e risposta
    //   domanda    : testo della domanda da mostrare
    //   valDefault : valore restituito se l'input non è numerico
    //   minVal     : valore minimo accettabile (clamp inferiore)
    //   maxVal     : valore massimo accettabile (clamp superiore)
    //   sfondo     : colore di sfondo della griglia
    int leggiInt(Griglia& g, const char* domanda,
        int valDefault, int minVal, int maxVal,
        WORD sfondo);


    // ── Getter ────────────────────────────────────────────────

    // Restituisce il codice dell'ultimo tasto letto.
    int getUltimoTasto();

    // Restituisce true se l'ultimo tasto era speciale (freccia, Fx).
    bool isSpeciale();

    // Restituisce true se il codice passato corrisponde a una freccia.
    // Comodo nei componenti di gioco per distinguere frecce da lettere.
    bool isFreccia(int codice);

    // ── Utilità ───────────────────────────────────────────────

    // Confronta il tasto passato con l'ultimo tasto letto.
    // Semplifica i costrutti if nel game loop:
    //   if (tastiera.uguale(TASTO_SU)) snake.cambiaDirezione(SU);
    bool uguale(int codice);
};
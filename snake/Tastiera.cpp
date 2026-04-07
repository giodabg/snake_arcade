#include "Tastiera.h"
#include <conio.h>
#include <iostream>
#include <stdlib.h>    // atoi
#include <string.h>

#define COLORE_DEFAULT (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

// ── Costruttore / Distruttore ────────────────────────────────

Tastiera::Tastiera()
{
    this->hInput = GetStdHandle(STD_INPUT_HANDLE);
    this->ultimoTasto = TASTO_NESSUNO;
    this->tastoSpeciale = false;
    this->lunghezzaTesto = 0;

    int i = 0;
    while (i < MAX_LUNGHEZZA_TESTO)
    {
        this->bufferTesto[i] = '\0';
        i = i + 1;
    }
}

Tastiera::~Tastiera()
{
    // Ripristina la modalità canonica della console
    // (necessario se si fosse disabilitato l'eco o il buffering)
    SetConsoleMode(this->hInput, ENABLE_ECHO_INPUT |
        ENABLE_LINE_INPUT |
        ENABLE_PROCESSED_INPUT);
}

// ── Helper privati ────────────────────────────────────────────

void Tastiera::scriviStringa(Griglia& g, int riga, int col,
    const char* testo, WORD cp, WORD cs)
{
    int i = 0;
    while (testo[i] != '\0')
    {
        if (g.isInterna(riga, col + i) == true)
            g.impostaCella(riga, col + i, testo[i], cp, cs);
        i = i + 1;
    }
}

void Tastiera::pulisciRiga(Griglia& g, int riga, WORD cs)
{
    int j = 0;
    while (j < g.getColonne())
    {
        g.impostaCella(riga, j, ' ', FG_BIANCO, cs);
        j = j + 1;
    }
}

int Tastiera::leggiLoop(Griglia& g, char* destinazione, WORD sfondo)
{
    // Azzera la destinazione
    int k = 0;
    while (k < MAX_LUNGHEZZA_TESTO)
    {
        destinazione[k] = '\0';
        k = k + 1;
    }

    int  lunghezza = 0;
    int  colCorrente = INPUT_COLONNA_START + 2;   // dopo "> "
    bool finito = false;

    while (finito == false)
    {
        int tasto = leggiBloccante();

        if (tasto == TASTO_INVIO)
        {
            finito = true;
        }
        else if (tasto == TASTO_BACKSPACE)
        {
            if (lunghezza > 0)
            {
                lunghezza = lunghezza - 1;
                destinazione[lunghezza] = '\0';
                colCorrente = colCorrente - 1;

                if (g.isInterna(INPUT_RIGA_RISPOSTA, colCorrente) == true)
                    g.impostaCella(INPUT_RIGA_RISPOSTA, colCorrente,
                        ' ', FG_BIANCO, sfondo);

                g.renderDifferenziale();
            }
        }
        else if (tasto == 0 || tasto == 224)
        {
            // tasto speciale già consumato: ignorato nell'input testo
        }
        else if (tasto >= 32 && lunghezza < MAX_LUNGHEZZA_TESTO - 1)
        {
            destinazione[lunghezza] = (char)tasto;
            lunghezza = lunghezza + 1;
            destinazione[lunghezza] = '\0';

            if (g.isInterna(INPUT_RIGA_RISPOSTA, colCorrente) == true)
                g.impostaCella(INPUT_RIGA_RISPOSTA, colCorrente,
                    (char)tasto, FG_BIANCO_I, sfondo);

            colCorrente = colCorrente + 1;
            g.renderDifferenziale();
        }
    }

    return lunghezza;
}


// ── leggiStringa ──────────────────────────────────────────────

int Tastiera::leggiStringa(Griglia& g, const char* domanda,
    char* dest, WORD sfondo)
{
    // Pulisce le righe coinvolte
    pulisciRiga(g, INPUT_RIGA_DOMANDA, sfondo);
    pulisciRiga(g, INPUT_RIGA_DOMANDA + 1, sfondo);
    pulisciRiga(g, INPUT_RIGA_RISPOSTA, sfondo);
    pulisciRiga(g, INPUT_RIGA_RISPOSTA + 1, sfondo);

    // Mostra la domanda in giallo
    scriviStringa(g, INPUT_RIGA_DOMANDA, INPUT_COLONNA_START,
        domanda, FG_GIALLO_I, sfondo);

    // Cursore visivo "> "
    g.impostaCella(INPUT_RIGA_RISPOSTA, INPUT_COLONNA_START,
        '>', FG_CIANO_I, sfondo);
    g.impostaCella(INPUT_RIGA_RISPOSTA, INPUT_COLONNA_START + 1,
        ' ', FG_BIANCO, sfondo);

    g.renderDifferenziale();

    return leggiLoop(g, dest, sfondo);
}


// ── leggiInt ──────────────────────────────────────────────────

int Tastiera::leggiInt(Griglia& g, const char* domanda,
    int valDefault, int minVal, int maxVal,
    WORD sfondo)
{
    char buffer[MAX_LUNGHEZZA_TESTO];

    // Riusa leggiStringa per la parte visiva e di acquisizione
    int len = leggiStringa(g, domanda, buffer, sfondo);

    // Se l'utente ha premuto Invio senza digitare nulla
    // restituisce il valore di default
    if (len == 0)
        return valDefault;

    // Conversione stringa → intero
    int valore = atoi(buffer);

    // Clamp nell'intervallo [minVal, maxVal]
    if (valore < minVal) valore = minVal;
    if (valore > maxVal) valore = maxVal;

    return valore;
}


// ── Input NON BLOCCANTE ───────────────────────────────────────

bool Tastiera::tastoPremuto()
{
    // _kbhit() restituisce non-zero se c'è almeno un tasto nel buffer
    // senza consumarlo — il tasto rimane disponibile per leggi().
    return (_kbhit() != 0);
}

int Tastiera::leggi()
{
    this->tastoSpeciale = false;
    this->ultimoTasto = TASTO_NESSUNO;

    if (_kbhit() == 0)
        return TASTO_NESSUNO;   // buffer vuoto, non si blocca

    int codice = _getch();

    // _getch() restituisce 0 o 224 come prefisso per i tasti
    // speciali (frecce, F1-F10): serve una seconda lettura.
    if (codice == 0 || codice == 224)
    {
        this->tastoSpeciale = true;
        this->ultimoTasto = _getch();   // legge il codice esteso
    }
    else
    {
        this->ultimoTasto = codice;
    }

    return this->ultimoTasto;
}

void Tastiera::svuotaBuffer()
{
    while (_kbhit() != 0)
        _getch();   // consuma e scarta tutti i tasti in coda
}


// ── Input BLOCCANTE ───────────────────────────────────────────

int Tastiera::leggiBloccante()
{
    this->tastoSpeciale = false;

    int codice = _getch();   // si blocca qui finché non arriva un tasto

    if (codice == 0 || codice == 224)
    {
        this->tastoSpeciale = true;
        this->ultimoTasto = _getch();
    }
    else
    {
        this->ultimoTasto = codice;
    }

    return this->ultimoTasto;
}

int Tastiera::leggiTesto(char* destinazione, bool eco)
{
    // Azzera il buffer interno
    this->lunghezzaTesto = 0;
    int i = 0;
    while (i < MAX_LUNGHEZZA_TESTO)
    {
        this->bufferTesto[i] = '\0';
        i = i + 1;
    }

    // Legge caratteri uno alla volta fino a Invio
    bool finito = false;
    while (finito == false)
    {
        int codice = _getch();

        if (codice == TASTO_INVIO)
        {
            // Invio: termina l'acquisizione
            finito = true;
        }
        else if (codice == TASTO_BACKSPACE)
        {
            // Backspace: rimuove l'ultimo carattere se ce n'è uno
            if (this->lunghezzaTesto > 0)
            {
                this->lunghezzaTesto = this->lunghezzaTesto - 1;
                this->bufferTesto[this->lunghezzaTesto] = '\0';

                if (eco == true)
                {
                    // Cancella il carattere a schermo:
                    // torna indietro, stampa spazio, torna indietro
                    std::cout << '\b' << ' ' << '\b';
                }
            }
        }
        else if (codice == 0 || codice == 224)
        {
            // Tasto speciale: consuma il secondo byte e lo ignora
            // (le frecce non hanno senso nell'inserimento testo)
            _getch();
        }
        else if (codice >= 32 && this->lunghezzaTesto < MAX_LUNGHEZZA_TESTO - 1)
        {
            // Carattere stampabile: aggiunge al buffer
            this->bufferTesto[this->lunghezzaTesto] = (char)codice;
            this->lunghezzaTesto = this->lunghezzaTesto + 1;
            this->bufferTesto[this->lunghezzaTesto] = '\0';

            if (eco == true)
                std::cout << (char)codice;
        }
    }

    // Copia il risultato nel buffer esterno
    i = 0;
    while (i <= this->lunghezzaTesto)   // include il terminatore '\0'
    {
        destinazione[i] = this->bufferTesto[i];
        i = i + 1;
    }

    return this->lunghezzaTesto;
}


// ── Getter ────────────────────────────────────────────────────

int Tastiera::getUltimoTasto()
{
    return this->ultimoTasto;
}

bool Tastiera::isSpeciale()
{
    return this->tastoSpeciale;
}

bool Tastiera::isFreccia(int codice)
{
    bool risultato = false;

    if (codice == TASTO_SU)        risultato = true;
    if (codice == TASTO_GIU)       risultato = true;
    if (codice == TASTO_SINISTRA)  risultato = true;
    if (codice == TASTO_DESTRA)    risultato = true;

    return risultato;
}

bool Tastiera::uguale(int codice)
{
    return (this->ultimoTasto == codice);
}

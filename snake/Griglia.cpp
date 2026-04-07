#include "Griglia.h"
#include <iostream>

// ============================================================
//  Costanti locali
// ============================================================
#define COLORE_DEFAULT (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define SFONDO_DEFAULT 0

// Caratteri box-drawing per il bordo decorativo (UTF-8 / CP 65001)
#define BORDO_ANGOLO_ALTO_SX  '\xC9'   // ╔
#define BORDO_ANGOLO_ALTO_DX  '\xBB'   // ╗
#define BORDO_ANGOLO_BASSO_SX '\xC8'   // ╚
#define BORDO_ANGOLO_BASSO_DX '\xBC'   // ╝
#define BORDO_ORIZZONTALE     '\xCD'   // ═
#define BORDO_VERTICALE       '\xBA'   // ║


// ── Costruttore / Distruttore ────────────────────────────────

void Griglia::inizializzaCelle()
{
    int i = 0;
    while (i < this->numRighe)
    {
        int j = 0;
        while (j < this->numColonne)
        {
            this->celle[i][j].impostaPosizione(i, j);
            this->celle[i][j].svuota();
            this->cellaModificata[i][j] = false;
            j = j + 1;
        }
        i = i + 1;
    }
}

Griglia::Griglia()
{
    this->numRighe = 30;
    this->numColonne = 30;

    this->origineX = 10;
    this->origineY = 10;
    this->bordoAbilitato = true;
    this->coloreBordo = FG_BIANCO;
    this->primoRender = true;

    // Ottiene l'handle della console e salva lo stato originale
    this->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(this->hConsole, &this->infoConsole);

    // Inizializza tutte le this->celle con posizione e stato vuoto
    inizializzaCelle();
}

Griglia::Griglia(int r, int c, int ox, int oy, bool bordo, WORD colBordo)
{
    // Clamp delle dimensioni entro i limiti della matrice statica
    if (r > MAX_RIGHE)
        this->numRighe = MAX_RIGHE;
    else
        this->numRighe = r;

    if (c > MAX_COLONNE)
        this->numColonne = MAX_COLONNE;
    else
        this->numColonne = c;

    this->origineX = ox;
    this->origineY = oy;
    this->bordoAbilitato = bordo;
    this->coloreBordo = colBordo;
    this->primoRender = true;

    // Ottiene l'handle della console e salva lo stato originale
    this->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(this->hConsole, &this->infoConsole);

    // Inizializza tutte le this->celle con posizione e stato vuoto
    inizializzaCelle();
}

Griglia::~Griglia()
{
    // Ripristina il colore e la visibilità originali del cursore
    SetConsoleTextAttribute(this->hConsole, this->infoConsole.wAttributes);
    mostraCursore();
}


// ── Accesso alle this->celle ────────────────────────────────────────

Cella& Griglia::getCella(int r, int c)
{
    if (!isInterna(r, c))
    {
        // Accesso fuori dai limiti: comportamento non definito.
        // Per sicurezza, restituisce un riferimento alla cella (0,0).
        return this->celle[0][0];
	}
    return this->celle[r][c];
}

void Griglia::impostaCella(int r, int c, char forma, WORD cp, WORD cs)
{
    if (!isInterna(r, c))
    {
        // Accesso fuori dai limiti: comportamento non definito.
        // Per sicurezza, non fa nulla.
        return;
    }
    this->celle[r][c].impostaContenuto(forma, cp, cs);
    this->cellaModificata[r][c] = true;
}

void Griglia::svuotaCella(int r, int c)
{
    if (!isInterna(r, c))
    {
        // Accesso fuori dai limiti: comportamento non definito.
        // Per sicurezza, non fa nulla.
        return;
	}
    this->celle[r][c].svuota();
    this->cellaModificata[r][c] = true;
}


// ── Operazioni sull'intera griglia ────────────────────────────

void Griglia::svuotaTutto()
{
    int i = 0;
    while (i < this->numRighe)
    {
        int j = 0;
        while (j < this->numColonne)
        {
            this->celle[i][j].svuota();
            this->cellaModificata[i][j] = false;
            j = j + 1;
        }
        i = i + 1;
    }
}

void Griglia::riempiTutto(char forma, WORD cp, WORD cs)
{
    int i = 0;
    while (i < this->numRighe)
    {
        int j = 0;
        while (j < this->numColonne)
        {
            this->celle[i][j].impostaContenuto(forma, cp, cs);
            this->cellaModificata[i][j] = true;
            j = j + 1;
        }
        i = i + 1;
    }
}


// ── Rendering ─────────────────────────────────────────────────

void Griglia::renderCompleto()
{
    if (this->primoRender == true)
    {
        pulisciSchermo();
        nascondiCursore();
        this->primoRender = false;
    }

    if (this->bordoAbilitato == true)
        disegnaBordo();

    int i = 0;
    while (i < this->numRighe)
    {
        int j = 0;
        while (j < this->numColonne)
        {
            posizionaCursore(i, j);
            this->celle[i][j].visualizza(this->hConsole);
            this->cellaModificata[i][j] = false;
            j = j + 1;
        }
        i = i + 1;
    }
}

void Griglia::renderDifferenziale()
{
    if (this->primoRender == true)
    {
        // Al primo ciclo forza sempre un render completo
        renderCompleto();
    }
    else
    {
        int i = 0;
        while (i < this->numRighe)
        {
            int j = 0;
            while (j < this->numColonne)
            {
                if (this->cellaModificata[i][j] == true)
                {
                    posizionaCursore(i, j);
                    this->celle[i][j].visualizza(this->hConsole);
                    this->cellaModificata[i][j] = false;
                }
                j = j + 1;
            }
            i = i + 1;
        }
    }
}

void Griglia::disegnaBordo()
{
    // Offset di bordo: il bordo occupa la colonna/riga immediatamente
    // esterna alla griglia, quindi si disegna a this->origineX-1, this->origineY-1.
    COORD pos;
    int   offset = 1;   // larghezza del bordo in caratteri

    SetConsoleTextAttribute(this->hConsole, this->coloreBordo);

    // ── Riga superiore: ╔═══...═══╗ ─────────────────────────
    pos.X = (short)(this->origineX - offset);
    pos.Y = (short)(this->origineY - offset);
    SetConsoleCursorPosition(this->hConsole, pos);
    cout << BORDO_ANGOLO_ALTO_SX;

    for (int j = 0; j < this->numColonne + offset + 1; j++)
    {
        cout << BORDO_ORIZZONTALE;
    }

    cout << BORDO_ANGOLO_ALTO_DX;

    // ── Righe laterali: ║ ... ║ ──────────────────────────────
    int i = 0;
    while (i < this->numRighe+offset)
    {
        // Bordo sinistro
        pos.X = (short)(this->origineX - offset);
        pos.Y = (short)(this->origineY + i);
        SetConsoleCursorPosition(this->hConsole, pos);
        cout << BORDO_VERTICALE;

        // Bordo destro
        pos.X = (short)(this->origineX + this->numColonne + offset + 1);
        pos.Y = (short)(this->origineY + i);
        SetConsoleCursorPosition(this->hConsole, pos);
        cout << BORDO_VERTICALE;

        i = i + 1;
    }

    // ── Riga inferiore: ╚═══...═══╝ ─────────────────────────
    pos.X = (short)(this->origineX - offset);
    pos.Y = (short)(this->origineY + this->numRighe + offset);
    SetConsoleCursorPosition(this->hConsole, pos);
    cout << BORDO_ANGOLO_BASSO_SX;

    for (int j = 0; j < this->numColonne + offset + 1; j++)
    {
        cout << BORDO_ORIZZONTALE;
    }
    
    cout << BORDO_ANGOLO_BASSO_DX;

    // Ripristina il colore di default al termine del bordo
    SetConsoleTextAttribute(this->hConsole, COLORE_DEFAULT);
}


// ── Utilità console ───────────────────────────────────────────

void Griglia::posizionaCursore(int r, int c)
{
    if (!isInterna(r, c))
    {
        // Accesso fuori dai limiti: comportamento non definito.
        // Per sicurezza, posiziona il cursore in alto a sinistra.
        COORD pos = { 0, 0 };
        SetConsoleCursorPosition(this->hConsole, pos);
        return;
	}

    // Se il bordo è abilitato, la griglia parte da this->origineX+1, this->origineY+1
    // per lasciare spazio ai caratteri del bordo esterno.
    COORD pos;

    if (this->bordoAbilitato == true)
    {
        pos.X = (short)(this->origineX + c + 1);
        pos.Y = (short)(this->origineY + r + 1);
    }
    else
    {
        pos.X = (short)(this->origineX + c);
        pos.Y = (short)(this->origineY + r);
    }

    SetConsoleCursorPosition(this->hConsole, pos);
}

void Griglia::nascondiCursore()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(this->hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(this->hConsole, &cursorInfo);
}

void Griglia::mostraCursore()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(this->hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(this->hConsole, &cursorInfo);
}

void Griglia::pulisciSchermo()
{
    // Tecnica WinAPI per pulire il buffer senza chiamare system("cls")
    // che causa sfarfallio visibile sulla console Windows.
    COORD                      coordHome = { 0, 0 };
    DWORD                      scritte;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(this->hConsole, &csbi);
    DWORD dimensioneBuffer = csbi.dwSize.X * csbi.dwSize.Y;

    // Riempie l'intero buffer di spazi
    FillConsoleOutputCharacter(this->hConsole, ' ',
        dimensioneBuffer, coordHome, &scritte);

    // Applica il colore di default a tutto il buffer
    FillConsoleOutputAttribute(this->hConsole, csbi.wAttributes,
        dimensioneBuffer, coordHome, &scritte);

    // Riporta il cursore in alto a sinistra
    SetConsoleCursorPosition(this->hConsole, coordHome);
}


// ── Getter ────────────────────────────────────────────────────

int Griglia::getRighe()
{
    return this->numRighe;
}

int Griglia::getColonne()
{
    return this->numColonne;
}

bool Griglia::isInterna(int r, int c)
{
    // Restituisce true solo se entrambe le coordinate rientrano
    // nei limiti effettivi della griglia corrente.
    // I componenti di gioco dovrebbero chiamare questo metodo prima di
    // accedere a alle celle per evitare accessi fuori matrice.
    bool rigaValida = (r >= 0) && (r < this->numRighe);
    bool colonnaValida = (c >= 0) && (c < this->numColonne);

    return rigaValida && colonnaValida;
}

void Griglia::scriviStringa(int r, int colStart,
    string testo, WORD cp, WORD cs)
{
    int i = 0;
    while (testo[i] != '\0')
    {
        if (this->isInterna(r, colStart + i) == true)
            this->impostaCella(r, colStart + i, testo[i], cp, cs);
        i = i + 1;
    }
}
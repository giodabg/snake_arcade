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

Griglia::Griglia(int r, int c, int ox, int oy, bool bordo, WORD colBordo)
{
    // Clamp delle dimensioni entro i limiti della matrice statica
    if (r > MAX_RIGHE)
        righe = MAX_RIGHE;
    else
        righe = r;

    if (c > MAX_COLONNE)
        colonne = MAX_COLONNE;
    else
        colonne = c;

    origineX = ox;
    origineY = oy;
    bordoAbilitato = bordo;
    coloreBordo = colBordo;
    primoRender = true;

    // Ottiene l'handle della console e salva lo stato originale
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &infoConsole);

    // Inizializza tutte le celle con posizione e stato vuoto
    int i = 0;
    while (i < righe)
    {
        int j = 0;
        while (j < colonne)
        {
            celle[i][j].impostaPosizione(i, j);
            celle[i][j].svuota();
            cellaModificata[i][j] = false;
            j = j + 1;
        }
        i = i + 1;
    }
}

Griglia::~Griglia()
{
    // Ripristina il colore e la visibilità originali del cursore
    SetConsoleTextAttribute(hConsole, infoConsole.wAttributes);
    mostraCursore();
}


// ── Accesso alle celle ────────────────────────────────────────

Cella& Griglia::getCella(int r, int c)
{
    return celle[r][c];
}

void Griglia::impostaCella(int r, int c, char forma, WORD cp, WORD cs)
{
    celle[r][c].impostaContenuto(forma, cp, cs);
    cellaModificata[r][c] = true;
}

void Griglia::svuotaCella(int r, int c)
{
    celle[r][c].svuota();
    cellaModificata[r][c] = true;
}


// ── Operazioni sull'intera griglia ────────────────────────────

void Griglia::svuotaTutto()
{
    int i = 0;
    while (i < righe)
    {
        int j = 0;
        while (j < colonne)
        {
            celle[i][j].svuota();
            cellaModificata[i][j] = false;
            j = j + 1;
        }
        i = i + 1;
    }
}

void Griglia::riempiTutto(char forma, WORD cp, WORD cs)
{
    int i = 0;
    while (i < righe)
    {
        int j = 0;
        while (j < colonne)
        {
            celle[i][j].impostaContenuto(forma, cp, cs);
            cellaModificata[i][j] = true;
            j = j + 1;
        }
        i = i + 1;
    }
}


// ── Rendering ─────────────────────────────────────────────────

void Griglia::renderCompleto()
{
    if (primoRender == true)
    {
        pulisciSchermo();
        nascondiCursore();
        primoRender = false;
    }

    if (bordoAbilitato == true)
        disegnaBordo();

    int i = 0;
    while (i < righe)
    {
        int j = 0;
        while (j < colonne)
        {
            posizionaCursore(i, j);
            celle[i][j].visualizza(hConsole);
            cellaModificata[i][j] = false;
            j = j + 1;
        }
        i = i + 1;
    }
}

void Griglia::renderDifferenziale()
{
    if (primoRender == true)
    {
        // Al primo ciclo forza sempre un render completo
        renderCompleto();
    }
    else
    {
        int i = 0;
        while (i < righe)
        {
            int j = 0;
            while (j < colonne)
            {
                if (cellaModificata[i][j] == true)
                {
                    posizionaCursore(i, j);
                    celle[i][j].visualizza(hConsole);
                    cellaModificata[i][j] = false;
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
    // esterna alla griglia, quindi si disegna a origineX-1, origineY-1.
    COORD pos;
    int   offset = 1;   // larghezza del bordo in caratteri

    SetConsoleTextAttribute(hConsole, coloreBordo);

    // ── Riga superiore: ╔═══...═══╗ ─────────────────────────
    pos.X = (short)(origineX - offset);
    pos.Y = (short)(origineY - offset);
    SetConsoleCursorPosition(hConsole, pos);
    cout << BORDO_ANGOLO_ALTO_SX;

    for (int j = 0; j < colonne + offset + 1; j++)
    {
        cout << BORDO_ORIZZONTALE;
    }

    cout << BORDO_ANGOLO_ALTO_DX;

    // ── Righe laterali: ║ ... ║ ──────────────────────────────
    int i = 0;
    while (i < righe+offset)
    {
        // Bordo sinistro
        pos.X = (short)(origineX - offset);
        pos.Y = (short)(origineY + i);
        SetConsoleCursorPosition(hConsole, pos);
        cout << BORDO_VERTICALE;

        // Bordo destro
        pos.X = (short)(origineX + colonne + offset + 1);
        pos.Y = (short)(origineY + i);
        SetConsoleCursorPosition(hConsole, pos);
        cout << BORDO_VERTICALE;

        i = i + 1;
    }

    // ── Riga inferiore: ╚═══...═══╝ ─────────────────────────
    pos.X = (short)(origineX - offset);
    pos.Y = (short)(origineY + righe + offset);
    SetConsoleCursorPosition(hConsole, pos);
    cout << BORDO_ANGOLO_BASSO_SX;

    for (int j = 0; j < colonne + offset + 1; j++)
    {
        cout << BORDO_ORIZZONTALE;
    }
    
    cout << BORDO_ANGOLO_BASSO_DX;

    // Ripristina il colore di default al termine del bordo
    SetConsoleTextAttribute(hConsole, COLORE_DEFAULT);
}


// ── Utilità console ───────────────────────────────────────────

void Griglia::posizionaCursore(int r, int c)
{
    // Se il bordo è abilitato, la griglia parte da origineX+1, origineY+1
    // per lasciare spazio ai caratteri del bordo esterno.
    COORD pos;

    if (bordoAbilitato == true)
    {
        pos.X = (short)(origineX + c + 1);
        pos.Y = (short)(origineY + r + 1);
    }
    else
    {
        pos.X = (short)(origineX + c);
        pos.Y = (short)(origineY + r);
    }

    SetConsoleCursorPosition(hConsole, pos);
}

void Griglia::nascondiCursore()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void Griglia::mostraCursore()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void Griglia::pulisciSchermo()
{
    // Tecnica WinAPI per pulire il buffer senza chiamare system("cls")
    // che causa sfarfallio visibile sulla console Windows.
    COORD                      coordHome = { 0, 0 };
    DWORD                      scritte;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dimensioneBuffer = csbi.dwSize.X * csbi.dwSize.Y;

    // Riempie l'intero buffer di spazi
    FillConsoleOutputCharacter(hConsole, ' ',
        dimensioneBuffer, coordHome, &scritte);

    // Applica il colore di default a tutto il buffer
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
        dimensioneBuffer, coordHome, &scritte);

    // Riporta il cursore in alto a sinistra
    SetConsoleCursorPosition(hConsole, coordHome);
}


// ── Getter ────────────────────────────────────────────────────

int Griglia::getRighe()
{
    return righe;
}

int Griglia::getColonne()
{
    return colonne;
}

bool Griglia::isInterna(int r, int c)
{
    // Restituisce true solo se entrambe le coordinate rientrano
    // nei limiti effettivi della griglia corrente.
    // I componenti di gioco chiamano questo metodo prima di
    // accedere a getCella() per evitare accessi fuori matrice.
    bool rigaValida = (r >= 0) && (r < righe);
    bool colonnaValida = (c >= 0) && (c < colonne);

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
#include <graphics.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <conio.h>
#define MAX1 10
#define MAX2 25
#define MAX_PLACED_PIECES 25
#define MAXZOOM 55.0
#define MINZOOM 15.0
#define PI 3.1415
#define MCOLOR1 COLOR(15,255,80) //neon green
#define MCOLOR2 COLOR(4, 55, 242) // blue
#define MCOLOR3 COLOR (31, 81, 255) // neon blue

using namespace std;

float zoom = MINZOOM;

struct xynodpiese
{
    float x, y;
};

struct Desen
{
    char tipfigura;
    float x1, y1, x2, y2;
};

struct piesa
{
    char Name[100];
    int NumberOfNodes;
    xynodpiese nodpiesa[MAX1];
    int NumberOfDrawingCommands;
    Desen comanda[MAX2];
};

struct MapOfSavedPieces
{
    int index;
    float x, y;
    int rotationangle;
    int sizep = zoom;
    int Color = WHITE;
};

piesa piese[MAX2];
MapOfSavedPieces placedPieces[MAX_PLACED_PIECES];
int nrPiese = 0;
int nrPlacedPieces = 0;
int width = 1280, height = 720; // 16:9 resolutions: 1920 x 1080, 1600 x 900, 1280 x 720
int selectedpiece = 0;
int PSelected = -1;
bool pieceSelected = false;
int c = 8; //number of buttons
int b = 12; //by what we divide height and some other variables, to create parts of the menu
int ok; //check for handling mouse events #1
int Pok = 0; //check for handling mouse events #2
bool isDragging = false;
float PixelOfZoom = (float)(width/c)/(float)(MAXZOOM-MINZOOM);
float PixelOfRotation = (width/c)/(float)180.5;
const char *bnames[] = { "INTRODUCE", /*TEXT #1 BUTTON*/ "STERGE", /*TEXT #2 BUTTON*/
                         "INSTRUMENTE", /*TEXT #3 BUTTON*/ "", /*TEXT #4 BUTTON*/
                         "", /*TEXT #5 BUTTON*/ "", /*TEXT #6 BUTTON*/
                         "INAPOI", /*TEXT #7 BUTTON*/ "IESI", /*TEXT #8 BUTTON*/
                       };
const char *bcarac[2] = {"UNGHI:", "MARIME:"};
const char *bworkspace[4] = {"STERGE TOT", "SALVEAZA", "SALVEAZA CA", "INCARCA"};
string lastLoadedFilePath;
int mouseXOnPress, mouseYOnPress;
float pieceXOnPress, pieceYOnPress;
int ScreenNumber = 1;
bool doubleClicked = false;

int ColorSelected()
{
    return MCOLOR3; // color selected to draw the menu/selected piece, etc
}

void citirePiesa(const string& filePath, piesa& p)
{
    ifstream file(filePath);

    file >> p.Name;
    file >> p.NumberOfNodes;

    for (int i = 0; i < p.NumberOfNodes; i++)
        file >> p.nodpiesa[i].x >> p.nodpiesa[i].y;

    file >> p.NumberOfDrawingCommands;
    for (int i = 0; i < p.NumberOfDrawingCommands; ++i)
    {
        file >> p.comanda[i].tipfigura;
        file >> p.comanda[i].x1 >> p.comanda[i].y1 >> p.comanda[i].x2 >> p.comanda[i].y2;
    }
}

void incarcapiesele(const string& directoryPath)
{
    for (const auto & entry : filesystem::directory_iterator(directoryPath))
    {
        citirePiesa(entry.path().string(), piese[nrPiese]);
        nrPiese++;
    }
}

void menustyle()
{
    setlinestyle(0,0,2);
    setcolor(ColorSelected());
}

void resetstyle() //used to change from the menu style to normal style
{
    setlinestyle(0,0,0);
    setcolor(WHITE);
}

void fillstyle(int Color)
{
    setfillstyle(SOLID_FILL, Color);
}

void Rotate(float &x, float &y, float theta)
{
    theta = theta * PI / 180;
    float tempX = x;
    x = x * cos(theta) - y * sin(theta);
    y = tempX * sin(theta) + y * cos(theta);
}

void RotateRectangle(float x, float y, float width, float height, float angle)
{
    float halfWidth = width / 2;
    float halfHeight = height / 2;

    float cosA = cos(angle * PI / 180);
    float sinA = sin(angle * PI / 180);

    float x1 = x + halfWidth * cosA - halfHeight * sinA;
    float y1 = y + halfWidth * sinA + halfHeight * cosA;
    float x2 = x - halfWidth * cosA - halfHeight * sinA;
    float y2 = y - halfWidth * sinA + halfHeight * cosA;
    float x3 = x - halfWidth * cosA + halfHeight * sinA;
    float y3 = y - halfWidth * sinA - halfHeight * cosA;
    float x4 = x + halfWidth * cosA + halfHeight * sinA;
    float y4 = y + halfWidth * sinA - halfHeight * cosA;


    line(x1, y1, x2, y2);
    line(x2, y2, x3, y3);
    line(x3, y3, x4, y4);
    line(x4, y4, x1, y1);
}


void DrawLine(piesa P, int i, float x, float y, float angle, float sizedrawing)
{
    float x_1 = P.comanda[i].x1 ;
    float y_1 = P.comanda[i].y1 ;
    float x_2 = P.comanda[i].x2 ;
    float y_2 = P.comanda[i].y2 ;
    Rotate(x_1, y_1, angle);
    Rotate(x_2, y_2, angle);
    line(x_1 * sizedrawing + x, y_1 * sizedrawing + y, x_2 * sizedrawing + x, y_2 * sizedrawing + y) ;
}

void DrawRectangle(piesa P, int i, float x, float y, float angle, float sizedrawing)
{
    float x1 = P.comanda[i].x1;
    float y1 = P.comanda[i].y1;
    float x2 = P.comanda[i].x2;
    float y2 = P.comanda[i].y2;

    RotateRectangle(x, y, (x2 - x1) * sizedrawing, (y2 - y1) * sizedrawing, angle);
}

void DrawCircle(piesa P,int i, float x, float y, float sizedrawing)
{
    circle(x+P.comanda[i].x2*sizedrawing,y+P.comanda[i].y2*sizedrawing,P.comanda[i].x1*sizedrawing);
}

void DrawButton(int xtop, int ytop, int xbottom, int ybottom)
{
    menustyle();
    rectangle(xtop, ytop, xbottom, ybottom);
    setlinestyle(0,0,1);
    rectangle(xtop+2, ytop+2, xbottom-3, ybottom-3);
    resetstyle();
}

void Drawing(piesa P, float x, float y, float angle, int Color, float sizedrawing)
{
    setcolor(Color);
    fillstyle(Color);//need to introduce in the antet of the function a int like Color, for selecting
    for(int i = 0; i < P.NumberOfNodes; i++)
    {
        float x_2 = P.nodpiesa[i].x;
        float y_2 = P.nodpiesa[i].y;
        Rotate(x_2, y_2, angle);
        circle(x+x_2*sizedrawing,y+y_2*sizedrawing, sizedrawing/5);
        floodfill(x+x_2*sizedrawing,y+y_2*sizedrawing,Color);
    }
    for(int i = 0; i < P.NumberOfDrawingCommands; i++)
        switch(P.comanda[i].tipfigura)
        {
        case 'L':
            DrawLine(P, i, x, y, angle, sizedrawing);
            break;
        case 'R':
            DrawRectangle(P, i, x, y, angle, sizedrawing);
            break;
        case 'O':
            DrawCircle(P, i, x, y, sizedrawing);
            break;
        }
    setcolor(WHITE);
    fillstyle(WHITE);
}

void printtext(int x, int y, const char* text)
{
    settextstyle(3,HORIZ_DIR,3);
    setcolor(ColorSelected());
    char temp[100];
    strcpy(temp, text);
    outtextxy(x - textwidth(temp) / 2, y - textheight(temp) / 2, temp);
    setcolor(WHITE);
}

void DrawMainMenu()
{
    cleardevice();


    setbkcolor(BLACK);
    clearviewport();


    setcolor(WHITE);


    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    outtextxy(width / 2 - textwidth("Electron") / 2, height / 8 - textheight("Electron") / 2, "Electron");


    setcolor(COLOR(0, 0, 255));
    rectangle(width / 4, height / 4, 3 * width / 4, 5 * height / 12);
    rectangle(width / 4, 5 * height / 12, 3 * width / 4, 2 * height / 3);
    rectangle(width / 4, 2 * height / 3, 3 * width / 4, 11 * height / 12);


    settextstyle(BOLD_FONT, HORIZ_DIR, 2);
    printtext(width / 2, (5 * height / 12 + height / 4) / 2, "Start");
    printtext(width / 2, (2 * height / 3 + 5 * height / 12) / 2, "Ajutor");
    printtext(width / 2, (11 * height / 12 + 2 * height / 3) / 2, "Iesire");
}




void Menu()
{
    menustyle();
    int yline = height/b;
    line(0, yline, width, yline);
    line(0, yline*(b-1), width, yline*(b-1));
    resetstyle();
    for(int i = 0; i < 3; i++)
    {
        DrawButton(width/c*i, 1.5, width/c*(i+1), yline);// /c - how many buttons can fit; 1.5 - for fitting
        printtext((width/c*i+width/c*(i+1))/2,yline/2,bnames[i]);
    }
    for(int i = c-1; i > c-3; i--)
    {
        DrawButton(width/c*i, 1.5, width/c*(i+1), yline);// /c - how many buttons can fit; 1.5 - for fitting
        printtext((width/c*i+width/c*(i+1))/2,yline/2,bnames[i]);
    }
}

void BackToStartScreen()
{
    DrawMainMenu();
    delay(100);
}

void DrawPlacedPieces()
{
    if(nrPlacedPieces!=0)
    {
        for(int i = 0; i < nrPlacedPieces; i++)
            Drawing(piese[placedPieces[i].index], placedPieces[i].x, placedPieces[i].y, placedPieces[i].rotationangle, placedPieces[i].Color, placedPieces[i].sizep);
    }
}

void CheckIfPieceIsSelected()
{

}

void PropertiesMenu()
{
    setlinestyle(0,0,5);
    line(width/c*(c-2)+width/c/2,(height/b*b+height/b*(b-1))/2, width/c*(c-1)+width/c/2, (height/b*b+height/b*(b-1))/2);
    line(width/c*(c-3),(height/b*b+height/b*(b-1))/2, width/c*(c-2), (height/b*b+height/b*(b-1))/2);
    setlinestyle(0,0,0);
    circle(width/c*(c-2)+width/c/2 + PixelOfZoom*(placedPieces[PSelected].sizep-MINZOOM), (height/b*b+height/b*(b-1))/2, height/b/4);
    circle(width/c*(c-3) + PixelOfRotation*placedPieces[PSelected].rotationangle, (height/b*b+height/b*(b-1))/2, height/b/4);
}


void RestartMenu()
{
    cleardevice();
    Menu();
    DrawPlacedPieces();
    if(Pok > 0)
    {
        PropertiesMenu();
        ok=4;
    }
    else
        ok=0;
}

void DeselectPiece()
{
    placedPieces[PSelected].Color=WHITE;
    PSelected = -1;
    RestartMenu();
}

void DeletePiece()
{
    if (PSelected != -1)
    {
        for (int i = PSelected; i < nrPlacedPieces - 1; ++i)
        {
            placedPieces[i] = placedPieces[i + 1];
        }
        nrPlacedPieces--;

        DeselectPiece();

        RestartMenu();
    }
}

void SaveMapToFile(const string& filePath, bool overwrite)
{
    if (!overwrite && filesystem::exists(filePath))
    {
        cout << "Error: File already exists. Choose a different filename or enable overwrite." << endl;
        return;
    }

    ofstream file(filePath);

    for (int i = 0; i < nrPlacedPieces; i++)
    {
        file << placedPieces[i].index << " " << placedPieces[i].x << " " << placedPieces[i].y << " "
             << placedPieces[i].rotationangle << " " << placedPieces[i].sizep << " " << placedPieces[i].Color << endl;
    }

    file.close();
}

void SaveMapOfPieces(bool overwrite = true)
{
    string filePath = (lastLoadedFilePath.empty()) ? "map.txt" : lastLoadedFilePath;
    SaveMapToFile(filePath, overwrite);
    cout << "Map saved to: " << filePath << endl;
}

void SaveMapAs(bool overwrite = true)
{
    string filePath;
    overwrite=false;
    cout << "Select a file to save the map: ";
    cin >> filePath;

    SaveMapToFile(filePath, overwrite);
    cout << "Map saved to: " << filePath << endl;
}

void SaveAsMapOfPieces()
{
    SaveMapAs();
}

void LoadMapFromFile(const string& filePath)
{
    ifstream file(filePath);

    nrPlacedPieces = 0;
    while (file >> placedPieces[nrPlacedPieces].index >> placedPieces[nrPlacedPieces].x >> placedPieces[nrPlacedPieces].y
            >> placedPieces[nrPlacedPieces].rotationangle >> placedPieces[nrPlacedPieces].sizep >> placedPieces[nrPlacedPieces].Color)
    {
        nrPlacedPieces++;
        if (nrPlacedPieces >= MAX_PLACED_PIECES)
            break;
    }

    file.close();

    lastLoadedFilePath = filePath;
}

void LoadMapOfPieces()
{
    string filePath;
    cout << "Select a file to load the map from: ";
    cin >> filePath;

    LoadMapFromFile(filePath);
    RestartMenu();
    cout << "Map loaded from: " << filePath << endl;
}


void ResetMapOfPieces()
{
    nrPlacedPieces = 0;
    RestartMenu();
    cout << "Map resetted" << endl;
}


void SelectPiece()
{
    placedPieces[PSelected].Color=ColorSelected();;
    RestartMenu();
    ok=4;
}

void HelpScreen()
{
    cleardevice();

    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);

    settextstyle(BOLD_FONT, HORIZ_DIR, 2);
    printtext(width / 2, height / 8, "Ajutor");

    printtext(width / 2, height / 4, "Click = Selectarea Piesei");
    printtext(width / 2, height / 4 + 30, "DoubleClick = Deselectarea Piesei");

    printtext(width / 2, height / 4 + 80, "Atunci cand piesa este selectata,");
    printtext(width / 2, height / 4 + 110, "doua slidere vor aparea:");

    printtext(width / 2, height / 4 + 160, "Sterge = Stergerea unei piese si a");
    printtext(width / 2, height / 4 + 190, "tuturor legaturilor cu piesa respectiva");

    printtext(width / 2, height / 4 + 240, "Sterge Tot = Curatarea ecranului de toate");
    printtext(width / 2, height / 4 + 270, "piesele si legaturile");

    printtext(width / 2, height / 4 + 320, "Salveaza = Salvarea progresului facut in");
    printtext(width / 2, height / 4 + 350, "fisierul de lucru");

    printtext(width / 2, height / 4 + 400, "Salveaza Ca = Crearea unui nou fisier in");
    printtext(width / 2, height / 4 + 430, "care va fi salvat schema electrica");

    printtext(width / 2, height / 4 + 480, "Incarca = Deschiderea unui fisier deja creat");
    printtext(width / 2, height / 4 + 510, "si lucrarea in acesta");

    DrawButton(width - 80, 20, width - 20, 60);
    printtext(width - 50, 40, "Inapoi");

}


void Lclick_handler(int x, int y)
{
    if(ScreenNumber==1)
    {
        if (x > width / 4 && x < 3 * width / 4)
    {
        if (y > height / 4 && y < 5 * height / 12)
        {
            ScreenNumber=2;
        }
        else if (y > 5 * height / 12 && y < 2 * height / 3)
        {
            ScreenNumber=3;
            HelpScreen();
        }
        else if (y > 2 * height / 3 && y < 11 * height / 12)
        {
            closegraph();
            getch();
        }
    }
    }
    else if(ScreenNumber==2)
    {
    bool condition1 = (x < width/c*c && width/c*(c-1) < x && 0 < y && y < height/b);
    bool condition2 = (x < width/c*(c-1) && width/c*(c-2) < x && 0 < y && y < height/b);
    bool condition3 = (x < width/c*2 && width/c < x && 0 < y && y < height/b);
    bool condition4 = (ok==1 && x < width/c && 0 < x && height/b < y && y < height/b*(nrPiese+1));
    bool condition5 = (ok==3 && height/b < y && y < height/b*(b-1));
    bool condition6 = (ok==2 && x < width/c*3 && width/c*2 < x && height/b < y && y < height/b*5);
    bool condition7 = (y > height/b*(b-1));
    bool condition8 = (height/b < y && y < height/b*(b-1));
    bool condition9 = (x < (width/c*(c-3) + PixelOfRotation*placedPieces[PSelected].rotationangle + height/b/4) && (width/c*(c-3) + PixelOfRotation*placedPieces[PSelected].rotationangle - height/b/4) < x && y < (height/b*b+height/b*(b-1))/2+height/b/4 && (height/b*b+height/b*(b-1))/2-height/b/4 < y);
    bool condition10 = (x < (width/c*(c-2)+width/c/2 + PixelOfZoom*(placedPieces[PSelected].sizep-MINZOOM) + height/b/4) && (height/b*b+height/b*(b-1))/2+height/b/4, (width/c*(c-2)+width/c/2 + PixelOfZoom*(placedPieces[PSelected].sizep-MINZOOM)-height/b/4) < x && y < (height/b*b+height/b*(b-1))/2+height/b/4 && (height/b*b+height/b*(b-1))/2-height/b/4 < y);

    if(condition1)
    {
        closegraph();
        getch();
    }
    if(!condition1 && condition2)
    {
        ScreenNumber=1;
        BackToStartScreen();
    }
    if(!condition1 && !condition2 && condition3)
        DeletePiece();
    if(!condition1 && !condition2 && !condition3 && condition4)
    {
        RestartMenu();
        selectedpiece = y/(height/b)-1;
        ok=3;
    }
    if(!condition1 && !condition2 && !condition3 && !condition4 && condition5)
    {
        if (selectedpiece != -1)
        {
            // Place the selected piece in the space
            Drawing(piese[selectedpiece], x, y, 0, WHITE, zoom);
            placedPieces[nrPlacedPieces].index = selectedpiece;
            placedPieces[nrPlacedPieces].x = x;
            placedPieces[nrPlacedPieces].y = y;
            placedPieces[nrPlacedPieces].rotationangle = 0;
            placedPieces[nrPlacedPieces].sizep = zoom;
            nrPlacedPieces++;
            ok=0;
        }
    }
    if(!condition1 && !condition2 && !condition3 && !condition4 && !condition5 && condition6)
    {
        if(y/(height/b) == 1)
            ResetMapOfPieces();
        if(y/(height/b) == 2)
            SaveMapOfPieces();
        if(y/(height/b) == 3)
            SaveAsMapOfPieces();
        if(y/(height/b) == 4)
            LoadMapOfPieces();
    }
    if(!condition1 && !condition2 && !condition3 && !condition4 && !condition5 && !condition6 && condition7 && ok==4)
    {
        if(condition9)
            Pok=1;
        if(condition10)
            Pok=2;
    }
    if(!condition1 && !condition2 && !condition3 && !condition4 && !condition5 && !condition6 && !condition7 && condition8 && (ok==0 || ok==4))
    {
        if(ok == 4)
            DeselectPiece();
        ok = 0;
        for(int i = 0 ; i < nrPlacedPieces; i++)
            if( x < placedPieces[i].x + placedPieces[i].sizep*1.5 && placedPieces[i].x - placedPieces[i].sizep*1.5 < x && y < placedPieces[i].y + placedPieces[i].sizep*1.5 && placedPieces[i].y - placedPieces[i].sizep*1.5 < y )
            {
                PSelected = i;
                ok=4;
                break;
            }
        if(ok == 0)
            DeselectPiece();
        else
        {
            SelectPiece();
            PropertiesMenu();
        }
    }
    if(!condition1 && !condition2 && !condition3 && !condition4 && !condition5 && !condition6 && condition7)
    {
    }
    }
    else if(ScreenNumber=3)
    {
        if (ismouseclick(WM_LBUTTONDOWN))
    {
        int mx, my;
        getmouseclick(WM_LBUTTONDOWN, mx, my);


        if (mx > width - 80 && mx < width - 20 && my > 20 && my < 60)
        {
            ScreenNumber=1;
            BackToStartScreen();
            return;
        }
        else
        {
            clearmouseclick(WM_LBUTTONDOWN);
        }
    }
    }
}



void Move_handler(int x, int y)
{
    if(ScreenNumber==2){

    if(ok == 4 && y < height/b && (x < width/c || (x < width/c*3 && width/c*2 < x)))
        DeselectPiece();
    if(Pok > 0)
    {
        if(Pok == 1)
        {
            if(x-width/c*(c-3) >= 0 && x <= width/c*(c-2))
            {
                placedPieces[PSelected].rotationangle = ((x-width/c*(c-3))/PixelOfRotation);
                RestartMenu();
            }
        }
        if(Pok == 2)
        {
            if(width/c*(c-2)+width/c/2 <= x && x <= width/c*(c-1)+width/c/2)
            {
                placedPieces[PSelected].sizep = ((x-(width/c*(c-2)+width/c/2))/PixelOfZoom)+MINZOOM;;
                RestartMenu();
            }
        }
    }
    if(x < width/c && 0 < x && 0 < y && y < height/b) //hover over first button to open the list of pieces
        for(int i = 0; i < nrPiese; i++)
        {
            DrawButton(0, height/b*(i+1), width/c, height/b*(i+2));
            Drawing(piese[i], width/c/2, (height/b*(i+1)+height/b*(i+2))/2, 0, WHITE, zoom);
            ok=1;
        }
    else
    {
        if(ok == 1 && x < width/c && 0 < x && height/b-5 < y && y < height/b*(nrPiese+1)) //keep that list of pieces open when hovering over that list/first button
        {
            return;
        }
        else if(ok == 1) //if we are not hovering over that list/first button it will dissapear
            RestartMenu();

    }
    if(x < width/c*3 && width/c*2 < x && 0 < y && y < height/b && ok!=1) // same logic as the first button, but we have the workspace tools in here
        for(int i = 0; i < 4; i++)
        {
            DrawButton(width/c*2, height/b*(i+1), width/c*3, height/b*(i+2));
            printtext(width/c*5/2, (height/b*(i+1)+height/b*(i+2))/2, bworkspace[i]);
            ok=2;
        }
    else if(ok == 2 && x < width/c*3 && width/c*2 < x && height/b-5 < y && y < height/b*5)
        return;
    else if(ok == 2)
        RestartMenu();
        }
}


void LDBLCLICK_handler(int x, int y)
{
    if (isDragging && PSelected != -1)
    {
        placedPieces[PSelected].x = x;
        placedPieces[PSelected].y = y;
        RestartMenu();
    }
}

void RClick_handler(int x, int y)
{
    if (PSelected != -1)
    {
        // Check if the mouse double-click is within the boundaries of the selected piece
        if (x > placedPieces[PSelected].x - placedPieces[PSelected].sizep * 1.5 &&
            x < placedPieces[PSelected].x + placedPieces[PSelected].sizep * 1.5 &&
            y > placedPieces[PSelected].y - placedPieces[PSelected].sizep * 1.5 &&
            y < placedPieces[PSelected].y + placedPieces[PSelected].sizep * 1.5) {
            DeselectPiece();
            isDragging = false;
        }
    }
}

void LUPclick_handler(int x, int y)
{
    if (Pok > 0)
    {
        Pok = 0;
    }

    isDragging = false;
}

void initializare()
{
    DrawMainMenu();
    registermousehandler(WM_LBUTTONDOWN, Lclick_handler);
    registermousehandler(WM_LBUTTONUP, LUPclick_handler);
    registermousehandler(WM_LBUTTONDBLCLK, LDBLCLICK_handler);
    registermousehandler(WM_RBUTTONDOWN, RClick_handler);
    registermousehandler(WM_MOUSEMOVE, Move_handler);
    delay(4000000);
}

int main()
{
    string path = R"(C:\Users\miha\Desktop\stari ale proiectului\piesele refacute)";
    bool inMainMenu = true;
    bool exitProgram = false;

    initwindow(width, height, "Electron");
    incarcapiesele(path);
    initializare();

    return 0;
}

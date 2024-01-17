#include <graphics.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#define MAX1 10
#define MAX2 25
#define MAX3 3
#define MAX_PLACED_PIECES 25
#define MAXZOOM 55.0
#define MINZOOM 15.0
#define PI 3.1415
#define MCOLOR1 COLOR(15,255,80) //neon green
#define MCOLOR2 COLOR(4, 55, 242) // blue
#define MCOLOR3 COLOR (31, 81, 255) // neon blue

using namespace std;

float zoom = MINZOOM;


struct Connections
{
    int PieceNumber = -1;
    int NodeNumber = -1;
};

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
    Connections node[MAX3];
};

piesa piese[MAX2];
MapOfSavedPieces placedPieces[MAX_PLACED_PIECES];
Connections PreviousSelectedNode;
int maxp=-1;
int minp;
int nrPiese = 0;
int nrPlacedPieces = 0;
int width = 1920, height = 1080; // 16:9 resolutions: 1920 x 1080, 1600 x 900, 1280 x 720
int selectedpiece = 0;
int PSelected = -1;
int c = 8; //number of buttons
int b = 12; //by what we divide height and some other variables, to create parts of the menu
int ok; //check for handling mouse events #1
int Pok = 0; //check for handling mouse events #2
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
bool inMainMenu = true;
int ScreenNumber=1;
bool isDragging=false;
bool pieceSelected=false;
int mouseXOnPress, mouseYOnPress;
float pieceXOnPress, pieceYOnPress;
bool movePiece=false;
int selectedPieceIndex=-1;


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
    theta = theta * PI / 180.0;
    float tempX = x;
    x = x * cos(theta) - y * sin(theta);
    y = tempX * sin(theta) + y * cos(theta);
}

void RotateRectangle(float x, float y, float width, float height, float angle)
{
    float halfWidth = width / 2;
    float halfHeight = height / 2;

    float cosA = cos(angle * PI / 180.0);
    float sinA = sin(angle * PI / 180.0);

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
    fillstyle(Color);
    for(int i = 0; i < P.NumberOfNodes; i++)
    {
        float x_2 = P.nodpiesa[i].x;
        float y_2 = P.nodpiesa[i].y;
        Rotate(x_2, y_2, angle);
        fillellipse(x+x_2*sizedrawing,y+y_2*sizedrawing, sizedrawing/5,sizedrawing/5);
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
    setlinestyle(0, 0, 4);
    line(width/c*3 + width/c/c, yline/2, width/c*3+width/c/2 - width/c/c, yline/2); // plus
    line(width/c*3 +width/c/4, 0 + yline/(b/2), width/c*3 +width/c/4, yline - (b/2)-4);  // plus
    line(width/c*3 + width/c/2 + width/c/c, yline/2, width/c*3 + width/c - width/c/c, yline/2); // minus
    resetstyle();
    for(int i = 0; i < 3; i++)
    {
        DrawButton(width/c*i, 1.5, width/c*(i+1), yline);// /c - how many buttons can fit; 1.5 - for fitting
        printtext((width/c*i+width/c*(i+1))/2,yline/2,bnames[i]);
    }
    DrawButton(width/c*3, 1.5, width/c*3+width/c/2, yline); //button for plus
    DrawButton(width/c*3+width/c/2, 1.5, width/c*4, yline); // button for minus
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
        {
            Drawing(piese[placedPieces[i].index], placedPieces[i].x, placedPieces[i].y, placedPieces[i].rotationangle, placedPieces[i].Color, placedPieces[i].sizep);
        }
    }
}


void PropertiesMenu()
{
    setlinestyle(0,0,5);
    line(width/c*(c-2)+width/c/2,(height/b*b+height/b*(b-1))/2, width/c*(c-1)+width/c/2, (height/b*b+height/b*(b-1))/2);
    line(width/c*(c-3),(height/b*b+height/b*(b-1))/2, width/c*(c-2), (height/b*b+height/b*(b-1))/2);
    setlinestyle(0,0,0);
    circle(width/c*(c-2)+width/c/2 + PixelOfZoom*(placedPieces[PSelected].sizep-MINZOOM), (height/b*b+height/b*(b-1))/2, height/b/4);
    circle(width/c*(c-3) + PixelOfRotation*placedPieces[PSelected].rotationangle, (height/b*b+height/b*(b-1))/2, height/b/4);
    char temp[4][100];
    sprintf(temp[2], "%d", placedPieces[PSelected].rotationangle);
    sprintf(temp[3], "%d", placedPieces[PSelected].sizep);
    strcpy(temp[0], "UNGHI:");
    outtextxy(width/c/2 - textwidth(temp[0]) / 2, (height/b*b+height/b*(b-1))/2 - textheight(temp[0]) / 2, temp[0]);
    outtextxy(width/c/2 + textwidth(temp[0]) / 2, (height/b*b+height/b*(b-1))/2 - textheight(temp[0]) / 2, temp[2]);
    strcpy(temp[1], "ZOOM:");
    outtextxy(width/c/2 + textwidth(temp[0]) / 2 + textwidth(temp[2])*2, (height/b*b+height/b*(b-1))/2 - textheight(temp[1]) / 2, temp[1]);
    outtextxy(width/c/2 + textwidth(temp[0]) / 2 + textwidth(temp[2])*2 + textwidth(temp[1]), (height/b*b+height/b*(b-1))/2 - textheight(temp[1]) / 2,  temp[3]);

}

void DrawLines(float x_1, float y_1, float x_2, float y_2, int size_1)
{
    float x1 = piese[placedPieces[PreviousSelectedNode.PieceNumber].index].nodpiesa[PreviousSelectedNode.NodeNumber].x;
    float y1 = piese[placedPieces[PreviousSelectedNode.PieceNumber].index].nodpiesa[PreviousSelectedNode.NodeNumber].y;
    Rotate(x1,y1,placedPieces[PreviousSelectedNode.PieceNumber].rotationangle);
    line(x_1 + size_1*x1, y_1 + size_1*y1, x_1 + size_1*x1, (y_1 + size_1*y1 + y_2)/2);
    line(x_1 + size_1*x1, (y_1 + size_1*y1 + y_2)/2, x_2, (y_1 + size_1*y1 + y_2)/2);
    line(x_2, (y_1 + size_1*y1 + y_2)/2, x_2, y_2);
}

void DrawConnections()
{
    for(int i = 0; i < nrPlacedPieces; i++)
        for(int j = 0; j < piese[placedPieces[i].index].NumberOfNodes; j++)
            if(placedPieces[i].node[j].PieceNumber!= -1 && i < placedPieces[i].node[j].PieceNumber)
            {
                float x1 = piese[placedPieces[i].index].nodpiesa[j].x; //coordonatele nodului piesei in care s-a gasit legatura
                float y1 = piese[placedPieces[i].index].nodpiesa[j].y; //coordonatele nodului piesei in care s-a gasit legatura
                float x2 = piese[placedPieces[placedPieces[i].node[j].PieceNumber].index].nodpiesa[placedPieces[i].node[j].NodeNumber].x; //coordonatele nodului celelaltei piese in care avem legatura
                float y2 = piese[placedPieces[placedPieces[i].node[j].PieceNumber].index].nodpiesa[placedPieces[i].node[j].NodeNumber].y; //coordonatele nodului celelaltei piese in care avem legatura
                Rotate(x1, y1, placedPieces[i].rotationangle);
                Rotate(x2, y2, placedPieces[placedPieces[i].node[j].PieceNumber].rotationangle);
                //line(placedPieces[i].x + placedPieces[i].sizep*x1, placedPieces[i].y + placedPieces[i].sizep*y1, placedPieces[placedPieces[i].node[j].PieceNumber].x + placedPieces[placedPieces[i].node[j].PieceNumber].sizep*x2, placedPieces[placedPieces[i].node[j].PieceNumber].y + placedPieces[placedPieces[i].node[j].PieceNumber].sizep*y2); //this one draws one line straight from the connection of one point to the another one                int x_1 = placedPieces[i].x; // coordonatele piesei in care s-a gasit legatura
                int x_1 = placedPieces[i].x; // coordonatele piesei in care s-a gasit legatura
                int y_1 = placedPieces[i].y; // coordonatele piesei in care s-a gasit legatura
                int x_2 = placedPieces[placedPieces[i].node[j].PieceNumber].x; // coordonatele piesei cu care se face legatura
                int y_2 = placedPieces[placedPieces[i].node[j].PieceNumber].y; // coordonatele piesei cu care se face legatura
                int size_1 = placedPieces[i].sizep; // marimea piesei
                int size_2 = placedPieces[placedPieces[i].node[j].PieceNumber].sizep; // marimea celei de-a doua piesa
                line(x_1 + size_1*x1, y_1 + size_1*y1, x_1 + size_1*x1, (y_1 + size_1*y1 + y_2 + size_2*y2)/2);
                line(x_1 + size_1*x1, (y_1 + size_1*y1 + y_2 + size_2*y2)/2, x_2 + size_2*x2, (y_1 + size_1*y1 + y_2 + size_2*y2)/2);
                line(x_2 + size_2*x2, (y_1 + size_1*y1 + y_2 + size_2*y2)/2, x_2 + size_2*x2, y_2 + size_2*y2);

            } //"x_1 + size_1*x1" "y_1 + size_1*y1"      , "x_2 + size_2*x2"  "y_2 + size_2*y2"
}

void RestartMenu()
{
    cleardevice();
    Menu();
    DrawPlacedPieces();
    if(Pok > 0)
    {
        PropertiesMenu();
        ok = 4;
    }
    else
        ok = 0;
    DrawConnections();
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
    overwrite = false;
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
    for(int i = 0 ; i < nrPlacedPieces; i++)
    {
        for(int j = 0; j < piese[placedPieces[i].index].NumberOfNodes; j++)
        {
            placedPieces[i].node[j].NodeNumber = -1;
            placedPieces[i].node[j].PieceNumber = -1;
        }
    }
    nrPlacedPieces = 0;
    RestartMenu();
    cout << "Map resetted" << endl;
}

void DeselectPiece()
{
    placedPieces[PSelected].Color = WHITE;
    PSelected = -1;
    RestartMenu();
}

void SelectPiece()
{
    placedPieces[PSelected].Color = ColorSelected();;
    RestartMenu();
    ok = 4;
}

void NullTheConnections(int i)
{
    for(int j = 0; j < piese[placedPieces[i].index].NumberOfNodes; j++)
    {
        placedPieces[placedPieces[i].node[j].PieceNumber].node[placedPieces[i].node[j].NodeNumber].NodeNumber = -1;
        placedPieces[placedPieces[i].node[j].PieceNumber].node[placedPieces[i].node[j].NodeNumber].PieceNumber = -1;
        placedPieces[i].node[j].NodeNumber = -1;
        placedPieces[i].node[j].PieceNumber = -1;
    }
}

void DeletePiece()
{
    if (PSelected != -1)
    {
        NullTheConnections(PSelected);
        placedPieces[PSelected].index = -1;
        DeselectPiece();
        RestartMenu();
    }
}

void HelpScreen()
{
    cleardevice();

    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);

    settextstyle(BOLD_FONT, HORIZ_DIR, 1);
    printtext(width / 2, height / 8, "Ajutor");

    printtext(width / 2, height / 4 - 5, "Left Click = Selectarea Piesei");
    printtext(width / 2, height / 4 + 25, "Double Left Click = Selectare Piesa pentru Mutare");
    printtext(width / 2, height / 4 + 55, "Middle Click = Deselectarea Piesei dupa Mutare");

    printtext(width / 2, height / 4 + 100, "Atunci cand piesa este selectata, doua slidere");
    printtext(width / 2, height / 4 + 120, "vor aparea: unul pentru zoom si unul pentru unghi");

    printtext(width / 2, height / 4 + 155, "Sterge = Stergerea unei piese si a");
    printtext(width / 2, height / 4 + 185, "tuturor legaturilor cu piesa respectiva");

    printtext(width / 2, height / 4 + 235, "Sterge Tot = Curatarea ecranului de toate");
    printtext(width / 2, height / 4 + 265, "piesele si legaturile");

    printtext(width / 2, height / 4 + 315, "Salveaza = Salvarea progresului facut in");
    printtext(width / 2, height / 4 + 345, "fisierul de lucru");

    printtext(width / 2, height / 4 + 395, "Salveaza Ca = Crearea unui nou fisier in");
    printtext(width / 2, height / 4 + 425, "care va fi salvat schema electrica");

    printtext(width / 2, height / 4 + 475, "Incarca = Deschiderea unui fisier deja creat");
    printtext(width / 2, height / 4 + 505, "si lucrarea in acesta");

    DrawButton(width - 80, 20, width - 20, 60);
    printtext(width - 50, 40, "Inapoi");

}

int maxsize()
{
    int temp=-1;
    if(nrPlacedPieces != 0)
    {
        for(int i = 0; i < nrPlacedPieces; i++)
            if(placedPieces[i].sizep > temp)
                temp = placedPieces[i].sizep;
        return temp;
    }

}

int minsize()
{
    int temp=MAXZOOM;
    if(nrPlacedPieces != 0)
    {
        for(int i = 0; i < nrPlacedPieces; i++)
            if(placedPieces[i].sizep < temp)
                temp = placedPieces[i].sizep;
        return temp;
    }
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
                RestartMenu();
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

        bool condition1 = (x < width/c*c && width/c*(c-1) < x && 0 < y && y < height/b); //condition for finding the close button
        bool condition2 = (x < width/c*(c-1) && width/c*(c-2) < x && 0 < y && y < height/b); //condition for finding the back to menu button
        bool condition3 = (x < width/c*2 && width/c < x && 0 < y && y < height/b); //condition for finding the delete button
        bool condition4 = (ok==1 && x < width/c && 0 < x && height/b < y && y < height/b*(nrPiese+1)); // condition for selection a piece from hovering over the first button
        bool condition5 = (ok==3 && height/b < y && y < height/b*(b-1)); // condition for drawing the selected piece in the space it is allowed
        bool condition6 = (ok==2 && x < width/c*3 && width/c*2 < x && height/b < y && y < height/b*5); // condition for accessing the workspace buttons
        bool condition7 = (y > height/b*(b-1)); // access the space under the bottom line drawn on the screen
        bool condition8 = (height/b < y && y < height/b*(b-1)); //access the space where you place the pieces, the space between the two drawn lines on the screen
        bool condition9 = (x < (width/c*(c-3) + PixelOfRotation*placedPieces[PSelected].rotationangle + height/b/4) && (width/c*(c-3) + PixelOfRotation*placedPieces[PSelected].rotationangle - height/b/4) < x && y < (height/b*b + height/b*(b-1))/2 + height/b/4 && (height/b*b + height/b*(b-1))/2 - height/b/4 < y); // condition for finding the slider for rotation
        bool condition10 = (x < (width/c*(c-2) + width/c/2 + PixelOfZoom*(placedPieces[PSelected].sizep-MINZOOM) + height/b/4) && (width/c*(c-2) + width/c/2 + PixelOfZoom*(placedPieces[PSelected].sizep-MINZOOM) - height/b/4) < x && y < (height/b*b + height/b*(b-1))/2 + height/b/4 && (height/b*b + height/b*(b-1))/2 - height/b/4 < y); // condtion for finding the slider for size
        bool condition11 = (x < width/c*3 + width/c/2 && width/c*3 < x && 0 < y && y < height/b); // condition for finding the plus button
        bool condition12 = (x < width/c*4 && width/c*3 + width/c/2 < x && 0 < y && y < height/b); // condition for finding the minus button
        bool condition13 = (PSelected != -1 && x > width/c*2 && width/c < x && height/b*5 < y && y < height/b*6); // condition for wasd movement

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
        if(!condition1 && !condition2 && !condition3 && condition4) // select a piece
        {
            RestartMenu();
            selectedpiece = y/(height/b)-1;
            ok=3;

        }
        if(!condition1 && !condition2 && !condition3 && !condition4 && condition5) // place the selected piece in the space
        {
            Drawing(piese[selectedpiece], x, y, 0, WHITE, zoom);
            placedPieces[nrPlacedPieces].index = selectedpiece;
            placedPieces[nrPlacedPieces].x = x;
            placedPieces[nrPlacedPieces].y = y;
            placedPieces[nrPlacedPieces].rotationangle = 0;
            placedPieces[nrPlacedPieces].sizep = zoom;
            nrPlacedPieces++;
            ok = 0;
        }
        if(!condition1 && !condition2 && !condition3 && !condition4 && !condition5 && condition6) //for clicks from the button instrumente
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
                Pok=1; // rotation angle
            if(condition10)
                Pok=2; // size
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
        if(!condition1 && !condition2 && !condition3 && !condition4 && !condition5 && !condition6 && !condition7 && condition8 && PSelected == -1 && (ok == 0 || ok == 5))
        {

            for(int i = 0 ; i < nrPlacedPieces; i++)
            {
                for(int j = 0; j < piese[placedPieces[i].index].NumberOfNodes; j++)
                {
                    float x_2 = piese[placedPieces[i].index].nodpiesa[j].x;
                    float y_2 = piese[placedPieces[i].index].nodpiesa[j].y;
                    Rotate(x_2, y_2, placedPieces[i].rotationangle);
                    if( x < (placedPieces[i].x + placedPieces[i].sizep*x_2 + placedPieces[i].sizep/5) && placedPieces[i].x + placedPieces[i].sizep*x_2 - placedPieces[i].sizep/5 < x && y < placedPieces[i].y + placedPieces[i].sizep*y_2 + placedPieces[i].sizep/5 && placedPieces[i].y + placedPieces[i].sizep*y_2 - placedPieces[i].sizep/5 < y )
                    {
                        if(ok == 0)
                        {
                            PreviousSelectedNode.PieceNumber = i;
                            PreviousSelectedNode.NodeNumber = j;
                            ok = 5;
                            break;
                        }
                        else if(ok == 5)
                        {
                            if(i != PreviousSelectedNode.PieceNumber)
                            {
                                placedPieces[placedPieces[i].node[j].PieceNumber].node[placedPieces[i].node[j].NodeNumber].NodeNumber = -1;
                                placedPieces[placedPieces[i].node[j].PieceNumber].node[placedPieces[i].node[j].NodeNumber].PieceNumber = -1;
                                placedPieces[i].node[j].NodeNumber = -1;
                                placedPieces[i].node[j].PieceNumber = -1;
                                placedPieces[placedPieces[PreviousSelectedNode.PieceNumber].node[PreviousSelectedNode.NodeNumber].PieceNumber].node[placedPieces[PreviousSelectedNode.PieceNumber].node[PreviousSelectedNode.NodeNumber].NodeNumber].NodeNumber = -1;
                                placedPieces[placedPieces[PreviousSelectedNode.PieceNumber].node[PreviousSelectedNode.NodeNumber].PieceNumber].node[placedPieces[PreviousSelectedNode.PieceNumber].node[PreviousSelectedNode.NodeNumber].NodeNumber].PieceNumber = -1;
                                placedPieces[PreviousSelectedNode.PieceNumber].node[PreviousSelectedNode.NodeNumber].NodeNumber = -1;
                                placedPieces[PreviousSelectedNode.PieceNumber].node[PreviousSelectedNode.NodeNumber].PieceNumber = -1;
                                placedPieces[i].node[j].NodeNumber = PreviousSelectedNode.NodeNumber;
                                placedPieces[i].node[j].PieceNumber = PreviousSelectedNode.PieceNumber;
                                placedPieces[PreviousSelectedNode.PieceNumber].node[PreviousSelectedNode.NodeNumber].NodeNumber = j;
                                placedPieces[PreviousSelectedNode.PieceNumber].node[PreviousSelectedNode.NodeNumber].PieceNumber = i;
                                PreviousSelectedNode.NodeNumber = -1;
                                PreviousSelectedNode.PieceNumber = -1;
                                ok = 0;
                                RestartMenu();
                            }
                        }
                    }

                }
            }
        }

        if(!condition1 && !condition2 && !condition3 && !condition4 && !condition5 && !condition6 && !condition7 && !condition8 && condition11)
        {
            if(maxsize() + 2 <= MAXZOOM)
                for(int i = 0; i < nrPlacedPieces; i++)
                    placedPieces[i].sizep += 2;
            else
                cout<<"MAXIMUM SIZE FOR ONE PIECE HAS BEEN REACHED"<<endl;
            DeselectPiece();
            RestartMenu();
        }
        if(!condition1 && !condition2 && !condition3 && !condition4 && !condition5 && !condition6 && !condition7 && !condition8 && condition12)
        {
            if(minsize() - 2 >= MINZOOM)
                for(int i = 0; i < nrPlacedPieces; i++)
                    placedPieces[i].sizep -= 2;
            else
                cout<<"MINIMUM SIZE FOR ONE PIECE HAS BEEN REACHED"<<endl;
            DeselectPiece();
            RestartMenu();
        }
    }
    else if(ScreenNumber==3)
    {
        if (x > width - 80 && x < width - 20 && y > 20 && y < 60)
        {
            ScreenNumber=1;
            BackToStartScreen();
            return;
        }
    }
}




void Move_handler(int x, int y)
{
    if(ScreenNumber==2)
    {
        if(isDragging && PSelected!=-1)
        {
            placedPieces[PSelected].x=x;
            placedPieces[PSelected].y=y;
            RestartMenu();
        }
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
        {
            for(int i = 0; i < nrPiese; i++)
            {
                DrawButton(0, height/b*(i+1), width/c, height/b*(i+2));
                Drawing(piese[i], width/c/2, (height/b*(i+1)+height/b*(i+2))/2, 0, WHITE, zoom);
                ok=1;
            }
        }
        else
        {
            if(ok == 1 && x < width/c && 0 < x && height/b-5 < y && y < height/b*(nrPiese+1)) //keep that list of pieces open when hovering over that list/first button
            {
                return;
            }
            else if(ok == 1) //if we are not hovering over that list/first button it will dissapear
            {
                RestartMenu();
            }

        }
        if(x < width/c*3 && width/c*2 < x && 0 < y && y < height/b && ok!=1) // same logic as the first button, but we have the workspace tools in here
        {
            for(int i = 0; i < 4; i++)
            {
                DrawButton(width/c*2, height/b*(i+1), width/c*3, height/b*(i+2));
                printtext(width/c*5/2, (height/b*(i+1)+height/b*(i+2))/2, bworkspace[i]);
                ok=2;
            }
        }
        else if(ok == 2 && x < width/c*3 && width/c*2 < x && height/b-5 < y && y < height/b*5)
        {
            return;
        }
        else if(ok == 2)
        {
            RestartMenu();
        }
        /*if(ok == 5)//daca scot asta merge, daca o las, da break la conexiuni
        {
            DrawLines(placedPieces[PreviousSelectedNode.PieceNumber].x,placedPieces[PreviousSelectedNode.PieceNumber].y,x,y,placedPieces[PreviousSelectedNode.PieceNumber].sizep);
            RestartMenu();
        }*/
    }
}

void LUPclick_handler(int x, int y)
{
    if(Pok > 0)
    {
        Pok = 0;
    }
}
void DoubleClick_Handler(int x, int y)
{
    if(PSelected!=-1)
    {
        isDragging=true;
    }
}

void StopDragging(int x, int y)
{
    if(PSelected != -1 && (height/b < y && y < height/b*(b-1)))
    {
        DeselectPiece();
        isDragging=false;
    }
}

void initializare()
{
    DrawMainMenu();
    registermousehandler(WM_LBUTTONUP,LUPclick_handler);
    registermousehandler(WM_LBUTTONDOWN,Lclick_handler);
    registermousehandler(WM_MOUSEMOVE,Move_handler);
    registermousehandler(WM_LBUTTONDBLCLK,DoubleClick_Handler);
    registermousehandler(WM_MBUTTONDOWN,StopDragging);
    delay(4000000);
}

int main()
{
    string path = R"(C:\Users\miha\Documents\CodeBlocks\electron\piese electrice)";
    bool inMainMenu = true;
    bool exitProgram = false;
    initwindow(width, height, "Electron");
    incarcapiesele(path);
    initializare();


    return 0;
}

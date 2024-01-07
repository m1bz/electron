#include <graphics.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#define MAX1 10
#define MAX2 25
#define MAX_PLACED_PIECES 25
#define PI 3.14159
#define MCOLOR1 COLOR(15,255,80) //neon green
#define MCOLOR2 COLOR(4, 55, 242) // blue
#define MCOLOR3 COLOR (31, 81, 255) // neon blue

using namespace std;

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
    char name[100];
    int nrnod;
    xynodpiese nodpiesa[MAX1];
    int nrcomenzidesen;
    Desen comanda[MAX2];
};

struct PlacedPiece
{
    int index;
    float x, y;
    int rotation;
};

piesa piese[MAX2];
PlacedPiece placedPieces[MAX_PLACED_PIECES];
int nrPiese = 0;
int nrPlacedPieces = 0;
int width = 1600, height = 900; // 16:9 resolutions: 1920 x 1080, 1600 x 900, 1280 x 720
float zoom = 20.0;
int c=8; //number of buttons
int b=15; //by what we divide height and other variables
const char *bnames[] = { "INTRODUCE", /*TEXT #1 BUTTON*/ "STERGE", /*TEXT #2 BUTTON*/
                         "INSTRUMENTE", /*TEXT #3 BUTTON*/ "", /*TEXT #4 BUTTON*/
                         "", /*TEXT #5 BUTTON*/ "", /*TEXT #6 BUTTON*/
                         "INAPOI", /*TEXT #7 BUTTON*/ "IESI", /*TEXT #8 BUTTON*/
                       };
const char *bcarac[2]= {"UNGHI:", "MARIME:"};

void citirePiesa(const string& filePath, piesa& p)
{
    ifstream file(filePath);

    file >> p.name;
    file >> p.nrnod;

    for (int i = 0; i < p.nrnod; i++)
        file >> p.nodpiesa[i].x >> p.nodpiesa[i].y;

    file >> p.nrcomenzidesen;
    for (int i = 0; i < p.nrcomenzidesen; ++i)
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
    setcolor(MCOLOR3);
}

void resetstyle() //used to change from the menu style to normal style
{
    setlinestyle(0,0,0);
    setcolor(WHITE);
}

void roteste(float &x, float &y, float theta)
{
    float dx = x;
    float dy = y;
    x = dx*cos(theta) - dy*sin(theta);
    y = dx*sin(theta) + dy*cos(theta);
}

void DesenLinie(piesa P, int i, float x, float y, float unghi)
{
    unghi = unghi * PI / 180;
    float x_1 = P.comanda[i].x1 ;
    float y_1 = P.comanda[i].y1 ;
    float x_2 = P.comanda[i].x2 ;
    float y_2 = P.comanda[i].y2 ;
    roteste(x_1, y_1, unghi);
    roteste(x_2, y_2, unghi);
    line(x_1 * zoom + x, y_1 * zoom + y, x_2 * zoom + x, y_2 * zoom + y) ;
}

void DesenDreptunghi(piesa P, int i, float x, float y, float unghi)
{
    unghi = unghi * PI / 180;
    float x_1 = P.comanda[i].x1;
    float y_1 = P.comanda[i].y1;
    float x_2 = P.comanda[i].x2;
    float y_2 = P.comanda[i].y2;
    roteste(x_1, y_1, unghi);
    roteste(x_2, y_2, unghi);
    rectangle(x_1 * zoom + x, y_1 * zoom + y, x_2 * zoom + x, y_2 * zoom + y);
}

void DrawCircle(piesa P,int i, float x, float y)
{
    circle(x+P.comanda[i].x2*zoom,y+P.comanda[i].y2*zoom,P.comanda[i].x1*zoom);
}

void DrawButton(int xtop, int ytop, int xbottom, int ybottom)
{
    menustyle();
    rectangle(xtop, ytop, xbottom, ybottom);
    setlinestyle(0,0,1);
    rectangle(xtop+2, ytop+2, xbottom-3, ybottom-3);
    resetstyle();
}

void desen(piesa P, float x, float y, float unghi)
{
    for(int i = 0; i < P.nrcomenzidesen; i++)
        switch(P.comanda[i].tipfigura)
        {
        case 'L':
            DesenLinie(P, i, x, y, unghi);
            break;
        case 'R':
            DesenDreptunghi(P, i, x, y, unghi);
            break;
        case 'O':
            DrawCircle(P,i,x,y);
            break;
        }
}

void printtext(int x, int y, const char* text)
{
    settextstyle(3,HORIZ_DIR,3);
    setcolor(MCOLOR3);
    char temp[100];
    strcpy(temp, text);
    outtextxy(x - textwidth(temp) / 2, y - textheight(temp) / 2, temp);
    setcolor(WHITE);
}

void prelucrarepartesus()
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

void Backtomenu()
{

}

void CheckIfPieceIsSelected()
{

}

void DeletePiece()
{
    CheckIfPieceIsSelected();
}


void Lclick_handler(int x, int y)
{
    if(x < width/c*c && width/c*(c-1) < x && 0 < y && y < height/b)
    {
        closegraph();
        getch();
    }
    if(x < width/c*(c-1) && width/c*(c-2) < x && 0 < y && y < height/b)
        Backtomenu();
    if(x < width/c*2 && width/c < x && 0 < y && y < height/b)
        DeletePiece();
}

void Move_handler(int x, int y)
{
    int ok=1;
    if(x < width/c && 0 < x && 0 < y && y < height/b)
        for(int i = 0; i < nrPiese; i++)
        {
            DrawButton(0, height/b*(i+1), width/c, height/b*(i+2));
            desen(piese[i], width/c/2, (height/b*(i+1)+height/b*(i+2))/2, 0);
            ok=0;
        }
        else
         {
            if(ok==0 && x < width/c && 0 < x && 0 < y && y < height/b*(nrPiese+1))
                outtextxy(400,400,"keep");
            else
            {
                cleardevice();
                prelucrarepartesus();
            }
        }
    if(x < width/c && 0 < x && 0 < y && y < height/b)
    {

    }
}

void initializare()
{
    initwindow(width, height);
    registermousehandler(WM_LBUTTONDOWN,Lclick_handler);
    registermousehandler(WM_MOUSEMOVE,Move_handler);
    delay(4000000);// so it doesnt instantly close the program
}

int main()
{
    string path = R"(C:\Users\miha\Documents\CodeBlocks\electron\piese electrice)";
    incarcapiesele(path);
    initializare();
    return 0;
}

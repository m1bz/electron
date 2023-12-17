#include <iostream>
#include <fstream>
#include <filesystem>
#include <graphics.h>
#include <cmath>

#define MAX1 10 // nr maxim de puncte de legatura pe care le poate avea o piesa
#define MAX2 50 // nr maxim de comenzi de desenare pentru realizarea unei piese si nr maxim de piese pe care il putem incarca
#define PI 3.14159 //self explanatory

using namespace std;


struct xynodpiese // structura unde se salveaza coordonatele punctului de legatura al unei piese
{
    float x,y;
};
struct desen // salveaza tipul de desen si coordonatele
{
    char tipfigura;
    float x1,y1,x2,y2;
};

struct piesa // contine informatiile unei piese
{
    char name[100]; // numele piesei
    int nrnod; // cate puncte de legatura are nodul respectiv
    xynodpiese nodpiesa[MAX1]; // accesam pozitia punctului de legatura x,y
    int nrcomenzidesen; // nr comenzilor de desen pentru a realiza piesa respectiva
    desen comanda[MAX2];
};

piesa piese[MAX2]; // vectorul in care salvam piesele vectorul este piese nu piesa()
int nrPiese = 0;
int width = 1600, height = 900;
int zoom = 10.0;
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


void roteste(float &x, float &y, float theta) {
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

void DesenElipsa(piesa P,int i, float x, float y,float unghi)
{
    float x_1 = P.comanda[i].x1;
    float y_1 = P.comanda[i].y1;
    float x_2 = P.comanda[i].x2;
    float y_2 = P.comanda[i].y2;
    roteste(x_1, y_1, unghi);
    ellipse(x_1 * zoom + x, y_1 * zoom + y, 0, 360, x_2 * zoom, y_2 * zoom);
}

void DesenArc(piesa P,int i, float x, float y,float unghi)
{
    float x_1 = P.comanda[i].x1;
    float y_1 = P.comanda[i].y1;
    float x_2 = P.comanda[i].x2;
    float y_2 = P.comanda[i].y2;
    roteste(x_1, y_1, unghi);
    arc(x_1 * zoom + x, y_1 * zoom + y, -90, 90, x_2 * zoom);
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
            DesenElipsa(P, i, x, y, unghi);
            break;
        case 'A':
            DesenArc(P, i, x, y, unghi);
            break;
        }
}


void prelucrarepartesus()
{
    int ylinie = height * 10 / 100;
    line(0, ylinie, width, ylinie);
    for(int i = 0; i <= nrPiese; i++)
    {
        line((float)width / nrPiese * i, 0,(float)width / nrPiese * i, ylinie);
        for(int j = 0; j < piese[i].nrcomenzidesen; j++)
        {
            desen(piese[i], (float)width / nrPiese *(i+0.5), ylinie / 2, 50);// aici modifici 0 daca vrei sa schimbi unghiunl pieselor din meniu, nu recomand totusi
        }
    }
}



void initializare()
{
    prelucrarepartesus();
    setcolor(WHITE);
}



int main()
{
    string path = R"(C:\Users\miha\Documents\CodeBlocks\electron\piese electrice)"; // replace with your directory path
    incarcapiesele(path);
    initwindow(width,height);
    initializare();
    getch();
    closegraph();
    return 0;
}

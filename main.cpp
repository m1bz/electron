#include <graphics.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#define MAX1 10
#define MAX2 25
#define MAX_PLACED_PIECES 25
#define PI 3.1415
#define MCOLOR1 COLOR(15,255,80) //neon green
#define MCOLOR2 COLOR(4, 55, 242) // blue
#define MCOLOR3 COLOR (31, 81, 255) // neon blue

using namespace std;

float zoom = 25.0;

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
int c=8; //number of buttons
int b=12; //by what we divide height and some other variables
int ok; //check for handling mouse events
const char *bnames[] = { "INTRODUCE", /*TEXT #1 BUTTON*/ "STERGE", /*TEXT #2 BUTTON*/
                         "INSTRUMENTE", /*TEXT #3 BUTTON*/ "", /*TEXT #4 BUTTON*/
                         "", /*TEXT #5 BUTTON*/ "", /*TEXT #6 BUTTON*/
                         "INAPOI", /*TEXT #7 BUTTON*/ "IESI", /*TEXT #8 BUTTON*/
                       };
const char *bcarac[2] = {"UNGHI:", "MARIME:"};
const char *bworkspace[4] = {"STERGE TOT", "SALVEAZA", "SALVEAZA CA", "INCARCA"};
string lastLoadedFilePath;

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
    setcolor(MCOLOR3);
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
    float dx = x;
    float dy = y;
    x = dx*cos(theta) - dy*sin(theta);
    y = dx*sin(theta) + dy*cos(theta);
}

void DrawLine(piesa P, int i, float x, float y, float angle)
{
    angle = angle * PI / 180;
    float x_1 = P.comanda[i].x1 ;
    float y_1 = P.comanda[i].y1 ;
    float x_2 = P.comanda[i].x2 ;
    float y_2 = P.comanda[i].y2 ;
    Rotate(x_1, y_1, angle);
    Rotate(x_2, y_2, angle);
    line(x_1 * zoom + x, y_1 * zoom + y, x_2 * zoom + x, y_2 * zoom + y) ;
}

void DrawRectangle(piesa P, int i, float x, float y, float angle)
{
    angle = angle * PI / 180;
    float x_1 = P.comanda[i].x1;
    float y_1 = P.comanda[i].y1;
    float x_2 = P.comanda[i].x2;
    float y_2 = P.comanda[i].y2;
    Rotate(x_1, y_1, angle);
    Rotate(x_2, y_2, angle);
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

void Drawing(piesa P, float x, float y, float angle, int Color)
{
    //rectangle(1.5*zoom+x,1.5*zoom+y,-1.5*zoom+x,-1.5*zoom+y); // imaginary rectangle where if you click you select the piece basically
    setcolor(Color);
    fillstyle(Color);//need to introduce in the antet of the function a int like Color, for selecting
    for(int i = 0; i < P.NumberOfNodes; i++)
    {
        circle(x+P.nodpiesa[i].x*zoom,y+P.nodpiesa[i].y*zoom, zoom/5);
        floodfill(x+P.nodpiesa[i].x*zoom,y+P.nodpiesa[i].y*zoom,Color);
    }
    for(int i = 0; i < P.NumberOfDrawingCommands; i++)
        switch(P.comanda[i].tipfigura)
        {
        case 'L':
            DrawLine(P, i, x, y, angle);
            break;
        case 'R':
            DrawRectangle(P, i, x, y, angle);
            break;
        case 'O':
            DrawCircle(P,i,x,y);
            break;
        }
    setcolor(WHITE);
    fillstyle(WHITE);
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

void BacktoStartScreen()
{

}

void DrawPlacedPieces()
{
    if(nrPlacedPieces!=0)
    {
        for(int i = 0; i < nrPlacedPieces; i++)
            Drawing(piese[placedPieces[i].index], placedPieces[i].x, placedPieces[i].y, placedPieces[i].rotationangle, placedPieces[i].Color);
    }
}

void CheckIfPieceIsSelected()
{

}

void RestartMenu()
{
    cleardevice();
    Menu();
    DrawPlacedPieces();
    ok=0;
}

void DeletePiece()
{
    CheckIfPieceIsSelected();
}

void SaveMapToFile(const string& filePath, bool overwrite)
{
    if (!overwrite && std::filesystem::exists(filePath))
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

void DeselectPiece()
{
    cout<<endl<<"se deselecteaza ok = 0";
    placedPieces[PSelected].Color=WHITE;
    RestartMenu();
    PSelected = -1;
}

void SelectPiece()
{
    cout<<endl<<"se selecteaza ok = 4";
    placedPieces[PSelected].Color=MCOLOR3;
    RestartMenu();
    ok=4;
}

void Lclick_handler(int x, int y)
{
    bool condition1 = (x < width/c*c && width/c*(c-1) < x && 0 < y && y < height/b); //condition for finding the close button
    bool condition2 = (x < width/c*(c-1) && width/c*(c-2) < x && 0 < y && y < height/b); //condition for finding the back to menu button
    bool condition3 = (x < width/c*2 && width/c < x && 0 < y && y < height/b); //condition for finding the delete button
    bool condition4 = (ok==1 && x < width/c && 0 < x && height/b < y && y < height/b*(nrPiese+1)); // condition for selection a piece from hovering over the first button
    bool condition5 = (ok==3 && height/b < y && y < height/b*(b-1)); // condition for drawing the selected piece in the space it is allowed
    bool condition6 = (ok==2 && x < width/c*3 && width/c*2 < x && height/b < y && y < height/b*5); // condition for accessing the workspace buttons
    bool condition7 = (y > height/b*(b-1)); // access the space under the bottom line drawn on the screen
    bool condition8 = (height/b < y && y < height/b*(b-1)); //access the space where you place the pieces, the space between the two drawn lines on the screen
    if(condition1)
    {
        closegraph();
        getch();
    }
    if(!condition1 && condition2)
        BacktoStartScreen();
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
        Drawing(piese[selectedpiece], x, y, 0, WHITE);
        placedPieces[nrPlacedPieces].index = selectedpiece;
        placedPieces[nrPlacedPieces].x = x;
        placedPieces[nrPlacedPieces].y = y;
        placedPieces[nrPlacedPieces].rotationangle = 0;
        nrPlacedPieces++;
        ok=0;
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
        cout<<endl<<"am intrat in partea de setari de zoooooooooomn";
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
                cout<<endl<<"AM APASAT PE PIESA CU NUMARUL:"<<i<<"   nrplacedpieces:"<<nrPlacedPieces;
                ok=4;
                break;
            }
        if(ok == 0)
            DeselectPiece();
        else
            SelectPiece();
        cout<<endl<<PSelected<<" "<<ok;
    }

}
void Move_handler(int x, int y)
{
    if(ok == 4 && y < height/b && (x < width/c || (x < width/c*3 && width/c*2 < x)))
        DeselectPiece();
    if(x < width/c && 0 < x && 0 < y && y < height/b) //hover over first button to open the list of pieces
        for(int i = 0; i < nrPiese; i++)
        {
            DrawButton(0, height/b*(i+1), width/c, height/b*(i+2));
            Drawing(piese[i], width/c/2, (height/b*(i+1)+height/b*(i+2))/2, 0, WHITE);
            ok=1;
        }
    else
    {
        if(ok==1 && x < width/c && 0 < x && height/b-5 < y && y < height/b*(nrPiese+1)) //keep that list of pieces open when hovering over that list/first button
        {
            return;
        }
        else if(ok==1) //if we are not hovering over that list/first button it will dissapear
            RestartMenu();

    }
    if(x < width/c*3 && width/c*2 < x && 0 < y && y < height/b && ok!=1) // same logic as the first button, but we have the workspace tools in here
        for(int i = 0; i < 4; i++)
        {
            DrawButton(width/c*2, height/b*(i+1), width/c*3, height/b*(i+2));
            printtext(width/c*5/2, (height/b*(i+1)+height/b*(i+2))/2, bworkspace[i]);
            ok=2;
        }
    else if(ok==2 && x < width/c*3 && width/c*2 < x && height/b-5 < y && y < height/b*5)
        return;
    else if(ok==2)
        RestartMenu();
}

void initializare()
{
    initwindow(width, height);
    RestartMenu();
    registermousehandler(WM_LBUTTONDOWN,Lclick_handler);
    registermousehandler(WM_MOUSEMOVE,Move_handler);
    delay(4000000);// so it doesnt instantly close the program
}

int main()
{
    string path = R"(C:\Users\miha\Desktop\stari ale proiectului\piesele refacute)";
    incarcapiesele(path);
    initializare();
    return 0;
}

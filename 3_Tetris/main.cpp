#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

#define out(x) std::cout<<x;
#define enter out('\n')
#define space out(' ')








int initWinX = 700;
int initWinY = 700;
int winX = initWinX;
int winY = initWinY;
int gridX = 25;
int gridY = 25;
SDL_Rect gameGridRect = {2,2,11,21};
SDL_FRect gameGridMask;
float gridXSize;
float gridYSize;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Event event;
TTF_Font* gFont = NULL;


void PiecesRedefine();
void ToggleFullscreen(SDL_Window* Window) {
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    auto Width = DM.w;
    auto Height = DM.h;

    Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
    bool IsFullscreen = SDL_GetWindowFlags(Window) & FullscreenFlag;
    if (IsFullscreen){
        winX = initWinX;
        winY = initWinY;
    }else{
        winX = DM.w;
        winY = DM.h;
    }

    gridXSize = float(winX) / gridX;
    gridYSize = float(winY) / gridY;
    // SDL_RenderSetScale(renderer,winX,winY);


    gameGridMask.x = gameGridRect.x * gridXSize;
    gameGridMask.y = gameGridRect.y * gridYSize;
    gameGridMask.w = gameGridRect.w * gridXSize;
    gameGridMask.h = gameGridRect.h * gridYSize;

    SDL_SetWindowFullscreen(Window, ~IsFullscreen);

    SDL_DestroyRenderer(renderer);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    PiecesRedefine();
    // SDL_ShowCursor(IsFullscreen);
}

std::string PayamText1 = "Score : ";
std::string PayamText2[2] = {"Pieces : ","/ Balance : "};
SDL_Texture* PayamTexture1;
SDL_Rect PayamRect1;
SDL_Texture* PayamTexture2;
SDL_Rect PayamRect2;
int Score = 1;
int totalPieces = 0;
bool shouldRun = true;
bool showGrid = true;
int tickSpeed = 450;
enum : unsigned char{
    UP,DOWN,
    RIGHT,LEFT,
    ENTER,F1
};

bool heldKeys[6];

struct Pos{
    int x;
    int y;
};

Pos MousePos = {0,0};
struct Logo{
    std::vector<Pos> array;
    int maxX;
    int maxY;
};

SDL_Color colorArray[12] = {
    {200,40,40},
    {40,200,40},
    {40,40,200},

    {200,200,40},
    {200,40,200},
    {40,200,200},

    {200,120,80},
    {120,200,80},

    {40,200,120},
    {40,120,200},

    {200,40,120},
    {120,40,200},
};

Logo logoArray[12];

bool isInCanvas(Logo logo,SDL_FRect rect,SDL_Rect canvas){

    for (int i=0;i!=logo.array.size();i++){
        if (logo.array[i].x + rect.x < canvas.x){return false;}
        if (logo.array[i].y + rect.y < canvas.y){return false;}
        if (logo.array[i].x + rect.x > canvas.x+canvas.w-1){return false;}
        if (logo.array[i].y + rect.y > canvas.y+canvas.h-1){return false;}
    }

    return true;
};

bool isCollided(Logo logo,SDL_FRect rect,int isHolding);
void GeneratePiece();
struct Piece{
    SDL_FRect rect;
    Logo logo;
    SDL_Color color;

    void rotate(){
        Logo tempLogo = logo;
        int maxX,maxY;
        maxX=maxY=0;
        
        for (int i=0;i!=logo.array.size();i++){
            tempLogo.array[i] = {-tempLogo.array[i].y,tempLogo.array[i].x};

            if (tempLogo.array[i].x>maxX) maxX = tempLogo.array[i].x;
            if (tempLogo.array[i].y>maxY) maxY = tempLogo.array[i].y;
        }
        if (isInCanvas(tempLogo,rect,gameGridRect) &&
            !isCollided(tempLogo,rect,1)){
            logo = tempLogo;
            logo.maxX = maxX;
            logo.maxY = maxY;
        }
    }


    void move(unsigned char Dirc){
        SDL_FRect tempRect = rect;
        if (Dirc==RIGHT){tempRect.x+=1;}
        else if(Dirc==LEFT){tempRect.x-=1;}
        else if(Dirc==DOWN){tempRect.y+=1;}
        if (isInCanvas(logo,tempRect,gameGridRect) &&
            !isCollided(logo,tempRect,1)){
            rect = tempRect;
        }else if(Dirc==DOWN){
            heldKeys[DOWN]=false;
            GeneratePiece();
        }

    }
};

std::vector<Piece> pieceArray;

bool isCollided(Logo logo,SDL_FRect rect,int isHolding)
{


    for (int i=0;i!=logo.array.size();i++)
    {
        for (int c=0;c!=pieceArray.size()-isHolding;c++)
        {

           if (pieceArray.size()==0) break;
           for (int j=0;j!=pieceArray[c].logo.array.size();j++)
           {

            if ((pieceArray[c].logo.array[j].x +
                pieceArray[c].rect.x == logo.array[i].x+rect.x)&&
                (pieceArray[c].logo.array[j].y +
                pieceArray[c].rect.y == logo.array[i].y+rect.y))
            {return true;}


           }
        }
    }

    return false;
}

void initLogo(){
    logoArray[0].array.push_back({0,0});
    logoArray[0].array.push_back({0,1});
    logoArray[0].array.push_back({1,0});
    logoArray[0].array.push_back({1,1});
    logoArray[0].maxX = 1;
    logoArray[0].maxY = 1;

    logoArray[1].array.push_back({0,0});
    logoArray[1].array.push_back({1,0});
    logoArray[1].array.push_back({2,0});
    logoArray[1].array.push_back({2,1});
    logoArray[1].maxX = 2;
    logoArray[1].maxY = 1;

    logoArray[2].array.push_back({0,0});
    logoArray[2].array.push_back({1,0});
    logoArray[2].array.push_back({2,0});
    logoArray[2].array.push_back({0,1});
    logoArray[2].maxX = 2;
    logoArray[2].maxY = 1;

    logoArray[3].array.push_back({0,0});
    logoArray[3].array.push_back({1,0});
    logoArray[3].array.push_back({2,0});
    logoArray[3].array.push_back({1,1});
    logoArray[3].maxX = 2;
    logoArray[3].maxY = 1;

    logoArray[4].array.push_back({0,0});
    logoArray[4].array.push_back({1,0});
    logoArray[4].array.push_back({1,1});
    logoArray[4].maxX = 1;
    logoArray[4].maxY = 1;

    logoArray[5].array.push_back({0,0});
    logoArray[5].array.push_back({1,0});
    logoArray[5].array.push_back({1,1});
    logoArray[5].array.push_back({2,1});
    logoArray[5].maxX = 2;
    logoArray[5].maxY = 1;

    logoArray[6].array.push_back({0,1});
    logoArray[6].array.push_back({1,1});
    logoArray[6].array.push_back({1,0});
    logoArray[6].array.push_back({2,0});
    logoArray[6].maxX = 2;
    logoArray[6].maxY = 1;

    logoArray[7].array.push_back({0,0});
    logoArray[7].array.push_back({1,0});
    logoArray[7].array.push_back({2,0});
    logoArray[7].array.push_back({3,0});
    logoArray[7].maxX = 3;
    logoArray[7].maxY = 0;

    logoArray[8].array.push_back({0,0});
    logoArray[8].array.push_back({1,0});
    logoArray[8].array.push_back({2,0});
    logoArray[8].array.push_back({0,1});
    logoArray[8].array.push_back({2,1});
    logoArray[8].maxX = 2;
    logoArray[8].maxY = 1;

    logoArray[9].array.push_back({0,0});
    logoArray[9].array.push_back({0,1});

    logoArray[9].maxX = 2;
    logoArray[9].maxY = 1;

    logoArray[10].array.push_back({1,0});
    logoArray[10].array.push_back({2,0});
    logoArray[10].array.push_back({3,0});

    logoArray[10].maxX = 2;
    logoArray[10].maxY = 1;




};

bool isOccupied(Pos pos,bool ifRemove=false)
{
    for (int i=0;i!=pieceArray.size();i++){
        for (int c=0;c!=pieceArray[i].logo.array.size();c++)
        {
            if ((pieceArray[i].logo.array[c].x + pieceArray[i].rect.x == pos.x)
                &&
                (pieceArray[i].logo.array[c].y + pieceArray[i].rect.y == pos.y))
            {
                if (ifRemove)
                {
                    pieceArray[i].logo.array.erase(
                        pieceArray[i].logo.array.begin()+c);
                }

                return true;
            }
        }
    }
    return false;
}
void FallPieces(int Limit){


    for (int i=0;i!=pieceArray.size();i++){
        for (int c=0;c!=pieceArray[i].logo.array.size();c++)
        {
            if ((pieceArray[i].logo.array[c].y + pieceArray[i].rect.y
                < Limit+1)&&(pieceArray[i].logo.array[c].y +
                pieceArray[i].rect.y <gameGridRect.y+
                gameGridRect.h - 1))
            {

                pieceArray[i].logo.array[c].y++;
            }
        }
    }

}

void PayamNegari();
void CleanRows(){
    bool shouldRemove = false;
    std::vector<int> cleanedRows;
    for (int y=gameGridRect.y;y!=gameGridRect.y+gameGridRect.h;y++)
    {

        for (int x=gameGridRect.x;x!=gameGridRect.x+gameGridRect.w;x++)
        {
            // out(x) space out(y) space out("| ")
            if (!shouldRemove){
                if (!isOccupied({x,y})){break;}
            }else{
                isOccupied({x,y},true);
            }

            if (x==gameGridRect.x+gameGridRect.w-1)
            {
                if (shouldRemove){

                    cleanedRows.push_back(y);
                }else{
                    x = gameGridRect.x-1;
                }

                shouldRemove=true;
            }
        }
        shouldRemove=false;

    }

    float tempScore = 0.25;
    for (int i=0;i!=cleanedRows.size();i++)
    {
        tempScore*=4;
        FallPieces(cleanedRows[i]-1);
    }

    if (cleanedRows.size()!=0){

        Score += tempScore;
        PayamNegari();

    }
}


void GeneratePiece(){


    CleanRows();
    // out(pieceArray.size());
    int num;
    num = rand()%11;
    //out(num) enter


    SDL_FRect  r1 = {float(gameGridRect.x+4)
                    ,float(gameGridRect.y),gridXSize,gridYSize};

    if (!isCollided(logoArray[num],r1,0)){
        pieceArray.push_back({r1,logoArray[num],colorArray[num]});
        totalPieces++;
        PayamNegari();
    }


}


void setColor(SDL_Color cr){
    SDL_SetRenderDrawColor(renderer,cr.r,cr.g,cr.b,255);
}

void PiecesRedefine(){
    for (int i=0;i!=pieceArray.size();i++){
        pieceArray[i].rect.w = gridXSize;
        pieceArray[i].rect.h = gridYSize; 
    }

}
void piecesDraw(){
    SDL_FRect mask={0,0,0,0};
    for (int i=0;i!=pieceArray.size();i++){
        for (int c=0;c!=pieceArray[i].logo.array.size();c++){
            mask.w = pieceArray[i].rect.w;
            mask.h = pieceArray[i].rect.h;
            // out(c) space out(mask.w) space out(mask.h) space
            mask.x = pieceArray[i].rect.x * mask.w +
            pieceArray[i].logo.array[c].x * mask.w;
            mask.y = pieceArray[i].rect.y * mask.h +
            pieceArray[i].logo.array[c].y * mask.h;
            // out(mask.x) space out(mask.y) enter
            setColor(pieceArray[i].color);
            SDL_RenderFillRectF(renderer,&mask);

        }

    }
}



void PayamNegari(){

    std::string tempStr1 = PayamText1 + std::to_string(Score);
        // std::string(" / Balance : ") + std::to_string(myRound(Score/float(totalPieces),2));
    SDL_Surface* tempSurface1 = TTF_RenderText_Solid(gFont,tempStr1.c_str(),{0,0,0});
    PayamTexture1 = SDL_CreateTextureFromSurface(renderer,tempSurface1);



    PayamRect1.w = tempSurface1->w;
    PayamRect1.h = tempSurface1->h;
    PayamRect1.x = ((gameGridRect.x * gridXSize) + ((gameGridRect.w * gridXSize)/2)
                                                        -PayamRect1.w/2);
    // out(PayamRect1.x) enter out(winX) enter

    SDL_FreeSurface(tempSurface1);


}

void DrawPayam(){
    SDL_RenderCopy(renderer,PayamTexture1,NULL,&PayamRect1);

}

void Init(){
    TTF_Init();
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("TETRIS",SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,winX,winY,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    initLogo();
    srand(time(0));
    gFont = TTF_OpenFont("./FreeSans.ttf",38);
    if (gFont==NULL){
        out("Error Opening Font : \n\t")
        out(SDL_GetError()) enter
    }
    gridXSize = float(winX) / gridX;
    gridYSize = float(winY) / gridY;

    GeneratePiece();

    PayamRect1.x = 0;
    PayamRect1.y = 0;
    PayamNegari();

    gameGridMask.x = gameGridRect.x * gridXSize;
    gameGridMask.y = gameGridRect.y * gridYSize;
    gameGridMask.w = gameGridRect.w * gridXSize;
    gameGridMask.h = gameGridRect.h * gridYSize;
};

void FetchEvents(){
    while (SDL_PollEvent(&event)!=0){
        if (event.type==SDL_QUIT){
            shouldRun = false;
        }else if(event.type==SDL_MOUSEMOTION){
         SDL_GetMouseState(&MousePos.x,&MousePos.y);
         PayamNegari();
        }else if(event.type==SDL_KEYDOWN){

            switch (event.key.keysym.sym){
                case SDLK_ESCAPE: shouldRun = false;break;
                case SDLK_UP: heldKeys[UP] = true; break;
                case SDLK_DOWN: heldKeys[DOWN] = true; break;
                case SDLK_RIGHT: heldKeys[RIGHT] = true; break;
                case SDLK_LEFT: heldKeys[LEFT] = true; break;
                case SDLK_RETURN: heldKeys[ENTER] = true; break;
                case SDLK_F1: heldKeys[F1] = true; break;
            }
        }
        else if(event.type==SDL_KEYUP){

            switch (event.key.keysym.sym){

                case SDLK_UP:    if(heldKeys[UP]) heldKeys[UP] = false; break;
                case SDLK_DOWN:  if(heldKeys[DOWN])  heldKeys[DOWN] = false; break;
                case SDLK_RIGHT: if(heldKeys[RIGHT])  heldKeys[RIGHT] = false; break;
                case SDLK_LEFT:  if(heldKeys[LEFT])  heldKeys[LEFT] = false; break;
                case SDLK_RETURN: if(heldKeys[ENTER]) heldKeys[ENTER] = false;break;
                case SDLK_F1:    if(heldKeys[F1])  heldKeys[F1] = false; break;
            }
        }
    }
};

int lastFrame = 0;
int currentFrame = 0;
bool shouldFall = false;
void CheckEvents(){
    currentFrame = SDL_GetTicks();

    if (currentFrame-lastFrame>tickSpeed){
        shouldFall=true;
        lastFrame = SDL_GetTicks();
    }

    if (heldKeys[F1]){heldKeys[F1]=false; showGrid=!showGrid;}
    if (heldKeys[ENTER]){heldKeys[ENTER]=false;ToggleFullscreen(window);}

    if (heldKeys[UP])
    {heldKeys[UP]=false; pieceArray[pieceArray.size()-1].rotate();}
    if (heldKeys[RIGHT])
    {heldKeys[RIGHT]=false; pieceArray[pieceArray.size()-1].move(RIGHT);}
    if (heldKeys[LEFT])
    {heldKeys[LEFT]=false; pieceArray[pieceArray.size()-1].move(LEFT);}
    if (heldKeys[DOWN] || shouldFall)
    { pieceArray[pieceArray.size()-1].move(DOWN);shouldFall=false;}


};

void DrawAndUpdate(){
    SDL_SetRenderDrawColor(renderer,200,200,200,255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer,40,40,40,255);
    SDL_RenderFillRectF(renderer,&gameGridMask);

    SDL_SetRenderDrawColor(renderer,0,200,0,255);

    SDL_FRect mouseRect = {MousePos.x-15,MousePos.y-15,30,30};
    SDL_RenderFillRectF(renderer,&mouseRect);

    piecesDraw();
    if (showGrid){
        SDL_SetRenderDrawColor(renderer,120,120,120,255);


        // for (float i= gameGridRect.x;i!=gameGridRect.x + gameGridRect.w;i++){
        //     SDL_RenderDrawLineF(renderer,i*gridXSize,gameGridRect.y*gridYSize,i*gridXSize,
        //         (gameGridRect.y*gridYSize)+gameGridRect.h*gridYSize);
        // }
        // for (float i=gameGridRect.y;i!=gameGridRect.y+gameGridRect.h;i++){
        //     SDL_RenderDrawLineF(renderer,gameGridRect.x*gridXSize,i*gridYSize,
        //     gameGridRect.x*gridXSize+gameGridRect.w*gridXSize
        //     ,i*gridYSize);
        // }

        for (float i= 0;i!=gridX;i++){
            SDL_RenderDrawLineF(renderer,i*gridXSize,0,i*gridXSize,
                winY);
        }
        for (float i=0 ;i!=gridY;i++){
            SDL_RenderDrawLineF(renderer,0,i*gridYSize,
            winX
            ,i*gridYSize);
        }
    }

    DrawPayam();

    SDL_RenderPresent(renderer);
    SDL_Delay(50);
};

int main(){
    Init();

    while (shouldRun){
        FetchEvents();
        CheckEvents();
        DrawAndUpdate();
    }


    return 0;
}
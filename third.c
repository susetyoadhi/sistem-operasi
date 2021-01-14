#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <Windows.h>
#include <conio.h>
// #include <pthread.h>
#include <time.h>

#define MAP_SIZE_W 15
#define MAP_SIZE_H 25
#define HALF_W 20
#define HALF_H 10
#define EXIT 100

#define WALL 5
#define EMPTY 0
#define BLOCK 1

#define UP 72
#define LEFT 75
#define RIGHT 77
#define SPACE 32
#define ESC 27
#define DOWN 80

#define kiri 1
#define kanan 2
#define atas 3

typedef char MData ;

typedef struct _currentlocation{
    int X;
    int Y;
} Location;


//hide cursor
void hidecursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

//move cursor.
void gotoxy(int x, int y){
    COORD P;
    P.X = 2*x;
    P.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), P);
}

//get keyboard input.
int getKeyDown(){
    if(kbhit()) return getch();
    else {
        return -1;
    }
}

//////////////////////////////////////////////////DRAW/////////////////////////////////////////////////////////////////
void drawWall(){
    int h, w;
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hand, 11);

    for(h=0; h<=MAP_SIZE_H +1; h++){
        for(w=0; w<=MAP_SIZE_W +1; w++){
            gotoxy(w+1,h+1);
            if(h == 0 || w == 0 || h == MAP_SIZE_H+1 || w == MAP_SIZE_W+1)
                printf("%c", 254);
        }
        printf("\n");
    }
    
    gotoxy(HALF_W, 1);
    SetConsoleTextAttribute(hand, 14);
    printf("<Next>");
    SetConsoleTextAttribute(hand, 11);

    for(h=2; h<=7 ;h++){
        for(w=HALF_W ; w<=HALF_W+5; w++){
            if(w==HALF_W || w==HALF_W+5 || h==2 || h==7){
                gotoxy(w, h);
                printf("%c", 254);
            }
        }
    }
    gotoxy(HALF_W, HALF_H+1);
    printf("Best  : ");
    gotoxy(HALF_W, HALF_H+2);
    printf("Score : ");
    gotoxy(HALF_W, HALF_H+12);
    printf("<Exit : 't' / Pause : 'p'>");
    SetConsoleTextAttribute(hand, 7);
}

int drawFrontMenu(){
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);
    int keyInput;
    gotoxy(1,2);
    SetConsoleTextAttribute(hand, 11);
    printf("=====================================================");
    gotoxy(1,3);
    SetConsoleTextAttribute(hand, 14);
    printf("=================== T E T R I S =====================");
    SetConsoleTextAttribute(hand, 11);
    gotoxy(1,4);
    printf("=====================================================\n");

    SetConsoleTextAttribute(hand, 14);
    gotoxy(2,6);
    printf("Left : a \n");
    gotoxy(2,7);
    printf("Right : d \n");
    gotoxy(2,8);
    printf("Rotation : w \n");
    gotoxy(2,9);
    gotoxy(2,10);
    printf("Exit: 't' \n");

    SetConsoleTextAttribute(hand, 14);

    while(1){
        keyInput = getch();
        if(keyInput == 's' || keyInput == 'S') break;
        if(keyInput == 't' || keyInput == 'T') break;

        gotoxy(7, 15);
        SetConsoleTextAttribute(hand, 11);
        printf(" === press 's' to start ===");
        SetConsoleTextAttribute(hand, 7);
        Sleep(1000/2);
        gotoxy(7, 15);
        printf("                            ");
        Sleep(1000/2);
    }

    return keyInput;
}

void drawMap(MData map[MAP_SIZE_H][MAP_SIZE_W]){
    int h, w;
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);

    for(h=0; h<MAP_SIZE_H; h++){
        for(w=0; w<MAP_SIZE_W; w++){
            gotoxy(w+2,h+2);
            if(map[h][w] == EMPTY){
                printf(" ");
            }else if(map[h][w] == BLOCK){
                SetConsoleTextAttribute(hand, 14);
                printf("%c", 254);
                SetConsoleTextAttribute(hand, 7);
            }
        }
        printf("\n");
    }

}
//show next shape, score, time, best score.
void drawSubMap(int best, int score){
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hand, 14);
    gotoxy(HALF_W + 4, HALF_H+1);
    printf("%4d", best);
    gotoxy(HALF_W + 4, HALF_H+2);
    printf("%4d", score);
    SetConsoleTextAttribute(hand, 7);
}

void drawSubShape(MData map[MAP_SIZE_H][MAP_SIZE_W],int shape[4][4]){
    int h, w;
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);

    for(h=3; h<=6 ;h++){
        for(w=HALF_W+1 ; w<=HALF_W+4; w++){
                gotoxy(w, h);
                printf(" ");

        }
    }

    for(h=3; h<=6 ;h++){
        for(w=HALF_W+1 ; w<=HALF_W+4; w++){
            if(shape[h-3][w - HALF_W-1] == BLOCK){
                gotoxy(w, h);
                SetConsoleTextAttribute(hand, 14);
                printf("%c", 254);
                SetConsoleTextAttribute(hand, 7);

            }
        }
    }
}

void drawShape(MData map[MAP_SIZE_H][MAP_SIZE_W],int shape[4][4], Location curLoc){
    int h, w;

    for(h=0; h<4;h++){
        for(w=0; w<4;w++){
            if(shape[h][w] == BLOCK){
                map[curLoc.Y+ h][curLoc.X +w]=BLOCK;
                //gotoxy(curLoc.X+2+w,curLoc.Y+2+h);
                // printf("■");
            }
        }
    }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void startTime(){
    int i;
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);

    for(i=0 ;i<3;i++){
        gotoxy(2,0);
        SetConsoleTextAttribute(hand, 14);
        printf("Start : %d sec", 3-i);
        SetConsoleTextAttribute(hand, 7);
        Sleep(1000);
    }
    gotoxy(2,0);
    printf("                       ");
}

void mapInit(MData map[MAP_SIZE_H][MAP_SIZE_W]){
    int i, j=0;
    for(i=0;i<MAP_SIZE_H;i++){
        for(j=0; j<MAP_SIZE_W; j++){
            map[i][j] = EMPTY;
        }
    }
}

void locationInit(Location * curLoc){
    curLoc->X =3;
    curLoc->Y =0;
}

void copyBlock(int blockShape[4][4], int copy[4][4]){
    int i, j;
    for(i=0;i<4;i++){
        for(j=0; j<4;j++){
            blockShape[i][j] = copy[i][j];
        }
    }
}

void setBlock(int blockShape[4][4]){

    int shape[7][4][4] = {
            {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},//I
            {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},//kotak
            {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}},//T
            {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},//S
            {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},//Z
            {{0,0,0,0},{0,1,0,0},{0,1,1,1},{0,0,0,0}},//L
            {{0,0,0,0},{0,1,1,1},{0,1,0,0},{0,0,0,0}}//L tidur
    };
    /*
    int shapeLong[4][4] = {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}};
    int shapeRock[4][4] = {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}};
    int shapeHorn[4][4] = {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}};
    int shapeStair[4][4] = {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}};
    int shapeRStair[4][4] = {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}};
    int shapeNienun[4][4] = {{0,0,0,0},{0,1,0,0},{0,1,1,1},{0,0,0,0}};
    int shapeRNieun[4][4] = {{0,0,0,0},{0,1,1,1},{0,1,0,0},{0,0,0,0}};
*/
    srand((unsigned int)(time(NULL)));

    switch(rand()%7) {
        case 0:
            copyBlock(blockShape, shape[0]);
            break;
        case 1:
            copyBlock(blockShape, shape[1]);
            break;
        case 2:
            copyBlock(blockShape, shape[2]);
            break;
        case 3:
            copyBlock(blockShape, shape[3]);
            break;
        case 4:
            copyBlock(blockShape, shape[4]);
            break;
        case 5:
            copyBlock(blockShape, shape[5]);
            break;
        case 6:
            copyBlock(blockShape, shape[6]);
            break;
        default :
            break;
    }
}

////////////////////////////////////////////////////////////////////!ah//////////////////////////////
void removeShape(MData map[MAP_SIZE_H][MAP_SIZE_W], int blockShape[4][4], Location * curLoc){
    int h, w;
    for(h=0; h<4;h++){
        for(w=0; w<4;w++){
            if(blockShape[h][w] == BLOCK)
                map[curLoc->Y + h][curLoc->X + w]=EMPTY;
        }
    }
}

int getShapeLeftLoc(int blockShape[4][4]){
    int h, w, leftW=4;
    for(w=0; w<4;w++){
        for(h=0; h<4;h++){
            if(blockShape[h][w] == BLOCK){
                if(leftW > w)
                    leftW = w;
            }
        }
    }
    return leftW;
}
int getShapeRightLoc(int blockShape[4][4]){
    int h, w, rightW=0;
    for(w=3; w>=0;w--){
        for(h=3; h>=0;h--){
            if(blockShape[h][w] == BLOCK){
                if(rightW < w)
                    rightW = w;
            }
        }
    }
    return rightW+1;
}
int getShapeBottomLoc(int blockShape[4][4]){
    int h, w, bottomH=0;
    for(w=3; w>=0;w--){
        for(h=3; h>=0;h--){
            if(blockShape[h][w] == BLOCK){
                if(bottomH < h)
                    bottomH =h;
            }
        }
    }
    return bottomH+1;
}
int getEachBottomLoc(int blockShape[4][4], int w){
    int h, bottomH=-1;
    for(h=3; h>=0;h--){
        if(blockShape[h][w] == BLOCK){
            if(bottomH < h)
                bottomH =h;
        }
    }
    return bottomH;
}
int getEachLeftLoc(int blockShape[4][4], int h){
    int w, leftW= 5;
    for(w=0; w<4;w++){
        if(blockShape[h][w] == BLOCK){
            if(leftW > w)
                leftW = w;
        }
    }
    return leftW;
}
int getEachRightLoc(int blockShape[4][4], int h){
    int w, rightW= -1;
    for(w=0; w<4;w++){
        if(blockShape[h][w] == BLOCK){
            if(rightW < w)
                rightW = w;
        }
    }
    return rightW;
}
void goLeft(MData map[MAP_SIZE_H][MAP_SIZE_W],int blockShape[4][4], Location *curLoc){
    int leftW = getShapeLeftLoc(blockShape);
    int boundaryArr[4] ={0};
    int i;
    for(i=0; i<4;i++){
        boundaryArr[i] = getEachLeftLoc(blockShape, i);

    }
    if((curLoc->X) + leftW > 0){
        if(!((boundaryArr[0] != 5 && map[curLoc->Y][curLoc->X + boundaryArr[0] -1] != EMPTY)
           ||(boundaryArr[1] != 5 && map[curLoc->Y +1][curLoc->X + boundaryArr[1] -1] != EMPTY)
           ||(boundaryArr[2] != 5 && map[curLoc->Y +2][curLoc->X + boundaryArr[2] -1] != EMPTY)
           ||(boundaryArr[3] != 5 && map[curLoc->Y +3][curLoc->X + boundaryArr[3] -1] != EMPTY))){

            removeShape(map, blockShape,curLoc);
            (curLoc->X)--;
        }
    }
}

void goRight(MData map[MAP_SIZE_H][MAP_SIZE_W],int blockShape[4][4], Location *curLoc){
    int rightW = getShapeRightLoc(blockShape);
    int boundaryArr[4] ={0};
    int i;
    for(i=0; i<4;i++){
        boundaryArr[i] = getEachLeftLoc(blockShape, i);

    }

    if((curLoc->X) + rightW < MAP_SIZE_W){
        if(!((boundaryArr[0] != 5 && map[curLoc->Y][curLoc->X + boundaryArr[0] +1] != EMPTY)
             ||(boundaryArr[1] != 5 && map[curLoc->Y +1][curLoc->X + boundaryArr[1] +1] != EMPTY)
             ||(boundaryArr[2] != 5 && map[curLoc->Y +2][curLoc->X + boundaryArr[2] +1] != EMPTY)
             ||(boundaryArr[3] != 5 && map[curLoc->Y +3][curLoc->X + boundaryArr[3] +1] != EMPTY))){

            removeShape(map, blockShape,curLoc);
            (curLoc->X)++;
        }

    }
}

int fixShape(MData map[MAP_SIZE_H][MAP_SIZE_W], int blockShape[4][4], Location *curLoc){
    int w, h;
    for(w=0; w<4; w++){
        for(h=0; h<4 ; h++){
            if(blockShape[h][w] ==1){
                map[curLoc->Y+ h][curLoc->X +w]=BLOCK;
            }
        }
    }
}

int goDown(MData map[MAP_SIZE_H][MAP_SIZE_W], int blockShape[4][4], Location *curLoc){
    int bottomH = getShapeBottomLoc(blockShape);
    int bottomArr[4] = {0};
    int i;
    for(i=0; i<4; i++){
        bottomArr[i] = getEachBottomLoc(blockShape, i);
    }
    if(curLoc->Y + bottomH  == MAP_SIZE_H
       ||(bottomArr[1] != -1 && map[curLoc->Y + bottomArr[1] +1][curLoc->X + 1] != EMPTY)
       ||(bottomArr[0] != -1 && map[curLoc->Y + bottomArr[0] +1][curLoc->X + 0] != EMPTY)
       ||(bottomArr[3] != -1 && map[curLoc->Y + bottomArr[3] +1][curLoc->X + 3] != EMPTY)
       ||(bottomArr[2] != -1 && map[curLoc->Y + bottomArr[2] +1][curLoc->X + 2] != EMPTY)
       ){

        fixShape(map, blockShape, curLoc);

        Sleep(1000/8);
        return TRUE;
    }


    if(curLoc->Y + bottomH < MAP_SIZE_H){
        removeShape(map, blockShape, curLoc);
        Sleep(1000/8);
        (curLoc->Y)++;
    }

    return FALSE;
}

void rotate(MData map[MAP_SIZE_H][MAP_SIZE_W],int blockShape[4][4], Location *curLoc){
    int i, j;
    int tmp[4][4];
    int leftW, rightW, bottomH;

    for(i=0; i<4;i++){
        for(j=0; j<4;j++){
            if(blockShape[i][j] == BLOCK){
                tmp[j][3-i] = blockShape[i][j];
                blockShape[i][j] = EMPTY;
            }

        }
    }

    for(i=0; i<4;i++){
        for(j=0; j<4;j++){
            if(tmp[i][j] == 1){
                blockShape[i][j] = BLOCK;
            }

        }
    }


    //(when rotate near the wall.
    leftW= getShapeLeftLoc(blockShape);
    if(curLoc->X + leftW <0){
        goRight(map, blockShape, curLoc);
        if(leftW == 0) goRight(map, blockShape, curLoc); //long shape
    }

    rightW = getShapeRightLoc(blockShape);
    if(curLoc->X + rightW >MAP_SIZE_W){
        goLeft(map, blockShape, curLoc);
        if(rightW == 4)goLeft(map, blockShape, curLoc); //long shape
    }


    bottomH = getShapeBottomLoc(blockShape);
    if(curLoc->Y + bottomH > MAP_SIZE_H){
        removeShape(map, blockShape, curLoc);
        (curLoc->Y)--;
        if(bottomH ==4) (curLoc->Y)--;      //long shape
    }
}


void deleteLine(MData map[MAP_SIZE_H][MAP_SIZE_W], int h){
    int w;
    for(w=0 ; w < MAP_SIZE_W ; w++){
        map[h][w] = EMPTY;
    }
}

void addline (MData map[MAP_SIZE_H][MAP_SIZE_W], int h){
    int w;
    for(w=0 ; w < MAP_SIZE_W ; w++){
        map[h][w] = BLOCK;
    }
}

void organizeLine(MData map[MAP_SIZE_H][MAP_SIZE_W], int h){
    int w;
    while(h > 1){
        for(w=0; w<MAP_SIZE_W;w++){
            map[h][w] = map[h-1][w];
        }
        h--;
    }
}

void checkLine(MData map[MAP_SIZE_H][MAP_SIZE_W], Location curLoc, int * score){
    int h, w, full, count =0;

    for(h=MAP_SIZE_H ; h >= (curLoc.Y -1); h--){
        full =0;
        for(w=0; w<MAP_SIZE_W  ;w++){
            if(map[h][w] == EMPTY){
                break;
            }else{
                full++;
            }
        }

        if(full == MAP_SIZE_W){
            (*score) += 100;
            deleteLine(map, h);
            drawMap(map);
            Sleep(100);
            addline(map, h);
            drawMap(map);
            Sleep(100);
            deleteLine(map, h);
            organizeLine(map, h);
        }
    }

}

int GameOver(MData map[MAP_SIZE_H][MAP_SIZE_W],int score, int bestScore){
    FILE * wfp;
    int w=0;
    for(w=0; w<MAP_SIZE_W; w++){
        if(map[0][w] == BLOCK){
            HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hand, 14);
            gotoxy(HALF_W -7, HALF_H-2);
            printf("====== Game Over ======");
            gotoxy(HALF_W -6, HALF_H-1);
            printf("Your Score : %4d\n", score);
            SetConsoleTextAttribute(hand, 7);
            gotoxy(1, MAP_SIZE_H+3);

            if(score >= bestScore){
                wfp = fopen("score.txt", "w");
                fprintf(wfp, "%d", score);
                fclose(wfp);
            }

            system("pause");
            return TRUE;
        }
    }

    return FALSE;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *GameStart(void *args){
    char map[MAP_SIZE_H][MAP_SIZE_W] = {0};
    int key;
    int reachBottom = FALSE;
    int one = TRUE;
    int score =0, bestScore =0;
    int blockShape[4][4] ={0};
    int blockShapeSub[4][4] = {0};
    Location curLoc = {2,2};

    FILE * rfp;
    if((rfp = fopen("score.txt", "r")) == NULL){
        FILE * wfp;
        wfp = fopen("score.txt", "w");
        fprintf(wfp, "%d", 0);
        fclose(wfp);
    }
    fscanf(rfp, "%d", &bestScore);

    mapInit(map);
    drawWall();
    drawMap(map);

    locationInit(&curLoc);
    setBlock(blockShape);
    startTime();
    setBlock(blockShapeSub);
    drawSubShape(map, blockShapeSub);
    
    while(1){

        if(reachBottom == TRUE){
            if(GameOver(map,score, bestScore)) break;

            checkLine(map, curLoc, &score);
            checkLine(map, curLoc, &score);
            locationInit(&curLoc);
            copyBlock(blockShape, blockShapeSub);
            setBlock(blockShapeSub);
            drawSubShape(map, blockShapeSub);
            reachBottom = FALSE;
        }

        drawSubMap(bestScore,score);
        drawShape(map,blockShape, curLoc);
        drawMap(map);
        reachBottom = goDown(map, blockShape, &curLoc);
        if(reachBottom == TRUE) continue;

        key = getKeyDown();
        if(key == 't' || key =='T') break;
        if(key == 'p' || key == 'P'){
            system("pause"); system("cls");
            drawMap(map); drawWall(map);
        }
        // pthread_join(input, NULL);
        // gerak(map,blockShape,&curLoc);
        if(key==224 || key ==0){
            key = getch();
            if(key == UP){
                rotate(map, blockShape, &curLoc);
            }else if(key == LEFT){
                goLeft(map, blockShape, &curLoc);
            }else if(key == RIGHT){
                goRight(map, blockShape, &curLoc);
            }
        }
    }
}


int main() {
    system("cls");
    char map[MAP_SIZE_H][MAP_SIZE_W] ={0};//map
    int key;
    hidecursor();
    
    pthread_t main;
    pthread_create(&main, NULL,GameStart,NULL);
    system("color 7");              
            //console color
    while(1){
        key = drawFrontMenu();
        if(key == 't' || key == 'T') break;
        else {
            system("cls");
            pthread_join(main,NULL);
            Sleep(1000/3);
            // system("cls");
        }
    }
    return 0;
}
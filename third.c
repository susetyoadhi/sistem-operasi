#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <Windows.h>//koordinat di terminal
#include <conio.h>
#include <time.h>

#define MAP_SIZE_W 15 //ukuran lebar
#define MAP_SIZE_H 25 //ukuran tinggi
#define HALF_W 20 //posisi untuk block berikutnya
#define HALF_H 10
#define EXIT 100

#define EMPTY 0 //kalau kosong
#define BLOCK 1 //kalau ada di koordinat

#define UP 72 //input untuk arrow up
#define LEFT 75 //arrow left
#define RIGHT 77 // arrow right
#define ESC 27
#define DOWN 80

typedef char MData ;//typedef untuk menampung data peta

typedef struct _currentlocation{//struct untuk koordinat block
    int X;
    int Y;
} Location;

//move cursor.
void gotoxy(int x, int y){//merubah posisi untuk output
    COORD P;
    P.X = 2*x;
    P.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), P);
}

//get keyboard input.
int getKeyDown(){//input keyboard
    if(kbhit()) return getch();
    else {
        return -1;
    }
}

//////////////////////////////////////////////////DRAW/////////////////////////////////////////////////////////////////
void drawWall(){//menggambar tembok
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

int drawFrontMenu(){//menu awal
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
    printf("Left :  \n");
    gotoxy(2,7);
    printf("Right :  \n");
    gotoxy(2,8);
    printf("Rotation :  \n");
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

void drawMap(MData map[MAP_SIZE_H][MAP_SIZE_W]){//menggambar peta untuk block
    int h, w;
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);

    for(h=0; h<MAP_SIZE_H; h++){
        for(w=0; w<MAP_SIZE_W; w++){
            gotoxy(w+2,h+2);
            if(map[h][w] == EMPTY){
                printf(" ");//jika kosong ouput spasi
            }else if(map[h][w] == BLOCK){
                SetConsoleTextAttribute(hand, 14);
                printf("%c", 254);//jika ada ouput block
                SetConsoleTextAttribute(hand, 7);
            }
        }
        printf("\n");
    }

}

void drawSubMap(int best, int score){//untuk peta block berikutnya
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hand, 14);
    gotoxy(HALF_W + 4, HALF_H+1);
    printf("%4d", best);
    gotoxy(HALF_W + 4, HALF_H+2);
    printf("%4d", score);
    SetConsoleTextAttribute(hand, 7);
}

void drawSubShape(MData map[MAP_SIZE_H][MAP_SIZE_W],int shape[4][4]){//untuk block berikutnya
    int h, w;
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);

    for(h=3; h<=6 ;h++){//dimulait di baris ke 3
        for(w=HALF_W+1 ; w<=HALF_W+4; w++){//line
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

void drawShape(MData map[MAP_SIZE_H][MAP_SIZE_W],int shape[4][4], Location curLoc){//memindahkan data balok ke peta
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
void startTime(){//start time
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

void mapInit(MData map[MAP_SIZE_H][MAP_SIZE_W]){//mengosongkan data peta
    int i, j=0;
    for(i=0;i<MAP_SIZE_H;i++){
        for(j=0; j<MAP_SIZE_W; j++){
            map[i][j] = EMPTY;
        }
    }
}

void locationInit(Location * curLoc){ //lokasi awal
    curLoc->X =3;
    curLoc->Y =0;
}

void copyBlock(int blockShape[4][4], int copy[4][4]){//memindahkan data block
    int i, j;
    for(i=0;i<4;i++){
        for(j=0; j<4;j++){
            blockShape[i][j] = copy[i][j];
        }
    }
}

void setBlock(int blockShape[4][4]){//merandom block

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
    srand((unsigned int)(time(NULL)));//random

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

/////////////////////////////////////////////////////////////////////////////////////////////
void removeShape(MData map[MAP_SIZE_H][MAP_SIZE_W], int blockShape[4][4], Location * curLoc){
    int h, w;
    for(h=0; h<4;h++){
        for(w=0; w<4;w++){
            if(blockShape[h][w] == BLOCK)
                map[curLoc->Y + h][curLoc->X + w]=EMPTY; //menghapus block saat turun
        }
    }
}

int getShapeLeftLoc(int blockShape[4][4]){//mengambil posisi block kiri
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
int getShapeRightLoc(int blockShape[4][4]){//mengambil posisi block kanan
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

int getShapeBottomLoc(int blockShape[4][4]){//mengambil posisi block bawah
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

void goLeft(MData map[MAP_SIZE_H][MAP_SIZE_W],int blockShape[4][4], Location *curLoc){//prosedur untuk bergerak ke kiri
    int leftW = getShapeLeftLoc(blockShape);
    int boundaryArr[4] ={0};
    int i;
    for(i=0; i<4;i++){
        boundaryArr[i] = getEachLeftLoc(blockShape, i);
    }
    
    if((curLoc->X) + leftW > 0){
        if(!((boundaryArr[0] != 5 && map[curLoc->Y][curLoc->X + boundaryArr[0] -1] != EMPTY)//untuk koordinat tiap bentuk balok
           ||(boundaryArr[1] != 5 && map[curLoc->Y +1][curLoc->X + boundaryArr[1] -1] != EMPTY)
           ||(boundaryArr[2] != 5 && map[curLoc->Y +2][curLoc->X + boundaryArr[2] -1] != EMPTY)
           ||(boundaryArr[3] != 5 && map[curLoc->Y +3][curLoc->X + boundaryArr[3] -1] != EMPTY))){

            removeShape(map, blockShape,curLoc);
            (curLoc->X)--;//mengurangi koordinat x / horizontal
        }
    }
}

void goRight(MData map[MAP_SIZE_H][MAP_SIZE_W],int blockShape[4][4], Location *curLoc){//prosedur untuk bergerak ke kanan
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
            (curLoc->X)++;//menambah koordinat x / horizontal
        }

    }
}

int fixShape(MData map[MAP_SIZE_H][MAP_SIZE_W], int blockShape[4][4], Location *curLoc){//memperbaiki bentuk block
    int w, h;
    for(w=0; w<4; w++){
        for(h=0; h<4 ; h++){
            if(blockShape[h][w] == 1){
                map[curLoc->Y+ h][curLoc->X +w]=BLOCK;
            }
        }
    }
}

int goDown(MData map[MAP_SIZE_H][MAP_SIZE_W], int blockShape[4][4], Location *curLoc){//prosedur untuk turun
    int bottomH = getShapeBottomLoc(blockShape);
    int bottomArr[4] = {0};
    int i;
    for(i=0; i<4; i++){
        bottomArr[i] = getEachBottomLoc(blockShape, i);
    }
    
    if(curLoc->Y + bottomH  == MAP_SIZE_H
       ||(bottomArr[1] != -1 && map[curLoc->Y + bottomArr[1] +1][curLoc->X + 1] != EMPTY)//posisi block paling bawah
       ||(bottomArr[0] != -1 && map[curLoc->Y + bottomArr[0] +1][curLoc->X + 0] != EMPTY)
       ||(bottomArr[3] != -1 && map[curLoc->Y + bottomArr[3] +1][curLoc->X + 3] != EMPTY)
       ||(bottomArr[2] != -1 && map[curLoc->Y + bottomArr[2] +1][curLoc->X + 2] != EMPTY)
       ){

        fixShape(map, blockShape, curLoc);

        Sleep(1000/8);
        return TRUE;
    }


    if(curLoc->Y + bottomH < MAP_SIZE_H){//selama belum sampai bawah
        removeShape(map, blockShape, curLoc);
        Sleep(1000/8);//sleep
        (curLoc->Y)++;//koordinat y / vertikal bertambah
    }

    return FALSE;
}

void rotate(MData map[MAP_SIZE_H][MAP_SIZE_W],int blockShape[4][4], Location *curLoc){//untuk memutar balok
    int i, j;
    int tmp[4][4];
    int leftW, rightW, bottomH;

    for(i=0; i<4;i++){//menggunakan prosedur memutar matriks dengan mempertimbangkan lokasi dari tiap balok
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


    //saat diputar di dekat tembok
    //menyesuaikan balok tersebut dengan tembok agar tidak tercampur / error di block
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


void deleteLine(MData map[MAP_SIZE_H][MAP_SIZE_W], int h){//menghapus garis saat sudah penuh
    int w;
    for(w=0 ; w < MAP_SIZE_W ; w++){
        map[h][w] = EMPTY;
    }
}

void addline (MData map[MAP_SIZE_H][MAP_SIZE_W], int h){//menambahkan garis
    int w;
    for(w=0 ; w < MAP_SIZE_W ; w++){
        map[h][w] = BLOCK;
    }
}

void organizeLine(MData map[MAP_SIZE_H][MAP_SIZE_W], int h){//mengisi baris yang kosong saat garis dihapus
    int w;
    while(h > 1){
        for(w=0; w<MAP_SIZE_W;w++){
            map[h][w] = map[h-1][w];
        }
        h--;
    }
}

void checkLine(MData map[MAP_SIZE_H][MAP_SIZE_W], Location curLoc, int * score){//mengecek baris jika baris tersebut sudah penuh
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

        if(full == MAP_SIZE_W){//jika penuh
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
void *GameStart(void *args){//thread untuk program utama
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
    
    pthread_t main;//menamakan thread
    pthread_create(&main, NULL,GameStart,NULL);//membuat thread
    system("color 7");              
            //console color
    while(1){
        key = drawFrontMenu();
        if(key == 't' || key == 'T') break;
        else {
            system("cls");
            pthread_join(main,NULL);//memanggil thread
            Sleep(1000/3);
            // system("cls");
        }
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <MMsystem.h>
#include <stdbool.h>
#include <string.h>
#include<time.h>

const char PLAYER1='X';
const char PLAYER2='O';

typedef struct{
    int score;
    char name[257];
    char color;
    int numbOfMoves;
}Player;

typedef struct{
    time_t start;
    time_t end;
    double diff;
    int hrs,mins,secs;
}Time;

typedef struct{
    char board[100][100];
    Player plr1;
    Player plr2;
}State;


int stateIndex= -1;
int maxRedo;
char gameMode;


void clearBoard();
void drawBoard();
void red();
void yellow();
void reset();
void printNamesAndScores();
void playerVsComputer();
void playerVsPlayer();
void takePlayerTurn();
void takeComputerTurn();
int  checkIfValidInput();
void checkScores();
bool checkForFreeSlots();
void printWinnerPlayerVsComputer();
void printWinnerPlayerVsPlayer();
void saveState();
void undo();
void redo();


bool saveGame();
//State *loadGame(char *filename);

int xml();


int main(){
    // Main Menu
    int quit = 0;
    //char newOrLoaded;
    do{
        system("cls");
        printf(" 1:Start a new game\n 2:Load a previous game\n 3:Quit\n");
        char choice;
        scanf(" %c", &choice);

        switch(choice){
            case '1':
                system("cls");
                printf(" 1: One player\n 2: Two players\n 3: Back\n");
                scanf(" %c",&choice);
                //newOrLoaded = 'n';
                switch(choice){
                    case '1':
                        // Player v Computer
                        playerVsComputer();
                        break;

                    case '2':
                        // Player v Player
                        playerVsPlayer();
                        break;
                    case '3':
                        break;
                    default:
                        printf("Invalid Input ! A single digit is required.\n");
                }
                break;

            case '2':
                system("cls");
                int gameSave;
                do{
                    printf("Choose the file you would like to load:\n1:Game Save 1\n2:Game Save 2\n3:Game Save 3\n");
                    scanf("%d", &gameSave);
                    if(gameSave>=1 && gameSave<=3){
                        break;
                    }
                    printf("You must enter an integer from 1 to 3\n");
                }while( gameSave<1 || gameSave>3 );

                 break;

            case '3': quit = 1; break;
            //default: printf("Invalid Input !\n");
        }
       }
       /*else{
        system("cls");
        printf(" 1:Start a new game\n 2:Load a previous game\n 3:Quit\n");
        scanf("%c", &choice);
       }*/

    while(quit == 0);


    return 0;
}



void clearBoard(int rows,int columns,char array[rows][columns]){
    int i,j;
    for (i=0; i<rows; i++){
        for(j=0; j<columns; j++){
            array[i][j]=' ';
        }
    }

}


void drawBoard(int rows,int columns,char array[rows][columns], Player player1, Player player2, Time timeTaken){
    system("cls");
    printf("\tConnect Four\n");
    printf("\t");

    red();
    printNamesAndScores(player1);
    reset();

    printf("\t\t");

    yellow();
    printNamesAndScores(player2);
    reset();

    printf("\n");
    timeTaken.diff=difftime(timeTaken.end,timeTaken.start);
    timeTaken.hrs=(timeTaken.diff/(60*60));
    timeTaken.mins=((int)timeTaken.diff%(60*60))/60;
    timeTaken.secs=((int)timeTaken.diff%(60*60))%60;
    printf("\n\t\tTime: %d hours : %d minutes : %d seconds\n\n",timeTaken.hrs,timeTaken.mins,timeTaken.secs);

    int i,j;
    int header[columns];
    for(j=0;j<columns;j++){
        header[j]=j+1;
    }
    for(j=0;j<columns;j++){
            printf("  %d ",header[j]);
    }
    printf("\n");
    for (i=0;i<rows;i++){
        for(j=0;j<columns;j++){
            printf("| ");
            if(array[i][j]==PLAYER1){
                red();
                printf("%c ", array[i][j]);
                reset();
            }
            else if(array[i][j]==PLAYER2){
                yellow();
                printf("%c ", array[i][j]);
                reset();
            }
            else{
                printf(" %c",array[i][j]);
            }

        }
        printf("|\n");
        for(j=0;j<columns;j++){
            printf("----");
        }
        printf("-\n");
    }
}

//color fn
void red () {
  printf("\033[1;31m");
}

void yellow (){
  printf("\033[1;33m");
}

void reset () {
  printf("\033[0m");
}


void printNamesAndScores(Player player){
    fputs(player.name,stdout);
    printf(": Score= %d & Number of moves= %d", player.score,player.numbOfMoves);
}

void playerVsComputer(){
    system("cls");
    int response = 1;
    gameMode = 'o';
    do{
        int rows,columns;
         /* while(1){
            printf("Enter the size of the grid: ");
            scanf("%d %d",&rows,&columns);
            if(rows<4 || columns<4){
                printf("Rows and Columns must be greater or equal to 4 !\n");
            }
            else{
                break;
            }
        }*/

        if(xml(&rows,&columns)){
        char array[rows][columns];
        State Timeline[rows*columns];

        Player player1;
        Player computer;

        printf("Enter Player name: "); scanf("%c");
        fgets(player1.name, 257, stdin);
        player1.name[strlen(player1.name) - 1]='\0';
        player1.color = PLAYER1;
        player1.score = 0;
        player1.numbOfMoves=0;

        strcpy(computer.name, "Computer");
        computer.color = PLAYER2;
        computer.score = 0;
        computer.numbOfMoves=0;

        Time timeTaken;
        clearBoard(rows,columns,array);
        saveState(rows,columns,array, player1, computer, Timeline);
        timeTaken.start=time(NULL);
        timeTaken.end=time(NULL);
        drawBoard(rows,columns,array, player1, computer, timeTaken);

        timeTaken.start=time(NULL);

        while( checkForFreeSlots(rows,columns,array) ){
            red();
            takePlayerTurn(rows,columns,array, &player1, &computer, &timeTaken, Timeline);
            reset();
            timeTaken.end=time(NULL);
            drawBoard(rows,columns,array, player1, computer, timeTaken);

            if(checkForFreeSlots(rows,columns,array)== 0){ break; }
            takeComputerTurn(rows,columns,array, &computer, &player1, Timeline);
            timeTaken.end=time(NULL);
            drawBoard(rows,columns,array, player1, computer, timeTaken);
        }

        printWinnerPlayerVsComputer(player1,computer);

        printf("1: Play Again\n2: Main Menu\n");
        scanf("%d", &response);
        switch(response){
            case 1:
                break;
            case 2:
                response = 0;
                break;
        }
        }
        else{
            break;
        }
    }while(response);
}

void playerVsPlayer(){
    system("cls");
    int response = 1;
    gameMode = 't';
    do{
        int rows,columns;
       /* while(1){
            printf("Enter the size of the grid: ");
            scanf("%d %d",&rows,&columns);
            if(rows<4 || columns<4){
                printf("Grid size must be at least 4x4 !\n");
            }
            else{
                break;
            }
        }*/

        if(xml(&rows,&columns)){
        char array[rows][columns];
        State Timeline[rows*columns];

        Player player1;
        Player player2;

        printf("Enter Player 1 name: "); scanf("%c");
        fgets(player1.name, 257, stdin);
        player1.name[strlen(player1.name) - 1]='\0';
        player1.color = PLAYER1;
        player1.score = 0;
        player1.numbOfMoves=0;

        printf("Enter Player 2 name: ");
        fgets(player2.name, 257, stdin);
        player2.name[strlen(player2.name) - 1]='\0';
        player2.color = PLAYER2;
        player2.score = 0;
        player2.numbOfMoves=0;


        clearBoard(rows,columns,array);
        saveState(rows,columns,array, player1, player2, Timeline);
        Time timeTaken;
        timeTaken.start=time(NULL);
        timeTaken.end=time(NULL);
        drawBoard(rows,columns,array, player1, player2,timeTaken);

        timeTaken.start=time(NULL);

        while( checkForFreeSlots(rows,columns,array) ){
            red();
            takePlayerTurn(rows,columns,array, &player1, &player2, &timeTaken, Timeline);
            reset();
            timeTaken.end=time(NULL);
            drawBoard(rows,columns,array, player1, player2, timeTaken);

            if(checkForFreeSlots(rows,columns,array)== 0){ break; }

            yellow();
            takePlayerTurn(rows,columns,array, &player2, &player1, &timeTaken, Timeline);
            reset();
            timeTaken.end=time(NULL);
            drawBoard(rows,columns,array, player1, player2, timeTaken);
        }

        printWinnerPlayerVsPlayer(player1,player2);

        printf("1: Play Again\n2: Main Menu\n");
        scanf("%d", &response);
        switch(response){
            case 1:
                break;
            case 2:
                response = 0;
                break;
        }
        }
        else{
            break;
        }
    }while(response);
}


void takePlayerTurn(int rows,int columns,char array[rows][columns], Player *player1, Player *player2, Time *timeTaken, State Timeline[rows*columns] ){
    char colDesired[100];
    long int enteredCol;
    double floatCol;
    while(1){
        do{
           for (int i=0; i<strlen((*player1).name); i++){
           printf("%c", (*player1).name[i]);
           }
           printf("'s turn, please enter col num (-1:Undo, -2:Redo, -3:Save and Exit, -4:Exit) : ");
           scanf("%s", colDesired);
           enteredCol=atol(colDesired);
           floatCol=atof(colDesired);
           if (enteredCol == 0 || (double)enteredCol!=floatCol){
            printf("Invalid Input. You have to enter an Integer\n");
           }
        }
        while(enteredCol == 0 || (double)enteredCol!=floatCol);
        if (enteredCol==-4){
                exit(0);
        }
        enteredCol--;
        if( checkIfValidInput(rows, columns, array, enteredCol) == 1){
            int i=0;
            while(array[i][enteredCol] ==' ' && i<rows){
                i++;
            }
            array[i-1][enteredCol]= (*player1).color;
            (*player1).numbOfMoves++;
            checkScores(rows, columns, array, i-1, enteredCol, &((*player1).score) );
            if(gameMode == 't'){
                if( (*player1).color == PLAYER1 ){
                    saveState(rows,columns,array, *player1, *player2, Timeline);
                }
                else{
                    saveState(rows,columns,array, *player2, *player1, Timeline);
                }
            }

            maxRedo = stateIndex;
            break;
        }
        else if( checkIfValidInput(rows, columns, array, enteredCol) == 2 ){   //Perform Undo
            if(stateIndex > 0){
                if( (*player1).color == PLAYER1 ){
                    undo( rows, columns, array, &(*player1), &(*player2), Timeline);
                    if(gameMode == 't'){
                        break;
                    }
                    else{
                        (*timeTaken).end=time(NULL);
                        drawBoard(rows,columns,array, *player1, *player2, *timeTaken);
                    }
                }
                else{
                    undo( rows, columns, array, &(*player2), &(*player1), Timeline);
                    if(gameMode == 't'){
                        break;
                    }
                    else{
                        (*timeTaken).end=time(NULL);
                        drawBoard(rows,columns,array, *player1, *player2, *timeTaken);
                    }
                }
            }
            else{
                printf("Nothing To Undo !\n");
            }
        }

        else if( checkIfValidInput(rows, columns, array, enteredCol) == 3 ){       //Perform Redo
            if(stateIndex < maxRedo){
                if( (*player1).color == PLAYER1 ){
                    redo( rows, columns, array, &(*player1), &(*player2), Timeline);
                    if(gameMode == 't'){
                        break;
                    }
                    else{
                        (*timeTaken).end=time(NULL);
                        drawBoard(rows,columns,array, *player1, *player2, *timeTaken);
                    }
                }
                else{
                    redo( rows, columns, array, &(*player2), &(*player1), Timeline);
                    if(gameMode == 't'){
                        break;
                    }
                    else{
                        (*timeTaken).end=time(NULL);
                        drawBoard(rows,columns,array, *player1, *player2, *timeTaken);
                    }
                }
            }
            else{
                printf("Nothing To Redo !\n");
            }
        }

        else if( checkIfValidInput(rows, columns, array, enteredCol) == 4 ){  //Save & quit
            system("cls");
            reset();
            int gameSave;
            do{
                printf("Choose the file you would like to save to:\n1:Game Save 1\n2:Game Save 2\n3:Game Save 3\n");
                scanf("%d", &gameSave);
                if(gameSave>=1 && gameSave<=3){
                    break;
                }
                printf("You must enter an integer from 1 to 3\3");
            }while( gameSave<1 || gameSave>3 );

            if(saveGame(gameSave, Timeline[stateIndex])){
                printf("Save Successful");
            }
            else{
                printf("Failed to save");
            }
            exit(0);
        }


        else{
            printf("Invalid Input !\n");
        }
        }
    }


void takeComputerTurn(int rows,int columns,char array[rows][columns], Player *computer, Player *player, State Timeline[rows*columns] ){
    srand(time(0));
    int i,j;
        do{
           j= rand()%columns;
        }
        while(array[0][j]!=' ');
        for(i=0;i<rows;i++){
            if(array[i][j]!=' '){
                break;
            }
        }

    array[i-1][j]=(*computer).color;
    (*computer).numbOfMoves++;
    checkScores(rows, columns, array, i-1, j, &((*computer).score) );
    saveState(rows,columns,array, *player, *computer, Timeline);
    maxRedo = stateIndex;
}


int checkIfValidInput(int rows,int columns,char array[rows][columns], int enteredCol){
    if(enteredCol>=columns || enteredCol < -4){
        return 0;
    }
    else if(array[0][enteredCol]== PLAYER1 || array[0][enteredCol]==PLAYER2 ){
        return 0;
    }
    else if (enteredCol == -2){
        return 2;
    }
    else if (enteredCol == -3){
        return 3;
    }
    else if (enteredCol == -4){
        return 4;
    }
    else{
        return 1;
    }
}

void checkScores(int rows,int columns,char array[rows][columns], int enteredRow, int enteredCol, int *playerScore){
    //Check Horizontal
    if(enteredCol<columns-3 && ( array[enteredRow][enteredCol]==array[enteredRow][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol+3] ) ){
        (*playerScore)++;
    }

    if(enteredCol>=3 && ( array[enteredRow][enteredCol]==array[enteredRow][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol-3] ) ){
        (*playerScore)++;
    }

    if(enteredCol<=columns-3 && enteredCol>0 && array[enteredRow][enteredCol]==array[enteredRow][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol+2] ){
        (*playerScore)++;
    }

    if(enteredCol<=columns-2 && enteredCol>1 && array[enteredRow][enteredCol]==array[enteredRow][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol+1] ){
        (*playerScore)++;
    }


    //Check Vertical
    if(enteredRow<rows-3 && (array[enteredRow][enteredCol] == array[enteredRow+1][enteredCol] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol] && array[enteredRow][enteredCol]==array[enteredRow+3][enteredCol])){
        (*playerScore)++;
    }

    //Check Diagonals
    if(enteredCol<columns-3){
       if(enteredRow<=rows-3 && (( array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow+3][enteredCol+3] )) ){
            (*playerScore)++;
        }
         if(enteredRow<=rows-3 &&  ( array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol-1] )){
            (*playerScore)++;
        }
         if(enteredRow<=rows-3 && enteredCol>1 && ( array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol-1] )){
            (*playerScore)++;
        }
        if(enteredRow<=rows-3 && enteredCol>0 && ( array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol+2] )){
            (*playerScore)++;
        }
        if(enteredRow>=3 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow-3][enteredCol+3] )){
            (*playerScore)++;
        }
         if(enteredRow>=3 && enteredCol>1 && enteredRow<rows-2 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol-2] )){
            (*playerScore)++;
        }
         if(enteredRow>=3 && enteredCol>0 && enteredRow<rows-1 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol-1] )){
            (*playerScore)++;
        }
        if(enteredRow>=3 && enteredCol>1 && enteredRow<rows-1 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol-2] )){
            (*playerScore)++;
        }

    }
    if(enteredCol>=3){
        if(enteredRow<rows-3 && ( array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow+3][enteredCol-3] )){
            (*playerScore)++;
        }
        if(enteredRow<rows-3 && enteredCol<columns-1 && enteredRow>0 && ( array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol+1] )){
            (*playerScore)++;
        }
        if(enteredRow<rows-3 && enteredCol<columns-1 && enteredRow>1 && ( array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol-1] )){
            (*playerScore)++;
        }
        if(enteredRow<rows-3 && enteredCol<columns-2 && enteredRow>2 && ( array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol+1] )){
            (*playerScore)++;
        }
        if(enteredRow<rows-3 && enteredCol<columns-1 && enteredRow>0 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol+1] )){
            (*playerScore)++;
        }
        if(enteredRow>=3 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow-3][enteredCol-3] )){
            (*playerScore)++;
        }
        if(enteredRow>=3 && enteredCol<columns-1 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol+1] )){
            (*playerScore)++;
        }
         if(enteredRow>=3 && enteredCol<columns-2 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol-1] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol+1] )){
            (*playerScore)++;
        }
        if(enteredRow>=3 && enteredCol<columns-2 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow-2][enteredCol+2] && array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol-1] )){
            (*playerScore)++;
        }
        if(enteredRow>=3 && enteredCol<columns-1 && ( array[enteredRow][enteredCol]==array[enteredRow-1][enteredCol+1] && array[enteredRow][enteredCol]==array[enteredRow+2][enteredCol-2] && array[enteredRow][enteredCol]==array[enteredRow+1][enteredCol-1] )){
            (*playerScore)++;
        }
    }

//if( array[enteredRow][enteredCol]==array[enteredRow][enteredCol] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol] && array[enteredRow][enteredCol]==array[enteredRow][enteredCol] )
}


bool checkForFreeSlots(int rows,int columns,char array[rows][columns]){
    bool free = false;
    for(int i=0; i<rows; i++){
        for (int j=0; j<columns; j++){
            if(array[i][j] == ' '){
                return 1;
            }
        }
    }
    return free;
}


void printWinnerPlayerVsComputer(Player player1,Player computer){
    if (player1.score>computer.score){
            red();
            printf("WINNER! Great job, ");
            fputs(player1.name,stdout);
            printf(".\n");
            reset();
        }
        else if (player1.score<computer.score){
            yellow();
            printf("You Lost. Hard luck. Have another try.\n");
            reset();
        }
        else{
            printf("Draw.\n");
        }
}


void printWinnerPlayerVsPlayer(Player player1,Player player2){
    if(player1.score>player2.score){
            red();
            fputs(player1.name,stdout);
            printf(" Wins! Congrats !\n");
            reset();
        }
        else if (player1.score<player2.score){
            yellow();
            fputs(player2.name,stdout);
            printf(" Wins! Congrats !\n");
            reset();
        }
        else{
            printf("Draw.\n");
        }
}

void saveState(int rows, int columns, char array[rows][columns], Player p1, Player p2, State Timeline[rows*columns]){
    if(stateIndex < (rows*columns)-1){
        stateIndex += 1;
        for (int i=0; i<rows; i++){
            for(int j=0; j<columns; j++){
                Timeline[stateIndex].board[i][j] = array[i][j];
            }
        }

        Timeline[stateIndex].plr1.score = p1.score;
        Timeline[stateIndex].plr1.numbOfMoves = p1.numbOfMoves;
        Timeline[stateIndex].plr2.score = p2.score;
        Timeline[stateIndex].plr2.numbOfMoves = p2.numbOfMoves;
    }
}

void undo(int rows,int columns,char array[rows][columns], Player *p1, Player *p2, State Timeline[rows*columns]){
    if(stateIndex > 0){ stateIndex -= 1 ; }
    //if(stateIndex > 1 && gameMode == 'o'){ stateIndex -= 2 ; }

    for (int i=0; i<rows; i++){
        for(int j=0; j<columns; j++){
            array[i][j]= Timeline[stateIndex].board[i][j];
        }
    }

    ((*p1).score) = Timeline[stateIndex].plr1.score;
    ((*p1).numbOfMoves) = Timeline[stateIndex].plr1.numbOfMoves;
    ((*p2).score) = Timeline[stateIndex].plr2.score;
    ((*p2).numbOfMoves) = Timeline[stateIndex].plr2.numbOfMoves;
}

void redo(int rows,int columns,char array[rows][columns], Player *p1, Player *p2, State Timeline[rows*columns]){
    if(stateIndex < maxRedo){ stateIndex += 1 ; }

    for (int i=0; i<rows; i++){
        for(int j=0; j<columns; j++){
            array[i][j]= Timeline[stateIndex].board[i][j];
        }
    }
    ((*p1).score) = Timeline[stateIndex].plr1.score;
    ((*p1).numbOfMoves) = Timeline[stateIndex].plr1.numbOfMoves;
    ((*p2).score) = Timeline[stateIndex].plr2.score;
    ((*p2).numbOfMoves) = Timeline[stateIndex].plr2.numbOfMoves;
}

bool saveGame(int gameSave, State *currentState){
    FILE *saveFile;

    switch(gameSave){
        case 1:
            saveFile = fopen("Game Save 1.bin", "wb");
            break;
        case 2:
            saveFile = fopen("Game Save 2.bin", "wb");
            break;
        case 3:
            saveFile = fopen("Game Save 3.bin", "wb");
            break;
    }

    if(saveFile == NULL){ return false; }

    if( fwrite(currentState, sizeof(State), 1, saveFile) != 1){return false;}

    if(fclose(saveFile) == EOF){return false;}

    return true;
}



int xml(int *rows,int *columns){
    int  highScores;
    char y,x[500]= {'0'},conf1[]="<Configurations>",width1[]="<Width>",height1[]="<Height>",highscores1[]="<Highscores>";
    char conf2[]="</Configurations>",width2[]="</Width>",height2[]="</Height>",highscores2[]="</Highscores>";
    int i=0, len=0, fileCorrupted=0;
    int j,k,l,m,n,q,h,s,r,f;
    char height[256],width[256],scores[256];
    FILE *file= fopen("project.xml", "r");

    while((y=fgetc(file)) && y!=EOF){
        if(y!=' '&&y!='\n'&&y!='\t'){
            x[i]=y;
            i++;
            len++;
        }
    }
    /*for(i=0;i<len;i++){
        printf("%c",x[i]);
    }*/

    for(i=0; i<sizeof(conf1)-1; i++){
        if(x[i]!=conf1[i]){
            printf("FILE CORRUPTED!");
            fileCorrupted=0;
            break;
        }
    }

    for(j=0;j<sizeof(height1)-1; j++){
        if(x[i+j]!=height1[j]){
            printf("FILE CORRUPTED!");
            fileCorrupted=0;
            break;
        }
    }
    if (fileCorrupted==0){
        for (k=0; k<sizeof(height)-1; k++){
            if(isdigit(x[i+j+k])){
                height[k]=x[i+j+k];
            }
            else{
                break;
            }
        }
    }
    *rows=atoi(height);

    for(l=0; l<sizeof(height2)-1; l++){
        if(x[i+j+k+l]!=height2[l]){
            printf("FILE CORRUPTED!");
            fileCorrupted=1;
            break;
        }
    }

    for(m=0; m<sizeof(width1)-1; m++){
        if(x[i+j+k+l+m]!=width1[m]){
            printf("FILE CORRUPTED!\n");
            fileCorrupted=1;
            break;
        }
    }

    if (fileCorrupted==0){
        for (n=0; n<sizeof(width)-1; n++){
            if(isdigit(x[i+j+k+l+m+n])){
                width[n]=x[i+j+k+l+m+n];
            }
            else{
                break;
            }
        }
    }
    *columns=atoi(width);


    for(q=0; q<sizeof(width2)-1; q++){
        if(x[i+j+k+l+m+n+q]!=width2[q]){
            printf("FILE CORRUPTED!\n");
            fileCorrupted=1;
            break;
        }
    }

    for(h=0; h<sizeof(highscores1)-1; h++){
        if(x[i+j+k+l+m+n+q+h]!=highscores1[h]){
            printf("FILE CORRUPTED!\n");
            fileCorrupted=1;
            break;
        }
    }

    if (fileCorrupted==0){
        for (s=0; s<sizeof(scores)-1; s++){
            if(isdigit(x[i+j+k+l+m+n+q+h+s])){
                scores[s]=x[i+j+k+l+m+n+q+h+s];
            }
            else{
                break;
            }
        }
    }
    highScores=atoi(scores);

    for(r=0; r<sizeof(highscores2)-1; r++){
        if(x[i+j+k+l+m+n+q+h+s+r]!=highscores2[r]){
            printf("FILE CORRUPTED!\n");
            fileCorrupted=1;
            break;
        }
    }
    for(f=0; f<sizeof(conf2)-1; f++){
        if(x[i+j+k+l+m+n+q+h+s+r+f]!=conf2[f]){
            printf("FILE CORRUPTED!\n");
            fileCorrupted=1;
            break;
        }
    }

    fclose(file);
    if(fileCorrupted==0){
        return 1;
    }
    else{
        return 0;
    }
}







/*int checkInt(char num[],MAX_SIZE){
    int i,sum=0;
    for(i=0;i<=strlen(num),i++){
        if (isdigit(num[i])){
            sum++;
        }
    }
    if(sum==strlen(nym)){
        return 1;
    }
    else {
        return 0;
    }
}*/

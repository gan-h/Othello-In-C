//TODO 
//Movelist
//Textbox
//File opening
//Shared memory computer forking
//run on ubuntu
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define BLANK 0
#define BLACK 1
#define WHITE 2

GtkWidget *window;
GtkGrid *chessboard;
GtkBuilder *builder;
GtkButton *buttonMappings[8][8];
GtkWidget *whitePiece;
GtkWidget *blackPiece;
GtkWidget *saveGame;
GtkWidget *loadGame;
GtkWidget *newGame;
GtkTextBuffer *textbuffer; 
GtkWidget *announcer;

int currentPlayer = BLACK;
bool preventSave = false;
bool guidesOn = true;
bool lockGrid = false;

int currentBoard[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,2,0,0,0},
    {0,0,0,2,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
}; 

int* boardHistory[64]; //64 currentBoards max
int boardsRecorded = 0;

int moveHistory[64]; //64 moves max
int movesRecorded = 0;

void textBoxLogic(int moveList[64]);
void recordMove(int moveToRecord);
void recordBoard(int boardToRecord[8][8]);
void copyBoard(int copyFrom[8][8], int copyTo[8][8]);
void printBoard(int boardState[8][8]);
void redraw_board(int boardState[8][8]);
void backClickedHandler(GtkWidget widget);
void forwardClickedHandler(GtkWidget widget);
void saveGameHandler();
void loadGameHandler();
bool legalMoveHelper(int boardState[8][8], int row, int col, int rowInc, int colInc, int player, bool changePiece);
int getOpponent(int currentPlayer);
int* getAllLegalMoves(int boardState[8][8], int player);
void endGame();

void drawBlackGuides(int boardstate[8][8]){
    if(guidesOn);
    else return;
    if(lockGrid) return;
    int *legalMoves = getAllLegalMoves(boardstate, BLACK);
    int iterator = 0;
    while(legalMoves[iterator] != 0){
        int moveMade = legalMoves[iterator];
        int i = (moveMade-1) / 8;
        int j = (moveMade-1) % 8;
        GtkWidget *blackGuide = gtk_image_new_from_file ("./blackGuide.png");
        gtk_button_set_image(buttonMappings[i][j], blackGuide);
        iterator++;
    }
    
}

void redraw_board(int boardState[8][8]){
    int i, j; 
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            GtkWidget *blackPiece = gtk_image_new_from_file ("./blackPiece.png");
            GtkWidget *whitePiece = gtk_image_new_from_file ("./whitePiece.png");
            GtkWidget *blankSquare = gtk_image_new_from_file ("./blankSquare.png"); //hack to stop grid resizing problem
            gtk_button_set_always_show_image (GTK_BUTTON(buttonMappings[i][j]), TRUE);
            switch (boardState[i][j])
            {
            case 0:
                gtk_button_set_image(buttonMappings[i][j], blankSquare);
                break;
            case 1:
                gtk_button_set_image(buttonMappings[i][j], blackPiece);
                break;
            case 2:
                gtk_button_set_image(buttonMappings[i][j], whitePiece);
                break;
            default:
                break;
            }
            
        }
    }
}

int viewBoardNumber = 0;

void backClickedHandler(GtkWidget widget){
    //printf("Back Pressed\n");
    if(viewBoardNumber - 1 >= 0) viewBoardNumber--;
    else return;
    int drawMe[8][8];
    int i, j, k;
    k = 0;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            drawMe[i][j] = boardHistory[viewBoardNumber][k];
            k++;
        }
    }
    
    redraw_board(drawMe);
    if(viewBoardNumber == boardsRecorded-1){
        drawBlackGuides(currentBoard);
    }
}

void forwardClickedHandler(GtkWidget widget){
    // printf("Forward Pressed\n");
    // printf("boardsRecorded: %d\n   viewBoardNumber: %d", boardsRecorded, viewBoardNumber);
    if(viewBoardNumber == boardsRecorded-1) return;
    else viewBoardNumber++;
    int drawMe[8][8];
    int i, j, k;
    k = 0;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            drawMe[i][j] = boardHistory[viewBoardNumber][k];
            k++;
        }
    }
   
    redraw_board(drawMe);
    if(viewBoardNumber == boardsRecorded-1){
        drawBlackGuides(currentBoard);
    }
}

void saveGameHandler(){
    if(preventSave) return;
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new ("Save File",
                                        GTK_WINDOW(window),
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                        NULL);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        char saveString[4097]; //64 boards * 64 squares = 4096   plus newline terminator
        saveString[4096] = '\0';
        int iterator = 0;
        for(; iterator < 4096; iterator++) saveString[iterator] = '9'; //9 will represent an unused thing
        int i = 0, j = 0;
        int saveStringPos = 0;
        for(i=0; i < boardsRecorded; i++){
            for(j=0; j < 64; j++){
                if(boardHistory[i][j] == BLANK){
                    saveString[saveStringPos] = '0';
                } else if(boardHistory[i][j] == BLACK){
                    saveString[saveStringPos] = '1';
                } else {
                    saveString[saveStringPos] = '2';
                }
            
                saveStringPos++;
            }
        }
        FILE *saveFile = fopen(filename, "w+");
        fprintf(saveFile, "%s\n", saveString);
        
        int moveIterator = 0;
        for(moveIterator = 0; moveIterator < 64; moveIterator++){
            if(moveIterator != 63) fprintf(saveFile, "%d ", moveHistory[moveIterator]);
            else fprintf(saveFile, "%d\n", moveHistory[moveIterator]);
        }

        fclose(saveFile);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

void loadGameFunction(char *filename){
        currentPlayer = BLACK;
        lockGrid = false;
        FILE *loadFile = fopen(filename, "r+");
        char loadBuffer[4500];
        fgets(loadBuffer, 4500, loadFile);
        char loadMoveBuffer[200];
        fgets(loadMoveBuffer, 200, loadFile);
        fclose(loadFile);
        gtk_label_set_text(GTK_LABEL(announcer), (const gchar*) "");
        
        //Reset moveHistory
        int i = 0;
        movesRecorded = 0;
        const char s[2] = " ";
        char *token;
        
        /* get the first token */
        token = strtok(loadMoveBuffer, s);
        
        int test = 0;
        /* walk through other tokens */
        while( token != NULL ) {
            moveHistory[test++] = atoi(token);
            if(atoi(token) != 0) movesRecorded++;
            //printf("Move: %d MaxMoves: %d\n", atoi(token), boardsRecorded);
            token = strtok(NULL, s);
        }
        
        textBoxLogic(moveHistory);
        
        int a = 0; 
        for(a = 0; a < boardsRecorded; a++) {
            free(boardHistory[a]);
        }
        boardsRecorded = 0;

        int offset = 0;
        int boardToRecord[8][8];
        while(loadBuffer[offset] != '9'){
            int i = 0;
            int j = 0;
            int k = 0;
            for(i = 0; i < 8; i++){
                for(j = 0; j < 8; j++){
                    boardToRecord[i][j] = loadBuffer[offset+k] - 48;
                    k++;
                }
                printf("\n");
            }
            
            recordBoard(boardToRecord);
            
            if(loadBuffer[offset + 64] == '9'){
                copyBoard(boardToRecord, currentBoard);
            }
            offset += 64;
        }
        viewBoardNumber = boardsRecorded -1;
        redraw_board(currentBoard);
        int *legalMoves = getAllLegalMoves(currentBoard, currentPlayer);
        if(legalMoves[0] == 0) { //If legalMoves[0] == 0 that means there were no legalMoves found
            endGame();
            return;
        } 
        free(legalMoves);
        legalMoves = getAllLegalMoves(currentBoard, getOpponent(currentPlayer));
        if(legalMoves[0] == 0) { //If legalMoves[0] == 0 that means there were no legalMoves found
            endGame();
            return;
        } 
        free(legalMoves);
        if(guidesOn){
            drawBlackGuides(currentBoard);
        }
}
void loadGameHandler(){
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Open File",
                                        GTK_WINDOW(window),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                        NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        loadGameFunction(filename);
        g_free (filename);
    }

    gtk_widget_destroy (GTK_WIDGET(dialog));
}

bool legalMoveHelper(int boardState[8][8], int row, int col, int rowInc, int colInc, int player, bool changePiece){
    int opponent = (player == WHITE) ? (BLACK) : (WHITE);
    row+=rowInc;
    col+=colInc;
    if(row <= 7 && row >= 0 && col <= 7 && col >= 0 && boardState[row][col] == opponent);
    else return false;
    
    row+=rowInc;
    col+=colInc;
    for(row, col; row <= 7 && row >= 0 && col <= 7 && col >= 0; row+=rowInc, col+=colInc){
        if(boardState[row][col] == opponent) continue;
        if(boardState[row][col] == player) {
            if(changePiece){
                while(boardState[row][col] != BLANK){ //Modify pieces according to rules
                    boardState[row][col] = player;
                    row-=rowInc;
                    col-=colInc;
                }
            }
            return true;
        }
        return false;
    }
    return false;
}

int* getAllLegalMoves(int boardState[8][8], int player){
    int opponent = (player == WHITE) ? (BLACK) : (WHITE);
    int *moves = calloc(48, sizeof(int));
    int move_i = 0;
    int i, j;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            if(!(boardState[i][j] == BLANK)) continue;
            //North -1, 0
            if(legalMoveHelper(boardState, i, j, -1, 0, player, false) ||
                //East  0, 1
                legalMoveHelper(boardState, i, j, 0, 1, player, false) ||
                //South 1, 0
                legalMoveHelper(boardState, i, j, 1, 0, player, false) ||
                //West  0, -1
                legalMoveHelper(boardState, i, j, 0, -1, player, false) ||
                //SW 1, -1
                legalMoveHelper(boardState, i, j, 1, -1, player, false) ||
                //NW -1,-1
                legalMoveHelper(boardState, i, j, -1, -1, player, false) ||
                //NE -1,1
                legalMoveHelper(boardState, i, j, -1, 1, player, false) ||
                //SE 1,1
                legalMoveHelper(boardState, i, j, 1, 1, player, false) 
            ) {
                moves[move_i] = i*8 + (j+1);
                move_i++;
            }
        }
    }
    return moves;
}

void makeMove(int player, int move, int boardState[8][8]){
    int i = (move-1) / 8;
    int j = (move-1) % 8;
    //North -1, 0
    legalMoveHelper(boardState, i, j, -1, 0, player, true);
    //East  0, 1
    legalMoveHelper(boardState, i, j, 0, 1, player, true);
    //South 1, 0
    legalMoveHelper(boardState, i, j, 1, 0, player, true);
    //West  0, -1
    legalMoveHelper(boardState, i, j, 0, -1, player, true);
    //SW 1, -1
    legalMoveHelper(boardState, i, j, 1, -1, player, true);
    //NW -1,-1
    legalMoveHelper(boardState, i, j, -1, -1, player, true);
    //NE -1,1
    legalMoveHelper(boardState, i, j, -1, 1, player, true);
    //SE 1,1
    legalMoveHelper(boardState, i, j, 1, 1, player, true);
    boardState[i][j] = player;
}

void recordBoard(int boardToRecord[8][8]){
    int *boardCopy = malloc(sizeof(int) * 64);
    int i,j;
    int k = 0;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            boardCopy[k] = boardToRecord[i][j];
            k++;
        }
    }
    boardHistory[boardsRecorded] = boardCopy;
    boardsRecorded++;
}

void recordMove(int moveToRecord){ //1-64
    if(movesRecorded == 0){ //Setup, 0 means no move
        int i = 0;
        for(i = 0; i < 64; i++){
            moveHistory[i] == 0;
        }
    }
    moveHistory[movesRecorded] = moveToRecord;
    movesRecorded++;
}


int squareValues[8][8] = {
    {1000,80,80,80,80,80,80,1000},
    {80,1,1,1,1,1,1,80},
    {80,1,1,1,1,1,1,80},
    {80,1,1,1,2,1,1,80},
    {80,1,1,2,1,1,1,80},
    {80,1,1,1,1,1,1,80},
    {80,1,1,1,1,1,1,80},
    {1000,80,80,80,80,80,80,1000}
}; 

int rawCount[8][8] = {
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1}
}; 

int boardHeuristic(int boardState[8][8], int player, int heuristic[8][8]){
    int opponent = (player == WHITE) ? (BLACK) : (WHITE);
    int boardScore = 0;
    int i, j;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            if(boardState[i][j] == player) {
                boardScore += heuristic[i][j];
            }
            if(boardState[i][j] == opponent){
                boardScore -= heuristic[i][j];
            }
        }
    }

    if(player == WHITE) return boardScore;
    if(player == BLACK) return boardScore * -1;
}

int max(int a, int b){
    if(a > b) return a;
    else return b;
}
int min(int a, int b){
    if(a < b) return a;
    else return b;
}

void copyBoard(int copyFrom[8][8], int copyTo[8][8]){
    int i, j;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            copyTo[i][j] = copyFrom[i][j];
        }
    }
}

void printBoard(int boardState[8][8]){
    int i, j;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            printf("%d ", boardState[i][j]);
        }
        printf("\n");
    }
}

int minimax(int player, int depth, int boardState[8][8]){
    int boardCopy[8][8];
    int opponent = (player == WHITE) ? (BLACK) : (WHITE);
    int score = (player == WHITE) ? (-99999) : (99999); //Initial score value

    int moveScores[40]; //Minimax children return values, maps 1-1 to "allMoves"
    int movesCounted = 0;

    int* allMoves = getAllLegalMoves(boardState, player);
    for(; allMoves[movesCounted] != 0; movesCounted++); //Store # of legal moves in "movesCounted"
    //printf("All legal moves top level: %d\n", movesCounted);

    if(movesCounted == 0){ //If there are no more moves, short-circuit
        int finalTally = boardHeuristic(boardState, player, rawCount);
        return (finalTally >= 0) ? 99999 : -99999;
    }
    if(depth == 0){ //If we have reached the limit
        return boardHeuristic(boardState, player, squareValues);
    }

    //Minimax recursion for each possible legal move stemming from the current board
    int i;
    for(i = 0; i < movesCounted; i++){ 
        copyBoard(boardState, boardCopy);
        makeMove(player, allMoves[i], boardCopy); //Changes the copied board
        moveScores[i] = minimax(opponent, depth-1, boardCopy); //Pass copied board to minimax children
    }

    //Return minimax'd children scores:
    int finalMove;
    int j = 0;
    int previousScore = -126365825;
    for(j = 0; j < movesCounted; j++){
        score = (player == WHITE) ? max(moveScores[j], score) : min(moveScores[j], score);
        if(previousScore != score){
            finalMove = allMoves[j];
            previousScore = score;
        }
    }
    free(allMoves);
    return score;
}

//-1 means no moves or fxn was called with depth = 0
int getComputerMove(int player, int depth, int boardState[8][8]){
    int boardCopy[8][8];
    copyBoard(boardState, boardCopy);
    int opponent = (player == WHITE) ? (BLACK) : (WHITE);
    int score = (player == WHITE) ? (-99999) : (99999); //Initial score value

    int moveScores[40]; //Minimax children return values, maps 1-1 to "allMoves"
    int movesCounted = 0;

    int* allMoves = getAllLegalMoves(boardState, player);
    for(; allMoves[movesCounted] != 0; movesCounted++); //Store # of legal moves in "movesCounted"

    if(movesCounted == 0){ //If there are no more moves, short-circuit
        return -1;
    }
    if(depth == 0){ //Should never be called with depth = 0
        return -1;
    }
    //Minimax recursion for each possible legal move stemming from the current board
    int i;
    for(i = 0; i < movesCounted; i++){ 
        copyBoard(boardState, boardCopy);
        makeMove(player, allMoves[i], boardCopy); //Changes the copied board
        moveScores[i] = minimax(opponent, depth-1, boardCopy); //Pass copied board to minimax children
    }

    //Return minimax'd children scores:
    int finalMove;
    int j = 0;
    int previousScore = -126365825;
    for(j = 0; j < movesCounted; j++){
        score = (player == WHITE) ? max(moveScores[j], score) : min(moveScores[j], score);
        if(previousScore != score){
            finalMove = allMoves[j];
            //printf("Computer said: %d\n", finalMove);
            previousScore = score;
        }
    }
    free(allMoves);
    return finalMove;
}

int getOpponent(int currentPlayer){
    return (currentPlayer == WHITE) ? (BLACK) : (WHITE);
}

void endGame(){
    preventSave = false;
    lockGrid = true;
    int i, j;
    int whiteCount = 0;
    int blackCount = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(currentBoard[i][j] == BLACK){
                blackCount += 1;
            }
            if(currentBoard[i][j] == WHITE){
                whiteCount += 1;
            }
        }
    }
    if(blackCount > whiteCount){
        gtk_label_set_text(GTK_LABEL(announcer), "Black won!");
    } else if (whiteCount > blackCount) {
        gtk_label_set_text(GTK_LABEL(announcer), "White won!");
    } else {
        gtk_label_set_text(GTK_LABEL(announcer), "It's a tie!");
    }

}


char* textMap[8][8] = {
    {"A1","A2","A3","A4","A5","A6","A7","A8"},
    {"B1","B2","B3","B4","B5","B6","B7","B8"},
    {"C1","C2","C3","C4","C5","C6","C7","C8"},
    {"D1","D2","D3","D4","D5","D6","D7","D8"},
    {"E1","E2","E3","E4","E5","E6","E7","E8"},
    {"F1","F2","F3","F4","F5","F6","F7","F8"},
    {"G1","G2","G3","G4","G5","G6","G7","G8"},
    {"H1","H2","H3","H4","H5","H6","H7","H8"},
}; 

void textBoxLogic(int moveList[64]){
    if(moveList[0] == 0) {
        gtk_text_buffer_set_text(textbuffer, "", -1);
        return;
    }
    char buffer[3000];
    buffer[2999] = '\0';
    int i = 0;
    for(i=0; moveList[i] != 0; i++){
         int row = (moveList[i]-1) / 8;
        int col = (moveList[i]-1) % 8;
        if(i == 0) sprintf(buffer, "%d. %s\n", i, textMap[row][col]);
        else sprintf(buffer + strlen(buffer), "%d. %s\n", i, textMap[row][col]);
    }
    gtk_text_buffer_set_text(textbuffer, buffer, -1);
}

void toggleGuideHandler(){
    guidesOn = !guidesOn;
    viewBoardNumber = boardsRecorded - 1;
    redraw_board(currentBoard);
    if(guidesOn) drawBlackGuides(currentBoard);
    if(guidesOn) {
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "toggleGuides")), "Guides On");
    } else {
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(builder, "toggleGuides")), "Guides Off");
    }
}

//Implements game logic for grid buttons clicked
void gridClickedHandler(GtkWidget *widget, void *data){
    //printBoard(currentBoard);
    if(viewBoardNumber != boardsRecorded-1) {
        viewBoardNumber = boardsRecorded-1;
        redraw_board(currentBoard);
        drawBlackGuides(currentBoard);
        return;
    }
    int moveMade = *(int *) g_object_get_data(G_OBJECT(widget), "gridPosition");

    int *legalMoves = getAllLegalMoves(currentBoard, currentPlayer);
    if(legalMoves[0] == 0) { //If legalMoves[0] == 0 that means there were no legalMoves found
        endGame();
        return;
    } 

    int iterator = 0;
    int i = (moveMade-1) / 8;
    int j = (moveMade-1) % 8;
    //printf("Row: %d     Col: %d\n", i, j);
    
    for(iterator = 0; legalMoves[iterator] != 0; iterator++){
        if(legalMoves[iterator] == moveMade){
            //Human makes a move that is legal
            int i = (moveMade-1) / 8;
            int j = (moveMade-1) % 8;
            makeMove(currentPlayer, moveMade, currentBoard);
            recordMove(moveMade);
            textBoxLogic(moveHistory);
            redraw_board(currentBoard);
            recordBoard(currentBoard);
            viewBoardNumber = boardsRecorded-1;
            //Flip the current player value:
            currentPlayer = getOpponent(currentPlayer);
            //Computer makes a move

            preventSave = true;
            
            int compMove = getComputerMove(currentPlayer, 2, currentBoard);
            if(compMove < 0) {
                endGame();
                return;
            } 
            // printf("Comp Move: %d\n", compMove);
            // printf("Legal Move: %d\n", moveMade);

            makeMove(currentPlayer, compMove, currentBoard);
            recordMove(compMove);
            textBoxLogic(moveHistory);
            redraw_board(currentBoard);
            recordBoard(currentBoard);
            viewBoardNumber = boardsRecorded-1;
            //Flip the current player value:
            currentPlayer = getOpponent(currentPlayer);
            drawBlackGuides(currentBoard);
            preventSave = false;
            
            break;
        }
    }
    free(legalMoves);
    
    //One more check for to see if the game has ended after the computer plays
    legalMoves = getAllLegalMoves(currentBoard, currentPlayer);
    if(legalMoves[0] == 0) { //If legalMoves[0] == 0 that means there were no legalMoves found
        endGame();
        return;
    } 

    free(legalMoves);
}

void map_buttons(GtkGrid *chessboard){
    int i, j; 
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            buttonMappings[i][j] = GTK_BUTTON(gtk_grid_get_child_at(chessboard, j, i));
            gtk_button_set_label(buttonMappings[i][j], "");
            void *ptr = malloc(sizeof(int));
            *((int*)ptr) = i*8 + (j+1);
            g_object_set_data(G_OBJECT(buttonMappings[i][j]), "gridPosition", ptr); 
            //printf("Number: %d\n", *(int* )ptr);
            g_signal_connect(buttonMappings[i][j], "button-press-event", G_CALLBACK(gridClickedHandler), NULL);
        }
    }
}

void myCSS(void){
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;
    int i, j;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            GtkStyleContext *context;
            context = gtk_widget_get_style_context(GTK_WIDGET(buttonMappings[i][j]));
            gtk_style_context_add_class(context,"gridBox");
        }
    }
    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *myCssFile = "style.css";
    GError *error = 0;

    gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
    g_object_unref (provider);
}

gboolean timer_handler(GtkWidget * widget){
    //printf("Hi.\n");
    return true;
}

void newGameHandler(){
    currentPlayer = BLACK;
    loadGameFunction("./NewGame");
}

int main(int argc, char *argv[]){
    printf("Wow!\n");
	gtk_init(&argc, &argv); //init Gtk
	builder = gtk_builder_new_from_file("Chess.glade");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "backButton"), "button-press-event", G_CALLBACK(backClickedHandler), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "forwardButton"), "button-press-event", G_CALLBACK(forwardClickedHandler), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "saveGame"), "button-press-event", G_CALLBACK(saveGameHandler), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "loadGame"), "button-press-event", G_CALLBACK(loadGameHandler), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "newGame"), "button-press-event", G_CALLBACK(newGameHandler), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "toggleGuides"), "button-press-event", G_CALLBACK(toggleGuideHandler), NULL);
	gtk_builder_connect_signals(builder, NULL);
    announcer = GTK_WIDGET(gtk_builder_get_object(builder, "announcerLabel"));
    newGame = GTK_WIDGET(gtk_builder_get_object(builder, "newGame"));
	chessboard = GTK_GRID(gtk_builder_get_object(builder, (const gchar*) "chessboard"));
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer1"));
    
    map_buttons(chessboard);
    myCSS();
    redraw_board(currentBoard);
    recordBoard(currentBoard);
	gtk_widget_show(window);

    int ID = g_timeout_add_seconds(1, G_SOURCE_FUNC(timer_handler), NULL);
    if(guidesOn){
        drawBlackGuides(currentBoard);
    }
	gtk_main();

	return 0;
}



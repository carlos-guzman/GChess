#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POS(c, r) 	((0 | (c-'a')) | ((r-'1')<<3))
#define ROW(c)		(((c & 0x38) >> 3) + '1')
#define COL(c)		((c & 0x7) + 'a')
#define BOARD(c, r) board[8-(r-'0')][c-'a']
#define BSIZE 		8
#define SYMBOLS 0
#if SYMBOLS == 1
#include <wchar.h>
#else
//#define wchar_t char
// #define %lc %c
#define wprintf printf
#endif
#define DEBUG 1

typedef struct piece{
	wchar_t type;
	char position;
	char color;
}piece;

piece* board[BSIZE][BSIZE];

void insert(char color, wchar_t type, char column, char row){
	piece* p = (piece*) malloc(sizeof(piece));
	p->color = color;
	p->type = type;
	p->position = POS(column, row);
	BOARD(column, row) = p;
}

void setup_board(){
	int i, j;
	piece* p;
	//Setup pawns
	for(i=0;i<BSIZE;i++){
		//Setup black pawns
		insert('b', 'P', 'a'+i, '7');

		//Setup white pawns
		insert('w', 'P', 'a'+i, '2');

		if(DEBUG){
			for (j = 0; j < 8; j++) {
	   		   printf("%d", !!((BOARD('a'+i, '7')->position << j) & 0x80));
	  		}
  			printf("\n\t");

			for (j = 0; j < 8; j++) {
	   		   printf("%d", !!((COL(BOARD('a'+i, '7')->position) << j) & 0x80));
	  		}

	  		printf("%c", COL((BOARD('a'+i, '7')->position)));
  			printf("\n");
	  	}
	}
	//Rooks
	insert('b', 'R', 'a', '8');
	insert('b', 'R', 'h', '8');
	insert('w', 'R', 'a', '1');
	insert('w', 'R', 'h', '1');

	//Knights
	insert('b', 'N', 'b', '8');
	insert('b', 'N', 'g', '8');
	insert('w', 'N', 'b', '1');
	insert('w', 'N', 'g', '1');

	//Bishops
	insert('b', 'B', 'c', '8');
	insert('b', 'B', 'f', '8');
	insert('w', 'B', 'c', '1');
	insert('w', 'B', 'f', '1');

	//Queens
	insert('b', 'Q', 'd', '8');
	insert('w', 'Q', 'd', '1');

	//Kings
	insert('b', 'K', 'e', '8');
	insert('w', 'K', 'e', '1');
}

void print_line(){
	int i;
	printf(" ");
	for(i=0;i<BSIZE*4-1;i++)
		printf("-");
	printf("\n");
}

wchar_t symbol(piece* p){
	/*
	FOLLOWING LINES ONLY WORK IF UTF-8 COULD BE ENABLED.
	i DON'T KNOW HOW TO DO THIS

	char c = (p->color == 'w')? 0x2654 : 0x265A;

	switch(p->type){
		case 'P': c++;
		case 'N': c++;
		case 'B': c++;
		case 'R': c++;
		case 'Q': c++;
	}
	*/
	wchar_t c;
	if(p->color == 'w'){
		switch(p->type){
			case 'K': c = L'♔';break;
			case 'Q': c = L'♕';break;
			case 'R': c = L'♖';break;
			case 'B': c = L'♗';break;
			case 'N': c = L'♘';break;
			case 'P': c = L'♙';break;
		}
	}else{
		switch(p->type){
			case 'K': c = L'♚';break;
			case 'Q': c = L'♛';break;
			case 'R': c = L'♜';break;
			case 'B': c = L'♝';break;
			case 'N': c = L'♞';break;
			case 'P': c = L'♟';break;
		}
	}
	return c;
}

void print_board(){
	int i, j;
	wchar_t piece;
	print_line();
	for(i=0;i<BSIZE;i++){
		printf("|");
		for(j=0;j<BSIZE;j++){
			
			if(board[i][j]==NULL)
				piece = ' ';
			else if(SYMBOLS)
				piece = symbol(board[i][j]);
			else if(board[i][j]->color=='w')
				piece = board[i][j]->type;
			else
				piece = tolower(board[i][j]->type);
			
			printf(" %lc |", piece);
		}
		printf("\n");
		print_line();
	}
}

void clear_board(){
	int i, j;
	for(i=0;i<BSIZE;i++)
		for(j=0;j<BSIZE;j++){
			if(board[i][j]!=NULL)
				free(board[i][j]);
		}
}

void move_piece(piece* p, char c, char r){
	printf("Moving %c%c%c\n", p->type, c, r);
	// BOARD(p)
	BOARD(c, r) = p;
	BOARD(COL(p->position), ROW(p->position)) = NULL;

	if(DEBUG) print_board();
}

char input_move(char* move){
	if(strlen(move) < 2 || strlen(move) > 7)//Take dxe8=Q+ into account
		return 0;
	//It is a pawn
	if(*move >= 'a' && *move <= 'h'){
		//The pawn is not taking
		if(move[1]!='x'){
			//Potentially first move (white)
			if(move[1]=='4'){
				//Not a first move
				if(BOARD(move[0], '3') != NULL 
					&& BOARD(move[0], '3')->type == 'P'){

					move_piece(BOARD(move[0], '3'), move[0], move[1]);
				//A first move with no pieces in between
				}else if(BOARD(move[0], '2') != NULL
					&& BOARD(move[0], '2')->type == 'P'
					&& BOARD(move[0], '3') == NULL){
					
					move_piece(BOARD(move[0], '2'), move[0], move[1]);
				}
			}

			//Potentially first move (black)
			if(move[1]=='5'){
				//Not a first move
				if(BOARD(move[0], '6') != NULL 
					&& BOARD(move[0], '6')->type == 'P'){

					move_piece(BOARD(move[0], '6'), move[0], move[1]);
				//A first move with no pieces in between
				}else if(BOARD(move[0], '7') != NULL
					&& BOARD(move[0], '7')->type == 'P'
					&& BOARD(move[0], '6') == NULL){
					
					move_piece(BOARD(move[0], '7'), move[0], move[1]);
				}
			}
		}
	}
}

int main(){
	setup_board();
	print_board();
	input_move("e4");
	input_move("e5");
	clear_board();
}
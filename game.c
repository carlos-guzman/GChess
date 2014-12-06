/* Copyright © 2014 by Carlos Guzmán Fernández
 * 
 * 
 * GChess
 * 
 * Chess board that allows to enter valid moves and positions into a board
 * In the future this might turn into an engine
 * But for now, let's just annotate games and then we step higher
 * 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

//Translate column and row characters into a byte containing both.
//00[3 bit row(0-7)][3 bit column(0-7)]
#define POS(c, r) 	((0 | (c-'a')) | ((r-'1')<<3))

//Get the row or column given a position byte
#define ROW(c)		(((c & 0x38) >> 3) + '1')
#define COL(c)		((c & 0x7) + 'a')

//Translate the board from chess notation to array notation
#define BOARD(c, r) board[8-(r-'0')][c-'a']

//Size of the board
#define BSIZE 		8

//Represent the board with symbols (1) or just letters(0)
#define SYMBOLS 0
#if SYMBOLS == 1
#include <wchar.h>
#else
#define wchar_t char
// #define %lc %c
#define wprintf printf
#endif

//Debug option, pretty much equivalent to more verbose
#define DEBUG 1

//Code for moving a knight in the direction of the signs of col and row
 									//Check for row limits
#define MOVE_KNIGHT(col, row) if(DEBUG)printf("Knight: %d %d %c %c %d\n", col, row, dest_column, dest_row, BOARD(dest_column+col, dest_row+row) != NULL); \
 		if(((row > 0)? (dest_row <= '8'-row) : (dest_row >= '1'-row)) \
 		/*Check for column limits */\
		&& ((col > 0)? (dest_column <= 'h'-col) : (dest_column >= 'a'-col)) \
		/*Check that there is a piece*/\
		&& BOARD(dest_column+col, dest_row+row) != NULL \
		/*Check that the piece is a knight */\
		&& BOARD(dest_column+col, dest_row+row)->type == 'N' \
		/*Check that the knight is of the right color */\
		&& BOARD(dest_column+col, dest_row+row)->color == turn){ \
			printf("aaa\n");\
			move_piece(BOARD(dest_column+col, dest_row+row), dest_column, dest_row); \
			return 1; \
		}

//Each piece has a type, position byte(column and row) and color
typedef struct piece{
	wchar_t type;
	char position;
	char color;
}piece;

piece* board[BSIZE][BSIZE];

//Determine whose move it is
char turn = 'w';

/*
 * insert
 * 
 * Insert a piece into the board, given its characteristics
 */
void insert(char color, wchar_t type, char column, char row){
	piece* p = (piece*) malloc(sizeof(piece));
	p->color = color;
	p->type = type;
	p->position = POS(column, row);
	BOARD(column, row) = p;
}

/*
 * setup_board 
 * 
 * Set the board up for a regular game
 */
void setup_board(){
	int i, j;
	piece* p;
	//Pawns
	for(i=0;i<BSIZE;i++){
		//Setup black pawns
		insert('b', 'P', 'a'+i, '7');

		//Setup white pawns
		insert('w', 'P', 'a'+i, '2');

		if(DEBUG){
			//Print position byte
			for (j = 0; j < 8; j++) {
	   		   printf("%d", !!((BOARD('a'+i, '7')->position << j) & 0x80));
	  		}
  			printf("\n\t");

  			//Print column/row value
			for (j = 0; j < 8; j++) {
	   		   printf("%d", !!((/*ROW*/COL(BOARD('a'+i, '7')->position) << j) & 0x80));
	  		}

	  		printf("%c", /*ROW*/COL((BOARD('a'+i, '7')->position)));
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

/*
 * print_line
 * 
 * Prints to the screen a horizontal line. Used when printing the board.
 */
void print_line(){
	int i;
	printf(" ");
	for(i=0;i<BSIZE*4-1;i++)
		printf("-");
	printf("\n");
}

/*
 * get_symbol
 * 
 * Get the unicode symbol for a specific piece to print on the screen
 * 
 * @return wchar_t wide character holding the piece's representation
 */
wchar_t get_symbol(piece* p){
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

/*
 * print_board
 * 
 * Prints the whole board with it's pieces to the screen
 * There are no square colors, so just consider the bottom right square (h1)
 * white and the rest intercalating colors.
 * 
 * Without symbols, the representations are as follows:
 *  WHITE is Uppercase, black is lowercase
 * P - pawn; N - Knight; B - Bishop;
 * R - Rook; Q - Queen;  K - King; 
 * 
 */
void print_board(){
	int i, j;
	wchar_t piece;
	print_line();
	//Print each row
	for(i=0;i<BSIZE;i++){
		printf("|");
		//Print each square
		for(j=0;j<BSIZE;j++){
			if(board[i][j]==NULL)
				piece = ' ';
			else if(SYMBOLS)
				piece = get_symbol(board[i][j]);
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

/*
 * clear_board
 * 
 * Frees all the allocated memory for the pieces on the board
 */
void clear_board(){
	int i, j;
	for(i=0;i<BSIZE;i++)
		for(j=0;j<BSIZE;j++){
			if(board[i][j]!=NULL)
				free(board[i][j]);
		}
}

/*
 * move_piece
 * 
 * Move a given piece to a valid given square
 */
void move_piece(piece* p, char c, char r){
	if(p == NULL) return;
	if(DEBUG) printf("Moving %c%c%c\n", p->type, c, r);
	
	BOARD(c, r) = p;
	BOARD(COL(p->position), ROW(p->position)) = NULL;
	p->position = POS(c, r);

	//Change turn
	turn = (turn == 'w')? 'b' : 'w';
	print_board();
}
/*
 * validate_move
 * 
 * Validate the given string for a move
 * 
 */
char validate_move(char* move){
	if(DEBUG)printf("Validating %s\n", move);
	regex_t regex;
	if(strlen(move) < 2 || strlen(move) > 7)//Take dxe8=Q+ into account
		return 0;

	char* expression = "^\\(K\\|\\([QRBN][a-h1-8]\\?\\)\\)\\?\\([a-h]x\\)\\?[a-h][1-8]\\(\\=[QRBN]\\)\\?[\\+\\#]\\?$";
	//https://regex101.com/r/sH3lF4/2
	regcomp(&regex, expression, 0);

	if(regexec(&regex, move, 0, NULL, 0)) return 0;
	
	regfree(&regex);
	return 1;
}

// char move_knight(char dest_column, char dest_row, char col, char row){
// 	if((row > 0)? (dest_row <= '8'-row) : (dest_row >= '1'-row) 
// 		&& (col > 0)? (dest_column <= 'h'-col) : (dest_column >= 'a'-col)
// 		&& BOARD(dest_column+col, dest_row+row)->type == 'N' &&
// 		BOARD(dest_column+col, dest_row+row)->color == turn){
// 			move_piece(BOARD(dest_column+col, dest_row+row), dest_column, dest_row);
// 			return 1;
// 		}
// 	return 0;
// }

/*
 * input_move
 * 
 * Given a string that represents a move, execute it if possible
 * 
 * @returns 0 if move is not valid/legal
 * 			1 if move is satisfactory
 */
char input_move(char* move){
	char dest_column, dest_row, source;
	if(!validate_move(move)) return 0;
	if(DEBUG) printf("Input move: %s", move);
	//It is a pawn
	if(*move >= 'a' && *move <= 'h'){
		if(DEBUG) printf(" -> P");
		//Not a capture
		if(move[1]!='x'){
			dest_column = move[0];
			dest_row = move[1];
			//Square already occupied
			if(BOARD(dest_column, dest_row)) return 0;
			//Regular case for white pawn
			if(BOARD(dest_column, dest_row-1) != NULL 
				&& BOARD(dest_column, dest_row-1)->type == 'P'
				&& BOARD(dest_column, dest_row-1)->color == 'w'
				&& turn == 'w'){
				
				move_piece(BOARD(dest_column, dest_row-1), dest_column, dest_row);

				//Check if in last row
				if(dest_row=='8')
					if(strlen(move)>3 && move[2]!='='
					&& (move[3]=='Q' || move[3]=='R' || move[3]=='B' || move[3]=='N'))
						BOARD(dest_column, dest_row)->type = move[3];
					else return 0;

			//Regular case for black pawn			
			}else if(BOARD(dest_column, dest_row+1) != NULL 
				&& BOARD(dest_column, dest_row+1)->type == 'P'
				&& BOARD(dest_column, dest_row+1)->color == 'b'
				&& turn == 'b'){
				
				move_piece(BOARD(dest_column, dest_row+1), dest_column, dest_row);	

				//Check if in last row for pawn
				if(dest_row=='1')
					if(strlen(move)>3 && move[2]!='='
					&& (move[3]=='Q' || move[3]=='R' || move[3]=='B' || move[3]=='N'))
						BOARD(dest_column, dest_row)->type = move[3];
					else return 0;			
			
			//First pawn move (white)
			}else if(dest_row=='4'){
				//A first move with no pieces in between
				if(BOARD(dest_column, '2') != NULL
					&& BOARD(dest_column, '2')->type == 'P'
					&& BOARD(dest_column, '2')->color == 'w'
					&& BOARD(dest_column, '3') == NULL){
					
					move_piece(BOARD(dest_column, '2'), dest_column, dest_row);
				}else return 0;
			
			//First pawn move (black)
			}else if(dest_row=='5'){
				//A first move with no pieces in between
				if(BOARD(dest_column, '7') != NULL
					&& BOARD(dest_column, '7')->type == 'P'
					&& BOARD(dest_column, '7')->color == 'b'
					&& BOARD(dest_column, '6') == NULL){

					move_piece(BOARD(dest_column, '7'), dest_column, dest_row);
				}else return 0;
			}else return 0;
		//Pawn capture (not en passant yet)
		}else{
			source = move[0];
			dest_column = move[2];
			dest_row = move[3];

			if(DEBUG) printf(" -> x");
			//Too short or not in adjacent column
			if(source != dest_column+1 && source != dest_column-1) return 0;
			//Capture with white pawn
			if(BOARD(dest_column, dest_row) != NULL && BOARD(source, dest_row-1) != NULL 
				//The pieces are different colors
				&& BOARD(dest_column, dest_row)->color != BOARD(source, dest_row-1)->color
				//The piece is a white pawn
				&& BOARD(source, dest_row-1)->type == 'P' && BOARD(source, dest_row-1)->color == 'w'
				//The other piece is not a king
				&& BOARD(dest_column, dest_row)->type != 'K'){

				if(DEBUG)printf(" -> w");
				move_piece(BOARD(source, dest_row-1), dest_column, dest_row);

			//Capture with black pawn
			}else if(BOARD(dest_column, dest_row) != NULL && BOARD(source, dest_row+1) != NULL 
				//The pieces are different colors
				&& BOARD(dest_column, dest_row)->color != BOARD(source, dest_row+1)->color
				//The piece is a white pawn
				&& BOARD(source, dest_row+1)->type == 'P' && BOARD(source, dest_row+1)->color == 'b'
				//The other piece is not a king
				&& BOARD(dest_column, dest_row)->type != 'K'){

				if(DEBUG)printf(" -> b");
				move_piece(BOARD(source, dest_row+1), dest_column, dest_row);
			}
		}
		if(DEBUG)printf("\n");
	}
	//It is a knight
	else if(*move == 'N'){
		//Nota a capture
		if(move[1] != 'x'){
			//Check for notation like Nbd7.*
			if(strlen(move)>3 && move[3] >= '1' && move[3] <= '8'){
				source = move[1];
				dest_column = move[2];
				dest_row = move[3];
			}else{
				dest_column = move[1];
				dest_row = move[2];
			}
			
			//Check for valid knights
			//Depending whether knight has been specified
			if(!source || source == dest_column+1){
				MOVE_KNIGHT(-1, 2);
				MOVE_KNIGHT(-1,-2);
				printf("1\n");
			}
			if(!source || source == dest_column-1){
				MOVE_KNIGHT(1, 2);
				MOVE_KNIGHT(1,-2);
				printf("2\n");
			}
			if(!source || source == dest_row+1){
				MOVE_KNIGHT(2,-1);
				MOVE_KNIGHT(-2,-1);
				printf("3\n");
			}
			if(!source || source == dest_row-1){
				MOVE_KNIGHT(2, 1);
				MOVE_KNIGHT(-2, 1);
			}else if(source == dest_column-2){
				MOVE_KNIGHT(2, 1);
				MOVE_KNIGHT(2,-1);
			}else if(source == dest_column+2){
				MOVE_KNIGHT(-2, 1);
				MOVE_KNIGHT(-2,-1);
			}else if(source == dest_row-2){
				MOVE_KNIGHT(1, 2);
				MOVE_KNIGHT(-1,2);
			}else if(source == dest_row+2){
				MOVE_KNIGHT(1, -2);
				MOVE_KNIGHT(-1,-2);
			}
		}
		// Knight captures
		else if(strlen(move)<4) return 0;
	}
	if(!DEBUG) printf("%s\n", move);

	source = 0;
	dest_row = 0;
	dest_column = 0;
}

int main(){
	setup_board();
	print_board();
	//Pawn testing
	input_move("e4");//First move white double
	input_move("e5");//First move black double
	input_move("d3");//First move white regular
	input_move("d6");//First move black regular
	input_move("f4");
	input_move("c5");
	input_move("f5");//Regular move white
	input_move("f6");
	input_move("e5");//Should not be played
	input_move("d5");//Should not be played
	input_move("d4");
	input_move("c4");//Regular move black
	input_move("c3");
	input_move("exd4");//Black takes
	input_move("cxd4");//White takes
	input_move("ac1");

	input_move("Nc6");//Knight game
	input_move("Nf3");
	//White and black can move to the same square
	if(0)
		input_move("Nh6");
	input_move("Ne5");
	input_move("Nc3");
	input_move("Nh6");
	input_move("b3");
	input_move("Nfg4");//Specify column


	clear_board();
}
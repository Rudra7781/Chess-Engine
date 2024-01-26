
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// neagtion of unsigned bit gives 2 compliment
// LEFT SHIFT GO DOWN << 
// RIGHT SHIFT GO UP >>
// i = rank ,  j =  file
// define bitboard data type
#define U64 unsigned long long

// FEN debug position  after board, who will move, castle, enpassant, halfmove(Draw after 50 fullmove so 100),  fullmove ( how many total move is made)
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQq h4 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w Qkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b kQ c4 0 9 "



//  positions
enum {
    a8,  b8,  c8,  d8,  e8,  f8,  g8,  h8, 
    a7,  b7,  c7,  d7,  e7,  f7,  g7,  h7, 
    a6,  b6,  c6,  d6,  e6,  f6,  g6,  h6, 
    a5,  b5,  c5,  d5,  e5,  f5,  g5,  h5, 
    a4,  b4,  c4,  d4,  e4,  f4,  g4,  h4, 
    a3,  b3,  c3,  d3,  e3,  f3,  g3,  h3, 
    a2,  b2,  c2,  d2,  e2,  f2,  g2,  h2, 
    a1,  b1,  c1,  d1,  e1,  f1,  g1,  h1, no_sqr 
};


// encodde pieces | upper case : WHITE | lower case : BLACK  *******************************************
enum { P, N, B, R, Q, K, p, n, b, r, q, k};
 
// castling bits **********************************************************************

/*
0001    1   white King King can Castle to king side
0010    1   white King King can Castle tu queen side
0100    1   Black King King can Castle tu king side
1000    1   Black King King can Castle tu queen side
-----
1111    any king Kings Castle in any direction
1001    Black King, King Castle to queen side and white King can Castle to King side
*/
enum { wk = 1, wq = 2, bk = 4, bq = 8};


// sides to move

const char *square_to_coordinates[] = {
    "a8",  "b8",  "c8",  "d8",  "e8",  "f8",  "g8",  "h8", 
    "a7",  "b7",  "c7",  "d7",  "e7",  "f7",  "g7",  "h7", 
    "a6",  "b6",  "c6",  "d6",  "e6",  "f6",  "g6",  "h6", 
    "a5",  "b5",  "c5",  "d5",  "e5",  "f5",  "g5",  "h5", 
    "a4",  "b4",  "c4",  "d4",  "e4",  "f4",  "g4",  "h4", 
    "a3",  "b3",  "c3",  "d3",  "e3",  "f3",  "g3",  "h3", 
    "a2",  "b2",  "c2",  "d2",  "e2",  "f2",  "g2",  "h2", 
    "a1",  "b1",  "c1",  "d1",  "e1",  "f1",  "g1",  "h1" 
};


// ascii pieces ************************************************************************
char ascii_pieces[12] = "PNBRQKpnbrqk";

// unicode pieces
char *unicode_pieces[12] = {  "♟︎", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};

//  "♟︎", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔" 

// convert to ascii character to encoded constants | as peer enum
int char_pieces[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k,
};

// promoted pieces
char promoted_pieces[] = {
    
    [N] = 'n',
    [B] = 'b',
    [R] = 'r',
    [Q] = 'q',
    [K] = 'K',
    [n] = 'n',
    [b] = 'b',
    [r] = 'r',
    [q] = 'q',
};


// *******************************************************************************
enum {white, black, both}; // 0,1

enum {rook, bishop}; // 0,1




// DEFINE BITBORADS  ************************************************************************

//define piece bit board : all 12 types of pieces
U64 bitboards[12];

//occupancies bit board  : white, black, all
U64 occupancies[3];

// side to move 
int side;

// init enpassant square
int enpassant  = no_sqr;

//  castling right variables
int castle;



//         RANDOM NUMBERS 

// generate pseudo random number state *************************************************************************************************

// pseudo random number state
unsigned int random_state = 1804289383;

unsigned int get_random_U32_number(){
    
    unsigned int number =  random_state; // 32 bit

    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    //update state
    random_state = number;
    return number;
}


// generate pseudo random number state
U64 get_random_U64_number(){
    
    //define 4 random numbers
    U64 n1, n2, n3, n4;

    //init number
    n1 = (U64)(get_random_U32_number()) & 0xFFFF; // slice first 16 bit 
    n2 = (U64)(get_random_U32_number()) & 0xFFFF; // slice first 16 bit 
    n3 = (U64)(get_random_U32_number()) & 0xFFFF; // slice first 16 bit 
    n4 = (U64)(get_random_U32_number()) & 0xFFFF; // slice first 16 bit 

    // return number n1 | n2 | n3 | n4
    return n1 | (n2 << 16) | (n3 << 32) |  (n4 <<  48);
}

//generat magic number 
U64 generate_magic_number(){
    return (get_random_U64_number() & get_random_U64_number() & get_random_U64_number());
}


// macors Bit manipulation ***************************************************************************************************************************************

#define get_bit(BitBoard,square) ((BitBoard) & ( 1ULL << (square))) // return zero, if Bit does not exist
#define set_bit(BitBoard,position) ((BitBoard) |= (1Ull << (position))) //
#define pop_bit(BitBoard,position) (((BitBoard) &= ~(1ULL << (position))))

//count bits

#define  count_bits(bitboard) __builtin_popcountll(bitboard)
// static inline int count_bits(U64 bitboard){    
//     int count = 0;
//     while(bitboard){
//         bitboard &= (bitboard - 1);
//         count ++;
//     }
//     return count;
// } 

// printBitBoard((block & -block) - 1);  // get inde of first significant bit from 0
#define get_ls1b_index(bitboard) ffsll(bitboard) - 1



// INPUT & OUTPUT *****************************************************************




//func to print 
void printBitBoard(U64 BitBoard){

    printf("\n");
    for (int i = 0; i < 8; i++)
    {
        printf(" %d  ",8 - i);
        for (int j = 0; j < 8; j++)
        {   
            int square = i * 8 + j;
            
            printf(" %d ", get_bit(BitBoard, square) ? 1 : 0);
        }
        printf("\n");
    }
    printf("\n     a  b  c  d  e  f  g  h  \n\n");
    printf("    BitBoard : %llu decimal  \n",BitBoard);
}


// print board

void print_board(){
    // loop over board
    
    printf("\n");
    for (int rank = 0; rank < 8; rank++)
    {
        printf("  %d ", 8 - rank);
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8  +  file;

            //define piece var
            int piece = -1;

            // loop over all 12 / pieces bitboards 
            for (int bb_piece = P; bb_piece <= k; bb_piece++)
            {
                if (get_bit(bitboards[bb_piece],square))
                    piece = bb_piece; 
                
            }

            #ifdef WIN64
                printf(" %c ", (piece == -1) ? '.' : ascii_pieces[piece]);
            #else
                printf(" %s ", (piece == -1) ? "." : unicode_pieces[piece]);
            #endif


        }
        printf("\n");
    }
    printf("\n     a  b  c  d  e  f  g  h  \n\n");
    
    // print side to move
    printf("     Side      :  %s\n", !side ? "White" : "Black");

    // print enpassant
    printf("     Enpassant :  %s\n", ( enpassant != no_sqr) ? square_to_coordinates[enpassant] : "No");
    
    // print castling rights
    printf("     Castling  :  %c%c%c%c\n\n", (castle & wk) ?  'K' :  '-', (castle & wq) ?  'Q' :  '-', (castle & bk) ?  'k' :  '-', (castle & bq) ?  'q' :  '-');

}

// parse FEN string
void parse_fen(char *fen){

    //reset all board position 
    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(occupancies, 0ULL, sizeof(occupancies));

    // state varaible
    side = 0;
    enpassant = no_sqr;
    castle = 0;

    // loop over board rank
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8  +  file;

            // match ascii pieces with fen
            if( (*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')){
                //init piece type 
                int piece = char_pieces[*fen];

                //set piece on bitboard
                set_bit(bitboards[piece], square);

                // increment the pointer
                fen++;
            }

            //match number

            if (*fen >= '0' && *fen <= '9'){

                // init offest ( convert char '0' to int 0)
                int offset = *fen - '0';

                int piece = -1;

                // loop over all 12 / pieces bitboards 
                for (int bb_piece = P; bb_piece <= k; bb_piece++)
                {
                    // if a piece is found on square that means no decrement
                    if (get_bit(bitboards[bb_piece],square))
                        piece = bb_piece; 
                    
                }

                if (piece == -1) file--;
                
                //adjust file count
                file += offset;

                //inc pointer
                fen++;
            }

            if (*fen == '/')
            {
                fen++;
            }
            
        }
            // fen++;
    }

    // going to side variable
    fen++; // blank space

    // parse side to move
    side =  (*fen == 'w') ? white : black;

    // go to parsing castling rights 
    fen += 2; // char and blank space

    // parsing castling rights 
    while (*fen != ' ')
    {
        switch (*fen)
        {
            case 'K': castle |= wk; break;
            case 'Q': castle |= wq; break;
            case 'k': castle |= bk; break;
            case 'q': castle |= bq; break;
            case '-': break;
        
            default:
                break;
        }
        fen++;
    }
    
    // go to enpassant
    fen++;

    // parse enpassant
    if (*fen != '-')
    {
        //  parse enpassant file & rank
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');
        enpassant = rank * 8 + file;
    }
    else 
        enpassant = no_sqr;
    
    // init white occupancies : loop over white  pieces bit board
    for(int piece = P; piece <= K; piece++){
        //populate white occupancy
        occupancies[white] |= bitboards[piece];
    }
    // loop over white  pieces bit board
    for(int piece = p; piece <= k; piece++){
        //populate black occupancy
        occupancies[black] |= bitboards[piece];
    }

    // all occupancies
    occupancies[both] = occupancies[white] | occupancies[black]; 

}



// ATTACKS *****************************************************************************




/* Not A FILE
  8   0  1  1  1  1  1  1  1 
  7   0  1  1  1  1  1  1  1 
  6   0  1  1  1  1  1  1  1 
  5   0  1  1  1  1  1  1  1 
  4   0  1  1  1  1  1  1  1 
  3   0  1  1  1  1  1  1  1 
  2   0  1  1  1  1  1  1  1 
  1   0  1  1  1  1  1  1  1  
  
  
  
  NOT H FILE
   8   1  1  1  1  1  1  1  0 
   7   1  1  1  1  1  1  1  0 
   6   1  1  1  1  1  1  1  0 
   5   1  1  1  1  1  1  1  0 
   4   1  1  1  1  1  1  1  0 
   3   1  1  1  1  1  1  1  0 
   2   1  1  1  1  1  1  1  0 
   1   1  1  1  1  1  1  1  0 

     a  b  c  d  e  f  g  h  
     
    NOT AB FILE 
      8   0  0  1  1  1  1  1  1 
      7   0  0  1  1  1  1  1  1 
      6   0  0  1  1  1  1  1  1 
      5   0  0  1  1  1  1  1  1 
      4   0  0  1  1  1  1  1  1 
      3   0  0  1  1  1  1  1  1 
      2   0  0  1  1  1  1  1  1 
      1   0  0  1  1  1  1  1  1 

     a  b  c  d  e  f  g  h 
     
    
     NOT GH FILE
     
      8   1  1  1  1  1  1  0  0 
      7   1  1  1  1  1  1  0  0 
      6   1  1  1  1  1  1  0  0 
      5   1  1  1  1  1  1  0  0 
      4   1  1  1  1  1  1  0  0 
      3   1  1  1  1  1  1  0  0 
      2   1  1  1  1  1  1  0  0 
      1   1  1  1  1  1  1  0  0 

     a  b  c  d  e  f  g  h  */

const U64 not_A_file =  18374403900871474942ULL;
const U64 not_H_file =  9187201950435737471ULL;
const U64 not_AB_file =  18229723555195321596ULL;
const U64 not_GH_file =  4557430888798830399ULL;
//      a  b  c  d  e  f  g  h  

// Relevant occupancy bit count for every square on board for bishop // How many squares is a piece attacking if it's on a particular square
const int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6,
};

// Relevant occupancy bit count for every square on board for rook
const int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12, 
};

// rook magic number
U64 rook_magic_number[64] = {
    0x8a80104000800020Ull,
    0x140002000100040Ull,
    0x2801880a0017001Ull,
    0x100081001000420Ull,
    0x200020010080420Ull,
    0x3001c0002010008Ull,
    0x8480008002000100Ull,
    0x2080088004402900Ull,
    0x800098204000Ull,
    0x2024401000200040Ull,
    0x100802000801000Ull,
    0x120800800801000Ull,
    0x208808088000400Ull,
    0x2802200800400Ull,
    0x2200800100020080Ull,
    0x801000060821100Ull,
    0x80044006422000Ull,
    0x100808020004000Ull,
    0x12108a0010204200Ull,
    0x140848010000802Ull,
    0x481828014002800Ull,
    0x8094004002004100Ull,
    0x4010040010010802Ull,
    0x20008806104Ull,
    0x100400080208000Ull,
    0x2040002120081000Ull,
    0x21200680100081Ull,
    0x20100080080080Ull,
    0x2000a00200410Ull,
    0x20080800400Ull,
    0x80088400100102Ull,
    0x80004600042881Ull,
    0x4040008040800020Ull,
    0x440003000200801Ull,
    0x4200011004500Ull,
    0x188020010100100Ull,
    0x14800401802800Ull,
    0x2080040080800200Ull,
    0x124080204001001Ull,
    0x200046502000484Ull,
    0x480400080088020Ull,
    0x1000422010034000Ull,
    0x30200100110040Ull,
    0x100021010009Ull,
    0x2002080100110004Ull,
    0x202008004008002Ull,
    0x20020004010100Ull,
    0x2048440040820001Ull,
    0x101002200408200Ull,
    0x40802000401080Ull,
    0x4008142004410100Ull,
    0x2060820c0120200Ull,
    0x1001004080100Ull,
    0x20c020080040080Ull,
    0x2935610830022400Ull,
    0x44440041009200Ull,
    0x280001040802101Ull,
    0x2100190040002085Ull,
    0x80c0084100102001Ull,
    0x4024081001000421Ull,
    0x20030a0244872Ull,
    0x12001008414402Ull,
    0x2006104900a0804Ull,
    0x1004081002402Ull,
};

// bishop magic number
U64 bishop_magic_number[64] = {
    0x40040844404084Ull,
    0x2004208a004208Ull,
    0x10190041080202Ull,
    0x108060845042010Ull,
    0x581104180800210Ull,
    0x2112080446200010Ull,
    0x1080820820060210Ull,
    0x3c0808410220200Ull,
    0x4050404440404Ull,
    0x21001420088Ull,
    0x24d0080801082102Ull,
    0x1020a0a020400Ull,
    0x40308200402Ull,
    0x4011002100800Ull,
    0x401484104104005Ull,
    0x801010402020200Ull,
    0x400210c3880100Ull,
    0x404022024108200Ull,
    0x810018200204102Ull,
    0x4002801a02003Ull,
    0x85040820080400Ull,
    0x810102c808880400Ull,
    0xe900410884800Ull,
    0x8002020480840102Ull,
    0x220200865090201Ull,
    0x2010100a02021202Ull,
    0x152048408022401Ull,
    0x20080002081110Ull,
    0x4001001021004000Ull,
    0x800040400a011002Ull,
    0xe4004081011002Ull,
    0x1c004001012080Ull,
    0x8004200962a00220Ull,
    0x8422100208500202Ull,
    0x2000402200300c08Ull,
    0x8646020080080080Ull,
    0x80020a0200100808Ull,
    0x2010004880111000Ull,
    0x623000a080011400Ull,
    0x42008c0340209202Ull,
    0x209188240001000Ull,
    0x400408a884001800Ull,
    0x110400a6080400Ull,
    0x1840060a44020800Ull,
    0x90080104000041Ull,
    0x201011000808101Ull,
    0x1a2208080504f080Ull,
    0x8012020600211212Ull,
    0x500861011240000Ull,
    0x180806108200800Ull,
    0x4000020e01040044Ull,
    0x300000261044000aUll,
    0x802241102020002Ull,
    0x20906061210001Ull,
    0x5a84841004010310Ull,
    0x4010801011c04Ull,
    0xa010109502200Ull,
    0x4a02012000Ull,
    0x500201010098b028Ull,
    0x8040002811040900Ull,
    0x28000010020204Ull,
    0x6000020202d0240Ull,
    0x8918844842082200Ull,
    0x4010011029020020Ull,
};


//pawn attack table [ side to move ] [pawn available on current given, square]
U64 pawn_attacks[2][64];

// KNIGHT ATTACK    17,15,10,6 
U64 knight_attacks[64];

// KING ATTACK   
U64 king_attacks[64];

// BISHOP ATTACK  mask  
U64 bishop_masks[64];

// ROOK ATTACK  mask
U64 rook_masks[64];

// BISHOP ATTACK  table [square] [occupancy] // occupancy : 2^(highest relevant bit)
U64 bishop_attacks[64][512];

// ROOK ATTACK  table [square] [occupancy] // occupancy : 2^(highest relevant bit)
U64 rook_attacks[64][4096];

// QUEEN ATTACK   
U64 queen_attacks[64];


//GENERATE ATTACKS ********************************************************************************************************************
//genrate pawn attack
U64 mask_pawn_attacks(int side, int square){

    // result attack bitboar
    U64 attack = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;
    
    //set piece on board
    set_bit(bitboard,square);

    //white
    if (!side){
        // white
        if( bitboard & not_H_file ) attack |= (bitboard >> 7);
        if( bitboard & not_A_file ) attack |= (bitboard >> 9);
    }else{
        if( bitboard & not_H_file ) attack |= (bitboard << 9);
        if( bitboard & not_A_file ) attack |= (bitboard << 7);

    }
    
    //return attack
    return attack;

}

// Generate knight attack

U64 mask_knight_attacks(int square){
    // result attack bitboar
    U64 attack = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;
    
    //set piece on board
    set_bit(bitboard,square);
    
    if( bitboard & not_A_file ) attack |= (bitboard >> 17); //right
    if( bitboard & not_AB_file ) attack |= (bitboard >> 10);
    if( bitboard & not_GH_file ) attack |= (bitboard >> 6);
    if( bitboard & not_H_file ) attack |= (bitboard >> 15);


    if( bitboard & not_H_file ) attack |= (bitboard << 17); //left d
    if( bitboard & not_GH_file ) attack |= (bitboard << 10);
    if( bitboard & not_AB_file ) attack |= (bitboard << 6);
    if( bitboard & not_A_file ) attack |= (bitboard << 15);

    
    //return attack
    return attack;
}

// Generate king attack

U64 mask_king_attacks(int square){
    // result attack bitboar
    U64 attack = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;
    
    //set piece on board
    set_bit(bitboard,square);
    
    if( bitboard & not_H_file ) attack |= (bitboard >> 7); //  not h
    attack |= (bitboard >> 8);
    if( bitboard & not_A_file ) attack |= (bitboard >> 9); // not a
    if( bitboard & not_A_file ) attack |= (bitboard >> 1); // not a


    if( bitboard & not_A_file ) attack |= (bitboard << 7); // not a 
    attack |= (bitboard << 8);
    if( bitboard & not_H_file ) attack |= (bitboard << 9); // not  h
    if( bitboard & not_H_file ) attack |= (bitboard << 1);  // not  h

    
    //return attack
    return attack;
}

// mask bishop

U64 mask_bishop_attacks(int square){
    // result attack bitboar
    U64 attack = 0ULL;

    // init  rank, file
    int r,f;

    //init target rank,file
    int tr = square / 8;
    int tf = square % 8;

    //mask relevent bishop squares
    for(r = tr + 1, f = tf + 1 ; r <= 6 && f <= 6 ; r++, f++ ) attack |= ( 1ULL <<  (r * 8 + f));
    for(r = tr - 1, f = tf - 1 ; r >= 1 && f >= 1 ; r--, f-- ) attack |= ( 1ULL <<  (r * 8 + f));
    for(r = tr + 1, f = tf - 1 ; r <= 6 && f >= 1 ; r++, f-- ) attack |= ( 1ULL <<  (r * 8 + f));
    for(r = tr - 1, f = tf + 1 ; r >= 1 && f <= 6 ; r--, f++ ) attack |= ( 1ULL <<  (r * 8 + f));


    //return attack
    return attack;
}

// mask rook

U64 mask_rook_attacks(int square){
    // result attack bitboar
    U64 attack = 0ULL;

    // init  rank, file
    int r,f;

    //init target rank,file
    int tr = square / 8;
    int tf = square % 8;

    //mask relevent bishop squares
    for(r = tr + 1 ; r <= 6 ; r++) attack |= ( 1ULL <<  (r * 8 + tf));
    for(r = tr - 1 ; r >= 1 ; r--) attack |= ( 1ULL <<  (r * 8 + tf));
    for(f = tf + 1 ; f <= 6 ; f++) attack |= ( 1ULL <<  (tr * 8 + f));
    for(f = tf - 1 ; f >= 1 ; f--) attack |= ( 1ULL <<  (tr * 8 + f));


    //return attack
    return attack;
}




// Generate Bishop attacks on the fly ***************************************************************************************************** 

U64 bishop_attacks_on_the_fly(int square,  U64 block){ // block are the pieces blocking it
    // result attack bitboar
    U64 attack = 0ULL;

    // init  rank, file
    int r,f;

    //init target rank,file
    int tr = square / 8;
    int tf = square % 8;

    //generate bishop attacks
    for(r = tr + 1, f = tf + 1 ; r <= 7 && f <= 7 ; r++, f++ ){
        attack |= ( 1ULL <<  (r * 8 + f));
        if((1ULL << (r * 8 + f)) & block) break;
        
    }
    for(r = tr - 1, f = tf - 1 ; r >= 0 && f >= 0 ; r--, f-- ) {
        attack |= ( 1ULL <<  (r * 8 + f));
        if((1ULL << (r * 8 + f)) & block) break;

    }
    for(r = tr + 1, f = tf - 1 ; r <= 7 && f >= 0 ; r++, f-- ) {
        attack |= ( 1ULL <<  (r * 8 + f));
        if((1ULL << (r * 8 + f)) & block) break;
        
    }
    for(r = tr - 1, f = tf + 1 ; r >= 0 && f <= 7 ; r--, f++ ) {
        attack |= ( 1ULL <<  (r * 8 + f));
        if((1ULL << (r * 8 + f)) & block) break;
    
    }


    //return attack
    return attack;
}


// Generate Bishop attacks on the fly

U64 rook_attacks_on_the_fly(int square,  U64 block){
    // result attack bitboar
    U64 attack = 0ULL;

    // init  rank, file
    int r,f;

    //init target rank,file
    int tr = square / 8;
    int tf = square % 8;

    //generate bishop attacks
    for(r = tr + 1 ; r <= 7; r++){
        attack |= ( 1ULL <<  (r * 8 + tf));
        if((1ULL << (r * 8 + tf)) & block) break;
        
    }
    for(r = tr - 1 ; r >= 0 ; r--) {
        attack |= ( 1ULL <<  (r * 8 + tf));
        if((1ULL << (r * 8 + tf)) & block) break;

    }
    for(f = tf - 1 ; f >=0  ; f--) {
        attack |= ( 1ULL <<  (tr * 8 + f));
        if((1ULL << (tr * 8 + f)) & block) break;
        
    }
    for(f = tf + 1 ; f <= 7 ; f++) {
        attack |= ( 1ULL <<  (tr * 8 + f));
        if((1ULL << (tr * 8 + f)) & block) break;
    
    }


    //return attack
    return attack;
}


// initialize leaper pieces attack ************************************************************************************

void init_leapers_attacks(){
    for (int i = 0; i < 64; i++) // i = square
    {
        //init pawn attacks
        pawn_attacks[white][i] = mask_pawn_attacks(white,i);
        pawn_attacks[black][i] = mask_pawn_attacks(black,i);


        //init knight attacks
        knight_attacks[i] = mask_knight_attacks(i);


        //init king attacks
        king_attacks[i] = mask_king_attacks(i);


        // //init bishop attacks
        // bishop_attacks[i] = mask_bishop_attacks(i);


        // //init rook attacks
        // rook_attacks[i] = mask_rook_attacks(i);
    }


}
    
//  set  occupancy // It gives all the possible occupancies on attack mask 
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask){ // represent the number "index" in bit wise( 3 = 11) among the bits in attack mask
 
    // init occupany map
    U64 occupancy = 0ULL;

    // Over the range of beats within attack mask
    for (int count = 0; count < bits_in_mask; count++){

        //get LS1B  index of attack mask
        int square =  get_ls1b_index(attack_mask);

        //pop LS1B  index of attack mask
        pop_bit(attack_mask,square);

        //set occupancy
        if (index & (1 <<  count)){
           set_bit(occupancy,square);
        }
    }
    

    //return occupancy 
    return occupancy;
}




// MAGIC ************************************************************************************************************************

// find appropriate magic number
U64 find_magic_number(int square, int relevant_bits, int bishop){ //relevant bits: number of sqaure, a piece can see when it is on "square"

    //init occupancy
    U64 occupancies[4096];  

    // init attack tables
    U64 attacks[4096];

    // init used attack tables
    U64 used_attacks[4096];

    // init attack mask for current piece
    U64 attack_mask = bishop ?  mask_bishop_attacks(square) : mask_rook_attacks(square); // moves piece can make from "sqaure"

    // init occupancy indicies : 2^relevant_bits
    int occupancy_indicies  = 1 << relevant_bits; 

    //loop over occupany indicies
    for (int index = 0; index < occupancy_indicies; index++)
    {
        //init occupancies : Store all variation of occupancy in "occupancies"
        occupancies[index]  = set_occupancy(index, relevant_bits, attack_mask);

        //init attacks : Store all variation of attacks in "attacks"
        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupancies[index]) : rook_attacks_on_the_fly(square, occupancies[index]);

    }
    
    //  test magic number loop

    for (int random_count = 0; random_count < 99999999; random_count++)
    {
        //genrate magic num 
        U64 magic_number = generate_magic_number();

        //skip useless/inappropriate number : less bits
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6){
            continue;
        } 


        //init used attack arrays
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        //  init index and fail flag
        int index, fail;
        //test magic index

        for(index = 0,  fail  = 0; !fail && index < occupancy_indicies; index++){
            //init magic index
            printBitBoard((occupancies[index] * magic_number) >> (64 - relevant_bits));;
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits)); 
            // if magic index works
            if (used_attacks[magic_index] == 0ULL){
                used_attacks[magic_index] = attacks[index];
            }
            else if (used_attacks[magic_index] != attacks[index]){ // magic index does not work
                fail = 1;

            }
            

        }

        if(!fail) return magic_number;
        

    }
    
    // if magic works
    printf("Magic number  fails");
    return 0ULL;

}

// inint magic numbers
void init_magic_numbers(){
    //loop over 64 square
    for (int square = 0; square < 64; square++)
    {
       //init rook magic number
       rook_magic_number[square] = find_magic_number(square, rook_relevant_bits[square], rook);
    }
    // printf("\n\n");
    for (int square = 0; square < 64; square++)
    {
       //init rook magic number
       bishop_magic_number[square] = find_magic_number(square, bishop_relevant_bits[square], bishop);
    }
    
}

// init slider pieces attack table:
void init_sliders_attacks(int bishop){

    // loop over 64 square
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook mask
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        // init current mask
        U64 attack_mask =  bishop ? bishop_masks[square] : rook_masks[square];

        //init relvant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);

        // init occupancy indicies : 2^relevant_bits
        int occupancy_indicies = 1 << relevant_bits_count; 

        // loop over occunacy indicies
        for (int index = 0; index < occupancy_indicies; index++){
            //  bishop
            if(bishop){
                // init occupancy
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                //init magic index
                int magic_index = occupancy * bishop_magic_number[square] >> (64 - bishop_relevant_bits[square]);

                //init bishop attacks
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);

            } // rook
            else{
                // init occupancy
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                //init magic index
                int magic_index = occupancy * rook_magic_number[square] >> (64 - rook_relevant_bits[square]); // we do right shift so that number is less the 2^(relevant bits)

                //init bishop attacks
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);

            }
        }   
    }
}
 

// I did all this magic number and calculation prior because on time to find a move I  don't have to do this every time

// get bishop attacks
static inline U64 get_bishop_attacks(int square, U64 occupancy){

    // make occupancy into magic index 
    occupancy &=  bishop_masks[square];
    occupancy *= bishop_magic_number[square];
    occupancy  >>= (64 - bishop_relevant_bits[square]);

    return bishop_attacks[square][occupancy];

}
 

// get rook attacks
static inline U64 get_rook_attacks(int square, U64 occupancy){

    //
    occupancy &=  rook_masks[square];
    occupancy *= rook_magic_number[square];
    occupancy  >>= (64 - rook_relevant_bits[square]);

    return rook_attacks[square][occupancy];
}

// get Queen attacks
static inline U64 get_queen_attacks(int square, U64 occupancy){


    U64 quuen_attacks = 0ULL;

    // init bishop occupancy
    U64 bishop_occupancy =  occupancy;

    // init rook occupancy
    U64 rook_occupancy =  occupancy;

    // bishop
    bishop_occupancy &=  bishop_masks[square];
    bishop_occupancy *= bishop_magic_number[square];
    bishop_occupancy  >>= (64 - bishop_relevant_bits[square]);
    
    quuen_attacks = bishop_attacks[square][bishop_occupancy];

    // rook
    rook_occupancy &=  rook_masks[square];
    rook_occupancy *= rook_magic_number[square];
    rook_occupancy  >>= (64 - rook_relevant_bits[square]);

    quuen_attacks |= rook_attacks[square][rook_occupancy];

    return quuen_attacks;
}


// MOVE GENEARTER ********************************************************************************************************************

// is the given [squared] attack by the [side]
static inline int is_sqaured_attacked(int square, int side){
    
    // attacked by pawns  - white // if white pawn is present on any Square which is attacked by the black pawn which is present on the given "Square". That means white. Can attack the given "Square"
    if((side == white) && ( pawn_attacks[black][square] & bitboards[P])) return 1;

    // attacked by pawns  - white 
    if((side == black) && ( pawn_attacks[white][square] & bitboards[p])) return 1;

    // attacked by knights
    if(knight_attacks[square]  & ((side == white) ? bitboards[N] : bitboards[n])) return 1;

    // attacked by bishops  // A square is attacked by the "Bishop", if there exist an imaginary bishop on that square that can attack the "Bishop"
    if(get_bishop_attacks(square, occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])) return 1;

    // attacked by rooks 
    if(get_rook_attacks(square, occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])) return 1;

    // attacked by queen 
    if(get_queen_attacks(square, occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])) return 1;

    // attacked by kings
    if(king_attacks[square]  & ((side == white) ? bitboards[K] : bitboards[k])) return 1;

    // by default return false
    return 0;
}

// print attacked squares // which side to give the attack squad for
void print_attacked_squares(int side){

    printf("\n");

    for (int rank = 0; rank < 8; rank++){

        printf("  %d ", 8 - rank);

        for (int file = 0; file < 8; file++){

            int square = rank * 8 + file;

            // check if sqaure is attacked or not
            printf(" %d ",is_sqaured_attacked(square,side) ? 1 : 0);

        }
        printf("\n");
    }
    printf("\n     a  b  c  d  e  f  g  h  \n\n");

}

// **********************************************

/*
          binary move bits                               hexidecimal constants
    
    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000

*/

// encode move
#define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
 (source) |              \
 ( target << 6 ) |       \
 ( piece  << 12 )  |     \
 ( promoted << 16 ) |    \
 ( capture << 20 ) |     \
 ( double << 21 ) |      \
 ( enpassant << 22 ) |   \
 ( castling << 23 )      \


// extract source square
#define get_move_source(move) (move  & 0x3f)

// extract target square
#define get_move_target(move) ((move  & 0xfc0) >> 6)

// extract piece
#define get_move_piece(move) ((move  & 0xf000) >> 12)

// extract promoted
#define get_move_promoted(move) ((move  & 0xf0000) >> 16)

// extract capture flag
#define get_move_capture(move) (move  & 0x100000)

// extract double flag
#define get_move_double(move) (move  & 0x200000)

// extract enpassant flag
#define get_move_enpassant(move) (move  & 0x400000)

// extract castling flag
#define get_move_castling(move) (move  & 0x800000)



//  move list structure
typedef struct {
    
    // moves
    int moves[256];

    // move count
    int count;

} moves;

// add move
static inline void add_move(moves *move_list, int move){
    
    move_list->moves[move_list->count] = move;
    move_list->count++;

}


// print move helper fuction
void print_move(int move){
    printf("%s%s%c\n", square_to_coordinates[get_move_source(move)], square_to_coordinates[get_move_target(move)], promoted_pieces[get_move_promoted(move)] );
}


// print move list
void print_move_list(moves *move_list){


    // skip on empty list
    if(!move_list->count){
        printf("\nNo Moves in the move_list\n");
        return;
    }
    printf("\n      move  piece, capture, double, enpassant, castling\n\n");

    // loop over move list
    for (int move_count = 0; move_count < move_list->count ; move_count++)
    {
        int move = move_list->moves[move_count];
                
        #ifdef WIN64
            // print move
            printf("    %s%s%c   %c       %d         %d         %d         %d\n", square_to_coordinates[get_move_source(move)],
                                                                                  square_to_coordinates[get_move_target(move)],
                                                                                  get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
                                                                                  ascii_pieces[get_move_piece(move)],
                                                                                  get_move_capture(move) ? 1 : 0,
                                                                                  get_move_double(move) ? 1 : 0,
                                                                                  get_move_enpassant(move) ? 1 : 0,
                                                                                  get_move_castling(move) ? 1 : 0);
        #else
            // print move
            printf("      %s%s%c   %s       %d         %d         %d         %d\n", square_to_coordinates[get_move_source(move)],
                                                                                  square_to_coordinates[get_move_target(move)],
                                                                                  get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
                                                                                  unicode_pieces[get_move_piece(move)],
                                                                                  get_move_capture(move) ? 1 : 0,
                                                                                  get_move_double(move) ? 1 : 0,
                                                                                  get_move_enpassant(move) ? 1 : 0,
                                                                                  get_move_castling(move) ? 1 : 0 );
        #endif
        
        // print total number of moves
    }
    
    printf("\n\n    Total number of moves: %d\n\n", move_list->count);
    
}

// generate all moves
static inline void generate_moves(moves  *move_list){
    

    // init move count
    move_list->count = 0;

    // init soure and target : sourse is very peace is standing, target is very peace will be moving
    int  source_square, target_square;

    // define current pieces bitboard copy & it's attacks
    U64 bitboard, attacks;

    //  loop over all the bitboards
    for(int piece = P; piece <=  k; piece++){
        
        // init piece bitboard copy
        bitboard = bitboards[piece];



        // generate white pawns and white king castling moves
        if(side == white){

            // pick up white pawn bitboards index
            if (piece  == P){

                //loop over white pawns
                while(bitboard){

                    // init source square
                    source_square = get_ls1b_index(bitboard);
                    // printf(" white pawn: %s  \n", square_to_coordinates[source_square]);

                    // init target square 
                    target_square = source_square - 8;
                    
                    // generate quite pawn moves // Run If statement when target Square is less than a8 and there is no bit on the target Square
                    if(!(target_square  < a8) && !get_bit(occupancies[both], target_square)){
                        
                        // promotion
                        if (source_square >= a7 && source_square <= h7)
                        {
                            add_move(move_list,encode_move(source_square, target_square, piece, Q, 0, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, R, 0, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, B, 0, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, N, 0, 0, 0, 0));
                        }else{
                            // one square ahead]
                            add_move(move_list,encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));

                            // two square ahead
                            if ((source_square >= a2 && source_square <= h2) && !get_bit(occupancies[both], target_square - 8))
                            {
                                add_move(move_list,encode_move(source_square, (target_square - 8), piece, 0, 0, 1, 0, 0));
                            }
                            
                        }
                        
                    }

                    // init pawn attack bitboards ===================
                    attacks = pawn_attacks[side][source_square] & occupancies[black];

                    // generate pawn capture 
                    while (attacks)
                    {
                        //  init target
                        target_square = get_ls1b_index(attacks);

                        // capture promotion
                        if (source_square >= a7 && source_square <= h7)
                        {
                            add_move(move_list,encode_move(source_square, target_square, piece, Q, 1, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, R, 1, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, B, 1, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, N, 1, 0, 0, 0));

                        }else{
                            // normal capture
                            add_move(move_list,encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                        }


                        pop_bit(attacks, target_square);
                    }
                    // generate enpassant capture
                    if(enpassant != no_sqr){
                        

                        U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);

                        // make sure it is available
                        if (enpassant_attacks)
                        {
                            int target_enpassant = get_ls1b_index(enpassant_attacks);
                            add_move(move_list,encode_move(source_square, target_enpassant, piece, 0, 1, 0, 1, 0));
                        }
                        
                    }

                    // pop ls1b from bitboard
                    pop_bit(bitboard, source_square);
                }
                
            }

            // castling moves
            if (piece == K)
            {
                // king side castling is available
                if (castle & wk)
                {
                    // make sure squares between king and h1 rook  are empty
                    if (!get_bit(occupancies[both],f1) && !get_bit(occupancies[both],g1))
                    {
                        // make sure king and f1 square are not under attack
                        if (!is_sqaured_attacked(e1,black) && !is_sqaured_attacked(f1,black )) add_move(move_list,encode_move(e1, g1, piece, 0, 0, 0, 0, 1));

                        
                        
                    }
                    
                }
                
                // queen side castling is available
                if (castle & wq)
                {
                    // make sure squares between king and h1 rook  are empty
                    if (!get_bit(occupancies[both],b1) && !get_bit(occupancies[both],c1) && !get_bit(occupancies[both],d1))
                    {
                        // make sure king, c1 and d1 square are not under attack
                        if (!is_sqaured_attacked(e1,black) && !is_sqaured_attacked(d1,black )) add_move(move_list,encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
     
                        
                    }
                }
                

            }
            
            

        }else{ //  generate black pawn moves and black king castling moves 
        
            if (piece  == p){

                //loop over black pawns
                while(bitboard){

                    // init source square
                    source_square = get_ls1b_index(bitboard);
                    // printf(" white pawn: %s  \n", square_to_coordinates[source_square]);

                    // init target square 
                    target_square = source_square + 8;
                    
                    // generate quite pawn moves // Run If statement when target Square is more than h1 and there is no bit on the target Square
                    if(!(target_square  > h1) && !get_bit(occupancies[both], target_square)){
                        
                        // promotion
                        if (source_square >= a2 && source_square <= h2)
                        {
                             add_move(move_list,encode_move(source_square, target_square, piece, q, 0, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, r, 0, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, b, 0, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, n, 0, 0, 0, 0));
                        }else{

                            // one square ahead
                            add_move(move_list,encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));

                            // two square ahead
                            if ((source_square >= a7 && source_square <= h7) && !get_bit(occupancies[both], target_square + 8))
                            {
                                // target_square += 8;
                                add_move(move_list,encode_move(source_square, (target_square + 8), piece, 0, 0, 1, 0, 0));
;
                            }
                            
                        }
                        
                    }

                    // init pawn attack bitboards ===================
                    attacks = pawn_attacks[side][source_square] & occupancies[white];

                    // generate pawn capture 
                    while (attacks)
                    {
                        //  init target
                        target_square = get_ls1b_index(attacks);

                        // capture promotion
                        if (source_square >= a2 && source_square <= h2)
                        {
                            // promotion  capture
                            add_move(move_list,encode_move(source_square, target_square, piece, q, 1, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, r, 1, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, b, 1, 0, 0, 0));
                            add_move(move_list,encode_move(source_square, target_square, piece, n, 1, 0, 0, 0));
                        }else{
                            // normal capture
                            add_move(move_list,encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                        }


                        pop_bit(attacks, target_square);
                    }
                    // generate enpassant capture
                    if(enpassant != no_sqr){
                        

                        U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);

                        // make sure it is available
                        if (enpassant_attacks)
                        {
                            int target_enpassant = get_ls1b_index(enpassant_attacks);
                            add_move(move_list,encode_move(source_square, target_enpassant, piece, 0, 0, 0, 1, 0));
                        }
                        

                    }
                    // pop ls1b from bitboard
                    pop_bit(bitboard, source_square);
                }
            }

            // castling moves
            if (piece == k)
            {
                // king side castling is available
                if (castle & bk)
                {
                    // make sure squares between king and h1 rook  are empty
                    if (!get_bit(occupancies[both],f8) && !get_bit(occupancies[both],g8))
                    {
                        // make sure king and f8 square are not under attack
                        if (!is_sqaured_attacked(e8,white) && !is_sqaured_attacked(f8,white )) 
                        {
                            add_move(move_list,encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
                        }

                    }
                    
                }
                
                // queen side castling is available
                if (castle & bq)
                {
                    // make sure squares between king and h8 rook  are empty
                    if (!get_bit(occupancies[both],b8) && !get_bit(occupancies[both],c8) && !get_bit(occupancies[both],d8))
                    {
                        // make sure king, c8 and d8 square are not under attack
                        if (!is_sqaured_attacked(e8,white) && !is_sqaured_attacked(d8,white ))
                        {
                            add_move(move_list,encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
                        }
                
                    }
                }
            }
        }

        // generate knight moves
        if((side == white) ? piece == N  : piece == n){
            // Code to be executed if the condition is true
            // This block will be executed if side is white and piece is N,
            // or if side is not white and piece is n.
            while (bitboard)
            {
                /* init source square */
                source_square = get_ls1b_index(bitboard);

                // piece attacks
                attacks = knight_attacks[source_square] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target square
                while(attacks){

                    target_square  = get_ls1b_index(attacks);

                    //quite 
                    if(!get_bit((side == white) ? occupancies[black] : occupancies[white], target_square)){
                        
                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                        
                    }else{ // capture

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));

                    }

                    // capture 

                    pop_bit(attacks, target_square);
                }

                // pop 
                pop_bit(bitboard, source_square);
            }
            

             
        }


        // generate bishop moves
        if((side == white) ? piece == B  : piece == b){
            // Code to be executed if the condition is true
            // This block will be executed if side is white and piece is N,
            // or if side is not white and piece is n.
            while (bitboard)
            {
                /* init source square */
                source_square = get_ls1b_index(bitboard);

                // piece attacks
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target square
                while(attacks){

                    target_square  = get_ls1b_index(attacks);

                    //quite 
                    if(!get_bit((side == white) ? occupancies[black] : occupancies[white], target_square)){

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));


                    }else{ // capture

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));

                    }

                    // capture 

                    pop_bit(attacks, target_square);
                }

                // pop 
                pop_bit(bitboard, source_square);
            }
            

             
        }

        // generate rook moves
        if((side == white) ? piece == R  : piece == r){
            // Code to be executed if the condition is true
            // This block will be executed if side is white and piece is N,
            // or if side is not white and piece is n.
            while (bitboard)
            {
                /* init source square */
                source_square = get_ls1b_index(bitboard);

                // piece attacks
                attacks = get_rook_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target square
                while(attacks){

                    target_square  = get_ls1b_index(attacks);

                    //quite 
                    if(!get_bit((side == white) ? occupancies[black] : occupancies[white], target_square)){

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));

                    }else{

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }

                    // capture 

                    pop_bit(attacks, target_square);
                }

                // pop 
                pop_bit(bitboard, source_square);
            }
            

             
        }


        // generate queen moves
        if((side == white) ? piece == Q  : piece == q){
            // Code to be executed if the condition is true
            // This block will be executed if side is white and piece is N,
            // or if side is not white and piece is n.
            while (bitboard)
            {
                /* init source square */
                source_square = get_ls1b_index(bitboard);

                // piece attacks
                attacks = get_queen_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target square
                while(attacks){

                    target_square  = get_ls1b_index(attacks);

                    //quite 
                    if(!get_bit((side == white) ? occupancies[black] : occupancies[white], target_square)){

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));

                    }else{

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));

                    }

                    // capture 

                    pop_bit(attacks, target_square);
                }

                // pop 
                pop_bit(bitboard, source_square);
            }
            

             
        }

        // generate king moves
        if((side == white) ? piece == K  : piece == k){
            // Code to be executed if the condition is true
            // This block will be executed if side is white and piece is N,
            // or if side is not white and piece is n.
            while (bitboard)
            {
                /* init source square */
                source_square = get_ls1b_index(bitboard);

                // piece attacks
                attacks = king_attacks[source_square] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target square
                while(attacks){

                    target_square  = get_ls1b_index(attacks);

                    //quite 
                    if(!get_bit((side == white) ? occupancies[black] : occupancies[white], target_square)){

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));

                    }else{

                        add_move(move_list,encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));

                    }

                    // capture 

                    pop_bit(attacks, target_square);
                }

                // pop 
                pop_bit(bitboard, source_square);
            }
                
        }

    }
}





// INIT ALL *****************************************************************************************************************
void init_all(){

    init_leapers_attacks();

    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);           

    // init_magic_numbers();
}


// MAIN ********************************************************************************************************************
int main(){
    

    init_all();

    // parse_fen("8/8/8/3B4/8/8/8/8/ b - - ");
    parse_fen(tricky_position);
    print_board();
    
    moves move_list[2];

    generate_moves(move_list);

    print_move_list(move_list);

    

    return 0;
}
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "anqi.hh"
#include "Protocol.h"
#include "ClientSocket.h"


using namespace std;

#ifdef _WINDOWS
#include<windows.h>
#else
#include<ctime>
#endif

const int DEFAULTTIME = 15;
typedef  int SCORE;

static const SCORE INF=1000001;
static const SCORE WIN=1000000; 

int evalBoard(const BOARD& B);
SCORE NegaScout(const BOARD&, SCORE, SCORE, int, int);

unsigned int zob[15][32];
unsigned int zob_player;
static HASH_ENTRY* hash_table;
static unsigned int hash_max_index;
void initZobrist();
void printZob();

#ifdef _WINDOWS
DWORD Tick;     // 開始時刻
int   TimeOut;  // 時限
#else
clock_t Tick;     // 開始時刻
clock_t TimeOut;  // 時限
#endif
MOV   BestMove; // 搜出來的最佳著法

bool TimesUp() {
#ifdef _WINDOWS
	return GetTickCount()-Tick>=TimeOut;
#else
	return clock() - Tick > TimeOut;
#endif
}

SCORE NegaScout(const BOARD &B, SCORE alpha, SCORE beta, int depth, int cut){
	// check hash table
	//printf("zob_key = %u, index = %u\n", B.zob_key, hash_max_index);
	unsigned int hash_key = B.zob_key % hash_max_index;
	//printf("hash_key = %u\n", hash_key);
	HASH_ENTRY* entry = hash_table + hash_key;
	bool hash_hit = false;


	SCORE m = -INF; // the current lower bound; fail soft
	SCORE n = beta; // the current upper bound

	// check hash table
	if(entry->used == true){
		if(entry->zob_key == B.zob_key){
			if(entry->exact){
				if(entry->player != B.who && 
					entry->best_move.st != entry->best_move.ed){
					BestMove = entry->best_move;
					//printf("hash return move (%d, %d)\n", BestMove.st, BestMove.ed);
					return entry->value;
				}
			}else{
				hash_hit = true;
				m = entry->value;
			}
		}else if(depth < entry->depth){
			hash_hit =true;
			entry->zob_key = B.zob_key;
			entry->depth = depth;
			entry->player = B.who;
		}
	}else{
		hash_hit = true;
		entry->used = true;
		entry->zob_key = B.zob_key;
		entry->depth = depth;
		entry->player = B.who;
	}

	if(B.ChkLose()) return -WIN; // heuristic condition
							     // return if lose

	MOVLST lst;
	// 終止條件 (1)到指定層數 (2)時間到 (3)沒有可走步
	if(cut==0||TimesUp()||B.MoveGen(lst)==0){
		int value = +evalBoard(B);
		if(depth%2 == 0)
			return +value;
		else
			return -value;
	}

	for(int i=0; i<lst.num; i++){
		BOARD N(B);
		N.Move(lst.mov[i]);
		
		const SCORE t = -NegaScout(N, -n, -max(alpha, m), depth+1, cut-1);
		if(t > m)
			if(n == beta || cut <3 || t >= beta)
				m = t;
			else
				m = -NegaScout(N, -beta, -t, depth+1, cut-1); // re-search
			if(depth==0){ // change the best move
				BestMove = lst.mov[i];
				printf("");
			}
		if(m >= beta){ // cut off
			if(hash_hit){
				entry->value = m;
				entry->best_move = lst.mov[i];
			}
			return m;
		}
		n = max(alpha, m) + 1; // setup a null window
	}
	if(hash_hit){
		entry->value = m;
		entry->exact = true;
	}

	return m;
}

MOV Play(const BOARD &B) {
#ifdef _WINDOWS
	Tick=GetTickCount();
	TimeOut = (DEFAULTTIME-3)*1000;
#else
	Tick=clock();
	TimeOut = (DEFAULTTIME-3)*CLOCKS_PER_SEC;
#endif
	POS p; int c=0;

	// new game flip random place
	if(B.who==-1){
		p=rand()%32;
		return MOV(p,p);
	}

	// 若搜出來的結果會比較好，就用搜出來的走法
	int scout = NegaScout(B, -INF, INF, 0, 20);
	int rightnow = evalBoard(B);
	//printf("scout = %d, rightnow = %d\n", scout, rightnow);
	//if(NegaScout(B, -INF, INF, 0, 20) > B.evalBoard()){
	if(scout > rightnow){
		//printf("bsm.st = %d, bsm.ed = %d\n", BestMove.st, BestMove.ed);
		return BestMove;
	}

	// 否則隨便翻一個地方，但小心可能已經沒地方翻了
	if(B.dark == 0) // 沒暗子了
		return BestMove;
	c=rand()%B.dark;
	for(p=0; p<32; p++)
		if(B.fin[p]==FIN_X && --c<0)
			break;

	return MOV(p,p);
}

int main(int argc, char* argv[]) {

#ifdef _WINDOWS
	srand(Tick=GetTickCount());
#else
	srand(Tick=time(NULL));
#endif
	
	// build zobrist table
	initZobrist();
	// printZob();
	
	BOARD B;
	if (argc!=3) {
	    TimeOut=(B.LoadGame("board.txt")-3)*1000;
	    if(!B.ChkLose())
			Output(Play(B));
	    return 0;
	}

	Protocol *protocol;
	protocol = new Protocol();
	protocol->init_protocol(argv[1],atoi(argv[2]));
	int iPieceCount[14];
	char iCurrentPosition[32];
	int type, remain_time;
	bool turn;
	PROTO_CLR color;

	char src[3], dst[3], mov[6];
	History moveRecord;
	protocol->init_board(iPieceCount, iCurrentPosition, moveRecord, remain_time);
	protocol->get_turn(turn,color);

	TimeOut = (DEFAULTTIME-3)*1000;

	// init board with char
	B.Init(iCurrentPosition, iPieceCount, (color==2)?(-1):(int)color);

	MOV m;
	if(turn){ // 自己先手
	    m = Play(B);
	    sprintf(src, "%c%c",(m.st%4)+'a', m.st/4+'1');
	    sprintf(dst, "%c%c",(m.ed%4)+'a', m.ed/4+'1');
	    protocol->send(src, dst);
	    protocol->recv(mov, remain_time);
	    if(color == 2) color = protocol->get_color(mov);
	    B.who = color;
	    B.self = B.who;
		B.DoMove(m, chess2fin(mov[3]));
		// B.Display();
	    protocol->recv(mov, remain_time); // 等對方下
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
		B.DoMove(m, chess2fin(mov[3]));
	}else{ // 等對方先手
	    protocol->recv(mov, remain_time);
	    if(color == 2){
			color = protocol->get_color(mov);
			B.who = color;
	    }else {
			B.who = color;
			B.who^=1;
	    }
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
		B.self = B.who^1;
	    B.DoMove(m, chess2fin(mov[3]));
	}
	B.Display();
	
	while(1){
	    m = Play(B); // 算步著
		sprintf(src, "%c%c",(m.st%4)+'a', m.st/4+'1');
	    sprintf(dst, "%c%c",(m.ed%4)+'a', m.ed/4+'1');
	    protocol->send(src, dst); // 給server看
	    protocol->recv(mov, remain_time); // server回傳結果
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	
	    B.DoMove(m, chess2fin(mov[3])); // 在自己的盤面上執行步著
	    B.Display(); // 秀出來

	    protocol->recv(mov, remain_time); // 等對面的步著
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
		B.DoMove(m, chess2fin(mov[3]));
	    B.Display();
	}
	
	return 0;
}

// 審局函數
int evalBoard(const BOARD& B){
	int power[14];
	for(int i=0; i<14; i++){
		power[i] = 51;
		switch(i){
		case FIN_K:
			power[i] += B.remain[7] + 4*(B.remain[8] + 
				B.remain[9] + B.remain[10] + B.remain[11] + B.remain[12]);
			break;
		case FIN_G:
			power[i] += B.remain[8] + 4*(B.remain[9] + 
				B.remain[10] + B.remain[11] + B.remain[12] + B.remain[13]);
			break;
		case FIN_M:
			power[i] += B.remain[9] + 4*(B.remain[10] + 
				B.remain[11] + B.remain[12] + B.remain[13]);
			break;
		case FIN_R:
			power[i] += B.remain[10] + 4*(B.remain[11] + 
				B.remain[12] + B.remain[13]);
			break;
		case FIN_N:
			power[i] += B.remain[11] + 4*(B.remain[12] + B.remain[13]);
			break;
		case FIN_C:
			power[i] += 4*(B.remain[0] + B.remain[1] + B.remain[2] 
				+ B.remain[3] + B.remain[4] + B.remain[5] + B.remain[6]) 
				+ (B.remain[7] + B.remain[8] + B.remain[9] 
				+ B.remain[10] + B.remain[11] + B.remain[12] + B.remain[13]) 
				- 1;
			break;
		case FIN_P:
			power[i] += 4*B.remain[7] + B.remain[13];
			break;
		case FIN_k:
			power[i] += B.remain[0] + 4*(B.remain[1] + 
				B.remain[2] + B.remain[3] + B.remain[4] + B.remain[5]);
			break;
		case FIN_g:
			power[i] += B.remain[1] + 4*(B.remain[2] + 
				B.remain[3] + B.remain[4] + B.remain[5] + B.remain[6]);
			break;
		case FIN_m:
			power[i] += B.remain[2] + 4*(B.remain[3] + 
				B.remain[4] + B.remain[5] + B.remain[6]);
			break;
		case FIN_r:
			power[i] += B.remain[3] + 4*(B.remain[4] + B.remain[5] + B.remain[6]);
			break;
		case FIN_n:
			power[i] += B.remain[4] + 4*(B.remain[5] + B.remain[6]);
			break;
		case FIN_c:
			power[i] += 4*(B.remain[7] + B.remain[8] + B.remain[9] 
				+ B.remain[10] + B.remain[11] + B.remain[12] + B.remain[13]) 
				+ (B.remain[0] + B.remain[1] + B.remain[2] 
				+ B.remain[3] + B.remain[4] + B.remain[5] + B.remain[6]) 
				- 1;
			break;
		case FIN_p:
			power[i] += 4*B.remain[0] + B.remain[6];
			break;
		}
	}

	int red_power = 0;
	int black_power = 0;

	for(int i=0; i<7; i++){
		//printf("%d on = %d, power = %d, remain = %d\n", i, B.onboard[i], power[i], B.remain[i]);
		red_power += B.onboard[i]*power[i];
	}
	for(int i=7; i<14; i++){
		//printf("%d on = %d, power = %d, remain = %d\n", i, B.onboard[i], power[i], B.remain[i]);
		black_power += B.onboard[i]*power[i];
	}
	//printf("red_power = %d, black_power = %d\n", red_power, black_power);
	
	if(B.self == 0)
		return red_power - black_power;
	else if(B.self == 1)
		return black_power - red_power;
}

void initZobrist(){
    // unsigned int zob[15][32];
    unsigned int random_max = pow(2, 32)-1;
    random_device seed;
    default_random_engine e1(seed());
    uniform_int_distribution<unsigned int> dist(0, random_max);
    
    zob_player = dist(e1);
    for(int i=0; i<15; i++)
        for(int j=0; j<32; j++){
            unsigned int r = dist(e1);
            zob[i][j] = r;
        }
    
    hash_max_index = pow(2, 20);
    
    hash_table = (HASH_ENTRY*)malloc(hash_max_index*sizeof(HASH_ENTRY));
    for(int i=0; i<hash_max_index; i++){
        hash_table[i].used = false;
        hash_table[i].exact = false;
    }
}

void printZob(){
	printf("create index = %u\n", hash_max_index);
    printf("zob_player = %u\n", zob_player);
    for(int i=14; i<15; i++)
        for(int j=0; j<32; j++){
            printf("%d %d %u\n", i, j, zob[i][j]);
        }

    for(int i=0; i<100; i++){
		HASH_ENTRY e = hash_table[i];
    	printf("%d exact = %d, move = (%d, %d)\n", i, e.exact, e.best_move.st, e.best_move.ed);
    }
}

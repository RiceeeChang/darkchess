#ifndef ANQI
#define ANQI

// (color)
//  0 = 紅方 (大寫字母)
//  1 = 黑方 (小寫字母)
// -1 = 都不是
typedef int CLR;

// (level)
enum LVL {
	LVL_K=0, // 將帥 King
	LVL_G=1, // 仕士 Guard
	LVL_M=2, // 相象 Minister
	LVL_R=3, // 車? Rook     // BIG5
	LVL_N=4, // 馬傌 kNight
	LVL_C=5, // 包炮 Cannon
	LVL_P=6  // 卒兵 Pawn
};

enum FIN {
	FIN_K= 0 /* 'K' 帥 */ , FIN_k= 7 /* 'k' 將 */ , FIN_X=14 /* 'X' 未翻 */ ,
	FIN_G= 1 /* 'G' 仕 */ , FIN_g= 8 /* 'g' 士 */ , FIN_E=15 /* '-' 空格 */ ,
	FIN_M= 2 /* 'M' 相 */ , FIN_m= 9 /* 'm' 象 */ ,
	FIN_R= 3 /* 'R' ? */ , FIN_r=10 /* 'r' 車 */ ,
	FIN_N= 4 /* 'N' 傌 */ , FIN_n=11 /* 'n' 馬 */ ,
	FIN_C= 5 /* 'C' 炮 */ , FIN_c=12 /* 'c' 包 */ ,
	FIN_P= 6 /* 'P' 兵 */ , FIN_p=13 /* 'p' 卒 */
};





// (position)
//  0  1  2  3
//  4  5  6  7
//  8  9 10 11
// 12 13 14 15
// 16 17 18 19
// 20 21 22 23
// 24 25 26 27
// 28 29 30 31
typedef int POS;

struct MOV {
	POS st; // 起點
	POS ed; // 終點 // ??ed == st 表示是翻子
	int priority;
	MOV() {}
	MOV(POS s,POS e):st(s),ed(e) {}

	bool operator==(const MOV &x) const {return st==x.st&&ed==x.ed;}
	MOV operator=(const MOV &x) {st=x.st;ed=x.ed;return MOV(x.st, x.ed);}
};

struct MOVLST {
	int num;     // 走法數(移動+吃子，不包括翻子)
	MOV mov[68];
};

struct BOARD {

	CLR who;     // 現在輪到哪一方下
	FIN fin[32]; // 各個位置上面擺了什麼
	int cnt[14]; // 各種棋子尚未翻開的數量

	CLR self;    // 自己的顏色
	int dark;    // 盤面暗子的數量
	int position[32];  // 紀錄每個子所在位置
	int current[34];   // 紀錄現在盤面上有那些子
	int onboard[14]; // 記錄各種棋子有多少個在棋盤上
	int remain[14];  // 記錄各種棋子還有多少個
	int zob_key; // hash key 空盤面為零

	void NewGame();                // 開心遊戲
	int  LoadGame(const char*);    // 載入遊戲並傳回時限(單位：秒)
	void Display() const;         // 顯示到stderr上
	int  MoveGen(MOVLST&) const;  // 列出所有走法(走子+吃子，不包括翻子)
	                            // 回傳走法數量
	bool ChkLose() const;        // 檢查當前玩家(who)是否輸了
	bool ChkValid(MOV) const;    // 檢查是否為合法走步
	void Flip(POS,FIN=FIN_X);    // 翻子
	void Move(MOV);             // 移動 or 吃子
	void DoMove(MOV m, FIN f) ;
	//int evalBoard() const;
	void Init(int Board[32], int Piece[14], int Color);
	void Init(char Board[32], int Piece[14], int Color);
};

struct HASH_ENTRY{
	bool used = false;
	unsigned zob_key;
	int depth;

	bool exact = false;
	int value;
	int player;
	MOV best_move;

};

extern unsigned int zob[15][32];
extern unsigned int zob_player;

CLR  GetColor(FIN);    // 算出棋子的顏色
LVL  GetLevel(FIN);    // 算出棋子的階級
bool ChkEats(FIN,FIN); // 判斷第一個棋子能否吃第二個棋子
void Output (MOV);     // 將答案傳給Gui

FIN type2fin(int type);
FIN chess2fin(char chess);

#endif

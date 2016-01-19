#ifndef ANQI
#define ANQI

// (color)
//  0 = ���� (�j�g�r��)
//  1 = �¤� (�p�g�r��)
// -1 = �����O
typedef int CLR;

// (level)
enum LVL {
	LVL_K=0, // �N�� King
	LVL_G=1, // �K�h Guard
	LVL_M=2, // �۶H Minister
	LVL_R=3, // ��? Rook     // BIG5
	LVL_N=4, // ���X kNight
	LVL_C=5, // �]�� Cannon
	LVL_P=6  // ��L Pawn
};

enum FIN {
	FIN_K= 0 /* 'K' �� */ , FIN_k= 7 /* 'k' �N */ , FIN_X=14 /* 'X' ��½ */ ,
	FIN_G= 1 /* 'G' �K */ , FIN_g= 8 /* 'g' �h */ , FIN_E=15 /* '-' �Ů� */ ,
	FIN_M= 2 /* 'M' �� */ , FIN_m= 9 /* 'm' �H */ ,
	FIN_R= 3 /* 'R' ? */ , FIN_r=10 /* 'r' �� */ ,
	FIN_N= 4 /* 'N' �X */ , FIN_n=11 /* 'n' �� */ ,
	FIN_C= 5 /* 'C' �� */ , FIN_c=12 /* 'c' �] */ ,
	FIN_P= 6 /* 'P' �L */ , FIN_p=13 /* 'p' �� */
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
	POS st; // �_�I
	POS ed; // ���I // ??ed == st ��ܬO½�l
	int priority;
	MOV() {}
	MOV(POS s,POS e):st(s),ed(e) {}

	bool operator==(const MOV &x) const {return st==x.st&&ed==x.ed;}
	MOV operator=(const MOV &x) {st=x.st;ed=x.ed;return MOV(x.st, x.ed);}
};

struct MOVLST {
	int num;     // ���k��(����+�Y�l�A���]�A½�l)
	MOV mov[68];
};

struct BOARD {

	CLR who;     // �{�b������@��U
	FIN fin[32]; // �U�Ӧ�m�W���\�F����
	int cnt[14]; // �U�شѤl�|��½�}���ƶq

	CLR self;    // �ۤv���C��
	int dark;    // �L���t�l���ƶq
	int position[32];  // �����C�Ӥl�Ҧb��m
	int current[34];   // �����{�b�L���W�����Ǥl
	int onboard[14]; // �O���U�شѤl���h�֭Ӧb�ѽL�W
	int remain[14];  // �O���U�شѤl�٦��h�֭�
	int zob_key; // hash key �ŽL�����s

	void NewGame();                // �}�߹C��
	int  LoadGame(const char*);    // ���J�C���öǦ^�ɭ�(���G��)
	void Display() const;         // ��ܨ�stderr�W
	int  MoveGen(MOVLST&) const;  // �C�X�Ҧ����k(���l+�Y�l�A���]�A½�l)
	                            // �^�Ǩ��k�ƶq
	bool ChkLose() const;        // �ˬd��e���a(who)�O�_��F
	bool ChkValid(MOV) const;    // �ˬd�O�_���X�k���B
	void Flip(POS,FIN=FIN_X);    // ½�l
	void Move(MOV);             // ���� or �Y�l
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

CLR  GetColor(FIN);    // ��X�Ѥl���C��
LVL  GetLevel(FIN);    // ��X�Ѥl������
bool ChkEats(FIN,FIN); // �P�_�Ĥ@�ӴѤl��_�Y�ĤG�ӴѤl
void Output (MOV);     // �N���׶ǵ�Gui

FIN type2fin(int type);
FIN chess2fin(char chess);

#endif

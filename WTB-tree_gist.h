



// 12���� geohash string Ű��
typedef char wkey[25];


// ����(range) ���� ���� ������ ����
typedef struct
{		
	char nkey[12];	// Node Key
} WTB_KEY_IN_NODE;


// ���� ������ ����
typedef struct
{
	char lkey[12];	// Lower Key
	char ukey[12];	// Upper Key
} WTB_KEY_IN_RANGE;
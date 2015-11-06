



// 12개의 geohash string 키값
typedef char wkey[25];


// 범위(range) 값을 단일 값으로 저장
typedef struct
{		
	char nkey[12];	// Node Key
} WTB_KEY_IN_NODE;


// 범위 값으로 저장
typedef struct
{
	char lkey[12];	// Lower Key
	char ukey[12];	// Upper Key
} WTB_KEY_IN_RANGE;
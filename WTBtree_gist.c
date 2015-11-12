

#include "postgres.h"

#include "access/gist.h"
#include "access/skey.h"

#include "WTBtree_gist.h"
#include "WTBtree_util.h"
#include "./geohash/geohash.h"


Datum WTBtree_consistent(PG_FUNCTION_ARGS);
Datum WTBtree_union(PG_FUNCTION_ARGS);
Datum WTBtree_compress(PG_FUNCTION_ARGS);
Datum WTBtree_decompress(PG_FUNCTION_ARGS);
Datum WTBtree_penalty(PG_FUNCTION_ARGS);
Datum WTBtree_same(PG_FUNCTION_ARGS);
Datum WTBtree_picksplit(PG_FUNCTION_ARGS);


/*
** GiST support methods
*/
PG_FUNCTION_INFO_V1(WTBtree_consistent);
PG_FUNCTION_INFO_V1(WTBtree_compress);
PG_FUNCTION_INFO_V1(WTBtree_decompress);
PG_FUNCTION_INFO_V1(WTBtree_penalty);
PG_FUNCTION_INFO_V1(WTBtree_picksplit);
PG_FUNCTION_INFO_V1(WTBtree_union);
PG_FUNCTION_INFO_V1(WTBtree_same);


Datum WTBtree_consistent(PG_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);	
	StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
	bool		result;
	
	/* Oid		subtype = PG_GETARG_OID(3); */
	
	/* PostgreSQL 8.4 and later require the RECHECK flag to be set here,
	   rather than being supplied as part of the operator class definition */
	bool	   *recheck = (bool *) PG_GETARG_POINTER(4);
	

	/* We set recheck to false to avoid repeatedly pulling every "possibly matched" geometry
	   out during index scans. For cases when the geometries are large, rechecking
	   can make things twice as slow. */
	*recheck = false;

	
	
	PG_RETURN_BOOL(result);
}


Datum WTBtree_union(PG_FUNCTION_ARGS)
{
	GistEntryVector	*entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	int *sizep = (int *) PG_GETARG_POINTER(1); /* Size of the return value */
	int	numranges, i;
	wkey *wkey_cur, *wkey_union;
	
	numranges = entryvec->n;
	
	wkey_cur = (wkey *) DatumGetPointer(entryvec->vector[0].key);
	
	//wkey_union = range_key_to_node_key(wkey_cur);
	
	for ( i = 1; i < numranges; i++ )
	{
		wkey_cur = (wkey *) DatumGetPointer(entryvec->vector[i].key);
		
	}
	
	*sizep = sizeof(wkey);
	
	PG_RETURN_POINTER(wkey_union);
}


Datum WTBtree_compress(PG_FUNCTION_ARGS)
{
	GISTENTRY *entry_in = (GISTENTRY *) PG_GETARG_POINTER(0);
	GISTENTRY *entry_out = NULL;
	wkey *leaf;
	WTB_KEY_IN_LKey *LKEY;
	
	// Leaf 키가 아닐 때,
	if ( ! entry_in->leafkey )
	{		
		PG_RETURN_POINTER(entry_in);
	}
	
	entry_out = palloc(sizeof(GISTENTRY));
	
	if ( DatumGetPointer(entry_in->key) == NULL )
	{	
		gistentryinit(*entry_out, (Datum) 0, entry_in->rel,
		              entry_in->page, entry_in->offset, FALSE);
		
		PG_RETURN_POINTER(entry_out);
	}
	
	leaf = (wkey *) DatumGetPointer(PG_DETOAST_DATUM(entry->key));
	LKEY = range_key_to_node_key(leaf);
	
	/* Prepare GISTENTRY for return. */
	gistentryinit(*entry_out, PointerGetDatum(LKEY),
	              entry_in->rel, entry_in->page, entry_in->offset, FALSE);
	
	PG_RETURN_POINTER(entry_out);
}

Datum WTBtree_decompress(PG_FUNCTION_ARGS)
{
	// decompress 불필요
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}


/*
** GiST support function. Calculate the "penalty" cost of adding this entry into an existing entry.
** Calculate the change in volume of the old entry once the new entry is added.
*/
Datum WTBtree_penalty(PG_FUNCTION_ARGS)
{
	GISTENTRY  *o = (GISTENTRY *) PG_GETARG_POINTER(0);
	GISTENTRY  *n = (GISTENTRY *) PG_GETARG_POINTER(1);
	float	   *result = (float *) PG_GETARG_POINTER(2);
	wkey *origKey, *newKey;
	
	origKey = (wkey *)DatumGetPointer(o->key);
	newKey = (wkey *)DatumGetPointer(n->key);
	
	if ( (origKey == NULL) && (newKey == NULL) )
	{		
		*result = 0.0;
		PG_RETURN_FLOAT8(*result);
	}

	// TODO : (sizeof(origKey) + sizeof(newKey)) - sizeof(origKey) => result
	
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}


Datum
WTBtree_same(PG_FUNCTION_ARGS)
{
	
	bool	   *result = (bool *) PG_GETARG_POINTER(2);

	//TODO 비교 함수
	
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}


Datum
WTBtree_picksplit(PG_FUNCTION_ARGS)
{
	
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) PG_GETARG_POINTER(1);
	
	
					  
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}
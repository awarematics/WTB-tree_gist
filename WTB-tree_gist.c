

#include "postgres.h"

#include "access/gist.h"
#include "access/skey.h"

#include "WTB-tree_gist.h"
#include "WTB-tree_util.h"
#include "./geohash/geohash.h"


Datum WTB-tree_consistent(PG_FUNCTION_ARGS);
Datum WTB-tree_union(PG_FUNCTION_ARGS);
Datum WTB-tree_compress(PG_FUNCTION_ARGS);
Datum WTB-tree_decompress(PG_FUNCTION_ARGS);
Datum WTB-tree_penalty(PG_FUNCTION_ARGS);
Datum WTB-tree_same(PG_FUNCTION_ARGS);
Datum WTB-tree_picksplit(PG_FUNCTION_ARGS);


/*
** GiST support methods
*/
PG_FUNCTION_INFO_V1(WTB-tree_consistent);
PG_FUNCTION_INFO_V1(WTB-tree_compress);
PG_FUNCTION_INFO_V1(WTB-tree_decompress);
PG_FUNCTION_INFO_V1(WTB-tree_penalty);
PG_FUNCTION_INFO_V1(WTB-tree_picksplit);
PG_FUNCTION_INFO_V1(WTB-tree_union);
PG_FUNCTION_INFO_V1(WTB-tree_same);


Datum
WTB-tree_consistent(PG_FUNCTION_ARGS)
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


Datum
WTB-tree_union(PG_FUNCTION_ARGS)
{
	
	
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}


Datum
WTB-tree_compress(PG_FUNCTION_ARGS)
{
	GISTENTRY *entry_in = (GISTENTRY *) PG_GETARG_POINTER(0);
	GISTENTRY *entry_out = NULL;
	wkey *leaf;
	WTB_KEY_IN_LKey *LKEY;
	
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

Datum
WTB-tree_decompress(PG_FUNCTION_ARGS)
{
	// decompress 불필요
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}


Datum
WTB-tree_penalty(PG_FUNCTION_ARGS)
{
	GISTENTRY  *o = (GISTENTRY *) PG_GETARG_POINTER(0);
	GISTENTRY  *n = (GISTENTRY *) PG_GETARG_POINTER(1);
	float	   *result = (float *) PG_GETARG_POINTER(2);

	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}


Datum
WTB-tree_same(PG_FUNCTION_ARGS)
{
	
	bool	   *result = (bool *) PG_GETARG_POINTER(2);

	//TODO 비교 함수
	
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}


Datum
WTB-tree_picksplit(PG_FUNCTION_ARGS)
{
	
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) PG_GETARG_POINTER(1);
	
	
					  
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}
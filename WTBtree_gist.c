

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

void
WTBtree_key_union(Datum *u, wkey *wkey_cur)
{	
	WTB_KEY_IN_IKey cur_ikey = node_key_to_range_key(wkey_cur);
	WTB_KEY_IN_IKey new_ikey;

	if (DatumGetPointer(*u))
	{		
		WTB_KEY_IN_IKey ikey = node_key_to_range_key((wkey *) DatumGetPointer(*u));
		bool update = false;

		new_ikey.lower = ikey.lower;
		new_ikey.upper = ikey.upper;

		if (strcmp(ikey.lower, cur_ikey.lower) > 0)
		{
			new_ikey.lower = cur_ikey.lower;
			update = true;
		}

		if (strcmp(ikey.upper, cur_ikey.upper) < 0)
		{
			new_ikey.upper = cur_ikey.upper;
			update = true;
		}

		if (update)
			*u =PointerGetDatum(range_key_to_wkey(new_ikey));
	}
	else
	{
		new_ikey.lower = cur_ikey.lower;
		new_ikey.upper = cur_ikey.upper;
		*u = PointerGetDatum(range_key_to_wkey(new_ikey));
	}
}

Datum WTBtree_union(PG_FUNCTION_ARGS)
{
	GistEntryVector	*entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	int *sizep = (int *) PG_GETARG_POINTER(1); /* Size of the return value */
	int	numranges, i;
	Datum out;
	wkey *wkey_cur;
	
	numranges = entryvec->n;
	
	wkey_cur = (wkey *) DatumGetPointer(entryvec->vector[0].key);
	
	for ( i = 1; i < numranges; i++ )
	{
		wkey_cur = (wkey *) DatumGetPointer(entryvec->vector[i].key);
		WTBtree_key_union(&out, wkey_cur);
	}
	
	*sizep = sizeof(wkey);
	
	PG_RETURN_POINTER(out);
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

 int
WTBtree_node_cp_len(const wkey *w)
{
	WTB_KEY_IN_IKey ikey = node_key_to_range_key(w);
	int		i = 0;
	int		l = 0;
	int		t1len = VARSIZE(ikey.lower) - VARHDRSZ;
	int		t2len = VARSIZE(ikey.upper) - VARHDRSZ;
	int		ml = Min(t1len, t2len);
	char	   *p1 = VARDATA(ikey.lower);
	char	   *p2 = VARDATA(ikey.upper);

	if (ml == 0)
		return 0;

	while (i < ml)
	{		
		if (*p1 != *p2)
		{
			return i;	
		}

		p1++;
		p2++;
		l--;
		i++;
	}
	return (ml);				/* lower == upper */
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

	Datum		d = PointerGetDatum(0);
	double		dres;
	int		ol, ul;
	
	
	WTBtree_key_union(&d, origKey);
	ol = WTBtree_node_cp_len((wkey *) DatumGetPointer(d));
	WTBtree_key_union(&d, newKey);
	ul = WTBtree_node_cp_len((wkey *) DatumGetPointer(d));
	
	if (ul < ol)
	{
		dres = (ol - ul);	/* reduction of common prefix len */
	}	
	
	*res += FLT_MIN;
	*res += (float) (dres / ((double) (ol + 1)));
	*res *= (FLT_MAX / (o->rel->rd_att->natts + 1));
		
	PG_RETURN_POINTER(result);
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
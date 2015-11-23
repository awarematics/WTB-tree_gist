

#include <string.h>
#include "postgres.h"

#include "access/gist.h"
#include "access/skey.h"

#include "WTBtree_gist.h"
#include "WTBtree_util.h"
#include "./geohash/geohash.h"


#define WTBtree_LessStrategyNumber			1
#define WTBtree_LessEqualStrategyNumber		2
#define WTBtree_EqualStrategyNumber			3
#define WTBtree_GreaterEqualStrategyNumber	4
#define WTBtree_GreaterStrategyNumber		5
#define WTBtree_NotEqualStrategyNumber		6


typedef struct
{
	int	i;
	wkey *t;
} Vsrt;

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


bool char_gt(const void *query, const char *key)
{	
	bool		result;

	result = (strcmp(query, key) > 0);

	return result;
}

bool char_ge(const void *query, const char *key)
{	
	bool		result;

	result = (strcmp(query, key) >= 0);

	return result;
}

bool char_eq(const void *query, const char *key)
{	
	bool		result;
	int			len1, len2;

	len1 = strlen(len1);
	len2 = strlen(len2);
	
	if (len1 != len2)
	{
		result = false;
	} else
	{		
		result = (strcmp(query, ikey->lower) == 0);
	}
	
	return result;
}

bool char_lt(const void *query, const char *key)
{	
	bool		result;

	result = (strcmp(query, key) < 0);

	return result;
}

bool char_le(const void *query, const char *key)
{	
	bool		result;

	result = (strcmp(query, key) <= 0);

	return result;
}

int char_cmp(const void *query, const char *key)
{	
	int		result;

	result = strcmp(query, key);

	return result;
}

Datum WTBtree_consistent(PG_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);	
	void	   *query = (void *) DatumGetTextP(PG_GETARG_DATUM(1));
	StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
	bool		result;	
	/* Oid		subtype = PG_GETARG_OID(3); */
	
	/* PostgreSQL 8.4 and later require the RECHECK flag to be set here,
	   rather than being supplied as part of the operator class definition */
	bool	   *recheck = (bool *) PG_GETARG_POINTER(4);
	
	wkey *key = (wkey *) DatumGetPointer(entry->key);
	WTB_KEY_IN_IKey *ikey = node_key_to_range_key(key);	

	/* We set recheck to false to avoid repeatedly pulling every "possibly matched" geometry
	   out during index scans. For cases when the geometries are large, rechecking
	   can make things twice as slow. */
	*recheck = false;
	
	switch (strategy)
	{
		case WTBtree_LessStrategyNumber:
			if (GIST_LEAF(entry))
				retval = char_lt(query, ikey->lower);
			else
				retval = char_cmp(query, ikey->upper) <= 0;
			break;	
		case WTBtree_LessEqualStrategyNumber:
			if (GIST_LEAF(entry))
				retval = char_le(query, ikey->lower);
			else
				retval = char_cmp(query, ikey->upper) <= 0;
			break;
		case WTBtree_EqualStrategyNumber:
			if (GIST_LEAF(entry))
				retval = char_le(query, ikey->lower);
			else
				retval =
					(char_cmp(query, ikey->lower) >= 0 &&
					 char_cmp(query, ikey->upper) <= 0);
			break;
		case WTBtree_GreaterEqualStrategyNumber:
			if (GIST_LEAF(entry))
				retval = char_ge(query, ikey->lower);
			else
				retval = char_cmp(query, ikey->lower) >= 0;
			break;		
		case WTBtree_GreaterStrategyNumber:
			if (GIST_LEAF(entry))
				retval = char_gt(query, ikey->lower);
			else
				retval = char_cmp(query, ikey->lower) >= 0;
			break;			
		case WTBtree_NotEqualStrategyNumber:
			retval = !(char_eq(query, ikey->lower) && char_eq(query, ikey->upper));
			break;
		default:
			retval = FALSE;
	}

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
	wkey *w1 = (wkey *)PG_GETARG_POINTER(0);
	wkey *w2 = (wkey *)PG_GETARG_POINTER(1);
	bool *result = (bool *) PG_GETARG_POINTER(2);

	if (w1 && w2)
	{
		if (strcmp(w1, w2) == 0)
			result = true;
		else 
			result = false;	
	} else 
	{
		result = false;
	}
	
	PG_RETURN_POINTER(result);
}


Datum
WTBtree_picksplit(PG_FUNCTION_ARGS)
{
	
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) PG_GETARG_POINTER(1);
	
	OffsetNumber i, maxoff = entryvec->n - 1;
	wkey *cur;
	int nbytes;
					  
	arr = (Vsrt *) palloc((maxoff + 1) * sizeof(Vsrt));
	nbytes = (maxoff + 2) * sizeof(OffsetNumber);	
	v->spl_left = (OffsetNumber *) palloc(nbytes);
	v->spl_right = (OffsetNumber *) palloc(nbytes);
	v->spl_ldatum = PointerGetDatum(0);
	v->spl_rdatum = PointerGetDatum(0);
	v->spl_nleft = 0;
	v->spl_nright = 0;
	
	//sv = palloc(sizeof(bytea *) * (maxoff + 1));

	for (i = FirstOffsetNumber; i <= maxoff; i = OffsetNumberNext(i))
	{
		WTB_KEY_IN_IKey ikey;

		cur = (wkey *) DatumGetPointer(entryvec->vector[i].key);
		ikey = node_key_to_range_key(cur);
		if (ikey.lower == ikey.upper)		/* leaf */
		{
			sv[svcntr] = gbt_var_leaf2node((wkey *) cur, tinfo);
			arr[i].t = sv[svcntr];
			if (sv[svcntr] != (wkey *) cur)
				svcntr++;
		}
		else
			arr[i].t = (wkey *) cur;
		arr[i].i = i;
	}
	
	/* sort */
	varg.tinfo = tinfo;
	varg.collation = collation;
	qsort_arg((void *) &arr[FirstOffsetNumber],
			  maxoff - FirstOffsetNumber + 1,
			  sizeof(Vsrt),
			  gbt_vsrt_cmp,
			  (void *) &varg);
			  
	for (i = FirstOffsetNumber; i <= maxoff; i = OffsetNumberNext(i))
	{
		if (i <= (maxoff - FirstOffsetNumber + 1) / 2)
		{
			WTBtree_key_union(&v->spl_ldatum, arr[i].t);
			v->spl_left[v->spl_nleft] = arr[i].i;
			v->spl_nleft++;
		}
		else
		{
			WTBtree_key_union(&v->spl_rdatum, arr[i].t);
			v->spl_right[v->spl_nright] = arr[i].i;
			v->spl_nright++;
		}
	}
	
	PG_RETURN_POINTER(v);
}
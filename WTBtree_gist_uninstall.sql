
DROP OPERATOR CLASS WTBtree_gist_ops using gist;

DROP OPERATOR < (wkey, wkey);

DROP OPERATOR <= (wkey, wkey);

DROP OPERATOR = (wkey, wkey);

DROP OPERATOR >= (wkey, wkey);

DROP OPERATOR > (wkey, wkey);

DROP FUNCTION wtb_lt(query wkey, wk wkey);

DROP FUNCTION wtb_le(query wkey, wk wkey);

DROP FUNCTION wtb_gt(query wkey, wk wkey);

DROP FUNCTION wtb_ge(query wkey, wk wkey);

DROP FUNCTION wtb_eq(query wkey, wk wkey);

DROP TYPE wkey;

DROP FUNCTION WTBtree_compress(internal);

DROP FUNCTION WTBtree_consistent(internal, integer, integer, oid, internal);

DROP FUNCTION WTBtree_decompress(internal);

DROP FUNCTION WTBtree_penalty(internal, internal, internal);

DROP FUNCTION WTBtree_picksplit(internal, internal);

DROP FUNCTION WTBtree_same(internal, internal, internal);

DROP FUNCTION WTBtree_union(internal, internal);
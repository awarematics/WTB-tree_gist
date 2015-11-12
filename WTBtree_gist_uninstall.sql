

DROP FUNCTION WTBtree_compress(internal);

DROP FUNCTION WTBtree_consistent(internal, integer, integer, oid, internal);

DROP FUNCTION WTBtree_decompress(internal);

DROP FUNCTION WTBtree_penalty(internal, internal, internal);

DROP FUNCTION WTBtree_picksplit(internal, internal);

DROP FUNCTION WTBtree_same(internal, internal, internal);

DROP FUNCTION WTBtree_union(internal, internal);

DROP OPERATOR CLASS WTBtree_gist_ops using gist

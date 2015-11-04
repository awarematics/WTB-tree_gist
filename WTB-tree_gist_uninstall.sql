

DROP FUNCTION WTB-tree_compress(internal);

DROP FUNCTION WTB-tree_consistent(internal, integer, integer, oid, internal);

DROP FUNCTION WTB-tree_decompress(internal);

DROP FUNCTION WTB-tree_penalty(internal, internal, internal);

DROP FUNCTION WTB-tree_picksplit(internal, internal);

DROP FUNCTION WTB-tree_same(internal, internal, internal);

DROP FUNCTION WTB-tree_union(internal, internal);

DROP OPERATOR CLASS WTB-tree_gist_ops using gist

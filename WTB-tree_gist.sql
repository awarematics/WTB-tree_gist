\set WTB-tree_LIB '/usr/local/posttrajectory/test/WTB-tree_gist/lib/WTB-tree_gist'

CREATE OR REPLACE FUNCTION WTB-tree_consistent(internal,int4,int,oid,internal)
RETURNS bool
AS :'WTB-tree_LIB','WTB-tree_consistent'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTB-tree_compress(internal)
RETURNS internal
AS :'WTB-tree_LIB','WTB-tree_compress'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTB-tree_decompress(internal)
RETURNS internal
AS :'WTB-tree_LIB','WTB-tree_decompress'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTB-tree_penalty(internal,internal,internal)
RETURNS internal
AS :'WTB-tree_LIB','WTB-tree_penalty'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTB-tree_picksplit(internal, internal)
RETURNS internal
AS :'WTB-tree_LIB','WTB-tree_picksplit'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTB-tree_union(internal, internal)
RETURNS internal
AS :'WTB-tree_LIB','WTB-tree_union'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTB-tree_same(internal, internal, internal)
RETURNS internal
AS :'WTB-tree_LIB','WTB-tree_same'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR CLASS WTB-tree_gist_ops
DEFAULT FOR TYPE trajectory USING gist
AS
	OPERATOR	1	<  ,
	OPERATOR	2	<= ,
	OPERATOR	3	=  ,
	OPERATOR	4	>= ,
	OPERATOR	5	>  ,
	FUNCTION	1	WTB-tree_consistent (internal, int4, int, oid, internal),
	FUNCTION	2	WTB-tree_union (internal, internal),
	FUNCTION	3	WTB-tree_compress (internal),
	FUNCTION	4	WTB-tree_decompress (internal),
	FUNCTION	5	WTB-tree_penalty (internal, internal, internal),
	FUNCTION	6	WTB-tree_picksplit (internal, internal),
	FUNCTION	7	WTB-tree_same (internal, internal, internal);

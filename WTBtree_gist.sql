\set WTBtree_LIB '/usr/local/posttrajectory/test/WTBtree_gist/lib/WTBtree_gist'

CREATE OR REPLACE FUNCTION WTBtree_consistent(internal,int4,int,oid,internal)
RETURNS bool
AS :'WTBtree_LIB','WTBtree_consistent'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTBtree_compress(internal)
RETURNS internal
AS :'WTBtree_LIB','WTBtree_compress'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTBtree_decompress(internal)
RETURNS internal
AS :'WTBtree_LIB','WTBtree_decompress'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTBtree_penalty(internal,internal,internal)
RETURNS internal
AS :'WTBtree_LIB','WTBtree_penalty'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTBtree_picksplit(internal, internal)
RETURNS internal
AS :'WTBtree_LIB','WTBtree_picksplit'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTBtree_union(internal, internal)
RETURNS internal
AS :'WTBtree_LIB','WTBtree_union'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION WTBtree_same(internal, internal, internal)
RETURNS internal
AS :'WTBtree_LIB','WTBtree_same'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR CLASS WTBtree_gist_ops
DEFAULT FOR TYPE trajectory USING gist
AS
	OPERATOR	1	<  ,
	OPERATOR	2	<= ,
	OPERATOR	3	=  ,
	OPERATOR	4	>= ,
	OPERATOR	5	>  ,
	FUNCTION	1	WTBtree_consistent (internal, int4, int, oid, internal),
	FUNCTION	2	WTBtree_union (internal, internal),
	FUNCTION	3	WTBtree_compress (internal),
	FUNCTION	4	WTBtree_decompress (internal),
	FUNCTION	5	WTBtree_penalty (internal, internal, internal),
	FUNCTION	6	WTBtree_picksplit (internal, internal),
	FUNCTION	7	WTBtree_same (internal, internal, internal);

# **********************************************************************
# * 
# *
# * 
# * 
# * 
# *
# * 
# * 
# *
# **********************************************************************

POSTGIS_PGSQL_VERSION=93
WTB-tree=salary_gist
ROOTDIR=/usr/local/posttrajectory
WTBTree_DIR=$(ROOTDIR)/test/$(WTB-tree)

#GeoHash Library
GEOHASHDIR=./geohash

# PostgreSQL psql
PSQL = /usr/local/pgsql/bin/psql

# PostgreSQL Directory 
pgsql = /usr/local/pgsql
includedir = /usr/local/pgsql/include
includedir_server = $(includedir)/server

# .so Directory
trjlibdir = $(WTBTree_DIR)/lib

CC=gcc

# SAIARY Files
WTB-tree_O = WTB-tree_gist.o
WTB-tree_SO = WTB-tree_gist.so
WTB-tree_SQL = WTB-tree_gist.sql
WTB-tree_SQL_uninstall =  WTB-tree_gist_uninstall.sql


all: geohashLib
	$(CC) -fpic -c WTB-tree_gist.c -I$(includedir_server) -I$(GEOHASHDIR)
	$(CC) -shared -o $(WTB-tree_SO) $(WTB-tree_O)

geohashLib: 
	$(CC) -c $(GEOHASHDIR)/geohash.c -o $(GEOHASHDIR)/geohash.o 
	ar rcs $(GEOHASHDIR)/libgeohash.a $(GEOHASHDIR)/geohash.o
	rm $(GEOHASHDIR)/geohash.o

clean:
	rm -f $(WTB-tree_O)
	rm -f $(WTB-tree_SO)
	rm -f $(GEOHASHDIR)/libgeohash.a

install: installdirs 
	cp $(WTB-tree_SO) $(trjlibdir)
	cp $(WTB-tree_SQL) $(WTBTree_DIR)	
	cp $(WTB-tree_SQL_uninstall) $(WTBTree_DIR)
	$(PSQL) -U postgres postgres < $(WTBTree_DIR)/$(WTB-tree_SQL)

installdirs:
	mkdir -p $(WTBTree_DIR)
	mkdir $(trjlibdir)

uninstall:   
	$(PSQL) -U postgres postgres < $(WTBTree_DIR)/$(WTB-tree_SQL_uninstall)
	rm -rf $(WTBTree_DIR)
	

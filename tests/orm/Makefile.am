
AM_CXXFLAGS = \
	-I $(top_srcdir)/include/yb \
	$(XML_CPPFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(SQLITE3_CFLAGS) \
	$(SOCI_CXXFLAGS) \
	$(CPPUNIT_CFLAGS) \
	$(WX_CFLAGS) \
	$(QT_CFLAGS)

check_SCRIPTS = mk_tables.sql

check_PROGRAMS = unit_tests

unit_tests_SOURCES = \
	test_expression.cpp \
	test_schema.cpp \
	test_engine.cpp \
	test_schema_config.cpp \
	test_xmlizer.cpp \
	test_data_object.cpp

unit_tests_LDFLAGS = \
	$(top_builddir)/tests/test_main/libtestmain.la \
	$(top_builddir)/src/orm/libyborm.la \
	$(top_builddir)/src/util/libybutil.la \
	$(XML_LIBS) \
	$(BOOST_LDFLAGS) \
	$(BOOST_LIBS_R) \
	$(CPPUNIT_LIBS) \
	$(ODBC_LIBS) \
	$(SQLITE3_LIBS) \
	$(SOCI_LIBS) \
	$(WX_LIBS) \
	$(QT_LDFLAGS) \
	$(QT_LIBS)

TESTS = unit_tests

export YBORM_DBTYPE=@YBORM_DBTYPE@
export YBORM_DB=@YBORM_DB@
export YBORM_USER=@YBORM_USER@
export YBORM_PASSWD=@YBORM_PASSWD@
export YBORM_URL=@YBORM_URL@

# Oracle specific
export TWO_TASK=balance-dev.yandex.ru
export ORACLE_HOME=/opt/oracle/instantclient_10_2
export LD_LIBRARY_PATH=/opt/oracle/instantclient_10_2
export NLS_NUMERIC_CHARACTERS=. 
export NLS_SORT=RUSSIAN
export NLS_LANG=AMERICAN_CIS.UTF8

mk_tables.sql: $(srcdir)/testdb_schema.xml
	$(top_builddir)/src/yborm_gen/yborm_gen --ddl $^ ${YBORM_DBTYPE} $@

clean-local:
	-rm -f mk_tables.sql

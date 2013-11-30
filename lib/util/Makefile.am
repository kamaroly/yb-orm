
if CPPUNIT_PRESENT
SUBDIRS = . test
else
SUBDIRS =
endif

AM_CXXFLAGS = \
	-I $(top_srcdir)/include/yb \
	$(XML_CPPFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(WX_CFLAGS) \
	$(QT_CFLAGS)

lib_LTLIBRARIES = libybutil.la

libybutil_la_SOURCES = \
	String.cpp \
	Exception.cpp \
	Thread.cpp \
	decimal.cpp \
	DataTypes.cpp \
	str_utils.cpp \
	XmlWriter.cpp \
	ElementTree.cpp \
	Value.cpp \
	nlogger.cpp

libybutil_la_LDFLAGS = -version-info 0:0:0

libybutil_la_LIBADD = \
	$(XML_LIBS) \
	$(BOOST_LDFLAGS) \
	$(BOOST_LIBS_R) \
	$(WX_LIBS) \
	$(QT_LDFLAGS) \
	$(QT_LIBS)


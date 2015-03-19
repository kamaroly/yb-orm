# Getting started #

To try YB.ORM tool in your environment, first you'll need to build
the library and the command line code generation tool. There are several ways to achieve this.
  * For Windows users, the easier way is to get a pre-built package compatible with your compiler. See **Pre-built packages**.
  * In any environment you can download one of released source package tarballs, configure and build. There are two build system available: [Autotools](http://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html) and [CMake](http://www.cmake.org/), see **Building with Autotools** and **Building with CMake**.
Also note, that you can also use [Git](http://git-scm.com/) to pull fresh code from repository, see [YB.ORM Git repository](http://code.google.com/p/yb-orm/source/checkout), and then use either Autotools or CMake.

Then you'll probably want to setup a test database to run provided tests and samples, see **Setting up test database**.

# Pre-built packages #

There are pre-built packages released for Microsoft Windows developers
in form of .zip-files, to make it easier to play with the library and tools.
These packages are built against a specific version of [Boost](http://www.boost.org/)
or [Qt](http://qt-project.org/) toolkit,
compiled using a specific version of C++ compiler (e.g. MinGW 4.4, MSVC 2010, MSVC 2008, etc.).
A Boost-enabled package includes all required components of Boost
libraries within itself, whereas Qt-enabled one requires you to install
proper version of Qt SDK beforehand.   By default, Boost-enabled version
has ODBC as database backend API plus SQLite native driver, and Qt-enabled
version relies on [QtSql](http://qt-project.org/doc/qt-4.8/qtsql.html).
Of course, you can still rebuild YB.ORM library to use your toolkit/compiler/database backend of choice, source code is included.

**Note for Qt4 and MinGW**: the Qt4 SDK was built using MinGW 4.4.0, that is not available from the official site anymore,
but you can grab it from [here](http://nosymbolfound.blogspot.ru/2012/12/since-until-now-qt-under-windows-is.html).

## Pre-built package contents ##

A .zip-file containing YB.ORM library pre-built for Microsoft Windows is extracted at location
`c:\yborm-<version>-<compiler name>` by default.  The description below assumes path `c:\yborm`
as a root folder.  Upon extraction there is the following folder layout:

<table border='1'>
<tr><th>Path</th><th>Description</th></tr>
<tr><td><code>c:\yborm</code></td><td>the root of YB.ORM installation</td></tr>
<tr><td><code>c:\yborm\bin</code></td><td>
<ul><li>.dll-files that this build may depend on: <code>boost_date_time</code>, <code>boost_thread</code>, <code>cppunit</code>, <code>libxml2</code>, <code>sqlite3</code>, misc. runtime<br>
</li><li>some utilities: <code>iconv.exe</code>, <code>xmllint.exe</code>, <code>xsltproc.exe</code>, <code>sqlite3.exe</code>
</li><li>code-generation utility: <strong><code>yborm_gen.exe</code></strong>
</li><li>environment configuration: <strong><code>yborm_env.bat</code></strong></td></tr>
<tr><td><code>c:\yborm\build</code></td><td>build folder for the library itself</td></tr>
<tr><td><code>c:\yborm\build-auth</code></td><td>build folder for the Auth demo</td></tr>
<tr><td><code>c:\yborm\build-tut</code></td><td>build folder for examples from the Tutorial</td></tr>
<tr><td><code>c:\yborm\doc</code></td><td>bits of documentation</td></tr>
<tr><td><code>c:\yborm\examples</code></td><td>compiled examples and tests, schema files, test <a href='http://www.sqlite.org/'>SQLite3</a> database, convenience <code>.bat</code>-files</td></tr>
<tr><td><code>c:\yborm\include</code></td><td>header files for <a href='http://www.sqlite.org/'>SQLite3</a> and also:</td></tr>
<tr><td><code>c:\yborm\include\boost</code></td><td>- for <a href='http://www.boost.org/'>Boost</a> version bundled</td></tr>
<tr><td><code>c:\yborm\include\cppunit</code></td><td>- for <a href='http://sourceforge.net/apps/mediawiki/cppunit/'>CppUnit</a> version bundled</td></tr>
<tr><td><code>c:\yborm\include\libxml</code></td><td>- for <a href='http://www.xmlsoft.org/'>LibXml2</a> version bundled</td></tr>
<tr><td><code>c:\yborm\include\yb</code></td><td>- <strong>YB.ORM header files</strong></td></tr>
<tr><td><code>c:\yborm\lib</code></td><td>YB.ORM in form of static libraries: <strong><code>ybutil.lib</code></strong>, <strong><code>yborm.lib</code></strong> (or <code>.a</code>-files for MinGW);<br>
as well as import libraries for YB.ORM dependencies</td></tr>
<tr><td><code>c:\yborm\src</code></td><td>source code of that released version<br>
</td></tr></table></li></ul>

After unpacking you can run tests and examples from `examples` folder with no additional configuration.

## Rebuilding YB.ORM package ##

The most simple way to build YB.ORM under MS Windows is to rebuild one of
pre-built packages.  This may be useful if you already have YB.ORM installed
and just want to update your installation with fresh sources from Git.
It this case put updated sources under `src` folder, then use command line window to `cd`
into `build` folder and start corresponding `build-*.bat`-file from there.  If you're using
MS Visual C++ you'll need to launch that `.bat`-file from within Visual Studio
command line prompt.  All those `.bat`-files do is just calling CMake script with
predefined configuration parameters and then run `make` command.
After build is successfully completed the new libraries, headers, examples
and code generation utility replace old ones in corresponding folders.
Below are the prerequisites for the rebuild process.

| Build system:  | CMake 2.8 installed, accessible via environment variable `PATH` |
|:---------------|:----------------------------------------------------------------|
| Compiler:      | MinGW GCC 4.4.0, MS Visual C++ 2008 or 2010, Borland C++ Builder 6, or whatever the package was built with |
| Qt SDK:        | tested with Qt4.8, required only for Qt-enabled build, see the note above, where to get the "right" version of MinGW. |

Make sure you specified proper toolkit mode (with or without Qt)
and also set `PATH` to your compiler and Qt SDK in file <strong><code>\bin\yborm_env.bat</code></strong>.

In same way you can build Auth example and Tutorial examples in folders `build-auth` and `build-tut`.

# Building with Autotools #

This piece of software is developed primarily under Ubuntu Linux, using GNU tool chain, including GNU Autotools.
You can build YB.ORM in a POSIX-compatible environment, such as Linux, FreeBSD or Mac OS X. Typical steps are:
```
$ sh autogen.sh # create configure script; only needed after "git clone", not for sources from tarball
$ ./configure --prefix=/home/user1/yb-orm --with-test-db-url=sqlite+sqlite:///home/user1/test1_db.sqlite --disable-static
$ make
$ make check # to run tests you need to create /home/user1/test1_db.sqlite first
$ make install # will be installed under /home/user1/yb-orm
```
The `configure` script tries to guess automatically which libraries are available and where are their header files.
There are number of options you can give to the `configure` script, to affect the process.
Also you can specify parameters for the test database connection.
To trigger build with Qt use `--with-qt-includes,--with-qt-libs`, for wxWidgets-enabled build use `--with-wx-config`.
```
$ ./configure --help
  --with-qt-includes=DIR  Directory where the QT4 C++ includes reside
  --with-qt-libs=DIR      Directory where the QT4 C++ libraries reside
  --with-wx-config=BIN    Configuration script for the wxWidgets C++ library
  --with-boost-includes=DIR
                          Directory where the Boost C++ includes reside
  --with-boost-libs=DIR   Directory where the Boost C++ libraries reside
  --with-soci-includes=DIR
                          Place where SOCI includes are
  --with-soci-libs=DIR    Place where SOCI library is
  --with-test-db-url=dialect+driver://user:password@database or like
                          Specify the URL to connect to the test database
...
```

The custom macros that `configure` uses are placed in `acinclude.m4` file. When you build
your own application with Autotools and YB.ORM you can take `YB_CHECK_YBORM()` macro to your `configure.ac` file.
This will add option `--with-yborm-root` to your `configure` script.
See the [Auth example](http://code.google.com/p/yb-orm/source/browse/examples/auth/README.auth).

## Minimal requirements ##

YB.ORM is known to build without problems on Ubuntu Linux, starting from Ubuntu 8.04.  The following versions of packages should be fine:
  * libboost-thread-dev >= 1.34.1
  * libboost-date-time-dev >= 1.34.1
  * libxml2-dev >= 2.6.31
  * libcppunit-dev >= 1.12.0 (if you're going to run the test suite)
  * autoconf >= 2.61
  * automake >= 1.10.1
  * libqt4-dev >= 4.5 (tested on Ubuntu 9.04)
  * libwxgtk2.8-dev >= 2.8.12

To access some database you will need one of the following:
  * libsqlite3-dev >= 3.7.9
  * soci >= 3.2.0
  * unixodbc-dev >= 2.2.14 (to make Oracle ODBC driver work under Ubuntu 8.04 64bit I had to use unixODBC = 2.3.0)
  * instantclient >= 10.2 (client and ODBC driver for Oracle)
  * libmyodbc >= 5.1.10
  * odbc-postgresql >= 09.00.0310

# Building with CMake #

GNU Autotools are useless when it comes to Microsoft Visual C++ or any other Windows compiler beside of MinGW GCC.
So there are CMake scripts for building YB.ORM on Windows, although they will do the job in POSIX environment too.
This method was tested for the following versions:
  * MinGW GCC 4.4.0, Boost 1.46.1, Qt 4.8.1
  * MS Visual C++ 2010 Express Edition, Boost 1.46.1, Qt 4.8.1
  * MS Visual C++ 2008 Express Edition, Boost 1.38.0, Qt 4.8.1
  * Borland C++ Builder 6, Boost 1.33.1
  * Borland C++ 5.5 Free Command Line Compiler, Boost 1.33.1
  * Ubuntu GCC 4.6.3, Boost 1.46.1, Qt 4.8.1

Download CMake 2.8 from here: http://www.cmake.org/cmake/resources/software.html
Make sure you have `cmake` command accessible on your `PATH` environment variable.

You can pass parameters to CMake, here are some examples:
| -G "NMake Makefiles" | what kind of Makefiles to generate |
|:---------------------|:-----------------------------------|
| -D USE\_QT4:BOOL=ON | build with Qt |
| -D CMAKE\_INSTALL\_PREFIX=c:/yborm | set installation prefix |
| -D SOCI\_INCLUDEDIR:PATH=c:/work/soci-3.2.1/include/soci | SOCI include directory |
| -D SOCI\_LIBS:FILEPATH=c:/work/soci-3.2.1/lib/libsoci\_core.lib | SOCI library file |

# Setting up test database #

To run tests and examples you'll need a test database. If you have downloaded a pre-built package for Windows,
then there is already an SQLite3 test database residing at `c:\yborm\examples\test1_db`, assuming you
unpacked the package to `c:\yborm`. In other cases you'll have to create the test database (or schema),
then create test tables within it.

In case you are using ODBC, you'll also have to set up ODBC datasource (DSN) for your database via
`odbcad32.exe`. In POSIX environment edit configuraion files: `/etc/odbcinst.ini` - for driver
setup and `~/.odbc.ini` - for user's DSN entries.

Note for Windows 64-bit users: since this library comes with pre-built packages compiled in 32-bit mode,
unless you've managed to rebuild it with a 64-bit compiler, use 32-bit version of `odbcad32.exe`: `c:\windows\syswow64\odbcad32.exe`.

Below are steps to follow in order to create an empty test database
with a test user account (using SQLite3, MySQL, Postgres or Firebird),
and then create test tables in there.

## SQLite3 ##

Sample connection strings for SQLite3:
| sqlite+sqlite://c:/yborm/examples/test1\_db |
|:--------------------------------------------|
| sqlite+qtsql://c:/yborm/examples/test1\_db |

This driver is the default for pre-build packages.  The `sqlite3.exe` shell is bundled with pre-built YB.ORM packages, see folder `bin`.

Apply generated SQL scripts like this:
```
C:\yborm\bin>sqlite3 c:\yborm\examples\test1_db < c:\yborm\examples\test_schema.sql
C:\yborm\bin>sqlite3 c:\yborm\examples\test1_db < c:\yborm\examples\ex2_schema.sql
C:\yborm\bin>sqlite3 c:\yborm\examples\test1_db < c:\yborm\examples\auth_schema.sql
```

## MySQL ##

Sample connection strings for MySQL:
| mysql+odbc://test1\_usr:test1\_pwd@test1\_dsn |
|:----------------------------------------------|
| mysql+qodbc3://test1\_usr:test1\_pwd@test1\_dsn |
| mysql+qtsql://test1\_usr:test1\_pwd@127.0.0.1:3306/test1\_db |

You must know the `root` password for your MySQL database to be able
to manage databases and users.

```
C:\MySQL\bin>mysql.exe -u root -p mysql
Enter password: ******** <-- type in here your root password for MySQL
Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 361
Server version: 5.0.51b-community-nt-log MySQL Community Edition (GPL)

Type 'help;' or '\h' for help. Type '\c' to clear the buffer.

mysql> create database test1_db default charset utf8;
Query OK, 1 row affected (0.00 sec)

mysql> create user 'test1_usr'@'localhost' identified by 'test1_pwd';
Query OK, 0 rows affected (0.00 sec)

mysql> grant all on test1_db.* to 'test1_usr'@'localhost';
Query OK, 0 rows affected (0.00 sec)

mysql> exit
Bye
```

Apply generated SQL scripts like this:
```
C:\MySQL\bin>mysql.exe -u test1_usr -ptest1_pwd test1_db < c:\yborm\examples\test_schema.sql
C:\MySQL\bin>mysql.exe -u test1_usr -ptest1_pwd test1_db < c:\yborm\examples\ex2_schema.sql
C:\MySQL\bin>mysql.exe -u test1_usr -ptest1_pwd test1_db < c:\yborm\examples\auth_schema.sql
```

## Postgres ##

Sample connection strings for Postgres:
| postgres+odbc://test1\_usr:test1\_pwd@test1\_dsn |
|:-------------------------------------------------|
| postgres+qodbc3://test1\_usr:test1\_pwd@test1\_dsn |
| postgres+qtsql://test1\_usr:test1\_pwd@127.0.0.1:5432/test1\_db |

```
C:\Program Files\PostgreSQL\9.1\bin>createuser -U postgres -h 127.0.0.1 -p 5432 -D -A -P test1_usr
Enter password for new role: test1_pwd
Enter it again: test1_pwd
Shall the new role be allowed to create more new roles? (y/n) n
Password: <your master password>

C:\Program Files\PostgreSQL\9.1\bin>createdb -U postgres -h 127.0.0.1 -p 5432 -O test1_usr test1_db
Password: <your master password>
```

Apply generated SQL scripts like this:
```
C:\Program Files\PostgreSQL\9.1\bin>psql -h 127.0.0.1 -p 5432 -d test1_db -U test1_usr < c:\yborm\examples\test_schema.sql
Password for user test1_usr: test1_pwd

NOTICE:  CREATE TABLE / PRIMARY KEY will create implicit index "t_orm_test_pkey" for table "t_orm_test"
CREATE TABLE
NOTICE:  CREATE TABLE / PRIMARY KEY will create implicit index "t_orm_xml_pkey" for table "t_orm_xml"
CREATE TABLE
ALTER TABLE
CREATE SEQUENCE
CREATE SEQUENCE

C:\Program Files\PostgreSQL\9.1\bin>psql -h 127.0.0.1 -p 5432 -d test1_db -U test1_usr < c:\yborm\examples\ex2_schema.sql
Password for user test1_usr: test1_pwd
...
C:\Program Files\PostgreSQL\9.1\bin>psql -h 127.0.0.1 -p 5432 -d test1_db -U test1_usr < c:\yborm\examples\auth_schema.sql
Password for user test1_usr: test1_pwd
...
```

## Firebird ##

Sample connection strings for Firebird (Interbase may also apply):
| `"interbase+odbc://test1_usr:test1_pwd@test1_dsn"` |
|:---------------------------------------------------|
| `"interbase+soci://service=localhost:/var/lib/firebird/2.5/data/test1_db.fdb user=test1_usr password=test1_pwd charset=UTF8"` |

The default password for admin user `SYSDBA` is `masterkey`.

```
C:\Program Files\Firebird\Firebird_2_5\bin>gsec -user SYSDBA -password masterkey
GSEC> add test1_usr -pw test1_pwd
Warning - maximum 8 significant bytes of password used
GSEC> quit

C:\Program Files\Firebird\Firebird_2_5\bin>isql
Use CONNECT or CREATE DATABASE to specify a database
SQL> CREATE DATABASE 'localhost:c:/yborm/examples/test1_db.fdb'
CON> page_size 8192 user 'test1_usr' password 'test1_pwd';
SQL> quit;
```

Apply generated SQL scripts like this:
```
C:\Program Files\Firebird\Firebird_2_5\bin>isql -u test1_usr -p test1_pwd localhost:c:/yborm/examples/test1_db.fdb < c:\yborm\examples\test_schema.sql
C:\Program Files\Firebird\Firebird_2_5\bin>isql -u test1_usr -p test1_pwd localhost:c:/yborm/examples/test1_db.fdb < c:\yborm\examples\ex2_schema.sql
C:\Program Files\Firebird\Firebird_2_5\bin>isql -u test1_usr -p test1_pwd localhost:c:/yborm/examples/test1_db.fdb < c:\yborm\examples\auth_schema.sql
```

In Ubuntu Linux, the usage of `gsec` is the same, but you may have to use `isql-fb` command instead of `isql`:
```
$ isql-fb
Use CONNECT or CREATE DATABASE to specify a database
SQL> CREATE DATABASE '/var/lib/firebird/2.5/data/test1_db.fdb'
CON> page_size 8192 user 'test1_usr' password 'test1_pwd';
SQL> quit;
$ isql-fb -u test1_usr -p test1_pwd \
  localhost:/var/lib/firebird/2.5/data/test1_db.fdb < lib/orm/test/mk_tables.sql
$ isql-fb -u test1_usr -p test1_pwd \
  localhost:/var/lib/firebird/2.5/data/test1_db.fdb < examples/domain/mk_tables.sql
$ isql-fb -u test1_usr -p test1_pwd \
  localhost:/var/lib/firebird/2.5/data/test1_db.fdb < examples/auth/src/domain/auth_schema.sql
```
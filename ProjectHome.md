# This page may be out of date! #

Please, look for the most recent updates here:

**Code:** https://github.com/vnaydionov/yb-orm

**Wiki:** https://github.com/vnaydionov/yb-orm/wiki

**Project downloads:** https://sourceforge.net/projects/yborm/


# News #

For a while, I will post news here too:

## Release 0.4.6 ##
  * New optional inline syntax for describing the mapping scheme within a class declaration, based on macros.
  * Porting tut2 and tut4 examples to the new declaration syntax instead of using XML configs.
  * Support .range() pager in queries for different SQL dialects, also add .first() filter based on that pager.
  * Updated documantation.
  * Simple constructor for Session that accepts the connection string.
  * It's possible now to construct the Session with a native connection, for example soci::session.
  * Logger can be set in Session, no need to explicitly create Engine object, in simple cases.
  * Schema object can export its current configuration to XML or SQL.
  * New options object for SQL generation procedure.
  * Many fixes to the build scripts.
  * Auth example: Fix TcpSocket class read() method to use select() and a buffer.

## Release 0.4.5 ##
  * New source tree layout: separate public includes from private, the library source file names are now in lowercase.
  * Tested to build on Windows with MS Visual Studio 2013.
  * Enable building .dll libraries on Windows by default.
  * Fixed Autoconf/Automake scripts to build on Ubuntu 14.04, as well as on FreeBSD 10, Cygwin.
  * Use third party boost.m4 to detect Boost at ./configure stage.
  * Added stuff to make Debian packages.
  * Many little API improvements.
  * Basic means to create indices from schema.
  * Updated tutorials.
  * Auth example: refactor micro\_http server.


# ABOUT #

[Версия на русском языке](http://code.google.com/p/yb-orm/wiki/About?wl=ru)

YB.ORM tool is developed to simplify for C++ developers creation of applications
that work with relational databases.  An Object-Relational Mapper (ORM) works by
mapping database tables to classes and table records to objects in some
programming language.  This approach may be not optimal for each and every
database application, but it proved to be reasonable in applications that
require complex logic and transaction handling. The goals of YB.ORM project are:
provide a convenient API for C++ developers, retain high performance of C++,
keep the source code easily portable across different platforms, support
most major relational DataBase Management Systems (DBMS).

Typical usage scenario is the following:
  * Describe your database schema i.e. tables with their columns and relationships between tables in [simple XML-based format](XmlFormat.md).
  * Use the code generation tool provided to produce C++ domain classes for your tables (see [Tutorial1](Tutorial1.md)).
  * Add your application-specific logic to the classes.
  * Should you make changes to the database schema, you then use the code generation tool to synchronize previously generated C++ classes with the schema.
  * Optionally, use the code generation tool to issue SQL statements  to populate your empty database schema with tables.
  * Now you can use the domain classes in cooperation with Session object to automate the following tasks:
    * create new objects or delete existing objects;
    * query objects from database, using object-based queries (see [Tutorial3](Tutorial3.md));
    * modify existing objects by just assignment to objects' fields;
    * link and unlink objects to/from each other using relations (see [Tutorial2](Tutorial2.md));
    * serialize your objects into XML and JSON.

This tool employs many ideas explained in the book
"Patterns of Enterprise Application Architecture" by Martin Fawler.
Such patterns as "Lazy Load", "Identity Map", "Unit of Work", etc.
Also, the project development was inspired by the power of [Hibernate](http://www.hibernate.org/) framework for Java, and especially by the design of
[SQLAlchemy](http://www.sqlalchemy.org/) for Python. Some details of how it works are here: [Internals](Internals.md).

## STATUS ##

At the moment, YB.ORM tool works in Linux, Mac OS X and Windows.
It can be built using the following compilers: [GCC](http://gcc.gnu.org/), [CLang](http://clang.llvm.org/),
[MinGW](http://mingw.org/), [MSVC 2008/2010](http://www.microsoft.com/visualstudio/), also Borland C++ Builder, see [Building Instructions](Build.md).

You can build YB.ORM library against your toolkit of choice: [Boost](http://www.boost.org/), [Qt](http://qt-project.org/),
or [wxWidgets](http://www.wxwidgets.org/); their native data types will be used for strings, date and time, threads, XML parsing, etc.

The tool was tested to work with the following SQL databases:
[MySQL](http://www.mysql.com/), [Oracle](http://www.oracle.com/technetwork/products/express-edition/),
[SQLite](http://www.sqlite.org/), [Firebird](http://www.firebirdsql.org/), [Postgres](http://www.postgresql.org/).
To connect to a database you have these options:
  * [ODBC](http://www.unixodbc.org/) drivers
  * popular database connectivity C++ library [SOCI](http://soci.sourceforge.net/)
  * database drivers for Qt library: [QtSql](http://qt-project.org/doc/qt-4.8/qtsql.html)
  * there is native driver for SQLite.
Optionally, use connection pooling the library provides.
Look here for details: [Connecting to database](SqlDriver.md).

The basic functionality of YB.ORM was tested on several little projects, and proved to be usable.
Keep in mind, that the tool is under development and its API may change between releases.
Some interesting features are still to be implemented (see [ToDo](ToDo.md) list).

## AUTHORS ##

Project has started in late 2006 as a part of billing system for on-line ads
at Yandex company (http://www.yandex.com/) by Viacheslav Naydenov (me) and Sergey Chmelev.
"YB" used to stand for Yandex.Balance, the name for the billing system.
Today YB.ORM project is not affiliated with Yandex company.
The source code was released in early 2011 under open source license, since then the code has been mostly rewritten.
Thanks to the author of [tiny odbc wrapper](http://code.google.com/p/tiodbc/),
whose code is used as a base for the first usable SQL driver (`OdbcDriver` class).
Also, I would like to mention feedback and contribution from several students of Moscow State Open University (MSOU),
where I was using this piece of software as a case study. The list of contributors:
  * Viacheslav Naydenov, vaclav at yandex dot ru - main developer
  * Sergey Chmelev, halty at yandex dash team dot ru - early contributor
  * SqUe, squarious at gmail dot com - tiodbc code
  * Nikita Buyvich, buyvich at gmail dot com - MSOU
  * Viacheslav Fedorov, funny dash story at yandex dot ru - MSOU
  * Andrey Skobenkov, andrey dot skobenkov at gmail dot com - MSOU

Any feedback, patches or comments are welcome!
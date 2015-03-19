# Data flow architecture #
ORM is all about abstraction layers, so let's look at the layers from top to bottom.

<img src='http://yb-orm.googlecode.com/git/doc/pics/SqlDriver.1.png' align='right' />

## Session ##
Like in many ORM solutions, the central role in YB.ORM workflow plays instance of `Yb::Session` class, which keeps track of all mapped objects. These objects may be created or deleted, get loaded or receive changes to their attributes. When there's a need to apply all changes to database, one calls `session.flush()`. This method issues sequences of SQL statements to the database to reflect all the changes made, so session needs a way to communicate to database.

## Engine ##
Each time a new session is created, the session is given an engine, an instance of `Yb::Engine` class, which acts like a gate between session and database. In particular, engine takes care of correct SQL code generation for certain dialect, and, of course, it handles the connection to database (`Yb::SqlConnection`) in one of the following ways:
  * Engine owns a single _connection object_, that was passed in on engine creation, it sends all SQL statements to that connection.
  * Like the above, but no connection object is passed on engine creation, instead, engine creates a new connection based on _connection string_ found in environment variable `YBORM_URL`.
  * Engine holds a _pool of database connections_ `Yb::SqlPool`, from which it fetches a connection object for each newly created session. Respectively, engine manages to return connection object back into the pool on session destruction.

## Connection ##
In C++, at least for now, there is no standard way to communicate to a relational database. Although, there are some proposals (see [N3612](https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/iqOtgxP_IRA)), so things may change. Use of pure C ODBC API is common, but is considered as lacking speed of native client libraries. So the obvious decision here was to make a thin layer of abstraction to allow for multiple future implementations. This is what classes `Yb::SqlDriver`, `Yb::SqlConnection`, `Yb::SqlCursor` and `Yb::SqlResultSet` do. They provide generic but still basic interface for SQL statement execution and result set fetching, for engine to do its job.

## SQL drivers ##
Several drivers for distinct database API are available now, each has its own name: `ODBC`, `SQLITE`, `QTSQL`, `QODBC3`, `SOCI` and `SOCI_ODBC`.  Some of the drivers may not be available, depending on your build configuration.  When building YB.ORM against Qt, there are native database drivers available under `QTSQL` name, while `QODBC3` means Qt wrapper around ODBC API.  Name `SQLITE` refers to the native driver for SQLite3. Recently `SOCI` and `SOCI_ODBC` drivers have been added, that work through a popular library [SOCI](http://soci.sourceforge.net/).

# Creating a connection object #
`Yb::SqlConnection` object may be initialized with a connection string, that contains all the necessary properties for the connection.
A connection string for YB.ORM looks like an URL.  First component of such URL is `dialect+driver://` part.  The rest of connection string may look driver-specific.  Dialect and driver names are case-insensitive.  For now `dialect` can be one of the following: `ORACLE`, `POSTGRES`, `INTERBASE`, `MYSQL` or `SQLITE`. Note, that `INTERBASE` dialect name represents both InterBase and Firebird RDBMS.

Look at some examples of how to specify a connection using ODBC:
  * `"interbase+odbc://test1_usr:test1_pwd@test1_dsn"`
  * `"postgres+qodbc3://test1_usr:test1_pwd@test1_dsn"`
  * `"mysql+soci_odbc://DSN=test1_dsn;UID=test1_usr;PWS=test1_pwd"`
and native drivers:
  * `"mysql+qtsql://test1_usr:test1_pwd@127.0.0.1:3306/test1_db"`
  * `"mysql+soci://user=test1_usr pass=test1_pwd host=localhost service=test1_db"`
  * `"sqlite+sqlite://c:/yborm/examples/test1_db"`

As you can see, drivers `ODBC`, `QTSQL`, `QODBC3`, `SQLITE` all employ common HTTP-like style for their connection strings:
  * `"dialect+driver://[user[:password]@]host[:port]/database[?property1=value1...]"`
or, in case of connection string for ODBC, the shorter form is used:
  * `"dialect+driver://[user[:password]@]dsn[?property1=value1...]"`
Whereas `SOCI` and `SOCI_ODBC` drivers just pass the rest of the connection string directly to the SOCI backend.

**ORACLE** and **ODBC**: to get Oracle ODBC driver working under GNU/Linux
x86\_64 I have used unixODBC=2.3.0.  Also note, that beside of the connection string, properly set up ODBC DSN record, and the usual Oracle environment variables (`ORACLE_HOME`, `TNS_ADMIN`), you may have to set special environment variable `TWO_TASK` to your database name from `tnsnames.ora` file, in order to get it working.

## Environment variable YBORM\_URL ##

The database connection string to run examples and tests is passed
through the environment variable `YBORM_URL`, except for those apps in "tutorial" directory: they have connection strings hard-coded directly in their source code.  In fact this environment variable is used if you construct `Yb::Engine` instance with no parameters, also you can give a connection object or a connection pool object to your engine, explicitly.

**POSIX**: You can set this variable for the whole build when you run
"configure" script, just pass a parameter like this:
  * `--with-test-db-url="mysql+odbc://test1_usr:test1_pwd@test1_dsn"`

**WINDOWS**: In a pre-built package for Windows look inside `\bin\yborm_env.bat` file for this variable.
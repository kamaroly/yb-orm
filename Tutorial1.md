## Mapping a single class to a table (tut1.cpp) ##

In this tutorial we assume that you are using some Unix-like environment: Linux, FreeBSD, Mac OS X or Cygwin for Windows are OK. For native Windows users all steps are basically the same, but paths and compiler command line options may vary. Let's assume that the root of your installation of YB.ORM is pointed to by the environment variable `YBORM_ROOT`.

First, describe your data schema in a simple XML format and save it into file named `tut1.xml` in your working directory.

```
<schema>
  <table name="client" sequence="seq_client" class="Client">
    <column name="id" type="longint">
      <primary-key />
    </column>
    <column name="dt" type="datetime" null="false" default="sysdate" />
    <column name="name" type="string" size="100" null="false" />
    <column name="email" type="string" size="100" null="false" />
    <column name="phone" type="string" size="50" null="true" />
    <column name="budget" type="decimal" />
  </table>
</schema>
```

Then use the code generation utility `yborm_gen` to produce C++ domain classes. The first argument is the mode of code generation, the second is the file name of the schema, the third is the directory to put your generated class files to, the fourth (optional) is the directory prefix for `#include` directive (by default: `domain/`).

```
$ mkdir domain
$ $YBORM_ROOT/bin/yborm_gen --domain tut1.xml domain
yborm_gen: table count: 1
yborm_gen: generation started...
yborm_gen: Generating file: domain/Client.h for table 'client'
yborm_gen: Generating cpp file: domain/Client.cpp for table 'client'
yborm_gen: generation successfully finished
```

Now there are two files in the `domain` directory: `Client.h` and `Client.cpp`. They contain domain class definition for `Client` with all its properties. You can view and (with some precautions) edit them. The schema file can be edited later and new entities and columns will be added, the subsequent run of `yborm_gen` utility will not overwrite the domain class files completely, only the sections marked as "auto-generated" will get updated.

Then use the same utility with another mode to generate SQL DDL statements to help create tables and sequences. When invoking that script specify the SQL dialect. For this simple example we will create an `SQLITE` database.

```
$ $YBORM_ROOT/bin/yborm_gen --ddl tut1.xml SQLITE > tut1.sql
yborm_gen: table count: 1
yborm_gen: generation started...
yborm_gen: generation successfully finished
```

The tool will produce SQL code like this:

```
CREATE TABLE client (
    id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) NOT NULL,
    phone VARCHAR(50),
    budget NUMERIC
);
```

Create test database as a file in the working directory, and verify that the table is empty:

```
$ sqlite3 tut1.sqlite < tut1.sql
$ sqlite3 tut1.sqlite
SQLite version 3.7.9 2011-11-01 00:52:41
Enter ".help" for instructions
Enter SQL statements terminated with a ";"
sqlite> select count(*) from client;
0
sqlite>
```

Now its possible to use domain class `Client`. Let's look at the example. To synchronize objects to the DB an instance of `Yb::Session` class is used. As an abstraction layer for SQL there are classes `Yb::Engine` and `Yb::SqlConnection`. In the example below you see the session initialization, then a new instance of mapped class is created and its properties are filled, then it is stored in a session and pushed down to the DB. During the synchronization the object is assigned an ID. Save the following example into `tut1.cpp`.

```
#include <iostream>
#include "domain/Client.h"
int main()
{
    std::auto_ptr<Yb::SqlConnection> conn(new Yb::SqlConnection(
            "sqlite+sqlite://./tut1.sqlite"));
    Yb::Engine engine(Yb::Engine::READ_WRITE, conn);
    Yb::Session session(Yb::init_schema(), &engine);
    Domain::Client client;
    std::string name, email, budget;
    std::cout << "Enter name, email, budget:\n";
    std::cin >> name >> email >> budget;
    client.name = name;
    client.email = email;
    client.budget = Yb::Decimal(budget);
    client.dt = Yb::now();
    client.save(session);
    session.flush();
    std::cout << "New client: " << client.id.value() << std::endl;
    engine.commit();
    return 0;
}
```

To use YB.ORM in your program you'll need to link with libraries `libyborm`, `libybutil`. The libraries required typically reside in `$YBORM_ROOT/lib`. If your build uses static linking you may also need to link with their dependencies: `-lxml2`, `-lboost_thread`, `-lboost_date_time`, and probably one of `-lodbc`, `-lsqlite3` or `-lsoci`. In the simplest case the following command will build and link the example:

```
$ c++ -I. -I$YBORM_ROOT/include/yb -o tut1 tut1.cpp domain/Client.cpp -L$YBORM_ROOT/lib -lybutil -lyborm
```

Let's run the example and look at the result. Note, if we have used an YB.ORM build with dynamic libraries we may have to add the `lib` directory to the search path.

```
$ export LD_LIBRARY_PATH=$YBORM_ROOT/lib
$ ./tut1
Enter name, email, budget:
Vasya vas@ya.ru 23.45   _<-- user input_
New client: 1
$
```

Let's check what is in the table now:

```
$ sqlite3 tut1.sqlite
SQLite version 3.7.9 2011-11-01 00:52:41
Enter ".help" for instructions
Enter SQL statements terminated with a ";"
sqlite> select * from client;
1|2014-06-01T13:30:36|Vasya|vas@ya.ru||23.45
sqlite>
```

There is one row now. To see what is happening under the hood we can enable logging facility. Make the following modifications to your `tut1.cpp`:

```
int main()
{
**    Yb::LogAppender appender(std::cerr);**
    std::auto_ptr<Yb::SqlConnection> conn(new Yb::SqlConnection(
            "sqlite+sqlite://./tut1.sqlite"));
    Yb::Engine engine(Yb::Engine::READ_WRITE, conn);
**    engine.set_logger(Yb::ILogger::Ptr(new Yb::Logger(&appender)));**
**    engine.set_echo(true);**
    Yb::Session session(Yb::init_schema(), &engine);
```

Compile it and run again, this reveals more details about the interaction with the database:

```
$ ./tut1
Enter name, email, budget:
Petya pet@ya.ru 56.78   _<-- user input_
14-06-01 13:55:12.738 28833/28833 DEBG orm: flush started
14-06-01 13:55:12.739 28833/28833 DEBG sql: begin transaction
14-06-01 13:55:12.739 28833/28833 DEBG sql: prepare: INSERT INTO client (dt, name, email, phone, budget) VALUES (?, ?, ?, ?, ?)
14-06-01 13:55:12.739 28833/28833 DEBG sql: bind: (DateTime, String, String, String, Decimal)
14-06-01 13:55:12.739 28833/28833 DEBG sql: exec prepared: p1="'2014-06-01 13:55:12'" p2="'Petya'" p3="'pet@ya.ru'" p4="NULL" p5="56.78"
14-06-01 13:55:12.740 28833/28833 DEBG sql: prepare: SELECT SEQ LID FROM SQLITE_SEQUENCE WHERE NAME = 'client'
14-06-01 13:55:12.740 28833/28833 DEBG sql: exec prepared:
14-06-01 13:55:12.740 28833/28833 DEBG sql: fetch: LID='2' 
14-06-01 13:55:12.740 28833/28833 DEBG sql: fetch: no more rows
14-06-01 13:55:12.740 28833/28833 DEBG orm: flush finished OK
New client: 2
14-06-01 13:55:12.740 28833/28833 DEBG sql: commit
$
```

As you can see YB.ORM gives a developer power to operate with tables and rows in an object-oriented fashion. All the properties of a mapped object are easily accessible. More than that it will handle transactions and logging.
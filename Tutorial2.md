## Mapping related tables: one-to-many (tut2.cpp) ##
Suppose you have two related tables: there are many rows in `order_tbl` table
corresponding to just one row in `client` table.  At SQL level this can be
expressed as having "foreign key constraint" on column `order_tbl.client_id`
in "child" table referencing column `client.id` in "parent" table.

At ORM level such relations are represented by objects' properties.
An instance of a class that maps to the child table usually has
an object reference property, referring to the parent object.
From the other side of such relation, the instance of a class that maps to
the parent table may have a collection-of-objects property
(which is sometimes called "backref"), to be able to iterate all over
its children.

First, let's define table mapping and describe their relationship in XML file:
```
<schema>
    <table name="client_tbl" sequence="seq_client" class="Client">
        <column name="id" type="longint">
            <primary-key />
        </column>
        <column name="dt" type="datetime" null="false" default="sysdate" />
        <column name="name" type="string" size="100" null="false" />
        <column name="email" type="string" size="100" null="false" />
        <column name="budget" type="decimal" />
    </table>
    <table name="order_tbl" sequence="seq_order" class="Order" xml-name="order">
        <column name="id" type="longint">
            <primary-key />
        </column>
        <column name="client_id" type="longint" null="false">
            <foreign-key table="client" key="id"/>
        </column>
        <column name="dt" type="datetime" null="false" default="sysdate" />
        <column name="memo" type="string" size="100" />
        <column name="total_sum" type="decimal" null="false" />
        <column name="receipt_sum" type="decimal" default="0" />
        <column name="receipt_dt" type="datetime" />
    </table>
    <relation type="one-to-many">
        <one class="Client" property="orders" />
        <many class="Order" property="owner" />
    </relation>
</schema>
```

When you generate SQL code (MySQL dialect) for the declaration above
(see [Tutorial1](Tutorial1.md)), it will look like this:
```sql

CREATE TABLE client (
id BIGINT NOT NULL AUTO_INCREMENT,
dt TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
name VARCHAR(100) NOT NULL,
email VARCHAR(100) NOT NULL,
budget DECIMAL(16, 6)
, PRIMARY KEY (id)
) ENGINE=INNODB DEFAULT CHARSET=utf8;

CREATE TABLE order_tbl (
id BIGINT NOT NULL AUTO_INCREMENT,
client_id BIGINT NOT NULL,
dt TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
memo VARCHAR(100),
total_sum DECIMAL(16, 6) NOT NULL,
receipt_sum DECIMAL(16, 6) DEFAULT 0,
receipt_dt TIMESTAMP
, PRIMARY KEY (id)
) ENGINE=INNODB DEFAULT CHARSET=utf8;

ALTER TABLE order_tbl ADD FOREIGN KEY (client_id) REFERENCES client(id);
```

The following program uses the domain classes, generated from the XML
description, see [Tutorial1](Tutorial1.md).
The program creates the child object first and assigns some
values to its fields, then it creates the parent object, and then it
links the child to the parent. After all it stores the objects to the session
and flushes them to the database committing transaction as well
(`session.commit();`).  Note, that these objects will be stored in proper
sequence (first - parent, second - child), also note that the foreign key value
is assigned automatically, as well as primary key values.
```
#include <memory>
#include <iostream>
#include "domain/Client.h"
#include "domain/Order.h"
using namespace std;

int main()
{
    //Yb::LogAppender appender(cerr);
    auto_ptr<Yb::SqlConnection> conn(new Yb::SqlConnection(
                "mysql://test1_usr:test1_pwd@test1_db"));
    Yb::Engine engine(Yb::Engine::READ_WRITE, conn);
    //engine.set_logger(Yb::ILogger::Ptr(new Yb::Logger(&appender)));
    //engine.set_echo(true);
    Yb::Session session(Yb::init_schema(), &engine);

    Domain::OrderHolder order;
    string amount;
    cout << "Enter order amount: \n";
    cin >> amount;
    order->total_sum = Yb::Decimal(amount);

    Domain::ClientHolder client;
    string name, email;
    cout << "Enter name, email: \n";
    cin >> name >> email;
    client->name = name;
    client->email = email;
    client->dt = Yb::now();

    cout << "Client's orders count: " << client->orders.size() << "\n";
    order->owner = client;
    cout << "Client's orders count: " << client->orders.size() << "\n";

    order->save(session);
    client->save(session);
    session.commit();
    cout << order->xmlize(1)->serialize() << endl;
    return 0;
}
```

Here you can see that linking a child (`Order` class) to a parent
(`Client` class) looks like an assignment (`order->owner = client;`).
But this operation also leads to the alteration of `orders`
collection of corresponding instance of `Client` class.
This is because both `client->orders` and `order->client` share
the same `RelationObject` internally.

This example does not operate the domain classes directly, as in [Tutorial1](Tutorial1.md),
but instead uses `<Domain>Holder` classes which are necessary to
implement recurrent (nested) object references.  That is, all the
object properties are implemented using `<Domain>Holder` classes, and
therefore they must be de-referenced using arrow (`->`), not just dot (`.`).

If you trace the SQL operators, by enabling logging for example (just uncomment
commented lines in the example),
then you'll see the following (in case of using MySQL):
```sql

prepare: INSERT INTO client (dt, name, email, budget) VALUES (?, ?, ?, ?)
exec prepared: p1="'2012-07-16 15:29:17'" p2="'Petya'" p3="'petya@pupkin.com'" p4="NULL"
exec_direct: SELECT LAST_INSERT_ID() LID
fetch: LID=9
fetch: no more rows
prepare: INSERT INTO order_tbl (client_id, dt, memo, total_sum, receipt_sum, receipt_dt) VALUES (?, ?, ?, ?, ?, ?)
exec prepared: p1="9" p2="'2012-07-16 15:29:01'" p3="NULL" p4="123.45" p5="0" p6="NULL"
exec_direct: SELECT LAST_INSERT_ID() LID
fetch: LID=8
fetch: no more rows
commit
prepare: SELECT id, client_id, dt, memo, total_sum, receipt_sum, receipt_dt FROM order_tbl WHERE 1=1 AND id = ?
exec prepared: p1="8"
fetch: ID=8 CLIENT_ID=9 DT='2012-07-16 15:29:01' MEMO=NULL TOTAL_SUM='123.450000' RECEIPT_SUM='0.000000' RECEIPT_DT=NULL
fetch: no more rows
prepare: SELECT id, dt, name, email, budget FROM client WHERE 1=1 AND id = ?
exec prepared: p1="9"
fetch: ID=9 DT='2012-07-16 15:29:17' NAME='Petya' EMAIL='petya@pupkin.com' BUDGET=NULL
fetch: no more rows
rollback
```
or something like that (in case you're using ORACLE):
```sql

prepare: SELECT seq_client.NEXTVAL FROM DUAL
exec prepared:
fetch: NEXTVAL='2'
fetch: no more rows
prepare: INSERT INTO client (id, dt, name, email, budget) VALUES (?, ?, ?, ?, ?)
exec prepared: p1="2" p2="'2012-07-16 15:28:09'" p3="'Petya'" p4="'petya@pupkin.com'" p5="NULL"
prepare: SELECT seq_order.NEXTVAL FROM DUAL
exec prepared:
fetch: NEXTVAL='4'
fetch: no more rows
prepare: INSERT INTO order_tbl (id, client_id, dt, memo, total_sum, receipt_sum, receipt_dt) VALUES (?, ?, ?, ?, ?, ?, ?)
exec prepared: p1="4" p2="2" p3="'2012-07-16 15:27:58'" p4="NULL" p5="123.45" p6="0" p7="NULL"
commit
prepare: SELECT id, client_id, dt, memo, total_sum, receipt_sum, receipt_dt FROM order_tbl WHERE 1=1 AND id = ?
exec prepared: p1="4"
fetch: ID='4' CLIENT_ID='2' DT='2012-07-16 15:27:58' MEMO=NULL TOTAL_SUM='123.45' RECEIPT_SUM='0' RECEIPT_DT=NULL
fetch: no more rows
prepare: SELECT id, dt, name, email, budget FROM client WHERE 1=1 AND id = ?
exec prepared: p1="2"
fetch: ID='2' DT='2012-07-16 15:28:09' NAME='Petya' EMAIL='petya@pupkin.com' BUDGET=NULL
fetch: no more rows
rollback
```
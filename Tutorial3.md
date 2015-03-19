## Querying objects (tut3.cpp) ##
We'll use the data schema from [Tutorial2](Tutorial2.md) "Mapping related tables: one-to-many". There are two tables: Clients (`client`) and Orders (`order_tbl`), one client is supposed to have zero or more orders. Let's define the tables, their mapping to classes and their relationship in an XML file:
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

If necessary, generate the SQL code to create tables and other objects in your database schema. Specify the SQL dialect when using the code generation tool, the following dialects are accepted: `SQLITE`, `MYSQL`, `POSTGRES`, `ORACLE`, `INTERBASE`. Then you can apply the generated script using standard means of your corresponding database engine, e.g. for Oracle there is `sqlplus`.
```
 $(YBORM_ROOT)/bin/yborm_gen --ddl tutorial/tut3.xml MYSQL script.sql
```

Generate the domain classes using the same tool with different command line option. If you have already done this, but since then the tables have received changes to their columns, you can use the tool to update your domain classes' definitions to reflect the changes made to the schema.
```
 $(YBORM_ROOT)/bin/yborm_gen --domain tutorial/tut3.xml domain/
```

To operate on a database we will need `Engine` object, that will manage all the operations at the SQL level. Object `Engine` needs one of the following to operate properly:
  1. a standalone database connection object `SqlConnection`, it will be used to execute all the SQL statements;
  1. or a pool of database connections `SqlPool`, that can manage the connections on its own, it can give a connection out of pool or create a new one per request.
In the example below the `Engine` object is given a standalone connection. There is `Session` object that works on top of Engine, it keeps track of state for each object, it performs loading and saving fro/to the database as necessary.
```
        auto_ptr<SqlConnection> conn(
                new SqlConnection("mysql://test1_usr:test1_pwd@test1_db"));
        Engine engine(Engine::READ_WRITE, conn);
        Session session(init_schema(), &engine);
```

And now to some querying examples.  **The simplest case: fetch an object by its primary key.** To do this just construct your domain class object with a parameter-primary key value. The domain object will automatically load all of its attributes on first attempt to access any of them. Until this moment the object is in so called "Ghost" state. This way of loading is called "lazy loading". Of course, this may be the case that there is no object in the database with such primary key value as we specified, then an exception will be thrown.
```
        Client client(session, 32738);
        try {
            cout << client.name.value() << endl;
        } catch (NoDataFound &) {
            cerr << "No such client\n";
        }
```
```sql

SQL:
SELECT client.id, client.dt, client.name, client.email, client.budget FROM client WHERE client.id = ?
```

Often we need to select an object by an arbitrary condition, which may be not necessarily filtering by primary key. This is **how one can query for just one Client object with filter by name.**
```
        Client client = query<Client>(session)
            .filter_by(Client::c.name == name).one();
```
```sql

SQL:
SELECT client.id, client.dt, client.name, client.email, client.budget FROM client WHERE client.name = ?
```

The template function `query<R>()` returns a query object `QueryObj`, that we may add filtering and sorting to. We may substitute a domain class for parameter R. To select just one object use method `one()` of `QueryObj` class. In case there's no such object or there are more than one object then a `NoDataFound` exception will be thrown.
To select all of the objects by some criteria use method `all()` of `QueryObj` class, returning an iterable collection. The iteration over the collection is also "lazy", a new object is fetched on iterator de-referencing. **Example: find all unpaid orders having total\_sum more than given one.**
```
        DomainResultSet<Order> rs = query<Order>(session)
            .filter_by(Order::c.total_sum > Decimal(100)
                       && Order::c.receipt_dt == Value()).all();
        BOOST_FOREACH(Order order, rs) {
            cout << order.id.value() << ",";
        }
```
```sql

SQL:
SELECT order_tbl.id, order_tbl.client_id, order_tbl.dt, order_tbl.memo,
order_tbl.total_sum, order_tbl.receipt_sum, order_tbl.receipt_dt
FROM order_tbl WHERE (order_tbl.total_sum > ?) AND (order_tbl.receipt_dt IS NULL)
```

Also there is method `count()` in class `QueryObj`. This call transforms the query into a sub-query then all the rows are counted in the sub-query. This method returns an integer number. **Example: count the number of all orders for given client.**
```
        cout << "Order count: " << query<Order>(session)
            .filter_by(Order::c.client_id == 32738).count() << endl;
```
```sql

SQL:
SELECT COUNT(*) CNT FROM (
SELECT order_tbl.id, order_tbl.client_id, order_tbl.dt, order_tbl.memo, order_tbl.total_sum,
order_tbl.receipt_sum, order_tbl.receipt_dt
FROM order_tbl WHERE order_tbl.client_id = ?) X
```

Sometimes we need more complicated queries involving table joined using foreign keys. In YB.ORM library we may pass as a parameter to the template function `query<R>()` not only a domain class but also a tuple (`boost::tuple`) of several domain classes, their order is important. In the latter case there must exist a declared relation between each two neighbour items in the list, this is necessary to build proper join condition for two corresponding tables. **Let's assume, we need to extract the orders filtered by total\_sum, together with their respective clients. The result must be sorted by client ID, then by order ID.**
```
        typedef tuple<Order, Client> Pair; 
        DomainResultSet<Pair> rs = query<Pair>(session) 
            .filter_by(Order::c.total_sum > Decimal("3.14") 
                       && Client::c.budget != Value())
            .order_by(ExpressionList(Client::c.id, Order::c.id)).all(); 
        BOOST_FOREACH(Pair pair, rs) { 
            cout << pair.get<1>().name.value() 
                 << " " << pair.get<0>().total_sum.value() << endl; 
        } 
```
```sql

SQL:
SELECT order_tbl.id, order_tbl.client_id, order_tbl.dt, order_tbl.memo, order_tbl.total_sum,
order_tbl.receipt_sum, order_tbl.receipt_dt,
client.id, client.dt, client.name, client.email, client.budget
FROM order_tbl JOIN client ON (client.ID = order_tbl.client_id)
WHERE (order_tbl.total_sum > ?) AND (client.budget IS NOT NULL)
ORDER BY client.id, order_tbl.id
```

In this example the lazy collection `rs` will yield object pairs of Client, Order.
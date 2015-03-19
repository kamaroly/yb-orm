## XML format for schema description ##

See also files `*.xml` in the `examples` folder.

The root element of the schema description document is `<schema>`. It can have nested elements `<table>` and `<relation>`.

To describe an entity (a table) and to map it to a class use `<table>` element. In turn, it contains one or more nested `<column>` elements to describe a single table column and to map it to object data member. The following attributes for `<table>` element are accepted:

  * `name` – The name of table, provide as in SQL. This is a _required_ attribute.
  * `class` – The name of class to map the table to, if not set no domain class will be created for the table.
  * `sequence` – To be able to generate unique IDs for newly inserted records some databases (Oracle, Postgres) utilize sequences or generators, some others (e.g. MySQL) – don't. You can safely set here the name of sequence that will be used to generate IDs if the underlying database supports this; for other databases this will mean that the value for the primary key in the table is auto-generated on insertion.
  * `autoinc` – Like the previous, but only suitable for databases with no sequences or generators, like MySQL and SQLite. The value of this attribute is ignored.
  * `xml-name` – The name will be used when a persistent object is serialized in form of XML (look at `include/yb/orm/xmlizer.h`). If not given the table's name is used, translated to lower case, where all `"_"` are replaced with `"-"`.

Use element `<column>` to describe a column/class data member. This element may have the following attributes:

  * `name` – The name of column, provide as in SQL. This is a _required_ attribute.
  * `property` – The name of class member if it must differ from column's name.
  * `type` – The data type of the column. This is a _required_ attribute. YB.ORM has its own set of types, and they are mapped both to SQL types, taking into account dialect specifics, and to C++ types. As of now, the following data types are supported:
    * `"string"` – String of text characters.
    * `"integer"` – 32-bit signed integer.
    * `"longint"` – 64-bit signed integer, suitable for ID generation.
    * `"decimal"` – Precise decimal number with fraction part, suitable for storing money values, etc.
    * `"float"` – Double precision floating point number.
    * `"datetime"` – Time stamp with date and time parts.
  * `size` – The maximum length of the value in that column, applicable to `"string"` type columns only.
  * `null` – The flag denotes the column that may be assigned `NULL`, the possible values are `"true"` and `"false"`. By default it's `"true"` unless the column is marked to be a (part of) primary key, see below.
  * `default` – The option sets the default value for the column. This attribute may contain a constant of type, specified by the `type` attribute, or a special value `"sysdate"` suitable for `"datetime"` type columns. In latter case the current time will be used as the default when a record is created.
  * `xml-name` – The name will be used when a persistent object is serialized in form of XML (look at `include/yb/orm/xmlizer.h`), if not given the column's name is used, translated to lower case, where all `"_"` are replaced with `"-"`. To exclude a column from the serialized representation of the object set this field to special value `"!"`.

The following nested elements may be found inside a `<column>` element, they describe additional constraints:

  * `<read-only>` – The column may only be assigned a value on selection from database or on object creation, any subsequent updates will produce an error.
  * `<primary-key>` – Mark the column as a primary key or as a part of compound primary key.
  * `<foreign-key>` – The column references to another table and column, this is also known as foreign key constraint. To describe the reference this element is given the following attributes:
    * `table` – The name of the table the column references. This is a _required_ attribute.
    * `key` – The column name in the referenced table. This may be omitted if the column referenced is the primary key.

To describe a relationship between domain classes use a top-level element `<relation>`. The contents of this element depends on the type of the relationship, that can be set using a required attribute `type`. As of now, the only accepted value for that attribute is `"one-to-many"`. Other kinds of relationships (e.g. `"many-to-many"`, `"parent-to-child"`) are yet to be implemented. The optional attribute `cascade` defines what action to take for the slave records on an attempt to delete the master record. Possible actions are the following.

  * `"restrict"` – Deny to delete the parent.
  * `"set-null"` – Set to `NULL` the foreign key column in all the child records. To use `"set-null"` the dependent table must have its foreign key column marked as nullable: `null="true"`.
  * `"delete"` – Cascade delete all the child records.

For `"one-to-many"` kind of relationship there are always two sides, each described by one of two required nested elements: `<one>` or `<many>`. The former refers to the master table and class, and the latter refers to the dependent (slave) table and its class. Element `<one>` may have the following attributes:

  * `class` – The class name, mapped to the master table. This is a _required_ attribute.
  * `property` – The name of the property that is added to the master class to reference (typically multiple) dependent objects. This is a _collection_ property unless attribute `use-list` specifies the opposite case. If not specified – no property is added to the master class for that relationship.
  * `use-list` – If there is a property at the master object, referencing its slave objects, then this attribute set to `"false"` instructs to add a _single object_ reference property rather than a collection property. This is particularly useful for modeling relationships of `"one-to-one"` kind.

Element `<many>` for the dependent class may have the following attributes:

  * `class` – The class name, mapped to the dependent table. This is a _required_ attribute.
  * `property` – The name of the property added to the dependent class to reference its master object.
  * `key` – The attribute specifies what column to use as a foreign key, may be useful if there's more than one relationship between the two tables.
  * `order-by` – Here you can put an SQL expression for `ORDER BY` clause to determine the order in which dependent objects will be selected, e.g. when walking through the collection property at the master object side.
# Near future plans #

These features should be implemented in next release or two. Some kind of roadmap:
  * QueryObj: optional explicit .select\_from() clause.
  * QueryObj: explicit .join() clause in Query object.
  * yborm\_gen: it should be possible to specify SQL type for columns explicitly.
  * yborm\_gen: database schema reverse engineering.
  * Support for BLOB data type.
  * SqlConnection: information about character encoding, support automatic encoding conversions.

# Wish list #

These things will be implemented one day. Need to keep in mind:
  * Relationships: implement inheritance, at least three models are possible.
  * Relationships: implement n-to-n relationship using link table, represent it as collection properties.
  * Improve query language, allow for intermixing domain classes and column expressions in query<>().
  * Optional caching of result sets.
  * Library API in common: more possibilities for customization.
  * Comment the source code! Use Doxygen for API docs.
  * Build Windows version as a DLL.
  * May be switch to C++11?
  * and more ...
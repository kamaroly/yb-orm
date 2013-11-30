#include <time.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <util/str_utils.hpp>
#include <util/Singleton.h>
#include <orm/SqlDriver.h>
#if defined(YB_USE_QT)
#include "QtSqlDriver.h"
#endif
#if defined(YB_USE_ODBC)
#include "OdbcDriver.h"
#endif
#if defined(YB_USE_SQLITE3)
#include "SQLiteDriver.h"
#endif
#if defined(YB_USE_SOCI)
#include "SOCIDriver.h"
#endif

#if defined(YB_USE_QT)
typedef Yb::QtSqlDriver DefaultSqlDriver;
#define DEFAULT_DRIVER _T("QODBC3")
#elif defined(YB_USE_ODBC)
typedef Yb::OdbcDriver DefaultSqlDriver;
#define DEFAULT_DRIVER _T("ODBC")
#elif defined(YB_USE_SOCI)
typedef Yb::SOCIDriver DefaultSqlDriver;
#define DEFAULT_DRIVER _T("SOCI")
#elif defined(YB_USE_SQLITE3)
typedef Yb::SQLiteDriver DefaultSqlDriver;
#define DEFAULT_DRIVER _T("SQLITE")
#endif

using namespace std;
using namespace Yb::StrUtils;

#if 0
#define DBG(x) do { char __s[40]; std::ostringstream __log; time_t __t = time(NULL); \
    strcpy(__s, ctime(&__t)); __s[strlen(__s) - 1] = 0; \
    __log << __s << ": " << NARROW(x) << '\n'; \
    std::cerr << __log.str(); } while(0)
#endif

namespace Yb {

DBError::DBError(const String &msg)
    : BaseError(msg)
{}

GenericDBError::GenericDBError(const String &err)
    : DBError(_T("Database error, details: ") + err)
{}

NoDataFound::NoDataFound(const String &msg)
    : DBError(_T("Data wasn't found, details: ") + msg)
{}

BadSQLOperation::BadSQLOperation(const String &msg)
    : DBError(msg)
{}

BadOperationInMode::BadOperationInMode(const String &msg)
    : DBError(msg)
{}

SqlDialectError::SqlDialectError(const String &msg)
    : DBError(msg)
{}

SqlDriverError::SqlDriverError(const String &msg)
    : DBError(msg)
{}

SqlDialect::~SqlDialect() {}

bool SqlDialect::parse_url_tail(const String &url_tail, StringDict &source)
{
    return false;
}

bool SqlDialect::native_driver_eats_slash() { return true; }

const String SqlDialect::select_last_inserted_id(const String &table_name) {
    throw SqlDialectError(_T("No autoincrement flag"));
}

bool SqlDialect::commit_ddl() { return false; }

bool SqlDialect::fk_internal() { return false; }

const String SqlDialect::suffix_create_table() { return String(); }

const String SqlDialect::primary_key_flag() { return String(); }

const String SqlDialect::autoinc_flag() { return String(); }

const String SqlDialect::sysdate_func() {
    return _T("CURRENT_TIMESTAMP");
}

bool SqlDialect::explicit_null() { return false; }

const String SqlDialect::not_null_default(const String &not_null_clause,
        const String &default_value)
{
    if (str_empty(not_null_clause))
        return default_value;
    if (str_empty(default_value))
        return not_null_clause;
    return default_value + _T(" ") + not_null_clause;
}

class OracleDialect: public SqlDialect
{
public:
    OracleDialect()
        : SqlDialect(_T("ORACLE"), _T("DUAL"), true)
    {}
    const String select_curr_value(const String &seq_name)
    { return seq_name + _T(".CURRVAL"); }
    const String select_next_value(const String &seq_name)
    { return seq_name + _T(".NEXTVAL"); }
    const String sql_value(const Value &x)
    {
        if (x.get_type() == Value::DATETIME)
            return _T("timestamp") + x.sql_str();
        return x.sql_str();
    }
    const String type2sql(int t) {
        switch (t) {
            case Value::INTEGER:    return _T("NUMBER(10)");    break;
            case Value::LONGINT:    return _T("NUMBER(20)");    break;
            case Value::STRING:     return _T("VARCHAR2");      break;
            case Value::DATETIME:   return _T("DATE");          break;
            case Value::FLOAT:
            case Value::DECIMAL:    return _T("NUMBER");        break;
        }
        throw SqlDialectError(_T("Bad type"));
    }
    const String create_sequence(const String &seq_name) {
        return _T("CREATE SEQUENCE ") + seq_name;
    }
    const String drop_sequence(const String &seq_name) {
        return _T("DROP SEQUENCE ") + seq_name;
    }
    const String sysdate_func() { return _T("SYSDATE"); }
};

class PostgresDialect: public SqlDialect
{
public:
    PostgresDialect()
        : SqlDialect(_T("POSTGRES"), _T(""), true)
    {}
    const String select_curr_value(const String &seq_name)
    { return _T("CURRVAL('") + seq_name + _T("')"); }
    const String select_next_value(const String &seq_name)
    { return _T("NEXTVAL('") + seq_name + _T("')"); }
    const String sql_value(const Value &x)
    {
        return x.sql_str();
    }
    const String type2sql(int t) {
        switch (t) {
            case Value::INTEGER:    return _T("INTEGER");       break;
            case Value::LONGINT:    return _T("BIGINT");        break;
            case Value::STRING:     return _T("VARCHAR");       break;
            case Value::DECIMAL:    return _T("DECIMAL");       break;
            case Value::DATETIME:   return _T("TIMESTAMP");     break;
            case Value::FLOAT:      return _T("DOUBLE PRECISION"); break;
        }
        throw SqlDialectError(_T("Bad type"));
    }
    const String create_sequence(const String &seq_name) {
        return _T("CREATE SEQUENCE ") + seq_name;
    }
    const String drop_sequence(const String &seq_name) {
        return _T("DROP SEQUENCE ") + seq_name;
    }
};

class InterbaseDialect: public SqlDialect
{
public:
    InterbaseDialect()
        : SqlDialect(_T("INTERBASE"), _T("RDB$DATABASE"), true)
    {}
    const String select_curr_value(const String &seq_name)
    { return _T("GEN_ID(") + seq_name + _T(", 0)"); }
    const String select_next_value(const String &seq_name)
    { return _T("GEN_ID(") + seq_name + _T(", 1)"); }
    const String sql_value(const Value &x)
    {
        return x.sql_str();
    }
    bool commit_ddl() { return true; }
    const String type2sql(int t) {
        switch (t) {
            case Value::INTEGER:    return _T("INTEGER");       break;
            case Value::LONGINT:    return _T("BIGINT");        break;
            case Value::STRING:     return _T("VARCHAR");       break;
            case Value::DECIMAL:    return _T("DECIMAL(16, 6)"); break;
            case Value::DATETIME:   return _T("TIMESTAMP");     break;
            case Value::FLOAT:      return _T("DOUBLE PRECISION"); break;
        }
        throw SqlDialectError(_T("Bad type"));
    }
    const String create_sequence(const String &seq_name) {
        return _T("CREATE GENERATOR ") + seq_name;
    }
    const String drop_sequence(const String &seq_name) {
        return _T("DROP GENERATOR ") + seq_name;
    }
};

class MysqlDialect: public SqlDialect
{
public:
    MysqlDialect()
        : SqlDialect(_T("MYSQL"), _T("DUAL"), false)
    {}
    const String select_curr_value(const String &seq_name)
    { throw SqlDialectError(_T("No sequences, please")); }
    const String select_next_value(const String &seq_name)
    { throw SqlDialectError(_T("No sequences, please")); }
    const String select_last_inserted_id(const String &table_name) {
        return _T("SELECT LAST_INSERT_ID() LID");
    }
    const String sql_value(const Value &x)
    {
        return x.sql_str();
    }
    const String type2sql(int t) {
        switch (t) {
            case Value::INTEGER:    return _T("INT");           break;
            case Value::LONGINT:    return _T("BIGINT");        break;
            case Value::STRING:     return _T("VARCHAR");       break;
            case Value::DECIMAL:    return _T("DECIMAL(16, 6)"); break;
            case Value::DATETIME:   return _T("TIMESTAMP");     break;
            case Value::FLOAT:      return _T("DOUBLE PRECISION"); break;
        }
        throw SqlDialectError(_T("Bad type"));
    }
    const String create_sequence(const String &seq_name) {
        throw SqlDialectError(_T("No sequences, please"));
    }
    const String drop_sequence(const String &seq_name) {
        throw SqlDialectError(_T("No sequences, please"));
    }
    const String suffix_create_table() {
        return _T(" ENGINE=INNODB DEFAULT CHARSET=utf8");
    }
    const String autoinc_flag() { return _T("AUTO_INCREMENT"); }
    bool explicit_null() { return true; }
    const String not_null_default(const String &not_null_clause,
            const String &default_value)
    {
        if (str_empty(not_null_clause))
            return default_value;
        if (str_empty(default_value))
            return not_null_clause;
        return not_null_clause + _T(" ") + default_value;
    }
};

class SQLite3Dialect: public SqlDialect
{
public:
    SQLite3Dialect()
        : SqlDialect(_T("SQLITE"), _T(""), false)
    {}
    bool native_driver_eats_slash() { return false; }
    const String select_curr_value(const String &seq_name)
    { throw SqlDialectError(_T("No sequences, please")); }
    const String select_next_value(const String &seq_name)
    { throw SqlDialectError(_T("No sequences, please")); }
    const String select_last_inserted_id(const String &table_name) {
        return _T("SELECT SEQ LID FROM SQLITE_SEQUENCE WHERE NAME = '")
            + table_name + _T("'");
    }
    const String sql_value(const Value &x)
    {
        return x.sql_str();
    }
    const String type2sql(int t) {
        switch (t) {
            case Value::INTEGER:    return _T("INTEGER");       break;
            case Value::LONGINT:    return _T("INTEGER");       break;
            case Value::STRING:     return _T("VARCHAR");       break;
            case Value::DECIMAL:    return _T("NUMERIC");       break;
            case Value::DATETIME:   return _T("TIMESTAMP");     break;
            case Value::FLOAT:      return _T("DOUBLE PRECISION"); break;
        }
        throw SqlDialectError(_T("Bad type"));
    }
    bool fk_internal() { return true; }
    const String create_sequence(const String &seq_name) {
        throw SqlDialectError(_T("No sequences, please"));
    }
    const String drop_sequence(const String &seq_name) {
        throw SqlDialectError(_T("No sequences, please"));
    }
    const String primary_key_flag() { return _T("PRIMARY KEY"); }
    const String autoinc_flag() { return _T("AUTOINCREMENT"); }
};

typedef SingletonHolder<ItemRegistry<SqlDialect> > theDialectRegistry;

void register_std_dialects()
{
    auto_ptr<SqlDialect> dialect;
    SqlDialect *p;
    dialect.reset((SqlDialect *)new OracleDialect());
    p = dialect.get();
    theDialectRegistry::instance().register_item(
            p->get_name(), dialect);
    dialect.reset((SqlDialect *)new PostgresDialect());
    p = dialect.get();
    theDialectRegistry::instance().register_item(
            p->get_name(), dialect);
    dialect.reset((SqlDialect *)new InterbaseDialect());
    p = dialect.get();
    theDialectRegistry::instance().register_item(
            p->get_name(), dialect);
    dialect.reset((SqlDialect *)new MysqlDialect());
    p = dialect.get();
    theDialectRegistry::instance().register_item(
            p->get_name(), dialect);
    dialect.reset((SqlDialect *)new SQLite3Dialect());
    p = dialect.get();
    theDialectRegistry::instance().register_item(
            p->get_name(), dialect);
}

SqlDialect *sql_dialect(const String &name)
{
    if (theDialectRegistry::instance().empty())
        register_std_dialects();
    SqlDialect *dialect = theDialectRegistry::instance().find_item(name);
    if (!dialect)
        throw SqlDialectError(_T("Unknown dialect: ") + name);
    return dialect;
}

bool register_sql_dialect(auto_ptr<SqlDialect> dialect)
{
    if (theDialectRegistry::instance().empty())
        register_std_dialects();
    SqlDialect *p = dialect.get();
    return theDialectRegistry::instance().register_item(
            p->get_name(), dialect);
}

const Strings list_sql_dialects()
{
    if (theDialectRegistry::instance().empty())
        register_std_dialects();
    return theDialectRegistry::instance().list_items();
}

SqlCursorBackend::~SqlCursorBackend() {}
void SqlCursorBackend::bind_params(const TypeCodes &types) {}
SqlConnectionBackend::~SqlConnectionBackend() {}
SqlDriver::~SqlDriver() {}

void SqlDriver::parse_url_tail(const String &dialect_name,
        const String &url_tail, StringDict &source)
{
    if (sql_dialect(dialect_name)->parse_url_tail(url_tail, source))
        return;
    Yb::StrUtils::parse_url_tail(url_tail, source);
}

// the default policies
bool SqlDriver::explicit_begin_trans_required() { return true; }
bool SqlDriver::numbered_params() { return false; }

const String SqlDriver::convert_to_numbered_params(
        const String &sql)
{
    String first_word;
    vector<int> pos_list;
    if (!find_subst_signs(sql, pos_list, first_word))
        throw DBError(_T("SQL syntax error"));
    vector<String> parts;
    split_by_subst_sign(sql, pos_list, parts);
    String sql2 = parts[0];
    for (int i = 1; i < parts.size(); ++i) {
        sql2 += _T(":") + to_string(i);
        sql2 += parts[i];
    }
    return sql2;
}

typedef SingletonHolder<ItemRegistry<SqlDriver> > theDriverRegistry;

void register_std_drivers()
{
    SqlDriver *p;
#if defined(YB_USE_QT)
    auto_ptr<SqlDriver> driver_qtsql((SqlDriver *)new QtSqlDriver(false));
    p = driver_qtsql.get();
    theDriverRegistry::instance().register_item(p->get_name(), driver_qtsql);
    auto_ptr<SqlDriver> driver_qodbc((SqlDriver *)new QtSqlDriver(true));
    p = driver_qodbc.get();
    theDriverRegistry::instance().register_item(p->get_name(), driver_qodbc);
#endif
#if defined(YB_USE_ODBC)
    auto_ptr<SqlDriver> driver_odbc((SqlDriver *)new OdbcDriver());
    p = driver_odbc.get();
    theDriverRegistry::instance().register_item(p->get_name(), driver_odbc);
#endif
#if defined(YB_USE_SQLITE3)
    auto_ptr<SqlDriver> driver_sqlite3((SqlDriver *)new SQLiteDriver());
    p = driver_sqlite3.get();
    theDriverRegistry::instance().register_item(p->get_name(), driver_sqlite3);
#endif
#if defined(YB_USE_SOCI)
    auto_ptr<SqlDriver> driver_soci((SqlDriver *)new SOCIDriver(false));
    p = driver_soci.get();
    theDriverRegistry::instance().register_item(p->get_name(), driver_soci);
    auto_ptr<SqlDriver> driver_soci_odbc((SqlDriver *)new SOCIDriver(true));
    p = driver_soci_odbc.get();
    theDriverRegistry::instance().register_item(p->get_name(), driver_soci_odbc);
#endif
}

SqlDriver *sql_driver(const String &name)
{
    if (theDriverRegistry::instance().empty())
        register_std_drivers();
    SqlDriver *driver = NULL;
    if (str_empty(name) || name == _T("DEFAULT"))
        driver = theDriverRegistry::instance().find_item(DEFAULT_DRIVER);
    else
        driver = theDriverRegistry::instance().find_item(name);
    if (!driver)
        throw SqlDriverError(_T("Unknown driver: ") + name);
    return driver;
}

bool register_sql_driver(auto_ptr<SqlDriver> driver)
{
    if (theDriverRegistry::instance().empty())
        register_std_drivers();
    SqlDriver *p = driver.get();
    return theDriverRegistry::instance().register_item(
            p->get_name(), driver);
}

const Strings list_sql_drivers()
{
    if (theDriverRegistry::instance().empty())
        register_std_drivers();
    return theDriverRegistry::instance().list_items();
}

SqlSource::SqlSource()
{
    set(_T("&id"), String());
    set(_T("&driver"), String());
    set(_T("&dialect"), String());
    set(_T("&db"), String());
    set(_T("&user"), String());
    set(_T("&passwd"), String());
    set(_T("&host"), String());
    set(_T("&port"), String());
}

static String find_driver_name(
        const String &driver_name, const String &dialect_name)
{
    if (str_empty(driver_name))
        return _T("DEFAULT");
    return str_to_upper(driver_name);
}

SqlSource::SqlSource(const String &url)
{
    String dialect_name, driver_name, url_tail;
    parse_url_proto(url, dialect_name, driver_name, url_tail);
    dialect_name = str_to_upper(dialect_name);
    driver_name = find_driver_name(driver_name, dialect_name);
    sql_driver(driver_name)->parse_url_tail(dialect_name, url_tail, *this);
    set(_T("&dialect"), dialect_name);
    set(_T("&driver"), driver_name);
    if (empty_key(_T("&db"))) {
        if (!empty_key(_T("&host")) &&
                empty_key(_T("&port")) && empty_key(_T("&path")))
            set(_T("&db"), pop(_T("&host"), String()));
        else
            set(_T("&db"), pop(_T("&path"), String()));
    }
    set(_T("&user"), pop(_T("&user"), String()));
    set(_T("&passwd"), pop(_T("&passwd"), String()));
    set(_T("&host"), pop(_T("&host"), String()));
    set(_T("&port"), pop(_T("&port"), String()));
    set(_T("&id"), format(true));
}

SqlSource::SqlSource(const String &id,
        const String &driver, const String &dialect,
        const String &db, const String &user, const String &passwd)
{
    set(_T("&id"), id);
    set(_T("&driver"), driver);
    set(_T("&dialect"), dialect);
    set(_T("&db"), db);
    set(_T("&user"), user);
    set(_T("&passwd"), passwd);
    set(_T("&host"), String());
    set(_T("&port"), String());
}

const String SqlSource::format(bool hide_passwd) const
{
    StringDict params = *(StringDict *)this;
    params[_T("&proto")] = str_to_lower(params[_T("&dialect")]);
    String driver = params[_T("&driver")];
    if (driver != _T("DEFAULT"))
        params[_T("&proto_ext")] = str_to_lower(driver);
    if (look_like_absolute_path(params[_T("&db")]))
        params[_T("&path")] = params[_T("&db")];
    else
        params[_T("&host")] = params[_T("&db")];
    return format_url(params, hide_passwd);
}

const Strings SqlSource::options() const
{
    Strings options;
    Strings k = keys();
    for (size_t i = 0; i < k.size(); ++i)
        if (!starts_with(k[i], _T("&")))
            options.push_back(k[i]);
    return options;
}

bool
SqlResultSet::fetch(Row &row)
{
    RowPtr p = cursor_.fetch_row();
    if (!p.get())
        return false;
    row.swap(*p);
    return true;
}

void
SqlResultSet::own(auto_ptr<SqlCursor> cursor)
{
    owned_cursor_.reset(NULL);
    owned_cursor_.reset(cursor.release());
}

SqlCursor::SqlCursor(SqlConnection &connection)
    : connection_(connection)
    , backend_(connection.backend_->new_cursor().release())
    , echo_(connection.echo_)
    , conv_params_(connection.conv_params_)
    , log_(connection.log_.get())
{}

void
SqlCursor::exec_direct(const String &sql)
{
    try {
        if (echo_)
            debug(_T("exec_direct: ") + sql);
        connection_.activity_ = true;
        backend_->exec_direct(sql);
    }
    catch (const std::exception &e) {
        connection_.mark_bad(e);
        throw;
    }
}

void
SqlCursor::prepare(const String &sql)
{
    try {
        String fixed_sql = sql;
        if (conv_params_ && connection_.driver_->numbered_params())
            fixed_sql = SqlDriver::convert_to_numbered_params(sql);
        if (echo_)
            debug(_T("prepare: ") + fixed_sql);
        connection_.activity_ = true;
        backend_->prepare(fixed_sql);
    }
    catch (const std::exception &e) {
        connection_.mark_bad(e);
        throw;
    }
}

void
SqlCursor::bind_params(const TypeCodes &types)
{
    try {
        if (echo_) {
            String type_names;
            for (size_t i = 0; i < types.size(); ++i) {
                if (i)
                    type_names += _T(", ");
                type_names += Value::get_type_name(types[i]);
            }
            debug(_T("bind: (") + type_names + _T(")"));
        }
        backend_->bind_params(types);
    }
    catch (const std::exception &e) {
        connection_.mark_bad(e);
        throw;
    }
}

SqlResultSet
SqlCursor::exec(const Values &params)
{
    try {
        if (echo_) {
            std::ostringstream out;
            out << "exec prepared:";
            for (unsigned i = 0; i < params.size(); ++i)
                out << " p" << (i + 1) << "=\""
                    << NARROW(params[i].sql_str()) << "\"";
            debug(WIDEN(out.str()));
        }
        connection_.activity_ = true;
        backend_->exec(params);
        return SqlResultSet(*this);
    }
    catch (const std::exception &e) {
        connection_.mark_bad(e);
        throw;
    }
}

RowPtr
SqlCursor::fetch_row()
{
    try {
        RowPtr row = backend_->fetch_row();
        if (row.get()) {
            Row::iterator j = row->begin(), jend = row->end();
            for (; j != jend; ++j) {
                String uname = str_to_upper(j->first);
                using namespace std;
                swap(j->first, uname);
            }
        }
        if (echo_) {
            if (row.get()) {
                std::ostringstream out;
                out << "fetch: ";
                Row::const_iterator j = row->begin(),
                                    jend = row->end();
                for (; j != jend; ++j)
                    out << NARROW(j->first) << "="
                        << NARROW(j->second.sql_str()) << " ";
                debug(WIDEN(out.str()));
            }
            else
                debug(_T("fetch: no more rows"));
        }
        return row;
    }
    catch (const std::exception &e) {
        connection_.mark_bad(e);
        throw;
    }
}

RowsPtr
SqlCursor::fetch_rows(int max_rows)
{
    try {
        RowsPtr rows(new Rows);
        SqlResultSet result(*this);
        copy_no_more_than_n(result.begin(), result.end(),
                            max_rows, back_inserter(*rows));
        return rows;
    }
    catch (const std::exception &e) {
        connection_.mark_bad(e);
        throw;
    }
}

void
SqlConnection::mark_bad(const std::exception &e)
{
    if (!bad_) {
        string s = e.what();
        size_t pos = s.find('\n');
        if (pos != string::npos)
            s = s.substr(0, pos);
        debug(_T("mark connection bad, because of ") + String(WIDEN(s)));
        bad_ = true;
    }
}

SqlConnection::SqlConnection(const String &driver_name,
        const String &dialect_name, const String &db,
        const String &user, const String &passwd)
    : source_(SqlSource(db, driver_name, dialect_name, db, user, passwd))
    , driver_(sql_driver(source_.driver()))
    , dialect_(sql_dialect(source_.dialect()))
    , activity_(false)
    , echo_(false)
    , conv_params_(false)
    , bad_(false)
    , explicit_trans_started_(false)
    , free_since_(0)
{
    source_[_T("&driver")] = driver_->get_name();
    backend_.reset(driver_->create_backend().release());
    backend_->open(dialect_, source_);
}

SqlConnection::SqlConnection(const SqlSource &source)
    : source_(source)
    , driver_(sql_driver(source_.driver()))
    , dialect_(sql_dialect(source_.dialect()))
    , activity_(false)
    , echo_(false)
    , conv_params_(false)
    , bad_(false)
    , explicit_trans_started_(false)
    , free_since_(0)
{
    source_[_T("&driver")] = driver_->get_name();
    backend_.reset(driver_->create_backend().release());
    backend_->open(dialect_, source_);
}

SqlConnection::SqlConnection(const String &url)
    : source_(url)
    , driver_(sql_driver(source_.driver()))
    , dialect_(sql_dialect(source_.dialect()))
    , activity_(false)
    , echo_(false)
    , conv_params_(false)
    , bad_(false)
    , explicit_trans_started_(false)
    , free_since_(0)
{
    source_[_T("&driver")] = driver_->get_name();
    backend_.reset(driver_->create_backend().release());
    backend_->open(dialect_, source_);
}

SqlConnection::~SqlConnection()
{
    bool err = false;
    try {
        clear();
        if (activity_)
            rollback();
    }
    catch (const std::exception &e) { err = true; }
    try {
        backend_->close();
    }
    catch (const std::exception &e) { err = true; }
    if (err)
        debug(_T("error while closing connection"));
}

auto_ptr<SqlCursor>
SqlConnection::new_cursor()
{
    return auto_ptr<SqlCursor>(new SqlCursor(*this));
}

bool
SqlConnection::explicit_transaction_control() const
{
    return driver_->explicit_begin_trans_required();
}

void
SqlConnection::begin_trans_if_necessary()
{
    if (explicit_transaction_control() && !explicit_trans_started_)
    {
        begin_trans();
    }
}

void
SqlConnection::begin_trans()
{
    try {
        debug(_T("begin transaction"));
        backend_->begin_trans();
        explicit_trans_started_ = true;
    }
    catch (const std::exception &e) {
        mark_bad(e);
        throw;
    }
}

void
SqlConnection::commit()
{
    try {
        if (!explicit_transaction_control() || explicit_trans_started_) {
            if (echo_)
                debug(_T("commit"));
            backend_->commit();
        }
        activity_ = false;
        explicit_trans_started_ = false;
    }
    catch (const std::exception &e) {
        mark_bad(e);
        throw;
    }
}

void
SqlConnection::rollback()
{
    try {
        if (!explicit_transaction_control() || explicit_trans_started_) {
            if (echo_)
                debug(_T("rollback"));
            backend_->rollback();
        }
        activity_ = false;
        explicit_trans_started_ = false;
    }
    catch (const std::exception &e) {
        mark_bad(e);
        throw;
    }
}

void
SqlConnection::clear()
{
    try {
        cursor_.reset(NULL);
    }
    catch (const std::exception &e) {
        mark_bad(e);
        throw;
    }
}

void
SqlConnection::exec_direct(const String &sql)
{
    auto_ptr<SqlCursor> cursor;
    try {
        cursor.reset(NULL);
        cursor.reset(new SqlCursor(*this));
    }
    catch (const std::exception &e) {
        mark_bad(e);
        throw;
    }
    cursor->exec_direct(sql);
}

void
SqlConnection::prepare(const String &sql)
{
    try {
        cursor_.reset(NULL);
        cursor_.reset(new SqlCursor(*this));
    }
    catch (const std::exception &e) {
        mark_bad(e);
        throw;
    }
    cursor_->prepare(sql);
}

SqlResultSet
SqlConnection::exec(const Values &params)
{
    YB_ASSERT(cursor_.get());
    SqlResultSet rs = cursor_->exec(params);
    return rs;
}

RowPtr
SqlConnection::fetch_row()
{
    YB_ASSERT(cursor_.get());
    return cursor_->fetch_row();
}

RowsPtr
SqlConnection::fetch_rows(int max_rows)
{
    YB_ASSERT(cursor_.get());
    return cursor_->fetch_rows(max_rows);
}

bool find_subst_signs(const String &sql, vector<int> &pos_list, String &first_word)
{
    enum { NORMAL, MINUS_FOUND, LINE_COMMENT, SLASH_FOUND, COMMENT,
        COMMENT_ASTER_FOUND, IN_QUOT, IN_QUOT_QFOUND, IN_DQUOT } st;
    bool found_first_word = false;
    first_word = String();
    st = NORMAL;
    for (int i = 0; i < sql.size();) {
        Char c = sql[i];
        switch (st) {
        case NORMAL:
            if (!found_first_word) {
                if (is_symbol_of_id(c))
                    first_word += c;
                else
                    if (!is_space(c) || !str_empty(first_word))
                        found_first_word = true;
            }
            switch (char_code(c)) {
            case '-': st = MINUS_FOUND; break;
            case '/': st = SLASH_FOUND; break;
            case '"': st = IN_DQUOT; break;
            case '\'': st = IN_QUOT; break;
            case '?': pos_list.push_back(i); break;
            }
            ++i;
            break;
        case MINUS_FOUND:
            if (c == '-') {
                st = LINE_COMMENT;
                ++i;
            }
            else
                st = NORMAL;
            break;
        case LINE_COMMENT:
            if (c == '\n')
                st = NORMAL;
            ++i;
            break;
        case SLASH_FOUND:
            if (c == '*') {
                st = COMMENT;
                ++i;
            }
            else
                st = NORMAL;
            break;
        case COMMENT:
            if (c == '*')
                st = COMMENT_ASTER_FOUND;
            ++i;
            break;
        case COMMENT_ASTER_FOUND:
            if (c == '/') {
                st = NORMAL;
                ++i;
            }
            else
                st = COMMENT;
            break;
        case IN_QUOT:
            if (c == '\'')
                st = IN_QUOT_QFOUND;
            ++i;
            break;
        case IN_QUOT_QFOUND:
            if (c == '\'') {
                st = IN_QUOT;
                ++i;
            }
            else
                st = NORMAL;
            break;
        case IN_DQUOT:
            if (c == '"')
                st = NORMAL;
            ++i;
            break;
        }
    }
    return st == NORMAL || st == IN_QUOT_QFOUND \
               || st == LINE_COMMENT || st == SLASH_FOUND;
}

void split_by_subst_sign(const String &sql,
        const vector<int> &pos_list, vector<String> &parts)
{
    int prev_pos = -1;
    for (int i = 0; i < pos_list.size(); ++i) {
        int cur_pos = pos_list[i];
        parts.push_back(str_substr(sql, prev_pos + 1, cur_pos - prev_pos - 1));
        prev_pos = cur_pos;
    }
    parts.push_back(str_substr(sql, prev_pos + 1, sql.size() - prev_pos - 1));
}

} // namespace Yb

// vim:ts=4:sts=4:sw=4:et:

// -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; -*-
#ifndef YB__ORM__ENGINE__INCLUDED
#define YB__ORM__ENGINE__INCLUDED

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdexcept>
#include <util/nlogger.h>
#include <orm/Value.h>
#include <orm/Expression.h>
#include <orm/SqlDriver.h>
#include <orm/SqlPool.h>
#include <orm/MetaData.h>

class TestEngine;

namespace Yb {

class EngineBase
{
public:
    virtual ~EngineBase();
    virtual SqlResultSet select_iter(const Expression &select_expr) = 0;
    virtual RowsPtr select(
        const Expression &what,
        const Expression &from,
        const Expression &where = Expression(),
        const Expression &group_by = Expression(),
        const Expression &having = Expression(),
        const Expression &order_by = Expression(),
        int max_rows = -1,
        bool for_update = false) = 0;
    virtual const std::vector<LongInt> insert(
        const String &table_name,
        const Rows &rows,
        const StringSet &exclude_fields = StringSet(),
        bool collect_new_ids = false) = 0;
    virtual void update(const Table &table, const RowsData &rows) = 0;
    virtual void delete_from(
        const String &table_name,
        const Filter &where) = 0;
    virtual void delete_from(
        const String &table_name,
        const Keys &keys) = 0;
    virtual void exec_proc(
        const String &proc_code) = 0;
    virtual void commit() = 0;
    virtual void rollback() = 0;
    virtual LongInt get_curr_value(const String &seq_name) = 0;
    virtual LongInt get_next_value(const String &seq_name) = 0;
    virtual SqlDialect *get_dialect() = 0;
    virtual ILogger *logger() = 0;
    virtual std::auto_ptr<EngineBase> clone() = 0;
};

const String env_cfg(const String &entry,
        const String &def_val = _T(""));

class Engine
    : public EngineBase, private NonCopyable
{
    friend class ::TestEngine;
public:
    enum Mode { READ_ONLY = 0, READ_WRITE = 1, MANUAL = 1 }; 

    static SqlSource sql_source_from_env(const String &id = _T(""));
    Engine(Mode work_mode = READ_WRITE);
    Engine(Mode work_mode, std::auto_ptr<SqlConnection> conn);
    Engine(Mode work_mode, std::auto_ptr<SqlPool> pool,
           const String &source_id, int timeout = YB_POOL_WAIT_TIME);
    Engine(Mode work_mode, SqlDialect *dialect); // for subclassing

    virtual ~Engine();

    SqlConnection *get_connection() { return conn_.get(); }
    SqlDialect *get_dialect() { return dialect_; }
    std::auto_ptr<EngineBase> clone();
    void touch();
    bool activity() { return get_conn()->activity(); }
    void set_echo(bool echo);
    void set_logger(ILogger::Ptr logger);
    ILogger *logger() { return logger_ptr_? logger_ptr_: logger_.get(); }

    // SQL operator wrappers
    SqlResultSet select_iter(const Expression &select_expr);
    RowsPtr select(
            const Expression &what,
            const Expression &from,
            const Expression &where = Expression(),
            const Expression &group_by = Expression(),
            const Expression &having = Expression(),
            const Expression &order_by = Expression(),
            int max_rows = -1,
            bool for_update = false);
    const std::vector<LongInt> insert(const String &table_name,
            const Rows &rows, const StringSet &exclude_fields = StringSet(),
            bool collect_new_ids = false);
    void update(const Table &table, const RowsData &rows);
    void delete_from(const String &table_name, const Filter &where);
    void delete_from(const String &table_name, const Keys &keys);
    void exec_proc(const String &proc_code);
    void commit();
    void rollback();

    // Convenience utility methods
    RowPtr select_row(const Expression &what,
            const Expression &from, const Expression &where);
    const Value select1(const Expression &what,
            const Expression &from, const Expression &where);
    LongInt get_curr_value(const String &seq_name);
    LongInt get_next_value(const String &seq_name);

    /* Use cases:
    Yb::EngineBase db;
    int count = db.select1(_T("count(1)"), _T("t_manager"),
        Yb::FilterEq(_T("hidden"), Yb::Value(1))).as_longint();
    Yb::RowPtr row = db.select_row(_T("v_ui_contract"),
        Yb::FilterEq(_T("contract_id"), Yb::Value(contract_id)));
    */

private:
    // clone support
    Engine(Mode work_mode, Engine *master, bool echo, ILogger *logger,
           SqlConnection *conn);
    Engine(Mode work_mode, Engine *master, bool echo, ILogger *logger,
           SqlPool *pool, const String &source_id, int timeout);

    SqlPool *get_pool();
    SqlConnection *get_conn(bool strict = true);

    virtual const std::vector<LongInt> on_insert(
            const String &table_name,
            const Rows &rows, const StringSet &exclude_fields,
            bool collect_new_ids);
    virtual void on_update(const Table &table, const RowsData &rows);
    virtual void on_delete(const String &table_name,
            const Filter &where);
    virtual void on_delete(const String &table_name,
            const Keys &keys);
    virtual void on_exec_proc(const String &proc_code);
    virtual void on_commit();
    virtual void on_rollback();

    virtual void do_gen_sql_insert(String &sql, Values &params,
            ParamNums &param_nums, const String &table_name,
            const Row &row, const StringSet &exclude_fields) const;
    virtual void do_gen_sql_update(String &sql, Values &params,
            ParamNums &param_nums, const Table &table) const;
    virtual void do_gen_sql_delete(String &sql, Values &params,
            const String &table, const Filter &where) const;

private:
    Engine *master_ptr_;
    bool echo_;
    Mode mode_;
    ILogger::Ptr logger_;
    std::auto_ptr<SqlPool> pool_;
    String source_id_;
    int timeout_;
    std::auto_ptr<SqlConnection> conn_;
    ILogger *logger_ptr_;
    SqlPool *pool_ptr_;
    SqlConnection *conn_ptr_;
    SqlDialect *dialect_;
};

} // namespace Yb

// vim:ts=4:sts=4:sw=4:et:
#endif // YB__ORM__ENGINE__INCLUDED

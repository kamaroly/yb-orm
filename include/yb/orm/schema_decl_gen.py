
'''
Run this script to produce header file "schema_decl.h":
    python schema_decl_gen.py > schema_decl.h
You can adjust the depth as necessary.
'''

DEPTH = 42
NPARAM = 10

def gen_macro_set(name, i, nparams, prefix, end_label, labels):
    def mname(label, num=i):
        return '%s%s_%d_%s' % (prefix, name, num, label)
    next_level_mname = '%s%s_%d' % (prefix, name, i + 1)
    params = ', '.join('p%d' % (j + 1) for j in range(nparams))
    macros = ''
    for (key, value) in labels.items():
        body = ''
        if value:
            do_mname = '%s%s_DO_%s' % (prefix, name, key)
            body = '%s(%d, %s)' % (do_mname, i, params)
        macros += '#define %s(%s) \\\n    %s%s\n' % (
                mname(key), params, body and (body + ' ') or '', next_level_mname)
    end_macro = '#define %s(%s)\n' % (mname(end_label), params)
    switch_mname = '%s%s%s' % (prefix, name, (i > 0 and ('_%d' % i) or ''))
    target_mname = '%s%s_%d_##kind' % (prefix, name, i)
    switch_macro = '#define %s(kind, %s) \\\n    %s(%s)\n' % (
            switch_mname, params, target_mname, params)
    return macros + end_macro + switch_macro

def gen_macros(name, n, nparams, prefix, end_label, labels):
    return '\n'.join(gen_macro_set(name, i, nparams, prefix, end_label, labels)
            for i in range(n - 1, -1, -1))

def gen_supp_macro(i):
    return (
            ('#define YB_COLON_OR_COMMA_%d %s\n' % (i, i == 0 and ':' or ',')) +
            ('#define YB_EMPTY_OR_COMMA_%d %s' % (i, i > 0 and ',' or ''))
            )

def gen_supp_macros(n):
    return '\n'.join(gen_supp_macro(i) for i in range(n)) + (
            '\n#define YB_COLON_OR_COMMA(i) YB_COLON_OR_COMMA_##i'
            '\n#define YB_EMPTY_OR_COMMA(i) YB_EMPTY_OR_COMMA_##i')

print r'''// -*- Mode: C++; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*-
#ifndef YB__ORM__SCHEMA_DECL__INCLUDED
#define YB__ORM__SCHEMA_DECL__INCLUDED

/*
 * Do not edit this file, use schema_decl_gen.py script instead.
 * Look here to learn more about the technique:
 * http://stackoverflow.com/questions/2361665/c-c-macro-how-to-generate-two-separate-sections-of-code-with-one-macro-boost?rq=1
 */

// Generic macro for declaring a column
#define YB_COL(prop_name, name, type, size, flags, default_value, fk_table, fk, xml_name, index_name) \
    (YB_COL_, prop_name, _T(name), type, size, flags, default_value, _T(fk_table), _T(fk), _T(xml_name), _T(index_name))

#define YB_COL_END \
    (YB_COL_END_, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

// Convenience macros for declaring most common cases
#define YB_COL_PK(prop_name, name) \
    YB_COL(prop_name, name, LONGINT, 0, Yb::Column::PK, YB_NULL, "", "", "", "")

#define YB_COL_FK(prop_name, name, fk_table, fk) \
    YB_COL(prop_name, name, LONGINT, 0, 0, YB_NULL, fk_table, fk, "", "")

#define YB_COL_FK_NULLABLE(prop_name, name, fk_table, fk) \
    YB_COL(prop_name, name, LONGINT, 0, Yb::Column::NULLABLE, YB_NULL, fk_table, fk, "", "")

#define YB_COL_DATA(prop_name, name, type) \
    YB_COL(prop_name, name, type, 0, Yb::Column::NULLABLE, YB_NULL, "", "", "", "")

#define YB_COL_STR(prop_name, name, size) \
    YB_COL(prop_name, name, STRING, size, Yb::Column::NULLABLE, YB_NULL, "", "", "", "")

// Generic macros for declaring sides of a one-to-many relation
#define YB_REL_ONE(one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by) \
    (YB_REL_ONE_, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, 9, 10)

#define YB_REL_MANY(one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by) \
    (YB_REL_MANY_, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, 9, 10)

// types
#define YB_CPP_TYPE_INTEGER int
#define YB_CPP_TYPE_LONGINT Yb::LongInt
#define YB_CPP_TYPE_DECIMAL Yb::Decimal
#define YB_CPP_TYPE_FLOAT double
#define YB_CPP_TYPE_DATETIME Yb::DateTime
#define YB_CPP_TYPE_STRING Yb::String
#define YB_CPP_TYPE(type__) YB_CPP_TYPE_##type__

// DO macros
#define YB_COL_MEMBERS_DO_YB_COL_(n, prop_name, name, type, size, flags, default_value, fk_table, fk, xml_name, index_name) \
    YB_EMPTY_OR_COMMA(n) prop_name
#define YB_COL_ADD_DO_YB_COL_(n, prop_name, name, type, size, flags, default_value, fk_table, fk, xml_name, index_name) \
    << prop_name
#define YB_COL_MEMB_CONS_DO_YB_COL_(n, prop_name, name, type, size, flags, default_value, fk_table, fk, xml_name, index_name) \
    YB_COLON_OR_COMMA(n) prop_name(name, Yb::Value::type, size, flags, default_value, fk_table, fk, xml_name, _T(#prop_name), index_name)
#define YB_PROP_DECL_DO_YB_COL_(n, prop_name, name, type, size, flags, default_value, fk_table, fk, xml_name, index_name) \
    Yb::Property<YB_CPP_TYPE(type), n> prop_name;
#define YB_PROP_DECL_DO_YB_REL_ONE_(n, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, p9, p10) \
    Yb::ManagedList<many_class> many_prop;
#define YB_PROP_DECL_DO_YB_REL_MANY_(n, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, p9, p10) \
    one_class::Holder one_prop;
#define YB_PROP_CONS_DO_YB_COL_(n, prop_name, name, type, size, flags, default_value, fk_table, fk, xml_name, index_name) \
    , prop_name(this)
#define YB_PROP_CONS_DO_YB_REL_ONE_(n, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, p9, p10) \
    , many_prop(this, _T(#many_prop))
#define YB_PROP_CONS_DO_YB_REL_MANY_(n, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, p9, p10) \
    , one_prop(this, _T(#one_prop))
#define YB_PROP_DEFAULT_DO_YB_COL_(n, prop_name, name, type, size, flags, default_value, fk_table, fk, xml_name, index_name) \
        set(n, Yb::Value(default_value));
#define YB_CREATE_RELS_DO_YB_REL_ONE_(n, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, p9, p10) \
        { \
            Yb::Relation::AttrMap attr1, attr2; \
            attr1[_T("property")] = _T(#many_prop); \
            attr1[_T("use-list")] = _T(#use_list); \
            attr1[_T("order-by")] = order_by; \
            attr2[_T("property")] = _T(#one_prop); \
            attr2[_T("key")] = key; \
            Yb::Relation::Ptr r(new Yb::Relation(Yb::Relation::ONE2MANY, \
                _T(#one_class), attr1, _T(#many_class), attr2, cascade)); \
            rels.push_back(r); \
        }
#define YB_CREATE_RELS_DO_YB_REL_MANY_(n, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, p9, p10) \
        YB_CREATE_RELS_DO_YB_REL_ONE_(n, one_class, one_prop, many_class, many_prop, cascade, key, use_list, order_by, p9, p10)


// Root macros
#define YB_DECLARE(class_name__, table_name__, seq_name__, table_xml_name__, col_defs__) \
private: \
    static Yb::Tables tbls; \
    static Yb::Relations rels; \
    static Yb::DomainMetaDataCreator<class_name__> mdc; \
public: \
    typedef Yb::DomainObjHolder<class_name__> Holder; \
    struct Columns { \
        Yb::Column \
        YB_COL_MEMBERS col_defs__ \
        ; \
        Columns() \
            YB_COL_MEMB_CONS col_defs__ \
        {} \
        void fill_table(Yb::Table &tbl) \
        { \
            tbl \
            YB_COL_ADD col_defs__ \
            ; \
        } \
    }; \
    static Columns c; \
    static const Yb::String get_table_name() { return _T(table_name__); } \
    typedef Yb::DomainResultSet<class_name__> ResultSet; \
    typedef std::vector<class_name__> List; \
    typedef std::auto_ptr<List> ListPtr; \
    static ListPtr find(Yb::Session &session, \
            const Yb::Expression &filter, const Yb::Expression &order_by = Yb::Expression()) \
    { \
        class_name__::ListPtr lst(new class_name__::List()); \
        Yb::ObjectList rows; \
        session.load_collection(rows, Yb::Expression(_T(table_name__)), filter, order_by); \
        if (rows.size()) { \
            Yb::ObjectList::iterator it = rows.begin(), end = rows.end(); \
            for (; it != end; ++it) \
                lst->push_back(class_name__(*it)); \
        } \
        return lst; \
    } \
    class_name__(Yb::DomainObject *owner, const Yb::String &prop_name) \
        : Yb::DomainObject(*tbls[0], owner, prop_name) \
        YB_PROP_CONS col_defs__ \
    {} \
    class_name__() \
        : Yb::DomainObject(*tbls[0]) \
        YB_PROP_CONS col_defs__ \
    { \
        YB_PROP_DEFAULT col_defs__ \
    } \
    class_name__(const class_name__ &other) \
        : Yb::DomainObject(other) \
        YB_PROP_CONS col_defs__ \
    {} \
    explicit class_name__(Yb::Session &session) \
        : Yb::DomainObject(session.schema(), _T(table_name__)) \
        YB_PROP_CONS col_defs__ \
    { \
        YB_PROP_DEFAULT col_defs__ \
        save(session); \
    } \
    explicit class_name__(Yb::DataObject::Ptr d) \
        : Yb::DomainObject(d) \
        YB_PROP_CONS col_defs__ \
    {} \
    class_name__(Yb::Session &session, const Yb::Key &key) \
        : Yb::DomainObject(session, key) \
        YB_PROP_CONS col_defs__ \
    {} \
    class_name__(Yb::Session &session, Yb::LongInt id) \
        : Yb::DomainObject(session, _T(table_name__), id) \
        YB_PROP_CONS col_defs__ \
    {} \
    class_name__ &operator=(const class_name__ &other) \
    { \
        if (this != &other) { \
            *(Yb::DomainObject *)this = (const Yb::DomainObject &)other; \
        } \
        return *this; \
    } \
    static void create_tables_meta(Yb::Tables &tbls) \
    { \
        Yb::Table::Ptr t(new Yb::Table(_T(table_name__), _T(table_xml_name__), _T(#class_name__))); \
        t->set_seq_name(_T(seq_name__)); \
        c.fill_table(*t); \
        tbls.push_back(t); \
    } \
    static void create_relations_meta(Yb::Relations &rels) \
    { \
        YB_CREATE_RELS col_defs__ \
    } \
    YB_PROP_DECL col_defs__ \
    struct Registrator \
    { \
        static void register_domain() { \
            Yb::theDomainFactory().register_creator(_T(table_name__), \
                Yb::CreatorPtr(new Yb::DomainCreator<class_name__>())); \
        } \
        Registrator() { register_domain(); } \
    }; \
    static Registrator registrator;

#define YB_DEFINE(class_name__) \
class_name__::Columns class_name__::c; \
Yb::Tables class_name__::tbls; \
Yb::Relations class_name__::rels; \
Yb::DomainMetaDataCreator<class_name__> class_name__::mdc(class_name__::tbls, class_name__::rels); \
class_name__::Registrator class_name__::registrator;

'''

print gen_supp_macros(DEPTH)

print gen_macros('COL_MEMBERS', DEPTH, NPARAM, prefix='YB_', end_label='YB_COL_END_',
        labels={'YB_COL_': 1, 'YB_REL_ONE_': 0, 'YB_REL_MANY_': 0})

print gen_macros('COL_ADD', DEPTH, NPARAM, prefix='YB_', end_label='YB_COL_END_',
        labels={'YB_COL_': 1, 'YB_REL_ONE_': 0, 'YB_REL_MANY_': 0})

print gen_macros('COL_MEMB_CONS', DEPTH, NPARAM, prefix='YB_', end_label='YB_COL_END_',
        labels={'YB_COL_': 1, 'YB_REL_ONE_': 0, 'YB_REL_MANY_': 0})

print gen_macros('PROP_DECL', DEPTH, NPARAM, prefix='YB_', end_label='YB_COL_END_',
        labels={'YB_COL_': 1, 'YB_REL_ONE_': 1, 'YB_REL_MANY_': 1})

print gen_macros('PROP_CONS', DEPTH, NPARAM, prefix='YB_', end_label='YB_COL_END_',
        labels={'YB_COL_': 1, 'YB_REL_ONE_': 1, 'YB_REL_MANY_': 1})

print gen_macros('PROP_DEFAULT', DEPTH, NPARAM, prefix='YB_', end_label='YB_COL_END_',
        labels={'YB_COL_': 1, 'YB_REL_ONE_': 0, 'YB_REL_MANY_': 0})

print gen_macros('CREATE_RELS', DEPTH, NPARAM, prefix='YB_', end_label='YB_COL_END_',
        labels={'YB_COL_': 0, 'YB_REL_ONE_': 1, 'YB_REL_MANY_': 1})

print r'''
// vim:ts=4:sts=4:sw=4:et:
#endif // YB__ORM__SCHEMA_DECL__INCLUDED'''


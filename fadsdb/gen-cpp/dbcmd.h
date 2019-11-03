#ifndef _dbcmd_778180924_H_
#define _dbcmd_778180924_H_

#include <string>
#include <bitset>
#include <stdint.h>
#include "proto/fads-message.h"
#include "proto/fads-message.h"
#include "proto/soce-proto.h"
#include "proto/type-tree.h"

using std::string;
using std::bitset;

namespace soce{ namespace fadsdb{ 

typedef enum{
    kCmdCreate,
    kCmdInsert,
    kCmdRemove,
    kCmdUpdate,
    kCmdSelect,
    kCmdSelUp,
}CmdType;

class HeaderTypeTree{
public:
    HeaderTypeTree(soce::proto::TypeTree& type_tree);
};

class Header : public soce::fads::FadsMessage{
public:
    void set_type(const CmdType& _type){
        type = _type;
        status_[0] = true;
    }
    void set_type(CmdType&& _type){
        type = std::move(_type);
        status_[0] = true;
    }
    const CmdType& get_type() const {
        return type;
    }
    CmdType& mutable_type(){
        status_[0] = true;
        return type;
    }
    bool has_type() const{
        return status_[0];
    }

    void set_id(const int64_t& _id){
        id = _id;
        status_[1] = true;
    }
    void set_id(int64_t&& _id){
        id = std::move(_id);
        status_[1] = true;
    }
    const int64_t& get_id() const {
        return id;
    }
    int64_t& mutable_id(){
        status_[1] = true;
        return id;
    }
    bool has_id() const{
        return status_[1];
    }

public:
    /* operators */
    Header() = default;
    Header(const Header&) = default;
    Header& operator=(const Header& rhs) = default;
    bool operator==(const Header& rhs) const{
        if (type != rhs.type)
            return false;
        if (id != rhs.id)
            return false;
        return true;
    }

    bool operator!=(const Header& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Header& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<int32_t>()(type)
            ^ std::hash<int64_t>()(id);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    CmdType type;
    int64_t id;

    /* for optional */
    bitset<2> attrs_ = 3;
    bitset<2> status_;

public:
    static HeaderTypeTree s_type_tree;
};

struct hash_Header
{
    size_t operator()(const Header& rhs) const{
        return rhs(rhs);
    }
};

class FadsDbCreateTypeTree{
public:
    FadsDbCreateTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbCreate : public soce::fads::FadsMessage{
public:
    void set_header(const Header& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(Header&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const Header& get_header() const {
        return header;
    }
    Header& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

    void set_schema(const string& _schema){
        schema = _schema;
        status_[1] = true;
    }
    void set_schema(string&& _schema){
        schema = std::move(_schema);
        status_[1] = true;
    }
    const string& get_schema() const {
        return schema;
    }
    string& mutable_schema(){
        status_[1] = true;
        return schema;
    }
    bool has_schema() const{
        return status_[1];
    }

public:
    /* operators */
    FadsDbCreate() = default;
    FadsDbCreate(const FadsDbCreate&) = default;
    FadsDbCreate& operator=(const FadsDbCreate& rhs) = default;
    bool operator==(const FadsDbCreate& rhs) const{
        if (header != rhs.header)
            return false;
        if (schema != rhs.schema)
            return false;
        return true;
    }

    bool operator!=(const FadsDbCreate& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbCreate& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(schema);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    Header header;
    string schema;

    /* for optional */
    bitset<2> attrs_ = 3;
    bitset<2> status_;

public:
    static FadsDbCreateTypeTree s_type_tree;
};

struct hash_FadsDbCreate
{
    size_t operator()(const FadsDbCreate& rhs) const{
        return rhs(rhs);
    }
};

class FadsDbInsertTypeTree{
public:
    FadsDbInsertTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbInsert : public soce::fads::FadsMessage{
public:
    void set_header(const Header& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(Header&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const Header& get_header() const {
        return header;
    }
    Header& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

    void set_key(const string& _key){
        key = _key;
        status_[1] = true;
    }
    void set_key(string&& _key){
        key = std::move(_key);
        status_[1] = true;
    }
    const string& get_key() const {
        return key;
    }
    string& mutable_key(){
        status_[1] = true;
        return key;
    }
    bool has_key() const{
        return status_[1];
    }

    void set_table(const string& _table){
        table = _table;
        status_[2] = true;
    }
    void set_table(string&& _table){
        table = std::move(_table);
        status_[2] = true;
    }
    const string& get_table() const {
        return table;
    }
    string& mutable_table(){
        status_[2] = true;
        return table;
    }
    bool has_table() const{
        return status_[2];
    }

    void set_data(const string& _data){
        data = _data;
        status_[3] = true;
    }
    void set_data(string&& _data){
        data = std::move(_data);
        status_[3] = true;
    }
    const string& get_data() const {
        return data;
    }
    string& mutable_data(){
        status_[3] = true;
        return data;
    }
    bool has_data() const{
        return status_[3];
    }

public:
    /* operators */
    FadsDbInsert() = default;
    FadsDbInsert(const FadsDbInsert&) = default;
    FadsDbInsert& operator=(const FadsDbInsert& rhs) = default;
    bool operator==(const FadsDbInsert& rhs) const{
        if (header != rhs.header)
            return false;
        if (key != rhs.key)
            return false;
        if (table != rhs.table)
            return false;
        if (data != rhs.data)
            return false;
        return true;
    }

    bool operator!=(const FadsDbInsert& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbInsert& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(key)
            ^ std::hash<string>()(table)
            ^ std::hash<string>()(data);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    Header header;
    string key;
    string table;
    string data;

    /* for optional */
    bitset<4> attrs_ = 15;
    bitset<4> status_;

public:
    static FadsDbInsertTypeTree s_type_tree;
};

struct hash_FadsDbInsert
{
    size_t operator()(const FadsDbInsert& rhs) const{
        return rhs(rhs);
    }
};

class FadsDbRemoveTypeTree{
public:
    FadsDbRemoveTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbRemove : public soce::fads::FadsMessage{
public:
    void set_header(const Header& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(Header&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const Header& get_header() const {
        return header;
    }
    Header& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

    void set_key(const string& _key){
        key = _key;
        status_[1] = true;
    }
    void set_key(string&& _key){
        key = std::move(_key);
        status_[1] = true;
    }
    const string& get_key() const {
        return key;
    }
    string& mutable_key(){
        status_[1] = true;
        return key;
    }
    bool has_key() const{
        return status_[1];
    }

    void set_table(const string& _table){
        table = _table;
        status_[2] = true;
    }
    void set_table(string&& _table){
        table = std::move(_table);
        status_[2] = true;
    }
    const string& get_table() const {
        return table;
    }
    string& mutable_table(){
        status_[2] = true;
        return table;
    }
    bool has_table() const{
        return status_[2];
    }

public:
    /* operators */
    FadsDbRemove() = default;
    FadsDbRemove(const FadsDbRemove&) = default;
    FadsDbRemove& operator=(const FadsDbRemove& rhs) = default;
    bool operator==(const FadsDbRemove& rhs) const{
        if (header != rhs.header)
            return false;
        if (key != rhs.key)
            return false;
        if (table != rhs.table)
            return false;
        return true;
    }

    bool operator!=(const FadsDbRemove& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbRemove& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(key)
            ^ std::hash<string>()(table);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    Header header;
    string key;
    string table;

    /* for optional */
    bitset<3> attrs_ = 7;
    bitset<3> status_;

public:
    static FadsDbRemoveTypeTree s_type_tree;
};

struct hash_FadsDbRemove
{
    size_t operator()(const FadsDbRemove& rhs) const{
        return rhs(rhs);
    }
};

class FadsDbUpdateTypeTree{
public:
    FadsDbUpdateTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbUpdate : public soce::fads::FadsMessage{
public:
    void set_header(const Header& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(Header&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const Header& get_header() const {
        return header;
    }
    Header& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

    void set_key(const string& _key){
        key = _key;
        status_[1] = true;
    }
    void set_key(string&& _key){
        key = std::move(_key);
        status_[1] = true;
    }
    const string& get_key() const {
        return key;
    }
    string& mutable_key(){
        status_[1] = true;
        return key;
    }
    bool has_key() const{
        return status_[1];
    }

    void set_table(const string& _table){
        table = _table;
        status_[2] = true;
    }
    void set_table(string&& _table){
        table = std::move(_table);
        status_[2] = true;
    }
    const string& get_table() const {
        return table;
    }
    string& mutable_table(){
        status_[2] = true;
        return table;
    }
    bool has_table() const{
        return status_[2];
    }

    void set_cmd(const string& _cmd){
        cmd = _cmd;
        status_[3] = true;
    }
    void set_cmd(string&& _cmd){
        cmd = std::move(_cmd);
        status_[3] = true;
    }
    const string& get_cmd() const {
        return cmd;
    }
    string& mutable_cmd(){
        status_[3] = true;
        return cmd;
    }
    bool has_cmd() const{
        return status_[3];
    }

    void set_filters(const string& _filters){
        filters = _filters;
        status_[4] = true;
    }
    void set_filters(string&& _filters){
        filters = std::move(_filters);
        status_[4] = true;
    }
    const string& get_filters() const {
        return filters;
    }
    string& mutable_filters(){
        status_[4] = true;
        return filters;
    }
    bool has_filters() const{
        return status_[4];
    }

public:
    /* operators */
    FadsDbUpdate() = default;
    FadsDbUpdate(const FadsDbUpdate&) = default;
    FadsDbUpdate& operator=(const FadsDbUpdate& rhs) = default;
    bool operator==(const FadsDbUpdate& rhs) const{
        if (header != rhs.header)
            return false;
        if (key != rhs.key)
            return false;
        if (table != rhs.table)
            return false;
        if (cmd != rhs.cmd)
            return false;
        if (filters != rhs.filters)
            return false;
        return true;
    }

    bool operator!=(const FadsDbUpdate& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbUpdate& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(key)
            ^ std::hash<string>()(table)
            ^ std::hash<string>()(cmd)
            ^ std::hash<string>()(filters);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    Header header;
    string key;
    string table;
    string cmd;
    string filters;

    /* for optional */
    bitset<5> attrs_ = 15;
    bitset<5> status_;

public:
    static FadsDbUpdateTypeTree s_type_tree;
};

struct hash_FadsDbUpdate
{
    size_t operator()(const FadsDbUpdate& rhs) const{
        return rhs(rhs);
    }
};

class FadsDbSelectTypeTree{
public:
    FadsDbSelectTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbSelect : public soce::fads::FadsMessage{
public:
    void set_header(const Header& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(Header&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const Header& get_header() const {
        return header;
    }
    Header& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

    void set_key(const string& _key){
        key = _key;
        status_[1] = true;
    }
    void set_key(string&& _key){
        key = std::move(_key);
        status_[1] = true;
    }
    const string& get_key() const {
        return key;
    }
    string& mutable_key(){
        status_[1] = true;
        return key;
    }
    bool has_key() const{
        return status_[1];
    }

    void set_table(const string& _table){
        table = _table;
        status_[2] = true;
    }
    void set_table(string&& _table){
        table = std::move(_table);
        status_[2] = true;
    }
    const string& get_table() const {
        return table;
    }
    string& mutable_table(){
        status_[2] = true;
        return table;
    }
    bool has_table() const{
        return status_[2];
    }

    void set_fields(const string& _fields){
        fields = _fields;
        status_[3] = true;
    }
    void set_fields(string&& _fields){
        fields = std::move(_fields);
        status_[3] = true;
    }
    const string& get_fields() const {
        return fields;
    }
    string& mutable_fields(){
        status_[3] = true;
        return fields;
    }
    bool has_fields() const{
        return status_[3];
    }

    void set_filters(const string& _filters){
        filters = _filters;
        status_[4] = true;
    }
    void set_filters(string&& _filters){
        filters = std::move(_filters);
        status_[4] = true;
    }
    const string& get_filters() const {
        return filters;
    }
    string& mutable_filters(){
        status_[4] = true;
        return filters;
    }
    bool has_filters() const{
        return status_[4];
    }

public:
    /* operators */
    FadsDbSelect() = default;
    FadsDbSelect(const FadsDbSelect&) = default;
    FadsDbSelect& operator=(const FadsDbSelect& rhs) = default;
    bool operator==(const FadsDbSelect& rhs) const{
        if (header != rhs.header)
            return false;
        if (key != rhs.key)
            return false;
        if (table != rhs.table)
            return false;
        if (fields != rhs.fields)
            return false;
        if (filters != rhs.filters)
            return false;
        return true;
    }

    bool operator!=(const FadsDbSelect& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbSelect& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(key)
            ^ std::hash<string>()(table)
            ^ std::hash<string>()(fields)
            ^ std::hash<string>()(filters);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    Header header;
    string key;
    string table;
    string fields;
    string filters;

    /* for optional */
    bitset<5> attrs_ = 15;
    bitset<5> status_;

public:
    static FadsDbSelectTypeTree s_type_tree;
};

struct hash_FadsDbSelect
{
    size_t operator()(const FadsDbSelect& rhs) const{
        return rhs(rhs);
    }
};

class FadsDbSelUpTypeTree{
public:
    FadsDbSelUpTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbSelUp : public soce::fads::FadsMessage{
public:
    void set_header(const Header& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(Header&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const Header& get_header() const {
        return header;
    }
    Header& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

    void set_key(const string& _key){
        key = _key;
        status_[1] = true;
    }
    void set_key(string&& _key){
        key = std::move(_key);
        status_[1] = true;
    }
    const string& get_key() const {
        return key;
    }
    string& mutable_key(){
        status_[1] = true;
        return key;
    }
    bool has_key() const{
        return status_[1];
    }

    void set_table(const string& _table){
        table = _table;
        status_[2] = true;
    }
    void set_table(string&& _table){
        table = std::move(_table);
        status_[2] = true;
    }
    const string& get_table() const {
        return table;
    }
    string& mutable_table(){
        status_[2] = true;
        return table;
    }
    bool has_table() const{
        return status_[2];
    }

    void set_fields(const string& _fields){
        fields = _fields;
        status_[3] = true;
    }
    void set_fields(string&& _fields){
        fields = std::move(_fields);
        status_[3] = true;
    }
    const string& get_fields() const {
        return fields;
    }
    string& mutable_fields(){
        status_[3] = true;
        return fields;
    }
    bool has_fields() const{
        return status_[3];
    }

    void set_uncond_cmds(const string& _uncond_cmds){
        uncond_cmds = _uncond_cmds;
        status_[4] = true;
    }
    void set_uncond_cmds(string&& _uncond_cmds){
        uncond_cmds = std::move(_uncond_cmds);
        status_[4] = true;
    }
    const string& get_uncond_cmds() const {
        return uncond_cmds;
    }
    string& mutable_uncond_cmds(){
        status_[4] = true;
        return uncond_cmds;
    }
    bool has_uncond_cmds() const{
        return status_[4];
    }

    void set_precmds(const string& _precmds){
        precmds = _precmds;
        status_[5] = true;
    }
    void set_precmds(string&& _precmds){
        precmds = std::move(_precmds);
        status_[5] = true;
    }
    const string& get_precmds() const {
        return precmds;
    }
    string& mutable_precmds(){
        status_[5] = true;
        return precmds;
    }
    bool has_precmds() const{
        return status_[5];
    }

    void set_subcmds(const string& _subcmds){
        subcmds = _subcmds;
        status_[6] = true;
    }
    void set_subcmds(string&& _subcmds){
        subcmds = std::move(_subcmds);
        status_[6] = true;
    }
    const string& get_subcmds() const {
        return subcmds;
    }
    string& mutable_subcmds(){
        status_[6] = true;
        return subcmds;
    }
    bool has_subcmds() const{
        return status_[6];
    }

    void set_filters(const string& _filters){
        filters = _filters;
        status_[7] = true;
    }
    void set_filters(string&& _filters){
        filters = std::move(_filters);
        status_[7] = true;
    }
    const string& get_filters() const {
        return filters;
    }
    string& mutable_filters(){
        status_[7] = true;
        return filters;
    }
    bool has_filters() const{
        return status_[7];
    }

public:
    /* operators */
    FadsDbSelUp() = default;
    FadsDbSelUp(const FadsDbSelUp&) = default;
    FadsDbSelUp& operator=(const FadsDbSelUp& rhs) = default;
    bool operator==(const FadsDbSelUp& rhs) const{
        if (header != rhs.header)
            return false;
        if (key != rhs.key)
            return false;
        if (table != rhs.table)
            return false;
        if (fields != rhs.fields)
            return false;
        if (uncond_cmds != rhs.uncond_cmds)
            return false;
        if (precmds != rhs.precmds)
            return false;
        if (subcmds != rhs.subcmds)
            return false;
        if (filters != rhs.filters)
            return false;
        return true;
    }

    bool operator!=(const FadsDbSelUp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbSelUp& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(key)
            ^ std::hash<string>()(table)
            ^ std::hash<string>()(fields)
            ^ std::hash<string>()(uncond_cmds)
            ^ std::hash<string>()(precmds)
            ^ std::hash<string>()(subcmds)
            ^ std::hash<string>()(filters);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    Header header;
    string key;
    string table;
    string fields;
    string uncond_cmds;
    string precmds;
    string subcmds;
    string filters;

    /* for optional */
    bitset<8> attrs_ = 15;
    bitset<8> status_;

public:
    static FadsDbSelUpTypeTree s_type_tree;
};

struct hash_FadsDbSelUp
{
    size_t operator()(const FadsDbSelUp& rhs) const{
        return rhs(rhs);
    }
};

typedef enum{
    kOk,
    kDupKey,
    kFilteredOut,
    kErrProto,
    kErrCrt,
    kErrTableNotExist,
    kErrKeyNotExist,
    kErrInvalidCmd,
    kErrInvalidField,
    kErrDoCmd,
    kErrInternal,
}FadsDbRespStatus;

class FadsDbResponseTypeTree{
public:
    FadsDbResponseTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbResponse : public soce::fads::FadsMessage{
public:
    void set_id(const int64_t& _id){
        id = _id;
        status_[0] = true;
    }
    void set_id(int64_t&& _id){
        id = std::move(_id);
        status_[0] = true;
    }
    const int64_t& get_id() const {
        return id;
    }
    int64_t& mutable_id(){
        status_[0] = true;
        return id;
    }
    bool has_id() const{
        return status_[0];
    }

    void set_status(const FadsDbRespStatus& _status){
        status = _status;
        status_[1] = true;
    }
    void set_status(FadsDbRespStatus&& _status){
        status = std::move(_status);
        status_[1] = true;
    }
    const FadsDbRespStatus& get_status() const {
        return status;
    }
    FadsDbRespStatus& mutable_status(){
        status_[1] = true;
        return status;
    }
    bool has_status() const{
        return status_[1];
    }

    void set_response(const string& _response){
        response = _response;
        status_[2] = true;
    }
    void set_response(string&& _response){
        response = std::move(_response);
        status_[2] = true;
    }
    const string& get_response() const {
        return response;
    }
    string& mutable_response(){
        status_[2] = true;
        return response;
    }
    bool has_response() const{
        return status_[2];
    }

public:
    /* operators */
    FadsDbResponse() = default;
    FadsDbResponse(const FadsDbResponse&) = default;
    FadsDbResponse& operator=(const FadsDbResponse& rhs) = default;
    bool operator==(const FadsDbResponse& rhs) const{
        if (id != rhs.id)
            return false;
        if (status != rhs.status)
            return false;
        if (response != rhs.response)
            return false;
        return true;
    }

    bool operator!=(const FadsDbResponse& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbResponse& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<int64_t>()(id)
            ^ std::hash<int32_t>()(status)
            ^ std::hash<string>()(response);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    int64_t id;
    FadsDbRespStatus status;
    string response;

    /* for optional */
    bitset<3> attrs_ = 0;
    bitset<3> status_;

public:
    static FadsDbResponseTypeTree s_type_tree;
};

struct hash_FadsDbResponse
{
    size_t operator()(const FadsDbResponse& rhs) const{
        return rhs(rhs);
    }
};

}}//end namespace

#endif


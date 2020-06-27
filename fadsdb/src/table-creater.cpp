/*
* Licensed to the Apache Software Foundation (ASF) under one
* or more contributor license agreements. See the NOTICE file
* distributed with this work for additional information
* regarding copyright ownership. The ASF licenses this file
* to you under the Apache License, Version 2.0 (the
* "License"); you may not use this file except in compliance
* with the License. You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied. See the License for the
* specific language governing permissions and limitations
* under the License.
*/

#include "table-creater.h"
#include "utils/str-helper.h"
#include "log4rel/logger.h"
#include "node.h"

using std::string;

namespace soce{
namespace fadsdb{

    TableCreater::TableCreater()
    {
        register_parser("struct", std::bind(&TableCreater::parse_struct, this, std::placeholders::_1));
        register_parser("list", std::bind(&TableCreater::parse_list, this, std::placeholders::_1));
        register_parser("set", std::bind(&TableCreater::parse_set, this, std::placeholders::_1));
        register_parser("map", std::bind(&TableCreater::parse_map, this, std::placeholders::_1));
    }

    TableCreater::~TableCreater()
    {
    }

    int TableCreater::create(const string& schema,
                             std::vector<std::shared_ptr<FadsTable>>& tables)
    {
        SymbolParser sp;
        sp.add_schema(schema);

        string keyword;
        while (!sp.end()) {
            if (sp.get_token(keyword).error()){
                return -1;
            }

            soce::utils::StrHelper::tolower(keyword);
            if (keyword == "table"){
                std::shared_ptr<FadsTable> table = parse_table(sp);
                if (!table){
                    return -1;
                }
                tables.push_back(table);
            }
            else{
                auto iter = parsers_.find(keyword);
                if (iter == parsers_.end()){
                    SOCE_FATAL << _S("invalid keyword", keyword);
                    return -1;
                }

                if (iter->second(sp)){
                    return -1;
                }
            }
        }

        return 0;
    }

    string TableCreater::get_tables()
    {
        string out;
        for (auto& i : tables_){
            if (!out.empty()){
                out += "\n";
            }
            out += i.first;
        }

        return std::move(out);
    }

    string TableCreater::get_custom_type()
    {
        string out;
        for (auto& i : custom_types_){
            if (!out.empty()){
                out += "\n";
            }
            out += i.first;
        }

        return std::move(out);
    }

    string TableCreater::get_table_schema()
    {
        string out;
        for (auto& i : tables_){
            if (!out.empty()){
                out += "\n";
            }
            out += i.second->get_schema();
        }

        return std::move(out);
    }

    string TableCreater::get_table_schema(const string& name)
    {
        string out;
        auto iter = tables_.find(name);
        if (iter != tables_.end()){
            out = iter->second->get_schema();
        }
        return std::move(out);
    }

    string TableCreater::get_custom_type_schema()
    {
        string out;
        for (auto& i : custom_types_){
            if (!out.empty()){
                out += "\n\n";
            }
            out += i.second->get_schema();
        }

        return std::move(out);
    }

    string TableCreater::get_custom_type_schema(const string& name)
    {
        string out;
        auto iter = custom_types_.find(name);
        if (iter != custom_types_.end()){
            out = iter->second->get_schema();
        }
        return std::move(out);
    }

    void TableCreater::register_parser(const string& keyword, KeywordParser parser)
    {
        parsers_[keyword] = parser;
    }

    void TableCreater::unregister_parser(const string& keyword)
    {
        parsers_.erase(keyword);
    }

    std::shared_ptr<FadsTable> TableCreater::parse_table(SymbolParser& sp)
    {
        string table_name;
        if (sp.get_token(table_name).skip_sep('{').error()){
            return std::shared_ptr<FadsTable>();
        }

        if (exist(table_name)){
            return std::shared_ptr<FadsTable>();
        }

        std::shared_ptr<FadsTable> table(new FadsTable);
        table->set_name(table_name);
        table->add_schema("table " + table_name + "\n{\n", 0);

        while (!sp.end()){
            string type;
            string name;
            if (sp.get_token(type).error()){
                return std::shared_ptr<FadsTable>();
            }

            if (is_primary(type)){
                if (sp.get_token(name).skip_sep(';').error()){
                    return std::shared_ptr<FadsTable>();
                }
                table->add_elem(type, name);
                table->add_schema(type + " " + name + ";\n", 4);
            }
            else if(type == "list"){
                Prototype proto;
                string inner_type;
                if (get_list_prototype(sp, false, proto, name, inner_type)){
                    return std::shared_ptr<FadsTable>();
                }

                table->add_elem(proto, name);
                table->add_schema("list " + name + "{" + inner_type + "}\n", 4);
            }
            else if (type == "set"){
                Prototype proto;
                string inner_type;
                if (get_set_prototype(sp, false, proto, name, inner_type)){
                    return std::shared_ptr<FadsTable>();
                }

                table->add_elem(proto, name);
                table->add_schema("set " + name + "{" + inner_type + "}\n", 4);
            }
            else if (type == "map"){
                Prototype proto;
                string key_type;
                string val_type;
                if (get_map_prototype(sp, false, proto, name, key_type, val_type)){
                    return std::shared_ptr<FadsTable>();
                }

                table->add_elem(proto, name);
                table->add_schema("map " + name + "{" + key_type + ", " + val_type + "}\n", 4);
            }
            else{
                if (sp.get_token(name)
                    .skip_sep(';')
                    .error()){
                    return std::shared_ptr<FadsTable>();
                }

                auto iter = custom_types_.find(type);
                if (iter == custom_types_.end()){
                    return std::shared_ptr<FadsTable>();
                }
                table->add_elem(iter->second->get_prototype(), name);
                table->add_schema(type + " " + name + ";\n", 4);
            }

            char sep;
            if (sp.get_sep(sep).error()){
                return std::shared_ptr<FadsTable>();
            }
            if (sep == '}'){
                table->add_schema("}\n", 0);
                break;
            }
        }
        tables_[table_name] = table;
        return table;
    }

    int TableCreater::parse_struct(SymbolParser& sp)
    {
        string struct_name;
        if (sp.get_token(struct_name).skip_sep('{').error()){
            return -1;
        }

        if (exist(struct_name)){
            return -1;
        }

        std::shared_ptr<CustomType> ctype(new CustomType);
        ctype->add_schema("struct " + struct_name + "\n{\n", 0);

        while (!sp.end()){
            string type;
            string name;
            if (sp.get_token(type).error()){
                return -1;
            }

            if (is_primary(type)){
                if (sp.get_token(name).skip_sep(';').error()){
                    return -1;
                }

                ctype->add_elem(type, name);
                ctype->add_schema(type + " " + name + ";\n", 4);
            }
            else if(type == "list"){
                Prototype proto;
                string inner_type;
                if (get_list_prototype(sp, false, proto, name, inner_type)){
                    return -1;
                }

                ctype->add_elem(proto, name);
                ctype->add_schema("list " + name + "{" + inner_type + "}\n", 4);
            }
            else if (type == "set"){
                Prototype proto;
                string inner_type;
                if (get_set_prototype(sp, false, proto, name, inner_type)){
                    return -1;
                }

                ctype->add_elem(proto, name);
                ctype->add_schema("set " + name + "{" + inner_type + "}\n", 4);
            }
            else if (type == "map"){
                Prototype proto;
                string key_type;
                string val_type;
                if (get_map_prototype(sp, false, proto, name, key_type, val_type)){
                    return -1;
                }

                ctype->add_elem(proto, name);
                ctype->add_schema("map " + name + "{" + key_type + ", " + val_type + "}\n", 4);
            }
            else{
                if (sp.get_token(name)
                    .skip_sep(';')
                    .error()){
                    return -1;
                }

                auto iter = custom_types_.find(type);
                if (iter == custom_types_.end()){
                    return -1;
                }
                ctype->add_elem(iter->second->get_prototype(), name);
                ctype->add_schema(type + " " + name + ";\n", 4);
            }

            char sep;
            if (sp.get_sep(sep).error()){
                return -1;
            }
            if (sep == '}'){
                ctype->add_schema("}\n", 0);
                break;
            }
        }

        custom_types_[struct_name] = ctype;
        return 0;
    }

    int TableCreater::parse_list(SymbolParser& sp)
    {
        Prototype proto;
        string name;
        string inner_type;
        if (get_list_prototype(sp, true, proto, name, inner_type)){
            return -1;
        }

        if (exist(name)){
            return -1;
        }

        std::shared_ptr<CustomType> ctype(new CustomType);
        ctype->add_schema("list " + name + "\n{\n    " + inner_type + "\n}\n", 0);
        ctype->set_prototype(proto);
        custom_types_[name] = ctype;
        return 0;
    }

    int TableCreater::parse_set(SymbolParser& sp)
    {
        Prototype proto;
        string name;
        string inner_type;
        if (get_set_prototype(sp, true, proto, name, inner_type)){
            return -1;
        }

        if (exist(name)){
            return -1;
        }

        std::shared_ptr<CustomType> ctype(new CustomType);
        ctype->add_schema("set " + name + "\n{\n    " + inner_type + "\n}\n", 0);
        ctype->set_prototype(proto);
        custom_types_[name] = ctype;
        return 0;
    }

    int TableCreater::parse_map(SymbolParser& sp)
    {
        Prototype proto;
        string name;
        string key_type;
        string val_type;
        if (get_map_prototype(sp, true, proto, name, key_type, val_type)){
            return -1;
        }

        if (exist(name)){
            return -1;
        }

        std::shared_ptr<CustomType> ctype(new CustomType);
        ctype->add_schema("map " + name + "\n{\n    " + key_type + ",\n" + val_type + "\n}\n", 0);
        ctype->set_prototype(proto);
        custom_types_[name] = ctype;
        return 0;
    }

    bool TableCreater::exist(const string& name)
    {
        return (tables_.find(name) != tables_.end()
                || custom_types_.find(name) != custom_types_.end()) ? true : false;
    }

    int TableCreater::get_list_prototype(SymbolParser& sp, bool outter, Prototype& proto, string& name, string& inner_type)
    {
        if (outter){
            if (sp.get_token(name)
                .skip_sep('{')
                .get_token(inner_type)
                .skip_sep('}')
                .error()){
                return -1;
            }
        }
        else{
            if (sp.skip_sep('<')
                .get_token(inner_type)
                .skip_sep('>')
                .get_token(name)
                .skip_sep(';').error()){
                return -1;
            }
        }

        std::shared_ptr<NodeIf> node;
        if (inner_type == "bool"){
            node.reset(new NodeListBool);
        }
        else if (inner_type == "i32"){
            node.reset(new NodeListInt32);
        }
        else if (inner_type == "i64"){
            node.reset(new NodeListInt64);
        }
        else if (inner_type == "float"){
            node.reset(new NodeListFloat);
        }
        else if (inner_type == "double"){
            node.reset(new NodeListDouble);
        }
        else if (inner_type == "string"){
            node.reset(new NodeListString);
        }
        else{
            auto iter = custom_types_.find(inner_type);
            if (iter == custom_types_.end()){
                return -1;
            }

            std::shared_ptr<NodeListCustom> nlc(new NodeListCustom);

            std::unordered_map<std::string, std::shared_ptr<NodeIf>> indexes;
            std::shared_ptr<NodeIf> proto = iter->second->get_prototype().clone(indexes);
            nlc->set_prototype(proto);
            node = nlc;
        }

        proto.set_node(node);
        return 0;
    }

    int TableCreater::get_set_prototype(SymbolParser& sp, bool outter, Prototype& proto, string& name, string& inner_type)
    {
        if (outter){
            if (sp.get_token(name)
                .skip_sep('{')
                .get_token(inner_type)
                .skip_sep('}')
                .error()){
                return -1;
            }
        }
        else{
            if (sp.skip_sep('<')
                .get_token(inner_type)
                .skip_sep('>')
                .get_token(name)
                .skip_sep(';').error()){
                return -1;
            }
        }

        std::shared_ptr<NodeIf> node;
        if (inner_type == "bool"){
            node.reset(new NodeSetBool);
        }
        else if (inner_type == "i32"){
            node.reset(new NodeSetInt32);
        }
        else if (inner_type == "i64"){
            node.reset(new NodeSetInt64);
        }
        else if (inner_type == "float"){
            node.reset(new NodeSetFloat);
        }
        else if (inner_type == "double"){
            node.reset(new NodeSetDouble);
        }
        else if (inner_type == "string"){
            node.reset(new NodeSetString);
        }
        else{
            auto iter = custom_types_.find(inner_type);
            if (iter == custom_types_.end()){
                return -1;
            }

            std::shared_ptr<NodeSetCustom> nlc(new NodeSetCustom);

            std::unordered_map<std::string, std::shared_ptr<NodeIf>> indexes;
            std::shared_ptr<NodeIf> proto = iter->second->get_prototype().clone(indexes);
            nlc->set_prototype(proto);
            node = nlc;
        }

        proto.set_node(node);
        return 0;
    }

    int TableCreater::get_map_prototype(SymbolParser& sp,
                                        bool outter,
                                        Prototype& proto,
                                        string& name,
                                        string& key_type,
                                        string& val_type)
    {
        if (outter){
            if (sp.get_token(name)
                .skip_sep('{')
                .get_token(key_type)
                .skip_sep(',')
                .get_token(val_type)
                .skip_sep('}')
                .error()){
                return -1;
            }
        }
        else{
            if (sp.skip_sep('<')
                .get_token(key_type)
                .skip_sep(',')
                .get_token(val_type)
                .skip_sep('>')
                .get_token(name)
                .skip_sep(';').error()){
                return -1;
            }
        }

        std::shared_ptr<NodeIf> node;
        if (key_type == "bool"){
            if (val_type == "bool"){
                node.reset(new NodeMapBoolBool);
            }
            else if (val_type == "i32"){
                node.reset(new NodeMapBoolInt32);
            }
            else if (val_type == "i64"){
                node.reset(new NodeMapBoolInt64);
            }
            else if (val_type == "float"){
                node.reset(new NodeMapBoolFloat);
            }
            else if (val_type == "double"){
                node.reset(new NodeMapBoolDouble);
            }
            else if (val_type == "string"){
                node.reset(new NodeMapBoolString);
            }
            else{
                std::shared_ptr<NodeIf> proto = get_proto(val_type);
                if (proto){
                    std::shared_ptr<NodeMapCustomBool> nmc(new NodeMapCustomBool);
                    nmc->set_prototype(proto);
                    node = nmc;
                }
            }
        }
        else if (key_type == "i32"){
            if (val_type == "bool"){
                node.reset(new NodeMapInt32Bool);
            }
            else if (val_type == "i32"){
                node.reset(new NodeMapInt32Int32);
            }
            else if (val_type == "i64"){
                node.reset(new NodeMapInt32Int64);
            }
            else if (val_type == "float"){
                node.reset(new NodeMapInt32Float);
            }
            else if (val_type == "double"){
                node.reset(new NodeMapInt32Double);
            }
            else if (val_type == "string"){
                node.reset(new NodeMapInt32String);
            }
            else{
                std::shared_ptr<NodeIf> proto = get_proto(val_type);
                if (proto){
                    std::shared_ptr<NodeMapCustomInt32> nmc(new NodeMapCustomInt32);
                    nmc->set_prototype(proto);
                    node = nmc;
                }
            }
        }
        else if (key_type == "i64"){
            if (val_type == "bool"){
                node.reset(new NodeMapInt64Bool);
            }
            else if (val_type == "i32"){
                node.reset(new NodeMapInt64Int32);
            }
            else if (val_type == "i64"){
                node.reset(new NodeMapInt64Int64);
            }
            else if (val_type == "float"){
                node.reset(new NodeMapInt64Float);
            }
            else if (val_type == "double"){
                node.reset(new NodeMapInt64Double);
            }
            else if (val_type == "string"){
                node.reset(new NodeMapInt64String);
            }
            else{
                std::shared_ptr<NodeIf> proto = get_proto(val_type);
                if (proto){
                    std::shared_ptr<NodeMapCustomInt64> nmc(new NodeMapCustomInt64);
                    nmc->set_prototype(proto);
                    node = nmc;
                }
            }
        }
        else if (key_type == "float"){
            if (val_type == "bool"){
                node.reset(new NodeMapFloatBool);
            }
            else if (val_type == "i32"){
                node.reset(new NodeMapFloatInt32);
            }
            else if (val_type == "i64"){
                node.reset(new NodeMapFloatInt64);
            }
            else if (val_type == "float"){
                node.reset(new NodeMapFloatFloat);
            }
            else if (val_type == "double"){
                node.reset(new NodeMapFloatDouble);
            }
            else if (val_type == "string"){
                node.reset(new NodeMapFloatString);
            }
            else{
                std::shared_ptr<NodeIf> proto = get_proto(val_type);
                if (proto){
                    std::shared_ptr<NodeMapCustomFloat> nmc(new NodeMapCustomFloat);
                    nmc->set_prototype(proto);
                    node = nmc;
                }
            }
        }
        else if (key_type == "double"){
            if (val_type == "bool"){
                node.reset(new NodeMapDoubleBool);
            }
            else if (val_type == "i32"){
                node.reset(new NodeMapDoubleInt32);
            }
            else if (val_type == "i64"){
                node.reset(new NodeMapDoubleInt64);
            }
            else if (val_type == "float"){
                node.reset(new NodeMapDoubleFloat);
            }
            else if (val_type == "double"){
                node.reset(new NodeMapDoubleDouble);
            }
            else if (val_type == "string"){
                node.reset(new NodeMapDoubleString);
            }
            else{
                std::shared_ptr<NodeIf> proto = get_proto(val_type);
                if (proto){
                    std::shared_ptr<NodeMapCustomDouble> nmc(new NodeMapCustomDouble);
                    nmc->set_prototype(proto);
                    node = nmc;
                }
            }
        }
        else if (key_type == "string"){
            if (val_type == "bool"){
                node.reset(new NodeMapStringBool);
            }
            else if (val_type == "i32"){
                node.reset(new NodeMapStringInt32);
            }
            else if (val_type == "i64"){
                node.reset(new NodeMapStringInt64);
            }
            else if (val_type == "float"){
                node.reset(new NodeMapStringFloat);
            }
            else if (val_type == "double"){
                node.reset(new NodeMapStringDouble);
            }
            else if (val_type == "string"){
                node.reset(new NodeMapStringString);
            }
            else{
                std::shared_ptr<NodeIf> proto = get_proto(val_type);
                if (proto){
                    std::shared_ptr<NodeMapCustomString> nmc(new NodeMapCustomString);
                    nmc->set_prototype(proto);
                    node = nmc;
                }
            }
        }
        else{
            return -1;
        }

        proto.set_node(node);
        return 0;
    }

    std::shared_ptr<NodeIf> TableCreater::get_proto(const string& name)
    {
        std::shared_ptr<NodeIf> proto;
        auto iter = custom_types_.find(name);
        if (iter == custom_types_.end()){
            return proto;
        }

        std::unordered_map<std::string, std::shared_ptr<NodeIf>> indexes;
        proto= iter->second->get_prototype().clone(indexes);
        return proto;
    }

    bool TableCreater::is_primary(const string& type)
    {
        return (type == "bool"
                || type == "i32"
                || type == "i64"
                || type == "float"
                || type == "double"
                || type == "string"
                || type == "any") ? true : false;
    }

    int TableCreater::CustomType::add_elem(const string& type,
                                           const string& name)
    {
        return proto_.add_elem(type, name);
    }

    int TableCreater::CustomType::add_elem(const Prototype& proto, const string& name)
    {
        return proto_.add_elem(proto, name);
    }

} // namespace fadsdb
} // namespace soce

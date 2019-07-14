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

#ifndef _SOCE_TABLE_CREATER_xZh8tYrp8a_H_
#define _SOCE_TABLE_CREATER_xZh8tYrp8a_H_

#include <vector>
#include "fads-table.h"
#include "prototype.h"
#include "symbol-parser.h"

namespace soce{
namespace fadsdb{

    class TableCreater
    {
    public:
        TableCreater();
        ~TableCreater();
        int create(const std::string& schema,
                   std::vector<std::shared_ptr<FadsTable>>& tables);

        std::string get_tables();
        std::string get_custom_type();
        std::string get_table_schema();
        std::string get_table_schema(const std::string& name);
        std::string get_custom_type_schema();
        std::string get_custom_type_schema(const std::string& name);

    private:
        using KeywordParser = std::function<int(SymbolParser& sp)>;

    private:
        void register_parser(const std::string& keyword, KeywordParser parser);
        void unregister_parser(const std::string& keyword);
        bool is_primary(const std::string& type);

        std::shared_ptr<FadsTable> parse_table(SymbolParser& sp);
        int parse_struct(SymbolParser& sp);
        int parse_list(SymbolParser& sp);
        int parse_set(SymbolParser& sp);
        int parse_map(SymbolParser& sp);
        bool exist(const std::string& name);
        int get_list_prototype(SymbolParser& sp, bool outter, Prototype& proto, std::string& name, std::string& inner_type);
        int get_set_prototype(SymbolParser& sp, bool outter, Prototype& proto, std::string& name, std::string& inner_type);
        int get_map_prototype(SymbolParser& sp,
                              bool outter,
                              Prototype& proto,
                              std::string& name,
                              std::string& key_type,
                              std::string& val_type);
        std::shared_ptr<NodeIf> get_proto(const std::string& name);

    private:
        class CustomType;

    private:
        std::unordered_map<std::string, std::shared_ptr<FadsTable>> tables_;
        std::unordered_map<std::string, std::shared_ptr<CustomType>> custom_types_;
        std::unordered_map<std::string, KeywordParser> parsers_;
    };

    class TableCreater::CustomType
    {
    public:
        /* for struct */
        int add_elem(const std::string& type, const std::string& name);
        int add_elem(const Prototype& proto, const std::string& name);

        inline void add_schema(const std::string& schema, uint32_t indent) {
            schema_ += std::string(indent, ' ') + schema;
        }
        inline const std::string& get_schema() const {return schema_;}
        inline const Prototype& get_prototype() const {return proto_;}
        inline void set_prototype(Prototype& proto) {proto_ = proto;}

    private:
        std::string schema_;
        Prototype proto_;
    };

} // namespace fadsdb
} // namespace soce

#endif

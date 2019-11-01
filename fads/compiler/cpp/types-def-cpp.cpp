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

#include <functional>
#include <unordered_map>
#include "utils/str-helper.h"
#include "types-def-cpp.h"
#include "fads.h"
#include "utils.h"

using std::function;
using std::ostringstream;
using std::string;
using std::vector;
using std::unordered_map;

namespace soce{
namespace compiler{

    void ReferenceCpp::gen_header(std::ostringstream& oss, const std::set<std::string>& types)
    {
        oss << "#include <string>\n"
            << "#include <bitset>\n"
            << "#include <stdint.h>\n";

        for (auto& i : types){
            if (i == "list" || i == "vector"){
                oss << "#include <vector>\n";
            }
            else if (i == "set"){
                oss << "#include <unordered_set>\n";
            }
            else if (i == "map"){
                oss << "#include <unordered_map>\n";
            }
        }

        if (!SSvrCrt.empty()){
            oss << "#include \"gen-cpp/crpc-header.h\"\n";
        }
        oss << "#include \"proto/fads-message.h\"\n#include \"proto/fads-message.h\"\n#include \"proto/soce-proto.h\"\n#include \"proto/type-tree.h\"\n\nusing std::string;\nusing std::bitset;\n";
        for (auto& i : types){
            if (i == "list" || i == "vector"){
                oss << "using std::vector;\n";
            }
            else if (i == "set"){
                oss << "using std::unordered_set;\n";
            }
            else if (i == "map"){
                oss << "using std::unordered_map;\n";
            }
        }

        oss << "\n";

        // wrtie namespace
        vector<string> ns;
        (void)soce::utils::StrHelper::split(SOpMgr.get_namespace(), ".", ns);
        for (auto& i : ns){
            oss << "namespace " << i << "{ ";
        }
        oss << "\n\n";
    }

    static string transfer_type(const string& type, bool for_func_name = false)
    {
        static unordered_map<string, string> type_map = {
            {"bool", "Bool"},
            {"int32_t", "Int32"},
            {"int64_t", "Int64"},
            {"float", "Float"},
            {"double", "Double"},
            {"string", "String"},
            {"byte", "Bytes"},
            {"enum", "Enum"},
            {"list", "List"},
            {"set", "Set"},
            {"map", "Map"},
            {"struct", "Struct"}
        };

        static unordered_map<string, string> type_map_for_func = {
            {"bool", "_bool"},
            {"int32_t", "_int32"},
            {"int64_t", "_int64"},
            {"float", "_float"},
            {"double", "_double"},
            {"string", "_string"},
            {"byte", "_bytes"},
            {"enum", "_enum"},
            {"list", "_list"},
            {"set", "_set"},
            {"map", "_map"},
            {"struct", "_struct"}
        };

        string rc;
        if (!for_func_name){
            auto iter = type_map.find(type);
            if (iter != type_map.end()){
                rc = iter->second;
            }
        }
        else{
            auto iter = type_map_for_func.find(type);
            if (iter != type_map_for_func.end()){
                rc = iter->second;
            }
        }

        return std::move(rc);
    }

    static std::string get_type_nature(const std::string& type)
    {
        string nature = STypeCreater.get_type_nature(type);
        string rc = "soce::proto::SoceDataType::kType";
        rc += transfer_type(nature);
        return std::move(rc);
    }

    static string indent(size_t margin, int level)
    {
        return std::move(string(margin + level * Utils::get_indent(), ' '));
    }

    static void gen_operator(ostringstream& oss,
                             size_t margin,
                             string& cname,
                             function<void(ostringstream&, size_t)> WriteSetterGetter,
                             function<void(ostringstream&, size_t)> WriteEqualBody,
                             function<void(ostringstream&, size_t)> WriteHashBody,
                             function<void(ostringstream&, size_t)> WriteInterface,
                             function<void(ostringstream&, size_t)> WriteMembers,
                             bool is_struct = false)
    {
        oss << indent(margin, 0)
            << "class "
            << cname;
        if (is_struct) {
            oss << " : public soce::fads::FadsMessage";
        }
        oss << "{\n";
        WriteSetterGetter(oss, margin + Utils::get_indent());
        oss << "\n"
            << indent(margin, 0)
            << "public:\n"
            << indent(margin, 1)
            << "/* operators */\n"
            << indent(margin, 1)
            << cname
            << "() = default;\n"
            << indent(margin, 1)
            << cname
            << "(const "
            << cname
            << "&) = default;\n"
            << indent(margin, 1)
            << cname
            << "& operator=(const " << cname << "& rhs) = default;\n"
            << indent(margin, 1)
            << "bool operator==(const "
            << cname
            << "& rhs) const{\n";
        WriteEqualBody(oss, margin + Utils::get_indent());
        oss << indent(margin, 2)
            << "return true;\n"
            << indent(margin, 1)
            << "}\n\n";

        // operator!=
        oss << indent(margin, 1)
            << "bool operator!=(const "
            << cname
            << "& rhs) const{\n"
            << indent(margin, 2)
            << "return !(*this == rhs);\n"
            << indent(margin, 1)
            << "}\n\n";

        // operator() for hash
        oss << indent(margin, 1)
            << "size_t operator()(const "
            << cname
            << "& rhs) const{\n"
            << indent(margin, 2)
            << "size_t hash_code = 0;\n";
        WriteHashBody(oss, margin + Utils::get_indent());
        oss << indent(margin, 2)
            << "return hash_code;\n"
            << indent(margin, 1)
            << "}\n\n";

        oss << indent(margin, 1)
            << "size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);\n"
            << indent(margin, 1)
            << "size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;\n";
        WriteInterface(oss, margin);

        oss << "\n"
            << indent(margin, 0)
            << "private:\n"
            << indent(margin, 1)
            << "/* class members */\n";
        WriteMembers(oss, margin);
        oss << indent(margin, 0)
            << "};\n\nstruct hash_"
            << cname
            << "\n{\n"
            << indent(margin, 1)
            << "size_t operator()(const "
            << cname
            << "& rhs) const{\n"
            << indent(margin, 2)
            << "return rhs(rhs);\n"
            << indent(margin, 1)
            << "}\n};\n\n";
    }

    static std::string gen_hasher(const std::string& type)
    {
        string hasher;
        string nature = STypeCreater.get_type_nature(type);
        if (nature == "list"
            || nature == "set"
            || nature == "map"
            || nature == "struct"){
            hasher = ", hash_" + type;
        }

        return hasher;
    }

    static std::string gen_hasher(const std::string& type, const std::string& name)
    {
        string hash_code;
        string nature = STypeCreater.get_type_nature(type);
        if (nature == "enum"){
            hash_code = "std::hash<int32_t>()(" + name + ")";
        }
        else if (nature == "list"
                 || nature == "set"
                 || nature == "map"
                 || nature == "struct"){
            hash_code = name + "(" + name + ")";
        }
        else{
            hash_code = "std::hash<" + type + ">()(" + name + ")";
        }
        return hash_code;
    }

    static std::string gen_read_by_type(const std::string& type,
                                        const std::string& arg,
                                        bool inside_struct = false)
    {
        string stype;
        string rc = "elem_len = ";
        if (Utils::is_primitive_type(type)){
            stype = transfer_type(type, true);
            if (inside_struct){
                rc += "soce::proto::SoceProto::read" + stype + "(proto, attrs_[index], " + arg + ");\n";
            }
            else{
                rc += "proto->read" + stype + "(" + arg + ");\n";
            }
        }
        else{
            string nature = STypeCreater.get_type_nature(type);

            if (inside_struct){
                if (nature == "enum"){
                    rc += "soce::proto::SoceProto::read_enum(proto, attrs_[index], *(int32_t*)&" + arg + ");\n";
                }
                else{
                    rc += arg + ".read(proto, attrs_[index], true);\n";
                }
            }
            else{
                if (nature == "enum"){
                    rc += "proto->read_enum(*(int32_t*)&" + arg + ");\n";
                }
                else{
                    rc += arg + ".read(proto, true, false);\n";
                }
            }
        }
        return move(rc);
    }

    static std::string gen_write_by_type(const std::string& type,
                                         const std::string& arg,
                                         bool inside_struct)
    {
        string stype;
        string rc = "elem_len = ";
        if (Utils::is_primitive_type(type)){
            stype = transfer_type(type, true);
            if (inside_struct){
                rc += "soce::proto::SoceProto::write"
                    + stype
                    + "(proto, attrs_[index], status_[index], "
                    + arg
                    + ");\n";
            }
            else{
                rc += "proto->write" + stype + "(" + arg + ");\n";
            }
        }
        else{
            string nature = STypeCreater.get_type_nature(type);

            if (inside_struct){
                if (nature == "enum"){
                    rc += "soce::proto::SoceProto::write_enum(proto, attrs_[index], status_[index], *(int32_t*)&"
                        + arg
                        + ");\n";
                }
                else{
                    rc += arg + ".write(proto, attrs_[index], status_[index], true);\n";
                }
            }
            else{
                if (nature == "enum"){
                    rc += "proto->write_enum(*(int32_t*)&" + arg + ");\n";
                }
                else{
                    rc += arg + ".write(proto, true, true, false);\n";
                }
            }
        }
        return move(rc);
    }

    static void gen_list_read(ostringstream& oss,
                              size_t margin,
                              bool as_member,
                              const std::string& name,
                              const std::string& val,
                              const std::string& eletype_)
    {
        string required;
        string with_type;
        if (as_member){
            required = "attrs_[index],\n";
            with_type = "true,\n";
        }
        else{
            required = "required,\n";
            with_type = "with_type,\n";

            oss << indent(margin, 0)
                << "size_t "
                << name
                << "::read(soce::proto::ProtoIf* proto, bool required, bool with_type){\n"
                << indent(margin, 1)
                << "uint32_t elem_len = 0;\n";
            margin += Utils::get_indent();
        }

        oss << indent(margin, 0)
            << "elem_len = soce::proto::SoceProto::read_list(proto,\n"
            << indent(margin, 1)
            << required
            << indent(margin, 1)
            << with_type
            << indent(margin, 1)
            << get_type_nature(eletype_)
            << ",\n"
            << indent(margin, 1)
            << "[this](uint32_t size){\n"
            << indent(margin, 2)
            << val
            << ".resize(size);\n"
            << indent(margin, 1)
            << "},\n"
            << indent(margin, 1)
            << "[&proto, this](uint32_t i){\n"
            << indent(margin, 2)
            << "uint32_t elem_len = 0;\n"
            << indent(margin, 2)
            << gen_read_by_type(eletype_, string(val + "[i]"))
            << indent(margin, 2)
            << "return elem_len;\n"
            << indent(margin, 1)
            << "});\n";

        if (as_member){
        }
        else{
            oss << indent(margin, 0)
                << "return elem_len;\n"
                << indent(margin, -1)
                << "};\n\n";
        }
    }

    static void gen_list_write(ostringstream& oss,
                               size_t margin,
                               bool as_member,
                               const std::string& name,
                               const std::string& val,
                               const std::string& eletype_)
    {
        string required;
        string has_set;
        string with_type;

        if (as_member){
            required = "attrs_[index],\n";
            has_set = "status_[index],\n";
            with_type = "true,\n";
        }
        else{
            required = "required,\n";
            has_set = "has_set,\n";
            with_type = "with_type,\n";
            oss << indent(margin, 0)
                << "size_t "
                << name
                << "::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {\n"
                << indent(margin, 1)
                << "uint32_t elem_len = 0;\n";
        }

        oss << indent(margin, 1)
            << "elem_len = soce::proto::SoceProto::write_list(proto,\n"
            << indent(margin, 1)
            << required
            << indent(margin, 1)
            << has_set
            << indent(margin, 1)
            << with_type
            << indent(margin, 1)
            << val
            << ".size(),\n"
            << indent(margin, 1)
            << get_type_nature(eletype_)
            << ",\n"
            << indent(margin, 1)
            << "[&proto, this](uint32_t i)->uint32_t{\n"
            << indent(margin, 2)
            << "uint32_t elem_len = 0;\n"
            << indent(margin, 2)
            << gen_write_by_type(eletype_, string(val + "[i]"), false)
            << indent(margin, 2)
            << "return elem_len;\n"
            << indent(margin, 1)
            << "});\n";


        if (as_member){
        }
        else{
            oss << indent(margin, 1)
                << "return elem_len;\n"
                << indent(margin, 0)
                << "}\n\n";
        }
    }

    static void gen_set_read(ostringstream& oss,
                             size_t margin,
                             bool as_member,
                             const std::string& name,
                             const std::string& val,
                             const std::string& eletype_)
    {
        string required;
        string with_type;
        if (as_member){
            required = "attrs_[index],\n";
            with_type = "true,\n";
        }
        else{
            required = "required,\n";
            with_type = "with_type,\n";

            oss << indent(margin, 0)
                << "size_t "
                << name
                << "::read(soce::proto::ProtoIf* proto, bool required, bool with_type){\n"
                << indent(margin, 1)
                << "uint32_t elem_len = 0;\n";
            margin += Utils::get_indent();
        }

        oss << indent(margin, 0)
            << "elem_len = soce::proto::SoceProto::read_set(proto,\n"
            << indent(margin, 1)
            << required
            << indent(margin, 1)
            << with_type
            << indent(margin, 1)
            << get_type_nature(eletype_)
            << ",\n"
            << indent(margin, 1)
            << "[&proto, this](){\n"
            << indent(margin, 2)
            << "uint32_t elem_len = 0;\n"
            << indent(margin, 2)
            << eletype_
            << " elem;\n"
            << indent(margin, 2)
            << gen_read_by_type(eletype_, "elem")
            << indent(margin, 2)
            << val
            << ".insert(elem);\n"
            << indent(margin, 2)
            << "return elem_len;\n"
            << indent(margin, 1)
            << "});\n";

        if (as_member){
        }
        else{
            oss << indent(margin, 0)
                << "return elem_len;\n"
                << indent(margin, -1)
                << "};\n\n";
        }
    }

    static void gen_set_write(ostringstream& oss,
                              size_t margin,
                              bool as_member,
                              const std::string& name,
                              const std::string& val,
                              const std::string& eletype_)
    {
        string required;
        string has_set;
        string with_type;

        if (as_member){
            required = "attrs_[index],\n";
            has_set = "status_[index],\n";
            with_type = "true,\n";
        }
        else{
            required = "required,\n";
            has_set = "has_set,\n";
            with_type = "with_type,\n";
            oss << indent(margin, 0)
                << "size_t "
                << name
                << "::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {\n"
                << indent(margin, 1)
                << "uint32_t elem_len = 0;\n";
        }

        margin += Utils::get_indent();

        oss << indent(margin, 0)
            << "elem_len = soce::proto::SoceProto::write_set(proto,\n"
            << indent(margin, 1)
            << required
            << indent(margin, 1)
            << has_set
            << indent(margin, 1)
            << with_type
            << indent(margin, 1)
            << val
            << ".size(),\n"
            << indent(margin, 1)
            << get_type_nature(eletype_)
            << ",\n"
            << indent(margin, 1)
            << "[&proto, this]()->uint32_t{\n"
            << indent(margin, 2)
            << "uint32_t len = 0;\n"
            << indent(margin, 2)
            << "uint32_t elem_len = 0;\n"
            << indent(margin, 2)
            << "for (auto& elem : "
            << val
            << "){\n"
            << indent(margin, 3)
            << gen_write_by_type(eletype_, "elem", false)
            << indent(margin, 3)
            << "if (!elem_len){\n"
            << indent(margin, 4)
            << "return 0;\n"
            << indent(margin, 3)
            << "}\n"
            << indent(margin, 3)
            << "len += elem_len;\n"
            << indent(margin, 2)
            << "}\n"
            << indent(margin, 2)
            << "return len;\n"
            << indent(margin, 1)
            << "});\n";


        if (as_member){
        }
        else{
            oss << indent(margin, 0)
                << "return elem_len;\n"
                << indent(margin, -1)
                << "}\n\n";
        }
    }

    static void gen_map_read(ostringstream& oss,
                             size_t margin,
                             bool as_member,
                             const std::string& name,
                             const std::string& val,
                             const std::string& key_type,
                             const std::string& val_type)
    {
        string required;
        string with_type;
        if (as_member){
            required = "attrs_[index],\n";
            with_type = "true,\n";
        }
        else{
            required = "required,\n";
            with_type = "with_type,\n";

            oss << indent(margin, 0)
                << "size_t "
                << name
                << "::read(soce::proto::ProtoIf* proto, bool required, bool with_type){\n"
                << indent(margin, 1)
                << "uint32_t elem_len = 0;\n";
            margin += Utils::get_indent();
        }

        oss << indent(margin, 0)
            << "elem_len = soce::proto::SoceProto::read_map(proto,\n"
            << indent(margin, 1)
            << required
            << indent(margin, 1)
            << with_type
            << indent(margin, 1)
            << get_type_nature(key_type)
            << ",\n"
            << indent(margin, 1)
            << get_type_nature(val_type)
            << ",\n"
            << indent(margin, 1)
            << "[&proto, this]()->uint32_t{\n"
            << indent(margin, 2)
            << "uint32_t len = 0;\n"
            << indent(margin, 2)
            << "uint32_t elem_len = 0;\n"
            << indent(margin, 2)
            << key_type
            << " key;\n"
            << indent(margin, 2)
            << gen_read_by_type(key_type, "key")
            << indent(margin, 2)
            << "if (!elem_len){\n"
            << indent(margin, 3)
            << "return 0;\n"
            << indent(margin, 2)
            << "}\n"
            << indent(margin, 2)
            << "len += elem_len;\n"
            << indent(margin, 2)
            << val_type
            << " val;\n"
            << indent(margin, 2)
            << gen_read_by_type(val_type, "val")
            << indent(margin, 2)
            << "if (!elem_len){\n"
            << indent(margin, 3)
            << "return 0;\n"
            << indent(margin, 2)
            << "}\n"
            << indent(margin, 2)
            << "len += elem_len;\n"
            << indent(margin, 2)
            << val
            << "[key] = val;\n"
            << indent(margin, 2)
            << "return len;\n"
            << indent(margin, 1)
            << "});\n";

        if (as_member){
        }
        else{
            oss << indent(margin, 0)
                << "return elem_len;\n"
                << indent(margin, -1)
                << "};\n\n";
        }
    }

    static void gen_map_write(ostringstream& oss,
                              size_t margin,
                              bool as_member,
                              const std::string& name,
                              const std::string& val,
                              const std::string& key_type,
                              const std::string& val_type)
    {
        string required;
        string has_set;
        string with_type;

        if (as_member){
            required = "attrs_[index],\n";
            has_set = "status_[index],\n";
            with_type = "true,\n";
        }
        else{
            required = "required,\n";
            has_set = "has_set,\n";
            with_type = "with_type,\n";
            oss << indent(margin, 0)
                << "size_t "
                << name
                << "::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {\n"
                << indent(margin, 1)
                << "uint32_t elem_len = 0;\n";
        }

        margin += Utils::get_indent();

        oss << indent(margin, 0)
            << "elem_len = soce::proto::SoceProto::write_map(proto,\n"
            << indent(margin, 1)
            << required
            << indent(margin, 1)
            << has_set
            << indent(margin, 1)
            << with_type
            << indent(margin, 1)
            << val
            << ".size(),\n"
            << indent(margin, 1)
            << get_type_nature(key_type)
            << ",\n"
            << indent(margin, 1)
            << get_type_nature(val_type)
            << ",\n"
            << indent(margin, 1)
            << "[&proto, this]()->uint32_t{\n"
            << indent(margin, 2)
            << "uint32_t len = 0;\n"
            << indent(margin, 2)
            << "uint32_t elem_len = 0;\n"
            << indent(margin, 2)
            << "for (auto& elem : "
            << val
            << "){\n"
            << indent(margin, 3)
            << gen_write_by_type(key_type, "elem.first", false)
            << indent(margin, 3)
            << "if (!elem_len){\n"
            << indent(margin, 4)
            << "return 0;\n"
            << indent(margin, 3)
            << "}\n"
            << indent(margin, 3)
            << "len += elem_len;\n"
            << indent(margin, 3)
            << gen_write_by_type(val_type, "elem.second", false)
            << indent(margin, 3)
            << "if (!elem_len){\n"
            << indent(margin, 4)
            << "return 0;\n"
            << indent(margin, 3)
            << "}\n"
            << indent(margin, 3)
            << "len += elem_len;\n"
            << indent(margin, 2)
            << "}\n"
            << indent(margin, 2)
            << "return len;\n"
            << indent(margin, 1)
            << "});\n";


        if (as_member){
        }
        else{
            oss << indent(margin, 0)
                << "return elem_len;\n"
                << indent(margin, -1)
                << "}\n\n";
        }
    }

    void TypeEnumCpp::gen_header(ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << "typedef enum{\n";
        for (auto& i : elems_){
            oss << indent(margin, 1)
                << i.first;
            if (!i.second.empty()){
                oss << " = "
                    << i.second
                    << ",\n";
            }
            else{
                oss << ",\n";
            }
        }
        oss << indent(margin, 0)
            << "}"
            << name_
            << ";\n\n";
    }

    void TypeEnumCpp::gen_impl(ostringstream& oss, size_t margin)
    {
    }

    void TypeListCpp::gen_header(ostringstream& oss, size_t margin)
    {
        gen_operator(oss,
                     margin,
                     name_,
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, -1)
                             << "public:\n"
                             << indent(margin, 0)
                             << "inline const vector<"
                             << m_eletype_
                             << ">& GetElems() const {return elems_;}\n"
                             << indent(margin, 0)
                             << "inline vector<"
                             << m_eletype_
                             << ">& GetElems() {return elems_;}\n"
                             << indent(margin, 0)
                             << "inline void SetElems(const vector<"
                             << m_eletype_
                             << ">& elems) {elems_ = elems;}\n"
                             << indent(margin, 0)
                             << "inline void SetElems(vector<"
                             << m_eletype_
                             << ">&& elems) {elems_ = std::move(elems);}\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "if (elems_ != rhs.elems_)\n"
                             << indent(margin, 2)
                             << "return false;\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "for (auto& i : rhs.elems_){\n"
                             << indent(margin, 2)
                             << "hash_code ^= "
                             << gen_hasher(m_eletype_, "i")
                             << ";\n"
                             << indent(margin, 1)
                             << "}\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "vector<"
                             << m_eletype_
                             << "> elems_;\n";
                     }
            );
    }

    void TypeListCpp::gen_impl(ostringstream& oss, size_t margin)
    {
        gen_list_read(oss, margin, false, name_, "elems_", m_eletype_);
        gen_list_write(oss, margin, false, name_, "elems_", m_eletype_);
    }

    void TypeSetCpp::gen_header(ostringstream& oss, size_t margin)
    {
        gen_operator(oss,
                     margin,
                     name_,
                     [this](ostringstream& oss, size_t margin){
                         oss <<indent(margin, -1)
                             << "public:\n"
                             << indent(margin, 0)
                             << "inline const unordered_set<"
                             << m_eletype_
                             << gen_hasher(m_eletype_)
                             << ">& GetElems() const {return elems_;}\n"
                             << indent(margin, 0)
                             << "inline unordered_set<"
                             << m_eletype_
                             << gen_hasher(m_eletype_)
                             << ">& GetElems() {return elems_;}\n"
                             << indent(margin, 0)
                             << "inline void SetElems(const unordered_set<"
                             << m_eletype_
                             << gen_hasher(m_eletype_)
                             << ">& elems) {elems_ = elems;}\n"
                             << indent(margin, 0)
                             << "inline void SetElems(unordered_set<"
                             << m_eletype_
                             << gen_hasher(m_eletype_)
                             << ">&& elems) {elems_ = std::move(elems);}\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "if (elems_ != rhs.elems_)\n"
                             << indent(margin, 2)
                             << "return false;\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "for (auto& i : rhs.elems_){\n"
                             << indent(margin, 2)
                             << "hash_code ^= "
                             << gen_hasher(m_eletype_, "i")
                             << ";\n"
                             << indent(margin, 1)
                             << "}\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "unordered_set<"
                             << m_eletype_
                             << gen_hasher(m_eletype_)
                             << "> elems_;\n";
                     }
            );
    }

    void TypeSetCpp::gen_impl(ostringstream& oss, size_t margin)
    {
        gen_set_read(oss, margin, false, name_, "elems_", m_eletype_);
        gen_set_write(oss, margin, false, name_, "elems_", m_eletype_);
    }

    void TypeMapCpp::gen_header(ostringstream& oss, size_t margin)
    {
        gen_operator(oss,
                     margin,
                     name_,
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, -1)
                             << "public:\n"
                             << indent(margin, 0)
                             << "inline const unordered_map<"
                             << key_type_
                             << ", "
                             << val_type_
                             << gen_hasher(key_type_)
                             << ">& GetElems() const {return elems_;}\n"
                             << indent(margin, 0)
                             << "inline unordered_map<"
                             << key_type_
                             << ", "
                             << val_type_
                             << gen_hasher(key_type_)
                             << ">& GetElems() {return elems_;}\n"
                             << indent(margin, 0)
                             << "inline void SetElems(const unordered_map<"
                             << key_type_
                             << ", "
                             << val_type_
                             << gen_hasher(key_type_)
                             << ">& elems) {elems_ = elems;}\n"
                             << indent(margin, 0)
                             << "inline void SetElems(unordered_map<"
                             << key_type_
                             << ", "
                             << val_type_
                             << gen_hasher(key_type_)
                             << ">&& elems) {elems_ = std::move(elems);}\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "if (elems_ != rhs.elems_)\n"
                             << indent(margin, 2)
                             << "return false;\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "for (auto& i : rhs.elems_){\n"
                             << indent(margin, 2)
                             << "hash_code ^= "
                             << gen_hasher(key_type_, "i.first")
                             << " ^ "
                             << gen_hasher(val_type_, "i.second")
                             << ";\n"
                             << indent(margin, 1)
                             << "}\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "unordered_map<"
                             << key_type_
                             << ", "
                             << val_type_
                             << gen_hasher(key_type_)
                             << "> elems_;\n";
                     }
            );
    }

    void TypeMapCpp::gen_impl(ostringstream& oss, size_t margin)
    {
        gen_map_read(oss, margin, false, name_, "elems_", key_type_, val_type_);
        gen_map_write(oss, margin, false, name_, "elems_", key_type_, val_type_);
    }

    void TypeStructCpp::gen_header(ostringstream& oss, size_t margin)
    {
        string TTName = name_ + "TypeTree";
        oss << "class "
            << TTName
            << "{\npublic:\n"
            << indent(margin, 1)
            << TTName
            << "(soce::proto::TypeTree& type_tree);\n"
            << indent(margin, 0)
            << "};\n\n";

        gen_operator(oss,
                     margin,
                     name_,
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, -1)
                             << "public:";
                         size_t index = 0;
                         for (auto iter=begin(); iter!=end(); ++iter){
                             iter->second->gen_setter_and_getter(oss, margin, index++);
                         }
                     },
                     [this](ostringstream& oss, size_t margin){
                         for (auto iter=begin(); iter!=end(); ++iter){
                             oss << indent(margin, 1)
                                 << "if ("
                                 << iter->second->get_name()
                                 << " != rhs."
                                 << iter->second->get_name()
                                 << ")\n";
                             oss << indent(margin, 2)
                                 << "return false;\n";
                         }
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "hash_code = 0";
                         for (auto iter=this->begin(); iter!=this->end(); ++iter){
                             iter->second->gen_hash_code(oss, margin);
                         }
                         oss << ";\n";
                     },
                     [this](ostringstream& oss, size_t margin){
                         oss << indent(margin, 1)
                             << "virtual size_t deserialize(soce::proto::ProtoIf* proto);\n"
                             << indent(margin, 1)
                             << "virtual size_t serialize(soce::proto::ProtoIf* proto) const;\n"
                             << indent(margin, 1)
                             << "static soce::proto::TypeTree* get_type_tree();\n";
                     },
                     [&TTName, this](ostringstream& oss, size_t margin){
                         margin += Utils::get_indent();
                         for (auto iter=this->begin(); iter!=this->end(); ++iter){
                             iter->second->gen_class_member(oss, margin);
                         }

                         size_t attrs = 0;
                         size_t index = 0;
                         for (auto iter=begin(); iter!=end(); ++iter){
                             if (iter->second->is_required()){
                                 attrs |= 1<<index;
                             }
                             ++index;
                         }

                         oss << "\n"
                             << indent(margin, 0)
                             << "/* for optional */\n"
                             << indent(margin, 0)
                             << "bitset<"
                             << this->size()
                             << "> attrs_ = "
                             << attrs
                             << ";\n"
                             << indent(margin, 0)
                             << "bitset<"
                             << this->size()
                             << "> status_;\n\n"
                             << indent(margin, -1)
                             << "public:\n"
                             << indent(margin, 0)
                             << "static "
                             << TTName
                             << " s_type_tree;\n";
                     },
                     true);
    }

    void TypeStructCpp::gen_impl(ostringstream& oss, size_t margin)
    {
        gen_read(oss, margin);
        gen_write(oss, margin);

        oss << "soce::proto::TypeTree* "
            << name_
            << "::get_type_tree(){\n"
            << indent(margin, 1)
            << "return &g_type_tree;\n"
            << indent(margin, 0)
            << "}\n\n";

        string TTName = name_ + "TypeTree";
        oss << TTName
            << "::"
            << TTName
            << "(soce::proto::TypeTree& type_tree){\n";

        for (auto iter=begin(); iter!=end(); ++iter){
            string stype =  get_type_nature(iter->second->get_type());
            if (stype == "soce::proto::SoceDataType::kTypeStruct"){
                string s = iter->second->get_type();
                if (s == "soce::crpc::CrpcReqHeader"){
                    s = "CrpcReqHeader";
                }
                else if (s == "soce::crpc::CrpcRespHeader"){
                    s = "CrpcRespHeader";
                }
                stype = "\"" + s + "\"";
            }


            oss << indent(margin, 1)
                << "type_tree.add(\""
                << name_
                << "\", \""
                << iter->second->get_name()
                << "\", "
                << stype
                << ");\n";
        }
        oss << indent(margin, 0)
            << "}\n\n";

        oss << TTName << " " << name_ << "::s_type_tree(g_type_tree);\n\n";
    }

    void TypeStructCpp::gen_read(std::ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << "size_t "
            << name_
            << "::deserialize(soce::proto::ProtoIf* proto){\n"
            << indent(margin, 1)
            << "return read(proto, true, false);\n"
            << indent(margin, 0)
            << "}\n\n";

        oss << indent(margin, 0)
            << "size_t "
            << name_
            << "::read(soce::proto::ProtoIf* proto, bool required, bool with_type){\n"
            << indent(margin, 1)
            << "return soce::proto::SoceProto::read_struct(proto, required, with_type,"
            << elems_.size()
            << ",\n"
            << indent(margin, 2)
            << "[this](uint32_t valid_num, uint32_t schema_elem_num){\n"
            << indent(margin, 3)
            << "for (uint32_t i=valid_num; i<schema_elem_num; ++i){\n"
            << indent(margin, 4)
            << "if (attrs_[i]){\n"
            << indent(margin, 5)
            << "return false;\n"
            << indent(margin, 4)
            << "}\n"
            << indent(margin, 3)
            << "}\n"
            << indent(margin, 2)
            << "return true;\n"
            << indent(margin, 2)
            << "},\n"
            << indent(margin, 2)
            << "[&proto, this](uint32_t valid_num)->uint32_t{\n"
            << indent(margin, 3)
            << "uint32_t len = 0;\n"
            << indent(margin, 3)
            << "uint32_t elem_len = 0;\n"
            << indent(margin, 3)
            << "uint32_t index = 0;\n"
            << indent(margin, 3)
            << "do{\n";

        margin += 4 * Utils::get_indent();
        for (auto iter=begin(); iter!=end(); ++iter){
            oss << indent(margin, 0)
                << "/* read "
                << iter->second->get_name()
                << " */\n"
                << indent(margin, 0)
                << "if (index >= valid_num){\n"
                << indent(margin, 1)
                << "break;\n"
                << indent(margin, 0)
                << "}\n";
            iter->second->gen_read(oss, margin);
            oss << indent(margin, 0)
                << "if (!elem_len){\n"
                << indent(margin, 1)
                << "return 0;\n"
                << indent(margin, 0)
                << "}\n"
                << indent(margin, 0)
                << "else if (elem_len != 1){\n"
                << indent(margin, 1)
                << "status_[index] = true;\n"
                << indent(margin, 0)
                << "}\n"
                << indent(margin, 0)
                << "len += elem_len;\n"
                << indent(margin, 0)
                << "++index;\n\n";
        }

        margin -= 2 * Utils::get_indent();
        oss << indent(margin, 0)
            << "}while(0);\n\n"
            << indent(margin, 0)
            << "return len;\n"
            << indent(margin, -1)
            << "});\n\n"

            << indent(margin, -2)
            << "}\n\n";
    }

    void TypeStructCpp::gen_write(std::ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << "size_t "
            << name_
            << "::serialize(soce::proto::ProtoIf* proto) const {\n"
            << indent(margin, 1)
            << "return write(proto, true, true, false);\n"
            << indent(margin, 0)
            << "}\n";

        oss << indent(margin, 0)
            << "size_t "
            << name_
            << "::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {\n"
            << indent(margin, 1)
            << "return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, "
            << elems_.size()
            << ", [&, this]()->uint32_t{\n"
            << indent(margin, 2)
            << "uint32_t len = 0;\n"
            << indent(margin, 2)
            << "uint32_t elem_len = 0;\n"
            << indent(margin, 2)
            << "uint32_t index = 0;\n"
            << indent(margin, 2)
            << "uint32_t elem_len_pos = 0;\n"
            << indent(margin, 2)
            << "uint32_t len_size = 0;\n"
            << indent(margin, 2)
            << "(void) elem_len_pos;\n"
            << indent(margin, 2)
            << "(void) len_size;\n"
            << indent(margin, 2)
            << "do{\n";

        for (auto iter=begin(); iter!=end(); ++iter){
            iter->second->gen_write(oss, margin + 3 * Utils::get_indent());
            oss << indent(margin, 3)
                << "if (!elem_len){\n"
                << indent(margin, 4)
                << "return 0;\n"
                << indent(margin, 3)
                << "}\n"
                << indent(margin, 3)
                << "len += elem_len;\n"
                << indent(margin, 3)
                << "++index;\n\n";
        }

        oss << indent(margin, 2)
            << "}while(0);\n\n"
            << indent(margin, 2)
            << "return len;\n"
            << indent(margin, 1)
            << "});\n"
            << indent(margin, 0)
            << "}\n\n";
    }

    void TypeStruct::TypeStructElemBase::gen_setter_and_getter(std::ostringstream& oss, size_t margin, size_t index)
    {
        oss << "\n"
            << indent(margin, 0)
            << "void set_"
            << name_
            << "(const "
            << format_type()
            << "& _"
            << name_
            << "){\n"
            << indent(margin, 1)
            << name_
            << " = _"
            << name_
            << ";\n"
            << indent(margin, 1)
            << "status_["
            << index
            << "] = true;\n"
            << indent(margin, 0)
            << "}\n";

        oss << indent(margin, 0)
            << "void set_"
            << name_
            << "("
            << format_type()
            << "&& _"
            << name_
            << "){\n"
            << indent(margin, 1)
            << name_
            << " = std::move(_"
            << name_
            << ");\n"
            << indent(margin, 1)
            << "status_["
            << index
            << "] = true;\n"
            << indent(margin, 0)
            << "}\n";

        oss << indent(margin, 0)
            << "const "
            << format_type()
            << "& get_"
            << name_
            << "() const {\n"
            << indent(margin, 1)
            << "return "
            << name_
            << ";\n"
            << indent(margin, 0)
            << "}\n";

        oss << indent(margin, 0)
            << format_type()
            << "& mutable_"
            << name_
            << "(){\n"
            << indent(margin, 1)
            << "status_["
            << index
            << "] = true;\n"
            << indent(margin, 1)
            << "return "
            << name_
            << ";\n"
            << indent(margin, 0)
            << "}\n";

        oss << indent(margin, 0)
            << "bool has_"
            << name_
            << "() const{\n"
            << indent(margin, 1)
            << "return status_["
            << index
            << "];\n"
            << indent(margin, 0)
            << "}\n";
    }

    void TypeStruct::TypeStructElemBase::gen_class_member(std::ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << format_type()
            << " "
            << name_
            << ";\n";
    }

    void TypeStructElemPrimeCpp::gen_hash_code(std::ostringstream& oss, size_t margin)
    {
        oss << "\n"
            << indent(margin, 2)
            << "^ std::hash<"
            << type_
            << ">()("
            << name_
            << ")";
    }

    void TypeStructElemPrimeCpp::gen_read(std::ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << gen_read_by_type(type_, name_, true);
    }

    void TypeStructElemPrimeCpp::gen_write(std::ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << gen_write_by_type(type_, name_, true);
    }

    void TypeStructElemCustomCpp::gen_hash_code(std::ostringstream& oss, size_t margin)
    {
        string nature = STypeCreater.get_type_nature(type_);
        string type = type_;
        oss << "\n"
            << indent(margin, 2)
            << "^ ";

        if (nature == "enum"){
            oss << "std::hash<int32_t>()(";
        }
        else{
            oss << name_
                << "(";
        }

        oss << name_
            << ")";
    }

    void TypeStructElemCustomCpp::gen_read(std::ostringstream& oss, size_t margin)
    {
        string nature = STypeCreater.get_type_nature(type_);

        if (nature == "enum"){
            oss << indent(margin, 0)
                << "elem_len = soce::proto::SoceProto::read_enum(proto, attrs_[index], *(int32_t*)&"
                << name_
                << ");\n";
        }
        else{
            oss << indent(margin, 0)
                << "elem_len = "
                << name_
                << ".read(proto, attrs_[index], true);\n";
        }
    }

    void TypeStructElemCustomCpp::gen_write(std::ostringstream& oss, size_t margin)
    {
        string nature = STypeCreater.get_type_nature(type_);

        if (nature == "enum"){
            oss << indent(margin, 0)
                << "elem_len = soce::proto::SoceProto::write_enum(proto, attrs_[index], status_[index], (uint32_t)"
                << name_
                << ");\n";
        }
        else{
            oss << indent(margin, 0)
                << "elem_len = "
                << name_
                << ".write(proto, attrs_[index], status_[index], true);\n";
        }
    }

    std::string TypeStructElemByteCpp::format_type() const
    {
        return "char";
    }

    void TypeStructElemByteCpp::gen_hash_code(std::ostringstream& oss, size_t margin){
        oss << "\n"
            << indent(margin, 2)
            << "^ std::hash<string>()(string("
            << name_
            << ","
            << ext_.front()
            << "))";
    }

    void TypeStructElemByteCpp::gen_class_member(std::ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << "char "
            << name_
            << "["
            << ext_.front()
            << "];\n";
    }

    void TypeStructElemByteCpp::gen_read(std::ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << "elem_len = soce::proto::SoceProto::read_bytes(proto, attrs_[index], "
            << name_
            << ", "
            << ext_.front()
            << ");\n";
    }

    void TypeStructElemByteCpp::gen_write(std::ostringstream& oss, size_t margin)
    {
        oss << indent(margin, 0)
            << "elem_len = soce::proto::SoceProto::write_bytes(proto, attrs_[index], status_[index], "
            << name_
            << ", "
            << ext_.front()
            << ");\n";
    }

    void TypeStructElemByteCpp::gen_setter_and_getter(std::ostringstream& oss, size_t margin, size_t index)
    {
        oss << "\n"
            << indent(margin, 0)
            << "void set_"
            << name_
            << "(const "
            << format_type()
            << " _"
            << name_
            << "["
            << ext_.front()
            << "]){\n"
            << indent(margin, 1)
            << "memcpy("
            << name_
            << ", _"
            << name_
            << ", "
            << ext_.front()
            << ");\n"
            << indent(margin, 1)
            << "status_["
            << index
            << "] = true;\n"
            << indent(margin, 0)
            << "}\n";

        oss << indent(margin, 0)
            << "const "
            << format_type()
            << "* get_"
            << name_
            << "() const {\n"
            << indent(margin, 1)
            << "return "
            << name_
            << ";\n"
            << indent(margin, 0)
            << "}\n";

        oss << indent(margin, 0)
            << format_type()
            << "* mutable_"
            << name_
            << "(){\n"
            << indent(margin, 1)
            << "status_["
            << index
            << "] = true;\n"
            << indent(margin, 1)
            << "return "
            << name_
            << ";\n"
            << indent(margin, 0)
            << "}\n";

        oss << indent(margin, 0)
            << "bool IsSet"
            << name_
            << "(){\n"
            << indent(margin, 1)
            << "return status_["
            << index
            << "];\n"
            << indent(margin, 0)
            << "}\n";
    }

    std::string TypeStructElemListCpp::format_type() const{
        string rc = "vector<";
        rc += ext_.front();
        rc += ">";
        return move(rc);
    }

    void TypeStructElemListCpp::gen_hash_code(std::ostringstream& oss, size_t margin)
    {
        oss << "\n"
            << indent(margin, 2)
            << "^ [&, this]{\n"
            << indent(margin, 3)
            << "size_t code = 0;\n"
            << indent(margin, 3)
            << "for (auto& _i : "
            << name_
            << "){\n"
            << indent(margin, 4)
            << "code ^= "
            << gen_hasher(ext_.front(), "_i")
            << ";\n"
            << indent(margin, 3)
            << "}\n"
            << indent(margin, 3)
            << "return code;\n"
            << indent(margin, 2)
            << "}()";
    }

    void TypeStructElemListCpp::gen_read(std::ostringstream& oss, size_t margin)
    {
        gen_list_read(oss, margin, true, name_, name_, ext_.front());
    }

    void TypeStructElemListCpp::gen_write(std::ostringstream& oss, size_t margin)
    {
        gen_list_write(oss, margin, true, name_, name_, ext_.front());
    }

    std::string TypeStructElemSetCpp::format_type() const{
        string rc = "unordered_set<";
        rc += ext_.front();
        rc += gen_hasher(ext_.front());
        rc += ">";
        return move(rc);
    }

    void TypeStructElemSetCpp::gen_hash_code(std::ostringstream& oss, size_t margin)
    {
        oss << "\n"
            << indent(margin, 2)
            << "^ [&, this]{\n"
            << indent(margin, 3)
            << "size_t code = 0;\n"
            << indent(margin, 3)
            << "for (auto& _i : "
            << name_
            << "){\n"
            << indent(margin, 4)
            << "code ^= "
            << gen_hasher(ext_.front(), "_i")
            << ";\n"
            << indent(margin, 3)
            << "}\n"
            << indent(margin, 3)
            << "return code;\n"
            << indent(margin, 2)
            << "}()";
    }

    void TypeStructElemSetCpp::gen_read(std::ostringstream& oss, size_t margin)
    {
        gen_set_read(oss, margin, true, name_, name_, ext_.front());
    }

    void TypeStructElemSetCpp::gen_write(std::ostringstream& oss, size_t margin)
    {
        gen_set_write(oss, margin, true, name_, name_, ext_.front());
    }

    std::string TypeStructElemMapCpp::format_type() const{
        string rc;
        rc += "unordered_map<" + ext_.front() + ", " + ext_.back();
        rc += gen_hasher(ext_.front());
        rc += ">";
        return move(rc);
    }

    void TypeStructElemMapCpp::gen_hash_code(std::ostringstream& oss, size_t margin)
    {
        oss << "\n"
            << indent(margin, 2)
            << "^ [&, this]{\n"
            << indent(margin, 3)
            << "size_t code = 0;\n"
            << indent(margin, 3)
            << "for (auto& _i : "
            << name_
            << "){\n"
            << indent(margin, 4)
            << "code ^= "
            << gen_hasher(ext_.front(), "_i.first")
            << "^ "
            << gen_hasher(ext_.back(), "_i.second")
            << ";\n"
            << indent(margin, 3)
            << "}\n"
            << indent(margin, 3)
            << "return code;\n"
            << indent(margin, 2)
            << "}()";
    }

    void TypeStructElemMapCpp::gen_read(std::ostringstream& oss, size_t margin)
    {
        gen_map_read(oss, margin, true, name_, name_, ext_.front(), ext_.back());
    }

    void TypeStructElemMapCpp::gen_write(std::ostringstream& oss, size_t margin)
    {
        gen_map_write(oss, margin, true, name_, name_, ext_.front(), ext_.back());
    }

} // namespace compiler
} // namespace soce

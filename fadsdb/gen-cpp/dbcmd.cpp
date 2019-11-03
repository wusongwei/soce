#include "dbcmd.h"

namespace soce{ namespace fadsdb{ 
static soce::proto::TypeTree g_type_tree;

size_t Header::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Header::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,2,
        [this](uint32_t valid_num, uint32_t schema_elem_num){
            for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                if (attrs_[i]){
                    return false;
                }
            }
        return true;
        },
        [&proto, this](uint32_t valid_num)->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            uint32_t index = 0;
            do{
                /* read type */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_enum(proto, attrs_[index], *(int32_t*)&type);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read id */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int64(proto, attrs_[index], id);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

        }while(0);

        return len;
    });

}

size_t Header::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Header::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 2, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = soce::proto::SoceProto::write_enum(proto, attrs_[index], status_[index], (uint32_t)type);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_int64(proto, attrs_[index], status_[index], id);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* Header::get_type_tree(){
    return &g_type_tree;
}

HeaderTypeTree::HeaderTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Header", "type", soce::proto::SoceDataType::kTypeEnum);
    type_tree.add("Header", "id", soce::proto::SoceDataType::kTypeInt64);
}

HeaderTypeTree Header::s_type_tree(g_type_tree);

size_t FadsDbCreate::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbCreate::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,2,
        [this](uint32_t valid_num, uint32_t schema_elem_num){
            for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                if (attrs_[i]){
                    return false;
                }
            }
        return true;
        },
        [&proto, this](uint32_t valid_num)->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            uint32_t index = 0;
            do{
                /* read header */
                if (index >= valid_num){
                    break;
                }
                elem_len = header.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read schema */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], schema);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

        }while(0);

        return len;
    });

}

size_t FadsDbCreate::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbCreate::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 2, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = header.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], schema);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* FadsDbCreate::get_type_tree(){
    return &g_type_tree;
}

FadsDbCreateTypeTree::FadsDbCreateTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbCreate", "header", "Header");
    type_tree.add("FadsDbCreate", "schema", soce::proto::SoceDataType::kTypeString);
}

FadsDbCreateTypeTree FadsDbCreate::s_type_tree(g_type_tree);

size_t FadsDbInsert::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbInsert::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,4,
        [this](uint32_t valid_num, uint32_t schema_elem_num){
            for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                if (attrs_[i]){
                    return false;
                }
            }
        return true;
        },
        [&proto, this](uint32_t valid_num)->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            uint32_t index = 0;
            do{
                /* read header */
                if (index >= valid_num){
                    break;
                }
                elem_len = header.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read key */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], key);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read table */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], table);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read data */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], data);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

        }while(0);

        return len;
    });

}

size_t FadsDbInsert::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbInsert::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 4, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = header.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], key);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], table);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], data);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* FadsDbInsert::get_type_tree(){
    return &g_type_tree;
}

FadsDbInsertTypeTree::FadsDbInsertTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbInsert", "header", "Header");
    type_tree.add("FadsDbInsert", "key", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbInsert", "table", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbInsert", "data", soce::proto::SoceDataType::kTypeString);
}

FadsDbInsertTypeTree FadsDbInsert::s_type_tree(g_type_tree);

size_t FadsDbRemove::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbRemove::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,3,
        [this](uint32_t valid_num, uint32_t schema_elem_num){
            for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                if (attrs_[i]){
                    return false;
                }
            }
        return true;
        },
        [&proto, this](uint32_t valid_num)->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            uint32_t index = 0;
            do{
                /* read header */
                if (index >= valid_num){
                    break;
                }
                elem_len = header.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read key */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], key);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read table */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], table);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

        }while(0);

        return len;
    });

}

size_t FadsDbRemove::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbRemove::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 3, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = header.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], key);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], table);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* FadsDbRemove::get_type_tree(){
    return &g_type_tree;
}

FadsDbRemoveTypeTree::FadsDbRemoveTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbRemove", "header", "Header");
    type_tree.add("FadsDbRemove", "key", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbRemove", "table", soce::proto::SoceDataType::kTypeString);
}

FadsDbRemoveTypeTree FadsDbRemove::s_type_tree(g_type_tree);

size_t FadsDbUpdate::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbUpdate::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,5,
        [this](uint32_t valid_num, uint32_t schema_elem_num){
            for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                if (attrs_[i]){
                    return false;
                }
            }
        return true;
        },
        [&proto, this](uint32_t valid_num)->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            uint32_t index = 0;
            do{
                /* read header */
                if (index >= valid_num){
                    break;
                }
                elem_len = header.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read key */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], key);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read table */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], table);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read cmd */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], cmd);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read filters */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], filters);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

        }while(0);

        return len;
    });

}

size_t FadsDbUpdate::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbUpdate::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 5, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = header.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], key);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], table);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], cmd);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], filters);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* FadsDbUpdate::get_type_tree(){
    return &g_type_tree;
}

FadsDbUpdateTypeTree::FadsDbUpdateTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbUpdate", "header", "Header");
    type_tree.add("FadsDbUpdate", "key", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbUpdate", "table", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbUpdate", "cmd", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbUpdate", "filters", soce::proto::SoceDataType::kTypeString);
}

FadsDbUpdateTypeTree FadsDbUpdate::s_type_tree(g_type_tree);

size_t FadsDbSelect::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbSelect::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,5,
        [this](uint32_t valid_num, uint32_t schema_elem_num){
            for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                if (attrs_[i]){
                    return false;
                }
            }
        return true;
        },
        [&proto, this](uint32_t valid_num)->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            uint32_t index = 0;
            do{
                /* read header */
                if (index >= valid_num){
                    break;
                }
                elem_len = header.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read key */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], key);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read table */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], table);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read fields */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], fields);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read filters */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], filters);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

        }while(0);

        return len;
    });

}

size_t FadsDbSelect::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbSelect::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 5, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = header.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], key);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], table);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], fields);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], filters);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* FadsDbSelect::get_type_tree(){
    return &g_type_tree;
}

FadsDbSelectTypeTree::FadsDbSelectTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbSelect", "header", "Header");
    type_tree.add("FadsDbSelect", "key", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelect", "table", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelect", "fields", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelect", "filters", soce::proto::SoceDataType::kTypeString);
}

FadsDbSelectTypeTree FadsDbSelect::s_type_tree(g_type_tree);

size_t FadsDbSelUp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbSelUp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,8,
        [this](uint32_t valid_num, uint32_t schema_elem_num){
            for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                if (attrs_[i]){
                    return false;
                }
            }
        return true;
        },
        [&proto, this](uint32_t valid_num)->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            uint32_t index = 0;
            do{
                /* read header */
                if (index >= valid_num){
                    break;
                }
                elem_len = header.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read key */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], key);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read table */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], table);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read fields */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], fields);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read uncond_cmds */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], uncond_cmds);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read precmds */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], precmds);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read subcmds */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], subcmds);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read filters */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], filters);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

        }while(0);

        return len;
    });

}

size_t FadsDbSelUp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbSelUp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 8, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = header.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], key);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], table);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], fields);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], uncond_cmds);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], precmds);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], subcmds);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], filters);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* FadsDbSelUp::get_type_tree(){
    return &g_type_tree;
}

FadsDbSelUpTypeTree::FadsDbSelUpTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbSelUp", "header", "Header");
    type_tree.add("FadsDbSelUp", "key", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelUp", "table", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelUp", "fields", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelUp", "uncond_cmds", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelUp", "precmds", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelUp", "subcmds", soce::proto::SoceDataType::kTypeString);
    type_tree.add("FadsDbSelUp", "filters", soce::proto::SoceDataType::kTypeString);
}

FadsDbSelUpTypeTree FadsDbSelUp::s_type_tree(g_type_tree);

size_t FadsDbResponse::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbResponse::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,3,
        [this](uint32_t valid_num, uint32_t schema_elem_num){
            for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                if (attrs_[i]){
                    return false;
                }
            }
        return true;
        },
        [&proto, this](uint32_t valid_num)->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            uint32_t index = 0;
            do{
                /* read id */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int64(proto, attrs_[index], id);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read status */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_enum(proto, attrs_[index], *(int32_t*)&status);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read response */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], response);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

        }while(0);

        return len;
    });

}

size_t FadsDbResponse::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbResponse::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 3, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = soce::proto::SoceProto::write_int64(proto, attrs_[index], status_[index], id);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_enum(proto, attrs_[index], status_[index], (uint32_t)status);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], response);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* FadsDbResponse::get_type_tree(){
    return &g_type_tree;
}

FadsDbResponseTypeTree::FadsDbResponseTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbResponse", "id", soce::proto::SoceDataType::kTypeInt64);
    type_tree.add("FadsDbResponse", "status", soce::proto::SoceDataType::kTypeEnum);
    type_tree.add("FadsDbResponse", "response", soce::proto::SoceDataType::kTypeString);
}

FadsDbResponseTypeTree FadsDbResponse::s_type_tree(g_type_tree);

}}//end namespace



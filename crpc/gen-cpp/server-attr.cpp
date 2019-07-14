#include "server-attr.h"

namespace soce{ namespace crpc{ namespace attr{ 
static soce::proto::TypeTree g_type_tree;

size_t MethodFilters::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t MethodFilters::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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
                /* read filter */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], filter);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read val_type */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_map(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeString,
                    soce::proto::SoceDataType::kTypeInt32,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        string key;
                        elem_len = proto->read_string(key);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        int32_t val;
                        elem_len = proto->read_int32(val);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        val_type[key] = val;
                        return len;
                    });
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

size_t MethodFilters::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t MethodFilters::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 2, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], filter);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

                elem_len = soce::proto::SoceProto::write_map(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    val_type.size(),
                    soce::proto::SoceDataType::kTypeString,
                    soce::proto::SoceDataType::kTypeInt32,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : val_type){
                            elem_len = proto->write_string(elem.first);
                            if (!elem_len){
                                return 0;
                            }
                            len += elem_len;
                            elem_len = proto->write_int32(elem.second);
                            if (!elem_len){
                                return 0;
                            }
                            len += elem_len;
                        }
                        return len;
                    });
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* MethodFilters::get_type_tree(){
    return &g_type_tree;
}

MethodFiltersTypeTree::MethodFiltersTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("MethodFilters", "filter", soce::proto::SoceDataType::kTypeString);
    type_tree.add("MethodFilters", "val_type", soce::proto::SoceDataType::kTypeMap);
}

MethodFiltersTypeTree MethodFilters::s_type_tree(g_type_tree);

size_t ServerAttrs::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t ServerAttrs::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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
                /* read name */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], name);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read methods */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_map(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeString,
                    soce::proto::SoceDataType::kTypeStruct,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        string key;
                        elem_len = proto->read_string(key);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        MethodFilters val;
                        elem_len = val.read(proto, true, false);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        methods[key] = val;
                        return len;
                    });
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

size_t ServerAttrs::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t ServerAttrs::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 2, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], name);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

                elem_len = soce::proto::SoceProto::write_map(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    methods.size(),
                    soce::proto::SoceDataType::kTypeString,
                    soce::proto::SoceDataType::kTypeStruct,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : methods){
                            elem_len = proto->write_string(elem.first);
                            if (!elem_len){
                                return 0;
                            }
                            len += elem_len;
                            elem_len = elem.second.write(proto, true, true, false);
                            if (!elem_len){
                                return 0;
                            }
                            len += elem_len;
                        }
                        return len;
                    });
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* ServerAttrs::get_type_tree(){
    return &g_type_tree;
}

ServerAttrsTypeTree::ServerAttrsTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("ServerAttrs", "name", soce::proto::SoceDataType::kTypeString);
    type_tree.add("ServerAttrs", "methods", soce::proto::SoceDataType::kTypeMap);
}

ServerAttrsTypeTree ServerAttrs::s_type_tree(g_type_tree);

size_t ProcessorAttrs::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t ProcessorAttrs::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,1,
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
                /* read server_attrs */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_list(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeString,
                    [this](uint32_t size){
                        server_attrs.resize(size);
                    },
                    [&proto, this](uint32_t i){
                        uint32_t elem_len = 0;
                        elem_len = proto->read_string(server_attrs[i]);
                        return elem_len;
                    });
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

size_t ProcessorAttrs::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t ProcessorAttrs::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 1, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
                elem_len = soce::proto::SoceProto::write_list(proto,
                attrs_[index],
                status_[index],
                true,
                server_attrs.size(),
                soce::proto::SoceDataType::kTypeString,
                [&proto, this](uint32_t i)->uint32_t{
                    uint32_t elem_len = 0;
                    elem_len = proto->write_string(server_attrs[i]);
                    return elem_len;
                });
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* ProcessorAttrs::get_type_tree(){
    return &g_type_tree;
}

ProcessorAttrsTypeTree::ProcessorAttrsTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("ProcessorAttrs", "server_attrs", soce::proto::SoceDataType::kTypeList);
}

ProcessorAttrsTypeTree ProcessorAttrs::s_type_tree(g_type_tree);

}}}//end namespace



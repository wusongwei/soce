#include "crpc-header.h"

namespace soce{ namespace crpc{ 
static soce::proto::TypeTree g_type_tree;

size_t CrpcReqHeader::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CrpcReqHeader::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,7,
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

                /* read service */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], service);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read method */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], method);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read req_id */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int64(proto, attrs_[index], req_id);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read tid */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], tid);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read target_addr */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], target_addr);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ext */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_map(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeString,
                    soce::proto::SoceDataType::kTypeString,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        string key;
                        elem_len = proto->read_string(key);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        string val;
                        elem_len = proto->read_string(val);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        ext[key] = val;
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

size_t CrpcReqHeader::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CrpcReqHeader::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 7, [&, this]()->uint32_t{
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

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], service);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], method);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_int64(proto, attrs_[index], status_[index], req_id);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], tid);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], target_addr);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

                elem_len = soce::proto::SoceProto::write_map(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    ext.size(),
                    soce::proto::SoceDataType::kTypeString,
                    soce::proto::SoceDataType::kTypeString,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : ext){
                            elem_len = proto->write_string(elem.first);
                            if (!elem_len){
                                return 0;
                            }
                            len += elem_len;
                            elem_len = proto->write_string(elem.second);
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

soce::proto::TypeTree* CrpcReqHeader::get_type_tree(){
    return &g_type_tree;
}

CrpcReqHeaderTypeTree::CrpcReqHeaderTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CrpcReqHeader", "type", soce::proto::SoceDataType::kTypeEnum);
    type_tree.add("CrpcReqHeader", "service", soce::proto::SoceDataType::kTypeString);
    type_tree.add("CrpcReqHeader", "method", soce::proto::SoceDataType::kTypeString);
    type_tree.add("CrpcReqHeader", "req_id", soce::proto::SoceDataType::kTypeInt64);
    type_tree.add("CrpcReqHeader", "tid", soce::proto::SoceDataType::kTypeInt32);
    type_tree.add("CrpcReqHeader", "target_addr", soce::proto::SoceDataType::kTypeString);
    type_tree.add("CrpcReqHeader", "ext", soce::proto::SoceDataType::kTypeMap);
}

CrpcReqHeaderTypeTree CrpcReqHeader::s_type_tree(g_type_tree);

size_t CrpcRespHeader::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CrpcRespHeader::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read req_id */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int64(proto, attrs_[index], req_id);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read tid */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], tid);
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

                /* read ext */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_map(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeString,
                    soce::proto::SoceDataType::kTypeString,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        string key;
                        elem_len = proto->read_string(key);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        string val;
                        elem_len = proto->read_string(val);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        ext[key] = val;
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

size_t CrpcRespHeader::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CrpcRespHeader::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 5, [&, this]()->uint32_t{
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

            elem_len = soce::proto::SoceProto::write_int64(proto, attrs_[index], status_[index], req_id);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], tid);
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

                elem_len = soce::proto::SoceProto::write_map(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    ext.size(),
                    soce::proto::SoceDataType::kTypeString,
                    soce::proto::SoceDataType::kTypeString,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : ext){
                            elem_len = proto->write_string(elem.first);
                            if (!elem_len){
                                return 0;
                            }
                            len += elem_len;
                            elem_len = proto->write_string(elem.second);
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

soce::proto::TypeTree* CrpcRespHeader::get_type_tree(){
    return &g_type_tree;
}

CrpcRespHeaderTypeTree::CrpcRespHeaderTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CrpcRespHeader", "type", soce::proto::SoceDataType::kTypeEnum);
    type_tree.add("CrpcRespHeader", "req_id", soce::proto::SoceDataType::kTypeInt64);
    type_tree.add("CrpcRespHeader", "tid", soce::proto::SoceDataType::kTypeInt32);
    type_tree.add("CrpcRespHeader", "status", soce::proto::SoceDataType::kTypeEnum);
    type_tree.add("CrpcRespHeader", "ext", soce::proto::SoceDataType::kTypeMap);
}

CrpcRespHeaderTypeTree CrpcRespHeader::s_type_tree(g_type_tree);

size_t CrpcErrorResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CrpcErrorResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

        }while(0);

        return len;
    });

}

size_t CrpcErrorResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CrpcErrorResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 1, [&, this]()->uint32_t{
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

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* CrpcErrorResp::get_type_tree(){
    return &g_type_tree;
}

CrpcErrorRespTypeTree::CrpcErrorRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CrpcErrorResp", "header", "CrpcRespHeader");
}

CrpcErrorRespTypeTree CrpcErrorResp::s_type_tree(g_type_tree);

}}//end namespace



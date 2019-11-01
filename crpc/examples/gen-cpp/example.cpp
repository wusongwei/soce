#include "example.h"

namespace crpc{ namespace example{ 
static soce::proto::TypeTree g_type_tree;
static soce::crpc::CrpcReqHeaderTypeTree s_crpc_req_header_type_tree(g_type_tree);
static soce::crpc::CrpcRespHeaderTypeTree s_crpc_resp_header_type_tree(g_type_tree);

size_t SyncDemohelloResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t SyncDemohelloResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read result */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], result);
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

size_t SyncDemohelloResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t SyncDemohelloResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], result);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* SyncDemohelloResp::get_type_tree(){
    return &g_type_tree;
}

SyncDemohelloRespTypeTree::SyncDemohelloRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("SyncDemohelloResp", "header", "CrpcRespHeader");
    type_tree.add("SyncDemohelloResp", "result", soce::proto::SoceDataType::kTypeString);
}

SyncDemohelloRespTypeTree SyncDemohelloResp::s_type_tree(g_type_tree);

size_t SyncDemohelloReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t SyncDemohelloReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read msg */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], msg);
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

size_t SyncDemohelloReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t SyncDemohelloReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], msg);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* SyncDemohelloReq::get_type_tree(){
    return &g_type_tree;
}

SyncDemohelloReqTypeTree::SyncDemohelloReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("SyncDemohelloReq", "header", "CrpcReqHeader");
    type_tree.add("SyncDemohelloReq", "msg", soce::proto::SoceDataType::kTypeString);
}

SyncDemohelloReqTypeTree SyncDemohelloReq::s_type_tree(g_type_tree);

size_t CortDemoechoResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemoechoResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read result */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], result);
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

size_t CortDemoechoResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemoechoResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], result);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* CortDemoechoResp::get_type_tree(){
    return &g_type_tree;
}

CortDemoechoRespTypeTree::CortDemoechoRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemoechoResp", "header", "CrpcRespHeader");
    type_tree.add("CortDemoechoResp", "result", soce::proto::SoceDataType::kTypeInt32);
}

CortDemoechoRespTypeTree CortDemoechoResp::s_type_tree(g_type_tree);

size_t CortDemoechoReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemoechoReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read p */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], p);
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

size_t CortDemoechoReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemoechoReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], p);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* CortDemoechoReq::get_type_tree(){
    return &g_type_tree;
}

CortDemoechoReqTypeTree::CortDemoechoReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemoechoReq", "header", "CrpcReqHeader");
    type_tree.add("CortDemoechoReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

CortDemoechoReqTypeTree CortDemoechoReq::s_type_tree(g_type_tree);

size_t CortDemotest_voidResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemotest_voidResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t CortDemotest_voidResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemotest_voidResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* CortDemotest_voidResp::get_type_tree(){
    return &g_type_tree;
}

CortDemotest_voidRespTypeTree::CortDemotest_voidRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemotest_voidResp", "header", "CrpcRespHeader");
}

CortDemotest_voidRespTypeTree CortDemotest_voidResp::s_type_tree(g_type_tree);

size_t CortDemotest_voidReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemotest_voidReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read p */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], p);
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

size_t CortDemotest_voidReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemotest_voidReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], p);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* CortDemotest_voidReq::get_type_tree(){
    return &g_type_tree;
}

CortDemotest_voidReqTypeTree::CortDemotest_voidReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemotest_voidReq", "header", "CrpcReqHeader");
    type_tree.add("CortDemotest_voidReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

CortDemotest_voidReqTypeTree CortDemotest_voidReq::s_type_tree(g_type_tree);

size_t CortDemotest_nullReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemotest_nullReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read p */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], p);
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

size_t CortDemotest_nullReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemotest_nullReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], p);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* CortDemotest_nullReq::get_type_tree(){
    return &g_type_tree;
}

CortDemotest_nullReqTypeTree::CortDemotest_nullReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemotest_nullReq", "header", "CrpcReqHeader");
    type_tree.add("CortDemotest_nullReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

CortDemotest_nullReqTypeTree CortDemotest_nullReq::s_type_tree(g_type_tree);

size_t CortDemotest_condcastResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemotest_condcastResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t CortDemotest_condcastResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemotest_condcastResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* CortDemotest_condcastResp::get_type_tree(){
    return &g_type_tree;
}

CortDemotest_condcastRespTypeTree::CortDemotest_condcastRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemotest_condcastResp", "header", "CrpcRespHeader");
}

CortDemotest_condcastRespTypeTree CortDemotest_condcastResp::s_type_tree(g_type_tree);

size_t CortDemotest_condcastReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemotest_condcastReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read p */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], p);
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

size_t CortDemotest_condcastReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemotest_condcastReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], p);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* CortDemotest_condcastReq::get_type_tree(){
    return &g_type_tree;
}

CortDemotest_condcastReqTypeTree::CortDemotest_condcastReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemotest_condcastReq", "header", "CrpcReqHeader");
    type_tree.add("CortDemotest_condcastReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

CortDemotest_condcastReqTypeTree CortDemotest_condcastReq::s_type_tree(g_type_tree);

size_t CortDemotest_uncondcastResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemotest_uncondcastResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t CortDemotest_uncondcastResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemotest_uncondcastResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* CortDemotest_uncondcastResp::get_type_tree(){
    return &g_type_tree;
}

CortDemotest_uncondcastRespTypeTree::CortDemotest_uncondcastRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemotest_uncondcastResp", "header", "CrpcRespHeader");
}

CortDemotest_uncondcastRespTypeTree CortDemotest_uncondcastResp::s_type_tree(g_type_tree);

size_t CortDemotest_uncondcastReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t CortDemotest_uncondcastReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read p */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], p);
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

size_t CortDemotest_uncondcastReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t CortDemotest_uncondcastReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], p);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* CortDemotest_uncondcastReq::get_type_tree(){
    return &g_type_tree;
}

CortDemotest_uncondcastReqTypeTree::CortDemotest_uncondcastReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("CortDemotest_uncondcastReq", "header", "CrpcReqHeader");
    type_tree.add("CortDemotest_uncondcastReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

CortDemotest_uncondcastReqTypeTree CortDemotest_uncondcastReq::s_type_tree(g_type_tree);

}}//end namespace



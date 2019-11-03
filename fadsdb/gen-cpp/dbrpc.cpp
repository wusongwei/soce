#include "dbrpc.h"

namespace soce{ namespace fadsdb{ 
static soce::proto::TypeTree g_type_tree;
static soce::crpc::CrpcReqHeaderTypeTree s_crpc_req_header_type_tree(g_type_tree);
static soce::crpc::CrpcRespHeaderTypeTree s_crpc_resp_header_type_tree(g_type_tree);

size_t FadsDbServiceexecResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbServiceexecResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t FadsDbServiceexecResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbServiceexecResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* FadsDbServiceexecResp::get_type_tree(){
    return &g_type_tree;
}

FadsDbServiceexecRespTypeTree::FadsDbServiceexecRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbServiceexecResp", "header", "CrpcRespHeader");
    type_tree.add("FadsDbServiceexecResp", "result", soce::proto::SoceDataType::kTypeString);
}

FadsDbServiceexecRespTypeTree FadsDbServiceexecResp::s_type_tree(g_type_tree);

size_t FadsDbServiceexecReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t FadsDbServiceexecReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read req */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], req);
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

size_t FadsDbServiceexecReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t FadsDbServiceexecReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_int64(proto, attrs_[index], status_[index], id);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], req);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* FadsDbServiceexecReq::get_type_tree(){
    return &g_type_tree;
}

FadsDbServiceexecReqTypeTree::FadsDbServiceexecReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("FadsDbServiceexecReq", "header", "CrpcReqHeader");
    type_tree.add("FadsDbServiceexecReq", "id", soce::proto::SoceDataType::kTypeInt64);
    type_tree.add("FadsDbServiceexecReq", "req", soce::proto::SoceDataType::kTypeString);
}

FadsDbServiceexecReqTypeTree FadsDbServiceexecReq::s_type_tree(g_type_tree);

}}//end namespace



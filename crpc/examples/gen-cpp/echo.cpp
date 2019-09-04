#include "echo.h"

namespace crpc{ namespace sample{ 
static soce::proto::TypeTree g_type_tree;
static soce::crpc::CrpcReqHeaderTypeTree s_crpc_req_header_type_tree(g_type_tree);
static soce::crpc::CrpcRespHeaderTypeTree s_crpc_resp_header_type_tree(g_type_tree);

size_t EchoechoResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t EchoechoResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t EchoechoResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t EchoechoResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* EchoechoResp::get_type_tree(){
    return &g_type_tree;
}

EchoechoRespTypeTree::EchoechoRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("EchoechoResp", "header", "CrpcRespHeader");
    type_tree.add("EchoechoResp", "result", soce::proto::SoceDataType::kTypeInt32);
}

EchoechoRespTypeTree EchoechoResp::s_type_tree(g_type_tree);

size_t EchoechoReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t EchoechoReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t EchoechoReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t EchoechoReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* EchoechoReq::get_type_tree(){
    return &g_type_tree;
}

EchoechoReqTypeTree::EchoechoReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("EchoechoReq", "header", "CrpcReqHeader");
    type_tree.add("EchoechoReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

EchoechoReqTypeTree EchoechoReq::s_type_tree(g_type_tree);

size_t Echodo_void_testResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Echodo_void_testResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t Echodo_void_testResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Echodo_void_testResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* Echodo_void_testResp::get_type_tree(){
    return &g_type_tree;
}

Echodo_void_testRespTypeTree::Echodo_void_testRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Echodo_void_testResp", "header", "CrpcRespHeader");
}

Echodo_void_testRespTypeTree Echodo_void_testResp::s_type_tree(g_type_tree);

size_t Echodo_void_testReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Echodo_void_testReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t Echodo_void_testReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Echodo_void_testReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* Echodo_void_testReq::get_type_tree(){
    return &g_type_tree;
}

Echodo_void_testReqTypeTree::Echodo_void_testReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Echodo_void_testReq", "header", "CrpcReqHeader");
    type_tree.add("Echodo_void_testReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

Echodo_void_testReqTypeTree Echodo_void_testReq::s_type_tree(g_type_tree);

size_t Echodo_null_testReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Echodo_null_testReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t Echodo_null_testReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Echodo_null_testReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* Echodo_null_testReq::get_type_tree(){
    return &g_type_tree;
}

Echodo_null_testReqTypeTree::Echodo_null_testReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Echodo_null_testReq", "header", "CrpcReqHeader");
    type_tree.add("Echodo_null_testReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

Echodo_null_testReqTypeTree Echodo_null_testReq::s_type_tree(g_type_tree);

size_t Echodo_condcast_testResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Echodo_condcast_testResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t Echodo_condcast_testResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Echodo_condcast_testResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* Echodo_condcast_testResp::get_type_tree(){
    return &g_type_tree;
}

Echodo_condcast_testRespTypeTree::Echodo_condcast_testRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Echodo_condcast_testResp", "header", "CrpcRespHeader");
}

Echodo_condcast_testRespTypeTree Echodo_condcast_testResp::s_type_tree(g_type_tree);

size_t Echodo_condcast_testReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Echodo_condcast_testReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t Echodo_condcast_testReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Echodo_condcast_testReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* Echodo_condcast_testReq::get_type_tree(){
    return &g_type_tree;
}

Echodo_condcast_testReqTypeTree::Echodo_condcast_testReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Echodo_condcast_testReq", "header", "CrpcReqHeader");
    type_tree.add("Echodo_condcast_testReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

Echodo_condcast_testReqTypeTree Echodo_condcast_testReq::s_type_tree(g_type_tree);

size_t Echodo_uncondcast_testResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Echodo_uncondcast_testResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t Echodo_uncondcast_testResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Echodo_uncondcast_testResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* Echodo_uncondcast_testResp::get_type_tree(){
    return &g_type_tree;
}

Echodo_uncondcast_testRespTypeTree::Echodo_uncondcast_testRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Echodo_uncondcast_testResp", "header", "CrpcRespHeader");
}

Echodo_uncondcast_testRespTypeTree Echodo_uncondcast_testResp::s_type_tree(g_type_tree);

size_t Echodo_uncondcast_testReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Echodo_uncondcast_testReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t Echodo_uncondcast_testReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Echodo_uncondcast_testReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* Echodo_uncondcast_testReq::get_type_tree(){
    return &g_type_tree;
}

Echodo_uncondcast_testReqTypeTree::Echodo_uncondcast_testReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Echodo_uncondcast_testReq", "header", "CrpcReqHeader");
    type_tree.add("Echodo_uncondcast_testReq", "p", soce::proto::SoceDataType::kTypeInt32);
}

Echodo_uncondcast_testReqTypeTree Echodo_uncondcast_testReq::s_type_tree(g_type_tree);

size_t GatewayrecvResp::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t GatewayrecvResp::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t GatewayrecvResp::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t GatewayrecvResp::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* GatewayrecvResp::get_type_tree(){
    return &g_type_tree;
}

GatewayrecvRespTypeTree::GatewayrecvRespTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("GatewayrecvResp", "header", "CrpcRespHeader");
}

GatewayrecvRespTypeTree GatewayrecvResp::s_type_tree(g_type_tree);

size_t GatewayrecvReq::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t GatewayrecvReq::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t GatewayrecvReq::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t GatewayrecvReq::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

soce::proto::TypeTree* GatewayrecvReq::get_type_tree(){
    return &g_type_tree;
}

GatewayrecvReqTypeTree::GatewayrecvReqTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("GatewayrecvReq", "header", "CrpcReqHeader");
    type_tree.add("GatewayrecvReq", "msg", soce::proto::SoceDataType::kTypeString);
}

GatewayrecvReqTypeTree GatewayrecvReq::s_type_tree(g_type_tree);

}}//end namespace



#include "proxy-msg.h"

namespace soce{ namespace crpc{ namespace proxy{ 
static soce::proto::TypeTree g_type_tree;

size_t ProxyMsgServerInfo::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t ProxyMsgServerInfo::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read server_addr */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], server_addr);
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

size_t ProxyMsgServerInfo::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t ProxyMsgServerInfo::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], server_addr);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* ProxyMsgServerInfo::get_type_tree(){
    return &g_type_tree;
}

ProxyMsgServerInfoTypeTree::ProxyMsgServerInfoTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("ProxyMsgServerInfo", "type", soce::proto::SoceDataType::kTypeEnum);
    type_tree.add("ProxyMsgServerInfo", "server_addr", soce::proto::SoceDataType::kTypeString);
}

ProxyMsgServerInfoTypeTree ProxyMsgServerInfo::s_type_tree(g_type_tree);

size_t ProxyMsgRegisterInfo::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t ProxyMsgRegisterInfo::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

size_t ProxyMsgRegisterInfo::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t ProxyMsgRegisterInfo::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 3, [&, this]()->uint32_t{
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

soce::proto::TypeTree* ProxyMsgRegisterInfo::get_type_tree(){
    return &g_type_tree;
}

ProxyMsgRegisterInfoTypeTree::ProxyMsgRegisterInfoTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("ProxyMsgRegisterInfo", "type", soce::proto::SoceDataType::kTypeEnum);
    type_tree.add("ProxyMsgRegisterInfo", "service", soce::proto::SoceDataType::kTypeString);
    type_tree.add("ProxyMsgRegisterInfo", "filters", soce::proto::SoceDataType::kTypeString);
}

ProxyMsgRegisterInfoTypeTree ProxyMsgRegisterInfo::s_type_tree(g_type_tree);

size_t ProxyMsgWatchService::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t ProxyMsgWatchService::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

        }while(0);

        return len;
    });

}

size_t ProxyMsgWatchService::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t ProxyMsgWatchService::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], service);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* ProxyMsgWatchService::get_type_tree(){
    return &g_type_tree;
}

ProxyMsgWatchServiceTypeTree::ProxyMsgWatchServiceTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("ProxyMsgWatchService", "type", soce::proto::SoceDataType::kTypeEnum);
    type_tree.add("ProxyMsgWatchService", "service", soce::proto::SoceDataType::kTypeString);
}

ProxyMsgWatchServiceTypeTree ProxyMsgWatchService::s_type_tree(g_type_tree);

}}}//end namespace



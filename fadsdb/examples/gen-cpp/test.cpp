#include "test.h"

namespace fads{ 
static soce::proto::TypeTree g_type_tree;

size_t Worker::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t Worker::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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

                /* read age */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], age);
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

size_t Worker::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t Worker::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
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

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], age);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* Worker::get_type_tree(){
    return &g_type_tree;
}

WorkerTypeTree::WorkerTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("Worker", "name", soce::proto::SoceDataType::kTypeString);
    type_tree.add("Worker", "age", soce::proto::SoceDataType::kTypeInt32);
}

WorkerTypeTree Worker::s_type_tree(g_type_tree);

size_t company::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t company::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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
                /* read CEO */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], CEO);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read worker */
                if (index >= valid_num){
                    break;
                }
                elem_len = worker.read(proto, attrs_[index], true);
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

size_t company::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t company::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 2, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], CEO);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = worker.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* company::get_type_tree(){
    return &g_type_tree;
}

companyTypeTree::companyTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("company", "CEO", soce::proto::SoceDataType::kTypeString);
    type_tree.add("company", "worker", "Worker");
}

companyTypeTree company::s_type_tree(g_type_tree);

}//end namespace



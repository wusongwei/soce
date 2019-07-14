#include "sample.h"

namespace fads{ namespace sample{ 
static soce::proto::TypeTree g_type_tree;

size_t PrimaryType::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t PrimaryType::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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
                /* read a */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_bool(proto, attrs_[index], a);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read b */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], b);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read c */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int64(proto, attrs_[index], c);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read d */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_float(proto, attrs_[index], d);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read e */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_double(proto, attrs_[index], e);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read f */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_string(proto, attrs_[index], f);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read g */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_bytes(proto, attrs_[index], g, 4);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read color */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_enum(proto, attrs_[index], *(int32_t*)&color);
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

size_t PrimaryType::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t PrimaryType::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 8, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = soce::proto::SoceProto::write_bool(proto, attrs_[index], status_[index], a);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], b);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_int64(proto, attrs_[index], status_[index], c);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_float(proto, attrs_[index], status_[index], d);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_double(proto, attrs_[index], status_[index], e);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_string(proto, attrs_[index], status_[index], f);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_bytes(proto, attrs_[index], status_[index], g, 4);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_enum(proto, attrs_[index], status_[index], (uint32_t)color);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* PrimaryType::get_type_tree(){
    return &g_type_tree;
}

PrimaryTypeTypeTree::PrimaryTypeTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("PrimaryType", "a", soce::proto::SoceDataType::kTypeBool);
    type_tree.add("PrimaryType", "b", soce::proto::SoceDataType::kTypeInt32);
    type_tree.add("PrimaryType", "c", soce::proto::SoceDataType::kTypeInt64);
    type_tree.add("PrimaryType", "d", soce::proto::SoceDataType::kTypeFloat);
    type_tree.add("PrimaryType", "e", soce::proto::SoceDataType::kTypeDouble);
    type_tree.add("PrimaryType", "f", soce::proto::SoceDataType::kTypeString);
    type_tree.add("PrimaryType", "g", soce::proto::SoceDataType::kTypeBytes);
    type_tree.add("PrimaryType", "color", soce::proto::SoceDataType::kTypeEnum);
}

PrimaryTypeTypeTree PrimaryType::s_type_tree(g_type_tree);

size_t ComplexType::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t ComplexType::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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
                /* read pt */
                if (index >= valid_num){
                    break;
                }
                elem_len = pt.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read a */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], a);
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

size_t ComplexType::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t ComplexType::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 2, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = pt.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], a);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* ComplexType::get_type_tree(){
    return &g_type_tree;
}

ComplexTypeTypeTree::ComplexTypeTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("ComplexType", "pt", "PrimaryType");
    type_tree.add("ComplexType", "a", soce::proto::SoceDataType::kTypeInt32);
}

ComplexTypeTypeTree ComplexType::s_type_tree(g_type_tree);

size_t SampleAsElem::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t SampleAsElem::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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
                /* read a */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_int32(proto, attrs_[index], a);
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

size_t SampleAsElem::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t SampleAsElem::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 1, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = soce::proto::SoceProto::write_int32(proto, attrs_[index], status_[index], a);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

        }while(0);

        return len;
    });
}

soce::proto::TypeTree* SampleAsElem::get_type_tree(){
    return &g_type_tree;
}

SampleAsElemTypeTree::SampleAsElemTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("SampleAsElem", "a", soce::proto::SoceDataType::kTypeInt32);
}

SampleAsElemTypeTree SampleAsElem::s_type_tree(g_type_tree);

size_t OutterPrimayList::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_list(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeInt32,
        [this](uint32_t size){
            elems_.resize(size);
        },
        [&proto, this](uint32_t i){
            uint32_t elem_len = 0;
            elem_len = proto->read_int32(elems_[i]);
            return elem_len;
        });
    return elem_len;
};

size_t OutterPrimayList::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_list(proto,
    required,
    has_set,
    with_type,
    elems_.size(),
    soce::proto::SoceDataType::kTypeInt32,
    [&proto, this](uint32_t i)->uint32_t{
        uint32_t elem_len = 0;
        elem_len = proto->write_int32(elems_[i]);
        return elem_len;
    });
    return elem_len;
}

size_t OutterComplexList::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_list(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeStruct,
        [this](uint32_t size){
            elems_.resize(size);
        },
        [&proto, this](uint32_t i){
            uint32_t elem_len = 0;
            elem_len = elems_[i].read(proto, true, false);
            return elem_len;
        });
    return elem_len;
};

size_t OutterComplexList::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_list(proto,
    required,
    has_set,
    with_type,
    elems_.size(),
    soce::proto::SoceDataType::kTypeStruct,
    [&proto, this](uint32_t i)->uint32_t{
        uint32_t elem_len = 0;
        elem_len = elems_[i].write(proto, true, true, false);
        return elem_len;
    });
    return elem_len;
}

size_t OutterMultiList::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_list(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeList,
        [this](uint32_t size){
            elems_.resize(size);
        },
        [&proto, this](uint32_t i){
            uint32_t elem_len = 0;
            elem_len = elems_[i].read(proto, true, false);
            return elem_len;
        });
    return elem_len;
};

size_t OutterMultiList::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_list(proto,
    required,
    has_set,
    with_type,
    elems_.size(),
    soce::proto::SoceDataType::kTypeList,
    [&proto, this](uint32_t i)->uint32_t{
        uint32_t elem_len = 0;
        elem_len = elems_[i].write(proto, true, true, false);
        return elem_len;
    });
    return elem_len;
}

size_t SampleList::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t SampleList::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,6,
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
                /* read opl */
                if (index >= valid_num){
                    break;
                }
                elem_len = opl.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ocl */
                if (index >= valid_num){
                    break;
                }
                elem_len = ocl.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read oml */
                if (index >= valid_num){
                    break;
                }
                elem_len = oml.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ipl */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_list(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeInt32,
                    [this](uint32_t size){
                        ipl.resize(size);
                    },
                    [&proto, this](uint32_t i){
                        uint32_t elem_len = 0;
                        elem_len = proto->read_int32(ipl[i]);
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

                /* read icl */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_list(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeStruct,
                    [this](uint32_t size){
                        icl.resize(size);
                    },
                    [&proto, this](uint32_t i){
                        uint32_t elem_len = 0;
                        elem_len = icl[i].read(proto, true, false);
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

                /* read iml */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_list(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeList,
                    [this](uint32_t size){
                        iml.resize(size);
                    },
                    [&proto, this](uint32_t i){
                        uint32_t elem_len = 0;
                        elem_len = iml[i].read(proto, true, false);
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

size_t SampleList::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t SampleList::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 6, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = opl.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = ocl.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = oml.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

                elem_len = soce::proto::SoceProto::write_list(proto,
                attrs_[index],
                status_[index],
                true,
                ipl.size(),
                soce::proto::SoceDataType::kTypeInt32,
                [&proto, this](uint32_t i)->uint32_t{
                    uint32_t elem_len = 0;
                    elem_len = proto->write_int32(ipl[i]);
                    return elem_len;
                });
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

                elem_len = soce::proto::SoceProto::write_list(proto,
                attrs_[index],
                status_[index],
                true,
                icl.size(),
                soce::proto::SoceDataType::kTypeStruct,
                [&proto, this](uint32_t i)->uint32_t{
                    uint32_t elem_len = 0;
                    elem_len = icl[i].write(proto, true, true, false);
                    return elem_len;
                });
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

                elem_len = soce::proto::SoceProto::write_list(proto,
                attrs_[index],
                status_[index],
                true,
                iml.size(),
                soce::proto::SoceDataType::kTypeList,
                [&proto, this](uint32_t i)->uint32_t{
                    uint32_t elem_len = 0;
                    elem_len = iml[i].write(proto, true, true, false);
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

soce::proto::TypeTree* SampleList::get_type_tree(){
    return &g_type_tree;
}

SampleListTypeTree::SampleListTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("SampleList", "opl", soce::proto::SoceDataType::kTypeList);
    type_tree.add("SampleList", "ocl", soce::proto::SoceDataType::kTypeList);
    type_tree.add("SampleList", "oml", soce::proto::SoceDataType::kTypeList);
    type_tree.add("SampleList", "ipl", soce::proto::SoceDataType::kTypeList);
    type_tree.add("SampleList", "icl", soce::proto::SoceDataType::kTypeList);
    type_tree.add("SampleList", "iml", soce::proto::SoceDataType::kTypeList);
}

SampleListTypeTree SampleList::s_type_tree(g_type_tree);

size_t OutterPrimaySet::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_set(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeInt32,
        [&proto, this](){
            uint32_t elem_len = 0;
            int32_t elem;
            elem_len = proto->read_int32(elem);
            elems_.insert(elem);
            return elem_len;
        });
    return elem_len;
};

size_t OutterPrimaySet::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_set(proto,
        required,
        has_set,
        with_type,
        elems_.size(),
        soce::proto::SoceDataType::kTypeInt32,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            for (auto& elem : elems_){
                elem_len = proto->write_int32(elem);
                if (!elem_len){
                    return 0;
                }
                len += elem_len;
            }
            return len;
        });
    return elem_len;
}

size_t OutterComplexSet::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_set(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeStruct,
        [&proto, this](){
            uint32_t elem_len = 0;
            SampleAsElem elem;
            elem_len = elem.read(proto, true, false);
            elems_.insert(elem);
            return elem_len;
        });
    return elem_len;
};

size_t OutterComplexSet::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_set(proto,
        required,
        has_set,
        with_type,
        elems_.size(),
        soce::proto::SoceDataType::kTypeStruct,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            for (auto& elem : elems_){
                elem_len = elem.write(proto, true, true, false);
                if (!elem_len){
                    return 0;
                }
                len += elem_len;
            }
            return len;
        });
    return elem_len;
}

size_t OutterMultiSet::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_set(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeSet,
        [&proto, this](){
            uint32_t elem_len = 0;
            OutterPrimaySet elem;
            elem_len = elem.read(proto, true, false);
            elems_.insert(elem);
            return elem_len;
        });
    return elem_len;
};

size_t OutterMultiSet::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_set(proto,
        required,
        has_set,
        with_type,
        elems_.size(),
        soce::proto::SoceDataType::kTypeSet,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            for (auto& elem : elems_){
                elem_len = elem.write(proto, true, true, false);
                if (!elem_len){
                    return 0;
                }
                len += elem_len;
            }
            return len;
        });
    return elem_len;
}

size_t SampleSet::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t SampleSet::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    return soce::proto::SoceProto::read_struct(proto, required, with_type,6,
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
                /* read opl */
                if (index >= valid_num){
                    break;
                }
                elem_len = opl.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ocl */
                if (index >= valid_num){
                    break;
                }
                elem_len = ocl.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read oml */
                if (index >= valid_num){
                    break;
                }
                elem_len = oml.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ipl */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_set(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeInt32,
                    [&proto, this](){
                        uint32_t elem_len = 0;
                        int32_t elem;
                        elem_len = proto->read_int32(elem);
                        ipl.insert(elem);
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

                /* read icl */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_set(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeStruct,
                    [&proto, this](){
                        uint32_t elem_len = 0;
                        SampleAsElem elem;
                        elem_len = elem.read(proto, true, false);
                        icl.insert(elem);
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

                /* read iml */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_set(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeSet,
                    [&proto, this](){
                        uint32_t elem_len = 0;
                        OutterPrimaySet elem;
                        elem_len = elem.read(proto, true, false);
                        iml.insert(elem);
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

size_t SampleSet::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t SampleSet::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 6, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = opl.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = ocl.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = oml.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

                elem_len = soce::proto::SoceProto::write_set(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    ipl.size(),
                    soce::proto::SoceDataType::kTypeInt32,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : ipl){
                            elem_len = proto->write_int32(elem);
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

                elem_len = soce::proto::SoceProto::write_set(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    icl.size(),
                    soce::proto::SoceDataType::kTypeStruct,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : icl){
                            elem_len = elem.write(proto, true, true, false);
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

                elem_len = soce::proto::SoceProto::write_set(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    iml.size(),
                    soce::proto::SoceDataType::kTypeSet,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : iml){
                            elem_len = elem.write(proto, true, true, false);
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

soce::proto::TypeTree* SampleSet::get_type_tree(){
    return &g_type_tree;
}

SampleSetTypeTree::SampleSetTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("SampleSet", "opl", soce::proto::SoceDataType::kTypeSet);
    type_tree.add("SampleSet", "ocl", soce::proto::SoceDataType::kTypeSet);
    type_tree.add("SampleSet", "oml", soce::proto::SoceDataType::kTypeSet);
    type_tree.add("SampleSet", "ipl", soce::proto::SoceDataType::kTypeSet);
    type_tree.add("SampleSet", "icl", soce::proto::SoceDataType::kTypeSet);
    type_tree.add("SampleSet", "iml", soce::proto::SoceDataType::kTypeSet);
}

SampleSetTypeTree SampleSet::s_type_tree(g_type_tree);

size_t OutterPrimayMap::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_map(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeInt32,
        soce::proto::SoceDataType::kTypeInt32,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            int32_t key;
            elem_len = proto->read_int32(key);
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
            elems_[key] = val;
            return len;
        });
    return elem_len;
};

size_t OutterPrimayMap::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_map(proto,
        required,
        has_set,
        with_type,
        elems_.size(),
        soce::proto::SoceDataType::kTypeInt32,
        soce::proto::SoceDataType::kTypeInt32,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            for (auto& elem : elems_){
                elem_len = proto->write_int32(elem.first);
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
    return elem_len;
}

size_t OutterComplexMapPC::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_map(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeInt32,
        soce::proto::SoceDataType::kTypeStruct,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            int32_t key;
            elem_len = proto->read_int32(key);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            SampleAsElem val;
            elem_len = val.read(proto, true, false);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            elems_[key] = val;
            return len;
        });
    return elem_len;
};

size_t OutterComplexMapPC::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_map(proto,
        required,
        has_set,
        with_type,
        elems_.size(),
        soce::proto::SoceDataType::kTypeInt32,
        soce::proto::SoceDataType::kTypeStruct,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            for (auto& elem : elems_){
                elem_len = proto->write_int32(elem.first);
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
    return elem_len;
}

size_t OutterComplexMapCP::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_map(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeStruct,
        soce::proto::SoceDataType::kTypeInt32,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            SampleAsElem key;
            elem_len = key.read(proto, true, false);
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
            elems_[key] = val;
            return len;
        });
    return elem_len;
};

size_t OutterComplexMapCP::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_map(proto,
        required,
        has_set,
        with_type,
        elems_.size(),
        soce::proto::SoceDataType::kTypeStruct,
        soce::proto::SoceDataType::kTypeInt32,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            for (auto& elem : elems_){
                elem_len = elem.first.write(proto, true, true, false);
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
    return elem_len;
}

size_t OutterComplexMapCC::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::read_map(proto,
        required,
        with_type,
        soce::proto::SoceDataType::kTypeStruct,
        soce::proto::SoceDataType::kTypeStruct,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            SampleAsElem key;
            elem_len = key.read(proto, true, false);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            SampleAsElem val;
            elem_len = val.read(proto, true, false);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            elems_[key] = val;
            return len;
        });
    return elem_len;
};

size_t OutterComplexMapCC::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    uint32_t elem_len = 0;
    elem_len = soce::proto::SoceProto::write_map(proto,
        required,
        has_set,
        with_type,
        elems_.size(),
        soce::proto::SoceDataType::kTypeStruct,
        soce::proto::SoceDataType::kTypeStruct,
        [&proto, this]()->uint32_t{
            uint32_t len = 0;
            uint32_t elem_len = 0;
            for (auto& elem : elems_){
                elem_len = elem.first.write(proto, true, true, false);
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
    return elem_len;
}

size_t SampleMap::deserialize(soce::proto::ProtoIf* proto){
    return read(proto, true, false);
}

size_t SampleMap::read(soce::proto::ProtoIf* proto, bool required, bool with_type){
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
                /* read opm */
                if (index >= valid_num){
                    break;
                }
                elem_len = opm.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ocmpc */
                if (index >= valid_num){
                    break;
                }
                elem_len = ocmpc.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ocmcp */
                if (index >= valid_num){
                    break;
                }
                elem_len = ocmcp.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ocmcc */
                if (index >= valid_num){
                    break;
                }
                elem_len = ocmcc.read(proto, attrs_[index], true);
                if (!elem_len){
                    return 0;
                }
                else if (elem_len != 1){
                    status_[index] = true;
                }
                len += elem_len;
                ++index;

                /* read ipm */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_map(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeInt32,
                    soce::proto::SoceDataType::kTypeInt32,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        int32_t key;
                        elem_len = proto->read_int32(key);
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
                        ipm[key] = val;
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

                /* read ipc */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_map(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeInt32,
                    soce::proto::SoceDataType::kTypeStruct,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        int32_t key;
                        elem_len = proto->read_int32(key);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        SampleAsElem val;
                        elem_len = val.read(proto, true, false);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        ipc[key] = val;
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

                /* read icp */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_map(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeStruct,
                    soce::proto::SoceDataType::kTypeInt32,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        SampleAsElem key;
                        elem_len = key.read(proto, true, false);
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
                        icp[key] = val;
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

                /* read icc */
                if (index >= valid_num){
                    break;
                }
                elem_len = soce::proto::SoceProto::read_map(proto,
                    attrs_[index],
                    true,
                    soce::proto::SoceDataType::kTypeStruct,
                    soce::proto::SoceDataType::kTypeStruct,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        SampleAsElem key;
                        elem_len = key.read(proto, true, false);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        SampleAsElem val;
                        elem_len = val.read(proto, true, false);
                        if (!elem_len){
                            return 0;
                        }
                        len += elem_len;
                        icc[key] = val;
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

size_t SampleMap::serialize(soce::proto::ProtoIf* proto) const {
    return write(proto, true, true, false);
}
size_t SampleMap::write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type) const {
    return soce::proto::SoceProto::write_struct(proto, required, has_set, with_type, 8, [&, this]()->uint32_t{
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t index = 0;
        uint32_t elem_len_pos = 0;
        uint32_t len_size = 0;
        (void) elem_len_pos;
        (void) len_size;
        do{
            elem_len = opm.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = ocmpc.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = ocmcp.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

            elem_len = ocmcc.write(proto, attrs_[index], status_[index], true);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
            ++index;

                elem_len = soce::proto::SoceProto::write_map(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    ipm.size(),
                    soce::proto::SoceDataType::kTypeInt32,
                    soce::proto::SoceDataType::kTypeInt32,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : ipm){
                            elem_len = proto->write_int32(elem.first);
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

                elem_len = soce::proto::SoceProto::write_map(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    ipc.size(),
                    soce::proto::SoceDataType::kTypeInt32,
                    soce::proto::SoceDataType::kTypeStruct,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : ipc){
                            elem_len = proto->write_int32(elem.first);
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

                elem_len = soce::proto::SoceProto::write_map(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    icp.size(),
                    soce::proto::SoceDataType::kTypeStruct,
                    soce::proto::SoceDataType::kTypeInt32,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : icp){
                            elem_len = elem.first.write(proto, true, true, false);
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

                elem_len = soce::proto::SoceProto::write_map(proto,
                    attrs_[index],
                    status_[index],
                    true,
                    icc.size(),
                    soce::proto::SoceDataType::kTypeStruct,
                    soce::proto::SoceDataType::kTypeStruct,
                    [&proto, this]()->uint32_t{
                        uint32_t len = 0;
                        uint32_t elem_len = 0;
                        for (auto& elem : icc){
                            elem_len = elem.first.write(proto, true, true, false);
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

soce::proto::TypeTree* SampleMap::get_type_tree(){
    return &g_type_tree;
}

SampleMapTypeTree::SampleMapTypeTree(soce::proto::TypeTree& type_tree){
    type_tree.add("SampleMap", "opm", soce::proto::SoceDataType::kTypeMap);
    type_tree.add("SampleMap", "ocmpc", soce::proto::SoceDataType::kTypeMap);
    type_tree.add("SampleMap", "ocmcp", soce::proto::SoceDataType::kTypeMap);
    type_tree.add("SampleMap", "ocmcc", soce::proto::SoceDataType::kTypeMap);
    type_tree.add("SampleMap", "ipm", soce::proto::SoceDataType::kTypeMap);
    type_tree.add("SampleMap", "ipc", soce::proto::SoceDataType::kTypeMap);
    type_tree.add("SampleMap", "icp", soce::proto::SoceDataType::kTypeMap);
    type_tree.add("SampleMap", "icc", soce::proto::SoceDataType::kTypeMap);
}

SampleMapTypeTree SampleMap::s_type_tree(g_type_tree);

}}//end namespace



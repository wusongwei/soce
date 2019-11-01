#include "proto/dynamic-setter.h"
#include "proto/binary-proto.h"

namespace soce {
namespace proto {

    int DynamicSetter::init(char* data, size_t len)
    {
        if (!data || !len){
            return -1;
        }
        data_ = data;
        len_ = len;
        return 0;
    }

    int DynamicSetter::set_bytes(const std::vector<uint32_t>& indexes, const char* data, size_t len)
    {
        BinaryProto bp;
        bp.init(data_, len_, false);

        return set_bytes(&bp, indexes, 0, data, len);
    }

    int DynamicSetter::set_bytes(ProtoIf* proto,
                                 const std::vector<uint32_t>& indexes,
                                 size_t layer,
                                 const char* data,
                                 size_t len)
    {
        if (layer >= indexes.size()) {
            return -1;
        }

        uint32_t struct_len = 0;
        if (proto->read_len(struct_len) == 0){
            return -1;
        }

        uint32_t elems = 0;
        if (!proto->read_len(elems)){
            return -1;
        }

        if (skip_elems(indexes[layer], proto)){
            return -1;
        }

        if (layer + 1 < indexes.size()) {
            SoceDataType type;
            if (!proto->read_type(type) || type != SoceDataType::kTypeStruct){
                return -1;
            }

            return set_bytes(proto, indexes, layer + 1, data, len);
        }
        else {
            SoceDataType type;
            if (!proto->read_type(type) || type != SoceDataType::kTypeBytes){
                return -1;
            }

            uint32_t cur_pos = proto->get_cur_pos();
            uint32_t data_var_len = proto->calc_len_size(len);
            if (cur_pos + data_var_len + len > len_) {
                return -1;
            }

            proto->write_len(len);
            memcpy(data_ + proto->get_cur_pos(), data, len);
        }

        return 0;
    }

    int DynamicSetter::skip_elem(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        uint32_t size = 0;
        uint32_t len = 0;
        int32_t i32 = 0;
        int64_t i64 = 0;

        if (!proto->read_type(type)){
            return -1;
        }

        switch(type){
        case SoceDataType::kTypeNull:
        case SoceDataType::kTypeBool:
            if (proto->skip(1)){
                return -1;
            }
            break;

        case SoceDataType::kTypeInt32:
        case SoceDataType::kTypeEnum:
            size = proto->read_int32(i32);
            if (!size){
                return -1;
            }
            break;

        case SoceDataType::kTypeFloat:
            if (proto->skip(4)){
                return -1;
            }
            break;

        case SoceDataType::kTypeInt64:
            size = proto->read_int64(i64);
            if (!size){
                return -1;
            }
            break;

        case SoceDataType::kTypeDouble:
            if (proto->skip(8)){
                return -1;
            }
            break;

        default:
            size = proto->read_len(len);
            if (size == 0){
                return -1;
            }
            if (proto->skip(len)){
                return -1;
            }
            break;
        }

        return 0;
    }

    int DynamicSetter::skip_elems(int n, soce::proto::ProtoIf* proto)
    {
        for (int i=0; i<n; ++i){
            if (skip_elem(proto)){
                return -1;
            }
        }

        return 0;
    }

} // namespace proto
} // namespace soce

#ifndef _SOCE_FADS_DYNAMIC_SETTER_XQTFp3rvaO_H_
#define _SOCE_FADS_DYNAMIC_SETTER_XQTFp3rvaO_H_

#include <vector>
#include "proto/proto-if.h"

namespace soce {
namespace proto{

    class DynamicSetter
    {
    public:
        int init(char* data, size_t len);

        int set_bytes(const std::vector<uint32_t>& indexes, const char* data, size_t len);

    private:
        int set_bytes(ProtoIf* proto,
                      const std::vector<uint32_t>& indexes,
                      size_t layer,
                      const char* data,
                      size_t len);
        int skip_elem(soce::proto::ProtoIf* proto);
        int skip_elems(int n, soce::proto::ProtoIf* proto);

    private:
        char* data_ = NULL;
        size_t len_ = 0;
    };

} // namespace proto
} // namespace soce
#endif

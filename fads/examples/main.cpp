/*
* Licensed to the Apache Software Foundation (ASF) under one
* or more contributor license agreements. See the NOTICE file
* distributed with this work for additional information
* regarding copyright ownership. The ASF licenses this file
* to you under the Apache License, Version 2.0 (the
* "License"); you may not use this file except in compliance
* with the License. You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied. See the License for the
* specific language governing permissions and limitations
* under the License.
*/

#include <iostream>
#include <stdio.h>
#include <sstream>
#include "gen-cpp/sample.h"
#include "proto/binary-proto.h"
#include "proto/dynamic-node.h"
#include "proto/dynamic-getter.h"
#include "proto/dynamic-parser.h"
#include "log4rel/logger.h"

using namespace std;
using namespace fads::sample;
using namespace soce::proto;

static void sample_primary_type()
{
    char sample_byte[4] = {'a', 'b', 'c', 'd'};
    PrimaryType pt;
    pt.set_a(true);
    pt.set_b(1);
    pt.set_c(2);
    pt.set_d(1.1);
    pt.set_e(2.2);
    pt.set_f("xyz");
    pt.set_g(sample_byte);
    pt.set_color(red);

    BinaryProto bp;
    if (pt.serialize(&bp) == 0){
        SOCE_ERROR << _D("serialize failed");
        return;
    }

    for (size_t i=0; i<bp.size(); ++i){
        printf("%2X ", bp.data()[i]);
    }
    printf("\n");

    PrimaryType pt2;
    BinaryProto bp2;
    bp2.init(bp.data(), bp.size());
    if (pt2.deserialize(&bp2) == 0){
        SOCE_ERROR << _D("deserialize failed");
        return;
    }

    SOCE_INFO << _S("pt2.a", pt2.get_a());
    SOCE_INFO << _S("pt2.b", pt2.get_b());
    SOCE_INFO << _S("pt2.c", pt2.get_c());
    SOCE_INFO << _S("pt2.d", pt2.get_d());
    SOCE_INFO << _S("pt2.e", pt2.get_e());
    SOCE_INFO << _S("pt2.f", pt2.get_f());
    SOCE_INFO << _S("pt2.g", string(pt2.get_g(), 4));
    SOCE_INFO << _S("pt2.color", pt2.get_color());
}

static void sample_complex_type()
{
    char sample_byte[4] = {'a', 'b', 'c', 'd'};
    ComplexType ct;
    ct.mutable_pt().set_a(true);
    ct.mutable_pt().set_b(1);
    ct.mutable_pt().set_c(2);
    ct.mutable_pt().set_d(1.1);
    ct.mutable_pt().set_e(2.2);
    ct.mutable_pt().set_f("xyz");
    ct.mutable_pt().set_g(sample_byte);
    ct.mutable_pt().set_color(red);
    ct.mutable_a() = 1;

    BinaryProto bp;
    if (ct.serialize(&bp) == 0){
        SOCE_ERROR << _D("serialize failed");
        return;
    }

    ComplexType ct2;
    BinaryProto bp2;
    bp2.init(bp.data(), bp.size());
    if (ct2.deserialize(&bp2) == 0){
        SOCE_ERROR << _D("deserialize failed");
        return;
    }

    SOCE_INFO << _S("ct2.pt.a", ct2.get_pt().get_a());
    SOCE_INFO << _S("ct2.pt.b", ct2.get_pt().get_b());
    SOCE_INFO << _S("ct2.pt.c", ct2.get_pt().get_c());
    SOCE_INFO << _S("ct2.pt.d", ct2.get_pt().get_d());
    SOCE_INFO << _S("ct2.pt.e", ct2.get_pt().get_e());
    SOCE_INFO << _S("ct2.pt.f", ct2.get_pt().get_f());
    SOCE_INFO << _S("ct2.pt.g", string(ct2.get_pt().get_g(), 4));
    SOCE_INFO << _S("ct2.pt.color", ct2.get_pt().get_color());
    SOCE_INFO << _S("ct2.a", ct2.get_a());
}

static void sample_list()
{
    SampleList sl;

    for (size_t i=0; i<4; ++i){
        sl.mutable_opl().GetElems().push_back(i);
    }

    for (size_t i=0; i<4; ++i){
        SampleAsElem sae;
        sae.set_a(i);

        sl.mutable_ocl().GetElems().push_back(std::move(sae));
    }

    size_t step = 0;
    for (size_t i=0; i<4; ++i){
        OutterPrimayList tmp;
        for (size_t j=0; j<4; ++j){
            tmp.GetElems().push_back(step + j);
        }
        sl.mutable_oml().GetElems().push_back(std::move(tmp));
        step += 4;
    }

    for (size_t i=0; i<4; ++i){
        sl.mutable_ipl().push_back(i);
    }

    for (size_t i=0; i<4; ++i){
        SampleAsElem sae;
        sae.set_a(i);

        sl.mutable_icl().push_back(std::move(sae));
    }

    step = 0;
    for (size_t i=0; i<4; ++i){
        OutterPrimayList tmp;
        for (size_t j=0; j<4; ++j){
            tmp.GetElems().push_back(step + j);
        }
        sl.mutable_iml().push_back(std::move(tmp));
        step += 4;
    }

    BinaryProto bp;
    if (sl.serialize(&bp) == 0){
        SOCE_ERROR << _D("serialize failed");
        return;
    }

    SampleList sl2;
    BinaryProto bp2;
    bp2.init(bp.data(), bp.size());
    if (sl2.deserialize(&bp2) == 0){
        SOCE_ERROR << _D("deserialize failed");
        return;
    }

    ostringstream oss;
    for (auto& i : sl2.get_opl().GetElems()){
        SOCE_INFO << _S("opl", i);
    }

    for (auto& i : sl2.get_ocl().GetElems()){
        SOCE_INFO << _S("ocl", i.get_a());
    }

    for (auto& i : sl2.get_oml().GetElems()){
        for (auto& j : i.GetElems()){
            SOCE_INFO << _S("oml", j);
        }
    }

    for (auto& i : sl2.get_ipl()){
        SOCE_INFO << _S("ipl", i);
    }

    for (auto& i : sl2.get_icl()){
        SOCE_INFO << _S("icl", i.get_a());
    }

    for (auto& i : sl2.get_iml()){
        for (auto& j : i.GetElems()){
            SOCE_INFO << _S("iml", j);
        }
    }
}

static void sample_set()
{
    SampleSet ss;

    for (size_t i=0; i<4; ++i){
        ss.mutable_opl().GetElems().insert(i);
    }

    for (size_t i=0; i<4; ++i){
        SampleAsElem sae;
        sae.set_a(i);

        ss.mutable_ocl().GetElems().insert(std::move(sae));
    }

    size_t step = 0;
    for (size_t i=0; i<4; ++i){
        OutterPrimaySet tmp;
        for (size_t j=0; j<4; ++j){
            tmp.GetElems().insert(step + j);
        }
        ss.mutable_oml().GetElems().insert(std::move(tmp));
        step += 4;
    }

    for (size_t i=0; i<4; ++i){
        ss.mutable_ipl().insert(i);
    }

    for (size_t i=0; i<4; ++i){
        SampleAsElem sae;
        sae.set_a(i);

        ss.mutable_icl().insert(std::move(sae));
    }

    step = 0;
    for (size_t i=0; i<4; ++i){
        OutterPrimaySet tmp;
        for (size_t j=0; j<4; ++j){
            tmp.GetElems().insert(step + j);
        }
        ss.mutable_iml().insert(std::move(tmp));
        step += 4;
    }

    BinaryProto bp;
    if (ss.serialize(&bp) == 0){
        SOCE_ERROR << _D("serialize failed");
        return;
    }

    SampleSet ss2;
    BinaryProto bp2;
    bp2.init(bp.data(), bp.size());
    if (ss2.deserialize(&bp2) == 0){
        SOCE_ERROR << _D("deserialize failed");
        return;
    }

    ostringstream oss;
    for (auto& i : ss2.get_opl().GetElems()){
        SOCE_INFO << _S("opl", i);
    }

    for (auto& i : ss2.get_ocl().GetElems()){
        SOCE_INFO << _S("ocl", i.get_a());
    }

    for (auto& i : ss2.get_oml().GetElems()){
        for (auto& j : i.GetElems()){
            SOCE_INFO << _S("oml", j);
        }
    }

    for (auto& i : ss2.get_ipl()){
        SOCE_INFO << _S("ipl", i);
    }

    for (auto& i : ss2.get_icl()){
        SOCE_INFO << _S("icl", i.get_a());
    }

    for (auto& i : ss2.get_iml()){
        for (auto& j : i.GetElems()){
            SOCE_INFO << _S("iml", j);
        }
    }
}

static void sample_map()
{
    SampleAsElem sae1;
    sae1.set_a(100);

    SampleAsElem sae2;
    sae2.set_a(200);

    SampleMap sm;
    sm.mutable_opm().GetElems()[1] = 1;
    sm.mutable_ocmpc().GetElems()[2] = sae1;
    sm.mutable_ocmcp().GetElems()[sae1] = 3;
    sm.mutable_ocmcc().GetElems()[sae1] = sae2;
    sm.mutable_ipm()[1] = 1;
    sm.mutable_ipc()[2] = sae1;
    sm.mutable_icp()[sae1] = 3;
    sm.mutable_icc()[sae1] = sae2;

    BinaryProto bp;
    if (sm.serialize(&bp) == 0){
        SOCE_ERROR << _D("serialize failed");
        return;
    }

    SampleMap sm2;
    BinaryProto bp2;
    bp2.init(bp.data(), bp.size());
    if (sm2.deserialize(&bp2) == 0){
        SOCE_ERROR << _D("deserialize failed");
        return;
    }

    SOCE_INFO << _S("sm2.opm.key", sm2.mutable_opm().GetElems().begin()->first)
                  << _S("sm2.opm.val", sm2.mutable_opm().GetElems().begin()->second) ;
    SOCE_INFO << _S("sm2.ocmpc.key", sm2.mutable_ocmpc().GetElems().begin()->first)
                  << _S("sm2.ocmpc.val", sm2.mutable_ocmpc().GetElems().begin()->second.get_a());
    SOCE_INFO << _S("sm2.ocmcp.key", sm2.mutable_ocmcp().GetElems().begin()->first.get_a())
                  << _S("sm2.ocmcp.val", sm2.mutable_ocmcp().GetElems().begin()->second);
    SOCE_INFO << _S("sm2.ocmcc.key", sm2.mutable_ocmcc().GetElems().begin()->first.get_a())
                  << _S("sm2.ocmcc.val", sm2.mutable_ocmcc().GetElems().begin()->second.get_a());
    SOCE_INFO << _S("sm2.ipm.key", sm2.mutable_ipm().begin()->first)
                  << _S("sm2.ipm.val", sm2.mutable_ipm().begin()->second);
    SOCE_INFO << _S("sm2.ipc.key", sm2.mutable_ipc().begin()->first)
                  << _S("sm2.ipc.val", sm2.mutable_ipc().begin()->second.get_a());
    SOCE_INFO << _S("sm2.icp.key", sm2.mutable_icp().begin()->first.get_a())
                  << _S("sm2.icp.val", sm2.mutable_icp().begin()->second);
    SOCE_INFO << _S("sm2.icc.key", sm2.mutable_icc().begin()->first.get_a())
                  << _S("sm2.icc.val", sm2.mutable_icc().begin()->second.get_a());
}

static void sample_dynamic_getter()
{
    char sample_byte[4] = {'a', 'b', 'c', 'd'};
    ComplexType ct;
    ct.mutable_pt().set_a(true);
    ct.mutable_pt().set_b(1);
    ct.mutable_pt().set_c(2);
    ct.mutable_pt().set_d(1.1);
    ct.mutable_pt().set_e(2.2);
    ct.mutable_pt().set_f("xyz");
    ct.mutable_pt().set_g(sample_byte);
    ct.mutable_pt().set_color(red);
    ct.mutable_a() = 1;

    BinaryProto bp;
    if (ct.serialize(&bp) == 0){
        SOCE_ERROR << _D("serialize failed");
        return;
    }

    int32_t b = 0;
    string f;

    DynamicGetter dg_fast;
    dg_fast.init(bp.data(), bp.size());
    std::vector<uint32_t> indexes;
    indexes.push_back(0);
    indexes.push_back(5);
    dg_fast.get_string(indexes, f);
    SOCE_INFO << _S("mode", "fast") << _S("f", f);

    b = 0;
    DynamicGetter dg;
    dg.init(bp.data(), bp.size());
    dg.get_int32("0.1", b, true);
    dg.get_string("0.5", f, true);
    dg.do_get();
    SOCE_INFO << _S("mode", "multi") << _S("b", b) << _S("f", f);
}

static void sample_dynamic_node()
{
    char sample_byte[4] = {'a', 'b', 'c', 'd'};
    ComplexType ct;
    ct.mutable_pt().set_a(true);
    ct.mutable_pt().set_b(1);
    ct.mutable_pt().set_c(2);
    ct.mutable_pt().set_d(1.1);
    ct.mutable_pt().set_e(2.2);
    ct.mutable_pt().set_f("xyz");
    ct.mutable_pt().set_g(sample_byte);
    ct.mutable_pt().set_color(red);
    ct.mutable_a() = 1;

    BinaryProto bp;
    if (ct.serialize(&bp) == 0){
        SOCE_ERROR << _D("serialize failed");
        return;
    }

    std::shared_ptr<DynamicNodeStruct> node;
    DynamicParser dp;

#if 0
    node = dp.parse(bp.data(), bp.size());
    if (!node) {
        SOCE_ERROR << _S("error", "without schema");
        return;
    }
#else
    node = std::make_shared<DynamicNodeStruct>("ComplexType");
    auto node_pt = std::make_shared<DynamicNodeStruct>("pt");
    auto node_bool = DynamicNode::create(SoceDataType::kTypeBool);
    node_bool->set_name("a");
    node_pt->add(node_bool);
    auto node_i32 = DynamicNode::create(SoceDataType::kTypeInt32);
    node_i32->set_name("a");
    node->add(node_pt);
    node->add(node_i32);
    if (dp.parse(bp.data(), bp.size(), node)) {
        SOCE_ERROR << _S("error", "with schema");
        return;
    }
#endif
    SOCE_INFO << _S("json", node->to_json());
}

int main()
{
    SOCE_CUR_LOGGER->reserve_field(soce::log4rel::kLogFieldLevel, true);
    SOCE_CUR_LOGGER->reserve_field(soce::log4rel::kLogFieldPos, true);

    sample_primary_type();
    sample_complex_type();
    sample_list();
    sample_set();
    sample_map();

    sample_dynamic_getter();
    sample_dynamic_node();

    return 0;
}

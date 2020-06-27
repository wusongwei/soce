#ifndef _sample_1317140219_H_
#define _sample_1317140219_H_

#include <string>
#include <bitset>
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "proto/fads-message.h"
#include "proto/fads-message.h"
#include "proto/soce-proto.h"
#include "proto/type-tree.h"

using std::string;
using std::bitset;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::vector;

namespace fads{ namespace sample{ 

typedef enum{
    red = 1,
    yellow,
    blue,
}Color;

class PrimaryTypeTypeTree{
public:
    PrimaryTypeTypeTree(soce::proto::TypeTree& type_tree);
};

class PrimaryType : public soce::fads::FadsMessage{
public:
    void set_a(const bool& _a){
        a = _a;
        status_[0] = true;
    }
    void set_a(bool&& _a){
        a = std::move(_a);
        status_[0] = true;
    }
    const bool& get_a() const {
        return a;
    }
    bool& mutable_a(){
        status_[0] = true;
        return a;
    }
    bool has_a() const{
        return status_[0];
    }

    void set_b(const int32_t& _b){
        b = _b;
        status_[1] = true;
    }
    void set_b(int32_t&& _b){
        b = std::move(_b);
        status_[1] = true;
    }
    const int32_t& get_b() const {
        return b;
    }
    int32_t& mutable_b(){
        status_[1] = true;
        return b;
    }
    bool has_b() const{
        return status_[1];
    }

    void set_c(const int64_t& _c){
        c = _c;
        status_[2] = true;
    }
    void set_c(int64_t&& _c){
        c = std::move(_c);
        status_[2] = true;
    }
    const int64_t& get_c() const {
        return c;
    }
    int64_t& mutable_c(){
        status_[2] = true;
        return c;
    }
    bool has_c() const{
        return status_[2];
    }

    void set_d(const float& _d){
        d = _d;
        status_[3] = true;
    }
    void set_d(float&& _d){
        d = std::move(_d);
        status_[3] = true;
    }
    const float& get_d() const {
        return d;
    }
    float& mutable_d(){
        status_[3] = true;
        return d;
    }
    bool has_d() const{
        return status_[3];
    }

    void set_e(const double& _e){
        e = _e;
        status_[4] = true;
    }
    void set_e(double&& _e){
        e = std::move(_e);
        status_[4] = true;
    }
    const double& get_e() const {
        return e;
    }
    double& mutable_e(){
        status_[4] = true;
        return e;
    }
    bool has_e() const{
        return status_[4];
    }

    void set_f(const string& _f){
        f = _f;
        status_[5] = true;
    }
    void set_f(string&& _f){
        f = std::move(_f);
        status_[5] = true;
    }
    const string& get_f() const {
        return f;
    }
    string& mutable_f(){
        status_[5] = true;
        return f;
    }
    bool has_f() const{
        return status_[5];
    }

    void set_g(const char _g[4]){
        memcpy(g, _g, 4);
        status_[6] = true;
    }
    const char* get_g() const {
        return g;
    }
    char* mutable_g(){
        status_[6] = true;
        return g;
    }
    bool IsSetg(){
        return status_[6];
    }

    void set_color(const Color& _color){
        color = _color;
        status_[7] = true;
    }
    void set_color(Color&& _color){
        color = std::move(_color);
        status_[7] = true;
    }
    const Color& get_color() const {
        return color;
    }
    Color& mutable_color(){
        status_[7] = true;
        return color;
    }
    bool has_color() const{
        return status_[7];
    }

public:
    /* operators */
    PrimaryType() = default;
    PrimaryType(const PrimaryType&) = default;
    PrimaryType& operator=(const PrimaryType& rhs) = default;
    bool operator==(const PrimaryType& rhs) const{
        if (a != rhs.a)
            return false;
        if (b != rhs.b)
            return false;
        if (c != rhs.c)
            return false;
        if (d != rhs.d)
            return false;
        if (e != rhs.e)
            return false;
        if (f != rhs.f)
            return false;
        if (g != rhs.g)
            return false;
        if (color != rhs.color)
            return false;
        return true;
    }

    bool operator!=(const PrimaryType& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const PrimaryType& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<bool>()(a)
            ^ std::hash<int32_t>()(b)
            ^ std::hash<int64_t>()(c)
            ^ std::hash<float>()(d)
            ^ std::hash<double>()(e)
            ^ std::hash<string>()(f)
            ^ std::hash<string>()(string(g,4))
            ^ std::hash<int32_t>()(color);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    bool a;
    int32_t b;
    int64_t c;
    float d;
    double e;
    string f;
    char g[4];
    Color color;

    /* for optional */
    bitset<8> attrs_ = 0;
    bitset<8> status_;

public:
    static PrimaryTypeTypeTree s_type_tree;
};

struct hash_PrimaryType
{
    size_t operator()(const PrimaryType& rhs) const{
        return rhs(rhs);
    }
};

class ComplexTypeTypeTree{
public:
    ComplexTypeTypeTree(soce::proto::TypeTree& type_tree);
};

class ComplexType : public soce::fads::FadsMessage{
public:
    void set_pt(const PrimaryType& _pt){
        pt = _pt;
        status_[0] = true;
    }
    void set_pt(PrimaryType&& _pt){
        pt = std::move(_pt);
        status_[0] = true;
    }
    const PrimaryType& get_pt() const {
        return pt;
    }
    PrimaryType& mutable_pt(){
        status_[0] = true;
        return pt;
    }
    bool has_pt() const{
        return status_[0];
    }

    void set_a(const int32_t& _a){
        a = _a;
        status_[1] = true;
    }
    void set_a(int32_t&& _a){
        a = std::move(_a);
        status_[1] = true;
    }
    const int32_t& get_a() const {
        return a;
    }
    int32_t& mutable_a(){
        status_[1] = true;
        return a;
    }
    bool has_a() const{
        return status_[1];
    }

public:
    /* operators */
    ComplexType() = default;
    ComplexType(const ComplexType&) = default;
    ComplexType& operator=(const ComplexType& rhs) = default;
    bool operator==(const ComplexType& rhs) const{
        if (pt != rhs.pt)
            return false;
        if (a != rhs.a)
            return false;
        return true;
    }

    bool operator!=(const ComplexType& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const ComplexType& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ pt(pt)
            ^ std::hash<int32_t>()(a);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    PrimaryType pt;
    int32_t a;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static ComplexTypeTypeTree s_type_tree;
};

struct hash_ComplexType
{
    size_t operator()(const ComplexType& rhs) const{
        return rhs(rhs);
    }
};

class SampleAsElemTypeTree{
public:
    SampleAsElemTypeTree(soce::proto::TypeTree& type_tree);
};

class SampleAsElem : public soce::fads::FadsMessage{
public:
    void set_a(const int32_t& _a){
        a = _a;
        status_[0] = true;
    }
    void set_a(int32_t&& _a){
        a = std::move(_a);
        status_[0] = true;
    }
    const int32_t& get_a() const {
        return a;
    }
    int32_t& mutable_a(){
        status_[0] = true;
        return a;
    }
    bool has_a() const{
        return status_[0];
    }

public:
    /* operators */
    SampleAsElem() = default;
    SampleAsElem(const SampleAsElem&) = default;
    SampleAsElem& operator=(const SampleAsElem& rhs) = default;
    bool operator==(const SampleAsElem& rhs) const{
        if (a != rhs.a)
            return false;
        return true;
    }

    bool operator!=(const SampleAsElem& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const SampleAsElem& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<int32_t>()(a);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    int32_t a;

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static SampleAsElemTypeTree s_type_tree;
};

struct hash_SampleAsElem
{
    size_t operator()(const SampleAsElem& rhs) const{
        return rhs(rhs);
    }
};

class OutterPrimayList{
public:
    inline const vector<int32_t>& GetElems() const {return elems_;}
    inline vector<int32_t>& GetElems() {return elems_;}
    inline void SetElems(const vector<int32_t>& elems) {elems_ = elems;}
    inline void SetElems(vector<int32_t>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterPrimayList() = default;
    OutterPrimayList(const OutterPrimayList&) = default;
    OutterPrimayList& operator=(const OutterPrimayList& rhs) = default;
    bool operator==(const OutterPrimayList& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterPrimayList& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterPrimayList& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= std::hash<int32_t>()(i);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    vector<int32_t> elems_;
};

struct hash_OutterPrimayList
{
    size_t operator()(const OutterPrimayList& rhs) const{
        return rhs(rhs);
    }
};

class OutterComplexList{
public:
    inline const vector<SampleAsElem>& GetElems() const {return elems_;}
    inline vector<SampleAsElem>& GetElems() {return elems_;}
    inline void SetElems(const vector<SampleAsElem>& elems) {elems_ = elems;}
    inline void SetElems(vector<SampleAsElem>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterComplexList() = default;
    OutterComplexList(const OutterComplexList&) = default;
    OutterComplexList& operator=(const OutterComplexList& rhs) = default;
    bool operator==(const OutterComplexList& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterComplexList& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterComplexList& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= i(i);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    vector<SampleAsElem> elems_;
};

struct hash_OutterComplexList
{
    size_t operator()(const OutterComplexList& rhs) const{
        return rhs(rhs);
    }
};

class OutterMultiList{
public:
    inline const vector<OutterPrimayList>& GetElems() const {return elems_;}
    inline vector<OutterPrimayList>& GetElems() {return elems_;}
    inline void SetElems(const vector<OutterPrimayList>& elems) {elems_ = elems;}
    inline void SetElems(vector<OutterPrimayList>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterMultiList() = default;
    OutterMultiList(const OutterMultiList&) = default;
    OutterMultiList& operator=(const OutterMultiList& rhs) = default;
    bool operator==(const OutterMultiList& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterMultiList& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterMultiList& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= i(i);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    vector<OutterPrimayList> elems_;
};

struct hash_OutterMultiList
{
    size_t operator()(const OutterMultiList& rhs) const{
        return rhs(rhs);
    }
};

class SampleListTypeTree{
public:
    SampleListTypeTree(soce::proto::TypeTree& type_tree);
};

class SampleList : public soce::fads::FadsMessage{
public:
    void set_opl(const OutterPrimayList& _opl){
        opl = _opl;
        status_[0] = true;
    }
    void set_opl(OutterPrimayList&& _opl){
        opl = std::move(_opl);
        status_[0] = true;
    }
    const OutterPrimayList& get_opl() const {
        return opl;
    }
    OutterPrimayList& mutable_opl(){
        status_[0] = true;
        return opl;
    }
    bool has_opl() const{
        return status_[0];
    }

    void set_ocl(const OutterComplexList& _ocl){
        ocl = _ocl;
        status_[1] = true;
    }
    void set_ocl(OutterComplexList&& _ocl){
        ocl = std::move(_ocl);
        status_[1] = true;
    }
    const OutterComplexList& get_ocl() const {
        return ocl;
    }
    OutterComplexList& mutable_ocl(){
        status_[1] = true;
        return ocl;
    }
    bool has_ocl() const{
        return status_[1];
    }

    void set_oml(const OutterMultiList& _oml){
        oml = _oml;
        status_[2] = true;
    }
    void set_oml(OutterMultiList&& _oml){
        oml = std::move(_oml);
        status_[2] = true;
    }
    const OutterMultiList& get_oml() const {
        return oml;
    }
    OutterMultiList& mutable_oml(){
        status_[2] = true;
        return oml;
    }
    bool has_oml() const{
        return status_[2];
    }

    void set_ipl(const vector<int32_t>& _ipl){
        ipl = _ipl;
        status_[3] = true;
    }
    void set_ipl(vector<int32_t>&& _ipl){
        ipl = std::move(_ipl);
        status_[3] = true;
    }
    const vector<int32_t>& get_ipl() const {
        return ipl;
    }
    vector<int32_t>& mutable_ipl(){
        status_[3] = true;
        return ipl;
    }
    bool has_ipl() const{
        return status_[3];
    }

    void set_icl(const vector<SampleAsElem>& _icl){
        icl = _icl;
        status_[4] = true;
    }
    void set_icl(vector<SampleAsElem>&& _icl){
        icl = std::move(_icl);
        status_[4] = true;
    }
    const vector<SampleAsElem>& get_icl() const {
        return icl;
    }
    vector<SampleAsElem>& mutable_icl(){
        status_[4] = true;
        return icl;
    }
    bool has_icl() const{
        return status_[4];
    }

    void set_iml(const vector<OutterPrimayList>& _iml){
        iml = _iml;
        status_[5] = true;
    }
    void set_iml(vector<OutterPrimayList>&& _iml){
        iml = std::move(_iml);
        status_[5] = true;
    }
    const vector<OutterPrimayList>& get_iml() const {
        return iml;
    }
    vector<OutterPrimayList>& mutable_iml(){
        status_[5] = true;
        return iml;
    }
    bool has_iml() const{
        return status_[5];
    }

public:
    /* operators */
    SampleList() = default;
    SampleList(const SampleList&) = default;
    SampleList& operator=(const SampleList& rhs) = default;
    bool operator==(const SampleList& rhs) const{
        if (opl != rhs.opl)
            return false;
        if (ocl != rhs.ocl)
            return false;
        if (oml != rhs.oml)
            return false;
        if (ipl != rhs.ipl)
            return false;
        if (icl != rhs.icl)
            return false;
        if (iml != rhs.iml)
            return false;
        return true;
    }

    bool operator!=(const SampleList& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const SampleList& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ opl(opl)
            ^ ocl(ocl)
            ^ oml(oml)
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : ipl){
                    code ^= std::hash<int32_t>()(_i);
                }
                return code;
            }()
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : icl){
                    code ^= _i(_i);
                }
                return code;
            }()
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : iml){
                    code ^= _i(_i);
                }
                return code;
            }();
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    OutterPrimayList opl;
    OutterComplexList ocl;
    OutterMultiList oml;
    vector<int32_t> ipl;
    vector<SampleAsElem> icl;
    vector<OutterPrimayList> iml;

    /* for optional */
    bitset<6> attrs_ = 0;
    bitset<6> status_;

public:
    static SampleListTypeTree s_type_tree;
};

struct hash_SampleList
{
    size_t operator()(const SampleList& rhs) const{
        return rhs(rhs);
    }
};

class OutterPrimaySet{
public:
    inline const unordered_set<int32_t>& GetElems() const {return elems_;}
    inline unordered_set<int32_t>& GetElems() {return elems_;}
    inline void SetElems(const unordered_set<int32_t>& elems) {elems_ = elems;}
    inline void SetElems(unordered_set<int32_t>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterPrimaySet() = default;
    OutterPrimaySet(const OutterPrimaySet&) = default;
    OutterPrimaySet& operator=(const OutterPrimaySet& rhs) = default;
    bool operator==(const OutterPrimaySet& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterPrimaySet& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterPrimaySet& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= std::hash<int32_t>()(i);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    unordered_set<int32_t> elems_;
};

struct hash_OutterPrimaySet
{
    size_t operator()(const OutterPrimaySet& rhs) const{
        return rhs(rhs);
    }
};

class OutterComplexSet{
public:
    inline const unordered_set<SampleAsElem, hash_SampleAsElem>& GetElems() const {return elems_;}
    inline unordered_set<SampleAsElem, hash_SampleAsElem>& GetElems() {return elems_;}
    inline void SetElems(const unordered_set<SampleAsElem, hash_SampleAsElem>& elems) {elems_ = elems;}
    inline void SetElems(unordered_set<SampleAsElem, hash_SampleAsElem>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterComplexSet() = default;
    OutterComplexSet(const OutterComplexSet&) = default;
    OutterComplexSet& operator=(const OutterComplexSet& rhs) = default;
    bool operator==(const OutterComplexSet& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterComplexSet& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterComplexSet& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= i(i);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    unordered_set<SampleAsElem, hash_SampleAsElem> elems_;
};

struct hash_OutterComplexSet
{
    size_t operator()(const OutterComplexSet& rhs) const{
        return rhs(rhs);
    }
};

class OutterMultiSet{
public:
    inline const unordered_set<OutterPrimaySet, hash_OutterPrimaySet>& GetElems() const {return elems_;}
    inline unordered_set<OutterPrimaySet, hash_OutterPrimaySet>& GetElems() {return elems_;}
    inline void SetElems(const unordered_set<OutterPrimaySet, hash_OutterPrimaySet>& elems) {elems_ = elems;}
    inline void SetElems(unordered_set<OutterPrimaySet, hash_OutterPrimaySet>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterMultiSet() = default;
    OutterMultiSet(const OutterMultiSet&) = default;
    OutterMultiSet& operator=(const OutterMultiSet& rhs) = default;
    bool operator==(const OutterMultiSet& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterMultiSet& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterMultiSet& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= i(i);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    unordered_set<OutterPrimaySet, hash_OutterPrimaySet> elems_;
};

struct hash_OutterMultiSet
{
    size_t operator()(const OutterMultiSet& rhs) const{
        return rhs(rhs);
    }
};

class SampleSetTypeTree{
public:
    SampleSetTypeTree(soce::proto::TypeTree& type_tree);
};

class SampleSet : public soce::fads::FadsMessage{
public:
    void set_opl(const OutterPrimaySet& _opl){
        opl = _opl;
        status_[0] = true;
    }
    void set_opl(OutterPrimaySet&& _opl){
        opl = std::move(_opl);
        status_[0] = true;
    }
    const OutterPrimaySet& get_opl() const {
        return opl;
    }
    OutterPrimaySet& mutable_opl(){
        status_[0] = true;
        return opl;
    }
    bool has_opl() const{
        return status_[0];
    }

    void set_ocl(const OutterComplexSet& _ocl){
        ocl = _ocl;
        status_[1] = true;
    }
    void set_ocl(OutterComplexSet&& _ocl){
        ocl = std::move(_ocl);
        status_[1] = true;
    }
    const OutterComplexSet& get_ocl() const {
        return ocl;
    }
    OutterComplexSet& mutable_ocl(){
        status_[1] = true;
        return ocl;
    }
    bool has_ocl() const{
        return status_[1];
    }

    void set_oml(const OutterMultiSet& _oml){
        oml = _oml;
        status_[2] = true;
    }
    void set_oml(OutterMultiSet&& _oml){
        oml = std::move(_oml);
        status_[2] = true;
    }
    const OutterMultiSet& get_oml() const {
        return oml;
    }
    OutterMultiSet& mutable_oml(){
        status_[2] = true;
        return oml;
    }
    bool has_oml() const{
        return status_[2];
    }

    void set_ipl(const unordered_set<int32_t>& _ipl){
        ipl = _ipl;
        status_[3] = true;
    }
    void set_ipl(unordered_set<int32_t>&& _ipl){
        ipl = std::move(_ipl);
        status_[3] = true;
    }
    const unordered_set<int32_t>& get_ipl() const {
        return ipl;
    }
    unordered_set<int32_t>& mutable_ipl(){
        status_[3] = true;
        return ipl;
    }
    bool has_ipl() const{
        return status_[3];
    }

    void set_icl(const unordered_set<SampleAsElem, hash_SampleAsElem>& _icl){
        icl = _icl;
        status_[4] = true;
    }
    void set_icl(unordered_set<SampleAsElem, hash_SampleAsElem>&& _icl){
        icl = std::move(_icl);
        status_[4] = true;
    }
    const unordered_set<SampleAsElem, hash_SampleAsElem>& get_icl() const {
        return icl;
    }
    unordered_set<SampleAsElem, hash_SampleAsElem>& mutable_icl(){
        status_[4] = true;
        return icl;
    }
    bool has_icl() const{
        return status_[4];
    }

    void set_iml(const unordered_set<OutterPrimaySet, hash_OutterPrimaySet>& _iml){
        iml = _iml;
        status_[5] = true;
    }
    void set_iml(unordered_set<OutterPrimaySet, hash_OutterPrimaySet>&& _iml){
        iml = std::move(_iml);
        status_[5] = true;
    }
    const unordered_set<OutterPrimaySet, hash_OutterPrimaySet>& get_iml() const {
        return iml;
    }
    unordered_set<OutterPrimaySet, hash_OutterPrimaySet>& mutable_iml(){
        status_[5] = true;
        return iml;
    }
    bool has_iml() const{
        return status_[5];
    }

public:
    /* operators */
    SampleSet() = default;
    SampleSet(const SampleSet&) = default;
    SampleSet& operator=(const SampleSet& rhs) = default;
    bool operator==(const SampleSet& rhs) const{
        if (opl != rhs.opl)
            return false;
        if (ocl != rhs.ocl)
            return false;
        if (oml != rhs.oml)
            return false;
        if (ipl != rhs.ipl)
            return false;
        if (icl != rhs.icl)
            return false;
        if (iml != rhs.iml)
            return false;
        return true;
    }

    bool operator!=(const SampleSet& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const SampleSet& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ opl(opl)
            ^ ocl(ocl)
            ^ oml(oml)
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : ipl){
                    code ^= std::hash<int32_t>()(_i);
                }
                return code;
            }()
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : icl){
                    code ^= _i(_i);
                }
                return code;
            }()
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : iml){
                    code ^= _i(_i);
                }
                return code;
            }();
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    OutterPrimaySet opl;
    OutterComplexSet ocl;
    OutterMultiSet oml;
    unordered_set<int32_t> ipl;
    unordered_set<SampleAsElem, hash_SampleAsElem> icl;
    unordered_set<OutterPrimaySet, hash_OutterPrimaySet> iml;

    /* for optional */
    bitset<6> attrs_ = 0;
    bitset<6> status_;

public:
    static SampleSetTypeTree s_type_tree;
};

struct hash_SampleSet
{
    size_t operator()(const SampleSet& rhs) const{
        return rhs(rhs);
    }
};

class OutterPrimayMap{
public:
    inline const unordered_map<int32_t, int32_t>& GetElems() const {return elems_;}
    inline unordered_map<int32_t, int32_t>& GetElems() {return elems_;}
    inline void SetElems(const unordered_map<int32_t, int32_t>& elems) {elems_ = elems;}
    inline void SetElems(unordered_map<int32_t, int32_t>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterPrimayMap() = default;
    OutterPrimayMap(const OutterPrimayMap&) = default;
    OutterPrimayMap& operator=(const OutterPrimayMap& rhs) = default;
    bool operator==(const OutterPrimayMap& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterPrimayMap& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterPrimayMap& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= std::hash<int32_t>()(i.first) ^ std::hash<int32_t>()(i.second);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    unordered_map<int32_t, int32_t> elems_;
};

struct hash_OutterPrimayMap
{
    size_t operator()(const OutterPrimayMap& rhs) const{
        return rhs(rhs);
    }
};

class OutterComplexMapPC{
public:
    inline const unordered_map<int32_t, SampleAsElem>& GetElems() const {return elems_;}
    inline unordered_map<int32_t, SampleAsElem>& GetElems() {return elems_;}
    inline void SetElems(const unordered_map<int32_t, SampleAsElem>& elems) {elems_ = elems;}
    inline void SetElems(unordered_map<int32_t, SampleAsElem>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterComplexMapPC() = default;
    OutterComplexMapPC(const OutterComplexMapPC&) = default;
    OutterComplexMapPC& operator=(const OutterComplexMapPC& rhs) = default;
    bool operator==(const OutterComplexMapPC& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterComplexMapPC& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterComplexMapPC& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= std::hash<int32_t>()(i.first) ^ i.second(i.second);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    unordered_map<int32_t, SampleAsElem> elems_;
};

struct hash_OutterComplexMapPC
{
    size_t operator()(const OutterComplexMapPC& rhs) const{
        return rhs(rhs);
    }
};

class OutterComplexMapCP{
public:
    inline const unordered_map<SampleAsElem, int32_t, hash_SampleAsElem>& GetElems() const {return elems_;}
    inline unordered_map<SampleAsElem, int32_t, hash_SampleAsElem>& GetElems() {return elems_;}
    inline void SetElems(const unordered_map<SampleAsElem, int32_t, hash_SampleAsElem>& elems) {elems_ = elems;}
    inline void SetElems(unordered_map<SampleAsElem, int32_t, hash_SampleAsElem>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterComplexMapCP() = default;
    OutterComplexMapCP(const OutterComplexMapCP&) = default;
    OutterComplexMapCP& operator=(const OutterComplexMapCP& rhs) = default;
    bool operator==(const OutterComplexMapCP& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterComplexMapCP& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterComplexMapCP& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= i.first(i.first) ^ std::hash<int32_t>()(i.second);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    unordered_map<SampleAsElem, int32_t, hash_SampleAsElem> elems_;
};

struct hash_OutterComplexMapCP
{
    size_t operator()(const OutterComplexMapCP& rhs) const{
        return rhs(rhs);
    }
};

class OutterComplexMapCC{
public:
    inline const unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem>& GetElems() const {return elems_;}
    inline unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem>& GetElems() {return elems_;}
    inline void SetElems(const unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem>& elems) {elems_ = elems;}
    inline void SetElems(unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem>&& elems) {elems_ = std::move(elems);}

public:
    /* operators */
    OutterComplexMapCC() = default;
    OutterComplexMapCC(const OutterComplexMapCC&) = default;
    OutterComplexMapCC& operator=(const OutterComplexMapCC& rhs) = default;
    bool operator==(const OutterComplexMapCC& rhs) const{
        if (elems_ != rhs.elems_)
            return false;
        return true;
    }

    bool operator!=(const OutterComplexMapCC& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const OutterComplexMapCC& rhs) const{
        size_t hash_code = 0;
        for (auto& i : rhs.elems_){
            hash_code ^= i.first(i.first) ^ i.second(i.second);
        }
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;

private:
    /* class members */
    unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem> elems_;
};

struct hash_OutterComplexMapCC
{
    size_t operator()(const OutterComplexMapCC& rhs) const{
        return rhs(rhs);
    }
};

class SampleMapTypeTree{
public:
    SampleMapTypeTree(soce::proto::TypeTree& type_tree);
};

class SampleMap : public soce::fads::FadsMessage{
public:
    void set_opm(const OutterPrimayMap& _opm){
        opm = _opm;
        status_[0] = true;
    }
    void set_opm(OutterPrimayMap&& _opm){
        opm = std::move(_opm);
        status_[0] = true;
    }
    const OutterPrimayMap& get_opm() const {
        return opm;
    }
    OutterPrimayMap& mutable_opm(){
        status_[0] = true;
        return opm;
    }
    bool has_opm() const{
        return status_[0];
    }

    void set_ocmpc(const OutterComplexMapPC& _ocmpc){
        ocmpc = _ocmpc;
        status_[1] = true;
    }
    void set_ocmpc(OutterComplexMapPC&& _ocmpc){
        ocmpc = std::move(_ocmpc);
        status_[1] = true;
    }
    const OutterComplexMapPC& get_ocmpc() const {
        return ocmpc;
    }
    OutterComplexMapPC& mutable_ocmpc(){
        status_[1] = true;
        return ocmpc;
    }
    bool has_ocmpc() const{
        return status_[1];
    }

    void set_ocmcp(const OutterComplexMapCP& _ocmcp){
        ocmcp = _ocmcp;
        status_[2] = true;
    }
    void set_ocmcp(OutterComplexMapCP&& _ocmcp){
        ocmcp = std::move(_ocmcp);
        status_[2] = true;
    }
    const OutterComplexMapCP& get_ocmcp() const {
        return ocmcp;
    }
    OutterComplexMapCP& mutable_ocmcp(){
        status_[2] = true;
        return ocmcp;
    }
    bool has_ocmcp() const{
        return status_[2];
    }

    void set_ocmcc(const OutterComplexMapCC& _ocmcc){
        ocmcc = _ocmcc;
        status_[3] = true;
    }
    void set_ocmcc(OutterComplexMapCC&& _ocmcc){
        ocmcc = std::move(_ocmcc);
        status_[3] = true;
    }
    const OutterComplexMapCC& get_ocmcc() const {
        return ocmcc;
    }
    OutterComplexMapCC& mutable_ocmcc(){
        status_[3] = true;
        return ocmcc;
    }
    bool has_ocmcc() const{
        return status_[3];
    }

    void set_ipm(const unordered_map<int32_t, int32_t>& _ipm){
        ipm = _ipm;
        status_[4] = true;
    }
    void set_ipm(unordered_map<int32_t, int32_t>&& _ipm){
        ipm = std::move(_ipm);
        status_[4] = true;
    }
    const unordered_map<int32_t, int32_t>& get_ipm() const {
        return ipm;
    }
    unordered_map<int32_t, int32_t>& mutable_ipm(){
        status_[4] = true;
        return ipm;
    }
    bool has_ipm() const{
        return status_[4];
    }

    void set_ipc(const unordered_map<int32_t, SampleAsElem>& _ipc){
        ipc = _ipc;
        status_[5] = true;
    }
    void set_ipc(unordered_map<int32_t, SampleAsElem>&& _ipc){
        ipc = std::move(_ipc);
        status_[5] = true;
    }
    const unordered_map<int32_t, SampleAsElem>& get_ipc() const {
        return ipc;
    }
    unordered_map<int32_t, SampleAsElem>& mutable_ipc(){
        status_[5] = true;
        return ipc;
    }
    bool has_ipc() const{
        return status_[5];
    }

    void set_icp(const unordered_map<SampleAsElem, int32_t, hash_SampleAsElem>& _icp){
        icp = _icp;
        status_[6] = true;
    }
    void set_icp(unordered_map<SampleAsElem, int32_t, hash_SampleAsElem>&& _icp){
        icp = std::move(_icp);
        status_[6] = true;
    }
    const unordered_map<SampleAsElem, int32_t, hash_SampleAsElem>& get_icp() const {
        return icp;
    }
    unordered_map<SampleAsElem, int32_t, hash_SampleAsElem>& mutable_icp(){
        status_[6] = true;
        return icp;
    }
    bool has_icp() const{
        return status_[6];
    }

    void set_icc(const unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem>& _icc){
        icc = _icc;
        status_[7] = true;
    }
    void set_icc(unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem>&& _icc){
        icc = std::move(_icc);
        status_[7] = true;
    }
    const unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem>& get_icc() const {
        return icc;
    }
    unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem>& mutable_icc(){
        status_[7] = true;
        return icc;
    }
    bool has_icc() const{
        return status_[7];
    }

public:
    /* operators */
    SampleMap() = default;
    SampleMap(const SampleMap&) = default;
    SampleMap& operator=(const SampleMap& rhs) = default;
    bool operator==(const SampleMap& rhs) const{
        if (opm != rhs.opm)
            return false;
        if (ocmpc != rhs.ocmpc)
            return false;
        if (ocmcp != rhs.ocmcp)
            return false;
        if (ocmcc != rhs.ocmcc)
            return false;
        if (ipm != rhs.ipm)
            return false;
        if (ipc != rhs.ipc)
            return false;
        if (icp != rhs.icp)
            return false;
        if (icc != rhs.icc)
            return false;
        return true;
    }

    bool operator!=(const SampleMap& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const SampleMap& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ opm(opm)
            ^ ocmpc(ocmpc)
            ^ ocmcp(ocmcp)
            ^ ocmcc(ocmcc)
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : ipm){
                    code ^= std::hash<int32_t>()(_i.first)^ std::hash<int32_t>()(_i.second);
                }
                return code;
            }()
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : ipc){
                    code ^= std::hash<int32_t>()(_i.first)^ _i.second(_i.second);
                }
                return code;
            }()
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : icp){
                    code ^= _i.first(_i.first)^ std::hash<int32_t>()(_i.second);
                }
                return code;
            }()
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : icc){
                    code ^= _i.first(_i.first)^ _i.second(_i.second);
                }
                return code;
            }();
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    OutterPrimayMap opm;
    OutterComplexMapPC ocmpc;
    OutterComplexMapCP ocmcp;
    OutterComplexMapCC ocmcc;
    unordered_map<int32_t, int32_t> ipm;
    unordered_map<int32_t, SampleAsElem> ipc;
    unordered_map<SampleAsElem, int32_t, hash_SampleAsElem> icp;
    unordered_map<SampleAsElem, SampleAsElem, hash_SampleAsElem> icc;

    /* for optional */
    bitset<8> attrs_ = 0;
    bitset<8> status_;

public:
    static SampleMapTypeTree s_type_tree;
};

struct hash_SampleMap
{
    size_t operator()(const SampleMap& rhs) const{
        return rhs(rhs);
    }
};

}}//end namespace

#endif


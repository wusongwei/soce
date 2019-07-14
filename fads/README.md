# FADS
Fads为Fast Access Data Serialization的简称，可根据索引下标快速访问到指定的成员。

## 基本思路
当Schema确定后，只允许添加，不允许删除。Fads为每种复合类型都添加一个长度信息，从而
可以快速跳过整个成员。

## 类型说明

类型名|说明|序列化方式
:---|:---|:---
null|表示此项未设置|一个字节，内容为0
bool|布尔类型|一个字节
i32|32位带符号整形|长度不定，采用zigzag算法压缩，
i64|64位带符号整形|长度不定，采用zigzag算法压缩
float|浮点类型|四个字节，大端字节序
double|双精度浮点类型|八个字节，大端字节序
string|字符串|长度+内容，长度采用zigzag算法压缩
byte[]|固定长度的字符|长度+内容，长度采用zigzag算法压缩
enum|定义枚举类型|长度不定，采用zigzag算法压缩
list|定义列表|不定长度信息 + 一字节元素类型信息 + 不带类型信息的具体成员
set|定义集合|不定长度信息 + 一字节元素类型信息 + 不带类型信息的具体成员
map|定义映射|不定长度信息 + 一字节键类型信息 + 一字节值类型信息 + 不带类型信息的具体成员
struct|结构体|不定长度信息 + 元素个数 + 各元素的类型及内容

## 定义规范
类型定义格式统一为：
```
[keyword] [name]{
    // defination
}
```

## 限定词
Fads中提供一些关键字用于限定，比如使用require表示必须存在。
限定词只能在struct类型中使用，单独一行，比如：
```
struct Person{
    string name;
    i32 age;
    
    require {name}
}

```
示例表示Person类型有两个成员，其中成员name是必须的，成员age是可选的。
后续还可以添加default、max、min等限定。

## 注释
井号到行尾的内容为注释内容。

## 示例
基础类型定义
```
enum Color
{
    red = 1,
    yellow,
    blue
}

struct PrimaryType
{
    bool a;
    i32 b;
    i64 c;
    float d;
    double e;
    string f;
    byte[4] g;
    Color color;
}
```
复合类型既可在struct内部定义，也可在外部定义。list与set定义基本一致。
```
struct SampleAsElem
{
    i32 a;
}

# 外部定义的list类型，成员类型为i32。
list OutterPrimayList {i32}

# 外部定义的list类型，成员类型为前面定义的struct类型。
list OutterComplexList {SampleAsElem}

# 外部定义的list类型，成员类型为list，即嵌套list
list OutterMultiList {OutterPrimayList}

struct SampleList
{
    # 外部定义的类型可用于成员类型定义
    OutterPrimayList opl;
    OutterComplexList ocl;
    OutterMultiList oml;
    
    # 内部定义list
    list<i32> ipl;
    list<SampleAsElem> icl;
    list<OutterPrimayList> iml;
}
```

map类型示例。
```
map OutterPrimayMap {i32, i32}
map OutterComplexMapPC {i32, SampleAsElem}
map OutterComplexMapCP {SampleAsElem, i32}
map OutterComplexMapCC {SampleAsElem, SampleAsElem}

struct SampleMap
{
    OutterPrimayMap opm;
    OutterComplexMapPC ocmpc;
    OutterComplexMapCP ocmcp;
    OutterComplexMapCC ocmcc;
    
    map<i32, i32> ipm;
    map<i32, SampleAsElem> ipc;
    map<SampleAsElem, i32> icp;
    map<SampleAsElem, SampleAsElem> icc;
}
```

## 使用
Fads提供了BinaryProto类来进行序列化和反序列化。详细示例参见：*soce/fads/sample/sample.cpp*   
序列化基本用法：
```
PrimaryType pt;
// init pt
BinaryProto bp;
if (pt.serialize(&bp) == 0){
    // serialize failed
}else{
     // 通过BinaryProto的data()和size()接口可访问序列化后的数据。
     cout <<　string(bp.data(), bp.size()) << endl;
 }
```
反序列化基本用法：
```
PrimaryType pt;
BinaryProto bp;
bp.init(data, size);
if (pt.deserialize(&bp) == 0){
    // deserialize failed
}
```

## 动态获取
Fads提供了根据索引下标快速访问元素的能力。struct类型中可能含有多个成员，每个
成员的顺序是固定的，如果将每个成员从0号开始编号，则每个成员拥有唯一编号。将多
级struct类型中的成员以点号分隔则构成索引下标。   
Fads提供了DynamicGetter类来实现在不完全解析序列化数据的情况下，快速访问指定
成员能力。DynamicGetter提供两种类型的获取方式：
- 直接返回指定索引下标成员的值。
```
DynamicGetter dg;
// 参数data和size为序列化后的数据内容和长度
dg.init(data, size);

// 准备索引下标
vector<uint32_t> indexes = {0, 1};

int32_t val;
dg.get_int32(indexes, val);
```
- 添加多个索引下标，一次返回所有成员的值。
```
DynamicGetter dg;
// 参数data和size为序列化后的数据内容和长度
dg.init(data, size);

// 准备第一个成员
int32_t i;
dg.get_int32("0.1", i);

// 准备第二个成员
string s;
dg.get_int32("0.1.1", s);

// 执行获取运作
dg.do_get();
```
## Roadmap
- 内部优化，对缓存下标的解析结果做缓存。
- 提供Fads类型与Json类型互转的能力。
- 提供DynamicGetter访问list、set、map等内部成员的能力。
- 提供在无schema情况下动态解析，动态添加成员、删除成员等功能。


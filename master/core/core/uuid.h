#ifndef _klib_uuid_h_
#define _klib_uuid_h_


#include "../inttype.h"
#include <string>


namespace klib {
namespace core {

// ����uuid
std::string create_uuid();


// twitter snowflake�㷨
// 64       63--------------22---------12---------0
// ����λ   |     41λʱ��   |10λ������|12λ������|
extern uint64_t get_time();

class unique_id_t
{
public:
    unique_id_t();
    ~unique_id_t();

    void set_epoch(uint64_t epoch);
    void set_machine(int32_t machine);
    int64_t generate();

private:
    uint64_t epoch_;
    uint64_t time_;
    int32_t machine_;
    int32_t sequence_;
};

}}

#endif // _klib_uuid_h_
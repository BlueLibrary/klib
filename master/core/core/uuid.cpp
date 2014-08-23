#include "uuid.h"

#ifdef WIN32
#else
#include <uuid/uuid.h>
#endif

#if defined(__GUNC__)
#include <sys/time.h>
#include <unistd.h>
#define EPOCHFILETIME 11644473600000000ULL
#else
#include <windows.h>
#include <time.h>
#define EPOCHFILETIME 11644473600000000Ui64
#endif

using namespace klib::core;


string create_uuid()
{
    char buf[64] = {0};
#ifdef WIN32
    GUID guid;
    if (S_OK == CoCreateGuid(&guid))
    {
        _snprintf(buf, sizeof(buf),
            "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
    }
#else
    uuid_t uu;  
    uuid_generate(uu);
    if (!uuid_is_null(uu)) 
    {
        snprintf(buf, sizeof(buf),
            "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            uu[0], uu[1], uu[2], uu[3], uu[4], uu[5], uu[6], uu[7], 
            uu[8], uu[9], uu[10], uu[11], uu[12], uu[13], uu[14], uu[15]);
    }
#endif

    return buf;
}



uint64_t get_time()
{
#ifdef __GUNC__
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64 time = tv.tv_usec;
    time /= 1000;
    time += (tv.tv_sec * 1000);
    return time;
#else
    FILETIME filetime;
    uint64_t time = 0;
    GetSystemTimeAsFileTime(&filetime);

    time |= filetime.dwHighDateTime;
    time <<= 32;
    time |= filetime.dwLowDateTime;

    time /= 10;
    time -= EPOCHFILETIME;
    return time / 1000;
#endif
}

unique_id_t::unique_id_t()
{
    epoch_ = 0;
    time_ = 0;
    machine_ = 0;
    sequence_ = 0;
}

unique_id_t::~unique_id_t()
{

}

void unique_id_t::set_epoch(uint64_t epoch)
{
    epoch_ = epoch;
}

void unique_id_t::set_machine(int32_t machine)
{
    machine_ = machine;
}

int64_t unique_id_t::generate()
{
    int64_t value = 0;
    uint64_t time = get_time() - epoch_;

    // ������41λʱ��
    value = time << 22;

    // �м�10λ�ǻ���ID
    value |= (machine_ & 0x3FF) << 12;

    // ���12λ��sequenceID
    value |= sequence_++ & 0xFFF;
    if (sequence_ == 0x1000)
    {
        sequence_ = 0;
    }

    return value;
}

#ifdef __TEST__
#include <iostream>
void test()
{
    utils::unique_id_t* u_id_ptr = new utils::unique_id_t();
    u_id_ptr->set_epoch(uint64_t(1367505795100));
    u_id_ptr->set_machine(int32_t(100));
    for (int i = 0; i < 1024; ++i)
    {
        std::cout << u_id_ptr->generate() << std::endl;;
    }
}
#endif

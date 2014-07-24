#pragma once

//KTrace_ID(id, "event");
//KTrace_Thread("");

#ifdef _WIN32
#include <Windows.h>

#else
#endif


#ifdef _WIN32
typedef DWORD  thread_id_type;
#else
#endif


#include "log_helper.h"



namespace klib{
namespace debug{


class tracer
{
    tracer();
public:
    ~tracer();

    static tracer* instance();
    Logger* get_logger();

    std::string call_backsss(const std::string& id, const std::string& root_path, tm*)
    { 
        return root_path + id;
    }

protected:
    thread_id_type get_thread_id();

public:
    file_device         device_;
    Logger              logger_;
    bool                switch_;
};


}}


inline klib::debug::Logger* def_tracer_loger()
{
    return klib::debug::tracer::instance()->get_logger();
}

inline klib::debug::log_helper get_tracer_loger(const std::string& identify)
{
    klib::debug::tracer* tracre_ = klib::debug::tracer::instance();
    klib::debug::log_device_callback callback = std::bind(&klib::debug::tracer::call_backsss, 
                                            tracre_, 
                                            identify, 
                                            std::placeholders::_1, 
                                            std::placeholders::_2);

    return std::move(klib::debug::log_helper(def_tracer_loger())
                    .set_log_callback(callback)
                    .set_log_device(&tracre_->device_));
}

#define TRACE_PATH(path)        \
    klib::debug::tracer* tracre_ = klib::debug::tracer::instance();\
    tracre_->device_.set_log_path(path);

// �����ɹ���ʱ���¼��־
#define TRACE_ID(identify) \
    if( ! klib::debug::tracer::instance()->switch_ ) ; \
  else get_tracer_loger(identify).set_ctx(ENSURE_DEBUG_LOG, \
                #identify,\
                __FILE__,\
                __LINE__).LOG_FORMATOR_A



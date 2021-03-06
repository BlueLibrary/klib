

#ifndef _klib_timer_test_h
#define _klib_timer_test_h

#include <core/timer_mgr.h>


bool print_info()
{
    printf("timer per second !!! \r\n");
    return true;
}

bool print_info_2second()
{
    printf("timer 2 second !!! \r\n");
    return true;
}



TEST(timer, 1)
{

    timer_mgr t;

    t.start();


    klib::timer_func_type f = std::bind(print_info);
    klib::timer_func_type f2 = print_info_2second;
    //klib::TimerFunType fa = print_info;
    timer* tmr = t.add(1000, f);
    t.add(2000, f2);

    t.remove(tmr);

    t.stop();

}

#endif

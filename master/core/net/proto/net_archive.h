#ifndef _klib_net_archive_h_
#define _klib_net_archive_h_


#include <string>
#include <list>
#include <map>
#include <vector>



#if (!defined(WIN32) || !(defined(_WIN32)))
typedef  unsigned int UINT;
typedef    int   BOOL;
typedef  unsigned short USHORT;
typedef  unsigned char  UCHAR;
typedef  unsigned long  DWORD;


#include "string.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <endian.h>

#else
#define __BYTE_ORDER  __LITTLE_ENDIAN

#if _MSC_VER > 1200
#include <cstdint>
#endif

#endif

#include "../../macro.h"
#include "../../inttype.h"



namespace klib {
namespace net {

typedef unsigned int length_type;


// �������л���
class net_archive 
{
public:
    net_archive() :  
        error_flag_(false)
    {}

    net_archive(char* buff, UINT buffLen) {
        set_buff(buff, buffLen);
        error_flag_ = false;
    }

    // ����Ҫ���л��Ļ�����
    void set_buff(char* buff, UINT buflen) {
        orig_pos_ = buff;
        buff_len_ = buflen;
        op_pos_   = (char*) orig_pos_;
    }

    // �������õĻ�����ָ��
    char* get_buff() {
        return (char*) orig_pos_;
    }

    // ����д��ĳ���
    UINT length() {
        return buff_len_;
    }

    // ���»ص����
    void reset() {
        set_buff((char*)orig_pos_, buff_len_);
    }

    //�����Ƿ�����־
    bool get_error() {
        return error_flag_;
    }

    //////////////////////////////////////////////////////////////////////////
    // ������һЩ���л�����
    net_archive& operator << (uint8_t a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(op_pos_, &a, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator >> (uint8_t& a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(&a, op_pos_, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator << (int8_t a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(op_pos_, &a, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator >> (int8_t& a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(&a, op_pos_, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator << (unsigned int a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        a = KHTON32(a);
        memcpy(op_pos_, &a, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator >> (unsigned int& a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(&a, op_pos_, sizeof(a));
        a = KHTON32(a);
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator << (unsigned short a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        a = KHTON16(a);
        memcpy(op_pos_, &a, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator >> (unsigned short& a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(&a, op_pos_, sizeof(a));
        a = KNTOH16(a);
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator << (BOOL a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        a = KHTON32(a);
        memcpy(op_pos_, &a, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator >> (BOOL& a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(&a, op_pos_, sizeof(a));
        a = KNTOH32(a);
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator << (long a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        a = KHTON32(a);
        memcpy(op_pos_, &a, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator << (unsigned long a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        a = KNTOH32(a);
        memcpy(op_pos_, &a, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator >> (long a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(&a, op_pos_, sizeof(a));
        a = KNTOH32(a);
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator >> (unsigned long& a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(&a, op_pos_, sizeof(a));
        a = KNTOH32(a);
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator << (uint64_t a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        a = KNTOH64(a);
        memcpy(op_pos_, &a, sizeof(a));
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator >> (uint64_t& a) {
        if (this->get_data_len() + sizeof(a) > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        memcpy(&a, op_pos_, sizeof(a));
        a = KNTOH64(a);
        op_pos_ += sizeof(a);
        return *this;
    }

    net_archive& operator << (::std::string& str) {
        if (this->get_data_len() + sizeof(length_type) + str.size() > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        this-> operator << ((length_type) str.size());
        memcpy(op_pos_, str.c_str(), str.size());
        op_pos_ += str.size();
        return *this;
    }

    net_archive& operator << (const ::std::string& str) {
        if (this->get_data_len() + sizeof(length_type) + str.size() > buff_len_) {
            error_flag_ = true;
            return *this;
        }
        this-> operator << ((length_type) str.size());
        memcpy(op_pos_, str.c_str(), str.size());
        op_pos_ += str.size();
        return *this;
    }

    net_archive& operator >> (::std::string& str) {
        if (this->get_data_len() + sizeof(length_type) + str.size() > buff_len_) {
            error_flag_ = true;
            return *this;
        }

        // extract length
        length_type len;
        this-> operator >> (len);

        // judge
        if (this->get_data_len() + len > buff_len_) {
            error_flag_ = true;
            return *this;
        }

        // archive string
        str = ::std::string(op_pos_, len);
        op_pos_ += str.size();
        return *this;
    }

    template <class T>
    net_archive& operator << (const T& t)
    {
        ::operator << (*this, t);
        return *this;
    }

    template <class T>
    net_archive& operator << (::std::list<T>& theList) {
        if (this->get_data_len() + sizeof(length_type) > buff_len_) {
            error_flag_ = true;
            return *this;
        }

        this-> operator << ((length_type)theList.size());
        auto itr = theList.begin();
        for (; itr != theList.end(); ++itr) {
            if (this->get_data_len() + sizeof(length_type) > buff_len_) {
                error_flag_ = true;
                return *this;
            }

            this-> operator << (*itr);
        }
        return *this;
    }

    template <typename T>
    net_archive& operator >> (::std::list<T>& theList) {
        length_type len;
        T target;

        if (this->get_data_len() + sizeof(length_type) > buff_len_) {
            error_flag_ = true;
            return *this;
        }

        this->operator >> (len);
        for (int i=0; i<len; ++i) {
            this->operator >> (target);
            theList.push_back(std::move(target));
        }
        return *this;
    }

    template <class T>
    net_archive& operator << (const ::std::vector<T>& theList) {
        if (this->get_data_len() + sizeof(length_type) > buff_len_) {
            error_flag_ = true;
            return *this;
        }

        this-> operator << ((length_type)theList.size());
        auto itr = theList.begin();
        for (; itr != theList.end(); ++itr) {
            if (this->get_data_len() + sizeof(length_type) > buff_len_) {
                error_flag_ = true;
                return *this;
            }

            this-> operator << (*itr);
        }
        return *this;
    }

    template <typename T>
    net_archive& operator >> (::std::vector<T>& theList) {
        length_type len;
        T target;

        if (this->get_data_len() + sizeof(length_type) > buff_len_) {
            error_flag_ = true;
            return *this;
        }

        this-> operator >> (len);
        for (int i=0; i<len; ++i) {
            this-> operator >> (target);
            theList.push_back(std::move(target));
        }
        return *this;
    }

    template <typename TKey, typename TVal>
    net_archive& operator << (::std::map<TKey, TVal>& theMap) {
        if (this->get_data_len() + sizeof(length_type) > buff_len_) {
            error_flag_ = true;
            return *this;
        }

        this-> operator << ((length_type)theMap.size());
        typename ::std::map<TKey, TVal>::iterator itr = theMap.begin();
        for (; itr != theMap.end(); ++itr) {
            this-> operator << (itr->first);
            this-> operator << (itr->second);
        }
        return *this;
    }

    template <typename TKey, typename TVal>
    net_archive& operator >> (::std::map<TKey, TVal>& theMap) {
        if (this->get_data_len() + sizeof(length_type) > buff_len_) {
            error_flag_ = true;
            return *this;
        }

        length_type len;
        this-> operator >> (len);
        TKey theKey;
        TVal theVal;
        for (int i=0; i<len; ++i) {
            this-> operator >> (theKey);
            this-> operator >> (theVal);
            theMap[theKey] = std::move(theVal);
        }
        return *this;
    }

    //����д�����ݵĳ���
    unsigned int get_data_len() const {
        return op_pos_ - orig_pos_;
    }

    operator std::string() const
    {
        return std::string(orig_pos_, (size_t) get_data_len());
    }

private:
    char*   op_pos_;            ///< ��ǰ�ڻ�������λ��
    const char* orig_pos_;      ///< �������ĳ�ʼλ��
    UINT buff_len_;             ///< ���������ܳ���
    bool error_flag_;           ///< �Ƿ����(��������С�������)
};

inline std::string archive(const std::string& str)
{
    size_t bf_size = str.size() + sizeof(length_type);
    char* buff = new char[bf_size];
    if (nullptr == buff)
    {
        return std::string();
    }
    std::shared_ptr<char> p(buff);

    net_archive ar(buff, bf_size);
    return ar << str;
}

}}



#endif  // _klib_net_archive_h_
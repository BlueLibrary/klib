#ifndef _klib_macro_h
#define _klib_macro_h

#include "inttype.h"

namespace klib
{

#define  KLIB_ASSERT(expr)        \
    _ASSERT(expr)

#define SAFE_DELETE(x)	if (x)	{ delete x; x=NULL; }
#define SAFE_DELETE_ARRAY(x) if (x)	{ delete []x; x=NULL; }
#define SAFE_CLOSE_SOCKET(x) if(INVALID_SOCKET!=x)	{ closesocket(x); x=INVALID_SOCKET; }
#define SAFE_FREE_CLASS(x)	if(x)	{ x->_Free(); x=NULL; }
#define SAFE_CLOSE_HANDLE(x) if(x)	{ CloseHandle(x); x=NULL; }
#define SAFE_CLOSE_FILE_HANDLE(x)	if(x)	{ CloseHandle(x); x=INVALID_HANDLE_VALUE; }
#define SAFE_CLOSE_INTERNET_HANDLE(x)	if (x) { InternetCloseHandle(x); x=NULL; }
#define SAFE_DELETE_DC(x)		if (x)	{ DeleteDC(x); x=NULL; }
#define SAFE_RELEASE_DC(hwnd,x)		if (x)	{ ReleaseDC(hwnd,x); x=NULL; }
#define	SAFE_DELETE_OBJECT(x)	if (x)	{ ::DeleteObject(x); x=NULL; }
#define SAFE_FREE_LIBRARY(x)	if (x)  { ::FreeLibrary(x); x=NULL; }
#define SAFE_DESTROY_WND(x)		if (x)  { ::DestroyWindow(x); x=NULL; }



///< ������ת��Ϊ�ַ���
#define  CLASS_2_STRING_T(_class_name)     _T(#_class_name)
#define  CLASS_2_STRING_A(_class_name)     (#_class_name)
#define  CLASS_2_STRING_W(_class_name)     L(#_class_name)

#define  TYPE_2_STR_A(_type_name)          (#_type_name)
#define  TYPE_2_STR_W(_type_name)          L(#_type_name)

#define  TO_CHAR(x) (#@x)
#define  TO_STRING(x) #x

// ����תΪ�ַ���
#define  TO_MACRO_STRING( x ) TO_STRING1( x )
#define  TO_MACRO_STRING1( x ) #x

// �ַ�������
#define  CAT_2_STR(first, second)          (#first # second)
#define  CAT_3_STR(first, second, third)   (#first # second # third)
#define  CAT_4_STR(first, second, third, fourth)   (#first # second # third # fourth)

// �ж��Ƿ���16����
#define HEX_CHK( c ) ( ((c) >= '0' && (c) <= '9') ||((c) >= 'A' && (c) <= 'F') ||((c) >= 'a' && (c) <= 'f') )
#define DEC_CHK( c ) ((c) >= '0' && (c) <= '9')

#define OFFSETOF( type, field ) ( (size_t) &(( type *) 0)-> field )
#define FSIZ( type, field ) sizeof( ((type *) 0)->field )

#define ARR_SIZE( a ) ( sizeof( (a) ) / sizeof( (a[0]) ) )
#define INC_SAT( val ) (val = ((val)+1 > (val)) ? (val)+1 : (val))

/**
 * @brief   λ������.
 * @param   x   �������ı���.
 * @param   n   �������ı���λ.
 */
#define BIT_SET(x, n) (x = x|(0x01 << n))
#define BIT_TEST(x, n) ((x & (0x01 << n)) !=0)
#define BIT_CLEAR(x, n) (x = x & ~(0x01 << n))



///< ��һ�������ֽ����16λ��ֵת��Ϊ�����ֽ�����ֵ
inline UINT16 KNTOH16(UINT16 uValue)
{
	return ((uValue & 0xFF00) >> 8) | ((uValue & 0x00FF) << 8);
}

///< ��һ�������ֽ����32λ��ֵת��Ϊ�����ֽ�����ֵ
inline UINT32 KNTOH32(UINT32 uValue)
{
	return ((uValue & 0xFF000000) >> 24)
			| ((uValue & 0x00FF0000) >> 8)
			| ((uValue & 0x0000FF00) << 8)
			| ((uValue & 0x000000FF) << 24);
}

///< ��һ�������ֽ����64λ��ֵת��Ϊ�����ֽ�����ֵ
inline UINT64 KNTOH64(UINT64 uValue)
{
	return ((uValue & 0xFF00000000000000) >> 56)
			| ((uValue & 0x00FF000000000000) >> 40)
			| ((uValue & 0x0000FF0000000000) >> 24)
			| ((uValue & 0x000000FF00000000) >> 8)
			| ((uValue & 0x00000000FF000000) << 8)
			| ((uValue & 0x0000000000FF0000) << 24)
			| ((uValue & 0x000000000000FF00) << 40)
			| ((uValue & 0x00000000000000FF) << 56);
}

#define KHTON16		KNTOH16
#define KHTON32		KNTOH32
#define KHTON64		KNTOH64


///< ������ʳ�Ա�ĺ���
#define DEFINE_ACCESS_FUN(Type, Target)         \
public:                                         \
    inline Type get_##Target() {                \
        return Target;                          \
    }                                           \
    inline void set_##Target(Type t)  {         \
        Target = t;                             \
    }

///< ������ʳ�Ա�ĺ���
#define DEFINE_ACCESS_FUN2(Type, Name, Target)  \
public:                                         \
    inline const Type get_##Name() const {      \
        return Target;                          \
    }                                           \
    inline void set_##Name(const Type t)  {     \
        Target = t;                             \
    }

///< �������÷��ʳ�Ա�ĺ���
#define DEFINE_ACCESS_FUN_REF(Type, Target)     \
public:                                         \
    inline Type& get_##Target()   {             \
        return Target;                          \
    }                                           \
    inline void set_##Target(Type& t)   {       \
        Target = t;                             \
    }

///< �������÷��ʳ�Ա�ĺ���
#define DEFINE_ACCESS_FUN_REF2(Type, Name, Target)     \
public:                                         \
    inline const Type& get_##Name() const  {    \
        return Target;                          \
    }                                           \
    inline void set_##Name(const Type& t)   {   \
        Target = t;                             \
    }

/**
 * @brief   ������ͨ�ĺ���.
 * @param   Type    Ҫ����ĳ�Ա����.
 * @param   Target  ����ĳ�Ա����.
 */
#define  DEFINE_MEMBER_FUN(Type, Target)        \
public:                                         \
    inline Type  get_##Target()  {              \
        return  m_##Target;                     \
    }                                           \
    inline void set_##Target(Type theVal)  {    \
        m_##Target = theVal;                    \
    }                                           \
    Type  m_##Target;

/**
 * @brief   �������Ų��������صĺ���.
 * @param   Type    Ҫ����ĳ�Ա����.
 * @param   Target  ����ĳ�Ա����.
 */
#define DEFINE_MEMBER_FUN_REF(Type, Target)     \
public:                                         \
    inline Type&  get_##Target()  {             \
        return  m_##Target;                         \
    }                                           \
    inline void set_##Target(const Type& theVal)  {   \
       m_##Target = theVal;                        \
    }                                           \
    Type  m_##Target;






























}

#endif

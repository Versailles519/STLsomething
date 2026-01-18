#ifndef __JJALLOC__
#define __JJALLOC__

#include <new>      // for placement new
#include <cstddef>  // for ptrdiff_t, size_t
#include <cstdlib>  // for exit()
#include <climits>  // for UNIT_MAX
#include <iostream> // for cerr

namespace JJ
{
    /*
        申请原始内存
    */
    template <class T>
    inline T *_allocate(ptrdiff_t size, T *)
    {
        /*
            设置当operator new无法分配足够内存时调用的处理函数
            该代码目的是确保内存分配失败时直接返回空指针，而不是调用用户可能设置的错误处理函数
        */
        std::set_new_handler(0);
        // std::set_new_handler(nullptr);

        /*
            (T *)(::operator new((size_t)(size * sizeof(T))))
            整体分为两大部分内容：
            1.(T*)为C风格强制类型转换,强制将后半部分的内容转换为T*
            2.(::operator new((size_t)(size * sizeof(T))))为一整块
                其中::operator new((size_t)(size * sizeof(T)))又分为三部分
                1.函数主体::operator new调用C++的原始内存分配函数,只分配原始内存，不调用构造函数
                2.参数(size_t)(size * sizeof(T))为分配size*sizeof(T)个字节的内容
                3.返回值类型为void*
        */
        T *tmp = (T *)(::operator new((size_t)(size * sizeof(T))));
        /*
        此处为C11风格写法.其中使用这种static_cast<T *>转换是因为这个地方是operator new返回的是void*指针
        T *tmp = static_cast<T *>(::operator new(static_cast<size_t>(size * sizeof(T))));

        此处为
        */
        if (tmp == 0)
        {
            std::cerr << "out of memory" << std::endl;
            exit(1);
        }
        return tmp;
    }

    /*
        释放原始内存
    */
    template <class T>
    inline void _dellocate(T *buffer)
    {
        ::operator delete(buffer);
    }

    /*
        初始化原始内存
        在原始内存的基础上,通过定位new运算符,初始化源代码
    */
    template <class T1, class T2>
    inline void _construct(T1 *p, const T2 &value)
    {
        new (p) T1(value); // placement new. invoke ctor of T1
    }

    /*
        调用析构函数
    */
    template <class T>
    inline void _destroy(T *ptr)
    {
        ptr->~T();
    }

    template <class T>
    class allocator
    {
    public:
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        /*
        // C11写法
        // 类型别名使用using
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        */

        // rebind allocator of type U
        template <class U>
        struct rebind
        {
            typedef allocator<U> other;
        };

        // hint used of locality. ref.[Austern].p189
        pointer allocate(size_type n, const void *hint = 0)
        {
            return _allocate((difference_type)n, (pointer)0);
        }

        void deallocate(pointer p, size_type n)
        {
            _dellocate(p);
        }

        void construct(pointer p, const T &value)
        {
            _construct(p, value);
        }
        void destroy(pointer p)
        {
            _destroy(p);
        }

        pointer address(reference x)
        {
            return (pointer)&x;
        }

        const_pointer const_address(const_reference x)
        {
            return (const_pointer)&x;
        }

        size_type max_size() const
        {
            return size_type(UINT_MAX / sizeof(T));
        }
    };
}

#endif
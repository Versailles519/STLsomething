#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream.h>
#define __THROW_BAD_ALLOC            \
    cerr << "out of memory" << endl; \
    exit(1)
#endif

template <int inst>
class __malloc_alloc_template
{
private:
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);
    static void (*__malloc_alloc_oom_handler)();

public:
    static void *allocate(size_t n)
    {
        void *result = malloc(n); // 第一集配置器直接使用malloc()
        // 以下无法满足需求时，改用oom_malloc()
        if (0 == result)
            result = oom_malloc();
        return result;
    }

    static void deallocate(void *p, size_t /*n */)
    {
        free(p); // 第一级配置器直接使用free()
    }

    static void *reallocate(void *p, size_t /*old_sz*/, size_t new_sz)
    {
        void *result = realloc(p, new_sz); // 第一级配置器直接使用realloc()
        // 以下无法满足需求时，改用oom_realloc()
        if (0 == result)
            result = oom_realloc(p, new_sz);
        return result;
    }

    // 以下仿真C++的set_new_handler().换句话说，你可以通过它
    // 指定你自己的out-of-memory handler
    // void (*set_malloc_handler(void (*f)()))()详细解释：
    //      void (*f)()  --是一个函数指针,f指向了返回值为void,参数为空的函数
    //      set_malloc_handler(void (*f)())  -- set_malloc_handler是一个函数,参数是一个函数指针
    //      void (*set_malloc_handler(void (*f)()))()
    //                  --set_malloc_handler返回一个函数指针
    //                  --该指针指向返回值为void,无参数的函数
    // 总结：该函数接受一个void(*f)()的函数指针作为参数,返回一个void(*set_malloc_handler)()的函数指针
    static void (*set_malloc_handler(void (*f)()))()
    {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;

        return (old);
    }
};

// malloc_alloc out-of-memory handling
// 初值为0.有待客端设定
template <int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void *__malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void (*my_malloc_handler)();
    void *result;

    for (;;) // 不断尝试释放、配置、再释放、再配置...
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler)
        {
            __THROW_BAD_ALLOC;
        }
        (*my_malloc_handler)(); // 调用处理例程，企图释放内存
        result = malloc(n);     // 再次尝试配置内存
        if (result)
            return (result);
    }
}

template <int inst>
void *__malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
    void (*my_malloc_handler)();
    void *result;

    for (;;) // 不断尝试释放、配置、再释放、再配置...
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler)
        {
            __THROW_BAD_ALLOC;
        }
        (*my_malloc_handler)();    // 调用处理例程，企图释放内存
        result = reallocate(p, n); // 再次尝试配置内存
        if (result)
            return (result);
    }
}

// 注意,以下直接将参数inst指定为0
typedef __malloc_alloc_template<0> malloc_alloc;
#include <new.h>

template <class T1, class T2>
inline void construct(T1 *p, const T2 &value)
{
    new (p) T1(value); // placement new; 调用T1::T1(value)
}

// 以下是destroy()第一个版本,接受一个指针
template <class T>
inline void destroy(T *pointer)
{
    pointer->~T(); // 调用dtor ~T()
}

// 以下是destroy()第二个版本,接受两个迭代器。此函数设法找出元素的数值型别
// 进而利用__type_traits<>求取最适当措施
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
    __destory(first, last, value_type(first));
}

// 判断元素的数值型别(value type)是否有trivial destructor
template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T *)
{
    /*
    注:
    对于模板类型的类型重命名必须加typename关键字
    否则编译器不知道has_trivial_destructor是类型还是静态成员
    */
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destrucor;

    __destroy_aux(first, last, trivial_destrucor());
}

// 如果元素的数值类型有non-trivial destructor...
// 对于容器类型,比如string和vector等,他为非平凡类型,需要循环调用析构函数回收资源
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
    for (; first < last; ++first)
        destroy(&*first);
}

// 如果元素的数值类型有trivial destructor
// 对于普通类型,比如int,double等,他为平凡类型,没有额外申请内存空间,故不需要回收资源
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator, __true_type) {}

// 以下是destroy()第二版本对迭代器为char*和wchar_t*的特化版
inline void destroy(char *, char *) {}
inline void destroy(wchar_t *, wchar_t *) {}
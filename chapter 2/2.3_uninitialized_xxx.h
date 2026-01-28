#include <type_traits>

/* uninitialized_fill_n
    args1: 迭代器first指向准备初始化空间的起始处
    args2: 准备初始化空间的大小
    args3: 空间的初始值
*/
template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x)
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T &x, T1 *)
{
    /*
        POD类型(plain old data, 平凡旧数据类型)主要分为两部分：
        1.trivial(平凡的)：没有自定义构造、析构、拷贝构造、移动构造、赋值构造函数, 以及不能包含虚函数和虚基类
        2.standard layout(标准布局):
            1.所有非静态成员有相同的访问控--不能：有的public，有的private
            2.没有虚函数或虚基类
            3.所有非静态成员都是标准布局
            4.所有基类都是标准布局
            5.没有与第一个非静态成员类型相同的基类
            6.在一个类及其所有基类中，最多只有一个有非静态成员

        注：简单理解,所有的基础类型都是POD类型,可以直接调用memcpy,memset等函数
    */
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

/*
    如果copy constructio等同于assignment,而且destructor是trivial,以下就有效
    如果是POD类型,执行流程就会转进到以下函数。
 */
template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type)
{
    // 底层直接使用memcpy等函数直接操作内存
    return fill_n(first, n, x);
}

/*
    如果不是POD类型，执行流程就会转进到以下函数。
*/
template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type)
{
    // 必须一个个的调用拷贝构造函数给内存上赋值
    ForwardIterator cur = first;
    for (; n > 0; --n, ++cur)
    {
        construct(&*cur, x)
    }
    return cur;
}

/* uninitialized_copy
    args1: 迭代器first指向输入端的起始位置
    args2: 迭代器last指向输入端的结束位置(前闭后开区间)
    args3: 迭代器result指向输出端（准备初始化空间）的起始处
*/
template <class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, value_type(result));
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

/*
    如果copy constructio等同于assignment,而且destructor是trivial,以下就有效
    如果是POD类型,执行流程就会转进到以下函数。
 */
template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
{
    // 调用STL算法copy()
    return copy(first, last, result);
}

/*
    如果不是POD类型，执行流程就会转进到以下函数。
*/
template <class InputIterator, class ForwardIterator>
ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
{
    ForwardIterator cur = result;
    for (; first != last; ++first, ++cur)
        construct(&*cur, *first); // 必须一个个元素地构造，无法批量进行
    return cur;
}

// 针对const char*的特化版本
inline char *uninitialized_copy(const char *first, const char *last, char *result)
{
    memmove(result, first, last - first);
    return result + (last - first);
}

// 针对const wchar_t*的特化版本
inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result)
{
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

/*
    uninitialized_fill
    args1: 迭代器first指向输出端(准备初始化空间)的起始位置
    args2: 迭代器last指向输出端(准备初始化空间)的结束位置(前闭后开区间)
    args3: 准备初始化空间的初始值
*/
template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x)
{
    __uninitialized_fill(first, last, x, value_type(first));
}

template <class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x, T1 *)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, x, is_POD());
}

/*
    如果copy constructio等同于assignment,而且destructor是trivial,以下就有效
    如果是POD类型,执行流程就会转进到以下函数。
 */
template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __true_type)
{
    // 调用STL算法fill()
    return fill(first, last, x);
}

/*
    如果不是POD类型，执行流程就会转进到以下函数。
*/
template <class ForwardIterator, class T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __false_type)
{
    ForwardIterator cur = result;
    for (; cur != last; ++cur)
        construct(&*cur, *x); // 必须一个个元素地构造，无法批量进行
}
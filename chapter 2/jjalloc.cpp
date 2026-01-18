#include "JJ_allocator.h"
#include <vector>
#include <iostream>

int main()
{
    using namespace std;
    int ia[5] = {0, 1, 2, 3, 4};
    unsigned int i;

    vector<int, JJ::allocator<int>> iv(ia, ia + 5);
    for (i = 0; i < iv.size(); ++i)
        cout << iv[i] << ' ';
    cout << endl;
}

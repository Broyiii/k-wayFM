#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

#define TIME_START              \
    clock_t ____start, ____end; \
    ____start = clock()
#define TIME_OUT                \
    ____end = clock();          \


unordered_set<int> t1;
vector<int> t2;
int i = 0;

void hash1()
{
    for (auto x : t1)
    {
        i += x;
        // cout << i << endl;
    }
    // cout << i << endl;
    i ++;
}

void vec()
{
    for (auto x : t2)
    {
        i += x;
        // cout << i << endl;
    }
    // cout << i << endl;
    i ++;
}

int main()
{
    clock_t ____start, ____end;

    ____start = clock();
    for (int i = 0; i < 10000000; ++i)
    {
        t1.insert(i);
    }
    ____end = clock();
    printf("- Hash   Insert Time = %0ld ms\n", (____end - ____start) / 1000); 
    for (int i = 0; i < 10000000; ++i)
    {
        t2.emplace_back(i);
    }
    ____start = clock();
    printf("- Vector Insert Time = %0ld ms\n", (____start - ____end) / 1000); 
    
    // ____start = clock();
    hash1();
    ____end = clock();
    printf("- Hash   Iter Time = %0ld ms\n", (____end - ____start) / 1000); 

    vec();
    ____start = clock();
    printf("- Vector Iter Time = %0ld ms\n", (____start - ____end) / 1000); 
    return 0;
}
// test.cc
// test efektivity standardnich kontejneru C++

const int MAXSZ = 10000000; // limit velikosti kontejneru
const int N = 20;

#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <algorithm>

/*  rdtsc
 *  function for reading 64 bit timer (Pentium, K6, etc.) 
 *  for GNU C/C++ only 
 */
#ifndef __GNUC__
#error "use GNU C, please"
#endif

// use RDTSCP for better accuracy
unsigned int xxx;
#define rdtsc() __builtin_ia32_rdtscp(&xxx)


template<typename Container>
int test_push_back(Container &c, int sz)
{
    typename Container::value_type x(0); // hodnota
    c.clear();
    for(int i=0;i<sz;i++)
        c.push_back(i);         // naplneni

    unsigned ta[N];
    long long t, t1, t2, t3;
    for(int j = 0; j < N; j++) {
        t1 = rdtsc();
        t2 = rdtsc();
        c.push_back(x); // operace
        t3 = rdtsc();
        t = t3 - t2 - (t2 - t1); // kompenzace
        ta[j] = t;		 // zaznam mereni
    }
    std::sort(ta, ta+N);
    c.clear();
    return ta[N / 2];    /* median */
}

template<typename Container>
int test_insert_begin(Container &c, int sz)
{
    typename Container::value_type x(0); // hodnota
    c.clear();
    for(int i=0;i<sz;i++)
        c.push_back(i);         // naplneni

    unsigned ta[N];
    long long t, t1, t2, t3;
    for(int j = 0; j < N; j++) {
        t1 = rdtsc();
        t2 = rdtsc();
        c.insert(c.begin(),x); // operace
        t3 = rdtsc();
        t = t3 - t2 - (t2 - t1); // kompenzace
        ta[j] = t;		 // zaznam mereni
    }
    std::sort(ta, ta+N);
    c.clear();
    return ta[N / 2];    /* median */
}

template<typename Container>
int test_insert(Container &c, int sz)
{
    c.clear();
    for(int i=0;i<sz;i++)
        c.insert(i+1);         // naplneni kontejneru

    unsigned ta[N];
    long long t, t1, t2, t3;
    for(int j = 0; j < N; j++) {
        typename Container::value_type x(j*sz/N); // hodnota
        t1 = rdtsc();
        t2 = rdtsc();
        c.insert(x); // operace
        t3 = rdtsc();
        t = t3 - t2 - (t2 - t1); // kompenzace
        ta[j] = t;		 // zaznam mereni
    }
    std::sort(ta, ta+N);
    c.clear();
    return ta[N / 2];    /* median */
}

int main()
{
    std::vector<int> v;
    std::deque<int> d;
    std::list<int> l;
    std::set<int> s;
    for(int i=16; i<=MAXSZ; i*=2) {
      std::cout << i << '\t' << test_push_back(v, i)
                     << '\t' << test_insert_begin(v, i)
                     << '\t' << test_push_back(l, i)
                     << '\t' << test_insert_begin(l, i)
                     << '\t' << test_insert(s, i)
                     << '\t' << test_push_back(d, i)
                     << '\t' << test_insert_begin(d, i)
                     << '\n';
    }
}

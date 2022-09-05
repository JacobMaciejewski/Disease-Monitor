#ifndef HEAP_ENTRY
#define HEAP_ENTRY
using namespace std;
#include <iostream>
#include <string>

class HeapEntry
{
private:
    string *dataString;
    int instances;
public:
    HeapEntry(string*, int);
    ~HeapEntry();
    void setString(string*);
    void setInstances(int);
    void incrementInstances();
    string* getString();
    int getInstances();
    bool contains(string*);
    friend bool operator==(HeapEntry&, HeapEntry&);
    friend bool operator<(HeapEntry&, HeapEntry&);
    friend bool operator>(HeapEntry&, HeapEntry&);
    friend ostream& operator<<(ostream&, HeapEntry&);
};


#endif
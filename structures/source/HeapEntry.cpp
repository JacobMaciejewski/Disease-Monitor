#include "../headers/HeapEntry.h"
using namespace std;

 HeapEntry::HeapEntry(string *givenString, int givenInstances)
 {
     this->setString(givenString);
     //initially no instances
     this->setInstances(givenInstances);
 }

//heap entry has just one identifier string, delete it
HeapEntry::~HeapEntry()
{
    delete this->getString();
}

void  HeapEntry::setString(string* givenString)
{
    this->dataString = givenString;
}

void  HeapEntry::setInstances(int givenInstances)
{
    this->instances = givenInstances;
}

void  HeapEntry::incrementInstances()
{
    int currInstances;
    currInstances = this->getInstances();
    this->setInstances(currInstances + 1);
}

string* HeapEntry::getString()
{
    return this->dataString;
}

int  HeapEntry::getInstances()
{
    return this->instances;
}

//checks if given string is equal to entry's one
bool  HeapEntry::contains(string* givenString)
{
    string *currString;
    currString = this->getString();

    return (*currString == *givenString);
}

//checks whether first entry has the same number of instances as the second one
bool operator==(HeapEntry &HeapEntry1, HeapEntry &HeapEntry2)
{
    int heapEntryInstances1, heapEntryInstances2;
    string *heapEntryString1, *heapEntryString2;
    heapEntryInstances1 = HeapEntry1.getInstances();
    heapEntryInstances2 = HeapEntry2.getInstances();
    heapEntryString1 = HeapEntry1.getString();
    heapEntryString2 = HeapEntry2.getString();

    return ((heapEntryInstances1 == heapEntryInstances2) && (*heapEntryString1 == *heapEntryString2));
}

//checks whether first entry has less instances than the second one
bool operator<(HeapEntry &HeapEntry1, HeapEntry &HeapEntry2)
{
    return (!(HeapEntry1 > HeapEntry2) && !(HeapEntry1 == HeapEntry2));
}

//checks whether first entry has more instances than the second one
//if equal, check their strings
bool operator>(HeapEntry &HeapEntry1, HeapEntry &HeapEntry2)
{
    int heapEntryInstances1, heapEntryInstances2;
    string *heapEntryString1, *heapEntryString2;
    heapEntryInstances1 = HeapEntry1.getInstances();
    heapEntryInstances2 = HeapEntry2.getInstances();
    heapEntryString1 = HeapEntry1.getString();
    heapEntryString2 = HeapEntry2.getString();

    if(heapEntryInstances1 > heapEntryInstances2)
    {
      return true;
    }else if(heapEntryInstances1 == heapEntryInstances2)
    {
      if(heapEntryString1 > heapEntryString2)
      {
        return true;
      }else
      {
        return false;
      }
    }else
    {
      return false;
    }
}


ostream& operator<<(ostream &out, HeapEntry &myHeapEntry)
{
    out << endl << "String: " << *(myHeapEntry.getString()) << endl;
    out << "Instances: " << myHeapEntry.getInstances() << endl;
    return out;
}

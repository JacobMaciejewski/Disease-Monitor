#ifndef DATE
#define DATE
#include <string>
#include <iostream>
using namespace std;

class Date
{
private:
    int day;
    int month;
    int year;
public:
    Date(string);
    ~Date();
    void setDay(int);
    void setMonth(int);
    void setYear(int);
    int getDay();
    int getMonth();
    int getYear();
    //useful for easy comparison between dates
    friend bool operator==(Date&, Date&);
    friend bool operator<(Date&, Date&);
    friend bool operator>(Date&, Date&);
    friend ostream& operator<<(ostream&, Date&);
};



#endif
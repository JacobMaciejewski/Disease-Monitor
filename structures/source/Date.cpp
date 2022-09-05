#include "../headers/Date.h"
using namespace std;

Date::Date(string dateString)
{
    string dayString, restString;
    size_t firstIndex, secondIndex;

    firstIndex = dateString.find("-");
    dayString = dateString.substr(0, firstIndex);
    this->setDay(stoi(dayString));

    //getting the part of the string without the day
    restString = dateString.substr(firstIndex + 1);
    secondIndex = restString.find("-");
    this->setMonth(stoi(restString.substr(0, secondIndex)));
    this->setYear(stoi(restString.substr(secondIndex +1)));
}

//date is just a triplet of ints, no special deletion needed
Date::~Date()
{

}

void Date::setDay(int newDay)
{
    this->day = newDay;
}

void Date::setMonth(int newMonth)
{
    this->month = newMonth;
}

void Date::setYear(int newYear)
{
    this->year = newYear;
}

int Date::getDay()
{
    return this->day;
}

int Date::getMonth()
{
    return this->month;
}

int Date::getYear()
{
    return this->year;
}

bool operator==(Date &date1, Date &date2)
{
    bool sameDay, sameMonth, sameYear;
    sameDay = (date1.getDay() == date2.getDay());
    sameMonth = (date1.getMonth() == date2.getMonth());
    sameYear = (date1.getYear() == date2.getYear());

    return (sameDay && sameMonth && sameYear);
}

bool operator<(Date &date1, Date &date2)
{
    int day1, month1, year1, day2, month2, year2;

    day1 = date1.getDay();
    month1 = date1.getMonth();
    year1 = date1.getYear();

    day2 = date2.getDay();
    month2 = date2.getMonth();
    year2 = date2.getYear();

    if(year2 > year1){
        return true;
    }else if(year2 == year1){
        if(month2 > month1){
            return true;
        }else if(month2 == month1){
            if(day2 > day1){
                return true;
            }else{
                return false;
            }
        }else{
            return false;
        }
    }else{
        return false;
    }
}

//date is bigger only when it is not smaller and not equal
bool operator>(Date &date1, Date &date2)
{
    return (!(date1 < date2) && !(date1 == date2));
}


ostream& operator<<(ostream &out, Date &myDate)
{
    out << myDate.getDay() << '/' << myDate.getMonth() << '/' << myDate.getYear();
    return out;
}

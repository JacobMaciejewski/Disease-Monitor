#ifndef RECORD
#define RECORD
#include <string>
#include <vector>
#include <iostream>
#include <bits/stdc++.h> 
#include "Date.h"
#include <string>
using namespace std;

class Record
{
private:
    string *recordID;
    string *patientFirstName;
    string *patientLastName;
    string *diseaseID;
    string *country;
    int age;
    Date *entryDate;
    Date *exitDate;
public:
    Record(string*, string*, string*, string, string, string, string);
    ~Record();
    friend bool operator==(Record&, Record&);
    friend ostream& operator<<(ostream&, Record&);
    void setRecordID(string*);
    void setPatientFirstName(string*);
    void setPatientLastName(string*);
    void setDiseaseID(string*);
    void setCountry(string*);
    void setEntryDate(Date*);
    void setExitDate(Date*);
    void setAge(int);
    void updateExitDate(string*);
    string* getRecordID();
    string* getPatientFirstName();
    string* getPatientLastName();
    string* getDiseaseID();
    string* getCountry();
    int getAge();
    int getAgeGroup();
    Date* getEntryDate();
    Date* getExitDate();
    bool exitInRange(Date*, Date*);
    bool hasValidDates();
    bool inHospital();
};


#endif
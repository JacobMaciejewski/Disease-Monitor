#include "../headers/Record.h"
using namespace std;

Record::Record(string *finalDisease, string *finalCountry, string *recordID, string recordName, string recordSurname, string recordAge, string recordDate)
{
    string *name, *surname;
    Date *entryDate;

    //got references from dis, cntr and id hashtables
    this->setDiseaseID(finalDisease);
    this->setCountry(finalCountry);
    this->setRecordID(recordID);
    //when constructing a record, it has no exit date
    this->setExitDate(NULL);

    name = new string(recordName);
    surname = new string(recordSurname);

    this->setPatientFirstName(name);
    this->setPatientLastName(surname);
    this->setAge(stoi(recordAge));

    entryDate = new Date(recordDate);
    this->setEntryDate(entryDate);
}

//id, country and disease are pointers to hashtables
//we only need to destroy name, surname and dates
Record::~Record()
{
    delete this->getPatientFirstName();
    delete this->getPatientLastName();
    delete this->getEntryDate();

    //only delete exit date if there is one
    if(this->getExitDate() != NULL)
    {
        delete this->getExitDate();
    }
}

//compares the ids of the two records
bool operator==(Record &record1, Record &record2)
{
    return (*(record1.getRecordID()) == *(record2.getRecordID()));
}

ostream& operator<<(ostream &out, Record &myRecord)
{
    out << "ID: " << *(myRecord.getRecordID()) << endl;
    out << "First name: " << *(myRecord.getPatientFirstName()) << endl;
    out << "Last name: " << *(myRecord.getPatientLastName()) << endl;
    out << "Disease: " << *(myRecord.getDiseaseID()) << endl;
    out << "Age: " << myRecord.getAge() << endl;
    out << "Country: " << *(myRecord.getCountry()) << endl;
    out << "Entry date: " << *(myRecord.getEntryDate()) << endl;
    if(myRecord.getExitDate() == NULL){
        out << "Exit date: STILL IN HOSPITAL!" << endl << endl;
    }else{
        out << "Exit date: " << *(myRecord.getExitDate()) << endl << endl;
    }
    return out;
}

void Record::setRecordID(string *givenID)
{
    this->recordID = givenID;
}

void Record::setPatientFirstName(string *givenFirstName)
{
    this->patientFirstName = givenFirstName;
}

void Record::setPatientLastName(string *givenLastName)
{
    this->patientLastName = givenLastName;
}

void Record::setDiseaseID(string *givenDisease)
{
    this->diseaseID = givenDisease;
}

void Record::setCountry(string *givenCountry)
{
    this->country = givenCountry;
}

void Record::setEntryDate(Date *givenEntryDate)
{
    this->entryDate = givenEntryDate;
}

void Record::setExitDate(Date *givenExitDate)
{
    this->exitDate = givenExitDate;
}

//TO DO: free old exit date
//updating the exit Date
void Record::updateExitDate(string *givenExitDate)
{
    Date *newExitDate;

    //if record had already an exit date, free its contents
    if(this->getExitDate() != NULL)
    {
        delete this->getExitDate();
    }

    //no exit date given (still in hospital)
    if(givenExitDate == NULL)
    {
        this->setExitDate(NULL);
    }else
    {
        newExitDate = new Date(*givenExitDate);
        this->setExitDate(newExitDate);
    }
    return;
}

void Record::setAge(int givenAge)
{
    this->age = givenAge;
}

string* Record::getRecordID()
{
    return this->recordID;
}

string* Record::getPatientFirstName()
{
    return this->patientFirstName;
}

string* Record::getPatientLastName()
{
    return this->patientLastName;
}

string* Record::getDiseaseID()
{
    return this->diseaseID;
}

string* Record::getCountry()
{
    return this->country;
}

int Record::getAge()
{
    return this->age;
}

//returns an index to age group arrray based on its age
int Record::getAgeGroup()
{
    int recordAge = this->getAge();
    if(recordAge >= 0 && recordAge <= 20)
    {
        return 0;
    }else if(recordAge >= 21 && recordAge <= 40)
    {
        return 1;
    }else if(recordAge >= 41 && recordAge <= 60)
    {
        return 2;
    }else
    {
        return 3;
    }
}

Date* Record::getEntryDate()
{
    return this->entryDate;
}

Date* Record::getExitDate()
{
    return this->exitDate;
}

bool Record::hasValidDates(){
    Date *entryDate, *exitDate;
    entryDate = this->getEntryDate();
    exitDate = this->getExitDate();
    //still in hospital
    if(this->getExitDate() == NULL)
    {
        return true;
    //entered hospital after leaving it, invalid
    }else if(*entryDate > *exitDate)
    {
        return false;
    }else
    {
        return true;
    }
}

//checks if given record has no exit date (still in hospital)
bool Record::inHospital()
{
    return (this->getExitDate() == NULL);
}

//checks if given record has exited hospital in given date range
bool Record::exitInRange(Date *entryDate, Date *exitDate)
{
    Date *recordExitDate;
    Date *recordEntryDate;

    recordExitDate = this->getExitDate();
    recordEntryDate = this->getEntryDate();

    if(this->inHospital())
    {
        return false;
    }

    bool lowerBound = ((*recordExitDate > *entryDate) || (*recordExitDate == *entryDate));
    bool upperBound = ((*recordExitDate < *exitDate) || (*recordExitDate == *exitDate));

    if(lowerBound && upperBound)
    {
        return true;
    }else
    {
        return false;
    } 
}


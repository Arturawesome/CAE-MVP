#include "BiMap.cpp"

#include <iostream>
#include <string>

using namespace std;

struct Student {
    string Surname, Name;
};

ostream& operator << (ostream& out, const Student& s) {
    return out << s.Surname << " " << s.Name;
}

int main() {
    BiMap<int, string, Student> bimap;  // студента можно определить либо по номеру, либо по логину
    bimap.Insert(42, {}, {"Ivanov", "Ivan"});
    bimap.Insert({}, "cshse-ami-512", {"Petrov", "Petr"});
    bimap.Insert(13, "cshse-ami-999", {"Fedorov", "Fedor"});

    cout << bimap.GetByPrimaryKey(42) << "\n";  // Ivanov Ivan

    cout << bimap.GetBySecondaryKey("cshse-ami-512") << "\n";  // Petrov Petr

    cout << bimap.GetByPrimaryKey(13) << "\n";  // Fedorov Fedor
    cout << bimap.GetBySecondaryKey("cshse-ami-999") << "\n";  // Fedorov Fedor

    // меняем значение по первичному ключу - по вторичному оно тоже должно измениться
    bimap.GetByPrimaryKey(13).Name = "Oleg";

    cout << bimap.GetByPrimaryKey(13) << "\n";  // Fedorov Oleg
    cout << bimap.GetBySecondaryKey("cshse-ami-999") << "\n";  // Fedorov Oleg
    return 0;
}
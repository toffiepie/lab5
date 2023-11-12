#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Перечисление для пола
enum class Gender { Male, Female };

// Структура данных для представления человека
class Person {
public:
    // Конструктор по умолчанию
    Person() = default;

    // Конструктор с параметрами
    Person(const std::string& name, int age, Gender gender)
        : name(name), age(age), gender(gender) {}

    // Деструктор по умолчанию
    ~Person() = default;

    // Конструктор копирования
    Person(const Person& other) = default;

    // Конструктор перемещения
    Person(Person&& other) noexcept = default;

    // Оператор присваивания
    Person& operator=(const Person& other) = default;

    // Оператор перемещения
    Person& operator=(Person&& other) noexcept = default;

    // Функция для сериализации
    friend std::ostream& operator<<(std::ostream& os, const Person& person);

    // Функция для десериализации
    friend std::istream& operator>>(std::istream& is, Person& person);

private:
    std::string name;  // Строковое поле (имя)
    int age;           // Целочисленное поле (возраст)
    Gender gender;     // Пол (перечисление)
};

// Оператор для сериализации объекта в поток
std::ostream& operator<<(std::ostream& os, const Person& person) {
    // Сериализация каждого поля, разделенного запятой
    os << person.name << "," << person.age << ",";
    switch (person.gender) {
    case Gender::Male:
        os << "M";
        break;
    case Gender::Female:
        os << "F";
        break;
    }
    os << "\n";
    return os;
}

// Оператор для десериализации объекта из потока
std::istream& operator>>(std::istream& is, Person& person) {
    // Десериализация каждого поля
    std::getline(is, person.name, ',');
    is >> person.age;
    char genderChar;
    is >> genderChar;
    switch (genderChar) {
    case 'M':
        person.gender = Gender::Male;
        break;
    case 'F':
        person.gender = Gender::Female;
        break;
    default:
        // Обработка ошибки или установка значения по умолчанию
        person.gender = Gender::Male;
        break;
    }
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Пропуск символа новой строки
    return is;
}

// Функция для сериализации в файл
void serializeToFile(const std::string& filename, const std::vector<Person>& people) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& person : people) {
            file << person;  // Используем оператор <<
        }
        file.close();
    }
    else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

// Функция для десериализации из файла
std::vector<Person> deserializeFromFile(const std::string& filename) {
    std::vector<Person> people;
    std::ifstream file(filename);
    if (file.is_open()) {
        Person person;
        while (file >> person) {  // Используем оператор >>
            people.push_back(person);
        }
        file.close();
    }
    else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
    return people;
}

int main() {
    // Пример использования

    // Создание и сериализация данных в файл
    std::vector<Person> people = {
        {"John Doe", 25, Gender::Male},
        {"Jane Doe", 30, Gender::Female}
    };
    serializeToFile("people.csv", people);

    // Десериализация данных из файла
    auto loadedPeople = deserializeFromFile("people.csv");

    // Внесение изменений
    if (!loadedPeople.empty()) {
        loadedPeople[0] = { "Updated John Doe", 26, Gender::Male };
        loadedPeople.push_back({ "Alice Smith", 28, Gender::Female });
    }

    // Сериализация измененных данных обратно в файл
    serializeToFile("updated_people.csv", loadedPeople);

    return 0;
}

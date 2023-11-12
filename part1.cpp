#include <iostream>
#include <fstream>
#include <array>
#include <vector>

// Перечисление для пола
enum class Gender { Male, Female };

// Структура данных для представления человека
struct Person {
    std::array<char, 50> name;  // Строковое поле (имя)
    int age;                    // Целочисленное поле (возраст)
    Gender gender;              // Пол (перечисление)
};

// Функция для сериализации строки
bool serializeString(const std::array<char, 50>& str, std::ostream& os) {
    os << str.data();
    return os.good();
}

// Функция для сериализации целого числа
bool serializeInt(int value, std::ostream& os) {
    os << value;
    return os.good();
}

// Функция для сериализации перечисления Gender
bool serializeGender(Gender gender, std::ostream& os) {
    switch (gender) {
    case Gender::Male:
        os << "M";
        break;
    case Gender::Female:
        os << "F";
        break;
    }
    return os.good();
}

// Функция для сериализации объекта Person
bool serializePerson(const Person& person, std::ostream& os) {
    // Сериализация каждого поля, разделенного запятой
    if (!serializeString(person.name, os)) return false;
    os << ",";
    if (!serializeInt(person.age, os)) return false;
    os << ",";
    if (!serializeGender(person.gender, os)) return false;
    os << "\n";
    return os.good();
}

// Функция для десериализации строки
bool deserializeString(std::array<char, 50>& str, const char* buffer) {
    std::strncpy(str.data(), buffer, str.size());
    str[str.size() - 1] = '\0';  // Добавляем нулевой символ в конец строки
    return true;
}

// Функция для десериализации целого числа
bool deserializeInt(int& value, const char* buffer) {
    try {
        value = std::atoi(buffer);
        return true;
    }
    catch (...) {
        return false;
    }
}


// Функция для десериализации перечисления Gender
bool deserializeGender(Gender& gender, const char* buffer) {
    if (buffer[0] == 'M') {
        gender = Gender::Male;
        return true;
    }
    else if (buffer[0] == 'F') {
        gender = Gender::Female;
        return true;
    }
    return false;
}

// Функция для десериализации объекта Person
bool deserializePerson(Person& person, const char* buffer) {
    // Десериализация каждого поля
    if (!deserializeString(person.name, buffer)) return false;
    buffer = std::strchr(buffer, ',') + 1;
    if (!deserializeInt(person.age, buffer)) return false;
    buffer = std::strchr(buffer, ',') + 1;
    if (!deserializeGender(person.gender, buffer)) return false;
    return true;
}

// Структура для хранения результата десериализации
struct DeserializationResult {
    bool success;
    Person value;
};

// Функция для десериализации из std::istream
DeserializationResult deserialize(std::istream& stream) {
    Person result;
    std::array<char, 256> buffer;  // Буфер для считывания данных
    // Считывание имени
    stream.getline(buffer.data(), buffer.size(), ',');
    if (stream.gcount() == 0) {
        return { false, {} };
    }
    if (!deserializeString(result.name, buffer.data())) {
        return { false, {} };
    }

    // Считывание возраста
    stream.getline(buffer.data(), buffer.size(), ',');
    if (stream.gcount() == 0) {
        return { false, {} };
    }
    if (!deserializeInt(result.age, buffer.data())) {
        return { false, {} };
    }

    // Считывание пола
    stream.getline(buffer.data(), buffer.size(), '\n');
    if (stream.gcount() == 0) {
        return { false, {} };
    }
    if (!deserializeGender(result.gender, buffer.data())) {
        return { false, {} };
    }

    return { true, result };
}

// Функция для сериализации в файл
void serializeToFile(const std::string& filename, const std::vector<Person>& people) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& person : people) {
            serializePerson(person, file);
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
        while (!file.eof()) {
            auto result = deserialize(file);
            if (result.success) {
                people.push_back(result.value);
            }
            else {
                break;
            }
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
        loadedPeople[0].age = 26;
        loadedPeople.push_back({ "Alice Smith", 28, Gender::Female });
    }

    // Сериализация измененных данных обратно в файл
    serializeToFile("updated_people.csv", loadedPeople);

    return 0;
}

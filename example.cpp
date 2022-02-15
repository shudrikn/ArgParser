#include "arg_parser.h"

using namespace std;

class TestType
{
public:
    int k;
    explicit TestType(int i)
    {
        k = i;
        std::cout << "Constructor " << k << std::endl;
    }
    TestType(const TestType& t)
    {
        k = t.k;
        std::cout << "Copy Constructor " << k << std::endl;
    }
    ~TestType()
    {
        std::cout << "Destructor " << k << std::endl;
    }

    // Перегрузка необходима для вывода значения по умолчанию в help
    friend std::ostream& operator<< (std::ostream& out, const TestType& a);
};

std::ostream& operator<< (std::ostream& out, const TestType& a)
{
    out << a.k;
    return out;
}

void CommandLineArgument<TestType>::SetValue(const std::string& arg_value)
{
    value = std::make_shared<TestType>(std::stoi(arg_value));
}




int main(int argc, char* argv[]) {
    ArgsParser& parser = ArgsParser::GetInstance();

    parser.AddArgument(CommandLineArgument<bool>("b", "-b", false, "boolean arg"));
    parser.AddArgument(CommandLineArgument<int>("i", "-i", 0, "int arg"));
    parser.AddArgument(CommandLineArgument<string>("s", "-s", "", "string arg"));
    parser.AddArgument(CommandLineArgument<TestType>("t", "-t", TestType(3), "custom type arg"));

    parser.PrintHelp();
    parser.Parse(argc, argv);

    cout << "b: " << *parser.GetValue<bool>("b") << endl;
    cout << "i: " << *parser.GetValue<int>("i") << endl;
    cout << "s: " << *parser.GetValue<string>("s") << endl;
    cout << "t: " << *parser.GetValue<TestType>("t") << endl;

    cout << "Fin" << endl;
}
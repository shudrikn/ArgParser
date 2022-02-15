#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include <iostream>
#include <iomanip>

struct BaseCommandLineArgument
{
    std::string name;
    std::string help_description;
    std::string prefix;

    void virtual SetValue(const std::string& arg_value) = 0;
    void* GetValue() const
    {
        if (value)
            return value.get();
        else
            return default_value.get();
    }

    void virtual PrintHelp() const = 0;

protected:

    std::shared_ptr<void> default_value;
    std::shared_ptr<void> value;

    virtual ~BaseCommandLineArgument() {};
};

template <typename T>
struct CommandLineArgument : public BaseCommandLineArgument
{
    CommandLineArgument(const std::string& name, const std::string& prefix, const T& default_value, const std::string& help_description)
    {
        this->name = name;
        this->prefix = prefix;
        this->help_description = help_description;

        this->default_value = std::make_shared<T>(default_value);
        this->value = nullptr;
    }

    void SetValue(const std::string& arg_value) override;

    void PrintHelp() const override
    {
        std::cout << std::left << std::setw(10) << prefix;
        std::cout << std::left << std::setw((help_description.size() / 20 + 1) * 20) << "- " + help_description;
        std::cout << "Default: " << *reinterpret_cast<T*>(default_value.get());
    }
};

void CommandLineArgument<bool>::SetValue(const std::string& arg_value)
{
    value = std::make_shared<bool>(true);
}
void CommandLineArgument<int>::SetValue(const std::string& arg_value)
{
    value = std::make_shared<int>(std::stoi(arg_value));
}
void CommandLineArgument<std::string>::SetValue(const std::string& arg_value)
{
    value = std::make_shared<std::string>(arg_value);
}

class ArgsParser
{
public:
    static ArgsParser& GetInstance()
    {
        static ArgsParser instance;
        return instance;
    }

    template <typename T>
    void AddArgument(const CommandLineArgument<T>& arg)
    {
        auto shared_arg = std::make_shared<CommandLineArgument<T>>(arg);
        arg_by_name[arg.name] = shared_arg;
        arg_by_prefix[arg.prefix] = shared_arg;
    }

    void Parse(int argc, char* argv[])
    {
        for (int i = 0; i < argc; ++i)
        {
            std::string prefix = argv[i];
            if (arg_by_prefix.count(prefix) == 1)
            {
                auto arg = arg_by_prefix[argv[i]];

                std::string next_world = i + 1 < argc ? argv[i + 1] : "";
                arg->SetValue(next_world);
            }
        }
    }

    template <typename T>
    const T* GetValue(std::string name)
    {
        if (arg_by_name.count(name) == 1)
            return reinterpret_cast<const T*>(arg_by_name[name]->GetValue());
        else
            return nullptr;
    }

    void PrintHelp() const
    {
        std::cout << std::endl << "Options:" << std::endl;
        for (auto& prefix_and_argument_pair : arg_by_prefix)
        {
            auto& argument = prefix_and_argument_pair.second;
            std::cout << "  ";
            argument->PrintHelp();
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    ArgsParser(ArgsParser const&) = delete;
    ArgsParser& operator=(ArgsParser const&) = delete;

private:

    std::map<std::string, std::shared_ptr<BaseCommandLineArgument>> arg_by_name;
    std::map<std::string, std::shared_ptr<BaseCommandLineArgument>> arg_by_prefix;

    ArgsParser() {};
    ~ArgsParser() {};
};

#pragma once

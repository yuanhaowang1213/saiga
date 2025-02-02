/**
 * Copyright (c) 2021 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once


#include "saiga/core/util/commandLineArguments.h"

#include "ini.h"


struct IniFileParamIterator
{
    Saiga::SimpleIni* ini;

    template <typename T>
    void SaigaParam(std::string section, T& variable, T default_value, std::string name, std::string comment = "")
    {
        variable = ReadWriteIni(*ini, variable, section, name, comment);
    }

    template <typename T>
    void SaigaParamList(std::string section, T& variable, T default_value, std::string name, char sep,
                        std::string comment = "")
    {
        // INI_GETADD_LIST_COMMENT(*ini, name.c_str(), variable, sep, comment);
        variable = ReadWriteIniList(*ini, variable, section, name, comment, sep);
    }
};

struct ApplicationParamIterator
{
    CLI::App* app;

    template <typename T>
    void SaigaParam(std::string section, T& variable, T default_value, std::string name, std::string comment = "")
    {
        app->add_option("--" + section + "." + name, variable, comment, true);
    }

    template <typename T>
    void SaigaParamList(std::string section, T& variable, T default_value, std::string name, char sep,
                        std::string comment = "")
    {
        if (sep == ' ')
        {
            // app->add_option("--" + name, *variable, comment, true);
        }
    }

    template <typename T>
    void SaigaParamList(std::string section, std::vector<T>& variable, std::vector<T> default_value, std::string name,
                        char sep, std::string comment = "")
    {
        if (sep == ' ')
        {
            app->add_option("--" + section + "." + name, variable, comment, true);
        }
    }
};

struct TablePrintParamIterator
{
    std::ostream& strm;
    int column_width = 15;

    TablePrintParamIterator(std::ostream& strm, int cw) : strm(strm), column_width(cw) {}

    template <typename T>
    void SaigaParam(std::string section, T& variable, T default_value, std::string name, std::string comment = "")
    {
        strm << std::left << std::setw(column_width) << name << std::right << variable << "\n";
    }

    template <typename T>
    void SaigaParamList(std::string section, T& variable, T default_value, std::string name, char sep,
                        std::string comment = "")
    {
        strm << std::left << std::setw(column_width) << name << std::right << "(skipped)"
             << "\n";
    }
};

// The saiga param macros (below) can be used to define simple param structs in ini files.
// An example struct should look like this:
//
//  SAIGA_PARAM_STRUCT(NetworkParams)
//  {
//      SAIGA_PARAM_STRUCT_FUNCTIONS(NetworkParams);
//
//      double d        = 2;
//      long n          = 10;
//      std::string str = "blabla";
//
//      void Params()
//      {
//          SAIGA_PARAM_DOUBLE(d);
//          SAIGA_PARAM_LONG(n);
//          SAIGA_PARAM_STRING(str);
//      }
//  };
//
// To update the parameters by the command line use:
//
//      // First load from config file
//      MyParams params("config.ini");
//
//      // No update from command line
//       CLI::App app{"Example programm", "exmaple_programm"};
//      params.Load(app);
//      CLI11_PARSE(app, argc, argv);

struct ParamsBase
{
    ParamsBase(const std::string name) : name_(name) {}
    std::string name_;

    // virtual void Params(Saiga::SimpleIni* ini, CLI::App* app) = 0;

    // template<class ParamIterator>
    // virtual void Params(ParamIterator* ini) = 0;
};

#define SAIGA_PARAM_STRUCT(_Name)                      \
    using ParamStructType = _Name;                     \
    _Name() : ParamsBase(#_Name)                       \
    {                                                  \
    }                                                  \
    _Name(const std::string file) : ParamsBase(#_Name) \
    {                                                  \
        Load(file);                                    \
    }

#define SAIGA_PARAM_STRUCT_FUNCTIONS                              \
    void Load(CLI::App& app)                                      \
    {                                                             \
        ApplicationParamIterator appit;                           \
        appit.app = &app;                                         \
        Params(&appit);                                           \
    }                                                             \
                                                                  \
                                                                  \
    virtual void Load(std::string file)                           \
    {                                                             \
        Saiga::SimpleIni ini_;                                    \
        ini_.LoadFile(file.c_str());                              \
        IniFileParamIterator iniit;                               \
        iniit.ini = &ini_;                                        \
        Params(&iniit);                                           \
        if (ini_.changed()) ini_.SaveFile(file.c_str());          \
    }                                                             \
                                                                  \
                                                                  \
    virtual void Save(std::string file)                           \
    {                                                             \
        Saiga::SimpleIni ini_;                                    \
        ini_.LoadFile(file.c_str());                              \
        IniFileParamIterator iniit;                               \
        iniit.ini = &ini_;                                        \
        Params(&iniit);                                           \
        ini_.SaveFile(file.c_str());                              \
    }                                                             \
    virtual void Print(std::ostream& strm, int column_width = 30) \
    {                                                             \
        TablePrintParamIterator tableit(strm, column_width);      \
        strm << "[" << name_ << "]\n";                            \
        Params(&tableit);                                         \
    }

#define SAIGA_PARAM_DEFAULT(_variable) (ParamStructType()._variable)

#define SAIGA_PARAM(_variable) \
    if (it) it->SaigaParam(name_, _variable, SAIGA_PARAM_DEFAULT(_variable), #_variable, "")

#define SAIGA_PARAM_COMMENT(_variable, _comment) \
    if (it) it->SaigaParam(name_, _variable, SAIGA_PARAM_DEFAULT(_variable), #_variable, _comment)

#define SAIGA_PARAM_LIST(_variable, _sep) \
    if (it) it->SaigaParamList(name_, _variable, SAIGA_PARAM_DEFAULT(_variable), #_variable, _sep, "")

// a variation where the list is also passed to the command line parser
#define SAIGA_PARAM_LIST2(_variable, _sep) \
    if (it) it->SaigaParamList(name_, _variable, SAIGA_PARAM_DEFAULT(_variable), #_variable, _sep, "")

#define SAIGA_PARAM_LIST_COMMENT(_variable, _sep, _comment) \
    if (it) it->SaigaParamList(name_, _variable, _variable, #_variable, _sep, _comment)

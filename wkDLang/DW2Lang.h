typedef struct IUnknown IUnknown;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "UserString.h"

using namespace std;

#pragma once
class DW2Lang
{
public:

    const int SupportedFileFormatVer = 1;

    // Fields
    int FileFormatVer = 0;
    int CodePage = 0;
    string Name;
    vector<UserString> UserStrings;

    // Constructors
    DW2Lang() {}
    DW2Lang(const string& path) {
        Read(path);
    }

    DW2Lang(ifstream& stream) {
        Read(stream);
    }

    // Methods
    void Read(const string& path) {
        ifstream stream(path, ios::binary);
        if (!stream.is_open()) {
            cerr << "Error opening file: " << path << endl;
            throw std::runtime_error("Error opening language file");
        }

        Read(stream);

        stream.close();
    }

    void Read(istream& stream) {
        stream.seekg(0, ios::beg);

        string sigText;
        sigText.resize(7);
        stream.read(&sigText[0], 7);

        if (sigText.compare("DW2Lang") != 0) {

            throw std::runtime_error("Invalid DW2Lang file signature");
        }

        // Read fields
        stream.read(reinterpret_cast<char*>(&FileFormatVer), sizeof(FileFormatVer));

        if (FileFormatVer > SupportedFileFormatVer)
        {
            throw std::runtime_error("DW2Lang file format version newer than supported");
        }

        stream.read(reinterpret_cast<char*>(&CodePage), sizeof(CodePage));

        Name = ReadString(stream);

        int UserStringCount = 0;
        stream.read(reinterpret_cast<char*>(&UserStringCount), sizeof(UserStringCount));

        for (size_t i = 0; i < UserStringCount; i++)
        {
            UserString us;
            us.Read(stream, FileFormatVer);
            UserStrings.push_back(us);
        }
    }

private:
    string ReadString(istream& stream) 
    {
        int length = 0;
        stream.read(reinterpret_cast<char*>(&length), sizeof(length));

        string text;
        text.resize(length);
        stream.read(&text[0], length);

        return text;
    }
};


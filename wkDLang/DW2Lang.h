typedef struct IUnknown IUnknown;

#include <Windows.h>

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

    const int SupportedFileFormatVer = 2;

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

        if (FileFormatVer == 1)
            stream.read(reinterpret_cast<char*>(&CodePage), sizeof(CodePage));
        else
            CodePage = 65001;

        Name = ReadString(stream);

        if (FileFormatVer >= 2)
        {
            int UserStringsPos = 0;
            stream.read(reinterpret_cast<char*>(&UserStringsPos), sizeof(UserStringsPos));

            stream.seekg(UserStringsPos, std::ios::beg);

            //int TBLCount = 0;
            //stream.read(reinterpret_cast<char*>(&TBLCount), sizeof(TBLCount));

            ////We can ignore loading the TBL for now, as we don't do anything with it in the game
            //for (size_t i = 0; i < TBLCount; i++)
            //{
            //    stream.seekg(2, std::ios::cur);
            //}
        }

        int UserStringCount = 0;
        stream.read(reinterpret_cast<char*>(&UserStringCount), sizeof(UserStringCount));

        for (size_t i = 0; i < UserStringCount; i++)
        {
            UserString us;
            us.Read(stream, FileFormatVer);
            UserStrings.push_back(us);
        }

        if (FileFormatVer == 1)
            CodePage = 65001;
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


typedef struct IUnknown IUnknown;

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#pragma once
#include <string>
class UserString
{
public:
    // Fields
    int ID;
    vector<long long> Offsets;
    string Value;

    // Methods
    void Read(istream& stream, int version) {
        // Read fields
        stream.read(reinterpret_cast<char*>(&ID), sizeof(ID));

        int offsetCount = 0;
        stream.read(reinterpret_cast<char*>(&offsetCount), sizeof(offsetCount));

        for (size_t i = 0; i < offsetCount; i++)
        {
            unsigned long long offset = 0;
            stream.read(reinterpret_cast<char*>(&offset), sizeof(offset));

            Offsets.push_back(offset);
        }

        Value = ReadString(stream);
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


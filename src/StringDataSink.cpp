#include "StringDataSink.h"

// Default constructor
CStringDataSink::CStringDataSink() noexcept{
    // Optionally, initialize members here
}

// Retrieve the string content
const std::string &CStringDataSink::String() const{
    return DString;
}

// Append a single character to the string
bool CStringDataSink::Put(const char &ch) noexcept{
    DString += std::string(1,ch);
    return true;
}

// Append a vector of characters to the string
bool CStringDataSink::Write(const std::vector<char> &buf) noexcept{
    DString += std::string(buf.data(),buf.size());
    return true;
}
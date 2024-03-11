#include "DSVWriter.h"
#include "StringUtils.h"
#include "StringDataSink.h"
#include <sstream>
#include <iostream>

// Implementation struct definition
struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    char DDelimiter;
    bool DQuoteAll;

    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : DDataSink(sink), DDelimiter(delimiter), DQuoteAll(quoteall) {}

    std::string QuoteIfNeeded(const std::string &field) {
    if (DQuoteAll || field.find(DDelimiter) != std::string::npos || field.find('"') != std::string::npos || field.find('\n') != std::string::npos) {
        std::string result = "\"";
        for (char ch : field) {
            if (ch == '"')
                result += "\"\"";
            else
                result += ch;
        }
        result += "\"";
        return result;
    }
    return field;
}
};

// Constructor
CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {}

// Destructor
CDSVWriter::~CDSVWriter() = default;

// WriteRow function
bool CDSVWriter::WriteRow(const std::vector<std::string> &row) {
    std::ostringstream oss;
    for (size_t i = 0; i < row.size(); ++i) {
        oss << DImplementation->QuoteIfNeeded(row[i]);
        if (i + 1 < row.size())
            oss << DImplementation->DDelimiter;
    }
    // Append newline character after each row
    oss << '\n';

    std::string str = oss.str();
    std::cout << "Row data after building: " << oss.str() << std::endl;
    // Remove the newline character at the end if it exists
    if (!str.empty() && str.back() == '\n') {
        str.pop_back();
    }

    return DImplementation->DDataSink->Write(std::vector<char>(str.begin(), str.end()));
}







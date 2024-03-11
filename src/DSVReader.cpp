#include "DSVReader.h"
#include "StringUtils.h"
#include "StringDataSource.h"

#include <sstream>

// Structure to hold implementation details of DSVReader
struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> DDataSource;  
    char DDelimiter;  

    // Constructor
    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DDataSource(src), DDelimiter(delimiter) {
    }
};

// Constructor for DSVReader
CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter) 
    : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {
}

// Destructor for DSVReader
CDSVReader::~CDSVReader() {
}

// Method to check if end of file is reached
bool CDSVReader::End() const {
    return DImplementation->DDataSource->End();  
}

bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    row.clear();  

    char ch;  
    std::string field;  
    bool inQuotedField = false;  

    // Read characters until the end of line or end of data source
    while (!DImplementation->DDataSource->End() && DImplementation->DDataSource->Get(ch)) {
        if (ch == '\n') {  
            if (inQuotedField) {
                field += ch;  
            } else {
                row.push_back(field); 
                return true;  
            }
        } else if (ch == DImplementation->DDelimiter && !inQuotedField) {  
            row.push_back(field); 
            field.clear();  
        } else if (ch == '\"') {
            if (inQuotedField && DImplementation->DDataSource->Peek(ch) && ch == '\"') {
                // Two consecutive double quotes represent an escaped double quote
                field += ch;
                DImplementation->DDataSource->Get(ch); // Consume the peeked character
            } else {
                inQuotedField = !inQuotedField; // Toggle inQuotedField flag
            }
        } else {
            field += ch; // Append character to the current field
        }
    }

    if (!field.empty()) {
        row.push_back(field); 
    }

    return !row.empty();  
}

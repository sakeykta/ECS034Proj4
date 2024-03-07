#include "DSVWriter.h"
#include "StringUtils.h"
#include "StringDataSink.h"

// Implementation struct definition
struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    char DDelimiter;
    bool DQuoteAll;

    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : DDataSink(sink), DDelimiter(delimiter), DQuoteAll(quoteall) {

    }
};

// Constructor
CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {

}

// Destructor
CDSVWriter::~CDSVWriter() {

}

// WriteRow function
bool CDSVWriter::WriteRow(const std::vector<std::string> &row) {

    if (!DImplementation->DDataSink) {
        return false;
    }
    std::string rowString;
    bool quoteNeeded = DImplementation->DQuoteAll;
    for (size_t i = 0; i < row.size(); ++i) {

        if (i > 0) {
            rowString += DImplementation->DDelimiter;
        }
        std::string field = row[i];
        if (!DImplementation->DQuoteAll) {
            if (field.find(DImplementation->DDelimiter) != std::string::npos ||
                field.find('\n') != std::string::npos) {
                quoteNeeded = true;
            }
        }
        if (quoteNeeded) {
            rowString += "\"" + field + "\"";
        } else {
            rowString += field;
        }
    }
    std::vector<char> buffer(rowString.begin(), rowString.end());
    if (!DImplementation->DDataSink->Write(buffer)) {
        return false;
    }
    char newline = '\n';
    if (!DImplementation->DDataSink->Write(std::vector<char>{newline})) {
        return false; 
    }

    return true;
}
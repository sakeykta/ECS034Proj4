#include "XMLWriter.h"
#include <stack>

// Helper function to escape special characters in XML
std::string EscapeXML(const std::string& data) {
    std::string result;
    for (char c : data) {
        switch (c) {
            case '&':
                result += "&amp;";
                break;
            case '"':
                result += "&quot;";
                break;
            case '\'':
                result += "&apos;";
                break;
            case '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            default:
                result += c;
        }
    }
    return result;
}

// Implementation of XMLWriter
struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    std::stack<SXMLEntity> DEntityStack;

    bool WriteEntity(const SXMLEntity& entity) {
        std::string xml_str = "";
        if (entity.DType == SXMLEntity::EType::StartElement) {
            xml_str += "<";
            xml_str += entity.DNameData;
            for (const auto& attr : entity.DAttributes) {
                xml_str += " ";
                xml_str += attr.first;
                xml_str += "=\"";
                xml_str += EscapeXML(attr.second);
                xml_str += "\"";
            }
            xml_str += ">";
            DEntityStack.push(entity);
        }
        else if (entity.DType == SXMLEntity::EType::EndElement) {
            xml_str += "</";
            xml_str += entity.DNameData;
            xml_str += ">";
            if (!DEntityStack.empty())
                DEntityStack.pop();
        }
        else if (entity.DType == SXMLEntity::EType::CompleteElement) {
            xml_str += "<";
            xml_str += entity.DNameData;
            for (const auto& attr : entity.DAttributes) {
                xml_str += " ";
                xml_str += attr.first;
                xml_str += "=\"";
                xml_str += EscapeXML(attr.second);
                xml_str += "\"";
            }
            xml_str += "/>";
        }
        else if (entity.DType == SXMLEntity::EType::CharData) {
            xml_str += EscapeXML(entity.DNameData);
        }
        return DDataSink->Write(std::vector<char>(xml_str.begin(), xml_str.end()));
    }

    bool Flush() {
        std::string end_str = "";
        while (!DEntityStack.empty()) {
            end_str += "</";
            end_str += DEntityStack.top().DNameData;
            end_str += ">";
            DEntityStack.pop();
        }
        return DDataSink->Write(std::vector<char>(end_str.begin(), end_str.end()));
    }

    SImplementation(std::shared_ptr<CDataSink> sink) : DDataSink(sink) {}
};

// Constructor
CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink) {
    DImplementation = std::make_unique<SImplementation>(sink);
}

// Destructor
CXMLWriter::~CXMLWriter() {}

// Flush method
bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}
        
bool CXMLWriter::WriteEntity(const SXMLEntity& entity) {
    return DImplementation->WriteEntity(entity);
}

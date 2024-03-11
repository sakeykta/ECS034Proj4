#include <gtest/gtest.h>
#include "XMLWriter.h"
#include "XMLReader.h"
#include "XMLEntity.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include "DataSink.h"
#include "DataSource.h"
#include <string>
#include <vector>

/*TEST(XML, Slice){
    std::string Base = "Hello";
}*/

TEST(XMLReader, ReadEntityTest){
    auto InputStream = std::make_shared<CStringDataSource>("<example attr=\"Greetings Human\"></example>");
    CXMLReader Reader(InputStream);
    SXMLEntity E;

    EXPECT_TRUE(Reader.ReadEntity(E));
    EXPECT_EQ(E.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(E.AttributeValue("attr"), "Greetings Human");

    EXPECT_TRUE(Reader.ReadEntity(E));
    EXPECT_EQ(E.DType, SXMLEntity::EType::EndElement);
}

TEST(XMLWriter, WriteEntityTest){
    auto OutputStream = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(OutputStream);

    EXPECT_TRUE(Writer.WriteEntity({SXMLEntity::EType::StartElement, "example", {{"attr","Greetings Human"}}}));
    EXPECT_TRUE(Writer.WriteEntity({SXMLEntity::EType::EndElement, "example", {}}));

    EXPECT_EQ(OutputStream->String(), "<example attr=\"Greetings Human\"></example>");
}

TEST(XMLReader, EndTestEmpty){
    // if xml source is empty, end returns true
    auto Source = std::make_shared<CStringDataSource>("");
    CXMLReader Reader(Source);
    EXPECT_TRUE(Reader.End());
}

TEST(XMLReader, EndTestFull){
    // if xml source has elements to be read, end returns false
    auto Source = std::make_shared<CStringDataSource>("Greetings,Human");
    CXMLReader Reader(Source);
    EXPECT_FALSE(Reader.End());
}

TEST(XMLReader, Read){
    auto InputStream = std::make_shared<CStringDataSource>(
        "<node id=\"8\" lat=\"38.5278183\" lon=\"-121.7691588\">"
        "</node>"
        "<node id=\"9\" lat=\"38.5274491\" lon=\"-121.7690089\"/>");
    CXMLReader Reader(InputStream);
    SXMLEntity entity;
    EXPECT_TRUE(Reader.ReadEntity(entity, true));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.AttributeValue("id"), "8");
    EXPECT_TRUE(Reader.ReadEntity(entity, true));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
}

TEST(XMLReader, ReadSimpleElement){
    auto InputStream = std::make_shared<CStringDataSource>("<node id=\"1\" lat=\"1.0\" lon=\"-1.0\"/>");
    CXMLReader Reader(InputStream);
    SXMLEntity E;
    
    EXPECT_TRUE(Reader.ReadEntity(E));
    EXPECT_EQ(E.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(E.DNameData, "node");
    EXPECT_EQ(E.AttributeValue("id"), "1");
    EXPECT_EQ(E.AttributeValue("lat"), "1.0");
    EXPECT_EQ(E.AttributeValue("lon"), "-1.0");
}




// Test(XMLReader, FlushTest){}

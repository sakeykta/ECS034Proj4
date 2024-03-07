/* the original header file
#ifndef OPENSTREETMAP_H
#define OPENSTREETMAP_H

#include "XMLReader.h"
#include "StreetMap.h"

class COpenStreetMap : public CStreetMap{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        COpenStreetMap(std::shared_ptr<CXMLReader> src);
        ~COpenStreetMap();

        std::size_t NodeCount() const noexcept override;
        std::size_t WayCount() const noexcept override;
        std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept override;
        std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept override;
};

#endif
*/

#ifndef OPENSTREETMAP_H
#define OPENSTREETMAP_H

#include "XMLReader.h"
#include "StreetMap.h"
#include <unordered_map>


class COpenStreetMap : public CStreetMap{
    public:
        COpenStreetMap(std::shared_ptr<CXMLReader> src);
        ~COpenStreetMap();

        struct SNode;
        struct SWay;

        std::size_t NodeCount() const noexcept override;
        std::size_t WayCount() const noexcept override;
        std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept override;
        std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept override;
    private:
        struct SImplementation {
            std::shared_ptr<CXMLReader> XMLReader;
            std::vector<std::shared_ptr<SNode>> Nodes;
            std::unordered_map<TNodeID, std::shared_ptr<SNode>> NodesById;
            std::vector<std::shared_ptr<SWay>> Ways;
            std::unordered_map<TWayID, std::shared_ptr<SWay>> WaysById;
        };

        std::unique_ptr<SImplementation> DImplementation;
};

#endif
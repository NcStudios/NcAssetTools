#include "BonesReader.h"

namespace nc::asset
{
void Read(RawNcaBuffer& bytes, std::unordered_map<std::string, uint32_t>* boneNamesToIds, size_t numBones)
{
    for (auto i = 0u; i < numBones; i++)
    {
        auto boneNameSize = size_t{};
        auto boneName = std::string{};
        auto boneId = uint32_t{};
        bytes.Read(&boneNameSize);
        boneName.reserve(boneNameSize);

        for (auto j = 0u; j < boneNameSize; j++)
        {
            auto boneNameChar = char{};
            bytes.Read(&boneNameChar);
            boneName.push_back(boneNameChar);
        }

        bytes.Read(&boneId);
        boneNamesToIds->emplace(boneName, boneId);
    }
}

void Read(RawNcaBuffer& bytes, nc::asset::BodySpaceNode* currentNode, nc::asset::BodySpaceNode* parentNode)
{
    auto generation = size_t{};
    auto numChildren = size_t{};
    bytes.Read(&generation);
    bytes.Read(&currentNode->boneName);
    //ReadMatrix(&parentNode->localSpace);
    currentNode->parent = parentNode;
    bytes.Read(&numChildren);
    parentNode->children.reserve(numChildren);
    for (auto i = 0u; i < numChildren; i++)
    {
        parentNode->children.emplace_back();
        Read(bytes, &parentNode->children.back(), currentNode);
    }
}
}
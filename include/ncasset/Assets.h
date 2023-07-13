#pragma once

#include "ncmath/Geometry.h"
#include "DirectXMath.h"

#include <array>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

namespace nc::asset
{
struct AudioClip
{
    size_t samplesPerChannel;
    std::vector<double> leftChannel;
    std::vector<double> rightChannel;
};

struct BodySpaceNode
{
    std::string boneName;
    DirectX::XMMATRIX localSpace;
    uint32_t numChildren;
};

struct BonesData
{
    std::unordered_map<std::string, uint32_t> boneNamesToIds;
    std::vector<DirectX::XMMATRIX> boneTransforms;
    std::vector<BodySpaceNode> bodySpaceOffsets;
};

struct HullCollider
{
    Vector3 extents;
    float maxExtent;
    std::vector<Vector3> vertices;
};

struct ConcaveCollider
{
    Vector3 extents;
    float maxExtent;
    std::vector<Triangle> triangles;
};

struct MeshVertex
{
    Vector3 position = Vector3::Zero();
    Vector3 normal = Vector3::Zero();
    Vector2 uv = Vector2::Zero();
    Vector3 tangent = Vector3::Zero();
    Vector3 bitangent = Vector3::Zero();
    Vector4 boneWeights = Vector4::Zero();
    std::array<uint32_t, 4> boneIds = {0, 0, 0, 0};
};

struct Mesh
{
    Vector3 extents;
    float maxExtent;
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
    std::optional<BonesData> bonesData;
};

struct PerVertexBones
{
    std::array<float, 4> boneWeights;
    std::array<uint32_t, 4> boneIds;
    
    void Add(uint32_t id, float weight)
    {
        for (auto i = 0u; i < boneIds.size(); i++)
        {
            if (boneWeights[i] == 0.0)
            {
                boneIds[i] = id;
                boneWeights[i] = weight;
            }
        }
    }
};

struct Shader
{
};

struct Texture
{
    static constexpr uint32_t numChannels = 4u;

    uint32_t width;
    uint32_t height;
    std::vector<unsigned char> pixelData;
};

struct CubeMap
{
    static constexpr uint32_t numChannels = 4u;

    uint32_t faceSideLength;
    std::vector<unsigned char> pixelData;
};
} // namespace nc::asset

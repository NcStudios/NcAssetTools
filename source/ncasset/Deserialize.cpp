#include "Deserialize.h"
#include "RawNcaBuffer.h"
#include "ncasset/Assets.h"

#include "ncutility/NcError.h"
#include "fmt/format.h"

#include <istream>
#include <vector>

namespace
{
auto ReadNcaHeader(std::istream& stream, std::string_view expectedMagicNumber) -> nc::asset::NcaHeader
{
    auto bytes = nc::asset::RawNcaBuffer{stream, nc::asset::NcaHeader::binarySize};
    auto header = nc::asset::NcaHeader{};
    bytes.Read(&header.magicNumber, 4); // not null-terminated in file

    // Verify we're reading the expected type of data
    if (std::string_view{header.magicNumber} != expectedMagicNumber)
    {
        throw nc::NcError(fmt::format(
            "Magic number mismatch actual: '{}' expected: '{}' ",
             header.magicNumber, expectedMagicNumber)
        );
    }

    bytes.Read(&header.compressionAlgorithm, 4); // not null-terminated in file
    bytes.Read(&header.assetId);
    bytes.Read(&header.size);
    return header;
}
} // anonymous namespace

namespace nc::asset
{
auto DeserializeAudioClip(std::istream& stream) -> DeserializedResult<AudioClip>
{
    const auto header = ::ReadNcaHeader(stream, MagicNumber::audioClip);
    auto bytes = RawNcaBuffer{stream, header.size};
    auto asset = AudioClip{};
    bytes.Read(&asset.samplesPerChannel);
    asset.leftChannel.resize(asset.samplesPerChannel);
    bytes.Read(asset.leftChannel.data(), asset.samplesPerChannel * sizeof(double));
    asset.rightChannel.resize(asset.samplesPerChannel);
    bytes.Read(asset.rightChannel.data(), asset.samplesPerChannel * sizeof(double));

    if (bytes.RemainingByteCount() != 0)
    {
        throw NcError("Not all AudioClip data was read from RawNcaBuffer");
    }

    return {header, asset};
}

auto DeserializeConcaveCollider(std::istream& stream) -> DeserializedResult<ConcaveCollider>
{
    const auto header = ::ReadNcaHeader(stream, MagicNumber::concaveCollider);
    auto bytes = RawNcaBuffer{stream, header.size};
    auto asset = ConcaveCollider{};
    auto triCount = size_t{};
    bytes.Read(&asset.extents);
    bytes.Read(&asset.maxExtent);
    bytes.Read(&triCount);
    asset.triangles.resize(triCount);
    bytes.Read(asset.triangles.data(), triCount * sizeof(Triangle));

    if (bytes.RemainingByteCount() != 0)
    {
        throw NcError("Not all ConcaveCollider data was read from RawNcaBuffer");
    }

    return {header, asset};
}

auto DeserializeCubeMap(std::istream& stream) -> DeserializedResult<CubeMap>
{
    const auto header = ::ReadNcaHeader(stream, MagicNumber::cubeMap);
    auto bytes = RawNcaBuffer{stream, header.size};
    auto asset = CubeMap{};
    bytes.Read(&asset.faceSideLength);
    constexpr auto faceCount = 6ull;
    const auto nBytes = asset.faceSideLength * asset.faceSideLength * faceCount * asset::CubeMap::numChannels;
    asset.pixelData.resize(nBytes);
    bytes.Read(asset.pixelData.data(), nBytes);

    if (bytes.RemainingByteCount() != 0)
    {
        throw NcError("Not all CubeMap data was read from RawNcaBuffer");
    }

    return {header, asset};
}

auto DeserializeHullCollider(std::istream& stream) -> DeserializedResult<HullCollider>
{
    const auto header = ::ReadNcaHeader(stream, MagicNumber::hullCollider);
    auto bytes = RawNcaBuffer{stream, header.size};
    auto asset = HullCollider{};
    auto vertexCount = size_t{};
    bytes.Read(&asset.extents);
    bytes.Read(&asset.maxExtent);
    bytes.Read(&vertexCount);
    asset.vertices.resize(vertexCount);
    bytes.Read(asset.vertices.data(), vertexCount * sizeof(Vector3));

    if (bytes.RemainingByteCount() != 0)
    {
        throw NcError("Not all HullCollider data was read from RawNcaBuffer");
    }

    return {header, asset};
}

auto DeserializeMesh(std::istream& stream) -> DeserializedResult<Mesh>
{
    const auto header = ::ReadNcaHeader(stream, MagicNumber::mesh);
    auto bytes = RawNcaBuffer{stream, header.size};
    auto asset = Mesh{};
    auto vertexCount = size_t{};
    auto indexCount = size_t{};
    bytes.Read(&asset.extents);
    bytes.Read(&asset.maxExtent);
    bytes.Read(&vertexCount);
    bytes.Read(&indexCount);
    asset.vertices.resize(vertexCount);
    bytes.Read(asset.vertices.data(), vertexCount * sizeof(MeshVertex));
    asset.indices.resize(indexCount);
    bytes.Read(asset.indices.data(), indexCount * sizeof(uint32_t));

    if (bytes.RemainingByteCount() != 0)
    {
        throw NcError("Not all Mesh data was read from RawNcaBuffer");
    }

    return {header, asset};
}

auto DeserializeTexture(std::istream& stream) -> DeserializedResult<Texture>
{
    const auto header = ::ReadNcaHeader(stream, MagicNumber::texture);
    auto bytes = RawNcaBuffer{stream, header.size};
    auto asset = Texture{};
    bytes.Read(&asset.width);
    bytes.Read(&asset.height);
    const auto nBytes = asset.width * asset.height * asset::Texture::numChannels;
    asset.pixelData.resize(nBytes);
    bytes.Read(asset.pixelData.data(), nBytes);

    if (bytes.RemainingByteCount() != 0)
    {
        throw NcError("Not all Texture data was read from RawNcaBuffer");
    }

    return {header, asset};
}
} // namespace nc::asset
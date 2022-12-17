#include "gtest/gtest.h"
#include "CollateralGeometry.h"
#include "CollateralTexture.h"
#include "GeometryTestUtility.h"
#include "TextureTestUtility.h"

#include "ncasset/AssetTypes.h"
#include "ncasset/Import.h"
#include "ncconvert/builder/Builder.h"
#include "ncconvert/builder/Target.h"
#include "ncconvert/converters/GeometryConverter.h"
#include "ncconvert/converters/TextureConverter.h"

const auto ncaTestOutDirectory = std::filesystem::path{"./test_temp_dir"};

class NcConvertIntegration : public ::testing::Test
{
    public:
        static void SetUpTestSuite()
        {
            if (!std::filesystem::exists(ncaTestOutDirectory))
            {
                std::filesystem::create_directory(ncaTestOutDirectory);
            }
        }

        static void TearDownTestSuite()
        {
            if (std::filesystem::exists(ncaTestOutDirectory))
            {
                std::filesystem::remove_all(ncaTestOutDirectory);
            }
        }
};

TEST_F(NcConvertIntegration, Texture_from_png)
{
    namespace test_data = collateral::rgb_corners;
    const auto inFile = test_data::pngFilePath;
    const auto outFile = ncaTestOutDirectory / "rgb_png.nca";
    const auto target = nc::convert::Target(inFile, outFile);
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::Texture, target));

    const auto asset = nc::asset::ImportTexture(outFile);

    EXPECT_EQ(test_data::width, asset.width);
    EXPECT_EQ(test_data::height, asset.height);
    ASSERT_EQ(test_data::numBytes, asset.pixelData.size());

    for (auto pixelIndex = 0u; pixelIndex < test_data::numPixels; ++pixelIndex)
    {
        const auto expectedPixel = test_data::pixels[pixelIndex];
        const auto actualPixel = ReadPixel(asset.pixelData.data(), pixelIndex * 4);
        EXPECT_EQ(expectedPixel, actualPixel);
    }
}

TEST_F(NcConvertIntegration, ConcaveCollider_from_fbx)
{
    namespace test_data = collateral::plane_fbx;
    const auto inFile = test_data::filePath;
    const auto outFile = ncaTestOutDirectory / "plane_concave.nca";
    const auto target = nc::convert::Target(inFile, outFile);
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::ConcaveCollider, target));

    const auto asset = nc::asset::ImportConcaveCollider(outFile);

    EXPECT_EQ(asset.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(asset.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(asset.triangles.size(), test_data::triangleCount);

    for (const auto& tri : asset.triangles)
    {
        const auto pos = std::ranges::find(test_data::possibleTriangles, tri);
        EXPECT_NE(pos, test_data::possibleTriangles.cend());
    }
}

TEST_F(NcConvertIntegration, HullCollider_from_fbx)
{
    namespace test_data = collateral::cube_fbx;
    const auto inFile = test_data::filePath;
    const auto outFile = ncaTestOutDirectory / "cube_hull.nca";
    const auto target = nc::convert::Target(inFile, outFile);
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::HullCollider, target));

    const auto asset = nc::asset::ImportHullCollider(outFile);

    EXPECT_EQ(asset.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(asset.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(asset.vertices.size(), test_data::vertexCount);

    for (const auto& vertex : asset.vertices)
    {
        const auto pos = std::ranges::find(test_data::possibleVertices, vertex);
        EXPECT_NE(pos, test_data::possibleVertices.cend());
    }
}

TEST_F(NcConvertIntegration, Mesh_from_fbx)
{
    namespace test_data = collateral::cube_fbx;
    const auto inFile = test_data::filePath;
    const auto outFile = ncaTestOutDirectory / "cube_mesh.nca";
    const auto target = nc::convert::Target(inFile, outFile);
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::Mesh, target));

    const auto asset = nc::asset::ImportMesh(outFile);

    EXPECT_EQ(asset.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(asset.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(asset.vertices.size(), test_data::vertexCount);

    for (const auto& vertex : asset.vertices)
    {
        // TODO: uvs
        const auto foundVertex = std::ranges::find(test_data::possibleVertices, vertex.position);
        const auto foundNormal = std::ranges::find(test_data::possibleNormals, vertex.normal);
        const auto foundTangent = std::ranges::find(test_data::possibleTangents, vertex.tangent);
        const auto foundBitangent = std::ranges::find(test_data::possibleBitangents, vertex.bitangent);
        EXPECT_NE(foundVertex, test_data::possibleVertices.cend());
        EXPECT_NE(foundNormal, test_data::possibleNormals.cend());
        EXPECT_NE(foundTangent, test_data::possibleTangents.cend());
        EXPECT_NE(foundBitangent, test_data::possibleBitangents.cend());
    }

    // should have triangular faces
    EXPECT_EQ(asset.indices.size() % 3, 0);

    // just verifying all indices point to a valid vertex
    const auto nVertices = asset.vertices.size();
    EXPECT_TRUE(std::ranges::all_of(asset.indices, [&nVertices](auto i){ return i < nVertices; }));
}

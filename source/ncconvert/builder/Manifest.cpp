#include "Manifest.h"
#include "Target.h"
#include "utility/EnumExtensions.h"
#include "utility/Log.h"
#include "utility/Path.h"

#include "ncutility/NcError.h"
#include "nlohmann/json.hpp"

#include <fstream>

namespace
{
const auto jsonAssetArrayTags = std::array<std::string, 6> {
    "audio-clip", "concave-collider", "cube-map", "hull-collider", "mesh", "texture"
};

struct GlobalManifestOptions
{
    std::filesystem::path outputDirectory;
    std::filesystem::path workingDirectory;
};

void from_json(const nlohmann::json& json, GlobalManifestOptions& options)
{
    options.outputDirectory = json.value("outputDirectory", "./");
    options.workingDirectory = json.value("workingDirectory", "./");
}

void ProcessOptions(GlobalManifestOptions& options, const std::filesystem::path& manifestPath)
{
    options.outputDirectory.make_preferred();
    options.workingDirectory.make_preferred();
    auto&& parentPath = std::filesystem::absolute(manifestPath.parent_path());

    if (options.workingDirectory.is_relative())
    {
        options.workingDirectory = parentPath / options.workingDirectory;
    }

    if (options.outputDirectory.is_relative())
    {
        options.outputDirectory = parentPath / options.outputDirectory;
    }

    LOG("Setting working directory: {}", options.workingDirectory.string());
    std::filesystem::current_path(options.workingDirectory);

    if(!std::filesystem::exists(options.outputDirectory))
    {
        LOG("Creating directory: {}", options.outputDirectory.string());
        if(!std::filesystem::create_directories(options.outputDirectory))
        {
            throw nc::NcError("Failed to create output directory: ", options.outputDirectory.string());
        }
    }
}

auto BuildTarget(const nlohmann::json& json, nc::asset::AssetType type, const std::filesystem::path& outputDirectory) -> nc::convert::Target
{
    if (!nc::convert::CanOutputMany(type))
    {
        return nc::convert::Target{
            json.at("sourcePath"),
            nc::convert::AssetNameToNcaPath(json.at("assetName"), outputDirectory)
        };
    }
    return nc::convert::Target{
        json.at("sourcePath"),
        outputDirectory
    };
}

} // anonymous namespace

namespace nc::convert
{
void ReadManifest(const std::filesystem::path& manifestPath, std::unordered_map<asset::AssetType, std::vector<Target>>& instructions)
{
    auto file = std::ifstream{manifestPath};
    if (!file.is_open())
    {
        throw nc::NcError{"Failed to open manifest: ", manifestPath.string()};
    }

    auto json = nlohmann::json::parse(file);
    auto options = json.value("globalOptions", ::GlobalManifestOptions{});
    ::ProcessOptions(options, manifestPath);

    for (const auto& typeTag : ::jsonAssetArrayTags)
    {
        if (!json.contains(typeTag))
        {
            continue;
        }

        const auto type = ToAssetType(typeTag);
        for (const auto& asset : json.at(typeTag))
        {
            auto target = ::BuildTarget(asset, type, options.outputDirectory);
            if (!std::filesystem::is_regular_file(target.sourcePath))
            {
                throw nc::NcError("Invalid source file: ", target.sourcePath.string());
            }

            LOG("Adding build target: {} -> {}", target.sourcePath.string(), target.destinationPath.string());
            instructions.at(type).push_back(std::move(target));
        }
    }
}
} // namespace nc::convert

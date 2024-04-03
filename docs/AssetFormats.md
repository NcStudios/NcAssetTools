# Asset Formats
This document describes the data layouts for NcEngine assets and asset file types.

## Contents
- [Nc Asset](#nc-asset)
  - [.nca File Format](#nca-file-format)
- [Nc Asset Package](#nc-asset-package)
  - [.ncp File Format](#ncp-file-format)
  - [LUT Entry Format](#lut-entry-format)
- [Blob Formats](#blob-formats)
    - [AudioClip](#audioclip-blob-format)
    - [ConcaveCollider](#concavecollider-blob-format)
    - [Cubemap](#cubemap-blob-format)
    - [HullCollider](#hullcollider-blob-format)
    - [Mesh](#mesh-blob-format)
    - [Shader](#shader-blob-format)
    - [SkeletalAnimation](#skeletalanimation-blob-format)
    - [Texture](#texture-blob-format)

## Nc Asset
-----------
An asset can exist on its own in a .nca file, or as an entry in a .ncp package. The data layout is identical in either case. Each nca consists of a header that is common between all asset types and
an asset blob, the layout of which is determined by the asset type.

### .nca File Format
| Name         | Type    | Size         | Note |
|--------------|---------|--------------|------
| magic number | string  | 4            | non-null-terminated string identifying the asset type
| compression  | string  | 4            | NONE until supported
| version      | u64     | 8            | major version of nc-convert used to create the asset
| blob size    | u64     | 8            | size of the asset blob
| asset blob   | -       | blob size    | unique layout for each asset type

## Nc Asset Package
-------------------
An asset package is one or more assets bundled together. It consists of a header, lookup table, and one or more .nca files packaged together.

### .ncp File Format
| Name          | Type       | Size                | Note |
|---------------|------------|---------------------|------|
| magic number  | string     | 4                   | always NCPK                      |
| version       | string     | 8                   | semver XX.YY.ZZ                  |
| asset count   | u64        | 8                   | number of table/asset entries    |
| look up table | lutEntry[] | asset count * 24    | identifies assets in the package |
| assets        | nca[]      | -                   | list of nca files                |

### LUT Entry Format
| Name         | Type   | Size |
|--------------|--------|------|
| asset id     | u64    | 8    |
| offset       | u64    | 8    |
| last updated | time_t | 8    |

## Blob Formats
---------------
### AudioClip Blob Format
> Magic Number: 'CLIP'

| Name                | Type     | Size                    |
|---------------------|----------|-------------------------|
| samples per channel | u64      | 8                       |
| left channel        | double[] | 8 * samples per channel |
| right channel       | double[] | 8 * samples per channel |

### ConcaveCollider Blob Format
> Magic Number: 'CONC'

| Name           | Type       | Size                |
|----------------|------------|---------------------|
| extents        | Vector3    | 12                  |
| max extent     | float      | 4                   |
| triangle count | u64        | 8                   |
| triangles      | Triangle[] | triangle count * 36 |

### Cubemap Blob Format
> Magic Number: 'CUBE'

| Name             | Type            | Size                         | Note 
|------------------|-----------------|------------------------------|------
| face side length | u32             | 4                            |
| pixel data       | unsigned char[] | face side length ^ 2 * 4 * 6 | 6 faces with 4 8-bit channels packed together

CubeMap faces in pixel data array are ordered: front, back, up, down, right, left.

### HullCollider Blob Format
> Magic number: 'HULL'

| Name         | Type      | Size              |
|--------------|-----------|-------------------|
| extents      | Vector3   | 12                |
| max extent   | float     | 4                 |
| vertex count | u64       | 8                 |
| vertex list  | Vector3[] | vertex count * 12 |

### Mesh Blob Format
> Magic Number: 'MESH'

| Name                 | Type                                 | Size              | Note
|----------------------|--------------------------------------|-------------------|-------------
| extents              | Vector3                              | 12                |
| max extent           | float                                | 4                 |
| vertex count         | u64                                  | 8                 |
| index count          | u64                                  | 8                 |
| vertex list          | MeshVertex[]                         | vertex count * 88 |
| indices              | int[]                                | index count * 4   |
| bones data has value | bool                                 | 1                 |
| BonesData            | BonesData                            |                   | [BonesData](#bones-data-blob-format)

### Bones Data Blob Format

| Name                         | Type                                                | Size                                                    | Note
|------------------------------|-----------------------------------------------------|---------------------------------------------------------|-------------
| vertexSpaceToBoneSpace count | u64                                                 | 8                                                       |
| boneSpaceToParentSpace count | u64                                                 | 8                                                       |
| boneMapping                  | (u64 + string + u32) * vertexSpaceToBoneSpace count | (12 + sizeof(boneName)) * vertexSpaceToBoneSpace count  |
| vertexSpaceToBoneSpace       | VertexSpaceToBoneSpace[]                            | (72 + sizeof(boneName)) * vertexSpaceToBoneSpace count  |
| boneSpaceToParentSpace       | BoneSpaceToParentSpace[]                            | (136 + sizeof(boneName)) * vertexSpaceToBoneSpace count |

### Shader Blob Format
> Magic Number: 'SHAD'

TODO

### SkeletalAnimation Blob Format
> Magic Number: 'SKEL'

| Name                       | Type             | Size                      | Note 
|----------------------------|------------------|---------------------------|------
| name size                  | u64              | 8                         |
| name                       | string           | name.size()               |
| durationInTicks            | u32              | 4                         |
| ticksPerSecond             | float            | 4                         |
| framesPerBone count        | u64              | 8                         |
| framesPerBone list         | FramesPerBone[]  |                           | [FramesPerBone](#FramesPerBone-blob-format)

### FramesPerBone Blob Format

| Name                 | Type             | Size                      | Note 
|----------------------|------------------|---------------------------|------
| name size            | u64              | 8                         |
| name                 | string           | name.size()               |
| position frames size | u64              | 8                         |
| position frames      | PositionFrames[] | position frames size * 20 |
| rotation frames size | u64              | 8                         |
| rotation frames      | RotationFrames[] | rotation frames size * 24 |
| scale frames size    | u64              | 8                         |
| scale frames         | ScaleFrames[]    | scale frames size * 20    |

### Texture Blob Format
> Magic Number: 'TEXT'

| Name      | Type            | Size               | Note 
|-----------|-----------------|--------------------|------
| width     | u32             | 4                  |
| height    | u32             | 4                  |
| pixelData | unsigned char[] | width * height * 4 | Always forced to 4 8-bit channels

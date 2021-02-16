/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/core/geometry/triangle_mesh.h"
#include "saiga/core/geometry/vertex.h"
#include "saiga/core/util/Range.h"

#include <vector>


namespace Saiga
{
struct UnifiedMaterial
{
    std::string name;
    vec4 color_diffuse  = vec4(0, 1, 0, 0);
    vec4 color_ambient  = vec4(0, 1, 0, 0);
    vec4 color_specular = vec4(0, 1, 0, 0);
    vec4 color_emissive = vec4(0, 1, 0, 0);
    std::string texture_diffuse;
    std::string texture_normal;
    std::string texture_alpha;

    UnifiedMaterial() {}
    UnifiedMaterial(const std::string& name) : name(name) {}
};

struct UnifiedMaterialGroup
{
    int startFace  = 0;
    int numFaces   = 0;
    int materialId = -1;

    Range<int> range() const { return Range<int>(startFace, startFace + numFaces); }
};

class SAIGA_CORE_API UnifiedModel
{
   public:
    UnifiedModel() {}
    UnifiedModel(const std::string& file_name);

    int NumVertices() const { return position.size(); }
    int NumFaces() const { return triangles.size(); }

    // Vertex Data
    std::vector<vec3> position;
    std::vector<vec3> normal;
    std::vector<vec4> color;
    std::vector<vec2> texture_coordinates;
    std::vector<vec4> data;

    // Face data for surface meshes stored as index-face set
    std::vector<ivec3> triangles;

    // The material is given on a per face basis.
    // The material group defines which faces have which material.
    std::vector<UnifiedMaterial> materials;
    std::vector<UnifiedMaterialGroup> material_groups;


    // Transforms this model inplace
    // returns a reference to this
    UnifiedModel& transform(const mat4& T);

    // Overwrite the color of every vertex
    // returns a reference to this
    UnifiedModel& SetVertexColor(const vec4& color);

    AABB BoundingBox();


    std::vector<vec4> ComputeVertexColorFromMaterial() const;


    // Check status
    bool HasPosition() const { return !position.empty(); }
    bool HasNormal() const { return !normal.empty(); }
    bool HasColor() const { return !color.empty(); }
    bool HasTC() const { return !texture_coordinates.empty(); }
    bool HasData() const { return !data.empty(); }
    bool HasMaterials() const { return !materials.empty(); }


    // Conversion Functions from unified model -> Triangle mesh
    // The basic conversion functions for the saiga vertices are defined below,
    // however you can also define conversions for custom vertex types.
    template <typename VertexType, typename IndexType>
    TriangleMesh<VertexType, IndexType> Mesh() const;
};


}  // namespace Saiga


#include "UnifiedModel.hpp"

#include "img/shadergen.h"
#include <memory>
#include <vector>

namespace img {

/*enum class TypeKind {
        Primitive,
        Struct,
        Image
};

struct SStructField {
        std::string name;
        const SType* type;
};

struct SStructType {
        std::vector<std::unique_ptr<SStructField>> fields;
};

struct SType {
        TypeKind kind;
        union {
                PrimitiveType prim;
                SStructType* struct_;
        };
};

struct SDeclFun {
        std::string name;

};



struct ShaderGeneratorPrivate {

        struct SamplerUniform {
                int tex;
                gfx::SamplerDesc desc;
        };

        std::vector<TextureBinding> textures_;
        std::vector<SamplerUniform> samplers_;

        SType voidTy;
        SType floatTy;
        SType float2Ty;
        SType float3Ty;
        SType float4Ty;
        SType intTy;
        SType int2Ty;
        SType int3Ty;
        SType int4Ty;
        SType float4x4Ty;
};

ShaderGenerator::ShaderGenerator(gfx::ShaderStageFlags stage, Target target)
{

}

SType* ShaderGenerator::typeOf(SVal* val)
{
        return SType();
}

SType* ShaderGenerator::primitiveType(PrimitiveType primty)
{
        return SType();
}

SVal ShaderGenerator::addTextureResource(gfx::ImageDimensions dim)
{
        return SVal();
}
SVal ShaderGenerator::addGlobalParameter(util::StringRef name, SType ty, int
buffer)
{
        return SVal();
}*/
}
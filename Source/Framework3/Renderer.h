/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / ? ` \     * ---------------- *
* Author's *     \_  (__\      \_?v?_/     * humus@rogers.com *
*   note   *     //   \\       //   \\     * ICQ #47010716    *
* -------- *    ((     ))     ((     ))    * ---------------- *
*          ****--""---""-------""---""--****                  ********\
* This file is a part of the work done by Humus. You are free to use  *
* the code in any way you like, modified, unmodified or copy'n'pasted *
* into your own work. However, I expect you to respect these points:  *
*  @ If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  @ For use in anything commercial, please request my approval.      *
*  @ Share your work and ideas too as much as you can.                *
\*********************************************************************/

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Platform.h"
#include "Util/Array.h"
#include "Math/Vector.h"
#include "Imaging/Image.h"

#include <stdio.h>

typedef int TextureID;
typedef int ShaderID;
typedef int VertexBufferID;
typedef int IndexBufferID;
typedef int VertexFormatID;
typedef int SamplerStateID;
typedef int BlendStateID;
typedef int DepthStateID;
typedef int RasterizerStateID;
typedef int FontID;

struct Texture;
struct Shader;
struct VertexBuffer;
struct IndexBuffer;
struct VertexFormat;
struct SamplerState;
struct BlendState;
struct DepthState;
struct RasterizerState;

enum ConstantType {
	CONSTANT_FLOAT,
	CONSTANT_VEC2,
	CONSTANT_VEC3,
	CONSTANT_VEC4,
	CONSTANT_INT,
	CONSTANT_IVEC2,
	CONSTANT_IVEC3,
	CONSTANT_IVEC4,
	CONSTANT_BOOL,
	CONSTANT_BVEC2,
	CONSTANT_BVEC3,
	CONSTANT_BVEC4,
	CONSTANT_MAT2,
	CONSTANT_MAT3,
	CONSTANT_MAT4,

	CONSTANT_TYPE_COUNT
};
extern int constantTypeSizes[];

enum Filter {
	NEAREST,
	LINEAR,
	BILINEAR,
	TRILINEAR,
	BILINEAR_ANISO,
	TRILINEAR_ANISO,
};

enum AddressMode {
	WRAP,
	CLAMP,
	CLAMP_TO_BORDER,
};

inline bool hasMipmaps(const Filter filter){ return (filter >= BILINEAR); }
inline bool hasAniso(const Filter filter){ return (filter >= BILINEAR_ANISO); }

struct Character {
	float x0, y0;
	float x1, y1;
	float ratio;
};

struct TexFont {
	Character chars[256];
	TextureID texture;
};

struct TexVertex {
	TexVertex(const vec2 p, const vec2 t){
		position = p;
		texCoord = t;
	}
	vec2 position;
	vec2 texCoord;
};

#define MAKEQUAD(x0, y0, x1, y1, o)\
	vec2(x0 + o, y0 + o),\
	vec2(x0 + o, y1 - o),\
	vec2(x1 - o, y0 + o),\
	vec2(x1 - o, y1 - o),

#define MAKETEXQUAD(x0, y0, x1, y1, o)\
	TexVertex(vec2(x0 + o, y0 + o), vec2(0, 0)),\
	TexVertex(vec2(x0 + o, y1 - o), vec2(0, 1)),\
	TexVertex(vec2(x1 - o, y0 + o), vec2(1, 0)),\
	TexVertex(vec2(x1 - o, y1 - o), vec2(1, 1)),

#define MAKERECT(x0, y0, x1, y1, lw)\
	vec2(x0, y0),\
	vec2(x0 + lw, y0 + lw),\
	vec2(x1, y0),\
	vec2(x1 - lw, y0 + lw),\
	vec2(x1, y1),\
	vec2(x1 - lw, y1 - lw),\
	vec2(x0, y1),\
	vec2(x0 + lw, y1 - lw),\
	vec2(x0, y0),\
	vec2(x0 + lw, y0 + lw),


#define TEXTURE_NONE  (-1)
#define SHADER_NONE   (-1)
#define BLENDING_NONE (-1)
#define VF_NONE   (-1)
#define VB_NONE   (-1)
#define IB_NONE   (-1)
#define SS_NONE   (-1)
#define BS_NONE   (-1)
#define DS_NONE   (-1)
#define RS_NONE   (-1)
#define FONT_NONE (-1)

#define FB_COLOR (-2)
#define FB_DEPTH (-2)

#define DONTCARE (-2)

// Texture flags
#define CUBEMAP      0x1
#define HALF_FLOAT   0x2
#define SRGB         0x4
#define SAMPLE_DEPTH 0x8

// Shader flags
#define ASSEMBLY 0x1
#define ALLOW_FAILURE 0x2

// Mask constants
#define RED   0x1
#define GREEN 0x2
#define BLUE  0x4
#define ALPHA 0x8

#define ALL (RED | GREEN | BLUE | ALPHA)
#define NONE 0


// reset() flags
#define RESET_ALL    0xFFFF
#define RESET_SHADER 0x1
#define RESET_VF     0x2
#define RESET_VB     0x4
#define RESET_IB     0x8
#define RESET_DS     0x10
#define RESET_BS     0x20
#define RESET_RS     0x40
#define RESET_SS     0x80
#define RESET_TEX    0x100

enum BufferAccess {
	STATIC,
	DEFAULT,
	DYNAMIC,
};

enum Primitives {
	PRIM_TRIANGLES      = 0,
	PRIM_TRIANGLE_FAN   = 1,
	PRIM_TRIANGLE_STRIP = 2,
	PRIM_QUADS          = 3,
	PRIM_LINES          = 4,
	PRIM_LINE_STRIP     = 5,
	PRIM_LINE_LOOP      = 6,
	PRIM_POINTS         = 7,
};

enum AttributeType {
	TYPE_GENERIC  = 0,
	TYPE_VERTEX   = 1,
	TYPE_TEXCOORD = 2,
	TYPE_NORMAL   = 3,
	TYPE_TANGENT  = 4,
	TYPE_BINORMAL = 5,
};

enum AttributeFormat {
	FORMAT_FLOAT = 0,
	FORMAT_HALF  = 1,
	FORMAT_UBYTE = 2,
};

struct FormatDesc {
	int stream;
	AttributeType type;
	AttributeFormat format;
	int size;
};

#define MAX_MRTS 8
#define MAX_VERTEXSTREAM 8
#define MAX_TEXTUREUNIT  16
#define MAX_SAMPLERSTATE 16

// Blending constants
extern const int ZERO;
extern const int ONE;
extern const int SRC_COLOR;
extern const int ONE_MINUS_SRC_COLOR;
extern const int DST_COLOR;
extern const int ONE_MINUS_DST_COLOR;
extern const int SRC_ALPHA;
extern const int ONE_MINUS_SRC_ALPHA;
extern const int DST_ALPHA;
extern const int ONE_MINUS_DST_ALPHA;
extern const int SRC_ALPHA_SATURATE;

extern const int BM_ADD;
extern const int BM_SUBTRACT;
extern const int BM_REVERSE_SUBTRACT;
extern const int BM_MIN;
extern const int BM_MAX;

// Depth testing constants
extern const int NEVER;
extern const int LESS;
extern const int EQUAL;
extern const int LEQUAL;
extern const int GREATER;
extern const int NOTEQUAL;
extern const int GEQUAL;
extern const int ALWAYS;

// Culling constants
extern const int CULL_NONE;
extern const int CULL_BACK;
extern const int CULL_FRONT;

// Fillmode constants
extern const int SOLID;
extern const int WIREFRAME;


class Renderer {
public:
	Renderer();
	virtual ~Renderer();

	virtual void resetToDefaults();
	virtual void reset(const uint flags = RESET_ALL);
	void apply();

	TextureID addTexture(const char *fileName, const bool useMipMaps, const SamplerStateID samplerState = SS_NONE, uint flags = 0);
	virtual TextureID addTexture(Image &img, const SamplerStateID samplerState = SS_NONE, uint flags = 0) = 0;
	TextureID addCubemap(const char **fileNames, const bool useMipMaps, const SamplerStateID samplerState = SS_NONE, uint flags = 0);
	TextureID addNormalMap(const char *fileName, const FORMAT destFormat, const bool useMipMaps, const SamplerStateID samplerState = SS_NONE, float sZ = 1.0f, float mipMapScaleZ = 2.0f, uint flags = 0);

	TextureID addRenderTarget(const int width, const int height, const FORMAT format, const SamplerStateID samplerState = SS_NONE, uint flags = 0){
		return addRenderTarget(width, height, 1, 1, format, 1, samplerState, flags);
	}
	virtual TextureID addRenderTarget(const int width, const int height, const int depth, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, uint flags = 0) = 0;
	TextureID addRenderDepth(const int width, const int height, const int depthBits){
		return addRenderDepth(width, height, 1, depthBits <= 16? FORMAT_DEPTH16 : FORMAT_DEPTH24);
	}
	virtual TextureID addRenderDepth(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, uint flags = 0) = 0;

	virtual bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int arraySize) = 0;

	virtual void removeTexture(const TextureID texture) = 0;

	virtual unsigned int CreateUBO(ShaderID shader, const char* uboName) const = 0;
	virtual void UpdateUBO(uint ubo, const void* data, size_t size) const = 0;
	ShaderID addShader(const char *fileName, const uint flags = 0);
	ShaderID addShader(const char *fileName, const char *extra, const uint flags = 0);
	ShaderID addShader(const char *fileName, const char **attributeNames, const int nAttributes, const char *extra = NULL, const uint flags = 0);
	virtual ShaderID addShader(const char *vsText, const char *gsText, const char *fsText, const int vsLine, const int gsLine, const int fsLine,
		const char *header = NULL, const char *extra = NULL, const char *fileName = NULL, const char **attributeNames = NULL, const int nAttributes = 0, const uint flags = 0) = 0;

	int getFormatSize(const AttributeFormat format) const;
	virtual VertexFormatID addVertexFormat(const FormatDesc *formatDesc, const uint nAttribs, const ShaderID shader = SHADER_NONE) = 0;
	virtual void updateBuffer(VertexBufferID vb, const void* data, size_t size) = 0;
	virtual VertexBufferID addVertexBuffer(const long size, const BufferAccess bufferAccess, const void *data = NULL) = 0;
	virtual IndexBufferID addIndexBuffer(const uint nIndices, const uint indexSize, const BufferAccess bufferAccess, const void *data = NULL) = 0;

	virtual SamplerStateID addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod = 0) = 0;

  virtual BlendStateID addBlendState(const int srcFactor, const int destFactor, const int blendMode = BM_ADD, const int mask = ALL) = 0;
	virtual BlendStateID addBlendStateSeperate(const int rgbSrcFactor, const int rgbDestFactor, const int alphaSrcFactor, const int alphaDestFactor, const int blendMode = BM_ADD, const int mask = ALL) = 0;

  virtual DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc = LEQUAL) = 0;
	virtual RasterizerStateID addRasterizerState(const int cullMode, const int fillMode = SOLID, const bool multiSample = true, const bool scissor = false) = 0;

	FontID addFont(const char *textureFile, const char *fontFile, const SamplerStateID samplerState);


/*	void setTexture(const uint unit, const TextureID texture){
		selectedTextures[unit] = texture;
	}
	void setTexture(const char *textureName, const TextureID texture){
		int unit = getTextureUnit(selectedShader, textureName);
		if (unit >= 0) selectedTextures[unit] = texture;
	}
	void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState){
		int unit = getTextureUnit(selectedShader, textureName);
		if (unit >= 0){
			selectedTextures[unit] = texture;
			selectedSamplerStates[unit] = samplerState;
		}
	}
*/

//	virtual int getTextureUnit(const ShaderID shader, const char *textureName) const = 0;
//	virtual int getSamplerUnit(const ShaderID shader, const char *samplerName) const = 0;

	virtual void setTexture(const char *textureName, const TextureID texture) = 0;
	virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState) = 0;

/*
	virtual void changeTexture(const uint imageUnit, const TextureID texture) = 0;
	void changeTexture(const char *textureName, const TextureID texture){
		int unit = getTextureUnit(currentShader, textureName);
		if (unit >= 0) changeTexture(unit, texture);
	}
*/
	virtual void applyTextures() = 0;

	virtual void setSamplerState(const char *samplerName, const SamplerStateID samplerState) = 0;
	virtual void applySamplerStates() = 0;



	void setShader(const ShaderID shader){
		selectedShader = shader;
	}
	void setVertexFormat(const VertexFormatID vertexFormat){
		selectedVertexFormat = vertexFormat;
	}
	void setVertexBuffer(const int stream, const VertexBufferID vertexBuffer, const intptr offset = 0){
		selectedVertexBuffers[stream] = vertexBuffer;
		selectedOffsets[stream] = offset;
	}
	void setVertexBuffer(const int stream, const void *base){
		selectedVertexBuffers[stream] = VB_NONE;
		selectedOffsets[stream] = (intptr) base;
	}
	void setIndexBuffer(const IndexBufferID indexBuffer){
		selectedIndexBuffer = indexBuffer;
	}

/*	void setSamplerState(const uint samplerUnit, const SamplerStateID samplerState){
		selectedSamplerStates[samplerUnit] = samplerState;		
	}*/
	void setBlendState(const BlendStateID blendState){
		selectedBlendState = blendState;
	}
	void setDepthState(const DepthStateID depthState){
		selectedDepthState = depthState;
	}
	void setRasterizerState(const RasterizerStateID rasterizerState){
		selectedRasterizerState = rasterizerState;
	}


	void setShaderConstant1i(const char *name, const int constant);
	void setShaderConstant1f(const char *name, const float constant);
	void setShaderConstant2f(const char *name, const vec2 &constant);
	void setShaderConstant3f(const char *name, const vec3 &constant);
	void setShaderConstant4f(const char *name, const vec4 &constant);
	void setShaderConstant4x4f(const char *name, const mat4 &constant);
	void setShaderConstantArray1f(const char *name, const float *constant, const uint count);
	void setShaderConstantArray2f(const char *name, const vec2  *constant, const uint count);
	void setShaderConstantArray3f(const char *name, const vec3  *constant, const uint count);
	void setShaderConstantArray4f(const char *name, const vec4  *constant, const uint count);

	virtual void setShaderConstantRaw(const char *name, const void *data, const int size) = 0;
	virtual void applyConstants() = 0;




	void changeRenderTarget(const TextureID colorRT, const TextureID depthRT = TEXTURE_NONE, const int face = 0){
		changeRenderTargets(&colorRT, 1, depthRT, &face);
	}
	virtual void changeRenderTargets(const TextureID *colorRTs, const uint nRenderTargets, const TextureID depthRT = TEXTURE_NONE, const int *faces = NULL) = 0;
	virtual void changeToMainFramebuffer() = 0;
	virtual void changeShader(const ShaderID shader) = 0;
	virtual void changeVertexFormat(const VertexFormatID vertexFormat) = 0;
	virtual void changeVertexBuffer(const int stream, const VertexBufferID vertexBuffer, const intptr offset = 0) = 0;
	void changeVertexBuffer(const void *base){
		changeVertexBuffer(0, VB_NONE, (intptr) base);
	}
	virtual void changeIndexBuffer(const IndexBufferID indexBuffer) = 0;

//	virtual void changeSamplerState(const uint samplerUnit, const SamplerStateID samplerState) = 0;
	virtual void changeBlendState(const BlendStateID blendState) = 0;
	virtual void changeDepthState(const DepthStateID depthState) = 0;
	virtual void changeRasterizerState(const RasterizerStateID rasterizerState) = 0;
/*
	virtual void changeShaderConstant1i(const char *name, const int constant) = 0;
	virtual void changeShaderConstant1f(const char *name, const float constant) = 0;
	virtual void changeShaderConstant2f(const char *name, const vec2 &constant) = 0;
	virtual void changeShaderConstant3f(const char *name, const vec3 &constant) = 0;
	virtual void changeShaderConstant4f(const char *name, const vec4 &constant) = 0;
	virtual void changeShaderConstant3x3f(const char *name, const mat3 &constant) = 0;
	virtual void changeShaderConstant4x4f(const char *name, const mat4 &constant) = 0;
	virtual void changeShaderConstantArray1f(const char *name, const float *constant, const uint count) = 0;
	virtual void changeShaderConstantArray2f(const char *name, const vec2 *constant, const uint count) = 0;
	virtual void changeShaderConstantArray3f(const char *name, const vec3 *constant, const uint count) = 0;
	virtual void changeShaderConstantArray4f(const char *name, const vec4 *constant, const uint count) = 0;
*/

	virtual void clear(const bool clearColor, const bool clearDepth, const float *color = NULL, const float depth = 1.0f) = 0;

	virtual void drawArrays(const Primitives primitives, const int firstVertex, const int nVertices) = 0;
	virtual void drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices) = 0;

	virtual void setup2DMode(const float left, const float right, const float top, const float bottom) = 0;
	virtual void drawPlain(const Primitives primitives, vec2 *vertices, const uint nVertices, const BlendStateID blendState, const DepthStateID depthState, const vec4 *color = NULL) = 0;
	virtual void drawTextured(const Primitives primitives, TexVertex *vertices, const uint nVertices, const TextureID texture, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const vec4 *color = NULL) = 0;

	float getTextWidth(const FontID font, const char *str, int length = -1) const;
	uint getTextQuads(const char *str) const;
	void fillTextBuffer(TexVertex *dest, const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font) const;
	bool drawText(const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState);

	void setViewport(const int width, const int height);

	// Statistics
	void resetStatistics();
	void addDrawCalls(const uint nCalls){ nDrawCalls += nCalls; }
	uint getDrawCallCount(){ return nDrawCalls; }

protected:
	Array <Texture> textures;
	Array <Shader> shaders;
	Array <VertexBuffer> vertexBuffers;
	Array <IndexBuffer> indexBuffers;
	Array <TexFont> fonts;
	Array <VertexFormat> vertexFormats;
	Array <SamplerState> samplerStates;
	Array <BlendState> blendStates;
	Array <DepthState> depthStates;
	Array <RasterizerState> rasterizerStates;

	uint nImageUnits, nMRTs;
	int maxAnisotropic;

//	float *textureLod;

	ShaderID currentShader, selectedShader;

	VertexFormatID currentVertexFormat, selectedVertexFormat;
	VertexBufferID currentVertexBuffers[MAX_VERTEXSTREAM], selectedVertexBuffers[MAX_VERTEXSTREAM];
	intptr selectedOffsets[MAX_VERTEXSTREAM], currentOffsets[MAX_VERTEXSTREAM];
	IndexBufferID currentIndexBuffer, selectedIndexBuffer;

	DepthStateID currentDepthState, selectedDepthState;
	BlendStateID currentBlendState, selectedBlendState;
	RasterizerStateID currentRasterizerState, selectedRasterizerState;

	TextureID currentColorRT[MAX_MRTS], currentDepthRT;
	int currentColorRTFace[MAX_MRTS];
	uint nCurrentRenderTargets;

	int viewportWidth, viewportHeight;

	// Statistics counters
	uint nDrawCalls;
private:
	TexVertex *fontBuffer;
	uint fontBufferCount;
#ifdef DEBUG
	bool wasReset;
#endif
};

#endif // _RENDERER_H_

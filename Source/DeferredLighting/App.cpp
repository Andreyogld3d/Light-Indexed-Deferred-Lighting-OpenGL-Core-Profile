/* ============================================================================
  Light Indexed Deferred Rendering Demo
  By Damian Trebilco
 
  Origional base lighting demo by "Humus"  
============================================================================ */

/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / ´ ` \     * ---------------- *
* Author's *     \_  (__\      \_°v°_/     * humus@rogers.com *
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

#include "App.h"
#include <cassert>
#include <algorithm>

//#define NO_LIGHT_VIEW_SPACE
//#define DRAW_PLANE_ONLY
#define USE_UBO
//#define USE_D3D_STYLE

BaseApp *app = new App();

#define SUBDIV_LEVEL 3
#define POW4(x) (1 << (2 * (x)))
#define SPHERE_SIZE (8 * 3 * POW4(SUBDIV_LEVEL))

enum LightCountPerFragment
{ 
  LCPF_One   = 0,  // Max of 1 light per fragment supported
  LCPF_Two   = 1,  // Max of 2 lights per fragment supported
  LCPF_Three = 2,  // Max of 3 lights per fragment supported
  LCPF_Four  = 3   // Max of 4 lights per fragment supported
};

LightData App::lightDataArray[MAX_LIGHT_TOTAL] = {
#include "LightPositions.h"
};

///////////////////////////////////////////////////////////////////////////////
//
LightData::LightData(const vec3& setColor, const vec3& setPosition, float setSize):
  isEnabled(false),
  screenX(0),
  screenY(0),
  screenWidth(0),
  screenHeight(0),
  color(setColor),
  position(setPosition),
  size(setSize)
{
}

///////////////////////////////////////////////////////////////////////////////
//
App::App():
  bitMaskLightColors(TEXTURE_NONE),
  bitMaskLightPos   (TEXTURE_NONE),
  staticLightSceneSet(false)
{
  lightDataArray[0].color = vec3(1, 0.7f, 0.2f);
  lightDataArray[1].color = vec3(0.8f, 1, 0.9f);
  lightDataArray[2].color = vec3(1, 0.2f, 0.1f);

  lightDataArray[0].size = 0.0f;
  lightDataArray[1].size = 0.0f;
  lightDataArray[2].size = 0.0f;

}

///////////////////////////////////////////////////////////////////////////////
//
void subDivide(vec3 *&dest, const vec3 &v0, const vec3 &v1, const vec3 &v2, int level){
	if (level){
		vec3 v3 = normalize(v0 + v1);
		vec3 v4 = normalize(v1 + v2);
		vec3 v5 = normalize(v2 + v0);

		subDivide(dest, v0, v3, v5, level - 1);
		subDivide(dest, v3, v4, v5, level - 1);
		subDivide(dest, v3, v1, v4, level - 1);
		subDivide(dest, v5, v4, v2, level - 1);
	} else {
		*dest++ = v0;
		*dest++ = v1;
		*dest++ = v2;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
void App::createSphereModel(){
#if 1
    bool optimized = true;
    const uint numRings = 250;
    const uint numSectors = 20;
    //const uint numRings = 150;
    //const uint numSectors = 15;
    const float Radius = 1.0f;
    const size_t vertexFormatSize = sizeof(Vector3D);
    uint numTriangles = (numRings + 1) * numSectors * 2;
    //
    uint numVertex = (numRings + 1) * numSectors + 2;

    const uint size = numVertex;
    const uint numIndices = numTriangles * 3;
    //
    const uint indicesSize = numIndices * sizeof(ushort);
    size_t vertexSize = numVertex * vertexFormatSize;
    size_t totalSize = vertexSize + indicesSize;
    // last index of last vertex
    uint last = numVertex - 1; // Soutch 
    const Vector3D firstPosition = Vector3D(0.0f, Radius, 0.0f);
    const Vector3D lastPosition = Vector3D(0.0f, -Radius, 0.0f);

    ubyte* data = reinterpret_cast<ubyte*>(malloc(totalSize));
    memset(data, 0, totalSize);
    ubyte* sphere = data;
    Vector3D& pos = *reinterpret_cast<Vector3D*>(data);

    // North
    pos = firstPosition;
    Vector3D& lastPos = reinterpret_cast<Vector3D*>(data)[last];
    lastPos = lastPosition;
#ifndef PI
    const float PI = static_cast<float>(Pi);
#endif
    //
    float da = PI / (numRings + 2.0f);
    float db = 2.0f * PI / numSectors;
    float af = PI - da / 2.0f;
    float bf = 2.0f * PI - db / 2.0f;
    //
    uint n = 1;
    uint color = 0xffffffff;
    //
    for (float a = da; a < af; a += da) {
        // 
        float y = Radius * cosf(a);
        // 
        float xz = Radius * sinf(a);
        //
        float rRadius = 1.0f / Radius;
        // 
        for (float b = 0.0; b < bf; n++, b += db) {
            //
            float x = xz * sinf(b);
            float z = xz * cosf(b);
            size_t Index = n * vertexFormatSize;
            Vector3D& position = *reinterpret_cast<Vector3D*>(&data[Index]);
            //assert(Index + vertexFormatSize < bufferData.size() && "Out Of Range");
            position = Vector3D(x, y, z);
        }
    }
#if 0
    uint* indices = NULL;
#else
    //
    struct Face {
        ushort i1;
        ushort i2;
        ushort i3;
        //uint i1;
        //uint i2;
        //uint i3;
    };

    Face* t = reinterpret_cast<Face*>(&data[vertexSize]);
    memset(t, 0, indicesSize);
    //Array<uint> Indices(numIndices);
    uint* indices = new uint[numIndices];
    const ushort* ushortIndices = &t->i1;
    // num sectors
    for (n = 0; n < numSectors; n += 3) {
        //
        t[n].i1 = 0;
        //
        t[n].i2 = n + 1;
        //
        t[n].i3 = n == numSectors - 1 ? 1 : n + 2;
        //
        t[numTriangles - numSectors + n].i1 = numVertex - 1;
        t[numTriangles - numSectors + n].i2 = numVertex - 2 - n;
        t[numTriangles - numSectors + n].i3 = numVertex - 2 - ((1 + n) % numSectors);
    }
    //
    int k = 1;
    // numSectors
    //S' n = numSectors;
    for (uint i = 0; i < numRings; i++, k += numSectors) {
        for (uint j = 0; j < numSectors; j++, n += 2) {
            //
            t[n].i1 = k + j;
            //
            t[n].i2 = k + numSectors + j;
            //
            t[n].i3 = k + numSectors + ((j + 1) % numSectors);
            //
            t[n + 1].i1 = t[n].i1;
            t[n + 1].i2 = t[n].i3;
            t[n + 1].i3 = k + ((j + 1) % numSectors);
        }
    }
    std::copy(ushortIndices, ushortIndices + numIndices, indices);
#endif
    sphereModel = new Model();

    sphereModel->addStream(TYPE_VERTEX, 3, size, (float*)sphere, indices, optimized);
    sphereModel->setIndexCount(numIndices);
    sphereModel->addBatch(0, numIndices);

#else

  // Do not like this - allocating an array of vec3's but Model class just calls delete, not delete [] (or for the right type)
	vec3 *sphere = new vec3[SPHERE_SIZE];
	vec3 *dest = sphere;

	subDivide(dest, vec3(0, 1,0), vec3( 0,0, 1), vec3( 1,0, 0), SUBDIV_LEVEL);
	subDivide(dest, vec3(0, 1,0), vec3( 1,0, 0), vec3( 0,0,-1), SUBDIV_LEVEL);
	subDivide(dest, vec3(0, 1,0), vec3( 0,0,-1), vec3(-1,0, 0), SUBDIV_LEVEL);
	subDivide(dest, vec3(0, 1,0), vec3(-1,0, 0), vec3( 0,0, 1), SUBDIV_LEVEL);

	subDivide(dest, vec3(0,-1,0), vec3( 1,0, 0), vec3( 0,0, 1), SUBDIV_LEVEL);
	subDivide(dest, vec3(0,-1,0), vec3( 0,0, 1), vec3(-1,0, 0), SUBDIV_LEVEL);
	subDivide(dest, vec3(0,-1,0), vec3(-1,0, 0), vec3( 0,0,-1), SUBDIV_LEVEL);
	subDivide(dest, vec3(0,-1,0), vec3( 0,0,-1), vec3( 1,0, 0), SUBDIV_LEVEL);

	sphereModel = new Model();

	sphereModel->addStream(TYPE_VERTEX, 3, SPHERE_SIZE, (float*)sphere, NULL, false);
  sphereModel->setIndexCount(SPHERE_SIZE);
  sphereModel->addBatch(0, SPHERE_SIZE);

  sphereModel->cleanUp();
#endif
  sphereModel->save("sphere.bin");
  sphereModel->saveBinary("sphere.lev");
}

///////////////////////////////////////////////////////////////////////////////
//
void App::resetCamera(){
	camPos = vec3(-557.0f, 135.0f, 5.8f);
	wx = 0.0634f;
  wy = -1.58f;
}

///////////////////////////////////////////////////////////////////////////////
//
void App::moveCamera(const vec3 &dir){
  vec3 newPos = camPos + dir * (speed * frameTime);

  vec3 point;
  const BTri *tri;
  if (bsp.intersects(camPos, newPos, &point, &tri)){
    newPos = point + tri->plane.xyz();
  }
  bsp.pushSphere(newPos, 30);

  camPos = newPos;
}

void App::InitLightSources()
{
	const vec3 off(5.0f, 5.0f, 0.0f);
	const vec3 Xdir = off;
	const vec3 Zdir = -off;
	for (size_t i = 0; i < _countof(lights); ++i) {
		Light& light = lights[i];
		const LightData& lightData = lightDataArray[i];
		light.posRange.x = lightData.position.x;
		light.posRange.x = lightData.position.y;
		light.posRange.y = lightData.position.z;
		light.posRange.w = 1.0f;
		light.lightColor.x = lightData.color.x;
		light.lightColor.y = lightData.color.y;
		light.lightColor.z = lightData.color.z;
		light.lightColor.w = 1.0f;

		if (i % 2) {
			dirPosOffsets[i] = Xdir;
		}
		else {
			dirPosOffsets[i] = Zdir;
		}
	}
}

void App::FillLightIndices()
{
	for (int lightIndex = MAX_LIGHT_TOTAL - 1; lightIndex >= 0; --lightIndex) {
		vec4& OutColor = lightIndices[lightIndex];
		// Set the light index color 
		ubyte convertColor = static_cast<ubyte>(lightIndex);
		ubyte redBit = (convertColor & (0x3 << 0)) << 6;
		ubyte greenBit = (convertColor & (0x3 << 2)) << 4;
		ubyte blueBit = (convertColor & (0x3 << 4)) << 2;
		ubyte alphaBit = (convertColor & (0x3 << 6)) << 0;
		OutColor = vec4(redBit, greenBit, blueBit, alphaBit);
		const float divisor = 255.0f;
		OutColor /= divisor;
	}

}

void App::CreatePlane(MeshData& meshData, uint width, uint height, uint stepX, uint stepZ, const Vector3D& normal, float d)
{
	assert(width && "Invalid Value");
	assert(height && "Invalid Value");
	//
	float tu = 1.0f / width;
	float tv = 1.0f / height;
	assert(stepX && "Invalid Value");
	assert(stepZ && "Invalid Value");

	assert(!(width % stepX) && "Invalid Value");
	assert(!(height % stepZ) && "Invalid Value");

	uint StepWidth = width / stepX + 1;
	uint StepHeight = height / stepZ + 1;
	// total vertexs
	uint numVertexs = StepWidth * StepHeight;
	// size of vertex data
	uint VertexDataSize = sizeof(Vertex) * numVertexs;
	uint numPolygons = 2 * (StepWidth - 1) * (StepHeight - 1);
	meshData.numFaces = numPolygons;
	uint NumIndices = numPolygons * 3;
	// size of index data
	uint IndexDataSize = NumIndices * sizeof(short);
	Array<ubyte> bufferData;
	size_t totalSize = IndexDataSize + VertexDataSize;
	bufferData.setCount(totalSize);
	ubyte* data = bufferData.getArray();
	Vertex* vertexs = reinterpret_cast<Vertex*>(data);
	memset(vertexs, 0, VertexDataSize);
	ushort* indices = reinterpret_cast<ushort*>(data + VertexDataSize);
	assert(vertexs && "NULL Pointer");
	assert(indices && "NULL Pointer");
	uint vertexIndex = 0;
	uint numIndices = 0;
	// Fill Vertex and indices
	int halfHeight = height >> 1;
	int halfWidth = width >> 1;
	for (int i = -halfHeight; i <= halfHeight; i += stepX) {
		for (int j = -halfWidth; j <= halfWidth; j += stepZ) {
			assert(vertexIndex < numVertexs && "Out Of Range");
			Vertex& v = vertexs[vertexIndex];
			v.position = Vector3D(static_cast<float>(i), 0.0f, static_cast<float>(j));
			v.tvert.x = tv * (i + halfHeight);
			v.tvert.y = tu * (j + halfWidth);
			vertexIndex++;
		}
	}
	uint vertexHOffset = 0;
	uint kOffset = height / stepZ + 1;
	vertexIndex = 0;
	//uint Step = (stepZ % 2) ? 0 : 1;
	for (uint i = 0; i < height; i += stepZ) {
		vertexIndex = vertexHOffset;
		for (uint j = 0; j < width; j += stepX) {

			assert(numIndices < NumIndices && "Out Of Range");
			indices[numIndices] = vertexIndex;

			assert(numIndices + 1 < NumIndices && "Out Of Range");
			indices[numIndices + 1] = vertexIndex + 1;

			assert(numIndices + 2 < NumIndices && "Out Of Range");
			indices[numIndices + 2] = vertexIndex + StepHeight + 1;

			assert(numIndices + 3 < NumIndices && "Out Of Range");
			indices[numIndices + 3] = vertexIndex + StepHeight + 1;

			assert(numIndices + 4 < NumIndices && "Out Of Range");
			indices[numIndices + 4] = vertexIndex + StepHeight;

			assert(numIndices + 5 < NumIndices && "Out Of Range");
			indices[numIndices + 5] = vertexIndex;

			numIndices += 6;
			vertexIndex++;
		}
		vertexHOffset += kOffset;
	}
	//MemoryUtils::CheckMemory();
	numIndices = 3 * numPolygons;
	for (uint i = 0; i < numIndices; i += 3) {
		// 1 index
		ushort index0 = indices[i + 0];
		// 2 index
		ushort index1 = indices[i + 1];
		// 3 index
		ushort index2 = indices[i + 2];
		assert(index0 < numVertexs && "Out Of Range");
		Vertex& V0 = vertexs[index0];
		assert(index1 < numVertexs && "Out Of Range");
		Vertex& V1 = vertexs[index1];
		assert(index2 < numVertexs && "Out Of Range");
		Vertex& V2 = vertexs[index2];
		const Vector3D& normal = Vector3D::Y();
		V0.normal = normal;
		V1.normal = normal;
		V2.normal = normal;
	}
	meshData.vb = renderer->addVertexBuffer(totalSize, STATIC, data);
	meshData.ib = renderer->addIndexBuffer(numIndices, sizeof(ushort), STATIC, indices);
	meshData.numFaces = numPolygons;
	//for(size_t i = 0; i < numVertexs; ++i) {
	//	Vector3D& normal = vertexs[i].normal;
	//	if (normal.y < 0.0f) {
	//		normal.y = - normal.y;
	//	}
	//	//assert(vertices[i].normal.y >= 0.0f);
	//	normal.Normalize();
	//}
	const FormatDesc format[] = { 
		{ 0, TYPE_VERTEX, FORMAT_FLOAT, 3 }, 
		{ 0, TYPE_NORMAL, FORMAT_FLOAT, 3 }, 
		{ 0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 }
	};
	meshData.format = renderer->addVertexFormat(format, elementsOf(format));
}


bool App::init(){
  map = new Model();
  if (!map->loadObj("../Models/Room6/Map.obj")){
    delete map;
    return false;
  }
  FillLightIndices();
  InitLightSources();
  {
    Stream stream = map->getStream(map->findStream(TYPE_VERTEX));
    vec3 *vertices = (vec3 *) stream.vertices;
    uint *indices = stream.indices;
    for (uint i = 0; i < map->getIndexCount(); i += 3){
      bsp.addTriangle(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
    }
  }
	// Load the geometry
	horseModel = new Model();
/*	
  if (!horseModel->loadObj("../Models/Horse.obj")){
		ErrorMsg("Couldn't load model file");
		return false;
	}
	horseModel->flipComponents(0, 1, 2);
  horseModel->reverseWinding();
	horseModel->flipComponents(1, 1, 2);
  float scaleSize[3] = {3000.0f, 3000.0f, 3000.0f};
  horseModel->scale(0, scaleSize);

  horseModel->save("../Models/Horse.hmdl");
  //*/

  if (!horseModel->load("../Models/Horse.hmdl")){
		ErrorMsg("Couldn't load model file");
		return false;
	}
  //*/

  bsp.build();

  map->computeTangentSpace(true);
  map->cleanUp();
  map->changeAllGeneric(true);

  // Create the render sphere model
  createSphereModel();

  int tab = configDialog->addTab("Rendering");
  
  configDialog->addWidget(tab, animateLights = new CheckBox(0, 0, 350, 36, "Animate lights", true));
  configDialog->addWidget(tab, staticLightScene = new CheckBox(0, 30, 350, 36, "Set static light scene", false));

  // Select the rendering tab as the active tab
  configDialog->setCurrentTab(tab);

  // Update the PFX moving lights into a stable starting condition
  // (spool up PFX)
  for(uint i=0; i<240; i++)
  {
    updateLights(1.0f/30.0f);
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
void App::exit(){
  delete map;
  delete sphereModel;
  delete horseModel;
}

///////////////////////////////////////////////////////////////////////////////
//
void App::onSize(const int w, const int h){
  OpenGLApp::onSize(w, h);

  if (renderer){
    // Make sure render targets are the size of the window
    renderer->resizeRenderTarget(lightIndexBuffer, w, h, 1, 1);
    renderer->resizeRenderTarget(depthRT, w, h, 1, 1);
  }
}

///////////////////////////////////////////////////////////////////////////////
//
bool App::onKey(const uint key, const bool pressed)
{
  // If in editor mode
  if(onKeyEditor(key, pressed)){
    return true;
  }
  return OpenGLApp::onKey(key, pressed);
}

///////////////////////////////////////////////////////////////////////////////
//
bool App::onMouseMove(const int x, const int y, const int deltaX, const int deltaY){

  // If in editor mode
  if(onMouseMoveEditor(x, y, deltaX,deltaY)){
    return true;
  }

  return OpenGLApp::onMouseMove(x, y, deltaX,deltaY);
}

///////////////////////////////////////////////////////////////////////////////
//
bool App::onMouseButton(const int x, const int y, const MouseButton button, const bool pressed){

  // If in editor mode
  if(onMouseButtonEditor(x, y, button, pressed)){
    return true;
  }

  return OpenGLApp::onMouseButton(x, y, button, pressed);
}

///////////////////////////////////////////////////////////////////////////////
//
bool App::onMouseWheel(const int x, const int y, const int scroll){

  // If in editor mode
  if(onMouseWheelEditor(x, y, scroll)){
    return true;
  }

  return OpenGLApp::onMouseWheel(x, y, scroll);
}

///////////////////////////////////////////////////////////////////////////////
//
bool App::load(){
#ifdef	USE_D3D_STYLE
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
#endif
  glGenVertexArrays(1, &vao);
  if (!map->makeDrawable(renderer)) return false;
  if (!sphereModel->makeDrawable(renderer)) return false;
  if (!horseModel->makeDrawable(renderer)) return false;

  // Samplerstates
  if ((trilinearAniso = renderer->addSamplerState(TRILINEAR_ANISO, WRAP, WRAP, WRAP)) == SS_NONE) return false;
  if ((linearWrap = renderer->addSamplerState(LINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;
  
  if ((pointClamp = renderer->addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;

  // FBO
  int fboDepthBits = 24;

  // Create the light direction buffers
  if ((lightIndexBuffer = renderer->addRenderTarget(width, height, FORMAT_RGBA8, pointClamp)) == TEXTURE_NONE) return false;
  if ((depthRT = renderer->addRenderDepth(width, height, fboDepthBits)) == TEXTURE_NONE) return false;

  // Shaders
  const char *attribs[] = { NULL, "textureCoord", "tangent", "binormal", "normal" };
  if ((plainTex = renderer->addShader("plainTex.shd", attribs, elementsOf(attribs))) == SHADER_NONE) return false;
  if ((depthOnly = renderer->addShader("depthOnly.shd")) == SHADER_NONE) return false;

  // Depth only pass for main view
  if ((lightingColorOnly = renderer->addShader("lightingColorOnly.shd")) == SHADER_NONE) return false;
  //if ((lightingColorOnly_depthClamp = renderer->addShader("lightingColorOnly.shd", "#define CLAMP_DEPTH 1\n")) == SHADER_NONE) return false;
  if ((lightingColorOnly_depthClamp = renderer->addShader("lightingColorOnly.shd")) == SHADER_NONE) return false;
  
  // Some shader limited cards cannot compile 4 lights per fragment
#ifdef USE_UBO
#ifdef NO_LIGHT_VIEW_SPACE
  lightingLIDefer = renderer->addShader("lightingLIDefer.shd", attribs, elementsOf(attribs), "#define OVERLAP_LIGHTS 4\n#define USE_UBO 1\n#define NO_LIGHT_VIEW_SPACE\n", ALLOW_FAILURE);
#else
  lightingLIDefer = renderer->addShader("lightingLIDefer.shd", attribs, elementsOf(attribs), "#define OVERLAP_LIGHTS 4\n#define USE_UBO 1", ALLOW_FAILURE);
#endif
#else
#ifdef NO_LIGHT_VIEW_SPACE
  lightingLIDefer = renderer->addShader("lightingLIDefer.shd", attribs, elementsOf(attribs), "#define OVERLAP_LIGHTS 4\n#define NO_LIGHT_VIEW_SPACE\n", ALLOW_FAILURE);
#else
  lightingLIDefer = renderer->addShader("lightingLIDefer.shd", attribs, elementsOf(attribs), "#define OVERLAP_LIGHTS 4\n", ALLOW_FAILURE);
#endif
#endif

  // Some shader limited cards cannot compile 4 lights per fragment
#ifdef USE_UBO
  UBOlightingLIDefer = renderer->CreateUBO(lightingLIDefer, "UBO");
#ifdef NO_LIGHT_VIEW_SPACE
  lightingLIDefer_stone = renderer->addShader("lightingLIDefer_stone.shd", "#define OVERLAP_LIGHTS 4\n#define USE_UBO 1\n#define NO_LIGHT_VIEW_SPACE 1\n", ALLOW_FAILURE);
#else
  lightingLIDefer_stone = renderer->addShader("lightingLIDefer_stone.shd", "#define OVERLAP_LIGHTS 4\n#define USE_UBO 1\n", ALLOW_FAILURE);
#endif
#else
#ifdef NO_LIGHT_VIEW_SPACE
  lightingLIDefer_stone = renderer->addShader("lightingLIDefer_stone.shd", "#define OVERLAP_LIGHTS 4\n#define NO_LIGHT_VIEW_SPACE\n", ALLOW_FAILURE);
#else
  lightingLIDefer_stone = renderer->addShader("lightingLIDefer_stone.shd", "#define OVERLAP_LIGHTS 4\n", ALLOW_FAILURE);
#endif
#endif
#ifdef USE_UBO
  UBOlightingLIDefer_stone = renderer->CreateUBO(lightingLIDefer_stone, "UBO");
#endif
  
  // Textures
  if ((base[0] = renderer->addTexture  ("../Textures/floor_wood_3.dds",                   true, trilinearAniso)) == SHADER_NONE) return false;
  if ((bump[0] = renderer->addNormalMap("../Textures/floor_wood_3Bump.dds", FORMAT_RGBA8, true, trilinearAniso)) == SHADER_NONE) return false;
  parallax[0] = 0.0f;

  if ((base[1] = renderer->addTexture  ("../Textures/brick01.dds",                   true, trilinearAniso)) == SHADER_NONE) return false;
  if ((bump[1] = renderer->addNormalMap("../Textures/brick01Bump.dds", FORMAT_RGBA8, true, trilinearAniso)) == SHADER_NONE) return false;
  parallax[1] = 0.04f;

  if ((base[2] = renderer->addTexture  ("../Textures/stone08.dds",                   true, trilinearAniso)) == SHADER_NONE) return false;
  if ((bump[2] = renderer->addNormalMap("../Textures/stone08Bump.dds", FORMAT_RGBA8, true, trilinearAniso)) == SHADER_NONE) return false;
  parallax[2] = 0.0f;

  if ((base[3] = renderer->addTexture  ("../Textures/StoneWall_1-4.dds",                   true, trilinearAniso)) == SHADER_NONE) return false;
  if ((bump[3] = renderer->addNormalMap("../Textures/StoneWall_1-4Bump.dds", FORMAT_RGBA8, true, trilinearAniso)) == SHADER_NONE) return false;
  parallax[3] = 0.03f;

  if ((light = renderer->addTexture("../Textures/spot.dds", false, linearClamp)) == SHADER_NONE) return false;

  if ((noise3D = renderer->addTexture("../Textures/NoiseVolume.dds", true, linearWrap)) == SHADER_NONE) return false;
  
  // Reset the bitmask texture lookups
  bitMaskLightColors = TEXTURE_NONE;
  bitMaskLightPos    = TEXTURE_NONE;

  // Blendstates
  if ((blendAdd = renderer->addBlendState(ONE, ONE)) == BS_NONE) return false;
  if ((blendCopy = renderer->addBlendState(ONE, ZERO)) == BS_NONE) return false;
  if ((blendTwoLightRender = renderer->addBlendStateSeperate(DST_ALPHA, ONE_MINUS_DST_ALPHA, ONE, ZERO)) == BS_NONE) return false;
  if ((blendBitShift = renderer->addBlendState(ONE, GL_CONSTANT_COLOR)) == BS_NONE) return false;
  if ((blendMax = renderer->addBlendState(ONE, ONE, BM_MAX)) == BS_NONE) return false;

  if ((noColorWrite = renderer->addBlendState(ONE, ZERO, BM_ADD, NONE)) == BS_NONE) return false;
  if ((depthNoWritePassGreater = renderer->addDepthState(true,  false, GEQUAL)) == BS_NONE) return false;
  
  // Huh? This is not already all 1's? (according to the spec? - Nvidia bug if main surface does not have stencil?)
  //glStencilMask(0xFFFFFFFF); 

  lightSourceBuffer = renderer->addVertexBuffer(4 * sizeof(vec3), DYNAMIC);
  const uint16_t indices[] = {
	0, 1, 2, 2, 3, 0
  };
  lightSourceIndexBuffer = renderer->addIndexBuffer(6, sizeof(uint16_t), STATIC, indices);
  FormatDesc format[] = { { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 } ,
  };
  quadVertexFormat = renderer->addVertexFormat(format, elementsOf(format), plainTex);
  CreatePlane(meshPlane, 1500, 1500, 20, 20, Vector3D::Y(), 0);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
void App::unload(){
  
}

///////////////////////////////////////////////////////////////////////////////
//
void App::drawLightParticles(const vec3 &dx, const vec3 &dy){


  for (uint i = 0; i < MAX_LIGHT_TOTAL; i++){

   if(lightDataArray[i].isEnabled){
    //glColor3fv(lightDataArray[i].color);
	float renderSize = lightDataArray[i].size / 10.0f;
	renderer->reset();
	renderer->setShader(plainTex);
	renderer->setShaderConstant4x4f("ModelViewProjectionMatrix", modelviewMatrixProjMatrix);
	renderer->setBlendState(blendAdd);
	renderer->setTexture("Base", light);
	renderer->setDepthState(noDepthWrite);
	renderer->setRasterizerState(cullNone);
	renderer->setVertexBuffer(0, lightSourceBuffer);
	renderer->setIndexBuffer(lightSourceIndexBuffer);
	renderer->setVertexFormat(quadVertexFormat);

	renderer->setShaderConstant3f("inColor", lightDataArray[i].color);
	renderer->applyConstants();
	renderer->apply();
	const vec3 data[] = {
	{	lightDataArray[i].position - renderSize * dx + renderSize * dy },
	{	lightDataArray[i].position + renderSize * dx + renderSize * dy },
	{	lightDataArray[i].position + renderSize * dx - renderSize * dy },
	{	lightDataArray[i].position - renderSize * dx - renderSize * dy }
	};
	renderer->updateBuffer(lightSourceBuffer, data, sizeof(data));
	//renderer->drawArrays(PRIM_TRIANGLE_STRIP, 0, 4);
	//renderer->drawArrays(PRIM_QUADS, 0, 4);
	renderer->drawElements(PRIM_TRIANGLES, 0, 6, 0, 4);
   }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
void App::updateBitMaskedLightTextures()
{
  // If the color texture does not exist, create it now
  if(bitMaskLightColors == TEXTURE_NONE) {
    Image loadData;
    loadData.create(FORMAT_RGBA8, MAX_LIGHT_TOTAL + 1, 1, 1, 1);

    unsigned char *dstData = loadData.getPixels();

    // Set the zero entry to black (no light)
    dstData[0] = 0;
    dstData[1] = 0;
    dstData[2] = 0;
    dstData[3] = 0;
    dstData += 4;

    for (int i =0; i<MAX_LIGHT_TOTAL; i++){
		LightData& lightData = lightDataArray[i];
		dstData[0] = (unsigned char)min((lightData.color.x * 256.0f), 255.0f);
		dstData[1] = (unsigned char)min((lightData.color.y * 256.0f), 255.0f);
		dstData[2] = (unsigned char)min((lightData.color.z * 256.0f), 255.0f);
      dstData[3] = 0;
#ifdef USE_UBO
	  Light& l = lights[i];
	  memcpy(&l.lightColor, &lightData.color, sizeof(vec3));
#endif
      dstData += 4;
    }

    // Load in the new texture
    bitMaskLightColors = renderer->addTexture(loadData, pointClamp);
  }

  // TODO: Test if doing a image-sub is faster - Use PBO's?
  // Delete the old lightPos texture
  if(bitMaskLightPos != TEXTURE_NONE){
    renderer->removeTexture(bitMaskLightPos);
    bitMaskLightPos = TEXTURE_NONE;
  }

  // Upload a new texture
  {
    Image loadPosData;
    loadPosData.create(FORMAT_RGBA32F, MAX_LIGHT_TOTAL + 1, 1, 1, 1);

    float *dstData = (float*)loadPosData.getPixels();

    // Set the zero entry to black (no light)
    dstData[0] = 0.0f;
    dstData[1] = 0.0f;
    dstData[2] = 0.0f;
    dstData[3] = 0.0f;
    dstData += 4;

    for (int i = 0; i < MAX_LIGHT_TOTAL; i++){
#if 1
      // Move position into view space
		//mat4 m = modelviewMatrix;
		//m.translate();
      //vec4 viewSpace = modelviewMatrix * vec4(lightDataArray[i].position, 1.0);
		Vector3D viewSpace = lightDataArray[i].position;
		TransformCoord(reinterpret_cast<Vector3D &>(viewSpace), reinterpret_cast<Matrix4x4 &>(modelviewMatrix));

      dstData[0] = viewSpace.x;
      dstData[1] = viewSpace.y;
      dstData[2] = viewSpace.z;
#else
		memcpy(dstData, lightDataArray[i].position, sizeof(vec3));
#endif
      dstData[3] = 1.0f / lightDataArray[i].size;
#ifdef USE_UBO
	  Light& l = lights[i];
	  memcpy(&l.posRange, dstData, sizeof(vec4));
#endif
      dstData += 4;
    }

    // Load in the new texture
    bitMaskLightPos = renderer->addTexture(loadPosData, pointClamp);
  }
}

void App::drawMeshData(const MeshData& meshData) const
{
	renderer->changeVertexFormat(meshData.format);
	renderer->changeVertexBuffer(0, meshData.vb);
	renderer->changeIndexBuffer(meshData.ib);
	renderer->drawElements(PRIM_TRIANGLES, 0, meshData.numFaces * 3, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
//
void App::drawDepthOnly()
{
  renderer->reset();
  renderer->setShader(depthOnly);
  renderer->setShaderConstant4x4f("ModelViewProjectionMatrix", modelviewMatrixProjMatrix);
  renderer->setBlendState(noColorWrite);
  renderer->setRasterizerState(cullBack);
  renderer->apply();
 
	//Clear the output color and depth
  float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  renderer->clear(true, true, clearColor);

  outGLError();
#ifndef DRAW_PLANE_ONLY
  map->draw(renderer);
  outGLError();
  horseModel->draw(renderer);
#else
  drawMeshData(meshPlane);
#endif
  outGLError();
}

///////////////////////////////////////////////////////////////////////////////
//
void App::drawLIDeferLight(GLubyte lightIndex, const vec3 &lightPosition, float lightSize){

	bool depthBoundTest = GetAsyncKeyState('B') || GetAsyncKeyState('b');
	if (!depthBoundTest) {

		vec4 diffVector = vec4(0.0f, 0.0f, lightSize, 0.0f);

		vec4 viewSpaceLightPos = modelviewMatrix * vec4(lightPosition, 1.0f);
		vec4 nearVec = projectionMatrix * (viewSpaceLightPos - diffVector);
		vec4 farVec = projectionMatrix * (viewSpaceLightPos + diffVector);
#ifdef	USE_D3D_STYLE
		float nearVal = clamp(nearVec.z / nearVec.w, 0.0f, 1.0f);// *0.5f + 0.5f;
		float farVal = clamp(farVec.z / farVec.w, 0.0f, 1.0f);// *0.5f + 0.5f;
#else
		float nearVal = clamp(nearVec.z / nearVec.w, -1.0f, 1.0f) * 0.5f + 0.5f;
		float farVal = clamp(farVec.z / farVec.w, -1.0f, 1.0f) * 0.5f + 0.5f;
#endif
		if (nearVec.w <= 0.0f) {
			nearVal = 0.0f;
		}
		if (farVec.w <= 0.0f) {
			farVal = 0.0f;
		}
		// Sanity check
		if (nearVal > farVal) {
			nearVal = farVal;
		}
		if (glDepthBoundsEXT) {
			glEnable(GL_DEPTH_BOUNDS_TEST_EXT);
			glDepthBoundsEXT(nearVal, farVal); // zMin <= z <= zMax // 0.0f...1.0f
		}
		// 0.96 .. 0.99
  }
	const vec4& outColor = lightIndices[lightIndex];
  // Note: Should use a infinite view projection matrix and cull front faces
  renderer->setShaderConstant4x4f("ModelViewProjectionMatrix", modelviewMatrixProjMatrix);
  renderer->setShaderConstant4f("LightData", vec4(lightPosition, lightSize));
  renderer->setShaderConstant4f("outColor", outColor);
  renderer->applyConstants();

  renderer->changeRasterizerState(cullFront);
  renderer->changeBlendState(blendBitShift);

  //TODO: Render lowers detail spheres when light is far away?
  // Draw a sphere the radius of the light
  sphereModel->draw(renderer);
  if (glDepthBoundsEXT && !depthBoundTest) {
	  glDisable(GL_DEPTH_BOUNDS_TEST_EXT);
  }
}

///////////////////////////////////////////////////////////////////////////////
//
void App::drawLIDeferLights(){

  // Set target to render depth only 
  // (ATI does not sopport only rendering to a depth buffer? Depth buffer seems inverted when bound to another FBO)
  renderer->changeRenderTarget(lightIndexBuffer, depthRT);
  drawDepthOnly();

  // Set the constant blend color to bit shift 2 bits down on each call
  glBlendColor(0.251f, 0.251f, 0.251f, 0.251f); 

 renderer->reset();
  renderer->setShader(lightingColorOnly);
  renderer->setDepthState(depthNoWritePassGreater);
  renderer->apply();
  
  // Loop for each light color to give each color an even chance of been visible
  //  Draw the primary lights last by iterating through the loop backwards
  for (int i = MAX_LIGHT_TOTAL - 1; i >= 0; i--){
    if(lightDataArray[i].isEnabled){
      drawLIDeferLight(i + 1, lightDataArray[i].position, lightDataArray[i].size);
    }
  }

  if (GetAsyncKeyState('L')) {
	Image image;
	unsigned char* pixels = image.create(FORMAT_RGBA8, width, height, 1, 1);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	image.saveTGA("LIDeferLights.tga");
  }
  renderer->changeToMainFramebuffer();
}

///////////////////////////////////////////////////////////////////////////////
//
void App::drawLIDeferLitObjects()
{
  // Do a z-pre pass (this is wasteful as we already have a render target with the depth)
  renderer->changeDepthState(DS_NONE);
  drawDepthOnly();


 // Setup render states
  renderer->reset();
  renderer->setShader(lightingLIDefer);
  renderer->setShaderConstant4x4f("ModelViewProjectionMatrix", modelviewMatrixProjMatrix);
  renderer->setShaderConstant4x4f("ModelViewMatrix", modelviewMatrix);
  //renderer->setShaderConstant4x4f("ModelViewMatrix", transpose(modelviewMatrix));
  renderer->setRasterizerState(cullBack);
  renderer->setBlendState(blendCopy);
  renderer->setDepthState(noDepthWrite);
  renderer->setShaderConstant3f("camPos", camPos);
  renderer->apply();

  renderer->setTexture("BitPlane", lightIndexBuffer);
  renderer->setTexture("LightColorTex", bitMaskLightColors);
  renderer->setTexture("LightPosTex", bitMaskLightPos);
#ifdef USE_UBO
  renderer->UpdateUBO(UBOlightingLIDefer, lights, sizeof(lights));
#endif
  //Loop for all pieces of geometry
#ifndef DRAW_PLANE_ONLY
  for (uint k = 0; k < 4; k++){
    renderer->setTexture("Base", base[k]);
    renderer->setTexture("Bump", bump[k]);
    renderer->applyTextures();

    renderer->setShaderConstant1i("hasParallax", int(parallax[k] > 0.0f));
    renderer->setShaderConstant2f("plxCoeffs", vec2(2, -1) * parallax[k]);
    renderer->applyConstants();

    map->drawBatch(renderer, k);
  }
  outGLError();
#endif
  renderer->reset();
  renderer->setShader(lightingLIDefer_stone);
  renderer->setShaderConstant4x4f("ModelViewProjectionMatrix", modelviewMatrixProjMatrix);
  renderer->setShaderConstant4x4f("ModelViewMatrix", modelviewMatrix);
  renderer->setRasterizerState(cullBack);
  renderer->setBlendState(blendCopy);
  renderer->setDepthState(noDepthWrite);

  renderer->UpdateUBO(UBOlightingLIDefer_stone, lights, sizeof(lights));

  renderer->setTexture("Noise", noise3D);
  renderer->setTexture("BitPlane", lightIndexBuffer);
  renderer->setTexture("LightColorTex", bitMaskLightColors);
  renderer->setTexture("LightPosTex", bitMaskLightPos);
  renderer->apply();
#ifndef DRAW_PLANE_ONLY
  horseModel->draw(renderer);
#else
  drawMeshData(meshPlane);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
void App::updateLightCull()
{
  // Update the PFX light culling
  for(uint i=0; i<MAX_LIGHT_TOTAL; i++)
  {
    if(lightDataArray[i].size > 0.0f){

      // Get if the light is visible on screen
      lightDataArray[i].isEnabled = getScissorRectangle(modelviewMatrix, lightDataArray[i].position, lightDataArray[i].size,
        1.5f, width, height, 
        &lightDataArray[i].screenX, 
        &lightDataArray[i].screenY, 
        &lightDataArray[i].screenWidth, 
        &lightDataArray[i].screenHeight);
    }
    else{
      lightDataArray[i].isEnabled = false;
    }
  }
}

void App::drawLightBuffer()
{

}



///////////////////////////////////////////////////////////////////////////////
//
void App::drawFrame(){
  // Update and load the modelview and projection matrices
#ifdef USE_D3D_STYLE
  Matrix4x4& mat = reinterpret_cast<Matrix4x4&>(projectionMatrix);
  mat.PerspectiveFovDirect3D(1.5f / (degrad), static_cast<float>(width) / height, 5, 4000);
  mat.Transpose();
  modelviewMatrix = rotateXY(wx, wy);
  modelviewMatrix.translate(camPos);
#else
	projectionMatrix = perspectiveMatrixY(1.5f, width, height, 5, 4000);
	modelviewMatrix = rotateXY(-wx, -wy);
	modelviewMatrix.translate(-camPos);
#endif
  modelviewMatrixProjMatrix = projectionMatrix * modelviewMatrix;

  // If switching to the static light scene
  if(staticLightScene->isChecked() != staticLightSceneSet){

    staticLightSceneSet = staticLightScene->isChecked();
    if(staticLightSceneSet){
      SetStaticLightScene();
    }
  }

  // Update light positions if necessary
  if(animateLights->isChecked() && !staticLightScene->isChecked())
  {
    updateLights(frameTime);
//#ifdef USE_UBO
//	for (size_t i = 0; i < _countof(lights); ++i) {
//		const LightData& lightData = lightDataArray[i];
//		Light& l = lights[i];
//		memcpy(&l.posRange, &lightData.position, sizeof(vec4));
//		memcpy(&l.lightColor, &lightData.color, sizeof(vec3));
//		l.lightColor /= 255.0f;
//	}
//#endif
  }

  // Cull the lights to the bounds of the screen
  updateLightCull();

    // Update the light textures
    // TODO: Don't update if camera pos/rotation shas not changed 
    // (possibly use world space lights instead of camera space if lights do not move?)
    updateBitMaskedLightTextures();
	glBindVertexArray(vao);
#if 1
    // Add light volumes
    drawLIDeferLights();

    // TODO: Find out why looking at the back wall 
    // is faster than looking across whole scene - even with no light - should be same fragment work...Fast depth Z not working?


    // Draw the lit objects - lighting using the deferred light indexes
    drawLIDeferLitObjects();

    drawLightParticles(modelviewMatrix.rows[0].xyz(), modelviewMatrix.rows[1].xyz());

  // Draw the editor data (if in editor mode)
  //drawFrameEditor();
#endif
  float xPos = 0.0f;
  float yPos = 0.0f;
  float width = 256;
  float height = 256;
  TexVertex quad[] = { MAKETEXQUAD(xPos, yPos + 0.2f * height, xPos + 0.6f * height, yPos + 0.8f * height, 3) };
  renderer->drawTextured(PRIM_TRIANGLE_STRIP, quad, elementsOf(quad), lightIndexBuffer, linearClamp, BS_NONE, noDepthTest);
}


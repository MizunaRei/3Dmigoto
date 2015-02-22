#pragma once

#include "Main.h"
#include <DirectXMath.h>
#include <ctime>
#include <vector>
#include <set>

// Key is index/vertex buffer, value is hash key.
typedef std::map<D3D11Base::ID3D11Buffer *, UINT64> DataBufferMap;

// Source compiled shaders.
typedef std::map<UINT64, std::string> CompiledShaderMap;

// Strategy: This OriginalShaderInfo record and associated map is to allow us to keep track of every
//	pixelshader and vertexshader that are compiled from hlsl text from the ShaderFixes
//	folder.  This keeps track of the original shader information using the ID3D11VertexShader*
//	or ID3D11PixelShader* as a master key to the key map.
//	We are using the base class of ID3D11DeviceChild* since both descend from that, and that allows
//	us to use the same structure for Pixel and Vertex shaders both.

// Info saved about originally overridden shaders passed in by the game in CreateVertexShader or
// CreatePixelShader that have been loaded as HLSL
//	shaderType is "vs" or "ps" or maybe later "gs" (type wstring for file name use)
//	shaderModel is only filled in when a shader is replaced.  (type string for old D3 API use)
//	linkage is passed as a parameter, seems to be rarely if ever used.
//	byteCode is the original shader byte code passed in by game, or recompiled by override.
//	timeStamp allows reloading/recompiling only modified shaders
//	replacement is either ID3D11VertexShader or ID3D11PixelShader
struct OriginalShaderInfo
{
	UINT64 hash;
	std::wstring shaderType;
	std::string shaderModel;
	D3D11Base::ID3D11ClassLinkage* linkage;
	D3D11Base::ID3DBlob* byteCode;
	FILETIME timeStamp;
	D3D11Base::ID3D11DeviceChild* replacement;
};

// Key is the overridden shader that was given back to the game at CreateVertexShader (vs or ps)
typedef std::map<D3D11Base::ID3D11DeviceChild *, OriginalShaderInfo> ShaderReloadMap;

// Key is vertexshader, value is hash key.
typedef std::map<D3D11Base::ID3D11VertexShader *, UINT64> VertexShaderMap;
typedef std::map<UINT64, D3D11Base::ID3D11VertexShader *> PreloadVertexShaderMap;
typedef std::map<D3D11Base::ID3D11VertexShader *, D3D11Base::ID3D11VertexShader *> VertexShaderReplacementMap;

// Key is pixelshader, value is hash key.
typedef std::map<D3D11Base::ID3D11PixelShader *, UINT64> PixelShaderMap;
typedef std::map<UINT64, D3D11Base::ID3D11PixelShader *> PreloadPixelShaderMap;
typedef std::map<D3D11Base::ID3D11PixelShader *, D3D11Base::ID3D11PixelShader *> PixelShaderReplacementMap;

typedef std::map<D3D11Base::ID3D11HullShader *, UINT64> HullShaderMap;
typedef std::map<D3D11Base::ID3D11DomainShader *, UINT64> DomainShaderMap;
typedef std::map<D3D11Base::ID3D11ComputeShader *, UINT64> ComputeShaderMap;
typedef std::map<D3D11Base::ID3D11GeometryShader *, UINT64> GeometryShaderMap;

struct ShaderOverride {
	float separation;
	float convergence;
	bool skip;
#if 0 /* Iterations are broken since we no longer use present() */
	std::vector<int> iterations; // Only for separation changes, not shaders.
#endif
	std::vector<UINT64> indexBufferFilter;

	ShaderOverride() :
		separation(FLT_MAX),
		convergence(FLT_MAX),
		skip(false)
	{}
};
typedef std::map<UINT64, struct ShaderOverride> ShaderOverrideMap;

struct TextureOverride {
	int stereoMode;
	int format;
#if 0 /* Iterations are broken since we no longer use present() */
	std::vector<int> iterations;
#endif

	TextureOverride() :
		stereoMode(-1),
		format(-1)
	{}
};
typedef std::map<UINT64, struct TextureOverride> TextureOverrideMap;

struct ShaderInfoData
{
	std::map<int, void *> ResourceRegisters;
	std::set<UINT64> PartnerShader;
	std::vector<std::set<void *>> RenderTargets;
	std::set<void *> DepthTargets;
};
struct SwapChainInfo
{
	int width, height;
};

struct ResourceInfo
{
	D3D11Base::D3D11_RESOURCE_DIMENSION type;
	union {
		D3D11Base::D3D11_TEXTURE2D_DESC tex2d_desc;
		D3D11Base::D3D11_TEXTURE3D_DESC tex3d_desc;
	};

	ResourceInfo() :
		type(D3D11Base::D3D11_RESOURCE_DIMENSION_UNKNOWN)
	{}

	struct ResourceInfo & operator= (D3D11Base::D3D11_TEXTURE2D_DESC desc)
	{
		type = D3D11Base::D3D11_RESOURCE_DIMENSION_TEXTURE2D;
		tex2d_desc = desc;
		return *this;
	}

	struct ResourceInfo & operator= (D3D11Base::D3D11_TEXTURE3D_DESC desc)
	{
		type = D3D11Base::D3D11_RESOURCE_DIMENSION_TEXTURE3D;
		tex3d_desc = desc;
		return *this;
	}
};

struct Globals
{
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	int SCREEN_REFRESH;
	int SCREEN_FULLSCREEN;
	int marking_mode;
	bool gForceStereo;
	bool gCreateStereoProfile;
	int gSurfaceCreateMode;
	int gSurfaceSquareCreateMode;

	bool hunting;
	time_t huntTime;

	int EXPORT_HLSL;		// 0=off, 1=HLSL only, 2=HLSL+OriginalASM, 3= HLSL+OriginalASM+recompiledASM
	bool EXPORT_SHADERS, EXPORT_FIXED, CACHE_SHADERS, PRELOAD_SHADERS, SCISSOR_DISABLE, COPY_ON_MARK;
	char ZRepair_DepthTextureReg1, ZRepair_DepthTextureReg2;
	std::string ZRepair_DepthTexture1, ZRepair_DepthTexture2;
	std::vector<std::string> ZRepair_Dependencies1, ZRepair_Dependencies2;
	std::string ZRepair_ZPosCalc1, ZRepair_ZPosCalc2;
	std::string ZRepair_PositionTexture, ZRepair_WorldPosCalc;
	std::vector<std::string> InvTransforms;
	std::string BackProject_Vector1, BackProject_Vector2;
	std::string ObjectPos_ID1, ObjectPos_ID2, ObjectPos_MUL1, ObjectPos_MUL2;
	std::string MatrixPos_ID1, MatrixPos_MUL1;
	UINT64 ZBufferHashToInject;
	bool FIX_SV_Position;
	bool FIX_Light_Position;
	bool FIX_Recompile_VS;
	bool DumpUsage;
	bool ENABLE_TUNE;
	float gTuneValue[4], gTuneStep;

	DirectX::XMFLOAT4 iniParams;

	SwapChainInfo mSwapChainInfo;

	ThreadSafePointerSet m_AdapterList;
	CRITICAL_SECTION mCriticalSection;
	bool ENABLE_CRITICAL_SECTION;

	DataBufferMap mDataBuffers;
	UINT64 mCurrentIndexBuffer;
	std::set<UINT64> mVisitedIndexBuffers;
	UINT64 mSelectedIndexBuffer;
	unsigned int mSelectedIndexBufferPos;
	std::set<UINT64> mSelectedIndexBuffer_VertexShader;
	std::set<UINT64> mSelectedIndexBuffer_PixelShader;

	CompiledShaderMap mCompiledShaderMap;

	VertexShaderMap mVertexShaders;							// All shaders ever registered with CreateVertexShader
	PreloadVertexShaderMap mPreloadedVertexShaders;			// All shaders that were preloaded as .bin
	VertexShaderReplacementMap mOriginalVertexShaders;		// When MarkingMode=Original, switch to original
	VertexShaderReplacementMap mZeroVertexShaders;			// When MarkingMode=zero.
	UINT64 mCurrentVertexShader;							// Shader currently live in GPU pipeline.
	std::set<UINT64> mVisitedVertexShaders;					// Only shaders seen in latest frame
	UINT64 mSelectedVertexShader;							// Shader selected using XInput
	unsigned int mSelectedVertexShaderPos;
	std::set<UINT64> mSelectedVertexShader_IndexBuffer;

	PixelShaderMap mPixelShaders;							// All shaders ever registered with CreatePixelShader
	PreloadPixelShaderMap mPreloadedPixelShaders;
	PixelShaderReplacementMap mOriginalPixelShaders;
	PixelShaderReplacementMap mZeroPixelShaders;
	UINT64 mCurrentPixelShader;
	std::set<UINT64> mVisitedPixelShaders;
	UINT64 mSelectedPixelShader;
	unsigned int mSelectedPixelShaderPos;
	std::set<UINT64> mSelectedPixelShader_IndexBuffer;

	ShaderReloadMap mReloadedShaders;						// Shaders that were reloaded live from ShaderFixes

	GeometryShaderMap mGeometryShaders;
	ComputeShaderMap mComputeShaders;
	DomainShaderMap mDomainShaders;
	HullShaderMap mHullShaders;

	ShaderOverrideMap mShaderOverrideMap;
	TextureOverrideMap mTextureOverrideMap;

	// Statistics
	std::map<void *, UINT64> mRenderTargets;
	std::map<UINT64, struct ResourceInfo> mRenderTargetInfo;
	std::map<UINT64, struct ResourceInfo> mDepthTargetInfo;
	std::set<void *> mVisitedRenderTargets;
	std::vector<void *> mCurrentRenderTargets;
	void *mSelectedRenderTarget;
	unsigned int mSelectedRenderTargetPos;
	void *mCurrentDepthTarget;
	// Snapshot of all targets for selection.
	void *mSelectedRenderTargetSnapshot;
	std::set<void *> mSelectedRenderTargetSnapshotList;
	// Relations
	std::map<D3D11Base::ID3D11Texture2D *, UINT64> mTexture2D_ID;
	std::map<D3D11Base::ID3D11Texture3D *, UINT64> mTexture3D_ID;
	std::map<UINT64, ShaderInfoData> mVertexShaderInfo;
	std::map<UINT64, ShaderInfoData> mPixelShaderInfo;

	bool mBlockingMode;

	Globals() :
		mBlockingMode(false),
		mSelectedRenderTargetSnapshot(0),
		mSelectedRenderTargetPos(0),
		mSelectedRenderTarget((void *)1),
		mCurrentDepthTarget(0),
		mCurrentPixelShader(0),
		mSelectedPixelShader(1),
		mSelectedPixelShaderPos(0),
		mCurrentVertexShader(0),
		mSelectedVertexShader(1),
		mSelectedVertexShaderPos(0),
		mCurrentIndexBuffer(0),
		mSelectedIndexBuffer(1),
		mSelectedIndexBufferPos(0),

		hunting(false),
		huntTime(0),

		EXPORT_SHADERS(false),
		EXPORT_HLSL(0),
		EXPORT_FIXED(false),
		CACHE_SHADERS(false),
		PRELOAD_SHADERS(false),
		FIX_SV_Position(false),
		FIX_Light_Position(false),
		FIX_Recompile_VS(false),
		DumpUsage(false),
		ENABLE_TUNE(false),
		gTuneStep(0.001f),

		iniParams{ FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX },

		ENABLE_CRITICAL_SECTION(false),
		SCREEN_WIDTH(-1),
		SCREEN_HEIGHT(-1),
		SCREEN_REFRESH(-1),
		SCREEN_FULLSCREEN(-1),
		marking_mode(-1),
		gForceStereo(false),
		gCreateStereoProfile(false),
		gSurfaceCreateMode(-1),
		gSurfaceSquareCreateMode(-1),
		ZBufferHashToInject(0),
		SCISSOR_DISABLE(0),
		COPY_ON_MARK(false)
	{
		mSwapChainInfo.width = -1;
		mSwapChainInfo.height = -1;

		for (int i = 0; i < 4; i++)
			gTuneValue[i] = 1.0f;
	}
};

extern Globals *G;
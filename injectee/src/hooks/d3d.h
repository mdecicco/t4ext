#pragma once
#include <types.h>

#include <d3d9.h>

class IDirect3D9;

namespace t4ext {
    struct d3d : public IDirect3D9 {
        public:
            d3d(IDirect3D9* originalInterface);

            virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
            virtual ULONG __stdcall AddRef();
            virtual ULONG __stdcall Release();
            virtual HRESULT __stdcall RegisterSoftwareDevice(void* pInitializeFunction);
            virtual UINT __stdcall GetAdapterCount();
            virtual HRESULT __stdcall GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier);
            virtual UINT __stdcall GetAdapterModeCount(UINT Adapter, D3DFORMAT Format);
            virtual HRESULT __stdcall EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode);
            virtual HRESULT __stdcall GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode);
            virtual HRESULT __stdcall CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed);
            virtual HRESULT __stdcall CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
            virtual HRESULT __stdcall CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
            virtual HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);
            virtual HRESULT __stdcall CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat);
            virtual HRESULT __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps);
            virtual HMONITOR __stdcall GetAdapterMonitor(UINT Adapter);
            virtual HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
        
        protected:
            IDirect3D9 *m_d3d;
            HWND m_window;
    };

    struct d3dDevice;

    struct d3dDevice : public IDirect3DDevice9 {
        public:
            d3dDevice(IDirect3DDevice9 *pReturnedDeviceInterface, D3DPRESENT_PARAMETERS *pPresentParam, IDirect3D9 *pIDirect3D9);

            virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
            virtual ULONG __stdcall AddRef();
            virtual ULONG __stdcall Release();
            virtual HRESULT __stdcall TestCooperativeLevel();
            virtual UINT __stdcall GetAvailableTextureMem();
            virtual HRESULT __stdcall EvictManagedResources();
            virtual HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9);
            virtual HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps);
            virtual HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode);
            virtual HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);
            virtual HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap);
            virtual void __stdcall SetCursorPosition(int X, int Y, DWORD Flags);
            virtual BOOL __stdcall ShowCursor(BOOL bShow);
            virtual HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain);
            virtual HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain);
            virtual UINT __stdcall  GetNumberOfSwapChains();
            virtual HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
            virtual HRESULT __stdcall Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
            virtual HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer);
            virtual HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus);
            virtual HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs);
            virtual void __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp);
            virtual void __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp);
            virtual HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle);
            virtual HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle);
            virtual HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle);
            virtual HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle);
            virtual HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle);
            virtual HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
            virtual HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
            virtual HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint);
            virtual HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture);
            virtual HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface);
            virtual HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface);
            virtual HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter);
            virtual HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color);
            virtual HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
            virtual HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget);
            virtual HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget);
            virtual HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);
            virtual HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface);
            virtual HRESULT __stdcall BeginScene();
            virtual HRESULT __stdcall EndScene();
            virtual HRESULT __stdcall Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
            virtual HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
            virtual HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);
            virtual HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*);
            virtual HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport);
            virtual HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport);
            virtual HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial);
            virtual HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial);
            virtual HRESULT __stdcall SetLight(DWORD Index, CONST D3DLIGHT9*);
            virtual HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9*);
            virtual HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable);
            virtual HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable);
            virtual HRESULT __stdcall SetClipPlane(DWORD Index, CONST float* pPlane);
            virtual HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane);
            virtual HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
            virtual HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue);
            virtual HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB);
            virtual HRESULT __stdcall BeginStateBlock();
            virtual HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB);
            virtual HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus);
            virtual HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus);
            virtual HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture);
            virtual HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);
            virtual HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue);
            virtual HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
            virtual HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue);
            virtual HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
            virtual HRESULT __stdcall ValidateDevice(DWORD* pNumPasses);
            virtual HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries);
            virtual HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries);
            virtual HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber);
            virtual HRESULT __stdcall GetCurrentTexturePalette(UINT *PaletteNumber);
            virtual HRESULT __stdcall SetScissorRect(CONST RECT* pRect);
            virtual HRESULT __stdcall GetScissorRect(RECT* pRect);
            virtual HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware);
            virtual BOOL __stdcall GetSoftwareVertexProcessing();
            virtual HRESULT __stdcall SetNPatchMode(float nSegments);
            virtual float __stdcall GetNPatchMode();
            virtual HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
            virtual HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
            virtual HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
            virtual HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
            virtual HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags);
            virtual HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl);
            virtual HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl);
            virtual HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl);
            virtual HRESULT __stdcall SetFVF(DWORD FVF);
            virtual HRESULT __stdcall GetFVF(DWORD* pFVF);
            virtual HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader);
            virtual HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader);
            virtual HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader);
            virtual HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
            virtual HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount);
            virtual HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount);
            virtual HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount);
            virtual HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount);
            virtual HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
            virtual HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
            virtual HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride);
            virtual HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Setting);
            virtual HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting);
            virtual HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData);
            virtual HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData);
            virtual HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader);
            virtual HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader);
            virtual HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader);
            virtual HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
            virtual HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount);
            virtual HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount);
            virtual HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount);
            virtual HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount);
            virtual HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
            virtual HRESULT __stdcall DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo);
            virtual HRESULT __stdcall DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo);
            virtual HRESULT __stdcall DeletePatch(UINT Handle);
            virtual HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery);
        
        protected:
            HWND m_window;
            IDirect3DDevice9 *m_dev;
            IDirect3D9 *m_d3d;
            D3DPRESENT_PARAMETERS m_PresentParam;
    };

    void InstallD3DHooks();
    void UninstallD3DHooks();
};
#include <hooks/d3d.h>
#include <utils/Singleton.hpp>
#include <Client.h>
#include <core/AppBase.h>

#include <MinHook.h>

namespace t4ext {
    //
    // IDirect3D9
    //

    d3d::d3d(IDirect3D9* originalInterface) {
		m_d3d = originalInterface;
        m_window = nullptr;
    }

    HRESULT APIENTRY d3d::QueryInterface(REFIID riid,  void **ppvObj) {
        return m_d3d->QueryInterface(riid,  ppvObj);
    }

    ULONG APIENTRY d3d::AddRef() {
        return m_d3d->AddRef();
    }

    HRESULT APIENTRY d3d::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) {
        return m_d3d->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
    }

    HRESULT APIENTRY d3d::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) {
        return m_d3d->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
    }

    HRESULT APIENTRY d3d::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) {
        return m_d3d->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
    }

    HRESULT APIENTRY d3d::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) {
        return m_d3d->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
    }

    HRESULT APIENTRY d3d::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed) {
        return m_d3d->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed);
    }

    HRESULT APIENTRY d3d::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface) {
        m_window = hFocusWindow;

        LONG lStyle = GetWindowLong(m_window, GWL_STYLE);
        lStyle |= WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
        SetWindowLong(m_window, GWL_STYLE, lStyle);

        LONG lExStyle = GetWindowLong(m_window, GWL_EXSTYLE);
        lExStyle |= WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;
        SetWindowLong(m_window, GWL_EXSTYLE, lExStyle);

        bool result = SetWindowPos(
            m_window,
            NULL,
            0,0,0,0,
            SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
        );

        pPresentationParameters->BackBufferWidth = AppBase::Get()->windowWidth;
        pPresentationParameters->BackBufferHeight = AppBase::Get()->windowHeight;
        pPresentationParameters->Windowed = TRUE;
        
        HRESULT hRet = m_d3d->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
        
        if (SUCCEEDED(hRet)) {
            *ppReturnedDeviceInterface = new d3dDevice(*ppReturnedDeviceInterface, pPresentationParameters, this);
        }

        return hRet;
    }

    HRESULT APIENTRY d3d::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) {
        return m_d3d->EnumAdapterModes(Adapter, Format, Mode, pMode);
    }

    UINT APIENTRY d3d::GetAdapterCount() {
        return m_d3d->GetAdapterCount();
    }

    HRESULT APIENTRY d3d::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode) {
        return m_d3d->GetAdapterDisplayMode(Adapter, pMode);
    }

    HRESULT APIENTRY d3d::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier) {
        return m_d3d->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
    }

    UINT APIENTRY d3d::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) {
        return m_d3d->GetAdapterModeCount(Adapter, Format);
    }

    HMONITOR APIENTRY d3d::GetAdapterMonitor(UINT Adapter) {
        return m_d3d->GetAdapterMonitor(Adapter);
    }

    HRESULT APIENTRY d3d::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps) {
        return m_d3d->GetDeviceCaps(Adapter, DeviceType, pCaps);
    }

    HRESULT APIENTRY d3d::RegisterSoftwareDevice(void *pInitializeFunction) {
        return m_d3d->RegisterSoftwareDevice(pInitializeFunction);
    }

    ULONG APIENTRY d3d::Release() {
        return m_d3d->Release();
    }



    //
    // IDirect3DDevice9
    //
    
    d3dDevice::d3dDevice(IDirect3DDevice9 *pReturnedDeviceInterface, D3DPRESENT_PARAMETERS *pPresentParam, IDirect3D9 *pIDirect3D9) {
		m_dev = pReturnedDeviceInterface; 
		m_PresentParam = *pPresentParam;
		m_d3d = pIDirect3D9;
        m_window = nullptr;
        
        gClient::Get()->onDeviceInit(this);
    }

    HRESULT APIENTRY d3dDevice::QueryInterface(REFIID riid, LPVOID *ppvObj) {
        return m_dev->QueryInterface(riid, ppvObj);
    }

    ULONG APIENTRY d3dDevice::AddRef() {
        return m_dev->AddRef();
    }

    HRESULT APIENTRY d3dDevice::BeginScene() {
        return m_dev->BeginScene();
    }

    HRESULT APIENTRY d3dDevice::BeginStateBlock() {
        return m_dev->BeginStateBlock();
    }

    HRESULT APIENTRY d3dDevice::Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
        if (pRects) return S_OK;
        return m_dev->Clear(Count, pRects, Flags, Color, Z, Stencil);
    }

    HRESULT APIENTRY d3dDevice::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color) {	
        return m_dev->ColorFill(pSurface,pRect,color);
    }

    HRESULT APIENTRY d3dDevice::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain) {
        return m_dev->CreateAdditionalSwapChain(pPresentationParameters, ppSwapChain);
    }

    HRESULT APIENTRY d3dDevice::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle) {
        return m_dev->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture,pSharedHandle);
    }

    HRESULT APIENTRY d3dDevice::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {
        return m_dev->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard,ppSurface, pSharedHandle);
    }

    HRESULT APIENTRY d3dDevice::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle) {
        return m_dev->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer,pSharedHandle);
    }

    HRESULT APIENTRY d3dDevice::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {
        return m_dev->CreateOffscreenPlainSurface(Width,Height,Format,Pool,ppSurface,pSharedHandle);
    }

    HRESULT APIENTRY d3dDevice::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) {
        return m_dev->CreatePixelShader(pFunction, ppShader);
    }

    HRESULT APIENTRY d3dDevice::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) {
        return m_dev->CreateQuery(Type,ppQuery);
    }

    HRESULT APIENTRY d3dDevice::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {
        return m_dev->CreateRenderTarget(Width, Height, Format, MultiSample,MultisampleQuality, Lockable, ppSurface,pSharedHandle);
    }

    HRESULT APIENTRY d3dDevice::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) {
        return m_dev->CreateStateBlock(Type, ppSB);
    }

    HRESULT APIENTRY d3dDevice::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle) {
        HRESULT ret = m_dev->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);

        // if (ret == D3D_OK) { new hkIDirect3DTexture9(ppTexture, this, Width, Height, Format); }

        return ret;
    }

    HRESULT APIENTRY d3dDevice::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle) {
        return m_dev->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer,pSharedHandle);
    }

    HRESULT APIENTRY d3dDevice::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) {
        return m_dev->CreateVertexDeclaration(pVertexElements,ppDecl);
    }

    HRESULT APIENTRY d3dDevice::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) {
        return m_dev->CreateVertexShader(pFunction, ppShader);
    }

    HRESULT APIENTRY d3dDevice::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle) {
        return m_dev->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture,pSharedHandle);
    }

    HRESULT APIENTRY d3dDevice::DeletePatch(UINT Handle) {
        return m_dev->DeletePatch(Handle);
    }

    HRESULT APIENTRY d3dDevice::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
        return m_dev->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
    }

    HRESULT APIENTRY d3dDevice::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) {	
        return m_dev->DrawIndexedPrimitiveUP(PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
    }

    HRESULT APIENTRY d3dDevice::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
        return m_dev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
    }

    HRESULT APIENTRY d3dDevice::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) {
        return m_dev->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
    }

    HRESULT APIENTRY d3dDevice::DrawRectPatch(UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo) {
        return m_dev->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
    }

    HRESULT APIENTRY d3dDevice::DrawTriPatch(UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo) {
        return m_dev->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
    }

    HRESULT APIENTRY d3dDevice::EndScene() {
        return m_dev->EndScene();
    }

    HRESULT APIENTRY d3dDevice::EndStateBlock(IDirect3DStateBlock9** ppSB) {
        return m_dev->EndStateBlock(ppSB);
    }

    HRESULT APIENTRY d3dDevice::EvictManagedResources() {
        return m_dev->EvictManagedResources();
    }

    UINT APIENTRY d3dDevice::GetAvailableTextureMem() {
        return m_dev->GetAvailableTextureMem();
    }

    HRESULT APIENTRY d3dDevice::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
        return m_dev->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
    }

    HRESULT APIENTRY d3dDevice::GetClipPlane(DWORD Index, float *pPlane) {
        return m_dev->GetClipPlane(Index, pPlane);
    }

    HRESULT APIENTRY d3dDevice::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus) {
        return m_dev->GetClipStatus(pClipStatus);
    }

    HRESULT APIENTRY d3dDevice::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) {
        return m_dev->GetCreationParameters(pParameters);
    }

    HRESULT APIENTRY d3dDevice::GetCurrentTexturePalette(UINT *pPaletteNumber) {
        return m_dev->GetCurrentTexturePalette(pPaletteNumber);
    }

    HRESULT APIENTRY d3dDevice::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface) {
        return m_dev->GetDepthStencilSurface(ppZStencilSurface);
    }

    HRESULT APIENTRY d3dDevice::GetDeviceCaps(D3DCAPS9 *pCaps) {
        return m_dev->GetDeviceCaps(pCaps);
    }

    HRESULT APIENTRY d3dDevice::GetDirect3D(IDirect3D9 **ppD3D9) {
        HRESULT hRet = m_dev->GetDirect3D(ppD3D9);
        if (SUCCEEDED(hRet)) *ppD3D9 = m_d3d;
        return hRet;
    }

    HRESULT APIENTRY d3dDevice::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
        return m_dev->GetDisplayMode(iSwapChain,pMode);
    }

    HRESULT APIENTRY d3dDevice::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) {
        return m_dev->GetFrontBufferData(iSwapChain,pDestSurface);
    }

    HRESULT APIENTRY d3dDevice::GetFVF(DWORD* pFVF) {
        return m_dev->GetFVF(pFVF);
    }

    void APIENTRY d3dDevice::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) {
        m_dev->GetGammaRamp(iSwapChain,pRamp);
    }

    HRESULT APIENTRY d3dDevice::GetIndices(IDirect3DIndexBuffer9** ppIndexData) {
        return m_dev->GetIndices(ppIndexData);
    }

    HRESULT APIENTRY d3dDevice::GetLight(DWORD Index, D3DLIGHT9 *pLight) {
        return m_dev->GetLight(Index, pLight);
    }

    HRESULT APIENTRY d3dDevice::GetLightEnable(DWORD Index, BOOL *pEnable) {
        return m_dev->GetLightEnable(Index, pEnable);
    }

    HRESULT APIENTRY d3dDevice::GetMaterial(D3DMATERIAL9 *pMaterial) {
        return m_dev->GetMaterial(pMaterial);
    }

    float APIENTRY d3dDevice::GetNPatchMode() {
        return m_dev->GetNPatchMode();
    }

    unsigned int APIENTRY d3dDevice::GetNumberOfSwapChains() {
        return m_dev->GetNumberOfSwapChains();
    }

    HRESULT APIENTRY d3dDevice::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries) {
        return m_dev->GetPaletteEntries(PaletteNumber, pEntries);
    }

    HRESULT APIENTRY d3dDevice::GetPixelShader(IDirect3DPixelShader9** ppShader) {
        return m_dev->GetPixelShader(ppShader);
    }

    HRESULT APIENTRY d3dDevice::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) {
        return m_dev->GetPixelShaderConstantB(StartRegister,pConstantData, BoolCount);
    }

    HRESULT APIENTRY d3dDevice::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) {
        return m_dev->GetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);
    }

    HRESULT APIENTRY d3dDevice::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) {
        return m_dev->GetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount);
    }

    HRESULT APIENTRY d3dDevice::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) {
        return m_dev->GetRasterStatus(iSwapChain,pRasterStatus);
    }

    HRESULT APIENTRY d3dDevice::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue) {
        return m_dev->GetRenderState(State, pValue);
    }

    HRESULT APIENTRY d3dDevice::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) {
        return m_dev->GetRenderTarget(RenderTargetIndex,ppRenderTarget);
    }

    HRESULT APIENTRY d3dDevice::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) {
        return m_dev->GetRenderTargetData(pRenderTarget,pDestSurface);
    }

    HRESULT APIENTRY d3dDevice::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) {
        return m_dev->GetSamplerState(Sampler,Type,pValue);
    }

    HRESULT APIENTRY d3dDevice::GetScissorRect(RECT* pRect) {
        return m_dev->GetScissorRect(pRect);
    }

    BOOL APIENTRY d3dDevice::GetSoftwareVertexProcessing() {
        return m_dev->GetSoftwareVertexProcessing();
    }

    HRESULT APIENTRY d3dDevice::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) {
        return m_dev->GetStreamSource(StreamNumber, ppStreamData,OffsetInBytes, pStride);
    }

    HRESULT APIENTRY d3dDevice::GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) {
        return m_dev->GetStreamSourceFreq(StreamNumber, Divider);
    }

    HRESULT APIENTRY d3dDevice::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) {
        return m_dev->GetSwapChain(iSwapChain,pSwapChain);
    }

    HRESULT APIENTRY d3dDevice::GetTexture(DWORD Stage, IDirect3DBaseTexture9 **ppTexture) {
        return m_dev->GetTexture(Stage, ppTexture);
    }

    HRESULT APIENTRY d3dDevice::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue) {
        return m_dev->GetTextureStageState(Stage, Type, pValue);
    }

    HRESULT APIENTRY d3dDevice::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix) {
        return m_dev->GetTransform(State, pMatrix);
    }

    HRESULT APIENTRY d3dDevice::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) {
        return m_dev->GetVertexDeclaration(ppDecl);
    }

    HRESULT APIENTRY d3dDevice::GetVertexShader(IDirect3DVertexShader9** ppShader) {
        return m_dev->GetVertexShader(ppShader);
    }

    HRESULT APIENTRY d3dDevice::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) {
        return m_dev->GetVertexShaderConstantB(StartRegister,pConstantData, BoolCount);
    }

    HRESULT APIENTRY d3dDevice::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) {
        return m_dev->GetVertexShaderConstantF(StartRegister,pConstantData,Vector4fCount);
    }

    HRESULT APIENTRY d3dDevice::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) {
        return m_dev->GetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount);
    }

    HRESULT APIENTRY d3dDevice::GetViewport(D3DVIEWPORT9 *pViewport) {
        return m_dev->GetViewport(pViewport);
    }

    HRESULT APIENTRY d3dDevice::LightEnable(DWORD LightIndex, BOOL bEnable) {
        return m_dev->LightEnable(LightIndex, bEnable);
    }

    HRESULT APIENTRY d3dDevice::MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix) {
        return m_dev->MultiplyTransform(State, pMatrix);
    }

    HRESULT APIENTRY d3dDevice::Present(CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {	
        if (m_dev && m_dev->TestCooperativeLevel() == D3D_OK) {
            gClient::Get()->onBeforeRender();
        }

        return m_dev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    }

    HRESULT APIENTRY d3dDevice::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) {
        return m_dev->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer,pVertexDecl, Flags);
    }

    ULONG APIENTRY d3dDevice::Release() {
        return m_dev->Release();
    }

    HRESULT APIENTRY d3dDevice::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters) {
        HRESULT hRet = m_dev->Reset(pPresentationParameters);

        if (SUCCEEDED(hRet)) {
            m_PresentParam = *pPresentationParameters;
            m_window = pPresentationParameters->hDeviceWindow;
            gClient::Get()->onDeviceReset(this);
        }

        return hRet;
    }

    HRESULT APIENTRY d3dDevice::SetClipPlane(DWORD Index, CONST float *pPlane) {
        return m_dev->SetClipPlane(Index, pPlane);
    }

    HRESULT APIENTRY d3dDevice::SetClipStatus(CONST D3DCLIPSTATUS9 *pClipStatus) {
        return m_dev->SetClipStatus(pClipStatus);
    }

    HRESULT APIENTRY d3dDevice::SetCurrentTexturePalette(UINT PaletteNumber) {
        return m_dev->SetCurrentTexturePalette(PaletteNumber);
    }

    void APIENTRY d3dDevice::SetCursorPosition(int X, int Y, DWORD Flags) {
        m_dev->SetCursorPosition(X, Y, Flags);
    }

    HRESULT APIENTRY d3dDevice::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap) {
        return m_dev->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
    }

    HRESULT APIENTRY d3dDevice::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
        return m_dev->SetDepthStencilSurface(pNewZStencil);
    }

    HRESULT APIENTRY d3dDevice::SetDialogBoxMode(BOOL bEnableDialogs) {
        return m_dev->SetDialogBoxMode(bEnableDialogs);
    }

    HRESULT APIENTRY d3dDevice::SetFVF(DWORD FVF) {
        return m_dev->SetFVF(FVF);
    }

    void APIENTRY d3dDevice::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp) {
        m_dev->SetGammaRamp(iSwapChain,Flags, pRamp);
    }

    HRESULT APIENTRY d3dDevice::SetIndices(IDirect3DIndexBuffer9* pIndexData) {
        return m_dev->SetIndices(pIndexData);
    }

    HRESULT APIENTRY d3dDevice::SetLight(DWORD Index, CONST D3DLIGHT9 *pLight) {
        return m_dev->SetLight(Index, pLight);
    }

    HRESULT APIENTRY d3dDevice::SetMaterial(CONST D3DMATERIAL9 *pMaterial) {	
        return m_dev->SetMaterial(pMaterial);
    }

    HRESULT APIENTRY d3dDevice::SetNPatchMode(float nSegments) {	
        return m_dev->SetNPatchMode(nSegments);
    }

    HRESULT APIENTRY d3dDevice::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY *pEntries) {
        return m_dev->SetPaletteEntries(PaletteNumber, pEntries);
    }

    HRESULT APIENTRY d3dDevice::SetPixelShader(IDirect3DPixelShader9* pShader) {
        return m_dev->SetPixelShader(pShader);
    }

    HRESULT APIENTRY d3dDevice::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount) {
        return m_dev->SetPixelShaderConstantB(StartRegister,pConstantData, BoolCount);
    }

    HRESULT APIENTRY d3dDevice::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
        return m_dev->SetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);
    }

    HRESULT APIENTRY d3dDevice::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) {
        return m_dev->SetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount);
    }

    HRESULT APIENTRY d3dDevice::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
        return m_dev->SetRenderState(State, Value);
    }

    HRESULT APIENTRY d3dDevice::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
        return m_dev->SetRenderTarget(RenderTargetIndex,pRenderTarget);
    }

    HRESULT APIENTRY d3dDevice::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) {
        return m_dev->SetSamplerState(Sampler,Type,Value);
    }

    HRESULT APIENTRY d3dDevice::SetScissorRect(CONST RECT* pRect) {
        return m_dev->SetScissorRect(pRect);
    }

    HRESULT APIENTRY d3dDevice::SetSoftwareVertexProcessing(BOOL bSoftware) {
        return m_dev->SetSoftwareVertexProcessing(bSoftware);
    }

    HRESULT APIENTRY d3dDevice::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) {
        return m_dev->SetStreamSource(StreamNumber, pStreamData,OffsetInBytes, Stride);
    }

    HRESULT APIENTRY d3dDevice::SetStreamSourceFreq(UINT StreamNumber, UINT Divider) {	
        return m_dev->SetStreamSourceFreq(StreamNumber, Divider);
    }

    HRESULT APIENTRY d3dDevice::SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture) {
        // IDirect3DDevice9 *dev = NULL;
        // if (pTexture != NULL && ((hkIDirect3DTexture9*)(pTexture))->GetDevice(&dev) == D3D_OK) {
        //     if (dev == this)
        //         return m_dev->SetTexture(Stage, ((hkIDirect3DTexture9*)(pTexture))->m_D3Dtex);
        // }
        
        return m_dev->SetTexture(Stage, pTexture);
    }

    HRESULT APIENTRY d3dDevice::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) {
        return m_dev->SetTextureStageState(Stage, Type, Value);
    }

    HRESULT APIENTRY d3dDevice::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix) {
        return m_dev->SetTransform(State, pMatrix);
    }

    HRESULT APIENTRY d3dDevice::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) {
        return m_dev->SetVertexDeclaration(pDecl);
    }

    HRESULT APIENTRY d3dDevice::SetVertexShader(IDirect3DVertexShader9* pShader) {
        return m_dev->SetVertexShader(pShader);
    }

    HRESULT APIENTRY d3dDevice::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount) {
        return m_dev->SetVertexShaderConstantB(StartRegister,pConstantData, BoolCount);
    }

    HRESULT APIENTRY d3dDevice::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
        return m_dev->SetVertexShaderConstantF(StartRegister,pConstantData,Vector4fCount);
    }

    HRESULT APIENTRY d3dDevice::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) {
        return m_dev->SetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount);
    }

    HRESULT APIENTRY d3dDevice::SetViewport(CONST D3DVIEWPORT9 *pViewport) {
        return m_dev->SetViewport(pViewport);
    }

    BOOL APIENTRY d3dDevice::ShowCursor(BOOL bShow) {
        return m_dev->ShowCursor(bShow);
    }

    HRESULT APIENTRY d3dDevice::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) {
        return m_dev->StretchRect(pSourceSurface,pSourceRect,pDestSurface,pDestRect,Filter);
    }

    HRESULT APIENTRY d3dDevice::TestCooperativeLevel() {
        return m_dev->TestCooperativeLevel();
    }

    HRESULT APIENTRY d3dDevice::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) {
        return m_dev->UpdateSurface(pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint);
    }

    HRESULT APIENTRY d3dDevice::UpdateTexture(IDirect3DBaseTexture9 *pSourceTexture, IDirect3DBaseTexture9 *pDestinationTexture) {
        return m_dev->UpdateTexture(pSourceTexture, pDestinationTexture);
    }

    HRESULT APIENTRY d3dDevice::ValidateDevice(DWORD *pNumPasses) {
        return m_dev->ValidateDevice(pNumPasses);
    }



    //
    // Hooks
    //

    extern "C" __declspec(dllexport) IDirect3D9* APIENTRY Direct3DCreate9Hook(UINT SDKVersion) {
        IDirect3D9* out = Direct3DCreate9(SDKVersion);
        gClient::Get()->log("Created Direct3D interface");

        if (out) out = new d3d(out);

        return out;
    }

    void InstallD3DHooks() {
    }

    void UninstallD3DHooks() {
    }
};
class CHook
{
public:
	PVOID pPresentAddress;
	PVOID pResizeBuffersAddress;
	PVOID pSetCursorPosAddress;
	WNDPROC pWndProc;
	void SetupDX11Hook();
	void SetupSetCursorPosHook();
	void SetupWndProcHook();
};
extern std::unique_ptr<CHook>m_pHook;

extern IDXGISwapChain* swapchain;
extern ID3D11Device* device;
extern ID3D11DeviceContext* context;
extern ID3D11RenderTargetView* render_view;

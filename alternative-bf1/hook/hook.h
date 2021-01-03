class CHook
{
public:
	PVOID pPresentAddress;
	PVOID pSetCursorPosAddress;
	WNDPROC pWndProc;
	void SetupDX11Hook();
	void SetupSetCursorPosHook();
	void SetupWndProcHook();
};
extern std::unique_ptr<CHook>m_pHook;

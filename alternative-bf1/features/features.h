struct players_data_s
{
	int m_Index;
	char* m_cszName;
	int m_iTeam;
	ClientVehicleEntity* m_ClientVehicleEntity;
	ClientSoldierEntity* m_ClientSoldierEntity;
};

enum BONE
{
	NONE,
	HEAD,
	NECK,
	SPINE
};

struct extra_players_data_s
{
	int m_Index;
	char* m_cszName;
	int m_iTeam;
	float m_flPlayerHealth;
	float m_flMaxPlayerHealth;
	float m_flVehicleHealth;
	float m_flMaxVehicleHealth;
	bool m_bIsHorse;
	bool m_IsVisible;
	bool m_InVehicle;
	float m_flYaw;
	Vector m_vOrigin;
	Vector m_vBoundBoxMax;
	Vector m_vBoundBoxMin;
	Vector vBoneOrigin[4];
};

enum LINE_STATUS_BAR
{
	LEFT,
	RIGHT,
	UPPER,
	BOTTOM
};

class CFeatures
{
public:	
	void Run();

	void ScreenInfo();

	void Debug();

	void InrecreaseFireRate(bool bIsEnable);

	void NoRecoil(bool bIsEnable);

	void NoOverheatingWeapon(bool bIsEnable);
private:
	bool UpdateLocalPlayer();

	void UpdatePlayers();

	void ClearPlayersData();

	void Aimbot();

	void PlayerESP();

	void DrawScreen(Vector origin, Vector myOrigin, float myYaw, ImColor col);

	ImColor PlayerColor(int iTeam, bool isVehicle, bool isHorse, const bool isVisible);

	void DrawBox(float x, float y, float w, float h, const ImColor col);

	void DrawName(const char* pcszPlayerName, float x, float y, float w, ImColor col);

	void DrawStatusLine(float x, float y, float w, float h, float status_value, float max_of_status_value, ImColor col, LINE_STATUS_BAR status_side = LINE_STATUS_BAR::LEFT);

	void DrawDistance(float x, float y, float w, float h, float distance);

	void Draw3DCircle(Vector vOrigin, ImColor col);

	void DrawSkeleton();

	extra_players_data_s g_Local;
	std::deque<players_data_s>g_Players;
	std::deque<extra_players_data_s>g_ExtraPlayers;
};
extern std::unique_ptr<CFeatures>m_pFeatures;
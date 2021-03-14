namespace vars
{
	extern bool bMenuOpen;
	namespace global
	{
		extern bool enable;
	}
	namespace aimbot
	{
		extern bool enable;
		extern bool only_enemy;
		extern float fov;
		extern float smooth;
		extern bool enable_recoil_compesation;
		extern float recoil_compesation;
		extern int bone;
		extern bool aiming_on_rmb;
		extern bool disable_on_vehicle;
		extern bool aim_from_vehicles_only_horse;
		extern bool no_recoil;
		extern bool increase_fire_rate;
		extern bool weapon_no_overheating;
	}
	namespace visuals
	{
		extern bool enable;
		extern int player_type;
		extern int box_type;
		extern bool name;
		extern bool health;
		extern bool radar_3d;
		extern bool offscreen;
		extern float max_distance_offscreen;
		extern bool distance;
		extern float col_enemy_vis[3];
		extern float col_enemy_invis[3];
		extern float col_teammate_vis[3];
		extern float col_teammate_invis[3];
		extern bool vehicle_esp;
		extern float col_enemy_horse[3];
		extern float col_teammate_horse[3];
		extern float col_enemy_vehicle[3];
		extern float col_teammate_vehicle[3];
	}
	namespace font
	{
		extern int style;
		extern float size;
	}
	namespace extra
	{
		extern bool debug;
	}
	void load_default_settings();
}
#include "../includes.h"

namespace vars
{
	bool bMenuOpen = false;
	namespace global
	{
		bool enable;
	}
	namespace aimbot
	{
		bool enable = false;
		bool only_enemy = false;
		float fov = 0.f;
		float smooth = 0.f;
		bool enable_recoil_compesation = false;
		float recoil_compesation = 0.f;
		int bone = 0;
		bool aiming_on_rmb = false;
		bool disable_on_vehicle = false;
		bool aim_from_vehicles_only_horse = false;
		bool no_recoil = false;
		bool increase_fire_rate = false;
		bool weapon_no_overheating = false;
	}
	namespace visuals
	{
		int player_type = 0;
		int box_type = 0;
		bool name = false;
		bool health = false;
		bool radar_3d = false;
		bool offscreen = false;
		float max_distance_offscreen = 0;
		bool distance = false;
		float col_enemy_vis[3];
		float col_enemy_invis[3];
		float col_teammate_vis[3];
		float col_teammate_invis[3];
		bool vehicle_esp = false;
		float col_enemy_horse[3];
		float col_teammate_horse[3];
		float col_enemy_vehicle[3];
		float col_teammate_vehicle[3];
	}
	namespace font
	{
		int style = 0;
		float size = 0;
	}
	namespace extra
	{
		bool debug = false;
	}
	void load_default_settings() {
		global::enable = true;

		aimbot::enable = true;
		aimbot::only_enemy = true;
		aimbot::fov = 50.f;
		aimbot::smooth = 0.f;
		aimbot::enable_recoil_compesation = true;
		aimbot::recoil_compesation = 1.05f;
		aimbot::bone = 0;
		aimbot::aiming_on_rmb = true;
		aimbot::disable_on_vehicle = true;
		aimbot::aim_from_vehicles_only_horse = true;
		aimbot::no_recoil = false;
		aimbot::increase_fire_rate = false;
		aimbot::weapon_no_overheating = false;

		visuals::player_type = 0;
		visuals::box_type = 4;
		visuals::name = true;
		visuals::health = true;
		visuals::radar_3d = true;
		visuals::offscreen = true;
		visuals::max_distance_offscreen = 100.f;
		visuals::distance = true;

		visuals::col_enemy_vis[0] = 1.f;
		visuals::col_enemy_vis[1] = 1.f;
		visuals::col_enemy_vis[2] = 0.f;
		visuals::col_enemy_invis[0] = 1.f;
		visuals::col_enemy_invis[1] = 0.f;
		visuals::col_enemy_invis[2] = 0.f;

		visuals::col_teammate_vis[0] = 0.f;
		visuals::col_teammate_vis[1] = 1.f;
		visuals::col_teammate_vis[2] = 1.f;
		visuals::col_teammate_invis[0] = 0.f;
		visuals::col_teammate_invis[1] = 0.f;
		visuals::col_teammate_invis[2] = 1.f;
		
		visuals::vehicle_esp = true;
		visuals::col_enemy_horse[0] = 0.f;
		visuals::col_enemy_horse[1] = 1.f;
		visuals::col_enemy_horse[2] = 0.f;
		visuals::col_teammate_horse[0] = 0.5f;
		visuals::col_teammate_horse[1] = 0.5f;
		visuals::col_teammate_horse[2] = 0.1f;
		visuals::col_enemy_vehicle[0] = 1.f;
		visuals::col_enemy_vehicle[1] = 0.f;
		visuals::col_enemy_vehicle[2] = 1.f;
		visuals::col_teammate_vehicle[0] = 0.f;
		visuals::col_teammate_vehicle[1] = 1.f;
		visuals::col_teammate_vehicle[2] = 0.f;

		font::style = 1;

		extra::debug = true;
	}
}
#include "../includes.h"

std::unique_ptr<CFeatures>m_pFeatures = std::make_unique<CFeatures>();

void CFeatures::Run()
{
	if (vars::global::enable == false)
		return;

	if (this->UpdateLocalPlayer() == false)
		return;

	this->UpdatePlayers();

	this->Aimbot();

	this->PlayerESP();

	this->ClearPlayersData();
}

bool CFeatures::UpdateLocalPlayer()
{
	auto* cClientPlayer = GetLocalPlayer();

	if (!IsValidPtr(cClientPlayer))
		return false;

	auto* cClientSoldier = cClientPlayer->clientSoldierEntity;

	if (!IsValidPtr(cClientSoldier))
		return false;

	this->g_Local.m_cszName = cClientPlayer->name;
	this->g_Local.m_IsVisible = !(bool)cClientSoldier->occluded;
	this->g_Local.m_iTeam = cClientPlayer->teamId;
	this->g_Local.m_flYaw = cClientSoldier->authorativeYaw;
	this->g_Local.m_InVehicle = cClientPlayer->InVehicle();
	this->g_Local.m_vOrigin = cClientSoldier->location;
	
	return true;
}

void CFeatures::UpdatePlayers()
{
	for (int i = 0; i <= 64; i++)
	{
		auto* cPlayer = GetPlayerById(i);

		if (!IsValidPtr(cPlayer))
			continue;

		players_data_s player;
		player.m_Index = i;
		player.m_cszName = cPlayer->name;
		player.m_iTeam = cPlayer->teamId;
		player.m_ClientSoldierEntity = cPlayer->clientSoldierEntity;
		player.m_ClientVehicleEntity = cPlayer->clientVehicleEntity;
		this->g_Players.push_back(player);
	}

	for (auto p : this->g_Players)
	{
		auto* cPlayerSoldier = p.m_ClientSoldierEntity;

		if (!IsValidPtr(cPlayerSoldier))
			continue;

		if (cPlayerSoldier->IsDead())
			continue;

		if (cPlayerSoldier->IsValid() == false)
			continue;

		extra_players_data_s player_soldier;
		player_soldier.m_Index = p.m_Index;
		player_soldier.m_cszName = p.m_cszName;
		player_soldier.m_iTeam = p.m_iTeam;
		player_soldier.m_IsVisible = !cPlayerSoldier->occluded;
		player_soldier.m_flPlayerHealth = cPlayerSoldier->healthcomponent->m_Health;
		player_soldier.m_flMaxPlayerHealth = cPlayerSoldier->healthcomponent->m_MaxHealth;
		player_soldier.m_InVehicle = false;
		player_soldier.m_vOrigin = cPlayerSoldier->location;
		player_soldier.m_vBoundBoxMax = cPlayerSoldier->GetAABB().max;
		player_soldier.m_vBoundBoxMin = cPlayerSoldier->GetAABB().min;
		Vector vCurrentBone;
		cPlayerSoldier->GetBonePos(UpdatePoseResultData::BONES::BONE_HEAD, vCurrentBone);
		player_soldier.vBoneOrigin[BONE::HEAD] = vCurrentBone;
		cPlayerSoldier->GetBonePos(UpdatePoseResultData::BONES::BONE_NECK, vCurrentBone);
		player_soldier.vBoneOrigin[BONE::NECK] = vCurrentBone;
		cPlayerSoldier->GetBonePos(UpdatePoseResultData::BONES::BONE_SPINE, vCurrentBone);
		player_soldier.vBoneOrigin[BONE::SPINE] = vCurrentBone;
		this->g_ExtraPlayers.push_back(player_soldier);
	}

	for (auto p : this->g_Players)
	{
		auto* cVehicleSoldier = p.m_ClientVehicleEntity;

		if (!IsValidPtr(cVehicleSoldier))
			continue;

		auto* cEntityData = cVehicleSoldier->GetEntityData();

		if (!IsValidPtr(cEntityData))
			continue;

		if (cVehicleSoldier->IsDead())
			continue;

		extra_players_data_s player_on_vehicle;
		player_on_vehicle.m_Index = p.m_Index;
		player_on_vehicle.m_cszName = p.m_cszName;
		player_on_vehicle.m_iTeam = p.m_iTeam;
		player_on_vehicle.m_IsVisible = true;
		player_on_vehicle.m_flPlayerHealth = cVehicleSoldier->GetHealthComponent()->m_Health;
		player_on_vehicle.m_flMaxPlayerHealth = cVehicleSoldier->GetHealthComponent()->m_MaxHealth;
		player_on_vehicle.m_flVehicleHealth = cVehicleSoldier->GetHealthComponent()->m_VehicleHealth;
		player_on_vehicle.m_flMaxVehicleHealth = cEntityData->m_FrontMaxHealth;
		player_on_vehicle.m_InVehicle = true;
		player_on_vehicle.m_bIsHorse = cVehicleSoldier->IsHorse();
		player_on_vehicle.m_vOrigin = cVehicleSoldier->GetVehiclePosition();
		player_on_vehicle.m_vBoundBoxMin = Vector();
		player_on_vehicle.m_vBoundBoxMax = Vector(0.f, 1.9f, 0.f);
		this->g_ExtraPlayers.push_back(player_on_vehicle);
	}
}

void CFeatures::ClearPlayersData()
{
	this->g_Players.clear();
	this->g_ExtraPlayers.clear();
}

void CFeatures::Debug()
{
	if (vars::extra::debug == false)
		return;

	ImGui::Begin("Debug", nullptr);
	ImGui::Text("Recoil: %.f", get_recoil());
	ImGui::End();
}

void CFeatures::Aimbot()
{
	if (globals::killed_focus)
		return;

	if (vars::aimbot::enable == false)
		return;

	if (vars::bMenuOpen)
		return;

	if (vars::aimbot::disable_on_vehicle && this->g_Local.m_InVehicle) 
		return;	

	float flFov = vars::aimbot::fov * 10.f; //75

	float flAimSpeed = vars::aimbot::smooth + 0.2f;

	int iBone = 0;

	switch (vars::aimbot::bone)
	{
	case 0:
		iBone = BONE::HEAD;
		break;
	case 1:
		iBone = BONE::NECK;
		break;
	case 2:
		iBone = BONE::SPINE;
	default:
		break;
	}

	bool isPressedMLeftButton = GetAsyncKeyState(VK_LBUTTON);
	bool isPressedMRightButton = GetAsyncKeyState(VK_RBUTTON);

	bool event = vars::aimbot::aiming_on_rmb ?
		isPressedMLeftButton == false && isPressedMRightButton == false
		: isPressedMLeftButton == false;

	if (event)
		return;

	int iTarget = INT_MAX;

	float flMaxClosetsToScreenCenter = flFov;

	for (int i = 0; i < this->g_ExtraPlayers.size(); i++)
	{
		if (vars::aimbot::only_enemy && this->g_ExtraPlayers[i].m_iTeam == this->g_Local.m_iTeam)
			continue;

		if (this->g_ExtraPlayers[i].m_InVehicle == false && this->g_ExtraPlayers[i].m_IsVisible == false)
			continue;

		if (vars::aimbot::aim_from_vehicles_only_horse && this->g_ExtraPlayers[i].m_InVehicle && this->g_ExtraPlayers[i].m_bIsHorse == false)
			continue;

		Vector vTarget;

		if (this->g_ExtraPlayers[i].m_InVehicle)
		{
			vTarget = this->g_ExtraPlayers[i].m_vOrigin + this->g_ExtraPlayers[i].m_vBoundBoxMax;
			if (vTarget.Distance(this->g_Local.m_vOrigin) > 100.f)
				continue;
		}
		else
		{
			vTarget = this->g_ExtraPlayers[i].vBoneOrigin[iBone];
		}
		
		float flScreenTarget[2];
		if (WorldToScreen(vTarget, flScreenTarget))
		{
			float flDifferenceFromScreenCenter[2] = {
				abs((ImGui::GetIO().DisplaySize.x / 2) - flScreenTarget[0]),
				abs((ImGui::GetIO().DisplaySize.y / 2) - flScreenTarget[1])
			};

			if (flDifferenceFromScreenCenter[0] <= flFov && flDifferenceFromScreenCenter[1] <= flFov
				&& flDifferenceFromScreenCenter[0] < flMaxClosetsToScreenCenter)
			{
				flMaxClosetsToScreenCenter = flDifferenceFromScreenCenter[0];
				iTarget = i;
			}
		}
	}

	if (iTarget == INT_MAX)
		return;

	if (iBone == BONE::NONE)
		return;

	Vector vTarget;

	if (this->g_ExtraPlayers[iTarget].m_InVehicle)
		vTarget = this->g_ExtraPlayers[iTarget].m_vOrigin + this->g_ExtraPlayers[iTarget].m_vBoundBoxMax;
	else
		vTarget = this->g_ExtraPlayers[iTarget].vBoneOrigin[iBone];

	float flScreenTarget[2];

	if (WorldToScreen(vTarget, flScreenTarget))
	{
		float flScreenCenter[2] = {
			ImGui::GetIO().DisplaySize.x / 2.f,
			ImGui::GetIO().DisplaySize.y / 2.f
		};

		float flTarget[2] = { 0.f, 0.f };

		for (int i = 0; i < 2; i++)
		{
			if (flScreenTarget[i] != 0.f)
			{
				if (flScreenTarget[i] > flScreenCenter[i])
				{
					flTarget[i] = -(flScreenCenter[i] - flScreenTarget[i]);
					flTarget[i] /= flAimSpeed;
					if (flTarget[i] + flScreenCenter[i] > flScreenCenter[i] * 2.f)
						flTarget[i] = 0.f;
				}

				if (flScreenTarget[i] < flScreenCenter[i])
				{
					flTarget[i] = flScreenTarget[i] - flScreenCenter[i];
					flTarget[i] /= flAimSpeed;
					if (flTarget[i] + flScreenCenter[i] < 0.f)
						flTarget[i] = 0.f;
				}
			}

			flTarget[i] /= 10.f;

			if (abs(flTarget[i]) < 1.f)
			{
				if (flTarget[i] > 0.f)
					flTarget[i] = 1.f;
				if (flTarget[i] < 0.f)
					flTarget[i] = -1.f;
			}
		}

		float flRecoil = vTarget.Distance(this->g_Local.m_vOrigin) > 70.f
			? 0.f : vars::aimbot::smooth > 0.f ? get_recoil() * vars::aimbot::recoil_compesation / ((ImClamp(flAimSpeed, 0.2f, 0.5f) + 1.f) * 2.f)
			: get_recoil() * vars::aimbot::recoil_compesation;

		POINT point;
		GetCursorPos(&point);
		point.x += flTarget[0];
		point.y += flTarget[1] + flRecoil;
		SendMessage(globals::hGame, WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));	
	}
}

void CFeatures::PlayerESP()
{
	if (vars::visuals::player_type == 2)
		return;

	for (auto p : this->g_ExtraPlayers)
	{
		if (vars::visuals::player_type == 0 && p.m_iTeam == this->g_Local.m_iTeam)
			continue;

		auto col_box = PlayerColor(p.m_iTeam, p.m_InVehicle, p.m_bIsHorse, p.m_IsVisible);
			
		auto vTop = p.m_vOrigin + p.m_vBoundBoxMax;
		auto vBot = p.m_vOrigin + p.m_vBoundBoxMin;
	
		this->Draw3DCircle(vBot, col_box);

		float flTop[2], flBot[2];
		if (WorldToScreen(vTop, flTop) && WorldToScreen(vBot, flBot))
		{
			float h = flBot[1] - flTop[1];
			float w = h / 2;
			float x = flBot[0] - w / 2;
			float y = flTop[1];

			this->DrawBox(x, y, w, h, col_box);

			this->DrawName(p.m_cszName, x, y, w, col_box);

			if (p.m_InVehicle)
				this->DrawStatusLine(x, y, w, h, p.m_flVehicleHealth, p.m_flMaxVehicleHealth, ImColor(0.f, 0.f, 1.f), LINE_STATUS_BAR::RIGHT);

			this->DrawStatusLine(x, y, w, h, p.m_flPlayerHealth, p.m_flMaxPlayerHealth, ImColor(0.f, 1.f, 0.f), LINE_STATUS_BAR::LEFT);

			this->DrawDistance(x, y, w, h, p.m_vOrigin.Distance(this->g_Local.m_vOrigin));
		}
		else
		{
			this->DrawScreen(p.m_vOrigin, this->g_Local.m_vOrigin, this->g_Local.m_flYaw, col_box);
		}
	}
}

void CFeatures::DrawBox(float x, float y, float w, float h, const ImColor col)
{
	m_pDrawing->DrawEspBox(vars::visuals::box_type, x, y, w, h, col.Value.x, col.Value.y, col.Value.z, col.Value.w);
}

void CFeatures::DrawName(const char *pcszPlayerName, float x, float y, float w, ImColor col)
{
	if (vars::visuals::name == false)
		return;

	if (pcszPlayerName == NULL)
		return;

	ImFont* Font = ImGui::GetIO().Fonts->Fonts[0];
	ImVec2 text_size = Font->CalcTextSizeA(vars::font::size ? vars::font::size : Font->FontSize, FLT_MAX, 0, "");

	m_pDrawing->AddText(x + w / 2.f, y - text_size.y - 2.f, ImColor(1.f, 1.f, 1.f, col.Value.w), vars::font::size, FL_CENTER_X, u8"%s", pcszPlayerName);
}

void CFeatures::DrawStatusLine(float x, float y, float w, float h, float status_value, float max_of_status_value, ImColor col, LINE_STATUS_BAR status_side)
{
	if (status_value <= 0.f)
		return;

	status_value = ImClamp(status_value, 0.f, max_of_status_value);

	const auto size_h = h / max_of_status_value * status_value;
	const auto size_w = w / max_of_status_value * status_value;

	const auto thickness = 2.f;

	switch (status_side)
	{
	case LINE_STATUS_BAR::LEFT:
		m_pDrawing->DrawFillArea(x - thickness - 1.9f, y + h, thickness, -size_h, ImColor(col.Value.x, col.Value.y, col.Value.z, col.Value.w));

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - thickness - 2.9f, y - 1.f, thickness + 2.f, h + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::RIGHT:
		m_pDrawing->DrawFillArea(x + w - thickness + (2.9f * 2.f), y + h, thickness, -size_h, ImColor(col.Value.x, col.Value.y, col.Value.z, col.Value.w));

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x + w - thickness + (2.9f * 2.f) - 0.9f, y - 1.f, thickness + 2.f, h + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::UPPER:
		m_pDrawing->DrawFillArea(x, y - thickness - 1.9f, size_w + 1.9f, thickness, ImColor(col.Value.x, col.Value.y, col.Value.z, col.Value.w));

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - 1.9f, y - thickness - 2.9f, w + 3.9f, thickness + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::BOTTOM:
		m_pDrawing->DrawFillArea(x, y + h + thickness + 0.9f, size_w + 1.9f, thickness, ImColor(col.Value.x, col.Value.y, col.Value.z, col.Value.w));

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - 1.9f, y + h + thickness, w + 3.9f, thickness + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	default: break;
	}
}

void CFeatures::DrawDistance(float x, float y, float w, float h, float distance)
{
	if (vars::visuals::distance == false)
		return;

	m_pDrawing->AddText(x + w / 2.f, y + h + 4.9f, ImColor(1.f, 1.f, 1.f), vars::font::size, FL_CENTER_X, "%.0f", distance);
}

void CFeatures::Draw3DCircle(Vector vOrigin, ImColor col)
{
	if (vars::visuals::radar_3d == false)
		return;

	m_pDrawing->AddCircle3D(vOrigin, 18.f, 0.8f, col);
}

void CFeatures::DrawSkeleton()
{
	//
}

void CFeatures::ScreenInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.1f);
	ImGui::Begin("##info", nullptr, 
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 229.f, 8.f));
	ImGui::SetWindowSize(ImVec2(219.f, 10.f));
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%H:%M:%S", timeinfo);
	ImGui::Text("alternative | ");
	ImGui::SameLine();
	ImGui::Text("%.f fps | ", ImGui::GetIO().Framerate);
	float calc_x = ImGui::CalcTextSize(buffer).x;
	ImGui::SameLine(ImGui::GetWindowWidth() - calc_x - 9.f);
	ImGui::Text(buffer);
	ImGui::End();
	ImGui::PopStyleVar();
}

auto deg_to_rad = [](float x)
{
	return ((float)(x) * (float)(IM_PI / 180.f));
};

auto rad_to_deg = [](float x)
{
	return (float)(x) * (float)(180.f / IM_PI);
};

void VectorAnglesRadar(Vector& forward, Vector& angles)
{
	if (forward.x == 0.f && forward.y == 0.f)
	{
		angles.x = forward.z > 0.f ? -90.f : 90.f;
		angles.y = 0.f;
	}
	else
	{
		angles.x = rad_to_deg(atan2(-forward.z, forward.Length2D()));
		angles.y = rad_to_deg(atan2(forward.y, forward.x));
	}
	angles.z = 0.f;
}

void RotateTriangle(std::array<Vector, 3>& points, float rotation)
{
	const auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
	for (auto& point : points)
	{
		point = point - points_center;

		const auto temp_x = point.x;
		const auto temp_y = point.y;

		const auto theta = deg_to_rad(rotation);
		const auto c = cosf(theta);
		const auto s = sinf(theta);

		point.x = temp_x * c - temp_y * s;
		point.y = temp_x * s + temp_y * c;

		point = point + points_center;
	}
}

Vector RotatePoint(Vector EntityPos, Vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom)
{
	float zs = LocalPlayerPos.z - EntityPos.z;
	float xs = LocalPlayerPos.x - EntityPos.x;

	double Yaw = -(double)angle;

	float single = xs * (float)cos(Yaw) - zs * (float)sin(Yaw);
	float ypisilum1 = xs * (float)sin(Yaw) + zs * (float)cos(Yaw);

	single *= zoom;
	ypisilum1 *= zoom;

	single = single * 2.f;
	single = single + (float)(posX + sizeX / 2);

	ypisilum1 = ypisilum1 * 2.f;
	ypisilum1 = ypisilum1 + (float)(posY + sizeY / 2);

	if (single < (float)posX)
		single = (float)posX;

	if (ypisilum1 < (float)posY)
		ypisilum1 = (float)posY;

	if (single > (float)(posX + sizeX - 3))
		single = (float)(posX + sizeX - 3);

	if (ypisilum1 > (float)(posY + sizeY - 3))
		ypisilum1 = (float)(posY + sizeY - 3);


	return Vector(single, ypisilum1, 0);
}

float Interp(float s1, float s2, float s3, float f1, float f3)
{
	if (s2 == s1) return f1;
	if (s2 == s3) return f3;
	if (s3 == s1) return f1;

	return f1 + ((s2 - s1) / (s3 - s1)) * (f3 - f1);
}

void CFeatures::DrawScreen(Vector origin, Vector myOrigin, float myYaw, ImColor col)
{
	if (vars::visuals::offscreen == false)
		return;

	if (origin.Distance(myOrigin) >= vars::visuals::max_distance_offscreen)
		return;

	float alpha = Interp(0.0f, 
		(vars::visuals::max_distance_offscreen) - origin.Distance(myOrigin),
		10.f, 0.0f, vars::visuals::max_distance_offscreen);

	col.Value.w = alpha / 255.f;

	int radar_range = 45;

	int x, y;

	ImVec2 vSize = ImGui::GetWindowSize();
	ImVec2 vPos = ImGui::GetWindowPos();

	Vector EntityPos = RotatePoint(origin, this->g_Local.m_vOrigin, vPos.x, vPos.y, vSize.x, vSize.y, this->g_Local.m_flYaw, 1.f);

	x = EntityPos.x;
	y = EntityPos.y;

	auto angle = Vector();
	VectorAnglesRadar(Vector((float)(ImGui::GetIO().DisplaySize.x / 2) - x, (float)(ImGui::GetIO().DisplaySize.y / 2) - y, 0.f), angle);

	const auto angle_yaw_rad = deg_to_rad(angle.y + 180.f);
	const auto new_point_x = (ImGui::GetIO().DisplaySize.x / 2) + (radar_range) / 2 * 8 * cosf(angle_yaw_rad);
	const auto new_point_y = (ImGui::GetIO().DisplaySize.y / 2) + (radar_range) / 2 * 8 * sinf(angle_yaw_rad);

	std::array<Vector, 3> points
	{
		Vector(new_point_x - ((90) / 4 + 3.5f) / 2, new_point_y - ((radar_range) / 4 + 3.5f) / 2, 0.f),
		Vector(new_point_x + ((90) / 4 + 3.5f) / 4, new_point_y, 0.f),
		Vector(new_point_x - ((90) / 4 + 3.5f) / 2, new_point_y + ((radar_range) / 4 + 3.5f) / 2, 0.f)
	};

	RotateTriangle(points, angle.y + 180.f);

	m_pDrawing->AddTriangleFilled(ImVec2(points.at(0).x, points.at(0).y), ImVec2(points.at(1).x, points.at(1).y), ImVec2(points.at(2).x, points.at(2).y), col);
}

ImColor CFeatures::PlayerColor(int iTeam, bool isVehicle, bool isHorse, const bool isVisible)
{
	if (iTeam == this->g_Local.m_iTeam)
	{
		if (isVehicle == false)
		{
			if (isVisible)
				return ImColor(vars::visuals::col_teammate_vis[0], vars::visuals::col_teammate_vis[1], vars::visuals::col_teammate_vis[2], 1.f);
			else
				return ImColor(vars::visuals::col_teammate_invis[0], vars::visuals::col_teammate_invis[1], vars::visuals::col_teammate_invis[2], 1.f);
		}
		else
		{
			if (isHorse)
				return ImColor(vars::visuals::col_teammate_horse[0], vars::visuals::col_teammate_horse[1], vars::visuals::col_teammate_horse[2], 1.f);
			else
				return ImColor(vars::visuals::col_teammate_vehicle[0], vars::visuals::col_teammate_vehicle[1], vars::visuals::col_teammate_vehicle[2], 1.f);
		}
	}
	else if (iTeam != this->g_Local.m_iTeam)
	{
		if (isVehicle == false)
		{
			if (isVisible)
				return ImColor(vars::visuals::col_enemy_vis[0], vars::visuals::col_enemy_vis[1], vars::visuals::col_enemy_vis[2], 1.f);
			else
				return ImColor(vars::visuals::col_enemy_invis[0], vars::visuals::col_enemy_invis[1], vars::visuals::col_enemy_invis[2], 1.f);
		}
		else
		{
			if (isHorse)
				return ImColor(vars::visuals::col_enemy_horse[0], vars::visuals::col_enemy_horse[1], vars::visuals::col_enemy_horse[2], 1.f);
			else
				return ImColor(vars::visuals::col_enemy_vehicle[0], vars::visuals::col_enemy_vehicle[1], vars::visuals::col_enemy_vehicle[2], 1.f);
		}
	}
	else
		return ImColor(1.f, 1.f, 1.f);
}

//I'm sorry, but I cannot leave the code of the three lower functions, as they introduce too much disbalance, there are too many people in the game that are running around invisible to other players. 
//I don't want other people to spoil the experience of this cool game.

void CFeatures::InrecreaseFireRate(bool bIsEnable)
{

}

void CFeatures::NoRecoil(bool bIsEnable)
{

}

void CFeatures::NoOverheatingWeapon(bool bIsEnable)
{

}
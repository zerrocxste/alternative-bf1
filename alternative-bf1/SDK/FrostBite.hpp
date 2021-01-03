#ifndef _FROSTBITE_HPP
#define _FROSTBITE_HPP

/* Includes for this header */
#include <Windows.h>
#include "sdk.hpp"

typedef DWORD64 QWORD;
typedef BYTE _BYTE;
typedef WORD _WORD;
typedef DWORD _DWORD;
typedef QWORD _QWORD;

inline hashtable_iterator<_QWORD>* __fastcall hashtable_find(hashtable<_QWORD>* table, hashtable_iterator<_QWORD>* iterator, _QWORD key)
{
	unsigned int mnBucketCount = table->mnBucketCount;

	unsigned int startCount = (unsigned int)(key) % mnBucketCount;

	hash_node<_QWORD>* node = table->mpBucketArray[startCount];

	if (IsValidPtr(node) && node->mValue.first)
	{
		while (key != node->mValue.first)
		{
			node = node->mpNext;
			if (!node || !IsValidPtr(node)
				)
				goto LABEL_4;
		}
		iterator->mpNode = node;
		iterator->mpBucket = &table->mpBucketArray[startCount];
	}
	else
	{
	LABEL_4:
		iterator->mpNode = table->mpBucketArray[mnBucketCount];
		iterator->mpBucket = &table->mpBucketArray[mnBucketCount];
	}
	return iterator;
}

inline void* DecryptPointer(_QWORD EncryptedPtr, _QWORD PointerKey)
{
	_QWORD pObfuscationMgr = *(_QWORD*)OFFSET_OBFUSCATEMGR;

	if (!IsValidPtr(pObfuscationMgr))
		return nullptr;

	if (!(EncryptedPtr & 0x8000000000000000))
		return nullptr; //invalid ptr

	_QWORD hashtableKey = PointerKey ^ *(_QWORD*)(pObfuscationMgr + 0x70);

	hashtable<_QWORD>* table = (hashtable<_QWORD>*)(pObfuscationMgr + 0x38);
	hashtable_iterator<_QWORD> iterator = {};

	hashtable_find(table, &iterator, hashtableKey);
	if (iterator.mpNode == table->mpBucketArray[table->mnBucketCount])
		return nullptr;

	_QWORD EncryptionKey = hashtableKey ^ (_QWORD)(iterator.mpNode->mValue.second);
	EncryptionKey ^= (7 * EncryptionKey);

	_QWORD DecryptedPtr = NULL;
	BYTE* pDecryptedPtrBytes = (BYTE*)&DecryptedPtr;
	BYTE* pEncryptedPtrBytes = (BYTE*)&EncryptedPtr;
	BYTE* pKeyBytes = (BYTE*)&EncryptionKey;

	for (char i = 0; i < 7; i++)
	{
		pDecryptedPtrBytes[i] = (pKeyBytes[i] * 0x7A) ^ (pEncryptedPtrBytes[i] + pKeyBytes[i]);
		EncryptionKey += 8;
	}
	pDecryptedPtrBytes[7] = pEncryptedPtrBytes[7];

	DecryptedPtr &= ~(0x8000000000000000); //to exclude the check bit

	return (void*)DecryptedPtr;
}

inline ClientPlayer* EncryptedPlayerMgr__GetPlayer(QWORD EncryptedPlayerMgr, int id)
{
	_QWORD XorValue1 = *(_QWORD*)(EncryptedPlayerMgr + 0x20) ^ *(_QWORD*)(EncryptedPlayerMgr + 8);
	_QWORD XorValue2 = (XorValue1 ^ *(_QWORD*)(EncryptedPlayerMgr + 0x10));
	if (!IsValidPtr(XorValue2)) return nullptr;
	_QWORD Player = XorValue1 ^ *(_QWORD*)(XorValue2 + 8 * id);

	return (ClientPlayer*)Player;
}

/* Implementation for get player by ID */
inline ClientPlayer* GetPlayerById(int id)
{
	ClientGameContext* pClientGameContext = ClientGameContext::GetInstance();
	if (!IsValidPtr(pClientGameContext)) return nullptr;
	ClientPlayerManager* pPlayerManager = pClientGameContext->m_clientPlayerManager;
	if (!IsValidPtr(pPlayerManager)) return nullptr;

	_QWORD pObfuscationMgr = *(_QWORD*)OFFSET_OBFUSCATEMGR;

	_QWORD PlayerListXorValue = *(_QWORD*)((_QWORD)pPlayerManager + 0xF8);
	_QWORD PlayerListKey = PlayerListXorValue ^ *(_QWORD*)(pObfuscationMgr + 0x70);

	hashtable<_QWORD>* table = (hashtable<_QWORD>*)(pObfuscationMgr + 8);
	hashtable_iterator<_QWORD> iterator = { 0 };

	hashtable_find(table, &iterator, PlayerListKey);
	if (iterator.mpNode == table->mpBucketArray[table->mnBucketCount])
		return nullptr;

	_QWORD EncryptedPlayerMgr = (_QWORD)iterator.mpNode->mValue.second;
	if (!IsValidPtr(EncryptedPlayerMgr)) return nullptr;

	_DWORD MaxPlayerCount = *(_DWORD*)(EncryptedPlayerMgr + 0x18);
	if (MaxPlayerCount != 70 || MaxPlayerCount <= id) return nullptr;

	return EncryptedPlayerMgr__GetPlayer(EncryptedPlayerMgr, id);
}

/* Implementationf for Get LocalPlayer*/
inline ClientPlayer* GetLocalPlayer(void)
{
	ClientGameContext* pClientGameContext = ClientGameContext::GetInstance();
	if (!IsValidPtr(pClientGameContext)) return nullptr;
	ClientPlayerManager* pPlayerManager = pClientGameContext->m_clientPlayerManager;
	if (!IsValidPtr(pPlayerManager)) return nullptr;

	_QWORD pObfuscationMgr = *(_QWORD*)OFFSET_OBFUSCATEMGR;
	if (!IsValidPtr(pObfuscationMgr)) return nullptr;

	_QWORD LocalPlayerListXorValue = *(_QWORD*)((_QWORD)pPlayerManager + 0xF0);
	_QWORD LocalPlayerListKey = LocalPlayerListXorValue ^ *(_QWORD*)(pObfuscationMgr + 0x70);

	hashtable<_QWORD>* table = (hashtable<_QWORD>*)(pObfuscationMgr + 8);
	hashtable_iterator<_QWORD> iterator = { 0 };

	hashtable_find(table, &iterator, LocalPlayerListKey);
	if (iterator.mpNode == table->mpBucketArray[table->mnBucketCount])
		return nullptr;
	if (!IsValidPtr(iterator.mpNode) || iterator.mpNode->mValue.first != LocalPlayerListKey) 
		return nullptr;

	_QWORD EncryptedPlayerMgr = (_QWORD)iterator.mpNode->mValue.second;
	if (!IsValidPtr(EncryptedPlayerMgr)) return nullptr;

	_DWORD MaxPlayerCount = *(_DWORD*)(EncryptedPlayerMgr + 0x18);
	if (MaxPlayerCount != 1) return nullptr;

	return EncryptedPlayerMgr__GetPlayer(EncryptedPlayerMgr, 0);
}

/* Implementation for World to screen */

inline bool W2S(Vector position, float *out)
{
	/* Get the game render instance */
	GameRenderer* game_render = GameRenderer::GetInstance();

	/* Validate the game render instance */
	if (!IsValidPtr(game_render) || !IsValidPtr(game_render->renderView)) return false;

	/* Get the view projection */
	auto view_projection = game_render->renderView->viewProj;

	/* Retrieve the screen size */
	float flScreenCenter[2] = {
		ImGui::GetIO().DisplaySize.x / 2.f,
		ImGui::GetIO().DisplaySize.y / 2.f
	};

	/* Calculate the w component of the vector */
	float w = view_projection.m[0][3] * position.x + view_projection.m[1][3] * position.y + view_projection.m[2][3] * position.z + view_projection.m[3][3];

	/* Check that player isn't behind us */
	if (w < 0.65f) return false;

	/* Calculate the x component of the vector */
	float x = view_projection.m[0][0] * position.x + view_projection.m[1][0] * position.y + view_projection.m[2][0] * position.z + view_projection.m[3][0];

	/* Calculate the y component of the vector */
	float y = view_projection.m[0][1] * position.x + view_projection.m[1][1] * position.y + view_projection.m[2][1] * position.z + view_projection.m[3][1];

	out[0] = flScreenCenter[0] + flScreenCenter[0] * x / w;
	out[1] = flScreenCenter[1] - flScreenCenter[1] * y / w;

	/* Return success */
	return true;
}

inline bool WorldToScreen(const Vector& vIn, float* flOut)
{
	GameRenderer* game_render = GameRenderer::GetInstance();

	if (!IsValidPtr(game_render) || !IsValidPtr(game_render->renderView)) 
		return false;

	auto view_projection = game_render->renderView->m_viewMatrixInverse;

	flOut[0] = view_projection.m[0][0] * vIn.x + view_projection.m[0][1] * vIn.y + view_projection.m[0][2] * vIn.z + view_projection.m[0][3];
	flOut[1] = view_projection.m[1][0] * vIn.x + view_projection.m[1][1] * vIn.y + view_projection.m[1][2] * vIn.z + view_projection.m[1][3];

	float w = view_projection.m[3][0] * vIn.x + view_projection.m[3][1] * vIn.y + view_projection.m[3][2] * vIn.z + view_projection.m[3][3];

	if (w < 0.01)
		return false;

	float invw = 1.0f / w;

	flOut[0] *= invw;
	flOut[1] *= invw;

	int width, height;

	auto io = ImGui::GetIO();
	width = io.DisplaySize.x;
	height = io.DisplaySize.y;

	float x = (float)width / 2;
	float y = (float)height / 2;

	x += 0.5 * flOut[0] * (float)width + 0.5;
	y -= 0.5 * flOut[1] * (float)height + 0.5;

	flOut[0] = x;
	flOut[1] = y;

	return true;
}

inline float CalculateDistance(Vector local_position, Vector entity_position)
{
	float xx = entity_position.x - local_position.x;
	float yy = entity_position.y - local_position.y;
	float zz = entity_position.z - local_position.z;
	return sqrt(xx * xx + yy * yy + zz * zz);
}

inline void ConnectBones(ClientSoldierEntity* entity, UpdatePoseResultData::BONES bone1, UpdatePoseResultData::BONES bone2, ImVec4 col)
{
	/* Check that the entity is valid */
	if (!entity) return;

	Vector bone1_position;
	if (!entity->GetBonePos(bone1, bone1_position))
		return;

	Vector bone2_position{};
	if (!entity->GetBonePos(bone2, bone2_position))
		return;

	Vector bscreen1;
	if (!W2S(bone1_position, bscreen1))
		return;

	Vector bscreen2;
	if (!W2S(bone2_position, bscreen2))
		return;

	//Draw::Line(ImVec2(bscreen1.x, bscreen1.y), ImVec2(bscreen2.x, bscreen2.y), (ImColor)col, 3);
}

inline void DrawSkeleton(ClientSoldierEntity* entity, ImVec4 col)
{
	ConnectBones(entity, UpdatePoseResultData::BONE_HEAD, UpdatePoseResultData::BONE_NECK, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_NECK, UpdatePoseResultData::BONE_SPINE2, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_SPINE2, UpdatePoseResultData::BONE_SPINE1, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_SPINE1, UpdatePoseResultData::BONE_SPINE, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_NECK, UpdatePoseResultData::BONE_LEFTSHOULDER, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_LEFTSHOULDER, UpdatePoseResultData::BONE_LEFTELBOWROLL, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_RIGHTSHOULDER, UpdatePoseResultData::BONE_RIGHTELBOWROLL, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_LEFTELBOWROLL, UpdatePoseResultData::BONE_LEFTHAND, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_RIGHTELBOWROLL, UpdatePoseResultData::BONE_RIGHTHAND, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_SPINE, UpdatePoseResultData::BONE_RIGHTKNEEROLL, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_SPINE, UpdatePoseResultData::BONE_LEFTKNEEROLL, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_RIGHTKNEEROLL, UpdatePoseResultData::BONE_RIGHTFOOT, col);
	ConnectBones(entity, UpdatePoseResultData::BONE_LEFTKNEEROLL, UpdatePoseResultData::BONE_LEFTFOOT, col);
}

#endif // _FROSTBITE_HPP

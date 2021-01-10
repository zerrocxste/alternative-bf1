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

inline bool WorldToScreen(const Vector& vIn, float* flOut)
{
	GameRenderer* game_render = GameRenderer::GetInstance();

	if (!IsValidPtr(game_render) || !IsValidPtr(game_render->renderView)) 
		return false;

	auto view_projection = game_render->renderView->viewProj;

	float w = view_projection.m[0][3] * vIn.x + view_projection.m[1][3] * vIn.y + view_projection.m[2][3] * vIn.z + view_projection.m[3][3];

	if (w < 0.01)
		return false;

	flOut[0] = view_projection.m[0][0] * vIn.x + view_projection.m[1][0] * vIn.y + view_projection.m[2][0] * vIn.z + view_projection.m[3][0];
	flOut[1] = view_projection.m[0][1] * vIn.x + view_projection.m[1][1] * vIn.y + view_projection.m[2][1] * vIn.z + view_projection.m[3][1];

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

#endif // _FROSTBITE_HPP

#ifndef _SDK_HPP
#define _SDK_HPP

#define OFFSET_CLIENTGAMECONTEXT 0x1437F7758
#define OFFSET_GAMERENDERER 0x1439e6d08
#define OFFSET_OBFUSCATEMGR 0x14351D058
#define OFFSET_DXRENDERER 0x1439E75F8

/* Check if the ptr parameter is valid */
#define IsValidPtr(x) ( x != NULL && (DWORD_PTR)x >= 0x10000 && (DWORD_PTR)x < 0x00007FFFFFFEFFFF)

bool WorldToScreen(const Vector& vIn, float* flOut);

template<typename T1, typename T2>
struct pair
{
	T1 first;
	T2 second;
};
template<typename T>
struct hash_node
{
	pair<uint64_t, T*> mValue;
	hash_node<T>* mpNext;
};

template<typename T>
struct hashtable
{
	uint64_t vtable;
	hash_node<T>** mpBucketArray;
	unsigned int mnBucketCount;
	unsigned int mnElementCount;
	//... some other stuff
};

template<typename T>
struct hashtable_iterator
{
	hash_node<T>* mpNode;
	hash_node<T>** mpBucket;
};

struct AxisAlignedBox {
	Vector min;
	Vector max;
	AxisAlignedBox::AxisAlignedBox() {}
};

struct LinearTransform_AABB {
public:
	AxisAlignedBox m_Box;
	Matrix4x4 pad;
	LinearTransform_AABB::LinearTransform_AABB() {}
};

class ClientPlayerManager {
public:
};
class ClientGameContext {
public:
	char pad_0x0000[0x68]; //0x0000
	ClientPlayerManager* m_clientPlayerManager; //0x0068
	static ClientGameContext* GetInstance() {
		return *(ClientGameContext**)(OFFSET_CLIENTGAMECONTEXT);
	}
};

class HealthComponent {
public:
	char pad_0000[32]; //0x0000
	float m_Health; //0x0020
	float m_MaxHealth; //0x0024
	char pad_0028[24]; //0x0028
	float m_VehicleHealth; //0x0040
	char pad_0044[4092]; //0x0044
}; //Size: 0x1040

class VehicleEntityData {
public:
	char pad_0000[504]; //0x0000
	float m_FrontMaxHealth; //0x01F8
	char pad_01FC[252]; //0x01FC
	char* m_namesid; //0x02F8
	char pad_0300[7488]; //0x0300
}; //Size: 0x2040

Vector Mat3Vector(Matrix4x4 mat, Vector vec);

class ClientVehicleEntity {
public:
	virtual void Function0(); //
	virtual void Function1(); //
	virtual void Function2(); //
	virtual void Function3(); //
	virtual void Function4(); //
	virtual void Function5(); //
	virtual void Function6(); //
	virtual void Function7(); //
	virtual void Function8(); //
	virtual void Function9(); //
	virtual void Function10(); //
	virtual void Function11(); //
	virtual void Function12(); //
	virtual void Function13(); //
	virtual void Function14(); //
	virtual void Function15(); //
	virtual void Function16(); //
	virtual void Function17(); //
	virtual void Function18(); //
	virtual void Function19(); //
	virtual void Function20(); //
	virtual void Function21(); //
	virtual void Function22(); //
	virtual void Function23(); //
	virtual void Function24();
	virtual void GetTransformAABB(LinearTransform_AABB& mTransform); // 26
	virtual void GetTransform(Matrix4x4* mTransform);
	HealthComponent* GetHealthComponent() {
		return *(HealthComponent**)((BYTE*)this + 0x1D0);
	};

	VehicleEntityData* GetEntityData() {
		return *(VehicleEntityData**)((BYTE*)this + 0x30);
	};

	Vector GetVehiclePosition()
	{
		LinearTransform_AABB _AABB;
		Matrix4x4 _Transform;
		GetTransform(&_Transform);
		Vector Position = Vector(_Transform.m[3][0], _Transform.m[3][1], _Transform.m[3][2]);
		Vector glmPos = Vector(Position.x, Position.y, Position.z);

		GetTransformAABB(_AABB);
		Vector min = Vector(_AABB.m_Box.min.x, _AABB.m_Box.min.y, _AABB.m_Box.min.z);
		Vector max = Vector(_AABB.m_Box.max.x, _AABB.m_Box.max.y, _AABB.m_Box.max.z);

		Matrix4x4 TransformMatrix{ _Transform.m[0][0], _Transform.m[1][0], _Transform.m[2][0], 0,
									_Transform.m[0][1], _Transform.m[1][1], _Transform.m[2][1], 0,
										_Transform.m[0][2], _Transform.m[1][2], _Transform.m[2][2], 0 };


		min = glmPos + Mat3Vector(TransformMatrix, min);
		max = glmPos + Mat3Vector(TransformMatrix, max);

		return Vector(min.x, min.y, min.z);
	}

	bool IsDead() {
		return !(this->GetHealthComponent()->m_VehicleHealth > 0.1f);
	}

	bool IsHorse()
	{
		return this->GetHealthComponent()->m_Health > 0.1f;
	}

}; //Size: 0x0048

class QuatTransform {
public:
	Vector4 m_TransAndScale; //0x0000
	Vector4 m_Rotation; //0x0010
}; //Size=0x0020

class UpdatePoseResultData {
public:
	enum BONES {
		BONE_HEAD = 0x35,
		BONE_NECK = 0x33,
		BONE_SPINE2 = 0x7,
		BONE_SPINE1 = 0x6,
		BONE_SPINE = 0x5,
		BONE_LEFTSHOULDER = 0x8,
		BONE_RIGHTSHOULDER = 0xA3,
		BONE_LEFTELBOWROLL = 0xE,
		BONE_RIGHTELBOWROLL = 0xA9,
		BONE_LEFTHAND = 0x10,
		BONE_RIGHTHAND = 0xAB,
		BONE_LEFTKNEEROLL = 0x11F,
		BONE_RIGHTKNEEROLL = 0x12D,
		BONE_LEFTFOOT = 0x115,
		BONE_RIGHTFOOT = 0x123
	};

	char pad_0000[24]; //0x0000
	class QuatTransform* m_ActiveWorldTransforms; //0x0020
	char pad_0020[4128]; //0x0020
}; //Size: 0x1040

class BoneCollisionComponent {
public:
	UpdatePoseResultData m_ragdollTransforms; //0x0000
}; //Size: 0x0008

class ClientSoldierEntity {
public:
	char pad_0000[464]; //0x0000
	HealthComponent* healthcomponent; //0x01D0
	char pad_01D8[696]; //0x01D8
	BoneCollisionComponent* bonecollisioncomponent; //0x0490
	char pad_0498[363]; //0x0498
	uint8_t N00000670; //0x0603
	float authorativeYaw; //0x0604
	char pad_0608[41]; //0x0608
	uint8_t N00000521; //0x0631
	char pad_0632[6]; //0x0632
	uint8_t poseType; //0x0638
	char pad_0639[176]; //0x0639
	uint8_t N00000538; //0x06E9
	uint8_t N0000022B; //0x06EA
	uint8_t occluded; //0x06EB
	char pad_06EC[669]; //0x06EC
	uint8_t N0000058C; //0x0989
	char pad_098A[6]; //0x098A
	Vector location; //0x0990
	char pad_099C[1712]; //0x099C

	bool IsValid() {
		return (this->healthcomponent->m_Health > 0.1f && this->healthcomponent->m_Health <= this->healthcomponent->m_MaxHealth);
	}
	bool IsDead() {
		return !(this->healthcomponent->m_Health > 0.1f);
	}
	bool IsVisible() {
		return (this->occluded == false);
	}
	AxisAlignedBox GetAABB() {
		AxisAlignedBox aabb = AxisAlignedBox();

		if (this->poseType == 0) // standing
		{
			aabb.min = Vector(0.f, 0.f, 0.f);
			aabb.max = Vector(0.f, 1.700000f, 0.f);
		}
		if (this->poseType == 1) // crouching
		{
			aabb.min = Vector(0.f, 0.f, 0.f);
			aabb.max = Vector(0.f, 1.150000f, 0.f);
		}
		if (this->poseType == 2) // prone
		{
			aabb.min = Vector(0.f, 0.f, 0.f);
			aabb.max = Vector(0.f, 0.400000f, 0.f);
		}
		return aabb;
	}
	bool GetBonePos(int BoneId, Vector& vOut);

}; //Size: 0x104C

class ClientPlayer {
public:
	virtual~ClientPlayer();
	virtual DWORD_PTR GetCharacterEntity(); //=> ClientSoldierEntity + 0x268
	virtual DWORD_PTR GetCharacterUserData(); //=> PlayerCharacterUserData
	virtual class EntryComponent* GetEntryComponent();
	virtual bool InVehicle();
	virtual unsigned int getId();
	char _0x0008[16];
	char* name; //0x0018
	char pad_0020[32]; //0x0020
	char szName[8]; //0x0040
	char pad_0048[7144]; //0x0048
	uint8_t N00000393; //0x1C30
	uint8_t N0000042C; //0x1C31
	char pad_1C32[2]; //0x1C32
	uint8_t teamId; //0x1C34
	char pad_1C35[259]; //0x1C35
	ClientVehicleEntity* clientVehicleEntity; //0x1D38
	char pad_1D40[8]; //0x1D40
	ClientSoldierEntity* clientSoldierEntity; //0x1D48
	char pad_1D50[736]; //0x1D50
};

class RenderView {
public:
	char pad_0x0000[0x320]; //0x0000
	Matrix4x4 m_viewMatrixInverse; //0x0320
	char pad_0x0360[0x100]; //0x0360
	Matrix4x4 viewProj; //0x0460
	char pad_0x04A0[0x28]; //0x04A0
}; //Size: 0x05C0

class GameRenderer {
public:
	char pad_0000[96]; //0x0000
	class RenderView* renderView; //0x0060
	char pad_0068[4112]; //0x0068

	static GameRenderer* GetInstance() {
		return *(GameRenderer**)OFFSET_GAMERENDERER;
	}
}; //Size: 0x0088

enum BoneIds {
	BONE_Head = 0x35,
	BONE_Neck = 0x33,
	BONE_SPINE2 = 0x7,
	BONE_SPINE1 = 0x6,
	BONE_Spine = 0x5,
	BONE_LeftShoulder = 0x8,
	BONE_RightShoulder = 0xA3,
	BONE_LeftElbowRoll = 0xE,
	BONE_RightElbowRoll = 0xA9,
	BONE_LeftHand = 0x10,
	BONE_RightHand = 0xAB,
	BONE_RightKneeRoll = 0x11F,
	BONE_LeftKneeRoll = 0x12D,
	BONE_LeftFoot = 0x115,
	BONE_RightFoot = 0x123
};

class DxRenderer;
class Screen;

class DxRenderer {
public:
	char pad_0x0000[0x810]; //0x0000
	__int32 m_frameCounter; //0x0810
	__int32 m_framesInProgress; //0x0814
	__int32 m_framesInProgress2; //0x0818
	unsigned char m_isActive; //0x081C
	char pad_0x081D[0x3]; //0x081D
	Screen* pScreen; //0x0820
	char pad_0x0828[0x78]; //0x0828
	ID3D11Device* m_pDevice; //0x08A0
	ID3D11DeviceContext* m_pContext; //0x08A8
	char pad_0x08B0[0x68]; //0x08B0
	char* m_AdapterName; //0x0918
	char pad_0x0920[0x28]; //0x0920

	static DxRenderer* GetInstance() {
		return *(DxRenderer**)(OFFSET_DXRENDERER);
	}
};

class Screen {
public:
	char pad_0x0000[0x5F]; //0x0000
	unsigned char m_isTopWindow; //0x005F
	unsigned char m_isMinimized; //0x0060
	unsigned char m_isMaximized; //0x0061
	unsigned char m_isResizing; //0x0062
	char pad_0x0063[0x5]; //0x0063
	__int32 m_width; //0x0068
	__int32 m_height; //0x006C
	char pad_0x0070[0x210]; //0x0070
	IDXGISwapChain* m_swapChain; //0x0280
	char pad_0x0288[0xB8]; //0x0288
}; //Size=0x0340

#endif // _SDK_HPP

float get_recoil();
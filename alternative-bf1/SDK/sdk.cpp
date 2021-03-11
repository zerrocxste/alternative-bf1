#include "../includes.h"

bool ClientSoldierEntity::GetBonePos(int BoneId, Vector& vOut)
{
	BoneCollisionComponent* pBoneCollisionComponent = this->bonecollisioncomponent;

	if (!IsValidPtr(pBoneCollisionComponent))
		return false;

	QuatTransform* pQuat = pBoneCollisionComponent->m_ragdollTransforms.m_ActiveWorldTransforms;

	if (!IsValidPtr(pQuat))
		return false;

	vOut.x = pQuat[BoneId].m_TransAndScale.x;
	vOut.y = pQuat[BoneId].m_TransAndScale.y;
	vOut.z = pQuat[BoneId].m_TransAndScale.z;

	return true;
}

Vector Mat3Vector(Matrix4x4 mat, Vector vec)
{
	return Vector
	(
		mat._11 * vec.x + mat._12 * vec.x + mat._13 * vec.x,
		mat._21 * vec.y + mat._22 * vec.y + mat._23 * vec.y,
		mat._31 * vec.z + mat._32 * vec.z + mat._33 * vec.z
	);
}

float get_recoil()
{
	return memory_utils::read<float>( { memory_utils::get_base_address(), 0x0351C150, 0x20, 0x3A8, 0x18, 0x138, 0x100, 0x0, 0x74 } );
}
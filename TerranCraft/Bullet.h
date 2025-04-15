#pragma once

#include "../Common/typedef.h"
#include "Flingy.h"
#include "../BWLib/WeaponType.h"
#include "../BWLib/FlingyType.h"

class Unit;

class Bullet final : public Flingy
{
public:
	Bullet() = default;
	virtual ~Bullet();
	bool Initialize(BW::WeaponType weaponType, Unit* sourceUnit);
	void Cleanup();

	BW::WeaponType GetWeaponType() const { return mWeaponType; }
	uint8 GetTimeRemaining() const { return mTimeRemaining; }
	uint8 GetFlags() const { return mFlags; }
	Unit* GetSourceUnit() const { return mSourceUnit; }


private:
	BW::WeaponType mWeaponType = BW::WeaponType::None;
	uint8 mTimeRemaining = 0;
	uint8 mFlags = 0; // 0x01 = ��� ��ħ, 0x02 = ���� ����(hallucination)
	//uint8 mRemaingBounces; // ��Ż����ũ ����(���� ��Ī: ubChain)�� �ٿ Ƚ��
	Unit* mSourceUnit = nullptr;
	//Unit* mNextBouncedUnit; .// ���� �ٿ ����, temporary
	//uint8 mSpreadSeed; // ������ �ֱ��� ���� ī����(��Ű��, �񸮾� ������ ���)
};

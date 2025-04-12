#pragma once

#include "../Common/typedef.h"
#include "../BWLib/WeaponType.h"

class Unit;

class Bullet final
{
public:

private:
	BW::WeaponType mWeaponType;
	uint8 mTimeRemaining;
	uint8 mFlags; // 0x01 = ��� ��ħ, 0x02 = ���� ����(hallucination)
	//uint8 mRemaingBounces; // ��Ż����ũ ����(���� ��Ī: ubChain)�� �ٿ Ƚ��
	Unit* mSourceUnit;
	//Unit* mNextBouncedUnit; .// ���� �ٿ ����, temporary
	//uint8 mSpreadSeed; // ������ �ֱ��� ���� ī����(��Ű��, �񸮾� ������ ���)
};

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
	uint8 mFlags = 0; // 0x01 = 대상 놓침, 0x02 = 피해 없음(hallucination)
	//uint8 mRemaingBounces; // 뮤탈리스크 무기(공식 명칭: ubChain)의 바운스 횟수
	Unit* mSourceUnit = nullptr;
	//Unit* mNextBouncedUnit; .// 다음 바운스 단위, temporary
	//uint8 mSpreadSeed; // 일종의 주기적 폭발 카운터(발키리, 골리앗 공습에 사용)
};

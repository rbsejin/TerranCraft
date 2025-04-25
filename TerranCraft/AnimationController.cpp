#include "pch.h"

#include "AnimationController.h"
#include "Image.h"
#include "Sprite.h"
#include "Unit.h"
#include "ScriptCodeType.h"
#include "Game.h"

AnimationController::~AnimationController()
{
	Destroy();
}

bool AnimationController::Load(const char* filename)
{
	bool bResult = false;

	FILE* fp = nullptr;

	fopen_s(&fp, filename, "rb");

	if (fp == nullptr)
	{
		goto LB_RETURN;
	}

	fseek(fp, 0, SEEK_END);

	int32 fileSize = ftell(fp);

	if (fileSize == 0)
	{
		goto LB_FILE_CLOSE;
	}

	rewind(fp);

	delete[] mData;
	mData = new uint8[fileSize];
	fread(mData, fileSize, 1, fp);

	uint16 entryListOffset = *(uint16*)mData;
	IndexEntry* indexEntry = (IndexEntry*)(mData + entryListOffset);

	while (indexEntry->ID != 0xFFFF)
	{
		uint8* p = mData + indexEntry->Offset;

		// magic 4byte + type 4byte + header + script
		uint32 magic = *(uint32*)p;
		p += 4;

#ifdef _DEBUG
		if (magic != 0x45504353)
		{
			__debugbreak();
		}
#endif // _DEBUG

		uint32 type = *(uint32*)p;
		p += 4;

		uint32 headerOffsetCount = sHeaderOffsetCounts[type];

		IScriptEntry* entry = mIScriptEntries + indexEntry->ID;
		entry->Header = indexEntry->Offset;
		entry->Type = type;

		uint16* pOffset = entry->Offsets;

		for (uint32 i = 0; i < headerOffsetCount; i++)
		{
			*pOffset++ = *(uint16*)p;
			p += 2;
		}

		indexEntry++;
	}

	bResult = true;

LB_FILE_CLOSE:
	fclose(fp);

LB_RETURN:
	return bResult;
}

void AnimationController::Destroy()
{
	delete[] mData;
	mData = nullptr;
}

void printOpcode(eImage imageNumber, const char* opcode)
{
	if (imageNumber != eImage::CursorMarker)
	{
		return;
	}

	char buffer[256];
	sprintf_s(buffer, "ImageNumber: %d, Opcode: %s\n", (int)imageNumber, opcode);
	//OutputDebugStringA(buffer);
}

void AnimationController::UpdateImageFrame(Thingy* thingy, Image* image) const
{
	int32 sleep = image->GetSleep();

	while (sleep == 0)
	{
		eImage imageNumber = image->GetImageID();

		uint8 opcode;
		uint16 iscriptOffset = image->GetIScriptOffset();
		GetData(&opcode, iscriptOffset, sizeof(opcode));
		iscriptOffset += sizeof(opcode);

		switch ((eScriptCode)opcode)
		{
		case eScriptCode::PlayFram:
		{
			uint16 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			uint8 direction = image->GetDirection();
			int32 d = direction;
			if (d > 0x10)
			{
				d = 0x20 - d;
			}
			frame += d;
			image->UpdateFrameIndex(frame);


			printOpcode(imageNumber, "PLAYFRAM");
		}
		break;
		case eScriptCode::PlayFramTile:
		{
			uint16 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			// TODO: 추후 구현

			printOpcode(imageNumber, "PLAYFRAMTILE");
		}
		break;
		case eScriptCode::SetHorPos:
		{
			uint8 x;
			GetData(&x, iscriptOffset, sizeof(x));

			iscriptOffset += sizeof(x);
		}
		break;
		case eScriptCode::SetVertPos:
		{
			uint8 y;
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);
			image->SetOffsets({ 0, y });

			printOpcode(imageNumber, "SETVERTPOS");
		}
		break;
		case eScriptCode::SetPos:
		{
			uint8 x;
			uint8 y;
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);
			image->SetOffsets({ x, y });

			printOpcode(imageNumber, "SETPOS");
		}
		break;
		case eScriptCode::Wait:
		{
			uint8 gameTicks;
			GetData(&gameTicks, iscriptOffset, sizeof(gameTicks));
			iscriptOffset += sizeof(gameTicks);
			sleep = gameTicks;

			printOpcode(imageNumber, "WAIT");
		}
		break;
		case eScriptCode::WaitRand:
		{
			uint8 gameTicks1;
			uint8 gameTicks2;
			GetData(&gameTicks1, iscriptOffset, sizeof(gameTicks1));
			iscriptOffset += sizeof(gameTicks1);
			GetData(&gameTicks2, iscriptOffset, sizeof(gameTicks2));
			iscriptOffset += sizeof(gameTicks2);

			sleep = (rand() % 2 == 0) ? gameTicks1 : gameTicks2;

			printOpcode(imageNumber, "WAITRAND");
		}
		break;
		case eScriptCode::Goto:
		{
			uint16 fileOffset;
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			iscriptOffset = fileOffset;

			printOpcode(imageNumber, "GOTO");
		}
		break;
		case eScriptCode::ImgOl:
		{
			uint16 imageID;
			uint8 x;
			uint8 y;
			GetData(&imageID, iscriptOffset, sizeof(imageID));
			iscriptOffset += sizeof(imageID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			// TODO: glow effect 렌더링 구현되면 추가
			Sprite* parent = image->GetParent();
			Image* child = new Image();
			child->Initialize((eImage)imageID, parent);
			child->SetOffsets({ x, y });
			parent->AddAffter(child);

			printOpcode(imageNumber, "IMGOL");
		}
		break;
		case eScriptCode::ImgUl:
		{
			uint16 imageID;
			uint8 x;
			uint8 y;
			GetData(&imageID, iscriptOffset, sizeof(imageID));
			iscriptOffset += sizeof(imageID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			//// TODO: shadow 렌더링 구현되면 추가
			Sprite* parent = image->GetParent();
			Image* child = new Image();
			child->Initialize((eImage)imageID, parent);
			child->SetOffsets({ x, y });
			parent->AddBefore(child);

			printOpcode(imageNumber, "IMGUL");
		}
		break;
		case eScriptCode::ImgOlOrig:
		{
			uint16 imageID;
			GetData(&imageID, iscriptOffset, sizeof(imageID));
			iscriptOffset += sizeof(imageID);

			printOpcode(imageNumber, "IMGOLORIG");
		}
		break;
		case eScriptCode::SwitchUl:
		{
			uint16 imageID;
			GetData(&imageID, iscriptOffset, sizeof(imageID));
			iscriptOffset += sizeof(imageID);

			printOpcode(imageNumber, "SWITCHUL");
		}
		break;
		case eScriptCode::_0C:
			printOpcode(imageNumber, "__0C");
			break;
		case eScriptCode::ImgOlUseLo:
		{
			uint16 imageID;
			uint8 x;
			uint8 y;
			GetData(&imageID, iscriptOffset, sizeof(imageID));
			iscriptOffset += sizeof(imageID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			printOpcode(imageNumber, "IMGOLUSELO");
		}
		break;
		case eScriptCode::ImgUlUseLo:
		{
			uint16 imageID;
			uint8 x;
			uint8 y;
			GetData(&imageID, iscriptOffset, sizeof(imageID));
			iscriptOffset += sizeof(imageID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			printOpcode(imageNumber, "IMGULUSELO");
		}
		break;
		case eScriptCode::SprOl:
		{
			uint16 spriteID;
			uint8 x;
			uint8 y;
			GetData(&spriteID, iscriptOffset, sizeof(spriteID));
			iscriptOffset += sizeof(spriteID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			Thingy* newThingy = new Thingy();
			newThingy->Initialize(0, (eSprite)spriteID);
			Sprite* sprite = newThingy->GetSprite();
			Sprite* parent = image->GetParent();
			Int32Vector2 position = parent->GetPosition();
			position.X += x;
			position.Y += y;
			sprite->SetPosition(position);
			gGame->Thingies.push_back(newThingy);

			printOpcode(imageNumber, "SPROL");
		}
		break;
		case eScriptCode::HighSprOl:
		{
			uint16 spriteID;
			uint8 x;
			uint8 y;
			GetData(&spriteID, iscriptOffset, sizeof(spriteID));
			iscriptOffset += sizeof(spriteID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			printOpcode(imageNumber, "HIGHSPROL");
		}
		break;
		case eScriptCode::LowSprUl:
		{
			uint16 spriteID;
			uint8 x;
			uint8 y;
			GetData(&spriteID, iscriptOffset, sizeof(spriteID));
			iscriptOffset += sizeof(spriteID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			Thingy* newThingy = new Thingy();
			newThingy->Initialize(0, (eSprite)spriteID);
			Sprite* sprite = newThingy->GetSprite();
			Sprite* parent = image->GetParent();
			Int32Vector2 position = parent->GetPosition();
			position.X += x;
			position.Y += y;
			sprite->SetPosition(position);
			gGame->Thingies.push_back(newThingy);

			printOpcode(imageNumber, "LOWSPRUL");
		}
		break;
		case eScriptCode::UfLUnstable:
		{
			uint16 flingyID;
			GetData(&flingyID, iscriptOffset, sizeof(flingyID));
			iscriptOffset += sizeof(flingyID);

			printOpcode(imageNumber, "UFLUNSTABLE");
		}
		break;
		case eScriptCode::SprUlUseLo:
		{
			uint16 spriteID;
			GetData(&spriteID, iscriptOffset, sizeof(spriteID));
			iscriptOffset += sizeof(spriteID);

			printOpcode(imageNumber, "SPRULUSELO");
		}
		break;
		case eScriptCode::SprUl:
		{
			uint16 spriteID;
			uint8 x;
			uint8 y;
			GetData(&spriteID, iscriptOffset, sizeof(spriteID));
			iscriptOffset += sizeof(spriteID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			printOpcode(imageNumber, "SPRUL");
		}
		break;
		case eScriptCode::SprOlUseLo:
		{
			uint16 spriteID;
			uint8 overlay;
			GetData(&spriteID, iscriptOffset, sizeof(spriteID));
			iscriptOffset += sizeof(spriteID);
			GetData(&overlay, iscriptOffset, sizeof(overlay));
			iscriptOffset += sizeof(overlay);

			printOpcode(imageNumber, "SPROLUSELO");
		}
		break;
		case eScriptCode::End:
		{
			Sprite* parent = image->GetParent();
			parent->RemoveImage(image);
			delete image;
			image = nullptr;

			uint32 imageCount = parent->GetImageCount();

			if (imageCount == 0)
			{
				if (thingy != nullptr)
				{
					thingy->SetSprite(nullptr);
				}

				delete parent;
				parent = nullptr;
			}

			printOpcode(imageNumber, "END");
			goto LB_RETURN;
		}
		break;
		case eScriptCode::SetFlipState:
		{
			uint8 flipState;
			GetData(&flipState, iscriptOffset, sizeof(flipState));
			iscriptOffset += sizeof(flipState);

			printOpcode(imageNumber, "SETFLIPSTATE");
		}
		break;
		case eScriptCode::PlaySnd:
		{
			uint16 soundID;
			GetData(&soundID, iscriptOffset, sizeof(soundID));
			iscriptOffset += sizeof(soundID);

			printOpcode(imageNumber, "PLAYSND");
		}
		break;
		case eScriptCode::PlaySndRand:
		{
			uint8 ofSounds;
			uint16 soundID;
			uint16 soundID2;
			// ...
			GetData(&ofSounds, iscriptOffset, sizeof(ofSounds));
			iscriptOffset += sizeof(ofSounds);
			GetData(&soundID, iscriptOffset, sizeof(soundID));
			iscriptOffset += sizeof(soundID);
			GetData(&soundID2, iscriptOffset, sizeof(soundID2));
			iscriptOffset += sizeof(soundID2);

			printOpcode(imageNumber, "PLAYSNDRAND");
		}
		break;
		case eScriptCode::PlaySndBtwn:
		{
			uint16 soundID;
			uint16 soundID2;
			GetData(&soundID, iscriptOffset, sizeof(soundID));
			iscriptOffset += sizeof(soundID);
			GetData(&soundID2, iscriptOffset, sizeof(soundID2));
			iscriptOffset += sizeof(soundID2);

			printOpcode(imageNumber, "PLAYSNDBTWN");
		}
		break;
		case eScriptCode::DoMissileDmg:
			printOpcode(imageNumber, "DOMISSILEDMG");
			break;
		case eScriptCode::AttackMelee:
		{
			uint8 ofSounds;
			uint16 soundID;
			uint16 soundID2;
			GetData(&ofSounds, iscriptOffset, sizeof(ofSounds));
			iscriptOffset += sizeof(ofSounds);
			GetData(&soundID, iscriptOffset, sizeof(soundID));
			iscriptOffset += sizeof(soundID);
			GetData(&soundID2, iscriptOffset, sizeof(soundID2));
			iscriptOffset += sizeof(soundID2);

			printOpcode(imageNumber, "ATTACKMELEE");
		}
		break;
		case eScriptCode::FollowMainGraphic:
		{
			Sprite* parent = image->GetParent();
			const Image* primaryImage = parent->GetPrimaryImage();

#ifdef _DEBUG
			if (primaryImage == nullptr)
			{
				__debugbreak();
			}
#endif // _DEBUG

			int16 frameSet = primaryImage->GetFrameSet();
			image->SetFrameSet(frameSet);

			int8 direction = primaryImage->GetDirection();
			image->SetDirection(direction);

			//uint32 frameIndex = primaryImage->GetFrameIndex();
			//image->UpdateFrameIndex(frameIndex);

			printOpcode(imageNumber, "FOLLOWMAINGRAPHIC");
		}
		break;
		case eScriptCode::RandCondJmp:
		{
			uint8 chance;
			uint16 fileOffset;
			GetData(&chance, iscriptOffset, sizeof(chance));
			iscriptOffset += sizeof(chance);
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			uint8 random = rand() % 256;
			if (random < chance)
			{
				iscriptOffset = fileOffset;
			}

			printOpcode(imageNumber, "RANDCONDJMP");

			char buffer[256];
			sprintf_s(buffer, "\tChance: %d, Random: %d\n", chance, random);
			//OutputDebugStringA(buffer);
		}
		break;
		case eScriptCode::TurnCCWise:
		{
			uint8 amount;
			GetData(&amount, iscriptOffset, sizeof(amount));
			iscriptOffset += sizeof(amount);

			uint8 direction = image->GetDirection();
			uint8 d = (direction - amount + 32) % 32;
			image->SetDirection(d);

			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetFacingDirection(d);
			}

			printOpcode(imageNumber, "TURNCCWISE");
		}
		break;
		case eScriptCode::TurnCWise:
		{
			uint8 amount;
			GetData(&amount, iscriptOffset, sizeof(amount));
			iscriptOffset += sizeof(amount);

			uint8 direction = image->GetDirection();
			uint8 d = (direction + amount + 32) % 32;
			image->SetDirection(d);

			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetFacingDirection(d);
			}

			printOpcode(imageNumber, "TURNCWISE");
		}
		break;
		case eScriptCode::Turn1CWise:
		{
			uint8 direction = image->GetDirection();
			uint8 d = (direction + 1) % 32;
			image->SetDirection(d);

			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetFacingDirection(d);
			}

			printOpcode(imageNumber, "TURN1CWISE");
		}
		break;
		case eScriptCode::TurnRand:
		{
			uint8 amount;
			GetData(&amount, iscriptOffset, sizeof(amount));
			iscriptOffset += sizeof(amount);

			uint8 random = rand() % 4;

			int32 randAmount = amount;

			if (random == 0)
			{
				randAmount = -randAmount;
			}

			uint8 direction = image->GetDirection();
			uint8 d = (direction + randAmount + 32) % 32;
			image->SetDirection(d);

			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetFacingDirection(d);
			}

			printOpcode(imageNumber, "TURNRAND");
		}
		break;
		case eScriptCode::SetSpawnFrame:
		{
			uint16 direction;
			GetData(&direction, iscriptOffset, sizeof(direction));
			iscriptOffset += sizeof(direction);

			printOpcode(imageNumber, "SETSPAWNFRAME");
		}
		break;
		case eScriptCode::SigOrder:
		{
			uint8 signal;
			GetData(&signal, iscriptOffset, sizeof(signal));
			iscriptOffset += sizeof(signal);

			printOpcode(imageNumber, "SIGORDER");
		}
		break;
		case eScriptCode::AttackWith:
		{
			uint8 weaponType;
			GetData(&weaponType, iscriptOffset, sizeof(weaponType));
			iscriptOffset += sizeof(weaponType);

			printOpcode(imageNumber, "ATTACKWITH");
		}
		break;
		case eScriptCode::Attack:
			printOpcode(imageNumber, "ATTACK");
			break;
		case eScriptCode::CastSpell:
			printOpcode(imageNumber, "CASTSPELL");
			break;
		case eScriptCode::UseWeapon:
		{
			uint8 weaponID;
			GetData(&weaponID, iscriptOffset, sizeof(weaponID));
			iscriptOffset += sizeof(weaponID);

			printOpcode(imageNumber, "USEWEAPON");
		}
		break;
		case eScriptCode::Move:
		{
			uint8 pixels;
			GetData(&pixels, iscriptOffset, sizeof(pixels));
			iscriptOffset += sizeof(pixels);

			printOpcode(imageNumber, "MOVE");
		}
		break;
		case eScriptCode::GotoRepeatAttk:
		{
			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetRepeatAttackable(true);
			}

			printOpcode(imageNumber, "GOTOREPEATATTK");
		}
		break;
		case eScriptCode::EngFrame:
		{
			uint8 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			printOpcode(imageNumber, "ENGFRAME");
		}
		break;
		case eScriptCode::EngSet:
		{
			uint8 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			// Plays a particular frame set, often used in engine glow animations.
			// TODO: 추후 구현

			printOpcode(imageNumber, "ENGSET");
		}
		break;
		case eScriptCode::_2D:
		{
			image->SetHidden(true);
			printOpcode(imageNumber, "__2D");
		}
		break;
		case eScriptCode::NoBrkCodeStart:
		{
			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetNobrkcode(true);
			}
			printOpcode(imageNumber, "NOBRKCODESTART");
		}
		break;
		case eScriptCode::NoBrkCodeEnd:
		{
			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetNobrkcode(false);
			}
			printOpcode(imageNumber, "NOBRKCODEEND");
		}
		break;
		case eScriptCode::IgnoreRest:
		{
			iscriptOffset -= sizeof(opcode);
			printOpcode(imageNumber, "IGNOREREST");
			goto LB_OUTER_LOOP;
		}
		break;
		case eScriptCode::AttkShiftProj:
			printOpcode(imageNumber, "ATTKSHIFTPROJ");
			break;
		case eScriptCode::TmpRmGraphicStart:
		{
			image->SetHidden(true);
			printOpcode(imageNumber, "TMPRMGRAPHICSTART");
		}
		break;
		case eScriptCode::TmpRmGraphicEnd:
		{
			image->SetHidden(false);
			printOpcode(imageNumber, "TMPRMGRAPHICEND");
		}
		break;
		case eScriptCode::SetFlDirect:
		{
			uint8 direction;
			GetData(&direction, iscriptOffset, sizeof(direction));
			iscriptOffset += sizeof(direction);

			image->SetDirection(direction);

			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetFacingDirection(direction);
			}

			printOpcode(imageNumber, "SETFLDIRECT");
		}
		break;
		case eScriptCode::Call:
		{
			uint16 fileOffset;
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "CALL");
		}
		break;
		case eScriptCode::Return:
			printOpcode(imageNumber, "RETURN");
			break;
		case eScriptCode::SetFlSpeed:
		{
			uint16 speed;
			GetData(&speed, iscriptOffset, sizeof(speed));
			iscriptOffset += sizeof(speed);

			printOpcode(imageNumber, "SETFLSPEED");
		}
		break;
		case eScriptCode::CreateGasOverlays:
		{
			uint8 overlay;
			GetData(&overlay, iscriptOffset, sizeof(overlay));
			iscriptOffset += sizeof(overlay);

			printOpcode(imageNumber, "CREATEGASOVERLAYS");
		}
		break;
		case eScriptCode::PwrUpCondJmp:
		{
			uint16 fileOffset;
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "PWRUPCONDJMP");
		}
		break;
		case eScriptCode::TrgtRangeCondJmp:
		{
			uint16 distance;
			uint16 fileOffset;
			GetData(&distance, iscriptOffset, sizeof(distance));
			iscriptOffset += sizeof(distance);
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "TRGTRANGECONDJMP");
		}
		break;
		case eScriptCode::TrgtArcCondJmp:
		{
			uint16 angle1;
			uint16 angle2;
			uint16 fileOffset;
			GetData(&angle1, iscriptOffset, sizeof(angle1));
			iscriptOffset += sizeof(angle1);
			GetData(&angle2, iscriptOffset, sizeof(angle2));
			iscriptOffset += sizeof(angle2);
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "TRGTARCCONDJMP");
		}
		break;
		case eScriptCode::CurDirectCondJmp:
		{
			uint16 angle1;
			uint16 angle2;
			uint16 fileOffset;
			GetData(&angle1, iscriptOffset, sizeof(angle1));
			iscriptOffset += sizeof(angle1);
			GetData(&angle2, iscriptOffset, sizeof(angle2));
			iscriptOffset += sizeof(angle2);
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "CURDIRECTCONDJMP");
		}
		break;
		case eScriptCode::ImgUlNextId:
		{
			uint8 x;
			uint8 y;
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			printOpcode(imageNumber, "IMGULNEXTID");
		}
		break;
		case eScriptCode::_3E:
			printOpcode(imageNumber, "__3E");
			break;
		case eScriptCode::LiftOffCondJmp:
		{
			uint16 fileOffset;
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "LIFTOFFCONDJMP");
		}
		break;
		case eScriptCode::WarpOverlay:
		{
			uint16 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			printOpcode(imageNumber, "WARPOVERLAY");
		}
		break;
		case eScriptCode::OrderDone:
		{
			uint8 signal;
			GetData(&signal, iscriptOffset, sizeof(signal));
			iscriptOffset += sizeof(signal);

			printOpcode(imageNumber, "ORDERDONE");
		}
		break;
		case eScriptCode::GrdSprOl:
		{
			uint16 spriteID;
			uint8 x;
			uint8 y;
			GetData(&spriteID, iscriptOffset, sizeof(spriteID));
			iscriptOffset += sizeof(spriteID);
			GetData(&x, iscriptOffset, sizeof(x));
			iscriptOffset += sizeof(x);
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);

			printOpcode(imageNumber, "GRDSPROL");
		}
		break;
		case eScriptCode::_43:
			printOpcode(imageNumber, "__43");
			break;
		case eScriptCode::DoGrdDamage:
			printOpcode(imageNumber, "DOGRDDAMAGE");
			break;
		default:
#ifdef _DEBUG
			__debugbreak();
#endif // _DEBUG

			break;
		}

		image->SetIScriptOffset(iscriptOffset);
	}

LB_OUTER_LOOP:
	sleep--;
	if (sleep < 0)
	{
		sleep = 0;
	}
	image->SetSleep(sleep);

LB_RETURN:
	return;
}

uint16 AnimationController::GetIScriptHeader(uint32 entryNumber) const
{
	return mIScriptEntries[entryNumber].Header;
}

uint16 AnimationController::GetIScriptOffset(uint16 headerOffset, eAnim animation) const
{
	uint16* offsets = (uint16*)(mData + headerOffset + 8);
	return offsets[(uint32)animation];
}

void AnimationController::GetData(void* dest, uint16 offset, uint32 size) const
{
	memcpy(dest, mData + offset, size);
}

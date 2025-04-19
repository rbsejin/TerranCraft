#include "pch.h"

#include "AnimationController.h"
#include "Image.h"
#include "Sprite.h"
#include "Unit.h"
#include "IScriptCode.h"
#include "Game.h"

AnimationController AnimationController::Instance;

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

void printOpcode(BW::ImageNumber imageNumber, const char* opcode)
{
	if (imageNumber != BW::ImageNumber::Cursor_Marker)
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
		BW::ImageNumber imageNumber = image->GetImageID();

		uint8 opcode;
		uint16 iscriptOffset = image->GetIScriptOffset();
		GetData(&opcode, iscriptOffset, sizeof(opcode));
		iscriptOffset += sizeof(opcode);

		switch ((IScriptOpcode)opcode)
		{
		case IScriptOpcode::PLAYFRAM:
		{
			uint16 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			uint16 frameSet = frame / 0x11;
			image->SetFrameSet(frameSet);

			uint8 direction = image->GetDirection();
			uint8 d = frame % 0x11;
			if (d != 0)
			{
				direction = d;
			}
			image->SetDirection(direction);

			printOpcode(imageNumber, "PLAYFRAM");
		}
		break;
		case IScriptOpcode::PLAYFRAMTILE:
		{
			uint16 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			// TODO: 추후 구현

			printOpcode(imageNumber, "PLAYFRAMTILE");
		}
		break;
		case IScriptOpcode::SETHORPOS:
		{
			uint8 x;
			GetData(&x, iscriptOffset, sizeof(x));

			iscriptOffset += sizeof(x);
		}
		break;
		case IScriptOpcode::SETVERTPOS:
		{
			uint8 y;
			GetData(&y, iscriptOffset, sizeof(y));
			iscriptOffset += sizeof(y);
			image->SetOffsets({ 0, y });

			printOpcode(imageNumber, "SETVERTPOS");
		}
		break;
		case IScriptOpcode::SETPOS:
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
		case IScriptOpcode::WAIT:
		{
			uint8 gameTicks;
			GetData(&gameTicks, iscriptOffset, sizeof(gameTicks));
			iscriptOffset += sizeof(gameTicks);
			sleep = gameTicks;
			//image->SetSleep(sleep);

			printOpcode(imageNumber, "WAIT");

			char buffer[256];
			sprintf_s(buffer, "\tGameTicks: %d\n", gameTicks);
			//OutputDebugStringA(buffer);
		}
		break;
		case IScriptOpcode::WAITRAND:
		{
			uint8 gameTicks1;
			uint8 gameTicks2;
			GetData(&gameTicks1, iscriptOffset, sizeof(gameTicks1));
			iscriptOffset += sizeof(gameTicks1);
			GetData(&gameTicks2, iscriptOffset, sizeof(gameTicks2));
			iscriptOffset += sizeof(gameTicks2);

			sleep = (rand() % 2 == 0) ? gameTicks1 : gameTicks2;
			//image->SetSleep(sleep);

			printOpcode(imageNumber, "WAITRAND");
		}
		break;
		case IScriptOpcode::GOTO:
		{
			uint16 fileOffset;
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			iscriptOffset = fileOffset;

			printOpcode(imageNumber, "GOTO");
		}
		break;
		case IScriptOpcode::IMGOL:
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
#if 1
			Sprite* parent = image->GetParent();
			Image* child = new Image();
			child->Initialize((BW::ImageNumber)imageID, parent);
			child->SetOffsets({ x, y });
			BW::IScriptAnimation anim = image->GetAnim();
			child->SetAnim(anim);
			parent->AddBefore(child);
#endif

			printOpcode(imageNumber, "IMGOL");
		}
		break;
		case IScriptOpcode::IMGUL:
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

			// TODO: shadow 렌더링 구현되면 추가
#if 0
			Sprite* parent = image->GetParent();
			Image* child = new Image();
			child->Initialize((BW::ImageNumber)imageID, parent);
			child->SetOffsets({ x, y });
			BW::IScriptAnimation anim = image->GetAnim();
			child->SetAnim(anim);
			parent->AddAffter(child);
#endif
			printOpcode(imageNumber, "IMGUL");
		}
		break;
		case IScriptOpcode::IMGOLORIG:
		{
			uint16 imageID;
			GetData(&imageID, iscriptOffset, sizeof(imageID));
			iscriptOffset += sizeof(imageID);

			printOpcode(imageNumber, "IMGOLORIG");
		}
		break;
		case IScriptOpcode::SWITCHUL:
		{
			uint16 imageID;
			GetData(&imageID, iscriptOffset, sizeof(imageID));
			iscriptOffset += sizeof(imageID);

			printOpcode(imageNumber, "SWITCHUL");
		}
		break;
		case IScriptOpcode::__0C:
			printOpcode(imageNumber, "__0C");
			break;
		case IScriptOpcode::IMGOLUSELO:
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
		case IScriptOpcode::IMGULUSELO:
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
		case IScriptOpcode::SPROL:
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
			newThingy->Initialize(1, (BW::SpriteNumber)spriteID);
			Sprite* sprite = newThingy->GetSprite();
			Sprite* parent = image->GetParent();
			IntVector2 position = parent->GetPosition();
			position.X += x;
			position.Y += y;
			sprite->SetPosition(position);
			Game::sThingies.push_back(newThingy);

			printOpcode(imageNumber, "SPROL");
		}
		break;
		case IScriptOpcode::HIGHSPROL:
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
		case IScriptOpcode::LOWSPRUL:
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
			newThingy->Initialize(1, (BW::SpriteNumber)spriteID);
			Sprite* sprite = newThingy->GetSprite();
			Sprite* parent = image->GetParent();
			IntVector2 position = parent->GetPosition();
			position.X += x;
			position.Y += y;
			sprite->SetPosition(position);
			Game::sThingies.push_back(newThingy);

			printOpcode(imageNumber, "LOWSPRUL");
		}
		break;
		case IScriptOpcode::UFLUNSTABLE:
		{
			uint16 flingyID;
			GetData(&flingyID, iscriptOffset, sizeof(flingyID));
			iscriptOffset += sizeof(flingyID);

			printOpcode(imageNumber, "UFLUNSTABLE");
		}
		break;
		case IScriptOpcode::SPRULUSELO:
		{
			uint16 spriteID;
			GetData(&spriteID, iscriptOffset, sizeof(spriteID));
			iscriptOffset += sizeof(spriteID);

			printOpcode(imageNumber, "SPRULUSELO");
		}
		break;
		case IScriptOpcode::SPRUL:
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
		case IScriptOpcode::SPROLUSELO:
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
		case IScriptOpcode::END:
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
		case IScriptOpcode::SETFLIPSTATE:
		{
			uint8 flipState;
			GetData(&flipState, iscriptOffset, sizeof(flipState));
			iscriptOffset += sizeof(flipState);

			printOpcode(imageNumber, "SETFLIPSTATE");
		}
		break;
		case IScriptOpcode::PLAYSND:
		{
			uint16 soundID;
			GetData(&soundID, iscriptOffset, sizeof(soundID));
			iscriptOffset += sizeof(soundID);

			printOpcode(imageNumber, "PLAYSND");
		}
		break;
		case IScriptOpcode::PLAYSNDRAND:
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
		case IScriptOpcode::PLAYSNDBTWN:
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
		case IScriptOpcode::DOMISSILEDMG:
			printOpcode(imageNumber, "DOMISSILEDMG");
			break;
		case IScriptOpcode::ATTACKMELEE:
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
		case IScriptOpcode::FOLLOWMAINGRAPHIC:
		{
			Sprite* parent = image->GetParent();
			const Image* primaryImage = parent->GetPrimaryImage();

#ifdef _DEBUG
			if (primaryImage == nullptr)
			{
				__debugbreak();
			}
#endif // _DEBUG

			int8 direction = primaryImage->GetDirection();
			image->SetDirection(direction);

			int16 frameSet = primaryImage->GetFrameSet();
			image->SetFrameSet(frameSet);

			image->UpdateFrameIndex();

			printOpcode(imageNumber, "FOLLOWMAINGRAPHIC");
		}
		break;
		case IScriptOpcode::RANDCONDJMP:
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
		case IScriptOpcode::TURNCCWISE:
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
		case IScriptOpcode::TURNCWISE:
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
		case IScriptOpcode::TURN1CWISE:
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
		case IScriptOpcode::TURNRAND:
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
		case IScriptOpcode::SETSPAWNFRAME:
		{
			uint16 direction;
			GetData(&direction, iscriptOffset, sizeof(direction));
			iscriptOffset += sizeof(direction);

			printOpcode(imageNumber, "SETSPAWNFRAME");
		}
		break;
		case IScriptOpcode::SIGORDER:
		{
			uint8 signal;
			GetData(&signal, iscriptOffset, sizeof(signal));
			iscriptOffset += sizeof(signal);

			printOpcode(imageNumber, "SIGORDER");
		}
		break;
		case IScriptOpcode::ATTACKWITH:
		{
			uint8 weaponType;
			GetData(&weaponType, iscriptOffset, sizeof(weaponType));
			iscriptOffset += sizeof(weaponType);

			printOpcode(imageNumber, "ATTACKWITH");
		}
		break;
		case IScriptOpcode::ATTACK:
			printOpcode(imageNumber, "ATTACK");
			break;
		case IScriptOpcode::CASTSPELL:
			printOpcode(imageNumber, "CASTSPELL");
			break;
		case IScriptOpcode::USEWEAPON:
		{
			uint8 weaponID;
			GetData(&weaponID, iscriptOffset, sizeof(weaponID));
			iscriptOffset += sizeof(weaponID);

			printOpcode(imageNumber, "USEWEAPON");
		}
		break;
		case IScriptOpcode::MOVE:
		{
			uint8 pixels;
			GetData(&pixels, iscriptOffset, sizeof(pixels));
			iscriptOffset += sizeof(pixels);

			printOpcode(imageNumber, "MOVE");
		}
		break;
		case IScriptOpcode::GOTOREPEATATTK:
		{
			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetRepeatAttackable(true);
			}

			printOpcode(imageNumber, "GOTOREPEATATTK");
		}
		break;
		case IScriptOpcode::ENGFRAME:
		{
			uint8 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			printOpcode(imageNumber, "ENGFRAME");
		}
		break;
		case IScriptOpcode::ENGSET:
		{
			uint8 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			// Plays a particular frame set, often used in engine glow animations.
			// TODO: 추후 구현

			printOpcode(imageNumber, "ENGSET");
		}
		break;
		case IScriptOpcode::__2D:
		{
			image->SetHidden(true);
			printOpcode(imageNumber, "__2D");
		}
		break;
		case IScriptOpcode::NOBRKCODESTART:
		{
			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetNobrkcode(true);
			}
			printOpcode(imageNumber, "NOBRKCODESTART");
		}
		break;
		case IScriptOpcode::NOBRKCODEEND:
		{
			if (thingy != nullptr)
			{
				Flingy* flingy = (Flingy*)thingy;
				flingy->SetNobrkcode(false);
			}
			printOpcode(imageNumber, "NOBRKCODEEND");
		}
		break;
		case IScriptOpcode::IGNOREREST:
		{
			iscriptOffset -= sizeof(opcode);
			printOpcode(imageNumber, "IGNOREREST");
			goto LB_OUTER_LOOP;
		}
		break;
		case IScriptOpcode::ATTKSHIFTPROJ:
			printOpcode(imageNumber, "ATTKSHIFTPROJ");
			break;
		case IScriptOpcode::TMPRMGRAPHICSTART:
		{
			image->SetHidden(true);
			printOpcode(imageNumber, "TMPRMGRAPHICSTART");
		}
		break;
		case IScriptOpcode::TMPRMGRAPHICEND:
		{
			image->SetHidden(false);
			printOpcode(imageNumber, "TMPRMGRAPHICEND");
		}
		break;
		case IScriptOpcode::SETFLDIRECT:
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
		case IScriptOpcode::CALL:
		{
			uint16 fileOffset;
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "CALL");
		}
		break;
		case IScriptOpcode::RETURN:
			printOpcode(imageNumber, "RETURN");
			break;
		case IScriptOpcode::SETFLSPEED:
		{
			uint16 speed;
			GetData(&speed, iscriptOffset, sizeof(speed));
			iscriptOffset += sizeof(speed);

			printOpcode(imageNumber, "SETFLSPEED");
		}
		break;
		case IScriptOpcode::CREATEGASOVERLAYS:
		{
			uint8 overlay;
			GetData(&overlay, iscriptOffset, sizeof(overlay));
			iscriptOffset += sizeof(overlay);

			printOpcode(imageNumber, "CREATEGASOVERLAYS");
		}
		break;
		case IScriptOpcode::PWRUPCONDJMP:
		{
			uint16 fileOffset;
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "PWRUPCONDJMP");
		}
		break;
		case IScriptOpcode::TRGTRANGECONDJMP:
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
		case IScriptOpcode::TRGTARCCONDJMP:
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
		case IScriptOpcode::CURDIRECTCONDJMP:
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
		case IScriptOpcode::IMGULNEXTID:
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
		case IScriptOpcode::__3E:
			printOpcode(imageNumber, "__3E");
			break;
		case IScriptOpcode::LIFTOFFCONDJMP:
		{
			uint16 fileOffset;
			GetData(&fileOffset, iscriptOffset, sizeof(fileOffset));
			iscriptOffset += sizeof(fileOffset);

			printOpcode(imageNumber, "LIFTOFFCONDJMP");
		}
		break;
		case IScriptOpcode::WARPOVERLAY:
		{
			uint16 frame;
			GetData(&frame, iscriptOffset, sizeof(frame));
			iscriptOffset += sizeof(frame);

			printOpcode(imageNumber, "WARPOVERLAY");
		}
		break;
		case IScriptOpcode::ORDERDONE:
		{
			uint8 signal;
			GetData(&signal, iscriptOffset, sizeof(signal));
			iscriptOffset += sizeof(signal);

			printOpcode(imageNumber, "ORDERDONE");
		}
		break;
		case IScriptOpcode::GRDSPROL:
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
		case IScriptOpcode::__43:
			printOpcode(imageNumber, "__43");
			break;
		case IScriptOpcode::DOGRDDAMAGE:
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

uint16 AnimationController::GetIScriptOffset(uint16 headerOffset, BW::IScriptAnimation animation) const
{
	uint16* offsets = (uint16*)(mData + headerOffset + 8);
	return offsets[(uint32)animation];
}

void AnimationController::GetData(void* dest, uint16 offset, uint32 size) const
{
	memcpy(dest, mData + offset, size);
}

// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Demonstration handwriting class
// 
//

#if !defined(__HNDLODR_H__)
#define __HNDLODR_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__W32STD_H__)
#include "W32STD.H"
#endif
#if !defined(__HANDCMD_H__)
#include "HANDCMD.H"
#endif


class RHandWritingAnim : public RAnim
	{
public:
	inline RHandWritingAnim(RAnimDll& aDll) :RAnim(aDll) {}
	TInt Construct(const RWsSprite& aDevice);
	void Activate();
	void Deactivate();
	TInt SpriteMaskChange(TBool aUsingSeparateMask);
	void SetDrawData(const THandwritingDrawData& aDrawData);
	TInt GetLastGeneratedCharacter();
	};

class CHandWriting : public CBase
	{
public:
	CHandWriting(RWsSession& aSession);
	void ConstructL(TSize aScreenSize,RWindowGroup& aGroup,TBool aUseSeparateMask);
	~CHandWriting();
	void SetMaskL(TBool aUseSeparateMask);
	void ToggleStatus();
private:
	void CreateSpriteL(TSize aScreenSize,RWindowGroup& aGroup,TBool aUseSeparateMask);
	void LoadDllL();
	void FillInSpriteMember(TSpriteMember& aMember);
private:
	RWsSession& iSession;
	RAnimDll iAnimDll;
	RHandWritingAnim iAnim;
	RWsSprite iSprite;
	CFbsBitmap *iBitmap;
	CFbsBitmap *iMaskBitmap;
	TBool iActive;
	};

#endif

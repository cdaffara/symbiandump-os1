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

#include "HNDLODR.H"


/*RHandWritingAnim*/

TInt RHandWritingAnim::Construct(const RWsSprite& aDevice)
	{
	TPtrC8 des(NULL,0);
	return RAnim::Construct(aDevice,0,des);
	}

void RHandWritingAnim::Activate()
	{
	Command(EHwOpActivate);
	}

void RHandWritingAnim::Deactivate()
	{
	Command(EHwOpDeactivate);
	}

TInt RHandWritingAnim::SpriteMaskChange(TBool aUsingSeparateMask)
	{
	TPckgBuf<TBool> param;
	param()=aUsingSeparateMask;
	return CommandReply(EHwOpSpriteMask,param);
	}

void RHandWritingAnim::SetDrawData(const THandwritingDrawData& aDrawData)
	{
	TPckgBuf<THandwritingDrawData> param;
	param()=aDrawData;
	Command(EHwOpSetDrawData,param);
	}

TInt RHandWritingAnim::GetLastGeneratedCharacter()
	{
	return CommandReply(EHwOpGetLastChar);
	}


/*CHandWriting*/

CHandWriting::CHandWriting(RWsSession& aSession)
	:iSession(aSession), iAnimDll(aSession), iAnim(iAnimDll), iSprite(aSession)
	{}

void CHandWriting::ConstructL(TSize aScreenSize,RWindowGroup& aGroup,TBool aUseSeparateMask)
	//
	//This function should be called with the correct screen size and 
	//the handle of any group window that will be around as long as the handwriting is required
	//
	{
	CreateSpriteL(aScreenSize,aGroup,aUseSeparateMask);
	LoadDllL();
	}

void CHandWriting::CreateSpriteL(TSize aScreenSize,RWindowGroup& aGroup,TBool aUseSeparateMask)
	{
	TInt color,gray;		//Unused variables
	TDisplayMode mode=iSession.GetDefModeMaxNumColors(color,gray);
	iBitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmap->Create(aScreenSize,mode));
	TSpriteMember member;
	member.iMaskBitmap=iBitmap;
	if (aUseSeparateMask)
		{
		iMaskBitmap=new(ELeave) CFbsBitmap();
		User::LeaveIfError(iMaskBitmap->Create(aScreenSize,mode));
		member.iMaskBitmap=iMaskBitmap;
		}
	User::LeaveIfError(iSprite.Construct(aGroup,TPoint(),ESpriteNoChildClip|ESpriteNoShadows));
	FillInSpriteMember(member);
	iSprite.AppendMember(member);
	}

void CHandWriting::LoadDllL()
	{
	_LIT(DllName,"HandAnim.DLL");
	TInt err=iAnimDll.Load(DllName);
	if (err==KErrNone)
		err=iAnim.Construct(iSprite);
	if (err==KErrNone)
		{
		iAnim.Activate();
		iActive=ETrue;
		}
	User::LeaveIfError(err);
	}

CHandWriting::~CHandWriting()
	{
	delete iBitmap;
	delete iMaskBitmap;
	iSprite.Close();
	iAnimDll.Close();
	}

void CHandWriting::SetMaskL(TBool aUseSeparateMask)
	{
	if ((iMaskBitmap!=NULL)==aUseSeparateMask)
		return;
	TSpriteMember member;
	FillInSpriteMember(member);
	if (aUseSeparateMask)
		{
		iMaskBitmap=new(ELeave) CFbsBitmap();
		member.iMaskBitmap=iMaskBitmap;
		}
	else
		{
		member.iMaskBitmap=iBitmap;
		}
	if (iActive)
		iAnim.Deactivate();
	iSprite.UpdateMember(1,member);
	TInt err=iAnim.SpriteMaskChange(aUseSeparateMask);
	if (err<0)
		{
		if (aUseSeparateMask)
			{
			delete iMaskBitmap;
			iMaskBitmap=NULL;
			member.iMaskBitmap=iBitmap;
			}
		else
			{
			member.iMaskBitmap=iMaskBitmap;
			}
		iSprite.UpdateMember(1,member);
		}
	else if (!aUseSeparateMask)
		{
		delete iMaskBitmap;
		iMaskBitmap=NULL;
		}
	if (iActive)
		iAnim.Activate();
	User::LeaveIfError(err);
	}

void CHandWriting::ToggleStatus()
	{
	if (iActive)
		iAnim.Deactivate();
	else
		iAnim.Activate();
	iActive=!iActive;
	}

void CHandWriting::FillInSpriteMember(TSpriteMember& aMember)
	{
	aMember.iBitmap=iBitmap;
	aMember.iInvertMask=ETrue;		//Must be inverted
	aMember.iDrawMode=CGraphicsContext::EDrawModePEN;		//Ignored when using mask
	aMember.iOffset=TPoint();		//Must be 0,0
	aMember.iInterval=0;		//Not used as only one TSpriteMember in sprite
	}

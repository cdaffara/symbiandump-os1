// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __SPRITEANIMDLL_H__
#define __SPRITEANIMDLL_H__


#if !defined(__W32ADLL_H__)
#include <w32adll.h>
#endif

_LIT(KAnimDLLName,"SPRITEANIMDLL.DLL");

enum AnimDllOpcodesSprite
	{
	EADllDraw1,
	EADllDraw2,
	EADllDraw3,
	EADllDrawBlank,
	};


enum AnimType
	{
	ESpriteAnimType
	};

class RTAnim : public RAnim
	{
public:
	RTAnim();
	RTAnim(RAnimDll& aDll);
 	inline TInt Construct(const RWsSprite &aSprite,TInt aType,const TDesC8 &aParams) {return(RAnim::Construct(aSprite,aType,aParams));}
 	inline void Command(TInt aOpcode) {RAnim::Command(aOpcode);}
	};

class CTAnimDll : public CAnimDll
	{
public:
	CAnim *CreateInstanceL(TInt aType);
private:
	};

class CTSpriteAnim : public CSpriteAnim
	{
public:
	virtual void ConstructL(TAny *aArgs);
	
//Pure virtual functions from CAnim	
	virtual void Animate(TDateTime *aDateTime);
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);

//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	void DrawL(TInt aOpcode);
	};	

#endif

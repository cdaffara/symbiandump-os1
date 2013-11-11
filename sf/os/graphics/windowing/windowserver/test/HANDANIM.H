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

#if !defined(__HANDANIM_H__)
#define __HANDANIM_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__GDI_H__)
#include <gdi.h>
#endif
#if !defined(__W32ADLL_H__)
#include <w32adll.h>
#endif
#if !defined(__HANDCMD_H__)
#include "HANDCMD.H"
#endif


class CPointStore : public CBase
	{
public:
	CPointStore();
	void ConstructL();
	void AddPoint(TPoint aPoint);
	TInt GetChar();
	void ClearPoints();
private:
	CArrayFixFlat<TPoint> *iPoints;
	TInt iNumPoints;
	};

class CHandWritingAnimDll : public CAnimDll 
	{
public:
	CAnim *CreateInstanceL(TInt aType);
private:
	};

class CHandWritingAnim : public CSpriteAnim
	{
public:
	enum TState
		{
		EHwStateDeactive,
		EHwStateInactive,		//Waiting for first point
		EHwStateWaitingMove,	//Waiting to see if a period elapses before the first move
		EHwStateDrawing,		//Pen down and drawing
		EHwStateWaitingStroke,	//Waiting for new stroke
		};
public:
	~CHandWritingAnim();
	//pure virtual functions from MEventHandler
	TBool OfferRawEvent(const TRawEvent &aRawEvent);
	//pure virtual functions from CAnim
	void ConstructL(TAny *aArgs);
	void Animate(TDateTime *aDateTime);
	void Redraw();
	void Command(TInt aOpcode, TAny *aArgs);
	TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	void FocusChanged(TBool aState);
private:
	void Activate();
	void Deactivate();
	void SpriteChangeL(TBool aUsingSeparateMask);
	void SetDrawData(THandwritingDrawData *aDrawData);
	TBool HandlePointerDown(TPoint aPoint);
	TBool HandlePointerMove(TPoint aPoint);
	TBool HandlePointerUp(TPoint aPoint);
	void DrawPoint();
	void DrawLine(TPoint aEndPoint);
	void UpdateSprite();
	void StartTimer();
	void SendEatenDownEvent();
	void CharacterFinished();
	void ClearSprite();
private:
	TInt iState;
	CFbsBitmapDevice *iBitmapDevice;
	CFbsBitmapDevice *iMaskBitmapDevice;
	CFbsBitGc *iSpriteGc;
	TBool iIsMask;
	THandwritingDrawData iDrawData;
	TPoint iCurrentDrawPoint;
	TTime iDownTime;			//Will need to use this when taken base changes
	CPointStore *iPointStore;
	TInt iLastGeneratedCharacter;
	};

#endif

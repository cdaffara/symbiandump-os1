
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
#ifndef __TTRANSPTANIM__
#define __TTRANSPTANIM__

#include <e32std.h>
#include <e32base.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "AUTODLL.H"

enum
	{
	GraphicsWServ0483 = 0
	};

class CTTestCase;
class CTransAnimTimer;
class CTTransparentAnim;
class CTTAnimation;
class MAnimationObserver;

class CTTransparentAnim : public CTWsGraphicsBase
	{
public:
	static CTTestCase* CreateTestCaseL(CTTransparentAnim*, TInt);
public:
	CTTransparentAnim(CTestStep*);
	virtual ~CTTransparentAnim();
	void ConstructL();

	void SetCurrent(CTTestCase*);
	CTTestCase* GetCurrent();
protected:
	virtual void RunTestCaseL(TInt);
	
private:
	CTTestCase* iCurrentTestCase;
	};

class CTTransparentAnimStep : public CTGraphicsStep
	{
public:
	CTTransparentAnimStep();

protected:
	virtual CTGraphicsBase* CreateTestL();
	};

class CTransAnimTimer : public CTimer
	{
public:
	CTransAnimTimer();
	~CTransAnimTimer();
	void ConstructL();
	void Start(TTimeIntervalMicroSeconds32 aInterval,TCallBack aCallBack);
private:
	void RunL();
	TCallBack iCallBack;
	};

class CTTAnimation
	{
public:
	CTTAnimation(CTWin*, TInt);
	~CTTAnimation();
	
	void StartL();
	TBool IsComplete();
	void SetCurrentFrameSet(TInt);
	CTWin* GetWindow();
	void Attach(MAnimationObserver*);
private:
	static TInt NextFrame(TAny*);
	void NextFrame();

private:
	CTWin* iWin;
	MAnimationObserver* iObserver;
	RAnimDll iAnimDll;
	RTestAnim iDrawAnim;
	CTransAnimTimer iAnimTimer;
	TInt iFrameCount;
	TInt iCurrentFrameSet;
	TBool iIsComplete;
	TInt iDrawArg;
	};

template <TBool AlphaTransparency = EFalse>
class CTAnimWin : public CTWin
	{
public:
	static CTWin* NewL(TPoint aPosition, TSize aSize)
		{
		CTAnimWin* w = new(ELeave) CTAnimWin();
	
		CleanupStack::PushL(w);
		w->ConstructL(aPosition, aSize);
		CleanupStack::Pop();
		
		return w;	
		}
public:
	virtual ~CTAnimWin(){}

	void Draw(){}
private:
	CTAnimWin(){}

	void ConstructL(TPoint aPoint, TSize aSize)
		{
		TRgb bgColor(150, 150, 150);
		ConstructExtLD(*TheClient->iGroup, aPoint, aSize);
		AssignGC(*TheClient->iGc);
		Win()->SetRequiredDisplayMode(EColor16MA);
		if (AlphaTransparency)
			{
			Win()->SetTransparencyAlphaChannel();
			bgColor.SetAlpha(150);
			}
		Win()->SetBackgroundColor(bgColor);
		Win()->Activate();
		}
	};

//*****************************************************************************
class MAnimationObserver
	{
public:
	virtual void Update(CTTAnimation*) = 0;
	};

class CTTestCase : public MAnimationObserver
	{
public:
	CTTestCase();
	virtual ~CTTestCase() {}

	TBool IsAlreadyRunning();
	TBool IsComplete();
	void StartL();
	TBool HasFailed();
	void SetError(TInt);
	
	// Implemented for MAnimationObserver
	virtual void Update(CTTAnimation*);
	virtual void ConstructL() = 0;
	virtual TPtrC TestCaseName() = 0;

protected:
	virtual void RunTestCaseL() = 0;
	void SetIsCompleted(TBool);
	
private:
	TInt iError;
	TBool iIsComplete;
	TBool iIsRunning;
	};

class CTGraphicsWServ0483 : public CTTestCase
	{
public:
	static CTGraphicsWServ0483* NewL();
	
public:
	virtual ~CTGraphicsWServ0483();
	virtual void RunTestCaseL();
	
	void Update(CTTAnimation*);
	
protected:
	CTGraphicsWServ0483(){}
	virtual void ConstructL();
	virtual TPtrC TestCaseName();	
	
private:
	CTTAnimation* iAnim;
	CTTAnimation* iCheckAnim;
	};

_LIT(KTTransparentAnimStep,"TTransparentAnim");

#endif // __TTRANSPTANIM__


// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TPNTKEY_H__
#define __TPNTKEY_H__

#include <e32std.h>
#include <hal.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

const TInt ENumPntKeyTests=6;
const TUint EModifierMask=EModifierCtrl|EModifierShift|EModifierFunc;

class CTPntKey;

class CTPntKeyWindow : public CTWin
	{
private:
	enum {KPointerMoveBufferSize=32};
public:
	CTPntKeyWindow(CTPntKey *aTest);
	~CTPntKeyWindow();
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent);
	void PointerL(const TPointerEvent &pointer,const TTime &aTime);
	void KeyUpL(const TKeyEvent &aKey,const TTime &);
	void KeyDownL(const TKeyEvent &aKey,const TTime &);
	void WinKeyL(const TKeyEvent &aKey,const TTime &);
	void SwitchOn(const TTime &aTime);
	void NextKey();
	void Test(TInt aCheck);
	void Error(TInt aWhere);
	void DrawButton(const TRect &aRect, const TDesC &aText);
	void Draw();
	void SendEvent();
	void SimulatePointerDownUp(const TRect& aRect);
	inline TInt KeyCount() {return iKeyCount;}
private:	
	TInt iKeyCount;
	TRect iKey1;
	TRect iKey2;
	TRect iKey3;
	static TInt iTestScanCodes[ENumPntKeyTests];
	static TUint iTestCodes[ENumPntKeyTests];
	static TUint iTestModifiers[ENumPntKeyTests];
	CTPntKey *iTest;
	};
	
class CTPntKey : public CTWsGraphicsBase
{
private:
	enum {KTimeOutAfter=60000000};	//60secs
	
public:
	CTPntKey(CTestStep* aStep);
	~CTPntKey();
	void ConstructL();
	static TInt TimeOut(TAny* aTest);
	void TimeOut();
	void Failed(TInt aWhere);
	inline TBool NoDigitiser() const {return iNoDigitiser;}
	
protected:	//from CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	
private:
	inline TestClient *Client() {return TheClient;}
	
private:
	CTimeOut* iTimeOut;
	CTPntKeyWindow *iWin;
	TSize iWinSize;
	TInt iState;
	TBool iFailed;
	TBool iOldPointerState;
	TBool iNoDigitiser;
	};

class CTPntKeyStep : public CTGraphicsStep
	{
public:
	CTPntKeyStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTPntKeyStep,"TPntKey");


#endif

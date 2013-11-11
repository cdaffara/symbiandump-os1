// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TCAPKEY_H__
#define __TCAPKEY_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

enum TTestMode
	{
	EModeNormalCapture,
	EModeNormalCaptureWithPriority,
	EModeWaitingForFirstNormalCapWithPriority,
	EModeWaitingForFirstDown,
	EModeWaitingForFirstDownWithPriority,
	EModeKeyUpsAndDowns,
	EModeKeyUpsAndDownsWithPriority,
	EModeAbortKeys,
	};
enum TCapKeyRetValues
	{
	ECapKeyOkay,
	ECapKeyRetry,
	ECapKeyFail,
	};

struct SCapKeyParams
	{
	TInt connIndex;		// Connection that should capture this key
	TUint keyCode;
	TUint modifier_mask;
	TUint modifiers;
	TInt priority;
	};

struct SCapKeyTestParams
	{
	TText *txt;			// Text message telling user what to do
	TInt connIndex;		// Connection that receive the key press
	TUint keyCode;		// Expected key code
	TUint modifier_mask;// Modifiers mask for expected modifiers
	TUint modifiers;	// Expected modifiers
	};

struct SAbortKeyTests
	{
	TUint keyCode;		// Expected key code
	TUint modifier_mask;// Modifiers mask for expected modifiers
	TUint modifiers;	// Expected modifiers
	};

LOCAL_D SCapKeyParams capKeys[]={
	{1,19,EModifierFunc|EModifierCtrl,EModifierFunc|EModifierCtrl,0},
	{2,'b',EModifierFunc|EModifierCtrl,EModifierFunc,0},
	{2,'n',EModifierFunc|EModifierCtrl,EModifierFunc,0},
	{0,3,EModifierFunc|EModifierCtrl,EModifierFunc|EModifierCtrl,0},
	{2,'q',0,0,0},
	{2,'w',0,0,0},
	};

LOCAL_D SCapKeyParams capKeysWithPriority[]={
	{2,'h',0,0,7},
	{1,'h',EModifierFunc|EModifierCtrl|EModifierShift,0,7},
	{0,'h',EModifierFunc|EModifierCtrl|EModifierShift,0,7},
	{2,'j',EModifierFunc|EModifierCtrl|EModifierShift,0,6},
	{1,'j',EModifierFunc|EModifierCtrl|EModifierShift,0,6},
	{0,'j',EModifierFunc|EModifierCtrl|EModifierShift,0,5},
	{2,'k',EModifierFunc|EModifierCtrl|EModifierShift,0,4},
	{1,'k',EModifierFunc|EModifierCtrl|EModifierShift,0,3},
	{0,'k',EModifierFunc|EModifierCtrl|EModifierShift,0,2},
	};
	
LOCAL_D SCapKeyParams capUpAndDownKeys[]={
	{2,EStdKeySpace,EModifierFunc|EModifierCtrl|EModifierShift,0,0},
	{2,EStdKeyEnter,EModifierFunc|EModifierCtrl|EModifierShift,EModifierShift,0},
	};

LOCAL_D SCapKeyParams capUpAndDownKeysWithPriority[]={
	{2,'H',EModifierFunc|EModifierCtrl|EModifierShift,0,7},
	{1,'H',EModifierFunc|EModifierCtrl|EModifierShift,0,7},
	{0,'H',EModifierFunc|EModifierCtrl|EModifierShift,0,7},
	{2,'J',EModifierFunc|EModifierCtrl|EModifierShift,0,6},
	{1,'J',EModifierFunc|EModifierCtrl|EModifierShift,0,6},
	{0,'J',EModifierFunc|EModifierCtrl|EModifierShift,0,5},
	{2,'K',EModifierFunc|EModifierCtrl|EModifierShift,0,4},
	{1,'K',EModifierFunc|EModifierCtrl|EModifierShift,0,3},
	{0,'K',EModifierFunc|EModifierCtrl|EModifierShift,0,2},
	};
	
LOCAL_D SCapKeyTestParams capKeyTests[]={
	{(TText *)_S("Press q"),2,'q',EModifierFunc|EModifierCtrl|EModifierShift,0},
	{(TText *)_S("Press <Alt>w"),2,'w',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press a"),0,'a',0},
	{(TText *)_S("Press <Ctrl><Alt>s"),1,19,EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl},
	{(TText *)_S("Press <Alt>n"),2,'n',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press <Ctrl><Alt>b"),0,2,EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl},
	{(TText *)_S("Press <Alt>d"),0,'d',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press <Ctrl><Alt>c"),0,3,EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl},
//
// Capture normal keys with priority
//
	{(TText *)_S("Press h"),0,'h',EModifierFunc|EModifierCtrl|EModifierShift,0},
	{(TText *)_S("Press j"),1,'j',EModifierFunc|EModifierCtrl|EModifierShift,0},
	{(TText *)_S("Press k"),2,'k',EModifierFunc|EModifierCtrl|EModifierShift,0},
//
// Capture ups and downs
//
	{(TText *)_S("Press <Space>"),2,EStdKeySpace,EModifierFunc|EModifierCtrl|EModifierShift,0},
	{(TText *)_S("Press <Enter>"),0,EStdKeyEnter,EModifierShift,0},
	{(TText *)_S("Press <Shift><Space>"),0,EStdKeySpace,EModifierFunc|EModifierCtrl|EModifierShift,EModifierShift},
	{(TText *)_S("Press <Shift><Enter>"),2,EStdKeyEnter,EModifierShift,EModifierShift},

//
// Capture ups and downs
//
	{(TText *)_S("Press H"),0,'H',EModifierFunc|EModifierCtrl|EModifierShift,0},
	{(TText *)_S("Press J"),1,'J',EModifierFunc|EModifierCtrl|EModifierShift,0},
	{(TText *)_S("Press K"),2,'K',EModifierFunc|EModifierCtrl|EModifierShift,0},
//
// Priority keys
//
	{(TText *)_S("Press <Ctrl>c"),0,0,0,0},
	{(TText *)_S("Press <Ctrl>d"),0,0,0,0},
	{(TText *)_S("Press z"),0,0,0,0},
	};

LOCAL_D SAbortKeyTests abortKeyTests[]={
	{3,EModifierCtrl,EModifierCtrl},
	{4,EModifierCtrl,EModifierCtrl},
	{'z',EModifierFunc|EModifierCtrl|EModifierShift,0}
	};

struct SErrorCapKey
	{
	TInt keyCode;
	TUint modifier_mask;
	TUint modifiers;
	TInt priority;
	};

LOCAL_D SErrorCapKey errorKeys[]={
	{'a', EModifierFunc|EModifierShift,EModifierFunc|EModifierCtrl, 0},
	{'1', 0, EModifierFunc, 0},
	{3, EModifierCtrl,EModifierCtrl|EModifierShift, 0},
	// @SYMPREQ 417-61800: Check that attempts to capture keys with
	// EModifierLong in the modifier mask fail with KErrArgument.
	{'T', EModifierLongKey, 0, 0},
	{'u', EModifierLongKey|EModifierAlt, EModifierLongKey, 0},
	{'V', EModifierLongKey|EModifierShift, EModifierLongKey|EModifierShift, 0},
	// @SYMPREQ 417-61800: Check that attempts to capture keys with a priority
	// of KMinTInt fail with KErrArgument.
	{'W', 0, 0, KMinTInt},
	};

const TInt numCapKeys=sizeof(capKeys)/sizeof(capKeys[0]);
const TInt numCapKeysWithPriority=sizeof(capKeysWithPriority)/sizeof(capKeysWithPriority[0]);
const TInt numTests=sizeof(capKeyTests)/sizeof(capKeyTests[0]);
const TInt numErrorKeys=sizeof(errorKeys)/sizeof(errorKeys[0]);
const TInt numAbortKeyTests=sizeof(abortKeyTests)/sizeof(abortKeyTests[0]);
const TInt numUpDownKeys=sizeof(capUpAndDownKeys)/sizeof(capUpAndDownKeys[0]);
const TInt numUpDownKeysWithPriority=sizeof(capUpAndDownKeysWithPriority)/sizeof(capUpAndDownKeysWithPriority[0]);
const TInt numUpDownKeyTests=4;
const TInt numUpDownKeyWithPriorityTests=3;
const TInt numCapKeyWithPriorityTests=3;
const TInt numCapKeyTests=numTests-numAbortKeyTests-numUpDownKeyTests-numUpDownKeyWithPriorityTests-numCapKeyWithPriorityTests;

LOCAL_D TInt capKeyHandle[numCapKeys];
LOCAL_D TInt capKeyWithPriorityHandle[numCapKeysWithPriority];
LOCAL_D TInt capUpDownKeyHandle[numUpDownKeys];
LOCAL_D TInt capUpDownKeyWithPriorityHandle[numUpDownKeysWithPriority];

class CCKWindow;
class CTCaptureKey;
class CCKAbortConnection;

class CCKConnectionBase : public CTClient
	{
public:
	CCKConnectionBase(CTCaptureKey *aTest);
	void ConstructL();
protected:
	CTCaptureKey *iTest;
	};

class CCKConnection : public CCKConnectionBase
	{
public:
	CCKConnection(CTCaptureKey *aTest, TInt aConnIndex);
	~CCKConnection();
	void ConstructL();
	void CheckKeyL(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey);
	void SubStateChanged();
	void KeyTestL(TInt aBool);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void CheckUpDownL(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey);
	void CheckUpDownL(const TKeyEvent &aKey);
	inline TInt Index() {return iConnIndex;}
protected:
	TInt iConnIndex;
	CCKWindow *iWin;
	};

class CAbortHandler : public CTEventBase
	{
public:
	CAbortHandler(RWsSession *aWs, CCKAbortConnection *aConnection);
	~CAbortHandler();
	void Request();
protected:
	void DoCancel();
	void doRunL();
protected:
	CCKAbortConnection *iConnection;
	};

class CCKAbortConnection : public CCKConnectionBase
	{
public:
	CCKAbortConnection(CTCaptureKey *aTest);
	~CCKAbortConnection();
	void ConstructL();
	void ReceivedAbortEvent(TInt aHandle, TKeyEvent *aKey);
	void Foreground();
protected:
	CAbortHandler *iAbortHandler;
	TInt iAbortKeyIndex;
	};

class CCKWindow : public CTWin
	{
public:
	CCKWindow(CTCaptureKey *aTest, TInt aConnIndex);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
protected:
	TInt iConnIndex;
	CTCaptureKey *iTest;
	TRgb iBack;
	};

class CCKWindowGroup : public CTWindowGroup
	{
public:
	CCKWindowGroup(CTClient *aClient, CTCaptureKey *aTest);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void KeyDownL(const TKeyEvent &aKey,const TTime &);
	void KeyUpL(const TKeyEvent &aKey,const TTime &);
	void CheckUpDown(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey);
private:
	CTCaptureKey *iTest;
	};

class CTCaptureKey : public CTWsGraphicsBase
	{
public:
	CTCaptureKey(CTestStep* aStep);
	~CTCaptureKey();
	void ConstructL();
	void EndCaptureKeyTest();
	inline TInt State() {return iState;}
	TInt SubState() const;
	void IncSubState();
	void BadParams();
	void CapKeyPurgingL();
	void SetUpPriorityKeyTest();
	void CaptureWithPriorityTest();
	void CaptureUpsAndDownsTest();
	void CaptureUpsAndDownsWithPriorityTest();
	TTestMode Mode();
	void SetMode(TTestMode aMode);
	inline TestClient* Client() {return TheClient;}
	void SendEvents();
	void CapKeyPowerOffL();
	inline TInt ScreenNumber() {return iTest->ScreenNumber();}
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	CCKConnection *iConn1;
	CCKConnection *iConn2;
	CCKConnection *iConn3;
	CCKAbortConnection *iAbortConn;
	TSize iWinSize;
	TInt iState;
	TInt iSubState;
	TTestMode iMode;
	};

class CTCaptureKeyStep : public CTGraphicsStep
	{
public:
	CTCaptureKeyStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTCaptureKeyStep,"TCaptureKey");


#endif

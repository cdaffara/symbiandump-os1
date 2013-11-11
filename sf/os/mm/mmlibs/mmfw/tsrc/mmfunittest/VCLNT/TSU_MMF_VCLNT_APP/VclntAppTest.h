// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __VCLNT_APPTEST_H__
#define __VCLNT_APPTEST_H__

#include <coecntrl.h>
#include <e32std.h>
#include <eikapp.h>
#include <eikappui.h>
#include <techview/eikdialg.h>
#include <eikdoc.h>
#include <eikenv.h>
#include <uikon.hrh>
#include "testframework.h"
#include "VclntAppTest.hrh"

#include <videoplayer.h>
#include <mmf/common/mmfvideo.h>
#include "CmmfVideoTestControllerUIDs.hrh"
#include "TS_Codes.h"
#include "VideoPlayerBody.h"

_LIT(KVclntAppSemName, "VclntAppSem");
_LIT(KVclntAppTestResultName, "C:\\VCLNT_APP_TEST_RESULT.txt");

_LIT8(KLitPass, "EPass");
_LIT8(KLitFail, "EFail");
_LIT8(KLitInconclusive, "EInconclusive");
_LIT8(KLitTestSuiteError, "ETestSuiteError");

const TUid KUidVclntTestApp = { 0x101F7C13 }; 

// MVclntTestObserver
class MVclntTestObserver
	{
public:
	virtual void TestFinishedL(const TVerdict aVerdict)=0;
	};

// CVclntTestView
class CVideoPlayerUtility::CTestView : public CCoeControl, public MVideoPlayerUtilityObserver
	{
public:
	static CTestView* NewL(const TRect& aRect, MVclntTestObserver* aObserver, CLog* aLog);
	~CTestView();
protected:
	CTestView(MVclntTestObserver* aObserver, CLog* aLog);
	void ConstructL(const TRect& aRect);
public:
	void CreateVideoPlayerL();
protected:
	void CheckFunctionsCalledL();
	static TInt VclntTestCallBack(TAny* aPtr);
	void DoVclntTestCallBackL();
	// from MVideoPlayerUtilityObserver
public:
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
protected:
	CVideoPlayerUtility* iPlayer;
	MVclntTestObserver* iObserver;
	CIdle* iIdle;
	CLog* iLog;
	CEikDialog* iDialog;
	};

// CVclntTestAppUi
class CVclntTestAppUi : public CEikAppUi, public MVclntTestObserver
	{
public:
	~CVclntTestAppUi();
	void ConstructL();
protected:
	void DoConstructL();
	static TInt StartTests(TAny* aPtr);
	void DoTestsL();
	void WriteResultFileL(const TVerdict aResult);
	void RunTest_MM_MMF_VCLNT_U_0050L();
	void SendExitKeyEvent();
	// from MVclntTestObserver
public:
	virtual void TestFinishedL(const TVerdict aVerdict);
	// from CEikAppUi
protected:
	virtual void HandleCommandL(TInt aCommand);
	virtual TBool ProcessCommandParametersL(TApaCommand aCommand, TFileName& aDocumentName, const TDesC8& aTail);
protected:
	CVideoPlayerUtility::CTestView* iView;
	CLog* iLog;
	TBuf8<KMaxLenTestStepName> iTestStepName;
	CIdle* iIdleStarter;
	};

// CVclntTestDoc
class CVclntTestDoc : public CEikDocument
	{
public:
	static CVclntTestDoc* NewL(CEikApplication& aApp);
protected:
	CVclntTestDoc(CEikApplication& aApp);
	void ConstructL();
protected:
	CEikAppUi* CreateAppUiL();
	};

// CVclntTestApplication
class CVclntTestApplication : public CEikApplication
	{
protected:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

#endif // __VCLNT_APPTEST_H__

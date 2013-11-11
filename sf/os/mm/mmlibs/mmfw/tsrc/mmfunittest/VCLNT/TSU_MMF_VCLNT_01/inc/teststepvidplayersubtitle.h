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
// This file contains testing steps for the subtitle Unit
// test of the MMF Video Clients.
// 
//


#ifndef TESTSTEPVIDPLAYERSUBTITLE_H
#define TESTSTEPVIDPLAYERSUBTITLE_H

#include <mmf/common/mmfvideosubtitlecustomcommands.h>
#include "TestStepUnitMMFVidClient.h"
#include "TS_Codes.h"

/*
Subtitle availability test
MM-MMF-VCLNT-U-1300-CP
*/
class CTestStepVidPlayerSubtitleAvailable : public CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerSubtitleAvailable* NewL(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	TVerdict DoTestStepPostambleL();
	
	void CheckSubtitleCallCountL(TMMFTestGetSubtitleCountType aType, TInt aCount);
	TInt SetSubtitleDisplayCheck(TMMFSubtitleWindowConfig& aConfig);

	//from MVideoPlayerUtilityObserver
	void MvpuoOpenComplete(TInt aError);
	void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	void MvpuoPlayComplete(TInt aError);
	void MvpuoPrepareComplete(TInt aError);
	void MvpuoEvent(const TMMFEvent& aEvent);

protected:
	CTestStepVidPlayerSubtitleAvailable(const TDesC& aTestName);
	
protected:
	TInt iError;
	TMMFMessageDestinationPckg iDestPckg;
	};

/*
Subtitle enable and disable test
MM-MMF-VCLNT-U-1301-CP
*/
class CTestStepVidPlayerSubtitleEnableDisable : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleEnableDisable* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleEnableDisable(const TDesC& aTestName);
	};

/*
RedrawSubtitle test
MM-MMF-VCLNT-U-1302-HP
*/
class CTestStepVidPlayerSubtitleRedraw : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleRedraw* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleRedraw(const TDesC& aTestName);
	};

/*
RedrawSubtitle when CRP ready is not received
MM-MMF-VCLNT-U-1303-HP
*/
class CTestStepVidPlayerSubtitleRedrawNotReady : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleRedrawNotReady* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleRedrawNotReady(const TDesC& aTestName);
	};
	
/*
Enable and disable subtitle with multiple window
MM-MMF-VCLNT-U-1304-HP
*/
class CTestStepVidPlayerSubtitleMultiWindow : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleMultiWindow* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();
	TVerdict DoTestStepPostambleL();

protected:
	CTestStepVidPlayerSubtitleMultiWindow(const TDesC& aTestName);
	
	void InitWindowsL(TBool aWindowTwoOnly = EFalse);
	
protected:
	CWsScreenDevice* iScreen2;
	CWsScreenDevice* iScreen3;

	RWindowGroup iRootWindow2;
	RWindowGroup iRootWindow3;

	RWindow* iWindow2;
	RWindow* iWindow3;
	};

/*
Enable subtitle and add and remove multiple windows
MM-MMF-VCLNT-U-1305-HP
*/
class CTestStepVidPlayerSubtitleAddRemoveMultiWindow : public CTestStepVidPlayerSubtitleMultiWindow
	{
public:
	static CTestStepVidPlayerSubtitleAddRemoveMultiWindow* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleAddRemoveMultiWindow(const TDesC& aTestName);
	};

/*
Enable subtitle with multiple windows and change rotation or clip rect setting 
MM-MMF-VCLNT-U-1306-MP and MM-MMF-VCLNT-U-1307-MP
*/
class CTestStepVidPlayerSubtitleMultiWindowSettingChange : public CTestStepVidPlayerSubtitleMultiWindow
	{
public:
	static CTestStepVidPlayerSubtitleMultiWindowSettingChange* NewL(const TDesC& aTestName, TBool aTestRotation);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleMultiWindowSettingChange(const TDesC& aTestName, TBool aTestRotation);

protected:
	TBool iTestRotation;
	};

/*
Subtitle language test
MM-MMF-VCLNT-U-1308-HP
*/
class CTestStepVidPlayerSubtitleLanguage : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleLanguage* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleLanguage(const TDesC& aTestName);
	};

/*
Enable subtitle with one window and change rotation or clip rect setting 
MM-MMF-VCLNT-U-1309-HP and MM-MMF-VCLNT-U-1310-HP
*/
class CTestStepVidPlayerSubtitleWindowSettingChange : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleWindowSettingChange* NewL(const TDesC& aTestName, TBool aTestRotation);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleWindowSettingChange(const TDesC& aTestName, TBool aTestRotation);

protected:
	TBool iTestRotation;
	};
	
/*
Subtitle enable negative tests
MM-MMF-VCLNT-U-1350-CP, MM-MMF-VCLNT-U-1351-CP, MM-MMF-VCLNT-U-1352-MP, MM-MMF-VCLNT-U-1353-CP
*/
class CTestStepVidPlayerSubtitleEnableNeg : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleEnableNeg* NewL(const TDesC& aTestName, 
														TBool aTestSubNotAvailble,
														TBool aTestSourceNotOpen,
														TBool aTestEnableTwice,
														TBool aTestNoDisplayAdded,
														TInt aExpectedErr);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleEnableNeg(const TDesC& aTestName, 
										TBool aTestSubNotAvailble,
										TBool aTestSourceNotOpen,
										TBool aTestEnableTwice,
										TBool aTestNoDisplayAdded,
										TInt aExpectedErr);
	
protected:
	TBool iTestSubNotAvailable;
	TBool iTestSourceNotOpen;
	TBool iTestEnableTwice;
	TBool iTestNoDisplayAdded;
	TInt iExpectedErr;
	};
	
/*
Test Subtitle language with subtitle not enabled
MM-MMF-VCLNT-U-1354-HP, MM-MMF-VCLNT-U-1355-HP, MM-MMF-VCLNT-U-1356-HP
*/
class CTestStepVidPlayerSubtitleLangNotEnable : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleLangNotEnable* NewL(const TDesC& aTestName, 
														TBool aTestGetSupportedLang,
														TBool aTestGetLang,
														TBool aTestSetLang);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleLangNotEnable(const TDesC& aTestName, 
														TBool aTestGetSupportedLang,
														TBool aTestGetLang,
														TBool aTestSetLang);
	
protected:
	TBool iTestGetSupportedLang;
	TBool iTestGetLang;
	TBool iTestSetLang;
	};

/*
Subtitle language not available test
MM-MMF-VCLNT-U-1357-HP
*/
class CTestStepVidPlayerSubtitleLangNotAvail : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleLangNotAvail* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleLangNotAvail(const TDesC& aTestName);
	};

/*
Test RedrawSubtitle when subtitle is not enabled
MM-MMF-VCLNT-U-1358-LP
*/
class CTestStepVidPlayerSubtitleNotEnabledRedraw : public CTestStepVidPlayerSubtitleAvailable
	{
public:
	static CTestStepVidPlayerSubtitleNotEnabledRedraw* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();

protected:
	CTestStepVidPlayerSubtitleNotEnabledRedraw(const TDesC& aTestName);
	};

#endif // TESTSTEPVIDPLAYERSUBTITLE_H

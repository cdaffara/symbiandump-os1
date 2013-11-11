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
// Testing key event, capturing keys and hot keys
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "TKEY.H"

#define LOGGING 1	//Uncomment this when there is a fail so that you get more logging

GLDEF_D ClientToKill *SecondClient=NULL;

GLDEF_C CTClient *CreateClientToKillL()
	{
	return(SecondClient=new(ELeave) ClientToKill());
	}

GLDEF_C TInt ProcToKill(TAny *aScreenNumber)
	{
	TInt ret=KErrNone;
	ret=TestLibStartUp(CreateClientToKillL,(TInt)aScreenNumber);
	return(ret);
	}

CVisibleGroup::CVisibleGroup(TInt aScreenNo): iScreenNo(aScreenNo)
	{}

void CVisibleGroup::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	iScr=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScr->Construct(iScreenNo));
	iScr->SetAppScreenMode(iMode);
	iGroup=RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(NULL,ETrue));
	TInt id=TheClient->iGroup->GroupWin()->Identifier();
	iGroup.SetOwningWindowGroup(id);
#if defined(LOGGING)
	TLogMessageText buf;
	_LIT(KIds,"Window Group Id: Main=%d, 2nd Client=%d");
	buf.Format(KIds,id,iGroup.Identifier());
	iWs.LogMessage(buf);
#endif
	iWs.Flush();
	}

CVisibleGroup::~CVisibleGroup()
	{
	iGroup.Close();
	delete iScr;
	iWs.Close();
	}

void CVisibleGroup::SetMode(TInt aMode)
	{
	iMode=aMode;
	iScr->SetAppScreenMode(iMode);
	iWs.Flush();
	}

ClientToKill::ClientToKill()
	{}

void ClientToKill::ConstructL()
	{
	CTClient::ConstructL();
	RSemaphore sem;
	sem.OpenGlobal(KSemaphoreName);

	iGroup=new(ELeave) TestWindowGroup(this);
	iGroup->ConstructL();

	iScreen->SetScreenMode(0);
	TSize screenSize=iScreen->SizeInPixels();

	TInt winWidth=3*screenSize.iWidth/4;
	TInt winHeight=3*screenSize.iHeight/4;
	iTestWin=new(ELeave) TestWindow();
	iTestWin->SetUpL(TPoint((screenSize.iWidth+4)/8,(screenSize.iHeight+4)/8),TSize(winWidth,winHeight),iGroup,*iGc);

#if defined(LOGGING)
	TLogMessageText buf;
	_LIT(KId,"Window Group Id in 2nd Process=%d");
	buf.Format(KId,iGroup->GroupWin()->Identifier());
	LogMessage(buf);
#endif
	sem.Signal();	// Signal thread fully initialised
	sem.Close();
	}

ClientToKill::~ClientToKill()
	{
	delete iTestWin;
	}


// CTKey

CTKey::CTKey(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}



void CTKey::ConstructL()
	{
	iSem.CreateGlobal(KSemaphoreName,0,KOwnerType);
	iVisibleGroup=new(ELeave) CVisibleGroup(iTest->iScreenNumber);
	iVisibleGroup->ConstructL();
	}

CTKey::~CTKey()
	{
	delete iVisibleGroup;
	iSem.Close();
	// Set focus back to this screen as it went to primary screen when iVisibleGroup was destroyed
	// Wserv focus handling made focus screen switch to another screen if there is no
	// focusable window group on that particular screen.
	TheClient->iWs.SetFocusScreen(iTest->iScreenNumber);
	}

void CTKey::SendKey(TInt aScanCode)
	{
	TKeyEvent keyEvent;
	keyEvent.iCode=aScanCode;
	keyEvent.iScanCode=aScanCode;
	keyEvent.iModifiers=0;
	keyEvent.iRepeats=0;
	TheClient->iWs.SimulateKeyEvent(keyEvent);
	TheClient->iWs.Flush();
	}

void CTKey::SendUpDown(TInt aScanCode)
	{
	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::EKeyDown,aScanCode);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	rawEvent.Set(TRawEvent::EKeyUp,aScanCode);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	}

void CTKey::SendCtrlAltShift(TRawEvent::TType aType)
	{
	TRawEvent rawEvent;
	rawEvent.Set(aType,EStdKeyLeftFunc);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	rawEvent.Set(aType,EStdKeyLeftCtrl);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	rawEvent.Set(aType,EStdKeyLeftShift);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	}

void CTKey::SendCtrlAltShift(TInt aScanCode)
	{
	SendCtrlAltShift(TRawEvent::EKeyDown);
	SendUpDown(aScanCode);
	SendCtrlAltShift(TRawEvent::EKeyUp);
	TheClient->iWs.Flush();
	}

void CTKey::LogWindowGroupIds()
	{
	CArrayFixFlat<TInt>* idList=new(ELeave) CArrayFixFlat<TInt>(5);
	if (!idList)
		return;
	TheClient->iWs.WindowGroupList(0,idList);
	TLogMessageText buf;
	_LIT(KWinGp,"Window Group Id's: %d");
	_LIT(KId,", %d");
	TInt ii=0;
	buf.Format(KWinGp,(*idList)[ii]);
	while (++ii<idList->Count())
		buf.AppendFormat(KId,(*idList)[ii]);
	LOG_MESSAGE(buf);
	delete idList;
	}

void CTKey::SetHotKey(THotKey aType, TUint aKeyCode)
	{
	TheClient->iWs.SetHotKey(aType,aKeyCode-'A'+1,EFuncCtrlShift,EFuncCtrlShift);
	}

void CTKey::KeyOfOrientation()
	{	
	TInt numScreenModes = TheClient->iScreen->NumScreenModes();
	TInt originalScreenMode = TheClient->iScreen->CurrentScreenMode();
	if(numScreenModes >= 2)
		{
		//1 possible rotation of 180deg found in wsini.ini screenmode 8
		iVisibleGroup->SetMode(numScreenModes-1);
		TheClient->iScreen->SetScreenMode(numScreenModes-1);
		SendCtrlAltShift('O');
		TheClient->iWs.Flush();
		//1 possible rotation of 90deg found in wsini.ini screenmode 7
		iVisibleGroup->SetMode(numScreenModes-2);
		TheClient->iScreen->SetScreenMode(numScreenModes-2);
		SendCtrlAltShift('O');
		TheClient->iWs.Flush();
		//reset to original screenmode
		iVisibleGroup->SetMode(originalScreenMode);
		TheClient->iScreen->SetScreenMode(originalScreenMode);
		}	
	else
		{
		TEST(numScreenModes>2);
		INFO_PRINTF2(_L("numScreenModes - Expected: >2, Actual: %d"), numScreenModes);
		}
	TPixelsAndRotation originalSizeAndRotation;
	TheClient->iScreen->GetScreenModeSizeAndRotation(TheClient->iScreen->CurrentScreenMode(),originalSizeAndRotation);
	SendCtrlAltShift('O');
	TheClient->iWs.Flush();
	TPixelsAndRotation sizeAndRotation;
	TheClient->iScreen->GetScreenModeSizeAndRotation(TheClient->iScreen->CurrentScreenMode(),sizeAndRotation);
	TEST(sizeAndRotation.iRotation!=originalSizeAndRotation.iRotation);
	if (sizeAndRotation.iRotation==originalSizeAndRotation.iRotation)
		INFO_PRINTF3(_L("iRotation - Not Expected: %d, Actual: %d"), originalSizeAndRotation.iRotation, sizeAndRotation.iRotation);
	SendCtrlAltShift('O');
	TheClient->iWs.Flush();
	}

void CTKey::CycleDisplaySize()
	{
	// The test does nothing if there is only one possible screen mode -> return
	TInt numScreenModes = TheClient->iScreen->NumScreenModes();
	if (numScreenModes<2)
		{
		TEST(numScreenModes>2);
		INFO_PRINTF2(_L("numScreenModes - Expected: >2, Actual: %d"), numScreenModes);
		return;
		}
	//Get original screen mode and size values
	TInt originalScreenMode = TheClient->iScreen->CurrentScreenMode();	
	TPixelsAndRotation sizeAndRotation;
	TheClient->iScreen->GetScreenModeSizeAndRotation(TheClient->iScreen->CurrentScreenMode(),sizeAndRotation);
	TSize originalScreenSize = sizeAndRotation.iPixelSize;
	
	SendCtrlAltShift('U');
	TheClient->iWs.Flush();
	TheClient->iScreen->GetScreenModeSizeAndRotation(TheClient->iScreen->CurrentScreenMode(),sizeAndRotation);
	TEST(sizeAndRotation.iPixelSize.iHeight!=originalScreenSize.iHeight && sizeAndRotation.iPixelSize.iWidth!=originalScreenSize.iWidth);
	if (sizeAndRotation.iPixelSize.iHeight==originalScreenSize.iHeight || sizeAndRotation.iPixelSize.iWidth==originalScreenSize.iWidth)
		INFO_PRINTF5(_L("iPixelSize - Expected: !%d !%d, Actual: %d %d"), originalScreenSize.iHeight, originalScreenSize.iWidth, sizeAndRotation.iPixelSize.iHeight, sizeAndRotation.iPixelSize.iWidth);
	TheClient->iScreen->SetScreenMode(originalScreenMode);
	TheClient->iWs.Flush();
	}

void CTKey::KeyOfDeathL()
	{
	//Key of Death
#if defined(LOGGING)
	TLogMessageText buf;
	_LIT(KNum1,"Priority Zero Window Groups=%d Total=%d (Before Process Creation)");
	buf.Format(KNum1,TheClient->iWs.NumWindowGroups(0),TheClient->iWs.NumWindowGroups());
	LOG_MESSAGE(buf);
	LogWindowGroupIds();
#endif
	CProcess* process = CProcess::NewL(CProcess::eProcessKeyTest,iTest->iScreenNumber);
	RWindowGroup group(TheClient->iWs);
	group.Construct(1234);
	TheClient->Flush();		//Make sure the window group above is created before the one in the other process
	TInt id=TheClient->iGroup->GroupWin()->Identifier();
	group.SetOwningWindowGroup(id);
	group.DefaultOwningWindow();

	iSem.Wait();
	User::After(TTimeIntervalMicroSeconds32(100000));		//0.1 secs
	TInt numGroups=TheClient->iWs.NumWindowGroups();
	TInt numGroups0=TheClient->iWs.NumWindowGroups(0);

#if defined(LOGGING)
	_LIT(KNum2,"NewWinGpId=%d Priority Zero Window Groups=%d Total=%d (After Process Creation)");
	buf.Format(KNum2,id,numGroups0,numGroups);
	LOG_MESSAGE(buf);
	_LIT(KLog1,"Sending Key Of Death");
	buf.Copy(KLog1);
	LOG_MESSAGE(buf);
	LogWindowGroupIds();
#endif
	group.SetOrdinalPosition(1);		//Make sure that the window group we have just created is behind the one created by the other process (belt and braces)
	SendCtrlAltShift('K');
	TheClient->iWs.Flush();
#if defined(LOGGING)
	_LIT(KLog2,"Sent Key Of Death");
	buf.Copy(KLog2);
	LOG_MESSAGE(buf);
#endif
	TInt numGroups2;
	TInt numGroups0b;
	TInt tries=3;
	FOREVER		//On multi-core platforms need to wait for the Key of Death procedure to complete
		{
		numGroups2=TheClient->iWs.NumWindowGroups();
		numGroups0b=TheClient->iWs.NumWindowGroups(0);
		if (--tries==0)
			break;
		if (numGroups!=numGroups2 || numGroups0!=numGroups0b)
			{
			tries=1;
			continue;
			}
		User::After(100000);		//0.1secs
		}
	TEST(numGroups==numGroups2+1);
	_LIT(KLog,"KeyOfDeath Num WinGps Before=%d After=%d (should be one less)");
	if (numGroups!=numGroups2+1)
		LOG_MESSAGE3(KLog,numGroups,numGroups2);
	TEST(numGroups0==numGroups0b+1);
	if (numGroups0!=numGroups0b+1)
		LOG_MESSAGE3(KLog,numGroups0,numGroups0b);
	group.Close();

	TBool processDead=!process->StillAlive();
	TEST(processDead);
	if (!processDead)
		{
		_LIT(KLog,"Other Thread Still Alive");
		LOG_MESSAGE(KLog);
		}
	delete process;

#if defined(LOGGING)
	_LIT(KLog3,"Finished KeyOfDeath Test");
	buf.Copy(KLog3);
	LOG_MESSAGE(buf);
	LogWindowGroupIds();
#endif
	}

#define CONTRAST_UP 'A'		//'a'-'a'+1
#define CONTRAST_DOWN 'B'		//'b'-'a'+1

void CTKey::TestContrastL(TBool aChange)
	{
	TInt contrast=0;
	TInt newContrast=0;
	TInt maxContrast=0;
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrastMax,maxContrast));
	TInt diff=aChange ? 1:0;
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,contrast));

	SendCtrlAltShift(CONTRAST_UP);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,newContrast));
	if ((contrast==maxContrast) && aChange)
		{
		TEST(newContrast==0);
		if (newContrast!=0)
			INFO_PRINTF3(_L("newContrast==0 - Expected: %d, Actual: %d"), 0, newContrast);		
		}
	else
		{
		TEST(contrast+diff==newContrast);
		if (contrast+diff!=newContrast)
			INFO_PRINTF3(_L("contrast+diff==newContrast - Expected: %d, Actual: %d"), contrast+diff, newContrast);
		}

	SendCtrlAltShift(CONTRAST_DOWN);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,newContrast));
	TEST(contrast==newContrast);
	if (contrast!=newContrast)
			INFO_PRINTF3(_L("contrast==newContrast - Expected: %d, Actual: %d"), contrast, newContrast);

	SendCtrlAltShift(CONTRAST_DOWN);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,newContrast));
	if (contrast==0)
		{
		TEST(maxContrast==newContrast);
		if (maxContrast!=newContrast)
			INFO_PRINTF3(_L("maxContrast==newContrast - Expected: %d, Actual: %d"), maxContrast, newContrast);

		}
	else
		{
		TEST(contrast-diff==newContrast);
		if (contrast-diff!=newContrast)
			INFO_PRINTF3(_L("contrast-diff==newContrast - Expected: %d, Actual: %d"), contrast-diff, newContrast);

		}

	SendCtrlAltShift(CONTRAST_UP);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,newContrast));
	TEST(contrast==newContrast);
	if (contrast!=newContrast)
		INFO_PRINTF3(_L("contrast==newContrast - Expected: %d, Actual: %d"), contrast, newContrast);

	}

void CTKey::TestDefaultContrastL(TBool aChange)
	{
	TInt contrast=0;
	TInt newContrast=0;
	TInt maxContrast=0;
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrastMax,maxContrast));
	TInt diff=aChange ? 1:0;
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,contrast));

	SendKey(EKeyIncContrast);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,newContrast));
	if ((contrast==maxContrast) && aChange)
		{
		TEST(newContrast==0);
		if (0!=newContrast)
			INFO_PRINTF3(_L("newContrast==0 - Expected: %d, Actual: %d"), 0, newContrast);
		}
	else
		{
		TEST(contrast+diff==newContrast);
		if (contrast+diff!=newContrast)
			INFO_PRINTF3(_L("newContrast==contrast+diff - Expected: %d, Actual: %d"), contrast+diff, newContrast);
		}

	SendKey(EKeyDecContrast);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,newContrast));
	TEST(contrast==newContrast);
	if (contrast!=newContrast)
			INFO_PRINTF3(_L("contrast==newContrast - Expected: %d, Actual: %d"), contrast, newContrast);

	SendKey(EKeyDecContrast);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,newContrast));
	if (contrast==0)
		{
		TEST(maxContrast==newContrast);
		if (maxContrast!=newContrast)
			INFO_PRINTF3(_L("maxContrast==newContrast - Expected: %d, Actual: %d"), maxContrast, newContrast);
		}
	else
		{
		TEST(contrast-diff==newContrast);
		if (contrast-diff!=newContrast)
			INFO_PRINTF3(_L("contrast-diff==newContrast - Expected: %d, Actual: %d"), contrast-diff, newContrast);
		}

	SendKey(EKeyIncContrast);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,newContrast));
	TEST(contrast==newContrast);
	if (contrast!=newContrast)
		INFO_PRINTF3(_L("contrast==newContrast - Expected: %d, Actual: %d"), contrast, newContrast);

	}

void CTKey::ContrastKeysL()
	{
	TInt maxContrast=0;
	TInt contrast=0;
	TInt err=HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrastMax,maxContrast);
	if (err==KErrNotSupported)
		return;
	else if (err==KErrNone)
		err=HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,contrast);
	if (err==KErrNotSupported)
		return;
	TEST(err==KErrNone);
	if (err!=KErrNone)
		INFO_PRINTF3(_L("HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,contrast)=KErrNone - Expected: %d, Actual: %d"), KErrNone, err);

	if (contrast==0)
		err=HAL::Set(iTest->iScreenNumber,HALData::EDisplayContrast,++contrast);
	else if (contrast==maxContrast)
		err=HAL::Set(iTest->iScreenNumber,HALData::EDisplayContrast,--contrast);
	if (err==KErrNotSupported)
		return;
	TEST(err==KErrNone);
	if (err!=KErrNone)
		INFO_PRINTF3(_L("HAL::Get(iTest->iScreenNumber,HALData::EDisplayContrast,contrast)=KErrNone - Expected: %d, Actual: %d"), KErrNone, err);
	TestDefaultContrastL(ETrue);

	SetHotKey(EHotKeyIncContrast,CONTRAST_UP);
	SetHotKey(EHotKeyDecContrast,CONTRAST_DOWN);
	TestContrastL(ETrue);
	TestDefaultContrastL(ETrue);

	TheClient->iWs.ClearHotKeys(EHotKeyIncContrast);
	TheClient->iWs.ClearHotKeys(EHotKeyDecContrast);
	TestContrastL(EFalse);
	TestDefaultContrastL(EFalse);

	TheClient->iWs.RestoreDefaultHotKey(EHotKeyIncContrast);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyDecContrast);
	TestContrastL(EFalse);
	TestDefaultContrastL(ETrue);

	SetHotKey(EHotKeyIncContrast,CONTRAST_UP);
	SetHotKey(EHotKeyDecContrast,CONTRAST_DOWN);
	TestContrastL(ETrue);
	TestDefaultContrastL(ETrue);

	TheClient->iWs.RestoreDefaultHotKey(EHotKeyIncContrast);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyDecContrast);
	TestContrastL(EFalse);
	TestDefaultContrastL(ETrue);
	}

#define BACKLIGHT_ON 'A'	//'o'-'a'+1
#define BACKLIGHT_OFF 'B'	//'p'-'a'+1
#define BACKLIGHT_TOGGLE 'C'	//'q'-'a'+1
/** Test for reaction against backlight ON/OFF/TOGGLE buttons defined above.
  If aChange is false button presses must not affect the state of backlight.
*/
void CTKey::TestBackLightL(TBool aChange)
	{
	TBool state;
	SendCtrlAltShift(BACKLIGHT_ON);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==aChange);
	if (state!=aChange)
		INFO_PRINTF3(_L("state==aChange - Expected: %d, Actual: %d"), aChange, state);

	SendCtrlAltShift(BACKLIGHT_TOGGLE);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==EFalse);
	if (state!=EFalse)
		INFO_PRINTF3(_L("state==EFalse - Expected: %d, Actual: %d"), EFalse, state);

	SendCtrlAltShift(BACKLIGHT_TOGGLE);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==aChange);
	if (state!=aChange)
		INFO_PRINTF3(_L("state==aChange - Expected: %d, Actual: %d"), aChange, state);

	SendCtrlAltShift(BACKLIGHT_OFF);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==EFalse);
	if (state!=EFalse)
		INFO_PRINTF3(_L("state==EFalse - Expected: %d, Actual: %d"), EFalse, state);

	SendCtrlAltShift(BACKLIGHT_TOGGLE);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==aChange);
		if (state!=aChange)
		INFO_PRINTF3(_L("state==aChange - Expected: %d, Actual: %d"), aChange, state);

	SendCtrlAltShift(BACKLIGHT_TOGGLE);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==EFalse);
	if (state!=EFalse)
		INFO_PRINTF3(_L("state==EFalse - Expected: %d, Actual: %d"), EFalse, state);

	}

/** Test for reaction against default backlight ON/OFF/TOGGLE buttons.
  If aChange is false button presses must not affect the state of backlight.
*/
void CTKey::TestDefaultBackLightL(TBool aChange)
	{
	TBool state;
	SendKey(EKeyBacklightOn);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==aChange);
	if (state!=aChange)
		INFO_PRINTF3(_L("state==aChange - Expected: %d, Actual: %d"), aChange, state);

	SendKey(EKeyBacklightToggle);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==EFalse);
	if (state!=EFalse)
		INFO_PRINTF3(_L("state==EFalse - Expected: %d, Actual: %d"), EFalse, state);

	SendKey(EKeyBacklightToggle);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==aChange);
	if (state!=aChange)
		INFO_PRINTF3(_L("state==aChange - Expected: %d, Actual: %d"), aChange, state);

	SendKey(EKeyBacklightOff);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==EFalse);
	if (state!=EFalse)
		INFO_PRINTF3(_L("state==EFalse - Expected: %d, Actual: %d"), EFalse, state);

	SendKey(EKeyBacklightToggle);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==aChange);
	if (state!=aChange)
		INFO_PRINTF3(_L("state==aChange - Expected: %d, Actual: %d"), aChange, state);

	SendKey(EKeyBacklightToggle);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,state));
	TEST(state==EFalse);
	if (state!=EFalse)
		INFO_PRINTF3(_L("state==EFalse - Expected: %d, Actual: %d"), EFalse, state);

	}

void CTKey::BackLightKeysL()
	{
	// Turns backlight off
	TBool prevState;
	TInt err=HAL::Get(iTest->iScreenNumber,HALData::EBacklightState,prevState);
	if (err==KErrNotSupported)
		{
		_LIT(KLog,"Cannot test BackLight Key presses as cannot read its state on screen %d");
		LOG_MESSAGE2(KLog,iTest->iScreenNumber);
		return;
		}
	else if (err==KErrNone)
		err=HAL::Set(iTest->iScreenNumber,HALData::EBacklightState,EFalse);
	if (err==KErrNotSupported)
		{
		_LIT(KLog,"Cannot test BackLight Key presses as cannot set its state on screen %d");
		LOG_MESSAGE2(KLog,iTest->iScreenNumber);
		return;
		}
	TEST(err==KErrNone);
	if (err!=KErrNone)
		{
		_LIT(KLog,"Reading or Setting BackLight state gave error %d on screen %d");
		LOG_MESSAGE3(KLog,err,iTest->iScreenNumber);
		}

	// Tests that default backlight ON/OFF/TOGGLE buttons work
	TestDefaultBackLightL(ETrue);

	// Sets user defined backlight buttons and tests that both default and user defined buttons work
	SetHotKey(EHotKeyBacklightOn,BACKLIGHT_ON);
	SetHotKey(EHotKeyBacklightOff,BACKLIGHT_OFF);
	SetHotKey(EHotKeyBacklightToggle,BACKLIGHT_TOGGLE);
	TestBackLightL(ETrue);
	TestDefaultBackLightL(ETrue);

	// Calls RWsSession::ClearHotKeys and tests that both default and user defined buttons don't work
	TheClient->iWs.ClearHotKeys(EHotKeyBacklightOn);
	TheClient->iWs.ClearHotKeys(EHotKeyBacklightOff);
	TheClient->iWs.ClearHotKeys(EHotKeyBacklightToggle);
	TestBackLightL(EFalse);
	TestDefaultBackLightL(EFalse);

	// Calls RWsSession::RestoreDefaultHotKey and tests that only default buttons work
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyBacklightOn);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyBacklightOff);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyBacklightToggle);
	TestBackLightL(EFalse);
	TestDefaultBackLightL(ETrue);

	// Sets user defined backlight buttons and tests that both default and user defined buttons work
	SetHotKey(EHotKeyBacklightOn,BACKLIGHT_ON);
	SetHotKey(EHotKeyBacklightOff,BACKLIGHT_OFF);
	SetHotKey(EHotKeyBacklightToggle,BACKLIGHT_TOGGLE);
	TestBackLightL(ETrue);
	TestDefaultBackLightL(ETrue);

	// Calls RWsSession::RestoreDefaultHotKey and tests that only default buttons work
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyBacklightOn);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyBacklightOff);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyBacklightToggle);
	TestBackLightL(EFalse);
	TestDefaultBackLightL(ETrue);

	//Restores previous backlight state
	//err=UserHal::SetBacklightOn(prevState);
	err=HAL::Set(iTest->iScreenNumber,HALData::EBacklightState,prevState);
	TEST(err==KErrNone);
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to set BackLight state back to previous state: %d on screen %d, error code: %d");
		LOG_MESSAGE4(KLog,prevState,iTest->iScreenNumber,err);
		}
	}

#define BRIGHTNESS_UP 'N'
#define BRIGHTNESS_DOWN 'J'
/** If aChange is true, checks that programmicaly defined brightness buttons increase and decrease brightness properly.
 If aChange is false, checks that programmicaly defined brightness buttons don't affect brightness.
*/
void CTKey::TestBrightnessL(TBool aChange)
	{
	TInt brightness;
	TInt newBrightness;
	TInt diff=aChange ? 1:0;
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,brightness));

	SendCtrlAltShift(BRIGHTNESS_UP);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,newBrightness));
	TEST(brightness+diff==newBrightness);
	if (brightness+diff!=newBrightness)
		INFO_PRINTF3(_L("brightness+diff==newBrightness - Expected: %d, Actual: %d"), newBrightness, brightness+diff);

	SendCtrlAltShift(BRIGHTNESS_DOWN);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,newBrightness));
	TEST(brightness==newBrightness);
	if (brightness!=newBrightness)
		INFO_PRINTF3(_L("brightness==newBrightness - Expected: %d, Actual: %d"), newBrightness, brightness);

	SendCtrlAltShift(BRIGHTNESS_DOWN);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,newBrightness));
	TEST(brightness-diff==newBrightness);
	if (brightness-diff!=newBrightness)
		INFO_PRINTF3(_L("brightness-diff==newBrightness - Expected: %d, Actual: %d"), newBrightness, brightness-diff);

	SendCtrlAltShift(BRIGHTNESS_UP);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,newBrightness));
	TEST(brightness==newBrightness);
	if (brightness!=newBrightness)
		INFO_PRINTF3(_L("brightness==newBrightness - Expected: %d, Actual: %d"), newBrightness, brightness);
	}

/** If aChange is true, checks that default brightness buttons increase and decrease brightness properly (increase/decrease by 1).
  If aChange is false, checks that default brightness buttons don't affect brightness.
*/
void CTKey::TestDefaultBrightnessL(TBool aChange)
	{
	TInt brightness;
	TInt newBrightness;
	TInt diff=aChange ? 1:0;
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,brightness));

	SendKey(EKeyIncBrightness);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,newBrightness));
	TEST(brightness+diff==newBrightness);
	if (brightness+diff!=newBrightness)
		INFO_PRINTF3(_L("brightness+diff==newBrightness - Expected: %d, Actual: %d"), newBrightness, brightness+diff);

	SendKey(EKeyDecBrightness);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,newBrightness));
	TEST(brightness==newBrightness);
	if (brightness!=newBrightness)
		INFO_PRINTF3(_L("brightness==newBrightness - Expected: %d, Actual: %d"), newBrightness, brightness);

	SendKey(EKeyDecBrightness);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,newBrightness));
	TEST(brightness-diff==newBrightness);
	if (brightness-diff!=newBrightness)
		INFO_PRINTF3(_L("brightness-diff==newBrightness - Expected: %d, Actual: %d"), newBrightness, brightness-diff);

	SendKey(EKeyIncBrightness);
	User::LeaveIfError(HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,newBrightness));
	TEST(brightness==newBrightness);
	if (brightness!=newBrightness)
		INFO_PRINTF3(_L("brightness==newBrightness - Expected: %d, Actual: %d"), newBrightness, brightness);
	}

void CTKey::BrightnessKeysL()
	{
	INFO_PRINTF1(_L("AUTO  Brightness Key Test "));
	TInt maxBrightness;
	TInt brightness=0;		//To stop warning
	// get maximum brightness and current
	TInt err=HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightnessMax,maxBrightness);
	if (err==KErrNotSupported)
		return;
	else if (err==KErrNone)
		err=HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,brightness);
	if (err==KErrNotSupported)
		return;
	INFO_PRINTF1(_L(" Supported"));
	TEST(err==KErrNone);
	if (err!=KErrNone)
		INFO_PRINTF3(_L("HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,brightness) return value - Expected: %d, Actual: %d"), KErrNone, err);

	// sets brightness to be neither maximal nor minimal
	if (brightness==0)
		err=HAL::Set(iTest->iScreenNumber,HALData::EDisplayBrightness,++brightness);
	else if (brightness==maxBrightness)
		err=HAL::Set(iTest->iScreenNumber,HALData::EDisplayBrightness,--brightness);
	if (err==KErrNotSupported)
		return;
	INFO_PRINTF1(_L(" Changeable"));
	TEST(err==KErrNone);
	if (err!=KErrNone)
		INFO_PRINTF3(_L("HAL::Get(iTest->iScreenNumber,HALData::EDisplayBrightness,brightness) return value - Expected: %d, Actual: %d"), KErrNone, err);

	// Test default brightness buttons increase/decrease brightness by 1
	TestDefaultBrightnessL(ETrue);

	// Add new inc/dec buttons, test that default and program defined brightness buttons increase/decrease brightness by 1
	INFO_PRINTF1(_L(" First Test"));
	SetHotKey(EHotKeyIncBrightness,BRIGHTNESS_UP);
	SetHotKey(EHotKeyDecBrightness,BRIGHTNESS_DOWN);
	TestBrightnessL(ETrue);
	TestDefaultBrightnessL(ETrue);

	// Clear brightness buttons, test that default and program defined brightness buttons doesn't affect brightness
	INFO_PRINTF1(_L(" Second Test"));
	TheClient->iWs.ClearHotKeys(EHotKeyIncBrightness);
	TheClient->iWs.ClearHotKeys(EHotKeyDecBrightness);
	TestBrightnessL(EFalse);
	TestDefaultBrightnessL(EFalse);

	// Restore default brightness buttons, test that only default brightness buttons increase/decrease brightness by 1
	INFO_PRINTF1(_L(" Third Test"));
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyIncBrightness);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyDecBrightness);
	TestBrightnessL(EFalse);
	TestDefaultBrightnessL(ETrue);

	// Add new inc/dec buttons, test that default and program defined brightness buttons increase/decrease brightness by 1
	INFO_PRINTF1(_L(" Fourth Test"));
	SetHotKey(EHotKeyIncBrightness,BRIGHTNESS_UP);
	SetHotKey(EHotKeyDecBrightness,BRIGHTNESS_DOWN);
	TestBrightnessL(ETrue);
	TestDefaultBrightnessL(ETrue);

	// Restore default brightness buttons, test that only default brightness buttons increase/decrease brightness by 1
	INFO_PRINTF1(_L(" Fifth Test"));
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyIncBrightness);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyDecBrightness);
	TestBrightnessL(EFalse);
	TestDefaultBrightnessL(ETrue);
	}

void CTKey::TestDefaultScreenSizeModeL(TInt aKeys,TBool aChange)
	{
	TInt screenMode=TheClient->iScreen->CurrentScreenMode();
	TInt key;
	TInt retVal;
	for (key=0;key<aKeys;++key)
		{
		iVisibleGroup->SetMode(key);
		SendKey(EKeyScreenDimension0+key);
		if (aChange)
			{
			retVal = TheClient->iScreen->CurrentScreenMode();
			TEST(retVal==key);
			if (retVal!=key)
				INFO_PRINTF3(_L("TheClient->iScreen->CurrentScreenMode() return value - Expected: %d, Actual: %d"), key, retVal);
			}
		else
			{
			retVal = TheClient->iScreen->CurrentScreenMode();
			TEST(retVal==screenMode);
			if (retVal!=screenMode)
				INFO_PRINTF3(_L("TheClient->iScreen->CurrentScreenMode() return value - Expected: %d, Actual: %d"), screenMode, retVal);
			}
		}
	}

#define DIMENSION0 'P'
#define DIMENSION1 'Q'
#define DIMENSION2 'R'
#define DIMENSION3 'N'
void CTKey::TestScreenSizeModeL(TInt aKeys,TBool aChange)
	{
	TInt screenMode=TheClient->iScreen->CurrentScreenMode();
	TInt key;
	TInt retVal;
	for (key=0;key<aKeys;++key)
		{
		iVisibleGroup->SetMode(key);
		switch (key)
		{
		case 0:
			SendCtrlAltShift(DIMENSION0);
			break;
		case 1:
			SendCtrlAltShift(DIMENSION1);
			break;
		case 2:
			SendCtrlAltShift(DIMENSION2);
			break;
		case 3:
			SendCtrlAltShift(DIMENSION3);
			break;
		default:
			INFO_PRINTF2(_L("Error Invalid Key: %d"),key);
			TEST(EFalse);
			break;
		}
			
		
		if (aChange)
			{
			retVal = TheClient->iScreen->CurrentScreenMode();
			TEST(retVal==key);
			if (retVal!=key)
				INFO_PRINTF3(_L("TheClient->iScreen->CurrentScreenMode() return value - Expected: %d, Actual: %d"), key, retVal);
	
			}
		else
			{
			retVal = TheClient->iScreen->CurrentScreenMode();
			TEST(retVal==screenMode);
			if (retVal!=screenMode)
				INFO_PRINTF3(_L("TheClient->iScreen->CurrentScreenMode() return value - Expected: %d, Actual: %d"), screenMode, retVal);
	
			}
		}
	}

void CTKey::ScreenSizeModeKeysL()
	{
	// The test does nothing if there is only one possible screen mode -> return
	TInt numScreenModes=TheClient->iScreen->NumScreenModes();
	if (numScreenModes<2)
		return;

	// Use at most 4 different modes for the test
	if (numScreenModes>4) {numScreenModes=4;}

	// Set screen mode 3
	TInt screenMode=TheClient->iScreen->CurrentScreenMode();
	iVisibleGroup->SetMode(numScreenModes-1);
	TheClient->iScreen->SetScreenMode(numScreenModes-1);

	// Test default screen mode switch buttons
	TestDefaultScreenSizeModeL(numScreenModes,ETrue);

	// Add new screen mode switch buttons. Test that default and program defined screen mode switch buttons switch modes.
	SetHotKey(EHotKeyScreenDimension0,DIMENSION0);
	SetHotKey(EHotKeyScreenDimension1,DIMENSION1);
	SetHotKey(EHotKeyScreenDimension2,DIMENSION2);
	SetHotKey(EHotKeyScreenDimension3,DIMENSION3);
	TestDefaultScreenSizeModeL(numScreenModes,ETrue);
	TestScreenSizeModeL(numScreenModes,ETrue);

	// Clear all screen mode switch buttons. Test that default and program defined screen mode switch buttons don't affect screen modes
	TheClient->iWs.ClearHotKeys(EHotKeyScreenDimension0);
	TheClient->iWs.ClearHotKeys(EHotKeyScreenDimension1);
	TheClient->iWs.ClearHotKeys(EHotKeyScreenDimension2);
	TheClient->iWs.ClearHotKeys(EHotKeyScreenDimension3);
	TestDefaultScreenSizeModeL(numScreenModes,EFalse);
	TestScreenSizeModeL(numScreenModes,EFalse);

	// Restore default screen mode switch buttons. Test that only default screen mode switch buttons switch screen modes
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyScreenDimension0);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyScreenDimension1);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyScreenDimension2);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyScreenDimension3);
	TestDefaultScreenSizeModeL(numScreenModes,ETrue);
	TestScreenSizeModeL(numScreenModes,EFalse);

	// Add new screen mode switch buttons. Test that default and program defined screen mode switch buttons switch modes.
	SetHotKey(EHotKeyScreenDimension0,DIMENSION0);
	SetHotKey(EHotKeyScreenDimension1,DIMENSION1);
	SetHotKey(EHotKeyScreenDimension2,DIMENSION2);
	SetHotKey(EHotKeyScreenDimension3,DIMENSION3);
	TestDefaultScreenSizeModeL(numScreenModes,ETrue);
	TestScreenSizeModeL(numScreenModes,ETrue);

	// Restore default screen mode switch buttons. Test that only default screen mode switch buttons switch screen modes
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyScreenDimension0);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyScreenDimension1);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyScreenDimension2);
	TheClient->iWs.RestoreDefaultHotKey(EHotKeyScreenDimension3);
	TestDefaultScreenSizeModeL(numScreenModes,ETrue);
	TestScreenSizeModeL(numScreenModes,EFalse);

	// Restore initial screen mode
	iVisibleGroup->SetMode(screenMode);
	TheClient->iScreen->SetScreenMode(screenMode);
	}

void CTKey::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest0,"KeyOfDeath");
	_LIT(KTest1,"Contrast");
	_LIT(KTest2,"BackLight");
	_LIT(KTest3,"Brightness");
	_LIT(KTest4,"Screen Size Mode");
	((CTKeyStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
#ifdef _DEBUG
	_LIT(KTest5,"Orientation");
	_LIT(KTest6,"CycleDisplaySize");
#endif
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0106

@SYMDEF             DEF081259

@SYMTestCaseDesc    Key of Death test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a new client process and runs it.
					Waits until the process is started and simulates Ctrl-Alt-Shift-K press.
					When starts the process assumes that TAutoServer.exe exists in z:\sys\bin directory.
					Otherwise the test will not exit a wait-state and will be aborted.

@SYMTestExpectedResults Checks that the process is dead after the Ctrl-Alt-Shift-K press.
					Check also that the difference between amounts of window groups efore
					death-key press and after death-key press is 1.
*/
		case 1:
			((CTKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0106"));
			iTest->LogSubTest(KTest0);
			KeyOfDeathL();
			break;
		case 2:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0539
*/
			((CTKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0539"));
			iTest->LogSubTest(KTest1);
			ContrastKeysL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0107

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test backlight

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Does nothing if hardware attributes are not supported.
					1. Turns backlight off.
					2. Tests that default backlight ON/OFF/TOGGLE buttons work.
					3. Sets user defined backlight buttons and tests that both default and user defined buttons work.
					4. Calls RWsSession::ClearHotKeys and tests that both default and user defined buttons don't work.
					5. Calls RWsSession::RestoreDefaultHotKey and tests that only default buttons work.
					6. Sets user defined backlight buttons and tests that both default and user defined buttons work.
					7. Calls RWsSession::RestoreDefaultHotKey and tests that only default buttons work.
					8. Restores previous backlight state.

@SYMTestExpectedResults Expects that actions described above give positive result.
						The test also checks that programmatic turn off/on doesn't fail.
*/
		case 3:
			((CTKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0107"));
			iTest->LogSubTest(KTest2);
			BackLightKeysL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0108

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test brightness

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Does nothing if hardware attributes are not supported.
					1. Get maximum brightness and current, test that those operations don't fail
					2. Sets brightness to be neither maximal nor minimal, test that those operations don't fail
					3. Test default brightness buttons increase/decrease brightness by 1
					4. Add new inc/dec buttons, test that default and program defined brightness buttons increase/decrease brightness by 1
					5. Clear brightness buttons, test that default and program defined brightness buttons doesn't affect brightness
					6. Restore default brightness buttons, test that only default brightness buttons increase/decrease brightness by 1
					7. Add new inc/dec buttons, test that default and program defined brightness buttons increase/decrease brightness by 1
					8. Restore default brightness buttons, test that only default brightness buttons increase/decrease brightness by 1

@SYMTestExpectedResults Expects that actions described above give positive result.
						The test also checks that programmatic brightness increase and decrease doesn't fail.
*/
		case 4:
			((CTKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0108"));
			iTest->LogSubTest(KTest3);
			BrightnessKeysL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0109

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test screen mode switching

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test does nothing if there is only one possible screen mode. It uses at most 4 different modes for the test.
					1. Test default screen mode switch buttons
					2. Add new screen mode switch buttons. Test that default and program defined screen mode switch buttons switch modes.
					3. Clear all screen mode switch buttons. Test that default and program defined screen mode switch buttons don't affect screen modes
					4. Restore default screen mode switch buttons. Test that only default screen mode switch buttons switch screen modes
					5. Add new screen mode switch buttons. Test that default and program defined screen mode switch buttons switch modes.
					6. Restore default screen mode switch buttons. Test that only default screen mode switch buttons switch screen modes
					7. Restore initial screen mode



@SYMTestExpectedResults Expects that actions described above give positive result.
*/
		case 5:
			((CTKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0109"));
			iTest->LogSubTest(KTest4);
			ScreenSizeModeKeysL();
			break;
#ifdef _DEBUG			
		case 6:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0540
*/
			((CTKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0540"));
			iTest->LogSubTest(KTest5);
			KeyOfOrientation();
			break;
		case 7:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0541
*/
			((CTKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0541"));
			iTest->LogSubTest(KTest6);
			CycleDisplaySize();
			break;
#endif		
		default:
			((CTKeyStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTKeyStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	((CTKeyStep*)iStep)->RecordTestResultL();
	}


__WS_CONSTRUCT_STEP__(Key)

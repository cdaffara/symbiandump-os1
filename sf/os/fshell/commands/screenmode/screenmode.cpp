// screenmode.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/common.mmh>

#if defined (FSHELL_PLATFORM_S60) && FSHELL_PLATFORM_S60 >= 5
#include "AknCapServerClient.h"	//for screen orientation change
#endif

#include <W32STD.H>

#include <fshell/ioutils.h>
using namespace IoUtils;

class CCmdScreenMode : public CCommandBase
	{
public:
	void PrintTime(const TTime& aTime, TBool aNewline);	
	static const TDesC* EventCodeToString(TUint aEvent);
	static const TDesC* DisplayModeToString(TDisplayMode aMode);
	static const TDesC* RotationToString(CFbsBitGc::TGraphicsOrientation aRotation);
	static const TDesC* EnforcementToString(TScreenModeEnforcement aEnforcement);
	static CCommandBase* NewLC();
	~CCmdScreenMode();
private:
	CCmdScreenMode();
	void PrintCurrentModeDetailL();
	void DoListModeL();
	void DoSetModeL();
	void DoFollowL();
	void DoRotateL();

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);

private:
	RWsSession iWsSes;
	CWsScreenDevice* iWsDev;
	
	TBool iVerbose;
	TInt iOptModeIndex; // Which mode to use.
	
	enum TScreenModeCmd
		{	
		ECmdListMode,
		ECmdSetMode,
		ECmdFollow,
		ECmdRotate
		};
			
	TScreenModeCmd iCommand;
	};

CCommandBase* CCmdScreenMode::NewLC()
	{
	CCmdScreenMode* self = new(ELeave) CCmdScreenMode();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdScreenMode::CCmdScreenMode()
	{
	}

CCmdScreenMode::~CCmdScreenMode()
	{
	delete iWsDev;
	iWsDev = NULL;
	iWsSes.Close();
	}

const TDesC& CCmdScreenMode::Name() const
	{
	_LIT(KName, "screenmode");
	return KName;
	}

void CCmdScreenMode::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgCommand, "command");
	aArguments.AppendEnumL((TInt&)iCommand, KArgCommand);
	}

void CCmdScreenMode::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	aOptions.AppendUintL((TUint&)iOptModeIndex, _L("mode"));
	}

void CCmdScreenMode::PrintCurrentModeDetailL()
	{
	TInt scrMode = iWsDev->CurrentScreenMode();
	TPixelsTwipsAndRotation modeSizeAndRotation;
	iWsDev->GetDefaultScreenSizeAndRotation(modeSizeAndRotation);
	const TDesC* pRotationStr = RotationToString(modeSizeAndRotation.iRotation);
    
	TPoint origin = iWsDev->GetDefaultScreenModeOrigin(); 
	TSize scale = iWsDev->GetCurrentScreenModeScale();
	TPoint scaledOrigin = iWsDev->GetCurrentScreenModeScaledOrigin(); 
	TScreenModeEnforcement screenModeEnforcement = iWsDev->ScreenModeEnforcement();
	const TDesC* pEnforcementStr = EnforcementToString(screenModeEnforcement);
	
	Printf(_L("Current ScreenMode %d,%dx%d,Twips:%dx%d,%S\r\n"), scrMode,
			modeSizeAndRotation.iPixelSize.iWidth, modeSizeAndRotation.iPixelSize.iHeight,
			modeSizeAndRotation.iTwipsSize.iWidth , modeSizeAndRotation.iTwipsSize.iHeight,			
			pRotationStr);
	Printf(_L("Origin:(%d,%d) Scale:%dx%d ScaledOrigin:(%d,%d) %S\r\n"),
			origin.iX, origin.iX, scale.iWidth, scale.iHeight, scaledOrigin.iX, scaledOrigin.iY,
			pEnforcementStr);	
	}

//if VerboseLevel > 0, then print every message received
//
void CCmdScreenMode::DoFollowL()
	{
	const TUint KClientHandle = 0xC0C0C0C0;
	TInt err;
	//TInt verboseLevel = iVerbose.Count();

	RWindowGroup wndGrp(iWsSes);
	err = wndGrp.Construct(KClientHandle);
	LeaveIfErr(err, _L("Could not construct window group"));
	
	CleanupClosePushL(wndGrp);
	err = wndGrp.EnableScreenChangeEvents();
	LeaveIfErr(err, _L("Could not enable screen change events"));
	
	Printf(_L("Tracking screen change events... press 'q' to quit\r\n"));
	
	TWsEvent event;
	TInt eventType;
	TUint eventHandle;
	TTime eventTime;
	TRequestStatus status;
	
	RIoConsoleReadHandle& keyIn = Stdin();
	TRequestStatus statusKey;
	keyIn.WaitForKey(statusKey);
	
	for(;;) 
		{
		iWsSes.EventReady(&status);
WaitAgain:		
		User::WaitForRequest(status, statusKey);
		
		if (status.Int()==KRequestPending)
			{
			err = statusKey.Int();
			ASSERT(err == KErrNone);
			//it must be user press a key, so check if that key is 'q'
			TUint pressedKey = keyIn.KeyCode();
			if (pressedKey=='q' || pressedKey=='Q')
				break;
			else
				{
				keyIn.WaitForKey(statusKey);
				goto WaitAgain;
				}
			}
		
		iWsSes.GetEvent(event);
		
		eventType = event.Type();
		eventHandle = event.Handle();
		eventTime = event.Time(); 

		/*
		if (verboseLevel > 0)
			{
			const TDesC* eventCodeStr = EventCodeToString(eventType);
			Printf(_L(" Event:%d(%S) Handle:0x%08x Time:"), eventType, eventCodeStr,
					eventHandle);
			PrintTime(eventTime, ETrue);
			}
		*/
		
		if (eventType == EEventScreenDeviceChanged && eventHandle==KClientHandle)
			{
			// Get the new screen mode and size
			TInt newMode = iWsDev->CurrentScreenMode();
			TPixelsTwipsAndRotation modeSizeAndRotation;
			iWsDev->GetScreenModeSizeAndRotation(newMode, modeSizeAndRotation);
			
    		Printf(_L("\r\n"));
    		Printf(_L("Screen Device Changed, EventTime:"));
			PrintTime(eventTime, ETrue);
			PrintCurrentModeDetailL();
			}
		}
	
	wndGrp.DisableScreenChangeEvents();
	CleanupStack::PopAndDestroy(&wndGrp);
	}

//S60 proprietary: toggle screen oritation by calling CAknCapServer::RotateScreenL
//
void CCmdScreenMode::DoRotateL()
	{
#if defined (FSHELL_PLATFORM_S60) && FSHELL_PLATFORM_S60 >= 5
	TInt err;
	RAknUiServer akSrv;
	err = akSrv.Connect();
	LeaveIfErr(err, _L("Could not connect to AknCapServer"));
	
	CleanupClosePushL(akSrv);
	
	if (iVerbose)
		{
		Printf(_L("Calling RAknUiServer::RotateScreen() / CAknCapServer::RotateScreenL...\r\n"));
		}
	err = akSrv.RotateScreen();
	LeaveIfErr(err, _L("RotateScreen() failed"));
		
	CleanupStack::PopAndDestroy(&akSrv);
	if (iVerbose)
		{
		PrintCurrentModeDetailL();
		}
#else
	LeaveIfErr(KErrNotSupported, _L("Rotate not supported on this platform."));
#endif	
	}

void CCmdScreenMode::DoSetModeL()
	{
	TInt numScreenModes = iWsDev->NumScreenModes();
	if ( (iOptModeIndex >= numScreenModes) || iOptModeIndex<0)
		{
		LeaveIfErr(KErrArgument, _L("Invalid screenmode index:%d"), iOptModeIndex);
		}
	
	if (iVerbose)
		{
		Printf(_L("Calling CWsScreenDevice::SetScreenMode(%d)\r\n"), iOptModeIndex);
		}
	iWsDev->SetScreenMode(iOptModeIndex);
	
	//verify the result by query current screen mode
	if (iVerbose)
		{
		PrintCurrentModeDetailL();
		}
	}

void CCmdScreenMode::DoListModeL()
	{
	//for platform defined with SYMBIAN_WSERV_AND_CONE_MULTIPLE_SCREENS
	//it would be possible to query how many screens on this device
	//but S60 5.0 doesn't define this
	//TInt Screens = iWsSes.NumberOfScreens();	
	TInt curScreenNum = iWsDev->GetScreenNumber();
	TInt numScreenModes = iWsDev->NumScreenModes();
	Printf(_L("Current Screen number:%d Screen Modes:%d \r\n"), curScreenNum, numScreenModes);
	
	for (TInt i=0; i<numScreenModes; i++)
		{
		Printf(_L("-------------------------------------------\r\n"));
		TDisplayMode dMode = iWsDev->GetScreenModeDisplayMode(i);
		const TDesC* pModeString = DisplayModeToString(dMode);
		TSize modeScale = iWsDev->GetScreenModeScale(i);
		TPoint modeOrigin = iWsDev->GetScreenModeOrigin(i);
		TPoint scaledOrigin = iWsDev->GetScreenModeScaledOrigin(i);
		
		TPixelsTwipsAndRotation modeSizeAndRotation;
		iWsDev->GetScreenModeSizeAndRotation(i, modeSizeAndRotation);
		const TDesC* pRotationStr = RotationToString(modeSizeAndRotation.iRotation);
		
		Printf(_L("Mode:%d DisplayMode:%S Scale(WxH):%dx%d Origin:(%d,%d) ScaledOrigin:(%d,%d)\r\n"), 
				i, pModeString, 
				modeScale.iWidth, modeScale.iHeight,
				modeOrigin.iX, modeOrigin.iY, scaledOrigin.iX, scaledOrigin.iY);
		
		Printf(_L("Size(WxH) Pixels:%dx%d, Twips:%dx%d %S\r\n"), 
				modeSizeAndRotation.iPixelSize.iWidth, modeSizeAndRotation.iPixelSize.iHeight,
				modeSizeAndRotation.iTwipsSize.iWidth, modeSizeAndRotation.iTwipsSize.iHeight,
				pRotationStr);
		}

	//current screen mode
	Printf(_L("===========================================\r\n"));
	PrintCurrentModeDetailL();	
	}

void CCmdScreenMode::DoRunL()
	{
	TInt err;
	err = iWsSes.Connect();
	LeaveIfErr(err, _L("Could not connect to the window server"));
	
	iWsDev = new (ELeave) CWsScreenDevice(iWsSes);
	err = iWsDev->Construct();
	LeaveIfErr(err, _L("Could not construct CWsScreenDevice"));
		
	switch(iCommand)
		{
		case ECmdListMode:
			DoListModeL();
			break;
		case ECmdSetMode:
			DoSetModeL();
			break;
		case ECmdFollow:
			DoFollowL();
			break;
		case ECmdRotate:
			DoRotateL();
			break;
		}	
		
	}

EXE_BOILER_PLATE(CCmdScreenMode)

#define CASE_MODELIT(x) case x: { _LIT(KName, #x); pString = &KName; break; }

const TDesC* CCmdScreenMode::DisplayModeToString(TDisplayMode aMode)
	{
	enum { EColor16MAP = EColor16MA+1 }; // Not defined in 9.1
	const TDesC* pString = NULL;
	switch(aMode)
		{
		CASE_MODELIT(ENone);
		CASE_MODELIT(EGray2);
		CASE_MODELIT(EGray4);
		CASE_MODELIT(EGray16);
		CASE_MODELIT(EGray256);
		CASE_MODELIT(EColor16);
		CASE_MODELIT(EColor256);
		CASE_MODELIT(EColor64K);
		CASE_MODELIT(EColor16M);
		CASE_MODELIT(ERgb);
		CASE_MODELIT(EColor4K);
		CASE_MODELIT(EColor16MU);
		CASE_MODELIT(EColor16MA);
		CASE_MODELIT(EColor16MAP);
		default:
			_LIT(KUnknowStr, "Unknown");
			pString = &KUnknowStr;		
		}
	return pString;
	}

const TDesC* CCmdScreenMode::EnforcementToString(TScreenModeEnforcement aEnforcement)
	{
	const TDesC* pString = NULL;
	switch(aEnforcement)
		{
		CASE_MODELIT(ESizeEnforcementNone);
		CASE_MODELIT(ESizeEnforcementPixelsAndRotation);
		CASE_MODELIT(ESizeEnforcementPixelsTwipsAndRotation);
		default:
			_LIT(KUnknowStr, "Unknown");
			pString = &KUnknowStr;		
		}
	return pString;
	}

const TDesC* CCmdScreenMode::EventCodeToString(TUint aEvent)
	{
	const TDesC* pString = NULL;
	switch(aEvent)
		{
		CASE_MODELIT(EEventNull);
		CASE_MODELIT(EEventKey);
		CASE_MODELIT(EEventKeyUp);
		CASE_MODELIT(EEventKeyDown);
		CASE_MODELIT(EEventModifiersChanged);
		CASE_MODELIT(EEventPointer);
		CASE_MODELIT(EEventPointerEnter);
		CASE_MODELIT(EEventPointerExit);
		CASE_MODELIT(EEventPointerBufferReady);
		CASE_MODELIT(EEventDragDrop);
		CASE_MODELIT(EEventFocusLost);
		CASE_MODELIT(EEventFocusGained);
		CASE_MODELIT(EEventSwitchOn);
		CASE_MODELIT(EEventPassword);
		CASE_MODELIT(EEventWindowGroupsChanged);
		CASE_MODELIT(EEventErrorMessage);
		CASE_MODELIT(EEventMessageReady);
		CASE_MODELIT(EEventMarkInvalid);
		CASE_MODELIT(EEventSwitchOff);
		CASE_MODELIT(EEventKeySwitchOff);
		CASE_MODELIT(EEventScreenDeviceChanged);
		CASE_MODELIT(EEventFocusGroupChanged);
		CASE_MODELIT(EEventCaseOpened);
		CASE_MODELIT(EEventCaseClosed);
		CASE_MODELIT(EEventWindowGroupListChanged);
		CASE_MODELIT(EEventWindowVisibilityChanged);
#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP		
		CASE_MODELIT(EEventRestartSystem);
#endif		
		CASE_MODELIT(EEventKeyRepeat);
		
		//the following 3 are not defined in S60 3.2
#if defined (FSHELL_PLATFORM_S60) && FSHELL_PLATFORM_S60 >= 5
		CASE_MODELIT(EEventGroupWindowOpen);
		CASE_MODELIT(EEventGroupWindowClose);
		CASE_MODELIT(EEventWindowClose);
#endif		
		
		CASE_MODELIT(EEventDirectScreenAccessBegin);
		CASE_MODELIT(EEventDirectScreenAccessEnd);
		CASE_MODELIT(EEventHeartbeatTimerStateChange);
		CASE_MODELIT(EEventPowerMgmt);
		CASE_MODELIT(EEventReserved);
		CASE_MODELIT(EEventUser);
		default:
			_LIT(KUnknowStr, "Unknown");
			pString = &KUnknowStr;
		}
	return pString;
	}

#define CASE_ROTLIT(x) case CFbsBitGc::x: { _LIT(KName, #x); pString = &KName; break; }

const TDesC* CCmdScreenMode::RotationToString(CFbsBitGc::TGraphicsOrientation aRotation)
	{
	const TDesC* pString = NULL;
	switch(aRotation)
		{
		CASE_ROTLIT(EGraphicsOrientationNormal);
		CASE_ROTLIT(EGraphicsOrientationRotated90);
		CASE_ROTLIT(EGraphicsOrientationRotated180);
		CASE_ROTLIT(EGraphicsOrientationRotated270);
		default:
			_LIT(KUnknowStr, "Unknown");
			pString = &KUnknowStr;		
		}
	return pString;
	}

void CCmdScreenMode::PrintTime(const TTime& aTime, TBool aNewline)
	{	
	TTime NullTime = Time::NullTTime();
	if (aTime == NullTime) 
		{
		Printf(_L("(NullTime)"));
		}
	else
		{
		_LIT8(KDateTimeFormat, "%d-%02d-%02d %02d:%02d:%02d");
		TDateTime dt = aTime.DateTime();
		Printf(KDateTimeFormat, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
		}
	
	if (aNewline)
		{
		Printf(_L("\r\n"));
		}
	}

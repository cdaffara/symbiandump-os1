// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/
#include "wsdynamicresbasic.h"
#include <test/extendtef.h>
#include "globalsettings.h"
#include <w32debug.h>
#include <w32stdgraphic.h>
#include "regionextend.h"
#include <fbs.h>
#include <e32svr.h>
#include <u32hal.h>
#include <graphics/surfaceconfiguration.h>
#include "surfaceutility.h"
#include <graphics/testscreencapture.h>

//verify which base code is available to the test
#include <dispchannel.h>
#if (!defined(K_DISPLAY_CH_MAJOR_VERSION_NUMBER) && !defined(K_DISPLAY_CH_MINOR_VERSION_NUMBER))
#define MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
#endif


//#define VISIBLE_PAUSES 1000
#define OVERLAPPING_CELL	0	//'m'	//0 indicates cells do not overlap
/*
 * CWsGceTestPlaceSurfExtra implementation
 * 
 * 
 * 
 * 
 */
CWsDynamicResBasic::CWsDynamicResBasic()
{
}

CWsDynamicResBasic::~CWsDynamicResBasic()
{
}

void CWsDynamicResBasic::SetupL()
	{
	MyBase::SetupL();
	}
void CWsDynamicResBasic::TearDownL()
	{
	MyBase::TearDownL();
	
	}
void CWsDynamicResBasic::TearDownFromDeleteL()
	{
	MyBase::TearDownFromDeleteL();
	}


CTestSuite* CWsDynamicResBasic::CreateSuiteL( const TDesC& aName )
	{
	//Create the surface manager here, before the test suite creates additional threads
	if (GCEIsSupportedStatic())
		{
		TRAP_IGNORE(PostTestCleanupInstance().CreateSharedUtilityL());
		}
	
	SUB_SUITE_OPT(CWsDynamicResBasic,NULL);

		//BASIC
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0001L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0002L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0003L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0004L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0005L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0006L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0007L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0008L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0009L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0010L);	//only run on screen 1
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0011L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0012L);	//only run on screen 0
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0013L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0014L);	//only run on screen 1
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0015L);	//only run with no scaling
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0016L);	//only run with anisotropic scaling
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0017L);	//only run with integer scaling
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0018L);

		ADD_TEST_STEP_PARAM_BOOL(GRAPHICS_WSERV_DYNAMICRES_0021L);	//only run on screen 1
		
		//OOM
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0031L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0032L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0033L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0034L);	//dont run with no scaling on screen 0

		//SCREEN CAPTURE
		ADD_TEST_STEP_PARAM_BOOL(GRAPHICS_WSERV_DYNAMICRES_0041L);
		ADD_TEST_STEP_PARAM_BOOL(GRAPHICS_WSERV_DYNAMICRES_0042L);
		
		//DYNAMIC SCREEN MODE - only run these with no scaling and isotropic scaling
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0051L);	
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0052L);	//only run on screen 1 (multiple resolutions)
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0053L);

		//DEFECT
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0101L);
		ADD_THIS_TEST_STEP(GRAPHICS_WSERV_DYNAMICRES_0102L);

	END_SUITE;
	}

#define LOG_AND_PANIC_IF_NOT_GCE											\
		{																	\
		if (!GCEIsSupported())												\
			{																\
			INFO_PRINTF1(_L("Test skipped: GCE support is not loaded"));	\
			User::Panic(_L("GCE.Wrong.Mode"),1);							\
			return;															\
			}																\
		}

CWsDisplayEvent::CWsDisplayEvent(RWsSession *aWs):
CActive(EPriorityStandard), iWs(aWs), iConfigSpinner(0), iResListSpinner(0),iReceivedEventCount(0), 
iReceivedPointerEventCount(0), iPointerPosTestPass(ETrue) 
	{
	CActiveScheduler::Add(this);
	}
CWsDisplayEvent::~CWsDisplayEvent()
	{
	Cancel();
	}

void CWsDisplayEvent::DoCancel()
	{
	iWs->EventReadyCancel();
	}

void CWsDisplayEvent::Request()
	{
	iWs->EventReady(&iStatus);
	SetActive();
	}

void CWsDisplayEvent::RunL()
	{
	TWsEvent event;
	iWs->GetEvent(event);
	
	if(iStatus == KErrNone)
		{
		switch(event.Type())
			{
			case EEventScreenDeviceChanged:
				{
				iReceivedDeviceEventCount++;
				}
			case EEventPointer:
				{
				iReceivedPointerEventCount++;
				TPointerEvent* ptEvent = event.Pointer();
				//make 1 pixels distance allowance, as non integer scalling from composition to app may occur
				//1 pixels allowance because we never scale greater than 2 times
				if(ptEvent->iPosition.iX < iPointerPos.iX - 1 && ptEvent->iPosition.iX > iPointerPos.iX + 1
						&& ptEvent->iPosition.iY < iPointerPos.iY - 1 &&ptEvent->iPosition.iY > iPointerPos.iY + 1)
					{
					iPointerPosTestPass = EFalse;
					}
				}
			case EEventDisplayChanged:
				{
				iConfigSpinner = event.DisplayChanged()->iConfigurationChangeId;
				iResListSpinner = event.DisplayChanged()->iResolutionListChangeId;
				iReceivedEventCount++;
				}
				break;
			default:
				break;
			}
		}
	else
		User::Leave(iStatus.Int());
	
	Request();
	}

CEventTimer* CEventTimer::NewL()
	{
	CEventTimer* self = new (ELeave) CEventTimer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
      
void CEventTimer::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}
 
CEventTimer::CEventTimer() : CActive(EPriorityStandard)
	{
	
	}

CEventTimer::~CEventTimer()
	{
	Cancel();
	iTimer.Close();
	}

void CEventTimer::DoCancel()
	{
	iTimer.Cancel();
	CActiveScheduler::Stop();
	}

void CEventTimer::RunL()
	{
	CActiveScheduler::Stop();
	}

void CEventTimer::Wait(TInt aDelay)
	{
	iTimer.After(iStatus, aDelay);
	SetActive();
	CActiveScheduler::Start();
	}

//Set display state to normal and to the first decent resolution
//A failed test could leave it in a bad state
void CWsDynamicResBasic::ResetScreens()
	{
	Pause(1000);
#ifndef MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
	TInt displayState = ENormalResolution; 
	UserSvr::HalFunction(EHalGroupDisplay | (iScreenDevice->GetScreenNumber()<<16), EDisplayHalSetDisplayState, &displayState, NULL);
#endif
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	RArray<TInt> appModeIndexList;
	TInt currMode=iScreenDevice->CurrentScreenMode();
	iScreenDevice->GetScreenSizeModeList(&appModeIndexList);
	if (appModeIndexList[0]!=currMode)
		{
		iScreenDevice->SetAppScreenMode(appModeIndexList[0]);
		iScreenDevice->SetScreenMode(appModeIndexList[0]);
		}
	appModeIndexList.Close();
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	RArray<MDisplayControl::TResolution> resolutionList1;
	TInt error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	TDisplayConfiguration config;
	TInt goodRes=0;
	if (resolutionList1[goodRes].iPixelSize==TSize())
		{
		goodRes++;
		ASSERT_TRUE(resolutions>goodRes);		//First res was blank, and no more to choose!
		}
	config.SetResolution(resolutionList1[goodRes].iPixelSize);
	error=interface->SetConfiguration(config);
	ASSERT_EQUALS(error,KErrNone);
	resolutionList1.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0001L
@SYMTestCaseDesc		WSERV Screen Device GetInterface returns known interface
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Basic classic behaviour
@SYMTestActions			
	Call GetInterface with a well known GUID
@SYMTestExpectedResults	
	Returns a pointer
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0001L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	MakeTitleAndCompareWindowsL(_L("GRAPHICS_WSERV_DYNAMICRES_0001L"),_L("Basic Dynamic Resolution test"));
	
	MDisplayControlBase* interface1 = static_cast<MDisplayControlBase*>
			(iScreenDevice->GetInterface(MDisplayControlBase::ETypeId));
	ASSERT_TRUE(interface1);
	
	MDisplayControl* interface2 = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface2);
	
	MDisplayMappingBase* interface3 = static_cast<MDisplayMappingBase*>
			(iScreenDevice->GetInterface(MDisplayMappingBase::ETypeId));
	ASSERT_TRUE(interface3);
	
	MDisplayMapping* interface4 = static_cast<MDisplayMapping*>
			(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	ASSERT_TRUE(interface4);
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0002L
@SYMTestCaseDesc		Basic test for NumberOfResolutions
@SYMREQ					REQ10328
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			NumberOfResolutions returns correctly
@SYMTestActions			
	GetInterface
	then call NumberOfResolutions.
@SYMTestExpectedResults	
	It shouldn't return KErrNotSupported
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0002L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions != KErrNotSupported);
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0003L
@SYMTestCaseDesc		Basic test for GetResolutions
@SYMREQ					REQ10328
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Check GetResolutions returns a valid TSize RArray
@SYMTestActions			
	GetInterface
	Call NumberOfResolutions
	Create an array and garbage fill based on amount of resolutions
	Call GetResolutions on this array
	Create an array (dont garbage fill)
	Call GetResolutions on this empty array
	Create an array with smaller size than required
	Call GetResolutions on this array
	Create an array with larger size than required
	Call GetResolutions on this array
@SYMTestExpectedResults	
	For both GetResolutions calls, it should allocate memory if needed, and fill
	with correct sizes
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0003L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	TInt count;
	TInt error;
	
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	for (count=0;count<resolutions;count++)
		{
		resolutionList1.Append(MDisplayControl::TResolution(TSize(),TSize()));
		}
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	RArray<MDisplayControl::TResolution> resolutionList2;
	error = interface->GetResolutions(resolutionList2);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList2.Count(), resolutions);
	
	RArray<MDisplayControl::TResolution> resolutionList3;
	for (count=0;count<resolutions-1;count++)
		{
		resolutionList3.Append(MDisplayControl::TResolution(TSize(),TSize()));
		}
	error = interface->GetResolutions(resolutionList3);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList3.Count(), resolutions);
	
	RArray<MDisplayControl::TResolution> resolutionList4;
	for (count=0;count<resolutions+2;count++)
		{
		resolutionList4.Append(MDisplayControl::TResolution(TSize(),TSize()));
		}
	error = interface->GetResolutions(resolutionList4);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList4.Count(), resolutions);
	
	for (count=0;count<resolutions;count++)
		{
		ASSERT_EQUALS(resolutionList1[count].iPixelSize,resolutionList2[count].iPixelSize);
		ASSERT_EQUALS(resolutionList1[count].iPixelSize,resolutionList3[count].iPixelSize);
		ASSERT_EQUALS(resolutionList1[count].iPixelSize,resolutionList4[count].iPixelSize);
		
		ASSERT_EQUALS(resolutionList1[count].iTwipsSize,resolutionList2[count].iTwipsSize);
		ASSERT_EQUALS(resolutionList1[count].iTwipsSize,resolutionList3[count].iTwipsSize);
		ASSERT_EQUALS(resolutionList1[count].iTwipsSize,resolutionList4[count].iTwipsSize);
		
		if (!(resolutionList1[count].iFlags == resolutionList2[count].iFlags))
			{
			ASSERT_TRUE(EFalse);
			}
		if (!(resolutionList1[count].iFlags == resolutionList3[count].iFlags))
			{
			ASSERT_TRUE(EFalse);
			}
		if (!(resolutionList1[count].iFlags == resolutionList4[count].iFlags))
			{
			ASSERT_TRUE(EFalse);
			}
		}
	resolutionList1.Close();
	resolutionList2.Close();
	resolutionList3.Close();
	resolutionList4.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0004L
@SYMTestCaseDesc		Test for GetConfiguration
@SYMREQ					REQ10328
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Check GetConfiguration returns a configuration object
@SYMTestActions			
	GetInterface
	GetConfiguration with standard TDisplayConfiguration
	GetResolution from the TDisplayConfiguration object
	GetConfiguration with TDisplayConfiguration object with a greater version/size
	GetResolution from the TDisplayConfiguration object
	GetConfiguration with TDisplayConfiguration object with a smaller version/size -- equal to TDisplayConfiguration1
	GetResolution from the TDisplayConfiguration object
	Check all the resolutions should be same
@SYMTestExpectedResults	
	
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0004L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
			
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TDisplayConfiguration dispConfig;
	interface->GetConfiguration(dispConfig);
	ASSERT_TRUE(dispConfig.IsDefined(TDisplayConfigurationBase::EResolution));
	TSize size;
	TSize twips;
	ASSERT_TRUE(dispConfig.GetResolution(size));
	ASSERT_TRUE(dispConfig.GetResolutionTwips(twips));
	
	TDisplayConfiguration dispConfig1(TDisplayConfiguration().Version() + 10);
	interface->GetConfiguration(dispConfig1);
	ASSERT_TRUE(dispConfig1.IsDefined(TDisplayConfigurationBase::EResolution));
	TSize size1;
	TSize twips1;
	ASSERT_TRUE(dispConfig1.GetResolution(size1));
	ASSERT_TRUE(dispConfig1.GetResolutionTwips(twips1));
	
	TDisplayConfiguration dispConfig2(TDisplayConfiguration1().Version());
	interface->GetConfiguration(dispConfig2);
	ASSERT_TRUE(dispConfig2.IsDefined(TDisplayConfigurationBase::EResolution));
	TSize size2;
	TSize twips2;
	ASSERT_TRUE(dispConfig2.GetResolution(size2));
	ASSERT_TRUE(dispConfig2.GetResolutionTwips(twips2));
	
	ASSERT_EQUALS(size, size1);
	ASSERT_EQUALS(size, size2);
	ASSERT_EQUALS(twips, twips1);
	ASSERT_EQUALS(twips, twips2);
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0005L
@SYMTestCaseDesc		Test getting display change event from event queue
@SYMREQ					REQ10329 REQ10330
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Verify the correct behaviour of display change notification
@SYMTestActions			
	Run through various ways of changing resolution
	For each, check if event has been received
@SYMTestExpectedResults	
	Based on type of change, check event
	@note Test uses HAL to simulate display disconnection. Not supported on production platforms.
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0005L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
#ifdef MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
	ASSERT_FALSE("Display connection test not compiled because MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER");
#else
	TInt screenNo = iScreenDevice->GetScreenNumber();
	
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	interface->EnableDisplayChangeEvents(ETrue);
	ASSERT_TRUE(interface->DisplayChangeEventsEnabled());
	
	CWsDisplayEvent* displayEventAO = new(ELeave) CWsDisplayEvent(&iSession);
	CleanupStack::PushL(displayEventAO);
	displayEventAO->Request();
	CEventTimer *timer = CEventTimer::NewL();
	CleanupStack::PushL(timer);
	TInt dispSpinnerValue = displayEventAO->ResListSpinner();
	TInt configSpinnerValue = displayEventAO->ConfigSpinner();
	
	iSession.Flush();		
	TInt displayState = EDisconnect;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	
	timer->Wait(100000);
	ASSERT_TRUE(displayEventAO->ConfigSpinner() >= configSpinnerValue); //this is also a config change
	ASSERT_TRUE(displayEventAO->ResListSpinner() > dispSpinnerValue); //1st display change event
	dispSpinnerValue = displayEventAO->ResListSpinner();
	configSpinnerValue = displayEventAO->ConfigSpinner();
	
	
	interface->EnableDisplayChangeEvents(ETrue);//enable again
	ASSERT_TRUE(interface->DisplayChangeEventsEnabled());
	interface->EnableDisplayChangeEvents(EFalse);//disable
	ASSERT_FALSE(interface->DisplayChangeEventsEnabled());
	interface->EnableDisplayChangeEvents(EFalse);//disable again
	interface->EnableDisplayChangeEvents(ETrue);//enable again
	iSession.Flush();
	
	TDisplayConfiguration disconnectedConfig;
	disconnectedConfig.SetResolution(TSize(10,11));
	disconnectedConfig.SetResolutionTwips(TSize(12,13));
	disconnectedConfig.SetRotation(TDisplayConfiguration1::ERotationNormal);
	interface->GetConfiguration(disconnectedConfig);
	ASSERT_FALSE(disconnectedConfig.IsDefined(disconnectedConfig.EResolution));
	ASSERT_FALSE(disconnectedConfig.IsDefined(disconnectedConfig.EResolutionTwips));
	ASSERT_FALSE(disconnectedConfig.IsDefined(disconnectedConfig.ERotation));
	
	displayState = ENormalResolution; 
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	
	timer->Wait(1000000);
	ASSERT_TRUE(displayEventAO->ConfigSpinner() >= configSpinnerValue); //this is a config change if display policy is enabled. not a change otherwise
	ASSERT_TRUE(displayEventAO->ResListSpinner() > dispSpinnerValue); //display change event	
	dispSpinnerValue = displayEventAO->ResListSpinner();
	configSpinnerValue = displayEventAO->ConfigSpinner();
	
	TDisplayConfiguration dispConfig, dispConfig2;
	TSize resolution,resolution2;
	interface->GetConfiguration(dispConfig2);
	dispConfig2.GetResolution(resolution2);
	RArray<MDisplayControl::TResolution> resolutions;
	TInt err = interface->GetResolutions(resolutions);
	ASSERT_EQUALS(err, KErrNone);
	dispConfig.SetResolution(resolutions[resolutions.Count()/2].iPixelSize);
	err = interface->SetConfiguration(dispConfig);
	ASSERT_EQUALS(err, KErrNone);
	interface->GetConfiguration(dispConfig);
	
	timer->Wait(100000);
	
	if(!(dispConfig2 == dispConfig))
		{
		ASSERT_TRUE(displayEventAO->ConfigSpinner() > configSpinnerValue); //a config change, but on screen 0 with DP disabled, as we only have 1 resolution when DP disabled
		}
	else
		{
		ASSERT_TRUE(displayEventAO->ConfigSpinner() == configSpinnerValue);
		}
	ASSERT_EQUALS(displayEventAO->ResListSpinner(), dispSpinnerValue); //this is not a display change event	
	dispSpinnerValue = displayEventAO->ResListSpinner();
	configSpinnerValue = displayEventAO->ConfigSpinner();
	resolutions.Close();
	
	//here we flood the event queue with key events, so the next display change
	//can't put event and queue and force the retry AO on server side to kick off
	iSession.SetFocusScreen(TGlobalSettings::Instance().iScreen);
	for(TInt scanCode = 'a'; scanCode < 't'; scanCode ++)
		{
		TRawEvent rawEvent;
		rawEvent.Set(TRawEvent::EKeyDown,scanCode);
		iSession.SimulateRawEvent(rawEvent);
		rawEvent.Set(TRawEvent::EKeyUp,scanCode);
		iSession.SimulateRawEvent(rawEvent);
		}
	iSession.Flush();

	TInt eventCount1 = displayEventAO->ReceivedEventCount(); 
	//we should still receive this
	displayState = EDisconnect; 
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	//let retry AO retry more times before we try to get it
	User::After(1000000);
	
	timer->Wait(10000000);
	ASSERT_TRUE(displayEventAO->ConfigSpinner()> configSpinnerValue ); //4th config change
	ASSERT_TRUE(displayEventAO->ResListSpinner() > dispSpinnerValue); //4th display change event
	
	//This part is doomed as well. Now the received event count includes all the pointer, device and display change event
	/*
	TInt eventCount2 = displayEventAO->ReceivedEventCount();
	//make sure retry AO is stopped - we only receive 1 display event
	ASSERT_EQUALS(eventCount1 + 1, eventCount2);
	*/
	displayState = ENormalResolution; 
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	
	CleanupStack::PopAndDestroy(2, displayEventAO);
#endif
	}

//Adds the first rotation defined in the TResolution to the display configuration
void CWsDynamicResBasic::SetRotation (TDisplayConfiguration& aConfig, const MDisplayControl::TResolution& aResolution)
	{
	if (aResolution.iFlags.IsSet(MDisplayControlBase::TResolution::ERotationNormalSupported))
		{
		aConfig.SetRotation(TDisplayConfiguration1::ERotationNormal);
		}
	else if (aResolution.iFlags.IsSet(MDisplayControlBase::TResolution::ERotation90Supported))
		{
		aConfig.SetRotation(TDisplayConfiguration1::ERotation90CW);
		}
	else if (aResolution.iFlags.IsSet(MDisplayControlBase::TResolution::ERotation180Supported))
		{
		aConfig.SetRotation(TDisplayConfiguration1::ERotation180);
		}
	else if (aResolution.iFlags.IsSet(MDisplayControlBase::TResolution::ERotation270Supported))
		{
		aConfig.SetRotation(TDisplayConfiguration1::ERotation270CW);
		}
	ASSERT_TRUE(aConfig.IsDefined(aConfig.ERotation));
	}
	
/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0006L
@SYMTestCaseDesc		Set Screen display res
@SYMREQ					REQ10326
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Checking window and layer extents continue working
@SYMTestActions			
	Get resolution list
	For every resolution
		Set resolution
		Draw many rectangles that shrink to demonstrate scaling issues
		Draw surface
		Move window around
@SYMTestExpectedResults	
	Every resolution that is supported by current app mode should successfully be set
	All rectangles and surface should be visible
	As the window moves, the rectangles and surface should move with it, keeping visible
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0006L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
	TInt error;
	MakeTitleAndCompareWindowsL(_L("GFX_WSERV_DYNAMICRES_0006L"),_L("Change Resolution"));

	iTestBack=RBlankWindow(iSession);
	ASSERT_EQUALS(iTestBack.Construct(iGroup, ++iWindowHandle), KErrNone);
	iTestBack.SetRequiredDisplayMode(iDisplayMode);
	iTestBack.SetColor(TRgb(255,0,0));
	iTestBack.SetExtent(iTestPos.iTl,iTestPos.Size());
	iTestBack.Activate();
	iTestBack.SetVisible(ETrue);
	
	struct DrawCompare
		{
		static void Draw(CWindowGc*	aGc)
			{
			aGc->SetPenStyle(aGc->ESolidPen);
			aGc->SetPenColor(TRgb(255,255,255));
			aGc->SetBrushColor(TRgb(0,0,0));
			TRect r(2,2,82,82);
			while(r.Width()>0)
				{
				aGc->DrawRect(r);
				r.Shrink(2,2);
				}
			}
		};
	TSurfaceId surfaceID;
	TRAPD(err, surfaceID = iUtility->CreateSurfaceL(TSize(200,200), 
			KSurfaceFormat, 200 * KBytesPerPixel));
	ASSERT_EQUALS(err,KErrNone);
	TRAP(err,iUtility->FanFillSurfaceL(surfaceID,iYellow,iRed,iMagenta));
	ASSERT_EQUALS(err,KErrNone);
	TSurfaceConfiguration surfConf;
	surfConf.SetSurfaceId(surfaceID);
	
	surfConf.SetExtent(TRect(2,84,82,166));
	iCompare.SetBackgroundSurface(surfConf,EFalse);
	
	if (CWindowGc*	gc=BeginActivateWithWipe(ETrue,iCompare,TRgb(128,128,128)))
		{
		DrawCompare::Draw(gc);
		gc->Deactivate();
		iCompare.EndRedraw();
		
		}
	iSession.Finish(ETrue);
	Pause(1000);
	
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TDisplayConfiguration dispConfigBefore;
	interface->GetConfiguration(dispConfigBefore);
	ASSERT_TRUE(dispConfigBefore.IsDefined(dispConfigBefore.EResolution))
	if (resolutions>1)
		{
		//Got a mode to change to!
		TSize resBefore;
		TBool ok=dispConfigBefore.GetResolution(resBefore);
		ASSERT_TRUE(ok);	//we "know" this should be ok, as we just asserted the flag
		TBool foundBeforeRes=EFalse;
		for (TInt res=0;res<resolutionList1.Count();res++)
			{
//			if (resolutionList1[res].iPixelSize!=resBefore)
				{
				if (resolutionList1[res].iPixelSize!=TSize(0,0))	//this test is not about turning the display off
					{
					TDisplayConfiguration dispConfigReq;
					dispConfigReq.SetResolution(resolutionList1[res].iPixelSize);
					ASSERT_TRUE(dispConfigReq.IsDefined(dispConfigReq.EResolution));
					
					SetRotation(dispConfigReq,resolutionList1[res]);	//set rotation to first defined in res list
					
					TInt errCode=interface->SetConfiguration(dispConfigReq);
					//ASSERT_EQUALS(errCode,KErrNone);
					if (errCode != KErrNone)
						{
						//Probably current size mode does not support the rotation of the passed in configuration
						
						ASSERT_EQUALS(errCode,KErrArgument);	//failed to find compatible res in the policy
						continue;
						}
					Pause(300);
					TRect outerrect(iTestPos.iTl,resolutionList1[res].iPixelSize.AsPoint());
					iTestBack.SetExtent(outerrect.iTl,outerrect.Size());
					iSession.Finish(ETrue);
					Pause(300);
					if (CWindowGc*	gc=BeginActivateWithWipe(ETrue,iCompare,TRgb(128,128,128)))
						{
						DrawCompare::Draw(gc);
						gc->Deactivate();
						iCompare.EndRedraw();
						}
					TBuf16<20> s;
					s.AppendNum(res);
					UpdateTitleWindowL(s,2);
					s.Zero();
					s.AppendNum(resolutionList1[res].iPixelSize.iWidth);
					s.Append('x');
					s.AppendNum(resolutionList1[res].iPixelSize.iHeight);
					UpdateTitleWindowL(s,3);
					
					iSession.Finish(ETrue);
					Pause(300);
					outerrect.iBr.iX-=5;
					outerrect.iBr.iY-=5;
					iTestBack.SetExtent(outerrect.iTl,outerrect.Size());
					iSession.Finish(ETrue);
					Pause(300);
					TPoint winpos=iCompare.AbsPosition();
					TSize winsize=iCompare.Size();
//					iInfoScreenDevice->ReleaseTwipsCache();
					RepaintTitleWindowL();
					for (TSize z=TSize(1,1);z!=TSize(-1,-1);)
						{
						for (TInt i=0;i<15;i++)
							{
							winpos+=z;
							iCompare.SetExtent(winpos,winsize);
							iSession.Finish(ETrue);
							Pause(100);
							}
						if (z.iHeight && z.iWidth)
							z=TSize(0,-1);
						else
						if (z.iHeight)
							z=TSize(-1,0);
						else
							z=TSize(-1,-1);
						}
					TDisplayConfiguration dispConfigAfter;
					interface->GetConfiguration(dispConfigAfter);
					TSize resAfter;
					TBool ok=dispConfigAfter.GetResolution(resAfter);
					ASSERT_TRUE(ok);
					if (resolutionList1[res].iPixelSize!=resAfter)
						{
						INFO_PRINTF3(_L("Did not receive expected resolution %ix%i"),resolutionList1[res].iPixelSize.iWidth,resolutionList1[res].iPixelSize.iHeight);
						INFO_PRINTF3(_L("Instead got %ix%i"),resAfter.iWidth,resAfter.iHeight);
						}
					}
				}
			if (resolutionList1[res].iPixelSize==resBefore)
				{
				foundBeforeRes=ETrue;
				}
			}
		ASSERT_TRUE(foundBeforeRes);
		//restore initial res
		interface->SetConfiguration(dispConfigBefore);
		Pause(1000);
		TDisplayConfiguration dispConfigAfter;
		interface->GetConfiguration(dispConfigAfter);
		TSize resAfter;
		ok=dispConfigAfter.GetResolution(resAfter);
		ASSERT_TRUE(ok);	
		ASSERT_EQUALS(resBefore,resAfter);
		}
	else
		{
		INFO_PRINTF1(_L("Only 1 resolution configured on this screen - res change test skipped."));
		}
	resolutionList1.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0007L
@SYMTestCaseDesc		Coordinates Mapping
@SYMREQ					REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Basic call to mapcoordinates
@SYMTestActions			
	Call mapCoordinates
@SYMTestExpectedResults	
	targetRect should be correctly filled in with same rect as sourceRect
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0007L()
	{
	MDisplayMapping* interface = static_cast<MDisplayMapping*>
			(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	ASSERT_TRUE(interface);
	TRect sourceRect(10, 10, 30, 50);
	TRect targetRect(5,5,10,10);
	TInt err = interface->MapCoordinates(EApplicationSpace, sourceRect, EApplicationSpace, targetRect);
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_EQUALS(sourceRect, targetRect);
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0008L
@SYMTestCaseDesc		SetModes and mapCoordinates
@SYMREQ					REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Basic SetSizeMode and complex MapCoordinates function
@SYMTestActions			
	Set a new size mode
	Perform many rect conversions using MapCoordinates
@SYMTestExpectedResults	
	Ensure that through multiple conversions, accuracy is not lost
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0008L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
	TInt error;
	MakeTitleAndCompareWindowsL(_L("GFX_WSERV_DYNAMICRES_0008L"),_L("Change Size Mode"));

    iTestBack=RBlankWindow(iSession);
    ASSERT_EQUALS(iTestBack.Construct(iGroup, ++iWindowHandle), KErrNone);
    iTestBack.SetRequiredDisplayMode(iDisplayMode);
    iTestBack.SetColor(TRgb(255,0,0));
    iTestBack.SetExtent(iTestPos.iTl,iTestPos.Size());
    iTestBack.Activate();
    iTestBack.SetVisible(ETrue);
    iSession.Finish(ETrue);
	
	
	if (CWindowGc*	gc=BeginActivateWithWipe(ETrue,iCompare,TRgb(128,128,128)))
		{
		gc->SetPenStyle(gc->ESolidPen);
		gc->SetPenColor(TRgb(255,255,255));
		gc->SetBrushColor(TRgb(0,0,0));
		TRect r(2,2,22,22);
		while(r.Width()>0)
			{
			gc->DrawRect(r);
			r.Shrink(2,2);
			}
		gc->Deactivate();
		iCompare.EndRedraw();
		
		}
	iSession.Finish(ETrue);
	Pause(1000);
	
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TDisplayConfiguration dispConfigBefore;
	interface->GetConfiguration(dispConfigBefore);
	ASSERT_TRUE(dispConfigBefore.IsDefined(dispConfigBefore.EResolution));
	
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	ASSERT_TRUE (screenModeList.Count() > 1);
	iScreenDevice->SetAppScreenMode(screenModeList[1]);
	iScreenDevice->SetScreenMode(screenModeList[1]);
	iSession.Finish(ETrue);
	Pause(1000);
	
	MDisplayMapping* interfaceM = static_cast<MDisplayMapping*>
			(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	TRect appRect(20, 40, 193, 110);
	TRect uiRect(1,2,3,4);
	TRect compRect(5,6,7,8);
	TRect tempRect(10,11,12,13);
	TInt err;
	while (ETrue)
		{
		//quick get for a print
		err = interfaceM->MapCoordinates(EApplicationSpace, appRect, EFullScreenSpace, uiRect);
		err = interfaceM->MapCoordinates(EApplicationSpace, appRect, ECompositionSpace, compRect);
		
//		print
//		INFO_PRINTF5(_L("appRect : %d , %d - %d , %d"),appRect.iTl.iX,appRect.iTl.iY,appRect.iBr.iX,appRect.iBr.iY);
//		INFO_PRINTF5(_L("uiRect : %d , %d - %d , %d"),uiRect.iTl.iX,uiRect.iTl.iY,uiRect.iBr.iX,uiRect.iBr.iY);
//		INFO_PRINTF5(_L("compRect : %d , %d - %d , %d"),compRect.iTl.iX,compRect.iTl.iY,compRect.iBr.iX,compRect.iBr.iY);
//		INFO_PRINTF1(_L("  "));
		
		//longer process to insure conversions both ways dont lose accuracy
		err = interfaceM->MapCoordinates(EApplicationSpace, appRect, EFullScreenSpace, uiRect);
		err = interfaceM->MapCoordinates(EFullScreenSpace, uiRect, EApplicationSpace, tempRect);
		ASSERT_EQUALS(tempRect,appRect);
		
		err = interfaceM->MapCoordinates(EApplicationSpace, appRect, ECompositionSpace, compRect);
		err = interfaceM->MapCoordinates(ECompositionSpace, compRect, EApplicationSpace, tempRect);
		ASSERT_EQUALS(tempRect,appRect);
		
		err = interfaceM->MapCoordinates(EApplicationSpace, appRect, EFullScreenSpace, uiRect);
		err = interfaceM->MapCoordinates(EFullScreenSpace, uiRect, ECompositionSpace, compRect);
		err = interfaceM->MapCoordinates(ECompositionSpace, compRect, EFullScreenSpace, tempRect);
		ASSERT_EQUALS(tempRect,uiRect);
		
		appRect.iBr.iX--;
		appRect.iBr.iY--;
		if (appRect.IsEmpty())
			{
			break;
			}
		}
	
	TRect rect1(5,5,10,10);
	TRect rect2(15,15,20,20);
	TRect rect3(50,50,50,50);
	//1
	err = interfaceM->MapCoordinates(ECompositionSpace, rect1, ECompositionSpace, rect2);
	err = interfaceM->MapCoordinates(EFullScreenSpace, rect2, ECompositionSpace, rect3);
	ASSERT_EQUALS(rect1,rect2);
	
	err = interfaceM->MapCoordinates(ECompositionSpace, rect1, EFullScreenSpace, rect2);
	err = interfaceM->MapCoordinates(EFullScreenSpace, rect2, ECompositionSpace, rect3);
	ASSERT_EQUALS(rect1,rect3);

	err = interfaceM->MapCoordinates(ECompositionSpace, rect1, EApplicationSpace, rect2);
	err = interfaceM->MapCoordinates(EApplicationSpace, rect2, ECompositionSpace, rect3);
	ASSERT_EQUALS(rect1,rect3);
	
	err = interfaceM->MapCoordinates(ECompositionSpace, rect1, EDirectScreenAccessSpace, rect2);
	err = interfaceM->MapCoordinates(EDirectScreenAccessSpace, rect2, ECompositionSpace, rect3);
	ASSERT_EQUALS(rect1,rect3);
	
	//2
	err = interfaceM->MapCoordinates(EFullScreenSpace, rect1, EFullScreenSpace, rect2);
	ASSERT_EQUALS(rect1,rect2);
	
	err = interfaceM->MapCoordinates(EFullScreenSpace, rect1, EApplicationSpace, rect2);
	err = interfaceM->MapCoordinates(EApplicationSpace, rect2, EFullScreenSpace, rect3);
	ASSERT_EQUALS(rect1,rect3);
	
	err = interfaceM->MapCoordinates(EFullScreenSpace, rect1, EDirectScreenAccessSpace, rect2);
	err = interfaceM->MapCoordinates(EDirectScreenAccessSpace, rect2, EFullScreenSpace, rect3);
	ASSERT_EQUALS(rect1,rect3);
	
	//3
	err = interfaceM->MapCoordinates(EApplicationSpace, rect1, EApplicationSpace, rect2);
	ASSERT_EQUALS(rect1,rect2);
	
	err = interfaceM->MapCoordinates(EApplicationSpace, rect1, EDirectScreenAccessSpace, rect2);
	err = interfaceM->MapCoordinates(EDirectScreenAccessSpace, rect2, EApplicationSpace, rect3);
	ASSERT_EQUALS(rect1,rect3);
	
	//4
	err = interfaceM->MapCoordinates(EDirectScreenAccessSpace, rect1, EDirectScreenAccessSpace, rect2);
	ASSERT_EQUALS(rect1,rect2);
	
	//invalid
	err = interfaceM->MapCoordinates(ECompositionSpace, rect1, (TCoordinateSpace)0, rect2);
	ASSERT_EQUALS(err,KErrNotSupported);
	err = interfaceM->MapCoordinates(EFullScreenSpace, rect1, (TCoordinateSpace)0, rect2);
	ASSERT_EQUALS(err,KErrNotSupported);
	err = interfaceM->MapCoordinates(EApplicationSpace, rect1, (TCoordinateSpace)0, rect2);
	ASSERT_EQUALS(err,KErrNotSupported);
	err = interfaceM->MapCoordinates(EDirectScreenAccessSpace, rect1, (TCoordinateSpace)0, rect2);
	ASSERT_EQUALS(err,KErrNotSupported);
	err = interfaceM->MapCoordinates((TCoordinateSpace)0, rect1, EDirectScreenAccessSpace, rect2);
	ASSERT_EQUALS(err,KErrNotSupported);
	
	TDisplayConfiguration dispConfigAfter;
	interface->GetConfiguration(dispConfigAfter);

	screenModeList.Close();
	resolutionList1.Close();
	iScreenDevice->SetAppScreenMode(0);
	iScreenDevice->SetScreenMode(0);
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0009L
@SYMTestCaseDesc		Set Screen display res using app mode
@SYMREQ					REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			SetScreenMode functions correctly
@SYMTestActions			
	Similar to 0006, but setting every available screen mode instead of resolution
@SYMTestExpectedResults	
	Every set should work, including modes that involve a rotation!
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0009L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
	TInt error;
	iSession.Finish(ETrue);
	MakeTitleAndCompareWindowsL(_L("GFX_WSERV_DYNAMICRES_0009L"),_L("Change Size Mode"));
	iSession.Finish(ETrue);

    iTestBack=RBlankWindow(iSession);
    ASSERT_EQUALS(iTestBack.Construct(iGroup, ++iWindowHandle), KErrNone);
    iTestBack.SetRequiredDisplayMode(iDisplayMode);
    iTestBack.SetColor(TRgb(255,0,0));
    iTestBack.SetExtent(iTestPos.iTl,iTestPos.Size());
    iTestBack.Activate();
    iTestBack.SetVisible(ETrue);
	iSession.Finish(ETrue);
	
	struct DrawCompare
		{
		static void Draw(CWindowGc*	aGc)
			{
			aGc->SetPenStyle(aGc->ESolidPen);
			aGc->SetPenColor(TRgb(255,255,255));
			aGc->SetBrushColor(TRgb(0,0,0));
			TRect r(2,2,82,82);
			while(r.Width()>0)
				{
				aGc->DrawRect(r);
				r.Shrink(2,2);
				}
			}
		};
	TSurfaceId surfaceID;
	TRAPD(err, surfaceID = iUtility->CreateSurfaceL(TSize(200,200), 
			KSurfaceFormat, 200 * KBytesPerPixel));
	ASSERT_EQUALS(err,KErrNone);
	TRAP(err,iUtility->FanFillSurfaceL(surfaceID,iYellow,iRed,iMagenta));
	ASSERT_EQUALS(err,KErrNone);
	TSurfaceConfiguration surfConf;
	surfConf.SetSurfaceId(surfaceID);
	
	surfConf.SetExtent(TRect(2,84,82,166));
	iCompare.SetBackgroundSurface(surfConf,EFalse);
	
	if (CWindowGc*	gc=BeginActivateWithWipe(ETrue,iCompare,TRgb(128,128,128)))
		{
		DrawCompare::Draw(gc);
		gc->Deactivate();
		iCompare.EndRedraw();
		
		}
	iSession.Finish(ETrue);
	Pause(1000);
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TDisplayConfiguration dispConfigBefore;
	interface->GetConfiguration(dispConfigBefore);
	ASSERT_TRUE(dispConfigBefore.IsDefined(dispConfigBefore.EResolution))
	if (screenModeList.Count()>1)
		{
		//Got a mode to change to!
		TSize resBefore;
		TBool ok=dispConfigBefore.GetResolution(resBefore);
		ASSERT_TRUE(ok);	//we "know" this should be ok, as we just asserted the flag
		for (TInt res=0;res<screenModeList.Count();res++)
			{
				{
					{
					TDisplayConfiguration dispConfigReq;
					iInfoScreenDevice->SetAppScreenMode(screenModeList[res]);
					iInfoScreenDevice->SetScreenMode(screenModeList[res]);
					iSession.Finish(ETrue);
					Pause(300);
					
					TPixelsAndRotation pr;
					iInfoScreenDevice->GetDefaultScreenSizeAndRotation(pr);
					TPoint 	origin=iInfoScreenDevice->GetDefaultScreenModeOrigin();
					Pause(300);
					
					if (CWindowGc*	gc=BeginActivateWithWipe(ETrue,iCompare,TRgb(128,128,128)))
						{
						DrawCompare::Draw(gc);
						gc->Deactivate();
						iCompare.EndRedraw();
						}
					if (iTitle.WsHandle())
						{
						TPoint	infoWinPos;
						TSize	infoWinSize;
						infoWinPos=iTitle.AbsPosition();
						infoWinSize=iTitle.Size();
						infoWinSize.iHeight=pr.iPixelSize.iHeight-2*infoWinPos.iY;
						iTitle.SetSize(infoWinSize);
						}
					TBuf16<20> s;
					s.AppendNum(res);
					UpdateTitleWindowL(s,1);
					s.Zero();
					s.AppendNum(pr.iPixelSize.iWidth);
					s.Append('x');
					s.AppendNum(pr.iPixelSize.iHeight);
					UpdateTitleWindowL(s,2);
					s.Zero();
					s.AppendNum(origin.iX);
					s.Append(',');
					s.AppendNum(origin.iY);
					UpdateTitleWindowL(s,3);
					
					iSession.Finish(ETrue);
					Pause(300);
					
					TPoint winpos=iCompare.AbsPosition();
					TSize winsize=iCompare.Size();

					for (TSize z=TSize(1,1);z!=TSize(-1,-1);)
						{
						for (TInt i=0;i<15;i++)
							{
							winpos+=z;
							iCompare.SetExtent(winpos,winsize);
							iSession.Finish(ETrue);
							Pause(100);
							}
						if (z.iHeight && z.iWidth)
							z=TSize(0,-1);
						else
						if (z.iHeight)
							z=TSize(-1,0);
						else
							z=TSize(-1,-1);
						}
					}
				}
			}
		//restore initial res
		if (screenModeList.Count() > 1)
            {   //set back to basics
            iInfoScreenDevice->SetAppScreenMode(screenModeList[0]);
            iInfoScreenDevice->SetScreenMode(screenModeList[0]);
            Pause(300);
            }
		
		interface->SetConfiguration(dispConfigBefore);
		Pause(1000);
		TDisplayConfiguration dispConfigAfter;
		interface->GetConfiguration(dispConfigAfter);
		TSize resAfter;
		ok=dispConfigAfter.GetResolution(resAfter);
		ASSERT_TRUE(ok);	
		ASSERT_EQUALS(resBefore,resAfter);
		}
	else
		{
		INFO_PRINTF1(_L("Only 1 screen size mode configured on this screen - res change test skipped."));
		}

	screenModeList.Close();
	resolutionList1.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0010L
@SYMTestCaseDesc		Test app mode setting during attach/detach
@SYMREQ					REQ10330
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Make sure display resumes last app mode when reattached
@SYMTestActions			
	Save app mode and display configuration before detach and compare it with those after reattach.
	Repeat for a second app mode
	If available, repeat with dynamic app mode
@SYMTestExpectedResults	
	For normal app mode, on reconnect it should re apply the old configuration
	For dynamic app mode, on reconnect it should be 0x0	
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0010L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
#ifdef MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
	ASSERT_FALSE("Test not compiled because MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER");
#else
	TInt error;
	iSession.Finish(ETrue);
	MakeTitleAndCompareWindowsL(_L("GFX_WSERV_DYNAMICRES_0010L"),_L("detach/attach"));
	iSession.Finish(ETrue);

	Pause(1000);
	TInt screenNo = iScreenDevice->GetScreenNumber();
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	
	//Find 2 real app modes and 1 dynamic app mode
	TInt firstAvailableAppMode = -1;
	TInt secondAvailableAppMode = -1;
	TInt dynamicAppMode = -1;
	for (TInt ii = 0; ii < screenModeList.Count(); ii++)
		{
		TBool dynamic = iInfoScreenDevice->IsModeDynamic(screenModeList[ii]);
		if (dynamic && dynamicAppMode == -1)
			{
			dynamicAppMode = screenModeList[ii];
			}
		if (!dynamic)
			{
			if (firstAvailableAppMode == -1)
				{
				firstAvailableAppMode = screenModeList[ii];
				}
			else if (secondAvailableAppMode == -1)
				{
				secondAvailableAppMode = screenModeList[ii];
				}
			}
		}
	ASSERT_TRUE(firstAvailableAppMode!=-1);
	ASSERT_TRUE(secondAvailableAppMode!=-1);
	
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	MDisplayMapping* mappingInterface = static_cast<MDisplayMapping*>
				(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	ASSERT_TRUE(mappingInterface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TDisplayConfiguration configBeforeDetach, configAfterReattach;
	TInt appModeBeforeDetach, appModeAfterReattach;

	//
	//1st detach - attach sequence
	appModeBeforeDetach = firstAvailableAppMode;
	iScreenDevice->SetAppScreenMode(appModeBeforeDetach );
	iScreenDevice->SetScreenMode(appModeBeforeDetach );
	Pause(200);
	interface->GetConfiguration(configBeforeDetach);
	
	TInt displayState = EDisconnect;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	
	//dummy mapping should return KErrNotReady if display is disconnected
	TRect dummyRect(1,2,5,8);
	error = mappingInterface->MapCoordinates(ECompositionSpace, dummyRect, EFullScreenSpace, dummyRect);
	ASSERT_EQUALS(error, KErrNotReady);
	
	displayState = ENormalResolution;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	
	interface->GetConfiguration(configAfterReattach);
	ASSERT_TRUE(configBeforeDetach==configAfterReattach);
	appModeAfterReattach = iScreenDevice->CurrentScreenMode();
	ASSERT_EQUALS(appModeBeforeDetach, appModeAfterReattach);
	//1st detach - attach sequence ends here
	//
	
	configBeforeDetach.ClearAll();
	configAfterReattach.ClearAll();
	
	//
	//2nd detach-attach sequence - non dynamic app mode
	iScreenDevice->SetAppScreenMode(secondAvailableAppMode);
	iScreenDevice->SetScreenMode(secondAvailableAppMode);
	Pause(200);
	appModeBeforeDetach = iScreenDevice->CurrentScreenMode();
	interface->GetConfiguration(configBeforeDetach);
	displayState = EDisconnect;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	//dummy mapping should return KErrNotReady if display is disconnected
	error = mappingInterface->MapCoordinates(ECompositionSpace, dummyRect, EFullScreenSpace, dummyRect);
	ASSERT_EQUALS(error, KErrNotReady);
	
	displayState = ENoResolution;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	//dummy mapping should return KErrNotReady if display is disconnected
	error = mappingInterface->MapCoordinates(ECompositionSpace, dummyRect, EFullScreenSpace, dummyRect);
	ASSERT_EQUALS(error, KErrNotReady);
	
	displayState = ENormalResolution;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	
	interface->GetConfiguration(configAfterReattach);
	ASSERT_TRUE(configBeforeDetach==configAfterReattach);
	appModeAfterReattach = iScreenDevice->CurrentScreenMode();
	ASSERT_EQUALS(appModeBeforeDetach, appModeAfterReattach);
	//2nd detach-attach sequence ends here
	//
	
	configBeforeDetach.ClearAll();
	configAfterReattach.ClearAll();
	
	//
	//3rd detach-attach sequence - dynamic app mode
	if (dynamicAppMode == -1)
		{
		screenModeList.Close();
		resolutionList1.Close();
		INFO_PRINTF1(_L("No dynamic app mode, end test"));	//not a fail if there isn't a dynamic app mode
		return;
		}
	
	iScreenDevice->SetAppScreenMode(dynamicAppMode);
	iScreenDevice->SetScreenMode(dynamicAppMode);
	Pause(200);
	appModeBeforeDetach = iScreenDevice->CurrentScreenMode();
	interface->GetConfiguration(configBeforeDetach);
	displayState = EDisconnect;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	//dummy mapping should return KErrNotReady if display is disconnected
	error = mappingInterface->MapCoordinates(ECompositionSpace, dummyRect, EFullScreenSpace, dummyRect);
	ASSERT_EQUALS(error, KErrNotReady);
	
	displayState = ENoResolution;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	//dummy mapping should return KErrNotReady if display is disconnected
	error = mappingInterface->MapCoordinates(ECompositionSpace, dummyRect, EFullScreenSpace, dummyRect);
	ASSERT_EQUALS(error, KErrNotReady);
	
	displayState = ENormalResolution;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	
	interface->GetConfiguration(configAfterReattach);
	ASSERT_FALSE(configBeforeDetach==configAfterReattach);
	TSize afterReattachSize;
	ASSERT_TRUE(configAfterReattach.GetResolution(afterReattachSize));
	ASSERT_EQUALS(afterReattachSize,TSize(0,0));
	ASSERT_TRUE(configAfterReattach.GetResolutionTwips(afterReattachSize));
	ASSERT_EQUALS(afterReattachSize,TSize(0,0));
	
	appModeAfterReattach = iScreenDevice->CurrentScreenMode();
	ASSERT_EQUALS(appModeBeforeDetach, appModeAfterReattach);
	//3rd detach-attach sequence ends here
	//
	
	screenModeList.Close();
	resolutionList1.Close();
#endif
	}


CDSATestDrawing::CDSATestDrawing():CTimer(EPriorityStandard)
	{}

CDSATestDrawing::~CDSATestDrawing()
	{
	Cancel();
	}
CDSATestDrawing* CDSATestDrawing::NewL()
	{
	CDSATestDrawing *self = new(ELeave) CDSATestDrawing();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CDSATestDrawing::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CDSATestDrawing::StartDrawingL(CDirectScreenAccess *aDSA)
	{
	
	if(aDSA)
		{
		iDSA = aDSA;
		}
	
	if(iDSA)
		{
		iDSA->StartL();
		
		iRect = TRect(TPoint(0,0), iDSA->DrawingRegion()->BoundingRect().Size());
		
		iWin->Invalidate();
		iWin->BeginRedraw();
		CFbsBitGc* gc = iDSA->Gc();
		gc->SetBrushColor(TRgb(220,220,220));
		gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc->Clear();
		iWin->EndRedraw();
		After(500000);
		}
	
	}

void CDSATestDrawing::RunL()
	{
	Draw();
	After(500000);
	}

void CDSATestDrawing::Draw()
	{
	//Should not invalidate the window containing DSA drawing. That's the whole point!
	CFbsBitGc* gc = iDSA->Gc();
	gc->SetPenStyle(gc->ESolidPen);
	gc->SetPenColor(TRgb(255,0,0));
	gc->SetBrushStyle(gc->ENullBrush);
	iRect.Shrink(1, 1);
	gc->DrawRect(iRect);
	iDSA->ScreenDevice()->Update();
	
	}
void CDSATestDrawing::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/) 
	{
	
	StartDrawingL(NULL);
	}

void CDSATestDrawing::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	Cancel();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0011L
@SYMTestCaseDesc		Test DSA drawing in different App Mode
@SYMREQ					REQ10332 REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Test DSA can restart and draw at correct place when screen mode changes
@SYMTestActions			
	A rectangle will be drawn 1 pixel inside the DSA drawing region, and continue to shrink before the drawing
	time runs out. when screen mode changes, DSA should restart and the rectangle should be reset to 1 pixel inside
	the new drawing region.
	
	note: DSA drawing region is clipped by DSA buffer, App Size and the visible area of the window which it drawns in
@SYMTestExpectedResults	
	All tests should pass.
	
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0011L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();

	Pause(1000);
	MakeTitleAndCompareWindowsL(_L("GFX_WSERV_DYNAMICRES_0011L"),_L("DSA drawing"));
	CDSATestDrawing *dsaDraw = CDSATestDrawing::NewL();
	CleanupStack::PushL(dsaDraw);
	CDirectScreenAccess *dsa = CDirectScreenAccess::NewL(iSession, *iScreenDevice, iCompare, *dsaDraw);
	CleanupStack::PushL(dsa);
	dsaDraw->SetWindow(iCompare);
	dsaDraw->StartDrawingL(dsa);
	
	CEventTimer *timer = CEventTimer::NewL();
	CleanupStack::PushL(timer);
	
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	MDisplayMapping* mappingInterface = static_cast<MDisplayMapping*>
				(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	ASSERT_TRUE(mappingInterface);
	
	CFbsScreenDevice *pixelReadBackDevice = CFbsScreenDevice::NewL(iScreenDevice->GetScreenNumber(), iScreenDevice->DisplayMode());
	CleanupStack::PushL(pixelReadBackDevice);
	
	for (TInt res=0;res<screenModeList.Count();res++)
		{
		iScreenDevice->SetAppScreenMode(screenModeList[res]);
		iScreenDevice->SetScreenMode(screenModeList[res]);
		dsa->ScreenDevice()->Update();
		Pause(300);
		//wait for DSA to restart
		timer->Wait(500000);
		TRect regionRect(dsa->DrawingRegion()->BoundingRect());

		TPoint winpos = iCompare.AbsPosition();
		TSize winsize = iCompare.Size();
		
		TPixelsAndRotation pr;
		iInfoScreenDevice->GetDefaultScreenSizeAndRotation(pr);
		TPoint 	origin=iInfoScreenDevice->GetDefaultScreenModeOrigin();
		Pause(300);
		if (iTitle.WsHandle())
			{
			TPoint	infoWinPos;
			TSize	infoWinSize;
			infoWinPos=iTitle.AbsPosition();
			infoWinSize=iTitle.Size();
			infoWinSize.iHeight=pr.iPixelSize.iHeight-2*infoWinPos.iY;
			iTitle.SetSize(infoWinSize);
			}
		TBuf16<20> s;
		s.AppendNum(res);
		UpdateTitleWindowL(s,1);
		s.Zero();
		s.AppendNum(pr.iPixelSize.iWidth);
		s.Append('x');
		s.AppendNum(pr.iPixelSize.iHeight);
		UpdateTitleWindowL(s,2);
		s.Zero();
		s.AppendNum(origin.iX);
		s.Append(',');
		s.AppendNum(origin.iY);
		UpdateTitleWindowL(s,3);
		
		INFO_PRINTF2(_L("---------test %i---------"), res);
		INFO_PRINTF3(_L("resolution %i x %i"), pr.iPixelSize.iWidth, pr.iPixelSize.iHeight);
		INFO_PRINTF3(_L("Origin (%i, %i)"), origin.iX, origin.iY);
		//INFO_PRINTF3(_L("Test Window Origin (%i, %i)"), iCompare.AbsPosition().iX, iCompare.AbsPosition().iY);
		INFO_PRINTF3(_L("DSA drawing region bounding rect origin (%i, %i)"), dsa->DrawingRegion()->BoundingRect().iTl.iX,
				dsa->DrawingRegion()->BoundingRect().iTl.iY);
		INFO_PRINTF3(_L("DSA drawing region bounding rect size (%i x %i)"), dsa->DrawingRegion()->BoundingRect().Width(),
						dsa->DrawingRegion()->BoundingRect().Height());
		//give time so DSA AO can start drawing
		timer->Wait(5000000);
		

		TRect readBackRect;
		mappingInterface->MapCoordinates(EApplicationSpace, TRect(0,0,1,1), EDirectScreenAccessSpace, readBackRect);
		pixelReadBackDevice->SetDrawDeviceOffset(readBackRect.iTl);
		pixelReadBackDevice->SetDeviceOrientation((TDeviceOrientation)(1 << pr.iRotation));
		readBackRect.SetRect(dsa->DrawingRegion()->BoundingRect().iTl, dsa->DrawingRegion()->BoundingRect().iBr);
		
		if(readBackRect.IsEmpty())
			{
			INFO_PRINTF1(_L("DSA drawing region is empty, skip checking pixel colour"));
			continue;
			}
		
		//Ruo: Oh I almost believe iBr is exclusive now
		TDisplayConfiguration dispConfigAfter2;
		interface->GetConfiguration(dispConfigAfter2);
		TSize theSize;
		dispConfigAfter2.GetResolution(theSize);
		TDisplayConfiguration1::TRotation orient;
		dispConfigAfter2.GetRotation(orient);
		if (orient == TDisplayConfiguration1::ERotation90CW || orient == TDisplayConfiguration1::ERotation270CW)
			{
			TInt temp = theSize.iHeight;
			theSize.iHeight = theSize.iWidth;
			theSize.iWidth = temp;
			}
		TRect uiSize(TPoint(0,0), theSize);
		MDisplayMapping* mappingInterface = static_cast<MDisplayMapping*>
					(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
		ASSERT_TRUE(mappingInterface);
		TRect finalSize;
		mappingInterface->MapCoordinates(EApplicationSpace, uiSize, ECompositionSpace, finalSize);
				
		CFbsBitmap* bmp2 = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(bmp2);
		User::LeaveIfError(bmp2->Create(finalSize.Size(), EColor16MU));
		MTestScreenCapture
				* csc =
						static_cast<MTestScreenCapture*> (iScreenDevice->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
	
		if (csc)
			{
			TInt errr = csc->ComposeScreen(*bmp2);
			ASSERT_TRUE(errr == KErrNone);
			}
		else
			{
			if (!cSCLogged)
				{
				cSCLogged = ETrue;
				_LIT(KNoCSC, "CSC testing not enabled as CSC render stage not defined");
				Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrAll, KNoCSC);
				}
			}

		readBackRect.iBr = readBackRect.iBr - TPoint(1,1);
		//check 4 outter corners
		TRgb pixelRgb;
		TRgb cornerRgb(220, 220, 220);
		TRgb innerRgb(255, 0, 0);
		readBackRect.Grow(1, 1);
		TRect compSpaceReadBackRect;
		mappingInterface->MapCoordinates(EApplicationSpace, readBackRect, ECompositionSpace, compSpaceReadBackRect);

			
		if (csc)
			{
			if (finalSize.Contains(compSpaceReadBackRect.iTl))
				{
				bmp2->GetPixel(pixelRgb, compSpaceReadBackRect.iTl);
				if(!(pixelRgb != cornerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(TPoint(compSpaceReadBackRect.iBr.iX, compSpaceReadBackRect.iTl.iY)))
				{
				bmp2->GetPixel(pixelRgb, TPoint(compSpaceReadBackRect.iBr.iX, compSpaceReadBackRect.iTl.iY));
				if(!(pixelRgb != cornerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(compSpaceReadBackRect.iBr))
				{
				bmp2->GetPixel(pixelRgb, compSpaceReadBackRect.iBr);
				if(!(pixelRgb != cornerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(TPoint(compSpaceReadBackRect.iTl.iX, compSpaceReadBackRect.iBr.iY)))
				{
				bmp2->GetPixel(pixelRgb, TPoint(compSpaceReadBackRect.iTl.iX, compSpaceReadBackRect.iBr.iY));
				if(!(pixelRgb != cornerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			}
		
		pixelReadBackDevice->GetPixel(pixelRgb, readBackRect.iTl);
		ASSERT_TRUE(pixelRgb != cornerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, TPoint(readBackRect.iBr.iX, readBackRect.iTl.iY));
		ASSERT_TRUE(pixelRgb != cornerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, readBackRect.iBr);
		ASSERT_TRUE(pixelRgb != cornerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, TPoint(readBackRect.iTl.iX, readBackRect.iBr.iY));
		ASSERT_TRUE(pixelRgb != cornerRgb);
		
		//check 4 inner corners
		readBackRect.Shrink(1,1);
		mappingInterface->MapCoordinates(EApplicationSpace, readBackRect, ECompositionSpace, compSpaceReadBackRect);
	
		if (csc)
			{
			if (finalSize.Contains(compSpaceReadBackRect.iTl))
				{
				bmp2->GetPixel(pixelRgb, compSpaceReadBackRect.iTl);
				if(!(pixelRgb == cornerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(TPoint(compSpaceReadBackRect.iBr.iX, compSpaceReadBackRect.iTl.iY)))
				{
				bmp2->GetPixel(pixelRgb, TPoint(compSpaceReadBackRect.iBr.iX, compSpaceReadBackRect.iTl.iY));
				if(!(pixelRgb == cornerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(compSpaceReadBackRect.iBr))
				{
				bmp2->GetPixel(pixelRgb, compSpaceReadBackRect.iBr);
				if(!(pixelRgb == cornerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(TPoint(compSpaceReadBackRect.iTl.iX, compSpaceReadBackRect.iBr.iY)))
				{
				bmp2->GetPixel(pixelRgb, TPoint(compSpaceReadBackRect.iTl.iX, compSpaceReadBackRect.iBr.iY));
				if(!(pixelRgb == cornerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			}
		
		pixelReadBackDevice->GetPixel(pixelRgb, readBackRect.iTl);
		ASSERT_TRUE(pixelRgb == cornerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, TPoint(readBackRect.iBr.iX, readBackRect.iTl.iY));
		ASSERT_TRUE(pixelRgb == cornerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, readBackRect.iBr);
		ASSERT_TRUE(pixelRgb == cornerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, TPoint(readBackRect.iTl.iX, readBackRect.iBr.iY));
		ASSERT_TRUE(pixelRgb == cornerRgb);
		//check inner colour
		if(readBackRect.Width() < 3 || readBackRect.Height() < 3)
			{
			INFO_PRINTF1(_L("DSA drawing region is too small for drawing inner rectangle skip checking inner colour"));
			CleanupStack::PopAndDestroy(bmp2);
			continue;
			}
		
		readBackRect.Shrink(1,1);
		mappingInterface->MapCoordinates(EApplicationSpace, readBackRect, ECompositionSpace, compSpaceReadBackRect);
		
		if (csc)
			{
			if (finalSize.Contains(compSpaceReadBackRect.iTl))
				{
				bmp2->GetPixel(pixelRgb, compSpaceReadBackRect.iTl);
				if(!(pixelRgb == innerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(TPoint(compSpaceReadBackRect.iBr.iX, compSpaceReadBackRect.iTl.iY)))
				{
				bmp2->GetPixel(pixelRgb, TPoint(compSpaceReadBackRect.iBr.iX, compSpaceReadBackRect.iTl.iY));
				if(!(pixelRgb == innerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(compSpaceReadBackRect.iBr))
				{
				bmp2->GetPixel(pixelRgb, compSpaceReadBackRect.iBr);
				if(!(pixelRgb == innerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			if (finalSize.Contains(TPoint(compSpaceReadBackRect.iTl.iX, compSpaceReadBackRect.iBr.iY)))
				{
				bmp2->GetPixel(pixelRgb, TPoint(compSpaceReadBackRect.iTl.iX, compSpaceReadBackRect.iBr.iY));
				if(!(pixelRgb == innerRgb))
					{
					_LIT(KCompareFailed, "Corner check failed on app mode: %d");
					TBuf<256> x;
					x.Format(KCompareFailed, res+1);
					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
					CleanupStack::PopAndDestroy(bmp2);
					continue;
					}
				}
			}
		
		pixelReadBackDevice->GetPixel(pixelRgb, readBackRect.iTl);
		ASSERT_TRUE(pixelRgb == innerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, TPoint(readBackRect.iBr.iX, readBackRect.iTl.iY));
		ASSERT_TRUE(pixelRgb == innerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, readBackRect.iBr);
		ASSERT_TRUE(pixelRgb == innerRgb);
		pixelReadBackDevice->GetPixel(pixelRgb, TPoint(readBackRect.iTl.iX, readBackRect.iBr.iY));
		ASSERT_TRUE(pixelRgb == innerRgb);

		CleanupStack::PopAndDestroy(bmp2);
		}
	screenModeList.Close();
	CleanupStack::PopAndDestroy(4, dsaDraw);
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0012L
@SYMTestCaseDesc		Test Pointer event coordinates are correct in different screen modes.
@SYMREQ					REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Test Pointer event coordinates are correct in different screen mode
@SYMTestActions			
	In each screen mode, the simulated pointer event should always at the same relative position
	inside iCompare window
@SYMTestExpectedResults	
	All test should pass
	**NOTE  Can only be tested in screen 0 **
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0012L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
	MakeTitleAndCompareWindowsL(_L("GFX_WSERV_DYNAMICRES_0012L"),_L("Pointer coordinates"));		
	Pause(300);

	CWsDisplayEvent* displayEventAO = new(ELeave) CWsDisplayEvent(&iSession);
	CleanupStack::PushL(displayEventAO);
	displayEventAO->Request();
	CEventTimer *timer = CEventTimer::NewL();
	CleanupStack::PushL(timer);
	
	MDisplayMapping* mappingInterface = static_cast<MDisplayMapping*>
				(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	ASSERT_TRUE(mappingInterface);
	
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	iSession.SetFocusScreen(TGlobalSettings::Instance().iScreen);
	
	for (TInt res=0;res<screenModeList.Count();res++)
		{
		iScreenDevice->SetAppScreenMode(screenModeList[res]);
		iScreenDevice->SetScreenMode(screenModeList[res]);
		Pause(300);
		
		//win position in app space
		TPoint winPos = iCompare.AbsPosition();
		TSize winSize = iCompare.Size();
		
		//pointer event at the origin of window
		TRect rectNearWinOrigin(winPos+TPoint(1,1), TSize(1,1));
		//the expected pointer position received by client to compare with the actual point received.
		//it's (1,1) because it's relative to window
		displayEventAO->SetExpectedPointPos(TPoint(1, 1)); 
		TRect rectInComp, rectBackInApp;
		mappingInterface->MapCoordinates(EApplicationSpace, rectNearWinOrigin, ECompositionSpace, rectInComp);
		//as we got the physical coordinates at where we simulate pointer event
		TRawEvent rawEvent;
		rawEvent.Set(TRawEvent::EButton1Down, rectInComp.iTl.iX, rectInComp.iTl.iY);
		iSession.SimulateRawEvent(rawEvent);
		rawEvent.Set(TRawEvent::EButton1Up, rectInComp.iTl.iX, rectInComp.iTl.iY);
		iSession.SimulateRawEvent(rawEvent);
		iSession.Flush();
		timer->Wait(1000);
		
		ASSERT_EQUALS(displayEventAO->ReceivedPointerEventCount(), res*4+2);
		ASSERT_TRUE(displayEventAO->PointerTestPassed());
		
		//pointer event at 1/2 width and height inside the window
		TRect rectAtWinCenter(winPos+TPoint(winSize.iWidth/2, winSize.iHeight/2), TSize(1,1));
		displayEventAO->SetExpectedPointPos(rectAtWinCenter.iTl - winPos);
		mappingInterface->MapCoordinates(EApplicationSpace, rectAtWinCenter, ECompositionSpace, rectInComp);
		//as we got the physical coordinates at where we simulate pointer event
		rawEvent.Set(TRawEvent::EButton1Down, rectInComp.iTl.iX, rectInComp.iTl.iY);
		iSession.SimulateRawEvent(rawEvent);
		rawEvent.Set(TRawEvent::EButton1Up, rectInComp.iTl.iX, rectInComp.iTl.iY);
		iSession.SimulateRawEvent(rawEvent);
		iSession.Flush();
		timer->Wait(1000);
		
		ASSERT_EQUALS(displayEventAO->ReceivedPointerEventCount(), res*4 + 4);
		ASSERT_TRUE(displayEventAO->PointerTestPassed());
		}
	screenModeList.Close();
	CleanupStack::PopAndDestroy(2, displayEventAO);
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0013L
@SYMTestCaseDesc		Test the twips size after detach
@SYMREQ					REQ10329
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Make sure the twips size is undefined after detach.
@SYMTestActions			
	Save display configuration before detach and compare it with the display configuration after detach.
@SYMTestExpectedResults	
	all tests should pass
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0013L()
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
#ifdef MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
	ASSERT_FALSE("Test not compiled because MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER");
#else
	TInt error;
	iSession.Finish(ETrue);
	MakeTitleAndCompareWindowsL(_L("GFX_WSERV_DYNAMICRES_0013L"),_L("detach/attach - twips size"));
	iSession.Finish(ETrue);

	Pause(1000);
	TInt screenNo = iScreenDevice->GetScreenNumber();
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	MDisplayMapping* mappingInterface = static_cast<MDisplayMapping*>
				(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	ASSERT_TRUE(mappingInterface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TDisplayConfiguration configBeforeDetach;
	
	// detach the display
	interface->GetConfiguration(configBeforeDetach);
	TInt displayState = EDisconnect;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(300);
	
	// retrieve the display configuration
	interface->GetConfiguration(configBeforeDetach);
	ASSERT_FALSE(configBeforeDetach.IsDefined(TDisplayConfiguration::EResolutionTwips));
	
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrDisconnected);
	
	configBeforeDetach.ClearAll();
	
	screenModeList.Close();
	resolutionList1.Close();
	
	displayState = ENormalResolution;
	UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
#endif
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0014L
@SYMTestCaseDesc		Test GetConfiguration immediatly after SetConfiguration
@SYMREQ					REQ10328
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Make sure the twips size is buffered so immediate GetConfiguration can get correct twip size
@SYMTestActions			
	successive SetConfiguration/GetConfiguration pair 
@SYMTestExpectedResults	
	all tests should pass
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0014L()
	{
	MDisplayControl* interface = static_cast<MDisplayControl*>
					(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	RArray<MDisplayControl::TResolution> resolutions;
	TInt err = interface->GetResolutions(resolutions);
	ASSERT_EQUALS(err, KErrNone);
	TDisplayConfiguration configSet, configGet;
	TSize twipsGet, resolutionGet;
	
	TInt screenNo = iScreenDevice->GetScreenNumber();
	iSession.SetFocusScreen(TGlobalSettings::Instance().iScreen);
	for(TInt testIndex = 0; testIndex < 2; testIndex++)
		{//run twice
#ifdef MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
	INFO_PRINTF1(_L("Test not compiled because MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER"));
#else
		TInt displayState;
		displayState = ENormalResolution;
		UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
#endif
		for (TInt i=0;i<resolutions.Count();i++)
			{
			INFO_PRINTF2(_L("i = %i"), i);
			INFO_PRINTF3(_L("resolutions: %i x %i"), resolutions[i].iPixelSize.iWidth, resolutions[i].iPixelSize.iHeight);
			INFO_PRINTF3(_L("twips: %i x %i"), resolutions[i].iTwipsSize.iWidth, resolutions[i].iTwipsSize.iHeight);
			configSet.ClearAll();
			configGet.ClearAll();
			configSet.SetResolution(resolutions[i].iPixelSize);
			err = interface->SetConfiguration(configSet);
			if(err != KErrNone)
				{
				//with scaling, KErrArgument means current config is not compatible with appmode. 
				//In none scaling, it does mean an error,
				//but the purpose of this test is to see if twips size is buffered so we can read it back instantly
				//let other tests test the functionality of SetConfiguration.
				ASSERT_EQUALS(err, KErrArgument); 
												
				INFO_PRINTF1(_L("this configuration is not compatible with current appmode, skip"));
				continue;
				}
			interface->GetConfiguration(configGet);
			ASSERT_TRUE(configGet.IsDefined(TDisplayConfigurationBase::EResolutionTwips));
			ASSERT_TRUE(configGet.IsDefined(TDisplayConfigurationBase::EResolution));
			configGet.GetResolutionTwips(twipsGet);
			configGet.GetResolution(resolutionGet);
			INFO_PRINTF3(_L("resolution readback: %i x %i"), resolutionGet.iWidth, resolutionGet.iHeight);
			INFO_PRINTF3(_L("twips readback: %i x %i"), twipsGet.iWidth, twipsGet.iHeight);
			ASSERT_EQUALS(twipsGet, resolutions[i].iTwipsSize);
			Pause(300);
			}
#ifdef MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
	INFO_PRINTF1(_L("Test not compiled because MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER"));
#else
		displayState = EDisconnect;
		UserSvr::HalFunction(EHalGroupDisplay | (screenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
#endif		
		}
	resolutions.Close();
	
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0015L
@SYMTestCaseDesc		No Scaling doesnt return virtual resolutions
@SYMREQ					REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			no scaling should mean basic resolution selection
@SYMTestActions			
	Get resolution list
	Should not contain virtual resolutions
	Set every app mode
	Should not change resolution
@SYMTestExpectedResults	
	All test should pass
	**SENSITIVE TO CHANGES IN EPOC.INI and WSINI.INI**
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0015L()
	{	//ONLY RUN WITH 'NO SCALING' WSINI.INI, can be found in resources
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	TInt screenNumber = iScreenDevice->GetScreenNumber();
	if (screenNumber == 0)
		{
		ASSERT_EQUALS(resolutions,1);
		}
	else if (screenNumber == 1)
		{
		ASSERT_EQUALS(resolutions,4);
		}	
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	TInt error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	
	TDisplayConfiguration dispConfig1;
	interface->GetConfiguration(dispConfig1);
	TSize size1;
	ASSERT_TRUE(dispConfig1.GetResolution(size1));
	
	for (TInt ii = 0; ii < screenModeList.Count(); ii++)
		{
		iInfoScreenDevice->SetAppScreenMode(screenModeList[ii]);
		iInfoScreenDevice->SetScreenMode(screenModeList[ii]);
		iSession.Finish(ETrue);
		Pause(300);
		
		TDisplayConfiguration newConfig;
		interface->GetConfiguration(newConfig);
		TSize newSize;
		ASSERT_TRUE(dispConfig1.GetResolution(newSize));
		ASSERT_EQUALS(size1,newSize);
		}
	iInfoScreenDevice->SetAppScreenMode(screenModeList[0]);
	iInfoScreenDevice->SetScreenMode(screenModeList[0]);
	iSession.Finish(ETrue);
	Pause(300);
	
	screenModeList.Close();
	resolutionList1.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0016L
@SYMTestCaseDesc		Anisotropic scaling
@SYMREQ					REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Check using twips will cause virtual resolutions to possibly be different
@SYMTestActions			
	Check the virtual resolution for XXX is correctly XXX
	Check when setting the appmode, it choses the correct resolution
@SYMTestExpectedResults	
	Tests should pass
	**SENSITIVE TO CHANGES IN EPOC.INI and WSINI.INI**
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0016L()
	{	//ONLY RUN WITH ANISOTROPIC WSINI.INI

	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0017L
@SYMTestCaseDesc		Integer scales correctly in policy
@SYMREQ					REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Integer scaling should not create resolutions of non integer value!
@SYMTestActions			
	For every virtual resolution
		Check its an integer size of a real resolution
@SYMTestExpectedResults	
	All should be integers (1:1 , 1:2 , 1:3 or 1:4 in current policy)
	**NOTE ONLY RUN WITH INTEGER WSINI.INI**
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0017L()
	{	//ONLY RUN WITH INTEGER WSINI.INI
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE(resolutions>1);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	TInt error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TInt index;
	for (index=0;index<resolutions;index++)
		{
		if (resolutionList1[index].iFlags.IsSet(MDisplayControlBase::TResolution::EIsVirtual))
			{
			TBool foundMatch = EFalse;
			for (TInt index2=0;index2<resolutions;index2++)
				{
				if (resolutionList1[index2].iFlags.IsClear(MDisplayControlBase::TResolution::EIsVirtual))
					{
					for (TInt scale=1;scale<=4;scale++)
						{
						TSize compare;
						compare.iWidth=resolutionList1[index].iPixelSize.iWidth/scale;
						compare.iHeight=resolutionList1[index].iPixelSize.iHeight/scale;
						if (resolutionList1[index2].iPixelSize == compare)
							{
							foundMatch = ETrue;
							break;
							}
						}
					}
				if (foundMatch)
					{
					break;
					}
				}
			ASSERT_TRUE(foundMatch);
			}
		}
	resolutionList1.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0018L
@SYMTestCaseDesc		MDisplayMapping interface function test
@SYMREQ					REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			All the MDisplayMapping interface functions are consistent
@SYMTestActions			
	Check different mapping functions are consistent
@SYMTestExpectedResults	
	All test should pass
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0018L()
	{
	
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
	TInt error;
	iSession.Finish(ETrue);
	MakeTitleAndCompareWindowsL(_L("GFX_WSERV_DYNAMICRES_0020L"),_L("Mapping function consistency"));
	iSession.Finish(ETrue);
	
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	MDisplayMapping* mapInterface = static_cast<MDisplayMapping*>
			(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	ASSERT_TRUE(mapInterface);
	
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	
	TRect maxWinExtent, refMaxWinExtent;
	TSize surfaceSize, surfaceTwips, refSurfaceSize, refSurfaceTwips;
	TRect displayWinExtent, refDisplayWinExtent;
	for (TInt i = 0; i < screenModeList.Count(); i++)
		{
		iInfoScreenDevice->SetAppScreenMode(screenModeList[i]);
		iInfoScreenDevice->SetScreenMode(screenModeList[i]);
		iSession.Finish(ETrue);
		Pause(300);
		
		INFO_PRINTF1(_L("---------------"));
		INFO_PRINTF2(_L("screen mode : %i"), screenModeList[i]);
		
		mapInterface->GetMaximumWindowExtent(maxWinExtent);
		INFO_PRINTF3(_L("MaximumWindowExtent Origin: (%i, %i)"), maxWinExtent.iTl.iX, maxWinExtent.iTl.iY);
		INFO_PRINTF3(_L("MaximumWindowExtent Size: %i x %i"), maxWinExtent.Width(), maxWinExtent.Height());
		
		
		TDisplayConfiguration config;
		interface->GetConfiguration(config);
		ASSERT_TRUE(config.IsDefined(TDisplayConfigurationBase::EResolution));
		TSize fullUiSize;
		config.GetResolution(fullUiSize);
		error = mapInterface->MapCoordinates(EFullScreenSpace, fullUiSize, EApplicationSpace, refMaxWinExtent);
		ASSERT_EQUALS(error, KErrNone);
		INFO_PRINTF3(_L("ref MaximumWindowExtent Origin: (%i, %i)"), refMaxWinExtent.iTl.iX, refMaxWinExtent.iTl.iY);
		INFO_PRINTF3(_L("ref MaximumWindowExtent Size: %i x %i"), refMaxWinExtent.Width(), refMaxWinExtent.Height());
		ASSERT_EQUALS(maxWinExtent, refMaxWinExtent);
		INFO_PRINTF1(_L("Match"));
		
		mapInterface->GetMaximumSurfaceSize(surfaceSize, surfaceTwips);
		INFO_PRINTF3(_L("MaxSurfaceSize: %i x %i"), surfaceSize.iWidth, surfaceSize.iHeight);
		INFO_PRINTF3(_L("MaxSurfaceTwips: %i x %i"), surfaceTwips.iWidth, surfaceSize.iHeight);
		TRect compositionRect;
		error = mapInterface->MapCoordinates(EFullScreenSpace, fullUiSize, ECompositionSpace, compositionRect);
		ASSERT_EQUALS(error, KErrNone);
		refSurfaceSize = compositionRect.Size();
		INFO_PRINTF3(_L("RefSurfaceSize: %i x %i"), refSurfaceSize.iWidth, refSurfaceSize.iHeight);
		ASSERT_TRUE(config.IsDefined(TDisplayConfigurationBase::EResolutionTwips));
		config.GetResolutionTwips(refSurfaceTwips);
		INFO_PRINTF3(_L("RefSurfaceTwips: %i x %i"), refSurfaceTwips.iWidth, refSurfaceTwips.iHeight);
		ASSERT_EQUALS(surfaceSize, refSurfaceSize);
		ASSERT_EQUALS(surfaceTwips, refSurfaceTwips);
		INFO_PRINTF1(_L("Match"));
		
		mapInterface->GetDisplayExtentOfWindow(iCompare, displayWinExtent);
		INFO_PRINTF3(_L("DisplayWin origin: (%i, %i)"), displayWinExtent.iTl.iX, displayWinExtent.iTl.iY);
		INFO_PRINTF3(_L("DisplayWin Size: (%i x %i"), displayWinExtent.Width(), displayWinExtent.Height());
		error = mapInterface->MapCoordinates(EApplicationSpace, TRect(TPoint(iCompare.AbsPosition()),iCompare.Size()), ECompositionSpace, refDisplayWinExtent);
		ASSERT_EQUALS(error, KErrNone);
		INFO_PRINTF3(_L("RefDisplayWin origin: (%i, %i)"), refDisplayWinExtent.iTl.iX, refDisplayWinExtent.iTl.iY);
		INFO_PRINTF3(_L("RefDisplayWin Size: (%i x %i"), refDisplayWinExtent.Width(), refDisplayWinExtent.Height());
		ASSERT_EQUALS(displayWinExtent, refDisplayWinExtent);
		INFO_PRINTF1(_L("Match"));
		}
	iInfoScreenDevice->SetAppScreenMode(screenModeList[0]);
	iInfoScreenDevice->SetScreenMode(screenModeList[0]);
	screenModeList.Close();
	}

/*
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0021L
@SYMTestCaseDesc		SetResolution negative test
@SYMREQ					REQ10326
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Invalid configurations should return error and change nothing!
@SYMTestActions			
	Set config with rubbish resolution
	Set config with rubbish twips
	Set config with valid resolution, rubbish twips
	Set config with valid twips, rubbish resolution
	Set config with nothing defined
@SYMTestExpectedResults	
	Should all fail safely and not change any settings.
	Config with nothing defined in a scaled mode will reset the resolution to current modes
	equivilent virtual resolution
	**NOTE ONLY RUN IN SCREEN 1 - we need multiple resolutions available**
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0021L(TBool aScaleMode)
	{	//aScaleMode 0 = no scaling, 1 = integer,isotropic or anisotropic
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 1);
		
	RArray<MDisplayControl::TResolution> resolutionList1;
	TInt error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TDisplayConfiguration original;
	original.SetResolution(resolutionList1[0].iPixelSize);
	error = interface->SetConfiguration(original);
	ASSERT_EQUALS(error,KErrNone);
	
	TDisplayConfiguration beforeConfig;
	TDisplayConfiguration afterConfig;
	interface->GetConfiguration(beforeConfig);
	
	TInt differentRes;
	for (differentRes = 1; differentRes < resolutions; differentRes++)
		{
		if (!(resolutionList1[differentRes].iPixelSize == resolutionList1[0].iPixelSize))
			{
			break;
			}
		}
	ASSERT_TRUE(differentRes < resolutions);	//otherwise it didnt find a resolution different to the one set
	
	TDisplayConfiguration test1;
	test1.SetResolution(TSize(5,6));	//rubbish resolution!
	error = interface->SetConfiguration(test1);

	ASSERT_EQUALS(error,KErrArgument);

	ASSERT_EQUALS(error,KErrArgument);
	iSession.Flush();
	interface->GetConfiguration(afterConfig);
	ASSERT_TRUE(beforeConfig == afterConfig);
	
	TDisplayConfiguration test2;
	test2.SetResolutionTwips(TSize(7,8));	//rubbish twips!
	error = interface->SetConfiguration(test2);
	ASSERT_EQUALS(error,KErrArgument);
	iSession.Flush();
	interface->GetConfiguration(afterConfig);
	ASSERT_TRUE(beforeConfig == afterConfig);
	
	TDisplayConfiguration test3;
	test3.SetResolution(resolutionList1[differentRes].iPixelSize);	//ok resolution!
	test3.SetResolutionTwips(TSize(9,10));	//rubbish twips!
	error = interface->SetConfiguration(test3);
	//ASSERT_EQUALS(error,KErrArgument);
	if (error != KErrArgument)
		{
		ASSERT_EQUALS (aScaleMode,0);	//if no policy, we currently have issue with confing not being validated
		INFO_PRINTF1(_L("config was not honoured!"));
		error = interface->SetConfiguration(original);
		ASSERT_EQUALS(error,KErrNone);
		iSession.Flush();
		Pause(200);
		}
	else
		{
		iSession.Flush();
		Pause(200);
		interface->GetConfiguration(afterConfig);
		ASSERT_TRUE(beforeConfig == afterConfig);
		}

	TDisplayConfiguration test4;
	test4.SetResolution(TSize(11,12));	//rubbish resolution!
	test4.SetResolutionTwips(resolutionList1[differentRes].iTwipsSize);	//ok twips!
	error = interface->SetConfiguration(test4);
	if (error != KErrArgument)
		{
		ASSERT_EQUALS (aScaleMode,0);	//if no policy, we currently have issue with config not being validated
		INFO_PRINTF1(_L("config was not honoured!"));
		error = interface->SetConfiguration(original);
		ASSERT_EQUALS(error,KErrNone);
		iSession.Flush();
		Pause(200);
		}
	else
		{
		iSession.Flush();
		Pause(200);
		interface->GetConfiguration(afterConfig);
		ASSERT_TRUE(beforeConfig == afterConfig);
		}
	
	if (aScaleMode)
		{
		RArray<TInt> screenModeList;
		iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
		ASSERT_TRUE(screenModeList.Count()>1);
		//set default screen mode, sets resolution to best fit for this mode
		iInfoScreenDevice->SetAppScreenMode(screenModeList[0]);
		iInfoScreenDevice->SetScreenMode(screenModeList[0]);	//also sets its best resolution
		iSession.Finish(ETrue);
		Pause(200);
		
		TDisplayConfiguration newModeConfig;
		interface->GetConfiguration(newModeConfig);
		//get best fit resolution
		TSize newModeRes;
		ASSERT_TRUE(newModeConfig.GetResolution(newModeRes));
		
		//find a different resolution
		TInt i;
		for (i = 0; i < resolutions; i++)
			{
			if (!(resolutionList1[i].iPixelSize == newModeRes))
				{
				break;
				}
			}
		ASSERT_TRUE(i < resolutions);	//otherwise it didnt find a resolution different to the one set
		TDisplayConfiguration newSetConfig;
		
		//set the different resolution
		newSetConfig.SetResolution(resolutionList1[i].iPixelSize);
		error = interface->SetConfiguration(newSetConfig);
		ASSERT_TRUE(error == KErrNone);
		iSession.Flush();
		Pause(200);
		TDisplayConfiguration checkConfig;
		
		//check its set this new resolution
		interface->GetConfiguration(checkConfig);
		TSize checkSize;
		checkConfig.GetResolution(checkSize);
		ASSERT_TRUE (checkSize == resolutionList1[i].iPixelSize);
		
		TDisplayConfiguration emptyConfig;
		TDisplayConfiguration newModeConfig2(newModeConfig);
		newModeConfig2.Clear(newModeConfig2.EResolutionTwips);
		//set empty config, which should reset resolution to current modes default
		error = interface->SetConfiguration(emptyConfig);//emptyConfig);
		ASSERT_TRUE(error == KErrNone);
		iSession.Finish();
		Pause(200);
	
		interface->GetConfiguration(checkConfig);
		ASSERT_TRUE (newModeConfig == checkConfig);	//empty config reset res to modes default res

		screenModeList.Close();
		}
	else
		{
		TDisplayConfiguration test5;
		error = interface->SetConfiguration(test5);
		ASSERT_EQUALS(error,KErrArgument);
		}
	resolutionList1.Close();	
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0031L
@SYMTestCaseDesc		SetConfiguration in OOM (changing resolution)
@SYMREQ					REQ10326
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Safety check against OOM
@SYMTestActions			
	Set OOM failure
	Set a resolution that will definately cause a change of resolution
	Increase length of time to failure, repeat
	Finish after 5 successful changes of resolution
@SYMTestExpectedResults	
	Set resolution should either return a fail and not have cause a change in resolution,
	or it should have returned no fail and have set the new resolution
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0031L()
	{
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	INFO_PRINTF1(_L("Interface obtained sucessfully."));
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	INFO_PRINTF1(_L("NumberOfResolutions obtained sucessfully."));
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	TInt error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	INFO_PRINTF1(_L("resolutionList1 has correct number of resolutions."));
	
	TInt index;
	TDisplayConfiguration dispConfigBefore;
	TDisplayConfiguration dispConfigAfter;
	
	dispConfigBefore.SetResolution(resolutionList1[0].iPixelSize);
	interface->SetConfiguration(dispConfigBefore);
	interface->GetConfiguration(dispConfigBefore);
	ASSERT_TRUE(dispConfigBefore.IsDefined(dispConfigBefore.EResolution));
	INFO_PRINTF1(_L("dispConfigBefore is defined."));
	TSize dispResBefore;
	TSize dispResAfter;
	dispConfigBefore.GetResolution(dispResBefore);
	TDisplayConfiguration::TRotation dispRotBefore;
	TDisplayConfiguration::TRotation dispRotReq;
	dispConfigBefore.GetRotation(dispRotBefore);

	for (index = 0; index < resolutionList1.Count(); index++)
		{
		INFO_PRINTF2(_L("Starting resolution %d."), index);
		if (resolutionList1[index].iPixelSize.iWidth == 0)
			{
			continue;
			}
		TInt heapFail = 1;
		TBool notComplete = ETrue;
		TInt completeCount = 0;
		while (notComplete)
			{
			TDisplayConfiguration dispConfigReq;
			dispConfigReq.SetResolution(resolutionList1[index].iPixelSize);
			ASSERT_TRUE(dispConfigReq.IsDefined(dispConfigReq.EResolution));
			SetRotation(dispConfigReq,resolutionList1[index]);	//set rotation to first defined in res list
			dispConfigReq.GetRotation(dispRotReq);
			INFO_PRINTF2(_L("Before heap failure, value of heapfail is %d."), heapFail);
			
			iSession.Finish(ETrue);
			iSession.HeapSetFail(RHeap::EDeterministic,heapFail);
			TInt errCode=interface->SetConfiguration(dispConfigReq);
			iSession.HeapSetFail(RHeap::ENone,0);
			INFO_PRINTF1(_L("After Heap Failure."));
			iSession.Finish(ETrue);
			Pause (100);
			INFO_PRINTF1(_L("After Session Finish."));
			
			interface->GetConfiguration(dispConfigAfter);
			ASSERT_TRUE(dispConfigAfter.IsDefined(dispConfigAfter.EResolution));
			dispConfigAfter.GetResolution(dispResAfter);
			
			TBool worked = ETrue;
			if (errCode < KErrNone)
				{			
				worked = EFalse;
				ASSERT_EQUALS(dispResAfter,dispResBefore);
				if (dispRotReq != dispRotBefore)
					{	//didnt expect it work-KErrGeneral can mean a memory allocation fail
					if (errCode == KErrArgument)
						{	//if it didnt panic, that is a good enough pass in this instance
						worked = ETrue;
						}
					}
				}
			else
				{				
				ASSERT_EQUALS(dispResAfter,resolutionList1[index].iPixelSize);
				}
			if (worked)
				{	
				completeCount++;
				if (completeCount == 5)
					{
					notComplete = EFalse;
					}
				}
			else
				{
				completeCount = 0;
				}
			interface->SetConfiguration(dispConfigBefore);
			INFO_PRINTF1(_L("After Setting Original Configuration."));
			heapFail++;
			if (heapFail == 80)
				{
				ASSERT_TRUE(0);	//worrying amount of fails
				}
			}
		}
	resolutionList1.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0032L
@SYMTestCaseDesc		SetAppMode in OOM
@SYMREQ					REQ10326 REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Safety check against OOM
@SYMTestActions			
	Set OOM failure
	Set new app mode
	Increase length of time to failure, repeat
	Finish after 5 successful changes of mode
@SYMTestExpectedResults	
	SetAppMode should either return a fail and not have cause a change in resolution,
	or it should have returned no fail and have set the new resolution
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0032L()
	{
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	ASSERT_TRUE (screenModeList.Count()>0);
	
	iInfoScreenDevice->SetAppScreenMode(screenModeList[0]);
	iInfoScreenDevice->SetScreenMode(screenModeList[0]);
	
	for (TInt index = 1; index < screenModeList.Count(); index++)
		{	//skip index 0, as that wouldn't be changing mode
		TInt heapFail = 1;
		TBool notComplete = ETrue;
		TInt completeCount = 0;
		while (notComplete)
			{
            RDebug::Printf("iInfoScreenDevice->SetAppScreenMode");
			iInfoScreenDevice->SetAppScreenMode(screenModeList[index]);
			iSession.Finish(ETrue);
			iSession.HeapSetFail(RHeap::EDeterministic,heapFail);
            RDebug::Printf("iInfoScreenDevice->SetAppScreenMode");
			iInfoScreenDevice->SetScreenMode(screenModeList[index]);
            RDebug::Printf("SetScreenMode done");
			iSession.HeapSetFail(RHeap::ENone,0);
			iSession.Finish(ETrue);
            RDebug::Printf("Finish done");
			Pause(50);
			TInt newMode = iInfoScreenDevice->CurrentScreenMode();

			if (screenModeList[index] == newMode)
				{
				completeCount++;
				if (completeCount == 5)
					{
					INFO_PRINTF3(_L("Mode %i Succeeded with heapFail = %d"),index, heapFail);
					notComplete = EFalse;
					}
				iInfoScreenDevice->SetAppScreenMode(screenModeList[0]);
				iInfoScreenDevice->SetScreenMode(screenModeList[0]);
				iSession.Finish(ETrue);
				Pause(50);
				}
			else
				{
				completeCount = 0;
				}
			heapFail++;
			if (heapFail == 80)
				{
				ASSERT_TRUE(0);	//worrying amount of fails
				}
			}
		}
	screenModeList.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0033L
@SYMTestCaseDesc		GetConfiguration in OOM
@SYMREQ					REQ10328
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Safety check against OOM
@SYMTestActions			
	Set OOM failure
	GetConfiguration
	Check config
	Increase time till failure
	Success after 5 completed gets
@SYMTestExpectedResults	
	Should always succeed
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0033L()
	{
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt heapFail = 1;
	for (TInt i=0;i<5;i++)
		{
		TDisplayConfiguration config;
		iSession.Finish(ETrue);
		iSession.HeapSetFail(RHeap::EDeterministic,heapFail);
		interface->GetConfiguration(config);
		iSession.HeapSetFail(RHeap::ENone,0);
		iSession.Finish(ETrue);
		ASSERT_TRUE(config.IsDefined(config.ERotation));
		ASSERT_TRUE(config.IsDefined(config.EResolution));
		ASSERT_TRUE(config.IsDefined(config.EResolutionTwips));
		heapFail++;
		}
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0034L
@SYMTestCaseDesc		GetResolutions in OOM
@SYMREQ					REQ10328
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Safety check against OOM
@SYMTestActions			
	Set OOM failure
	GetResolutions
	Check resolutions filled in
	Success after 5 completed gets
@SYMTestExpectedResults	
	Should never panic!
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0034L()
	{
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE(resolutions>1);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	TInt error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_TRUE(resolutionList1.Count() > 0);

	TInt heapFail = 1;
	TInt completeCount = 0;
	TBool notComplete = ETrue;
	while (notComplete)
		{
		RArray<MDisplayControl::TResolution> resolutionList2;
		iSession.Finish(ETrue);
		iSession.HeapSetFail(RHeap::EDeterministic,heapFail);
		error = interface->GetResolutions(resolutionList2);
		iSession.HeapSetFail(RHeap::ENone,0);
		iSession.Finish(ETrue);
		if (error<KErrNone)
			{
			ASSERT_EQUALS(error,KErrNoMemory);
			completeCount = 0;
			}
		else
			{
			ASSERT_EQUALS(resolutionList2.Count(),resolutionList1.Count());
			for (TInt i=0;i<resolutionList2.Count();i++)
				{
				ASSERT_EQUALS(resolutionList1[i].iPixelSize,resolutionList2[i].iPixelSize);
				ASSERT_EQUALS(resolutionList1[i].iTwipsSize,resolutionList2[i].iTwipsSize);
				if (!(resolutionList1[i].iFlags==resolutionList2[i].iFlags))
					{
					ASSERT_TRUE(0);
					}
				}
			completeCount++;
			}
		heapFail++;
		resolutionList2.Close();
		if (completeCount == 5)
			{
			notComplete = EFalse;
			}
		if (heapFail == 20)
			{
			ASSERT_TRUE(0);	//worrying amount of fails
			}
		}
	resolutionList1.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0041L
@SYMTestCaseDesc		Test Drawing of bitmaps (case 0) and surfaces (case 1) in all different AppModes
@SYMREQ					REQ10332 REQ10336
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Check to see if Bitmaps and Surfaces are drawn correctly in different app modes
@SYMTestActions			
	A Simple pattern will be drawn by a bitmap on the left and a surface on the right, about half the window size each. The Composited Screen Capture code will be used to compare the drawing to a unchanged copy of the bitmap to see if they are drawn correctly
	
	Note, add .0 to test name to test bitmap drawing and .1 to test surface drawing
@SYMTestExpectedResults	
	All tests should pass.
	
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0041L(TBool aIsSurface)
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
	TInt error;
	iSession.Finish(ETrue);
	
	TRect screenSize(iInfoScreenDevice->SizeInPixels());
	screenSize.Shrink(5,5);
	
	RBlankWindow testWindow;
	if (iInfoGc)
		{
		testWindow=RBlankWindow(iSession);
		ASSERT_EQUALS(testWindow.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
		testWindow.SetColor(iBlue);
		testWindow.SetExtent(screenSize.iTl,screenSize.Size());
		testWindow.Activate();
		
		if (testWindow.WsHandle())
			{		
			iSession.Flush();
			iSession.Finish();
			}
		testWindow.SetVisible(ETrue);
		}
	
	iSession.Finish(ETrue);
	
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TDisplayConfiguration dispConfigBefore;
	interface->GetConfiguration(dispConfigBefore);
	ASSERT_TRUE(dispConfigBefore.IsDefined(dispConfigBefore.EResolution))
	if (screenModeList.Count()>1)
		{
		//Got a mode to change to!
		TSize resBefore;
		TBool ok=dispConfigBefore.GetResolution(resBefore);
		ASSERT_TRUE(ok);	//we "know" this should be ok, as we just asserted the flag
		for (TInt res=0;res<screenModeList.Count();res++)
			{
				{
					{
					TDisplayConfiguration dispConfigReq;
					iInfoScreenDevice->SetAppScreenMode(screenModeList[res]);
					iInfoScreenDevice->SetScreenMode(screenModeList[res]);
					iSession.Finish(ETrue);
					Pause(300);
					

					TSize pr = iInfoScreenDevice->SizeInPixels();
					TPoint 	origin=iInfoScreenDevice->GetDefaultScreenModeOrigin();					
					Pause(300);
					TSize winpos;
					
					TSize surfaceSize = pr;
					surfaceSize.iWidth /= 2;
					surfaceSize.iWidth -= 20;
					surfaceSize.iHeight -= 20;
					TSurfaceId surfaceID;
					TRAPD(err, surfaceID = iUtility->CreateSurfaceL(surfaceSize, 
								KSurfaceFormat, surfaceSize.iWidth * KBytesPerPixel));
					ASSERT_EQUALS(err,KErrNone);
					TRAP(err,iUtility->PatternFillSurfaceL(surfaceID));
					ASSERT_EQUALS(err,KErrNone);
								
					CFbsBitmap* equivalentBitmap=NULL;
					TRAP(err,equivalentBitmap=iUtility->EquivalentBitmapL(surfaceID));
					CleanupStack::PushL(equivalentBitmap);
					ASSERT_EQUALS(err,KErrNone);
					
					screenSize = iInfoScreenDevice->SizeInPixels();
					TSize testSize = iScreenDevice->SizeInPixels();
					screenSize.Shrink(5,5);
					
					if (iInfoGc)
						{
						testWindow.SetExtent(screenSize.iTl,screenSize.Size());
						
						if (testWindow.WsHandle())
							{
							
							iSession.Flush();
							iSession.Finish();
							}
						testWindow.SetVisible(ETrue);
						}
					Pause(200);			
					iSession.Finish(ETrue);
					
					RWindow surfWindow;
					if (iInfoGc)
						{
						surfWindow=RWindow(iSession);
						ASSERT_EQUALS(surfWindow.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
						surfWindow.SetBackgroundSurface(surfaceID);
						surfWindow.SetExtent(screenSize.iTl+TPoint( screenSize.Width()/2 +5,5),surfaceSize);
						surfWindow.Activate();
						
						if (surfWindow.WsHandle())
							{
							surfWindow.Invalidate();
						
							surfWindow.BeginRedraw();
							ActivateWithWipe(iInfoGc,surfWindow,iRed);
							surfWindow.EndRedraw();
						
							iSession.Flush();
							iSession.Finish();
							}
						surfWindow.SetVisible(ETrue);
						iInfoGc->Deactivate();
						}
					
					iSession.Finish(ETrue);
					
					TRect bitmapDrawRect;
					bitmapDrawRect = surfaceSize;
					
					RWindow bmpWindow;
					if (iInfoGc)
						{
						bmpWindow=RWindow(iSession);
						ASSERT_EQUALS(bmpWindow.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
						bmpWindow.SetBackgroundColor(iRed);
						bmpWindow.SetExtent(screenSize.iTl+TPoint(5,5),surfaceSize);
						bmpWindow.Activate();
						
						if (bmpWindow.WsHandle())
							{
							bmpWindow.Invalidate();
						
							bmpWindow.BeginRedraw();
							ActivateWithWipe(iInfoGc,bmpWindow,iRed);
							// Not drawing, draw twice??
							iInfoGc->DrawBitmap(bitmapDrawRect, equivalentBitmap);
							bmpWindow.EndRedraw();
						
							iSession.Flush();
							iSession.Finish();
							}
						bmpWindow.SetVisible(ETrue);
						iInfoGc->Deactivate();
						}
					Pause(200);			
					iSession.Finish(ETrue);

					
					if (iInfoGc)
						{
						
						if (bmpWindow.WsHandle())
							{
							bmpWindow.Invalidate();
						
							bmpWindow.BeginRedraw();
							ActivateWithWipe(iInfoGc,bmpWindow,iRed);
							// Not drawing, draw twice??
							iInfoGc->DrawBitmap(bitmapDrawRect, equivalentBitmap);
							bmpWindow.EndRedraw();
							
							iSession.Flush();
							iSession.Finish();
							}
						bmpWindow.SetVisible(ETrue);
						iInfoGc->Deactivate();
						}
					Pause(200);			
					iSession.Finish(ETrue);
					
					if (iInfoGc)
						{
								
						if (bmpWindow.WsHandle())
							{
							bmpWindow.Invalidate();
											
							bmpWindow.BeginRedraw();
							ActivateWithWipe(iInfoGc,bmpWindow,iRed);
							// Not drawing, draw twice??
							iInfoGc->DrawBitmap(bitmapDrawRect, equivalentBitmap);
							bmpWindow.EndRedraw();
							iSession.Flush();
							iSession.Finish();
							}
						bmpWindow.SetVisible(ETrue);
						iInfoGc->Deactivate();
						}
					Pause(200);			
					iSession.Finish(ETrue);

					RArray<TBitmapRegionPair> regionArray;					

					RRegion equivRegion;
					TBitmapRegionPair bitmap1Region;
					bitmap1Region.bitmap = equivalentBitmap;
					bitmap1Region.drawRect = TRect(TPoint(0,0),surfaceSize);
					equivRegion.AddRect(TRect(TPoint(0,0),surfaceSize));
					bitmap1Region.region = &equivRegion;

					regionArray.Append(bitmap1Region);


					TDisplayConfiguration dispConfigAfter2;
					interface->GetConfiguration(dispConfigAfter2);
					TSize theSize;
					dispConfigAfter2.GetResolution(theSize);
					TDisplayConfiguration1::TRotation orient;
					dispConfigAfter2.GetRotation(orient);
					if (orient == TDisplayConfiguration1::ERotation90CW || orient == TDisplayConfiguration1::ERotation270CW)
						{
						TInt temp = theSize.iHeight;
						theSize.iHeight = theSize.iWidth;
						theSize.iWidth = temp;
						}
					TRect uiSize(TPoint(0,0), theSize);
					MDisplayMapping* mappingInterface = static_cast<MDisplayMapping*>
								(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
					ASSERT_TRUE(mappingInterface);
					TRect finalSize;
					mappingInterface->MapCoordinates(EApplicationSpace, uiSize, ECompositionSpace, finalSize);
						

					CFbsBitmap* bmp2 = new (ELeave) CFbsBitmap;
					CleanupStack::PushL(bmp2);
					User::LeaveIfError(bmp2->Create(finalSize.Size(), EColor16MU));

					MTestScreenCapture
							* csc =
									static_cast<MTestScreenCapture*> (iScreenDevice->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
					if (csc)
						{
						TInt errr = csc->ComposeScreen(*bmp2);
						ASSERT_TRUE(errr == KErrNone);
						}
					else
						{
						if (!cSCLogged)
							{
							cSCLogged = ETrue;
							_LIT(KNoCSC, "CSC testing not enabled as CSC render stage not defined");
							Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrAll, KNoCSC);
							}
						}
					
					
					TRect compareRect(bmpWindow.AbsPosition(), bmpWindow.Size());
					TRect testRect(surfWindow.AbsPosition(), surfWindow.Size());

					if (csc)
						{
						if(!Compare(*bmp2, compareRect, testRect, regionArray, aIsSurface))
							{
							_LIT(KCompareFailed, "Compare Failed on app mode: %d");
							TBuf<256> x;
							x.Format(KCompareFailed, res+1);
							Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
							}
						}

					CleanupStack::PopAndDestroy(bmp2);

					equivRegion.Close();
					regionArray.Close();
					
					CleanupStack::PopAndDestroy(equivalentBitmap);
					surfWindow.Close();
					bmpWindow.Close();
					}
				}
			}
		//restore initial res
		if (screenModeList.Count() > 1)
            {   //set back to basics
            iInfoScreenDevice->SetAppScreenMode(screenModeList[0]);
            iInfoScreenDevice->SetScreenMode(screenModeList[0]);
            Pause(300);
            }
		
		interface->SetConfiguration(dispConfigBefore);
		Pause(1000);
		TDisplayConfiguration dispConfigAfter;
		interface->GetConfiguration(dispConfigAfter);
		TSize resAfter;
		ok=dispConfigAfter.GetResolution(resAfter);
		ASSERT_TRUE(ok);	
		ASSERT_EQUALS(resBefore,resAfter);
		}
	else
		{
		INFO_PRINTF1(_L("Only 1 screen size mode configured on this screen - res change test skipped."));
		}
	testWindow.Close();
	screenModeList.Close();
	resolutionList1.Close();
	}
	

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0042L
@SYMTestCaseDesc		Test Drawing of bitmaps (case 0) and surfaces (case 1) in all different resolutions
@SYMREQ					REQ10332
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Check to see if Bitmaps and Surfaces are drawn correctly in different resolutions
@SYMTestActions			
	A Simple pattern will be drawn by a bitmap on the left and a surface on the right, about half the window size each. The Composited Screen Capture code will be used to compare the drawing to a unchanged copy of the bitmap to see if they are drawn correctly
	
	Note, add .0 to test name to test bitmap drawing and .1 to test surface drawing
@SYMTestExpectedResults	
	All tests should pass.
	
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0042L(TBool aIsSurface)
	{
	LOG_AND_PANIC_IF_NOT_GCE;
	ResetScreens();
	TInt error;
	iSession.Finish(ETrue);
	
	TRect screenSize(iInfoScreenDevice->SizeInPixels());
	screenSize.Shrink(5,5);
	
	RBlankWindow testWindow;
	if (iInfoGc)
		{
		testWindow=RBlankWindow(iSession);
		ASSERT_EQUALS(testWindow.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
		testWindow.SetColor(iBlue);
		testWindow.SetExtent(screenSize.iTl,screenSize.Size());
		testWindow.Activate();
		
		if (testWindow.WsHandle())
			{		
			iSession.Flush();
			iSession.Finish();
			}
		testWindow.SetVisible(ETrue);
		}
	
	iSession.Finish(ETrue);
	
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	MDisplayControl* interface = static_cast<MDisplayControl*>
				(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 0);
	
	RArray<MDisplayControl::TResolution> resolutionList1;
	error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(error,KErrNone);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	TDisplayConfiguration dispConfigBefore;
	interface->GetConfiguration(dispConfigBefore);

	iInfoScreenDevice->SetAppScreenMode(screenModeList[1]);
	iInfoScreenDevice->SetScreenMode(screenModeList[1]);
	iSession.Finish();
	Pause(50);
	
	if (resolutions>1)
		{
		//Got a mode to change to!
		TSize resBefore;
		TBool ok=dispConfigBefore.GetResolution(resBefore);
		ASSERT_TRUE(ok);	//we "know" this should be ok, as we just asserted the flag
		for (TInt res=0;res<resolutionList1.Count();res++)
			{
			if (resolutionList1[res].iPixelSize!=TSize(0,0))	//this test is not about turning the display off
				{
				TDisplayConfiguration dispConfigReq;
				dispConfigReq.SetResolution(resolutionList1[res].iPixelSize);
				ASSERT_TRUE(dispConfigReq.IsDefined(dispConfigReq.EResolution));
				
				SetRotation(dispConfigReq,resolutionList1[res]);	//set rotation to first defined in res list
				
				
				

				TInt errCode=interface->SetConfiguration(dispConfigReq);
				//ASSERT_EQUALS(errCode,KErrNone);
				if (errCode != KErrNone)
					{
					//Probably current size mode does not support the rotation of the passed in configuration
					
					ASSERT_EQUALS(errCode,KErrArgument);	//failed to find compatible res in the policy
					continue;
					}
				
				
				TSize pr = iInfoScreenDevice->SizeInPixels();
				TPoint 	origin=iInfoScreenDevice->GetDefaultScreenModeOrigin();					
				Pause(300);
				TSize winpos;
				
				TSize surfaceSize = pr;
				surfaceSize.iWidth /= 2;
				surfaceSize.iWidth -= 20;
				surfaceSize.iHeight -= 20;
				TSurfaceId surfaceID;
				TRAPD(err, surfaceID = iUtility->CreateSurfaceL(surfaceSize, 
							KSurfaceFormat, surfaceSize.iWidth * KBytesPerPixel));
				ASSERT_EQUALS(err,KErrNone);
				TRAP(err,iUtility->PatternFillSurfaceL(surfaceID));
				ASSERT_EQUALS(err,KErrNone);
							
				CFbsBitmap* equivalentBitmap=NULL;
				TRAP(err,equivalentBitmap=iUtility->EquivalentBitmapL(surfaceID));
				CleanupStack::PushL(equivalentBitmap);
				ASSERT_EQUALS(err,KErrNone);
				
				screenSize = iInfoScreenDevice->SizeInPixels();
				screenSize.Shrink(5,5);
				
				if (iInfoGc)
					{
					testWindow.SetExtent(screenSize.iTl,screenSize.Size());
					
					if (testWindow.WsHandle())
						{
						
						iSession.Flush();
						iSession.Finish();
						}
					testWindow.SetVisible(ETrue);
					}
				Pause(200);			
				iSession.Finish(ETrue);
				
				RWindow surfWindow;
				if (iInfoGc)
					{
					surfWindow=RWindow(iSession);
					ASSERT_EQUALS(surfWindow.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
					surfWindow.SetBackgroundSurface(surfaceID);
					surfWindow.SetExtent(screenSize.iTl+TPoint( screenSize.Width()/2 +5,5),surfaceSize);
					surfWindow.Activate();
					
					if (surfWindow.WsHandle())
						{
						surfWindow.Invalidate();
					
						surfWindow.BeginRedraw();
						ActivateWithWipe(iInfoGc,surfWindow,iRed);
						surfWindow.EndRedraw();
					
						iSession.Flush();
						iSession.Finish();
						}
					surfWindow.SetVisible(ETrue);
					iInfoGc->Deactivate();
					}
				
				iSession.Finish(ETrue);
				
				TRect bitmapDrawRect;
				bitmapDrawRect = surfaceSize;
				
				RWindow bmpWindow;
				if (iInfoGc)
					{
					bmpWindow=RWindow(iSession);
					ASSERT_EQUALS(bmpWindow.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
					bmpWindow.SetBackgroundColor(iRed);
					bmpWindow.SetExtent(screenSize.iTl+TPoint(5,5),surfaceSize);
					bmpWindow.Activate();
					
					if (bmpWindow.WsHandle())
						{
						bmpWindow.Invalidate();
					
						bmpWindow.BeginRedraw();
						ActivateWithWipe(iInfoGc,bmpWindow,iRed);
						// Not drawing, draw twice??
						iInfoGc->DrawBitmap(bitmapDrawRect, equivalentBitmap);
						bmpWindow.EndRedraw();
					
						iSession.Flush();
						iSession.Finish();
						}
					bmpWindow.SetVisible(ETrue);
					iInfoGc->Deactivate();
					}
				Pause(200);			
				iSession.Finish(ETrue);
				
				
				if (iInfoGc)
					{
					
					if (bmpWindow.WsHandle())
						{
						bmpWindow.Invalidate();
							
						bmpWindow.BeginRedraw();
						ActivateWithWipe(iInfoGc,bmpWindow,iRed);
						// Not drawing, draw twice??
						iInfoGc->DrawBitmap(bitmapDrawRect, equivalentBitmap);
						bmpWindow.EndRedraw();
											
						iSession.Flush();
						iSession.Finish();
						}
					bmpWindow.SetVisible(ETrue);
					iInfoGc->Deactivate();
					}
				Pause(200);			
				iSession.Finish(ETrue);
									

				

				RArray<TBitmapRegionPair> regionArray;					

				RRegion equivRegion;
				TBitmapRegionPair bitmap1Region;
				bitmap1Region.bitmap = equivalentBitmap;
				bitmap1Region.drawRect = TRect(TPoint(0,0),surfaceSize);
				equivRegion.AddRect(TRect(TPoint(0,0),surfaceSize));
				bitmap1Region.region = &equivRegion;

				regionArray.Append(bitmap1Region);


				TDisplayConfiguration dispConfigAfter2;
				interface->GetConfiguration(dispConfigAfter2);
				TSize theSize;
				dispConfigAfter2.GetResolution(theSize);
				TDisplayConfiguration1::TRotation orient;
				dispConfigAfter2.GetRotation(orient);
				if (orient & 0x1)
					{
					TInt temp = theSize.iHeight;
					theSize.iHeight = theSize.iWidth;
					theSize.iWidth = temp;
					}
				TRect uiSize(TPoint(0,0), theSize);
				MDisplayMapping* mappingInterface = static_cast<MDisplayMapping*>
							(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
				ASSERT_TRUE(mappingInterface);
				TRect finalSize;
				mappingInterface->MapCoordinates(EApplicationSpace, uiSize, ECompositionSpace, finalSize);

				
				

				CFbsBitmap* bmp2 = new (ELeave) CFbsBitmap;
				CleanupStack::PushL(bmp2);
				TInt bmpErr = bmp2->Create(finalSize.Size(), EColor16MU);
				ASSERT_EQUALS(bmpErr, KErrNone);

				MTestScreenCapture
						* csc =
								static_cast<MTestScreenCapture*> (iScreenDevice->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
				if (csc)
					{
					TInt errr = csc->ComposeScreen(*bmp2);
					}
				else
					{
					if (!cSCLogged)
						{
						cSCLogged = ETrue;
						_LIT(KNoCSC, "CSC testing not enabled as CSC render stage not defined");
						Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrAll, KNoCSC);
						}
					}
				TRect compareRect(bmpWindow.AbsPosition(), bmpWindow.Size());
				TRect testRect(surfWindow.AbsPosition(), surfWindow.Size());

				if (csc)
					{
					if(!Compare(*bmp2, compareRect, testRect, regionArray, aIsSurface))
						{
						_LIT(KCompareFailed, "Compare Failed on resolution: %d");
						TBuf<256> x;
						x.Format(KCompareFailed, res+1);
						Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, x);
						}
					}
				CleanupStack::PopAndDestroy(bmp2);

				equivRegion.Close();
				regionArray.Close();
				
				CleanupStack::PopAndDestroy(equivalentBitmap);
				surfWindow.Close();
				bmpWindow.Close();
				}
			}
			//restore initial res
			iInfoScreenDevice->SetAppScreenMode(0);
			iInfoScreenDevice->SetScreenMode(0);
			}
		else
			{
			INFO_PRINTF1(_L("Only 1 screen size mode configured on this screen - res change test skipped."));
			}
		testWindow.Close();
		screenModeList.Close();
		resolutionList1.Close();
	}


/**
 Tests to see if the pixels in the bitmap match what should be drawn with regards to the bitmaps and regions in aBitmapRegionPairArray
 It does this by in the case of non-scaled bitmaps, performing a pixel by pixel comparison between what has been drawn to the CSC created bitmap and either the pixels of the bitmap if they are contained within the related region or the pixels of the simulation screen. In the case of scalingm only certain pixels are compared from the CSC bitmap to the comparison drawing as the scaling algorithm is hard to replicate.


 @param aBitmap a bitmap of the entire screen which should be generated using the Composited Screen Capture code
 @param aRect1 A rectangle representing the middle window in the comparison triple
 @param aRect2 A rectangle represneting the right window in the comparison triple
 @param aBitmapRegionPairArray An array of the different regions that are being drawn on the screen. They should be placed in order such that the closest to the foreground is first and the furthest away is last in the queue. Each member of the array is a misleadingly titled TBitmapRegion pair, the bitmap member of this should be the bitmap that is being drawn to the region of the screen, if the region is just a blank color then a blank color bitmap should be drawn. The region should be the region that the bitmap is being drawn to and should be the same as the clipping region which is set jsut before the bitmap is drawn to screen. Finally the draw rect is the basic rectangle the bitmap is being drawn to and is mainly used to test if the bitmap is being scaled
 @return True
 */
TBool CWsDynamicResBasic::Compare(const CFbsBitmap& aBitmap,
		const TRect& aRect1, const TRect& aRect2,
		RArray<TBitmapRegionPair>& aBitmapRegionPairArray, TBool aIsSurface)
	{
	MDisplayMapping* mappingInterface = static_cast<MDisplayMapping*>
				(iScreenDevice->GetInterface(MDisplayMapping::ETypeId));
	ASSERT_TRUE(mappingInterface);
		
	
	TRgb color1;
	TRgb color2;

	TInt errorPixels =0;
	TInt diffPixels = 0;
	RRegion ignoreDueToResizing;
	TInt regionCount = aBitmapRegionPairArray.Count();
	for (TInt i=regionCount-1; i>=0; i--)
		{
		if (aBitmapRegionPairArray[i].bitmap->SizeInPixels()
				!= aBitmapRegionPairArray[i].drawRect.Size())
			{
			RArray<TPoint> corners;
			corners.Append(aBitmapRegionPairArray[i].drawRect.iTl);
			corners.Append(TPoint(aBitmapRegionPairArray[i].drawRect.iBr.iX-1,
					aBitmapRegionPairArray[i].drawRect.iTl.iY));
			corners.Append(TPoint(aBitmapRegionPairArray[i].drawRect.iTl.iX,
					aBitmapRegionPairArray[i].drawRect.iBr.iY-1));
			corners.Append(aBitmapRegionPairArray[i].drawRect.iBr- TPoint(1,1));
			for (TInt j=0; j<corners.Count(); j++)
				{
				TBool inOther=EFalse;
				for (TInt k=0; k<i; k++)
					{
					if (aBitmapRegionPairArray[k].region->Contains(corners[j]))
						{
						inOther = ETrue;
						}
					}
				if (inOther)
					{
					continue;
					}
				aBitmap.GetPixel(color1, aRect2.iTl + corners[j]);
				aBitmap.GetPixel(color2, aRect1.iTl + corners[j]);
				TRgb color3 = GreatestColor(color1);
				TRgb color4 = GreatestColor(color2);
				if (color3 != color4)
					{
					// Bitmap copy can be off by a pixel on grid filled surface so allow for green and yellow to be treated equally as they are the background colours, this may allow a misfunctioning gce to pass the test though.
					if (color4 == TRgb(0, 1, 0) && color3 == TRgb(1, 1, 0))
						{
						}
					else
						if (color4 == TRgb(1, 1, 0) && color3 == TRgb(0, 1, 0))
							{
							}
						else
							{
							corners.Close();
							return EFalse;
							}
					}

				}
			// testPoints contains first the pixel within the destRect followed by the pixel in the original position for comparison to
			RArray<TPoint> testPoints;
			testPoints.Append(aBitmapRegionPairArray[i].drawRect.Center());
			testPoints.Append(TPoint(
					aBitmapRegionPairArray[i].bitmap->SizeInPixels().iWidth/2,
					aBitmapRegionPairArray[i].bitmap->SizeInPixels().iHeight/2));
			testPoints.Append(TPoint(aBitmapRegionPairArray[i].drawRect.iTl.iX,
					aBitmapRegionPairArray[i].drawRect.Center().iY));
			testPoints.Append(TPoint(0,
					aBitmapRegionPairArray[i].bitmap->SizeInPixels().iHeight/2));
			testPoints.Append(TPoint(aBitmapRegionPairArray[i].drawRect.iBr.iX
					-1, aBitmapRegionPairArray[i].drawRect.Center().iY));
			testPoints.Append(TPoint(
					aBitmapRegionPairArray[i].bitmap->SizeInPixels().iWidth-1,
					aBitmapRegionPairArray[i].bitmap->SizeInPixels().iHeight/2));
			TSize sizeee = aBitmapRegionPairArray[i].drawRect.Size();
			if (aBitmapRegionPairArray[i].drawRect.Size().iWidth > 11 && aBitmapRegionPairArray[i].drawRect.Size().iHeight > 20)
				{
				testPoints.Append(TPoint(
						aBitmapRegionPairArray[i].drawRect.Center().iX, aBitmapRegionPairArray[i].drawRect.iTl.iY));
				testPoints.Append(TPoint(
						aBitmapRegionPairArray[i].bitmap->SizeInPixels().iWidth/2, 0));

				testPoints.Append(TPoint(
						aBitmapRegionPairArray[i].drawRect.Center().iX, aBitmapRegionPairArray[i].drawRect.iBr.iY-1));
				testPoints.Append(TPoint(
						aBitmapRegionPairArray[i].bitmap->SizeInPixels().iWidth/2,
						aBitmapRegionPairArray[i].bitmap->SizeInPixels().iHeight-1));

				if (aBitmapRegionPairArray[i].drawRect.Size().iWidth > 34
						&& aBitmapRegionPairArray[i].drawRect.Size().iHeight > 43)
					{
					testPoints.Append(aBitmapRegionPairArray[i].drawRect.iTl
							+ TPoint(5, 5));
					testPoints.Append(TPoint(5, 5));
					testPoints.Append(TPoint(
							aBitmapRegionPairArray[i].drawRect.iBr.iX - 6,
							aBitmapRegionPairArray[i].drawRect.iTl.iY + 5));
					testPoints.Append(TPoint(
							aBitmapRegionPairArray[i].bitmap->SizeInPixels().iWidth-6, 5));
					testPoints.Append(TPoint(
							aBitmapRegionPairArray[i].drawRect.iTl.iX + 5,
							aBitmapRegionPairArray[i].drawRect.iBr.iY -6));
					testPoints.Append(TPoint(5,
							aBitmapRegionPairArray[i].bitmap->SizeInPixels().iHeight - 6));
					testPoints.Append(aBitmapRegionPairArray[i].drawRect.iBr
							- TPoint(6, 6));
					testPoints.Append(TPoint(
							aBitmapRegionPairArray[i].bitmap->SizeInPixels().iWidth-6,
							aBitmapRegionPairArray[i].bitmap->SizeInPixels().iHeight-6));
					}
				else
					{
					testPoints.Append(aBitmapRegionPairArray[i].drawRect.iTl
							+ TPoint(2, 3));
					testPoints.Append(TPoint(5, 5));
					testPoints.Append(TPoint(
							aBitmapRegionPairArray[i].drawRect.iBr.iX - 3,
							aBitmapRegionPairArray[i].drawRect.iTl.iY + 3));
					testPoints.Append(TPoint(
							aBitmapRegionPairArray[i].bitmap->SizeInPixels().iWidth-6, 5));
					testPoints.Append(TPoint(
							aBitmapRegionPairArray[i].drawRect.iTl.iX + 2,
							aBitmapRegionPairArray[i].drawRect.iBr.iY -4));
					testPoints.Append(TPoint(5,
							aBitmapRegionPairArray[i].bitmap->SizeInPixels().iHeight - 6));
					testPoints.Append(aBitmapRegionPairArray[i].drawRect.iBr
							- TPoint(3, 4));
					testPoints.Append(TPoint(
							aBitmapRegionPairArray[i].bitmap->SizeInPixels().iWidth-6,
							aBitmapRegionPairArray[i].bitmap->SizeInPixels().iHeight-6));
					}
				}
			for (int jj=0; jj<testPoints.Count(); jj+=2)
				{
				TBool inOther=EFalse;
				if (!aBitmapRegionPairArray[i].region->Contains(testPoints[jj]))
					{
					continue;
					}
				for (TInt k=0; k<i; k++)
					{
					if (aBitmapRegionPairArray[k].region->Contains(testPoints[jj]))
						{
						inOther = ETrue;
						}
					}
				if (inOther)
					{
					continue;
					}
				TPoint currentPoint = testPoints[jj];
				TRect bound = aBitmapRegionPairArray[i].region->BoundingRect();
				TBool
						contat =
								aBitmapRegionPairArray[i].region->Contains(testPoints[jj]);
				aBitmap.GetPixel(color1, aRect2.iTl + testPoints[jj]);
				aBitmapRegionPairArray[i].bitmap->GetPixel(color2,
						testPoints[jj+1]);
				TRgb color5 = GreatestColor(color1);
				TRgb color6 = GreatestColor(color2);
				if (color5 != color6)
					{
					// Bitmap copy can be off by a pixel on grid filled surface so allow for green and yellow to be treated equally as they are the background colours, this may allow a misfunctioning gce to pass the test though.
					if (color6 == TRgb(0, 1, 0) && color5 == TRgb(1, 1, 0))
						{
						}
					else
						if (color6 == TRgb(1, 1, 0) && color5 == TRgb(0, 1, 0))
							{
							}
						else
							{
							testPoints.Close();
							return EFalse;
							}
					}
				}

			testPoints.Close();
			corners.Close();
			ignoreDueToResizing.Union(*aBitmapRegionPairArray[i].region);
			aBitmapRegionPairArray.Remove(i);
			}
		}

	RRegion superRegion;

	regionCount = aBitmapRegionPairArray.Count();

	for (TInt i=0; i<regionCount; i++)
		{
		superRegion.Union(*(aBitmapRegionPairArray[i].region));
		}
	TRect boundingRect = superRegion.BoundingRect();
	superRegion.Close();
	TInt width = boundingRect.Width();
	TInt height = boundingRect.Height();
	TInt xStart = boundingRect.iTl.iX;
	TInt yStart = boundingRect.iTl.iY;
	
	TSize superSize = aBitmap.SizeInPixels();
	TRgb HHH1;
	TInt nonWhiteCount = 0;
	for (TInt iii = 0; iii<superSize.iWidth; iii++)
		{
		for (TInt jjj=0; jjj<superSize.iHeight; jjj++)
			{
			aBitmap.GetPixel(HHH1, TPoint(iii,jjj));
			if (HHH1 != TRgb (255, 255, 255))
				{
				nonWhiteCount++;
				}
			}
		}

	if (aBitmapRegionPairArray.Count()>0)
		{
        RDebug::Printf("Checking");
        TInt countchecks=0;
		for (TInt i=0; i<width; i++)
			{
			for (TInt j=0; j<height; j++)
				{
				TInt arrayIndex = 0;
				TBool pointInArray= EFalse;
				TPoint currentPoint(xStart + i, yStart + j);
				if (!ignoreDueToResizing.Contains(currentPoint))
					{
					do
						{
						if (aBitmapRegionPairArray[arrayIndex].region->Contains(currentPoint))
							{							
							if (aIsSurface)
								{
								TPoint checkPoint = currentPoint + aRect2.iTl;
								TRect readBackRect;
								mappingInterface->MapCoordinates(EApplicationSpace, TRect(checkPoint, TSize(1,1)), ECompositionSpace, readBackRect);
								aBitmap.GetPixel(color1, readBackRect.iTl);
								}
							else 
								{
								TPoint checkPoint = currentPoint + aRect1.iTl;
								TRect readBackRect;
								mappingInterface->MapCoordinates(EApplicationSpace, TRect(checkPoint, TSize(1,1)), ECompositionSpace, readBackRect);
								aBitmap.GetPixel(color1, readBackRect.iTl);
								}

							aBitmapRegionPairArray[arrayIndex].bitmap->GetPixel(color2,	currentPoint
																		- aBitmapRegionPairArray[arrayIndex].drawRect.iTl);
							
							TRgb color7 = GreatestColor(color1);
							TRgb color8 = GreatestColor(color2);

							if (color7 != color8)
								{
								diffPixels++;
								//return EFalse;
								}
							pointInArray = ETrue;
							}
						arrayIndex++;
						}
					while (!pointInArray && !(arrayIndex
							>= aBitmapRegionPairArray.Count()));

					if (!pointInArray)
						{
						aBitmap.GetPixel(color1, TPoint(i + aRect2.iTl.iX
								+ xStart, j +aRect2.iTl.iY +yStart));
						aBitmap.GetPixel(color2, TPoint(i + aRect1.iTl.iX
								+xStart, j +aRect1.iTl.iY+yStart));
						// Bitmap copy is duller on first few iterations so just pick the greatest color or two colors and see if they match
						TRgb color3 = GreatestColor(color1);
						TRgb color4 = GreatestColor(color2);

						if (color3 != color4)
							{
							// Bitmap copy can be off by a pixel on grid filled surface so allow for green and yellow to be treated equally as they are the background colours, this may allow a misfunctioning gce to pass the test though.
							if (GreatestColor(color2) == TRgb(0, 1, 0)
									&& GreatestColor(color1) == TRgb(1, 1, 0))
								{
								continue;
								}
							else
								if (GreatestColor(color2) == TRgb(1, 1, 0)
										&& GreatestColor(color1) == TRgb(0, 1,
												0))
									{
									continue;
									}
								else
									{
									errorPixels++;
									//return EFalse;
									}
							}
						}
					}
				}
			}
		}
	
	ignoreDueToResizing.Close();
	if (errorPixels+diffPixels < 2)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}

	}


/*
  Creates a rough apprxoiamtion to the color pass in. This is used because of issues with the bitmap drwaing in the comparison window. If these issues are fixed then this method can be changed to simply return aColor.

 @param aColor the color to be approximated
 @return an approximation of aColor.
 */
TRgb CWsDynamicResBasic::GreatestColor(TRgb& aColor) const
	{
	TInt test = aColor.Difference(TRgb(0, 0, 0));
	// Accounts for a case when Rgb = 20,20,20 due to poor bitmap drawing
	if (aColor == TRgb(32, 32, 32))
		{
		return TRgb(0, 0, 0);
		}
	if (aColor == TRgb(0, 0, 0))
		{
		return TRgb(0, 0, 0);
		}
	TRgb test2;
	test2.SetInternal(0xFFFEDF01);
	if (aColor == test2)
		{
		return TRgb(1, 1, 0);
		}

	if (test < 70)
		{
		return TRgb(0, 0, 0);
		}
	if (aColor.Green() > aColor.Blue())
		{
		if (aColor.Green() > aColor.Red())
			{
			return TRgb(0, 1, 0);
			}
		else
			if (aColor.Green() == aColor.Red())
				{
				return TRgb(1, 1, 0);
				}
		}
	if (aColor.Green() > aColor.Red())
		{
		if (aColor.Green() > aColor.Blue())
			{
			return TRgb(0, 1, 0);
			}
		else
			if (aColor.Green() == aColor.Blue())
				{
				return TRgb(0, 1, 1);
				}
		}

	if (aColor.Red() > aColor.Green())
		{
		if (aColor.Red() > aColor.Blue())
			{
			return TRgb(1, 0, 0);
			}
		else
			if (aColor.Red() == aColor.Blue())
				{
				return TRgb(1, 0, 1);
				}
		}
	if (aColor.Red() > aColor.Blue())
		{
		if (aColor.Red() > aColor.Green())
			{
			return TRgb(1, 0, 0);
			}
		else
			if (aColor.Red() == aColor.Green())
				{
				return TRgb(1, 1, 0);
				}
		}

	if (aColor.Blue() > aColor.Red())
		{
		if (aColor.Blue() > aColor.Green())
			{
			return TRgb(0, 0, 1);
			}
		else
			if (aColor.Blue() == aColor.Green())
				{
				return TRgb(0, 1, 1);
				}
		}
	if (aColor.Blue() > aColor.Green())
		{
		if (aColor.Blue() > aColor.Red())
			{
			return TRgb(0, 0, 1);
			}
		else
			if (aColor.Blue() == aColor.Red())
				{
				return TRgb(1, 0, 1);
				}
		}

	// Should never reach here, but the compiler cannot be sure
	return TRgb(0, 0, 0);
	}

/*
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0051L
@SYMTestCaseDesc		Getting if a mode is dynamic
@SYMREQ					REQ11554
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Check functions work correctly
@SYMTestActions			
	Do IsModeDynamic for a wrong mode number
	Do IsModeDynamic for a mode that isnt dynamic
	Do IsModeDynamic for a mode that is dynamic
	Do IsCurrentModeDynamic when current mode is not dynamic
	Do IsCurrentModeDynamic when current mode is dynamic
	MODE 10 must be dynamic
@SYMTestExpectedResults	
	All should return as expected.
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0051L()
	{
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);
	iInfoScreenDevice->SetAppScreenMode(0);
	iInfoScreenDevice->SetScreenMode(0);
	iSession.Flush();
	Pause(50);
	ASSERT_FALSE(iInfoScreenDevice->IsModeDynamic(25));
	ASSERT_FALSE(iInfoScreenDevice->IsModeDynamic(1));
	ASSERT_TRUE(iInfoScreenDevice->IsModeDynamic(10));
	ASSERT_FALSE(iInfoScreenDevice->IsCurrentModeDynamic());
	iInfoScreenDevice->SetAppScreenMode(10);
	iInfoScreenDevice->SetScreenMode(10);
	iSession.Flush();
	Pause(50);
	ASSERT_TRUE(iInfoScreenDevice->IsCurrentModeDynamic());
	screenModeList.Close();
	iInfoScreenDevice->SetAppScreenMode(0);
	iInfoScreenDevice->SetScreenMode(0);
	}

/* Takes any 0x0 resolutions out of the res list, useful for some tests
*/
void ResListCleanup(RArray<MDisplayControl::TResolution>& aResList)
	{
	for (TInt ii=0; ii<aResList.Count(); ii++)
		{
		if (aResList[ii].iPixelSize.iWidth == 0 ||
				aResList[ii].iPixelSize.iHeight == 0)
			{
			aResList.Remove(ii);
			ii--;
			}
		}
	}

/*
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0052L
@SYMTestCaseDesc		Getting if a mode is dynamic
@SYMREQ					REQ11554
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Check functions work correctly
@SYMTestActions			
	For every standard screen mode
		Check twips and pixel conversions are correct based on screen mode values
	Set dynamic app mode
	For every resolution
		Check twips and pixel conversions are correct based on physical screen values
@SYMTestExpectedResults	
	All should return as expected.
	NOTE- there must be a dynamic screen mode available
	NOTE- only run on a screen with multiple resolutions available
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0052L()
	{
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 1);
		
	RArray<MDisplayControl::TResolution> resolutionList1;
	TInt error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	ResListCleanup(resolutionList1);	//remove any 0x0
	ASSERT_TRUE (resolutionList1.Count() > 1);	//without multiple proper res this test is pointless
	
	TDisplayConfiguration newResolution;
	newResolution.SetResolution(resolutionList1[0].iPixelSize);
	error = interface->SetConfiguration(newResolution);
	ASSERT_EQUALS(error,KErrNone);
	
	TSizeMode info;
	RArray<TInt> screenModes;
	error = iInfoScreenDevice->GetScreenSizeModeList(&screenModes);
	ASSERT_TRUE(error>KErrNone);
	TInt dynamicMode1 = -1;
	TInt dynamicMode2 = -1;
	TPixelsTwipsAndRotation modeAttributes;
	
	//for every app mode, check calculations update correctly
	for (TInt ii=0;ii<screenModes.Count();ii++)
		{
		TInt screenMode = screenModes[ii];
		if (iInfoScreenDevice->IsModeDynamic(screenMode))
			{
			CArrayFixFlat<TInt>* rotations=new(ELeave) CArrayFixFlat<TInt>(1);
			CleanupStack::PushL(rotations);
			TInt error = iInfoScreenDevice->GetRotationsList(screenMode,rotations);
			ASSERT_EQUALS(error,KErrNone);
			ASSERT_TRUE(rotations->Count()>0);
			for (TInt jj = 0; jj < rotations->Count(); jj++)
			    {
			    if ((*rotations)[jj] == (TInt)CFbsBitGc::EGraphicsOrientationNormal ||
			            (*rotations)[jj] == (TInt)CFbsBitGc::EGraphicsOrientationRotated180)
                    {
                    dynamicMode1 = screenMode;
                    }
			    else if ((*rotations)[jj] == (TInt)CFbsBitGc::EGraphicsOrientationRotated90 ||
			            (*rotations)[jj] == (TInt)CFbsBitGc::EGraphicsOrientationRotated270)
                    {
                    dynamicMode2 = screenMode;
                    }
			    }
			CleanupStack::PopAndDestroy(rotations);
			continue;	//dont want to test dynamic modes
			}
		
		INFO_PRINTF2(_L("ScreenMode %d"),screenMode);
		newResolution.ClearAll();
		
		iInfoScreenDevice->SetAppScreenMode(screenMode);
		iInfoScreenDevice->SetScreenMode(screenMode);
		iSession.Flush();
		Pause(50);
		
		info = iInfoScreenDevice->GetCurrentScreenModeAttributes();
		//These 2 asserts relate to DEF136304 - disconnect on startup causes invalid
		//twips values.  They need to be calculated when the screen is connected
		ASSERT_TRUE(info.iScreenTwipsSize.iWidth < 40000);
		ASSERT_TRUE(info.iScreenTwipsSize.iHeight < 40000);
		
		//test that conversions correlate to values reported by config
		TInt test = iInfoScreenDevice->HorizontalTwipsToPixels(
				info.iScreenTwipsSize.iWidth);
		ASSERT_EQUALS (test,info.iScreenSize.iWidth);
		
		test = iInfoScreenDevice->VerticalTwipsToPixels(
				info.iScreenTwipsSize.iHeight);
		ASSERT_EQUALS (test,info.iScreenSize.iHeight);
		
		test = iInfoScreenDevice->HorizontalPixelsToTwips(
				info.iScreenSize.iWidth);
		ASSERT_EQUALS (test,info.iScreenTwipsSize.iWidth);
		
		test = iInfoScreenDevice->VerticalPixelsToTwips(
				info.iScreenSize.iHeight);
		ASSERT_EQUALS (test,info.iScreenTwipsSize.iHeight);
		}

	if (dynamicMode1 == -1 && dynamicMode2 == -1)	//expected to find at least 1 dynamic mode
		{
		ASSERT_TRUE(0);
		}

	for (TInt jj=0;jj<2;jj++)  //for dynamic mode 1 and 2
	    {
	    TInt dynMode = (jj == 0) ? dynamicMode1 : dynamicMode2;
	    if (dynMode == -1)
	        continue;
        //set dynamic app mode
	    iInfoScreenDevice->SetAppScreenMode(dynMode);
	    iInfoScreenDevice->SetScreenMode(dynMode);
	    iSession.Flush();
	    Pause(50);
	    ASSERT_TRUE(iInfoScreenDevice->IsModeDynamic(dynMode));
	    ASSERT_TRUE(iInfoScreenDevice->IsCurrentModeDynamic());
	    ASSERT_EQUALS(dynMode,iInfoScreenDevice->CurrentScreenMode());
        for (TInt ii=0;ii<resolutionList1.Count();ii++)
            {
            INFO_PRINTF2(_L("ResIndex %d"),ii);
            newResolution.ClearAll();
            //set config
            newResolution.SetResolution(resolutionList1[ii].iPixelSize);
            newResolution.SetResolutionTwips(resolutionList1[ii].iTwipsSize);
            SetRotation(newResolution,resolutionList1[ii]);
            TDisplayConfiguration1::TRotation tempSetRot;
            newResolution.GetRotation(tempSetRot);
            
            error = interface->SetConfiguration(newResolution);

            if (jj == 0)
                {
                if (tempSetRot == TDisplayConfiguration1::ERotation90CW ||
                        tempSetRot == TDisplayConfiguration1::ERotation270CW) //mode rotation will not work with config rotation
                    {
                    ASSERT_EQUALS(error,KErrArgument);
                    continue;
                    }
                }
            else    //jj == 1
                {
                if (tempSetRot == TDisplayConfiguration1::ERotationNormal ||
                        tempSetRot == TDisplayConfiguration1::ERotation180) //mode rotation will not work with config rotation
                    {
                    ASSERT_EQUALS(error,KErrArgument);
                    continue;
                    }
                }
            ASSERT_EQUALS(error,KErrNone);
            interface->GetConfiguration(newResolution);
            iSession.Flush();
            Pause(50);
            
            //test that conversions correlate to values reported by config
            TInt test = iInfoScreenDevice->HorizontalTwipsToPixels(
                    resolutionList1[ii].iTwipsSize.iWidth);
            ASSERT_EQUALS (test,resolutionList1[ii].iPixelSize.iWidth);
            
            test = iInfoScreenDevice->VerticalTwipsToPixels(
                    resolutionList1[ii].iTwipsSize.iHeight);
            ASSERT_EQUALS (test,resolutionList1[ii].iPixelSize.iHeight);
            
            test = iInfoScreenDevice->HorizontalPixelsToTwips(
                    resolutionList1[ii].iPixelSize.iWidth);
            ASSERT_EQUALS (test,resolutionList1[ii].iTwipsSize.iWidth);
            
            test = iInfoScreenDevice->VerticalPixelsToTwips(
                    resolutionList1[ii].iPixelSize.iHeight);
            ASSERT_EQUALS (test,resolutionList1[ii].iTwipsSize.iHeight);
            }
	    }

	resolutionList1.Close();
	screenModes.Close();
	}

/*
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0053L
@SYMTestCaseDesc		SetScreenSizeAndRotation
@SYMREQ					REQ11554
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPriority		1
@SYMTestPurpose			Check functions work correctly
@SYMTestActions			
	Perform calls to SetScreenSizeAndRotation with the various structure types
	MODE 10 must be dynamic
@SYMTestExpectedResults	
	Cannt fail, used for debugging and coverage.
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0053L()
	{
	MDisplayControl* interface = static_cast<MDisplayControl*>
			(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
	ASSERT_TRUE(interface);
	
	TInt resolutions = interface->NumberOfResolutions();
	ASSERT_TRUE (resolutions > 1);
		
	RArray<MDisplayControl::TResolution> resolutionList1;
	TInt error = interface->GetResolutions(resolutionList1);
	ASSERT_EQUALS(resolutionList1.Count(), resolutions);
	
	//set default app mode and default resolution
	iInfoScreenDevice->SetAppScreenMode(0);
	iInfoScreenDevice->SetScreenMode(0);
	iSession.Flush();
	Pause(50);
	TDisplayConfiguration original;
	original.SetResolution(resolutionList1[0].iPixelSize);
	interface->SetConfiguration(original);
	iSession.Flush();
	Pause(50);
	
	TPixelsAndRotation setup0;
	setup0.iPixelSize = TSize(30,40);
	setup0.iRotation = CFbsBitGc::EGraphicsOrientationNormal;
	
	TPixelsTwipsAndRotation setup1;
	setup1.iPixelSize = TSize(50,60);
	setup1.iTwipsSize = TSize(70,80);
	setup1.iRotation = CFbsBitGc::EGraphicsOrientationNormal;
	
	iInfoScreenDevice->SetScreenSizeAndRotation(setup0);
	iInfoScreenDevice->SetScreenSizeAndRotation(setup1);
	
	//set dynamic app mode
	iInfoScreenDevice->SetAppScreenMode(10);
	iInfoScreenDevice->SetScreenMode(10);
	iSession.Flush();
	Pause(50);
	
	iInfoScreenDevice->SetScreenSizeAndRotation(setup0);
	iInfoScreenDevice->SetScreenSizeAndRotation(setup1);

	const CWsScreenDevice* newDevice = iScreenDevice;
	const MDisplayControl* interface2 = (MDisplayControl*)
			newDevice->GetInterface(MDisplayControl::ETypeId);
	TInt version = interface2->PreferredDisplayVersion();	//for coverage!
	(void)version;

	//set default app mode and default resolution
	iInfoScreenDevice->SetAppScreenMode(1);
	iInfoScreenDevice->SetScreenMode(1);
	iSession.Flush();
	Pause(50);
	interface->SetConfiguration(original);
	iSession.Flush();
	Pause(50);
	
	resolutionList1.Close();
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0101L
@SYMTestCaseDesc		Quick demonstration of app modes sizes and positions on screen
@SYMREQ					
@SYMPREQ				PREQ2102
@SYMTestType			CT (manual visual test)
@SYMTestPurpose			To show app modes
@SYMTestActions			
	For every app mode
	Draw a blue box showing the full size and position of the app mode.
	Draw thin red lines demonstrating a window can be drawn outside of the app area to fill
	the screen.
	Green borders at the edge of the appmode, to show we are drawing up to the edges of the
	appmode.
@SYMTestExpectedResults	
	Blue window should represent appmode (with offset and size)
	Red windows should be visible across screen
	Green border (made of windows) around the appmode (blue window)
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0101L()
	{
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);

	if (screenModeList.Count()>1)
		{
		//Got modes to change between!
		TSize resBefore;
		for (TInt res=0;res<screenModeList.Count();res++)
			{
			TDisplayConfiguration dispConfigReq;
			iInfoScreenDevice->SetAppScreenMode(screenModeList[res]);
			iInfoScreenDevice->SetScreenMode(screenModeList[res]);
			iSession.Flush();
			Pause(50);

			TSize appSize = iInfoScreenDevice->SizeInPixels();
			RWindow backWindow(iSession);
			ASSERT_EQUALS(backWindow.Construct(iGroup, 123455), KErrNone);
			backWindow.SetRequiredDisplayMode(iDisplayMode);
			backWindow.SetBackgroundColor(iCyan);
			backWindow.SetExtent(TPoint(40,40),appSize-TSize(40,40));	//shows size of the apparea
			backWindow.Activate();
		
			TSurfaceId surfaceID;
			TRAPD(err, surfaceID = iUtility->CreateSurfaceL(TSize(200,200), 
					KSurfaceFormat, 200 * KBytesPerPixel));
			ASSERT_EQUALS(err,KErrNone);
			TRAP(err,iUtility->FanFillSurfaceL(surfaceID,iYellow,iRed,iMagenta));
			ASSERT_EQUALS(err,KErrNone);
			TSurfaceConfiguration surfConf;
			surfConf.SetSurfaceId(surfaceID);
			
			backWindow.SetBackgroundSurface(surfaceID);
			
			DrawPlainUI(backWindow,ETrue,iBlue);
			iSession.Flush();
			Pause(200);

			RWindow longWindow1(iSession);
			ASSERT_EQUALS(longWindow1.Construct(iGroup, 123456), KErrNone);
			longWindow1.SetRequiredDisplayMode(iDisplayMode);
			longWindow1.SetBackgroundColor(iRed);
			longWindow1.SetExtent(TPoint(-2000,10),TSize(5000,5));	//shows you can draw outside the apparea
			longWindow1.Activate();
			DrawPlainUI(longWindow1,ETrue,iRed);

			iBackground.SetColor(TRgb(0x001000*res|0x800000));
			iSession.SetBackgroundColor(TRgb(0x001000*res+0x40));
			iSession.Flush();
			Pause(100);
			
			RWindow longWindow2(iSession);
			ASSERT_EQUALS(longWindow2.Construct(iGroup, 123457), KErrNone);
			longWindow2.SetRequiredDisplayMode(iDisplayMode);
			longWindow2.SetBackgroundColor(iRed);
			longWindow2.SetExtent(TPoint(20,-2000),TSize(5,5000));	//shows you can draw outside the apparea
			longWindow2.Activate();
			DrawPlainUI(longWindow2,ETrue,iRed);
			
			//borders
			RWindow borderTop(iSession);
			ASSERT_EQUALS(borderTop.Construct(iGroup, 123460), KErrNone);
			borderTop.SetRequiredDisplayMode(iDisplayMode);
			borderTop.SetBackgroundColor(iGreen);
			borderTop.SetExtent(TPoint(0,0),TSize(appSize.iWidth,5));	//border
			borderTop.Activate();
			DrawPlainUI(borderTop,ETrue,iGreen);
			
			RWindow borderLeft(iSession);
			ASSERT_EQUALS(borderLeft.Construct(iGroup, 123461), KErrNone);
			borderLeft.SetRequiredDisplayMode(iDisplayMode);
			borderLeft.SetBackgroundColor(iGreen);
			borderLeft.SetExtent(TPoint(0,0),TSize(5,appSize.iHeight));	//border
			borderLeft.Activate();
			DrawPlainUI(borderLeft,ETrue,iGreen);
						
			RWindow borderRight(iSession);
			ASSERT_EQUALS(borderRight.Construct(iGroup, 123462), KErrNone);
			borderRight.SetRequiredDisplayMode(iDisplayMode);
			borderRight.SetBackgroundColor(iGreen);
			borderRight.SetExtent(TPoint(appSize.iWidth-5,0),TSize(5,appSize.iHeight));	//border
			borderRight.Activate();
			DrawPlainUI(borderRight,ETrue,iGreen);
									
			RWindow borderBottom(iSession);
			ASSERT_EQUALS(borderBottom.Construct(iGroup, 123463), KErrNone);
			borderBottom.SetRequiredDisplayMode(iDisplayMode);
			borderBottom.SetBackgroundColor(iGreen);
			borderBottom.SetExtent(TPoint(0,appSize.iHeight-5),TSize(appSize.iWidth,5));	//border
			borderBottom.Activate();
			DrawPlainUI(borderBottom,ETrue,iGreen);

			iSession.Flush();
			Pause(100);

			backWindow.Close();
			longWindow1.Close();
			longWindow2.Close();
			borderTop.Close();
			borderLeft.Close();
			borderRight.Close();
			borderBottom.Close();
			
			iSession.Finish();
			Pause(50);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Only 1 screen size mode configured on this screen - test skipped."));
		}

	screenModeList.Close();
	iInfoScreenDevice->SetAppScreenMode(0);
	iInfoScreenDevice->SetScreenMode(0);
	iSession.Finish(ETrue);
	Pause(100);
	INFO_PRINTF1(_L("Drawing to borderBottom completed."));
	}

/**
@SYMTestCaseID			GRAPHICS_WSERV_DYNAMICRES_0102L
@SYMTestCaseDesc		Quick demonstration of square app modes in all 4 rotations
@SYMREQ					
@SYMPREQ				PREQ2102
@SYMTestType			CT (manual visual test)
@SYMTestPurpose			To show square app mode in all 4 rotations
@SYMTestActions			
	Draw a blue box showing the full size and position of the app mode.
	Draw thin red lines demonstrating a window can be drawn outside of the app area to fill
	the screen.
	Green borders at the edge of the appmode, to show we are drawing up to the edges of the
	appmode.
@SYMTestExpectedResults	
	Blue window should represent appmode (with offset and size)
	Red windows should be visible across screen
	Green border (made of windows) around the appmode (blue window)
*/
void	CWsDynamicResBasic::GRAPHICS_WSERV_DYNAMICRES_0102L()
	{
	RArray<TInt> screenModeList;
	iInfoScreenDevice->GetScreenSizeModeList(&screenModeList);


	TSize resBefore;
	TInt res;
	for (res=0;res<screenModeList.Count();res++)
		{
		iInfoScreenDevice->SetAppScreenMode(screenModeList[res]);
		iInfoScreenDevice->SetScreenMode(screenModeList[res]);

		iSession.Finish(ETrue);
		iSession.Flush();
		Pause(50);
		
		TPixelsAndRotation pr;
		iInfoScreenDevice->GetDefaultScreenSizeAndRotation(pr);
		if(pr.iPixelSize.iHeight == pr.iPixelSize.iWidth)
			{
			//found square appmode
			break;
			}
		}
	
	for(TInt rotation = CFbsBitGc::EGraphicsOrientationNormal; rotation <= CFbsBitGc::EGraphicsOrientationRotated270; rotation++)
		{
		iScreenDevice->SetCurrentRotations(screenModeList[res], (CFbsBitGc::TGraphicsOrientation)rotation);
		iSession.Flush();
		Pause(50);
		TDisplayConfiguration dispConfigReq;
		TSize appSize = iInfoScreenDevice->SizeInPixels();
		RWindow backWindow(iSession);
		ASSERT_EQUALS(backWindow.Construct(iGroup, 123455), KErrNone);
		backWindow.SetRequiredDisplayMode(iDisplayMode);
		backWindow.SetBackgroundColor(iBlue);
		backWindow.SetExtent(TPoint(0,0),appSize);	//shows size of the apparea
		backWindow.Activate();
		DrawPlainUI(backWindow,ETrue,iBlue);
		iSession.Flush();
		Pause(200);

		RWindow longWindow1(iSession);
		ASSERT_EQUALS(longWindow1.Construct(iGroup, 123456), KErrNone);
		longWindow1.SetRequiredDisplayMode(iDisplayMode);
		longWindow1.SetBackgroundColor(iRed);
		longWindow1.SetExtent(TPoint(-2000,10),TSize(5000,5));	//shows you can draw outside the apparea
		longWindow1.Activate();
		DrawPlainUI(longWindow1,ETrue,iRed);

		iBackground.SetColor(TRgb(0x001000*res|0x800000));
		iSession.SetBackgroundColor(TRgb(0x001000*res+0x40));
		iSession.Flush();
		Pause(100);
		
		RWindow longWindow2(iSession);
		ASSERT_EQUALS(longWindow2.Construct(iGroup, 123457), KErrNone);
		longWindow2.SetRequiredDisplayMode(iDisplayMode);
		longWindow2.SetBackgroundColor(iRed);
		longWindow2.SetExtent(TPoint(20,-2000),TSize(5,5000));	//shows you can draw outside the apparea
		longWindow2.Activate();
		DrawPlainUI(longWindow2,ETrue,iRed);
		
		//borders
		RWindow borderTop(iSession);
		ASSERT_EQUALS(borderTop.Construct(iGroup, 123460), KErrNone);
		borderTop.SetRequiredDisplayMode(iDisplayMode);
		borderTop.SetBackgroundColor(iGreen);
		borderTop.SetExtent(TPoint(0,0),TSize(appSize.iWidth,5));	//border
		borderTop.Activate();
		DrawPlainUI(borderTop,ETrue,iGreen);
		
		RWindow borderLeft(iSession);
		ASSERT_EQUALS(borderLeft.Construct(iGroup, 123461), KErrNone);
		borderLeft.SetRequiredDisplayMode(iDisplayMode);
		borderLeft.SetBackgroundColor(iGreen);
		borderLeft.SetExtent(TPoint(0,0),TSize(5,appSize.iHeight));	//border
		borderLeft.Activate();
		DrawPlainUI(borderLeft,ETrue,iGreen);
					
		RWindow borderRight(iSession);
		ASSERT_EQUALS(borderRight.Construct(iGroup, 123462), KErrNone);
		borderRight.SetRequiredDisplayMode(iDisplayMode);
		borderRight.SetBackgroundColor(iGreen);
		borderRight.SetExtent(TPoint(appSize.iWidth-5,0),TSize(5,appSize.iHeight));	//border
		borderRight.Activate();
		DrawPlainUI(borderRight,ETrue,iGreen);
								
		RWindow borderBottom(iSession);
		ASSERT_EQUALS(borderBottom.Construct(iGroup, 123463), KErrNone);
		borderBottom.SetRequiredDisplayMode(iDisplayMode);
		borderBottom.SetBackgroundColor(iGreen);
		borderBottom.SetExtent(TPoint(0,appSize.iHeight-5),TSize(appSize.iWidth,5));	//border
		borderBottom.Activate();
		DrawPlainUI(borderBottom,ETrue,iGreen);

		iSession.Flush();
		Pause(100);

		backWindow.Close();
		longWindow1.Close();
		longWindow2.Close();
		borderTop.Close();
		borderLeft.Close();
		borderRight.Close();
		borderBottom.Close();
		
		iSession.Finish();
		Pause(50);
		
		}


	screenModeList.Close();
	iInfoScreenDevice->SetAppScreenMode(0);
	iInfoScreenDevice->SetScreenMode(0);
	iSession.Finish(ETrue);
	Pause(100);
	}

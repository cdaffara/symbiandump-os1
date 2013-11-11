// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Set of tests for Tracing Device Rotation. These tests generally test
// RWsSession::IndicateAppOrientation(...).
//

/**
 @file
 @test
 @internalComponent - Internal Nokia test code
*/

#include "tdevicerotation.h"
#include "themeserverpropertydefine.h"
#include "..\..\nga\server\renderorientationtracker.h" 
#include <hal.h>
#include <hal_data.h>
#include <w32std.h>

const TInt KPublishTimeout = 1000000; // 1 second in microseconds

//
// CTDeviceRotation Definition
//

CTDeviceRotation::CTDeviceRotation(CTestStep* aStep):
	CTGraphicsBase(aStep), iWaitForPublishOnNextTest(ETrue)
	{
	}

CTDeviceRotation::~CTDeviceRotation()
	{
    iPublishTimer.Close();
    
    iChildWindow.Close();
    iWindowGroup.Close();
    iWs.Close();    
    
    iSecondChildWindow.Close();
    iSecondWindowGroup.Close();    
    iSecondWs.Close();
   
    /* This Process called with the argument KThemeServerPropertyDefineCmdDelete, deletes 
       the theme server RProperty. This is because an RProperty can only be defined and 
       deleted from within a process with the same UID3 as the RProperty catogory you are 
       trying to define/delete.*/
    RProcess themeServerPropertyDefine;
    TInt err = themeServerPropertyDefine.Create(KThemeServerPropertyDefine,
            KThemeServerPropertyDefineCmdDelete);
    if (KErrNone != err)
        {
        _LIT(KLog, "themeServerPropertyDefine.Create() failed with error: %d");
        INFO_PRINTF2(KLog, err);
        TEST(EFalse);        
        }
    //wait for themeServerPropertyDefine process to terminate
    TRequestStatus themeServerPropertyDefineLogonStatus;
    themeServerPropertyDefine.Logon(themeServerPropertyDefineLogonStatus);
    themeServerPropertyDefine.Resume();
    User::WaitForRequest(themeServerPropertyDefineLogonStatus);
    if (themeServerPropertyDefineLogonStatus != KErrNone)
        {
        _LIT(KLog, "themeServerPropertyDefine.Logon() failed with error: %d");
        INFO_PRINTF2(KLog, themeServerPropertyDefineLogonStatus);
        TEST(EFalse);        
        }
    themeServerPropertyDefine.Close();
    
    iRenderOrientationProperty.Delete(KRenderOrientationCategory, KRenderOrientationKey);
    iRenderOrientationProperty.Close();
    iThemeServerOrientationProperty.Close();
    }

void CTDeviceRotation::ConstructL()
    {
    TInt err = iWs.Connect();
    TESTL(err == KErrNone);
            
    err = iSecondWs.Connect();
    TESTL(KErrNone == err);
    
    iWs.SetAutoFlush(ETrue);
    iSecondWs.SetAutoFlush(ETrue);
    
    iWindowGroup = RWindowGroup(iWs);
    err = iWindowGroup.Construct(iWs.Handle());
    TESTL(KErrNone == err);
    iWindowGroup.SetOrdinalPosition(0,1);
    
    iChildWindow = RWindow(iWs);
    err = iChildWindow.Construct(iWindowGroup, reinterpret_cast<TUint32>(&iChildWindow));
    TESTL(KErrNone == err);    

    iSecondWindowGroup = RWindowGroup(iSecondWs);
    err = iSecondWindowGroup.Construct(iSecondWs.Handle());
    TESTL(KErrNone == err);
    iSecondWindowGroup.SetOrdinalPosition(1,1);
    
    iSecondChildWindow = RWindow(iSecondWs);
    err = iSecondChildWindow.Construct(iSecondWindowGroup, reinterpret_cast<TUint32>(&iSecondChildWindow));
    TESTL(KErrNone == err);    
    
    err= iRenderOrientationProperty.Attach(KRenderOrientationCategory, KRenderOrientationKey, EOwnerThread);
    TESTL(KErrNone == err);
    iRenderOrientationProperty.Subscribe(iRenderOrientationStatus);
    
    /* This Process called with the argument KThemeServerPropertyDefineCmdDefine, defines
       the theme server catagory to be the same as the theme servers process ID. This is 
       because an RProperty can only be defined and deleted from within a process with the 
       same UID3 as the RProperty catogory you are trying to define/delete.*/
    RProcess themeServerPropertyDefine;
    err = themeServerPropertyDefine.Create(KThemeServerPropertyDefine,KThemeServerPropertyDefineCmdDefine);
    TESTL(KErrNone == err);
    TRequestStatus themeServerPropertyDefineLogonStatus;
    themeServerPropertyDefine.Logon(themeServerPropertyDefineLogonStatus);
    themeServerPropertyDefine.Resume();
    User::WaitForRequest(themeServerPropertyDefineLogonStatus);
    TESTL(KErrNone == themeServerPropertyDefineLogonStatus.Int()); 
    themeServerPropertyDefine.Close(); 

    err = iThemeServerOrientationProperty.Attach(KThemeOrientationCategory, KThemeOrientationKey, EOwnerThread);
    TESTL(KErrNone == err);
    
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    
    iPublishTimer.CreateLocal();
    }

void CTDeviceRotation::SimulateThemeServerOrientation(TRenderOrientation aOrientation)
    {
    _LIT(KFunctionInfo, "SimulateThemeServerOrientation(aOrientation = %d)");
    INFO_PRINTF2(KFunctionInfo, aOrientation);
    TInt err = iThemeServerOrientationProperty.Set(aOrientation);
    if (KErrNone != err)
        {
        _LIT(KLog,"iThemeServerOrientationProperty.Set(%d) failed with err %d");
        INFO_PRINTF3(KLog,aOrientation,err);
        TEST(EFalse);
        }
    switch(aOrientation)
        {
        case EDisplayOrientationNormal:
        case EDisplayOrientation90CW:
        case EDisplayOrientation180:
        case EDisplayOrientation270CW:
            {
            if(KErrNone == err)
                {
                iCurrentThemeServerOrientation = aOrientation;            
                }                
            break;
            }
        default:
            _LIT(KLog, "This orientation is not supported by theme server.");
        	INFO_PRINTF1(KLog);
        }
    }

void CTDeviceRotation::IsOrientationCorrect(TRenderOrientation aExpectedOrientation)
    {
    // timer to timeout when nothing is published
    iPublishTimer.After(iPublishTimerStatus, KPublishTimeout);
    
    // wait for either
    User::WaitForRequest(iRenderOrientationStatus, iPublishTimerStatus);
    
    // check that the orientation was published or not, as expected
    if(iWaitForPublishOnNextTest)
        {
        // Check that it really has published
        if(iRenderOrientationStatus.Int()==KErrNone && iPublishTimerStatus.Int()==KRequestPending)
            {
            iPublishTimer.Cancel();
            // re-subscribe
            iRenderOrientationProperty.Subscribe(iRenderOrientationStatus);            
            }
        else
            {
            _LIT(KLog,"Timed out (%d) while waiting for render orientation %d to be published");
            INFO_PRINTF3(KLog,iRenderOrientationStatus.Int(),aExpectedOrientation);        
            TEST(EFalse);
            }
        }
    else
        {
        // Check that it really hasn't published    
        if(iRenderOrientationStatus.Int()!=KRequestPending)
            {
            _LIT(KLog,"Render Orientation %d was published (%d) when not expected (timeout = %d)");
            INFO_PRINTF4(KLog,aExpectedOrientation, iRenderOrientationStatus.Int(),iPublishTimerStatus.Int());
            iPublishTimer.Cancel();
            TEST(EFalse);
            }
        else if(iPublishTimerStatus.Int()!=KErrNone)
            {
            _LIT(KLog,"Timeout failure %d");
            INFO_PRINTF2(KLog,iPublishTimerStatus.Int());           
            TEST(EFalse);        
            }
        
        // reset to default
        iWaitForPublishOnNextTest = ETrue;
        }
    
    // Retrieve the value.
    TInt orientation;
    TInt err = iRenderOrientationProperty.Get(orientation);
    if(KErrNone != err)
           {
           _LIT(KLog,"iThemeServerOrientationProperty.Get(...) failed with err %d");
           INFO_PRINTF2(KLog,err);
           TEST(EFalse);
           }
    else if(aExpectedOrientation == EDisplayOrientationAuto)
        {
        TEST(orientation == iCurrentThemeServerOrientation);
        CheckHalSetting(iCurrentThemeServerOrientation);
        }
    else
        {
        TEST(orientation == aExpectedOrientation);
        CheckHalSetting(static_cast<TRenderOrientation>(orientation));
        }
    }

void CTDeviceRotation::CheckHalSetting(TRenderOrientation aOrientation)
    {
    HALData::TDigitiserOrientation halOrientationExp = static_cast<HALData::TDigitiserOrientation>
            (HALData::EDigitiserOrientation_000 + (aOrientation - EDisplayOrientationNormal));
    TInt halOrientation;
    TInt err = HAL::Get(iWs.GetFocusScreen(), HALData::EDigitiserOrientation, halOrientation);
    if (err != KErrNotSupported)
        {
        if (err != KErrNone)
            {
            _LIT(KLog,"Getting HAL orientation attribute returned error %d when no error expected");
            INFO_PRINTF2(KLog,err);
            }
        TEST(err==KErrNone);
        if (halOrientationExp != halOrientation)
            {
            _LIT(KLog,"HAL orientation is %d when expected to be %d");
            INFO_PRINTF3(KLog, halOrientation, halOrientationExp);
            }
        TEST(halOrientationExp==halOrientation);
        }
    else
        {
        _LIT(KLog,"HAL-Orientation HALData::EDigitiserOrientation isn't supported by Driver");
        INFO_PRINTF1(KLog);
        }
    }

void CTDeviceRotation::TestIndicateAppOrientation(TRenderOrientation aOrientation)
	{
    _LIT(KTestInfo, "TestIndicateAppOrientation(aOrientation = %d)");
    INFO_PRINTF2(KTestInfo, aOrientation);
	if (EDisplayOrientationIgnore == aOrientation)
	    {
        TEST(EFalse);
        _LIT(KLog, "TestIndicateAppOrientation(TRenderOrientation aOrientation) cannot be used with EDisplayOrientationIgnore");
        INFO_PRINTF1(KLog);
	    }
	iWs.IndicateAppOrientation(aOrientation);
	iWindowGroup.SetOrdinalPosition(0);
	IsOrientationCorrect(aOrientation);
	}

void CTDeviceRotation::TestIndicateAppOrientation(TRenderOrientation aOrientation, TRenderOrientation aSecondOrientation)
    {
    _LIT(KTestInfo, "TestIndicateAppOrientation(aOrientation = %d, aSecondOrientation = %d)");
    INFO_PRINTF3(KTestInfo, aOrientation, aSecondOrientation);
    //Inform window serve the orientation status
    iWs.IndicateAppOrientation(aOrientation);
    iSecondWs.IndicateAppOrientation(aSecondOrientation);
    
    iWindowGroup.SetOrdinalPosition(0,1);
    iSecondWindowGroup.SetOrdinalPosition(1,1);

    if(aOrientation != EDisplayOrientationIgnore)
        {
        IsOrientationCorrect(aOrientation);
        }
    else
        {
        IsOrientationCorrect(aSecondOrientation);
        }
    }

void CTDeviceRotation::TestAppOrientationOnSwap(TRenderOrientation aOrientation, TRenderOrientation aSecondOrientation)
    {    
    _LIT(KTestInfo, "TestAppOrientationOnSwap(aOrientation = %d, aSecondOrientation = %d)");
    INFO_PRINTF3(KTestInfo, aOrientation, aSecondOrientation);
    
    iWs.IndicateAppOrientation(aOrientation);
    iSecondWs.IndicateAppOrientation(aSecondOrientation);
    
    iSecondWindowGroup.SetOrdinalPosition(0);
    if (aSecondOrientation != EDisplayOrientationIgnore)
        {
        IsOrientationCorrect(aSecondOrientation);
        }
    else
        {
        IsOrientationCorrect(aOrientation);
        }
    }

void CTDeviceRotation::RunTestCaseL(TInt aCurTestCase)
	{
    _LIT(KNewLine, "\n");
    (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	    case 1:
            _LIT(KTestStepID1,"Test Initial Orientations");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID1);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID1);
            TestInitialOrientation();
            break;
		case 2:
    	    _LIT(KTestStepID2,"Test Fixed Orientations");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID2);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID2);
            TestFixedOrientations();
			break;
		case 3:
    	    _LIT(KTestStepID3,"Test Auto Orientation");
		    (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID3);
		    INFO_PRINTF1(KNewLine);
		    INFO_PRINTF1(KTestStepID3);
		    TestAutoOrientation();
			break;
		case 4:
            _LIT(KTestStepID4,"Test Ignore Orientation");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID4);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID4);
            TestIgnoreOrientation();
			break;
        case 5:
            _LIT(KTestStepID5,"Test Swap Orientations");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID5);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID5);
            TestFixedOrientationsOnWindowSwap();
			break;
        case 6:
            _LIT(KTestStepID6,"Test Auto Swap Orientations");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID6);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID6);
            TestAutoOrientationOnWindowSwap();
            break;
        case 7:
            _LIT(KTestStepID7,"Test Ignore Swap Orientations");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID7);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID7);
            TestIgnoreOrientationOnWindowSwap();
			break;
        case 8:
            _LIT(KTestStepID8,"Test Auto Swap Orientations");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID8);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID8);
            TestIgnoreAutoOrientationOnWindowSwap();
            break;
        case 9:
            _LIT(KTestStepID9,"Test Invalid App Orientations");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID9);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID9);
            TestInvalidAppOrientation();
            break;
        case 10:
            _LIT(KTestStepID10,"Test Invalid Theme Server Orientations");
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KTestStepID10);
            INFO_PRINTF1(KNewLine);
            INFO_PRINTF1(KTestStepID10);
            TestInvalidThemeServerOrientation();
            break;
		default:
            (reinterpret_cast<CTDeviceRotationStep*>(iStep))->SetTestStepID(KNotATestSYMTestCaseIDName);
			(reinterpret_cast<CTDeviceRotationStep*>(iStep))->CloseTMSGraphicsStep();
			TestComplete();
		}
	(reinterpret_cast<CTDeviceRotationStep*>(iStep))->RecordTestResultL();
	}


/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0001
 @SYMTestCaseDesc           Test Auto Orientation
 @SYMPREQ                   460936 Tracking Device Rotation
 @SYMTestPriority           1
 @SYMTestPurpose            To test that we can return the correct initial orientation value from 
                            the windwoserver.
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            We have set up an RWindowGroup at the foreground and not altered its
                            indicated orientation.
 @SYMTestActions            1) Get the value of the orientation as published by window server.

 @SYMTestExpectedResults    The windowserver should return EDisplayOrientationNormal
 */
void CTDeviceRotation::TestInitialOrientation()
    {
    iWaitForPublishOnNextTest = EFalse;
    IsOrientationCorrect(EDisplayOrientationNormal);
    }

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0002
 @SYMTestCaseDesc           Test Fixed Orientations
 @SYMPREQ                   460936 Tracking Device Rotation
 @SYMTestPriority           1
 @SYMTestPurpose            To test that we can return the correct orientation value from 
                            the windwoserver after we indicate the application orientation
                            as fixed values using RWsSession::IndicateAppOrientation. Any 
                            theme server orientations should be ignored.
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            An RProperty is set up to publish Theme server Orientation changes. 
                            We have set up an RWindowGroup in the foreground.
                            Windowserver orientation currently set to EDisplayOrientationNormal.

 @SYMTestActions            1) Set Theme Server orientation to EDisplayOrientation90CW.
                            2) Set the RWindowgroup in ordinal position 0 to a fixed orientation.
                            3) Check the published orinetation value in windowserver.
                            4) Repeat steps 2) and 3) for all fixed orientations
                            3) Set Theme Server orientation to EDisplayOrientationNormal.
                            4) Repeat Steps 2-4).

 @SYMTestExpectedResults    The orientation set in step 2) should always be the same as the 
                            orientation set in step 2)
 */
void CTDeviceRotation::TestFixedOrientations()
    {
    iWaitForPublishOnNextTest = EFalse;
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientation180);
    TestIndicateAppOrientation(EDisplayOrientation270CW);
    
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientation180);
    TestIndicateAppOrientation(EDisplayOrientation270CW);
    
    SimulateThemeServerOrientation(EDisplayOrientation180);
    TestIndicateAppOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientation180);
    TestIndicateAppOrientation(EDisplayOrientation270CW);
    
    SimulateThemeServerOrientation(EDisplayOrientation270CW);
    TestIndicateAppOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientation180);
    TestIndicateAppOrientation(EDisplayOrientation270CW);    
    }

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0003
 @SYMTestCaseDesc           Test Auto Orientation
 @SYMPREQ                   460936 Tracking Device Rotation
 @SYMTestPriority           1
 @SYMTestPurpose            To test that we can return the correct orientation value from 
                            the windwoserver after we indicate the application orientation
                            as auto values using RWsSession::IndicateAppOrientation. The wserv
                            should publish the theme server orientation changes only.
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            An RProperty is set up to publish Theme server Orientation changes.
                            We have set up an RWindowGroup in the foreground.
                            WindowServer orinetation is not set to EDisplayOrientation90CW.

 @SYMTestActions            1) Set Theme Server orientation to EDisplayOrientation90CW.
                            2) Set the RWindowgroup in ordinal position 0 to auto orientation.
                            3) Check the published orinetation value in windowserver.
                            4) Set Theme Server orientation to EDisplayOrientationNormal.
                            5) Check the value of the orientation as published by window server.
                            6) Repeat 4) and 5) for EDisplayOrientation180 and 
                               EDisplayOrientation270CW
                            7) repeat 4) and 5) again for EDisplayOrientation270CW

 @SYMTestExpectedResults    We should get expected notifications where orientations have changed.
                            The orientation checked in step 3) should be the theme server 
                            orientation set in step 1).
                            The orientations checked in step 5) should be the theme server 
                            orientations set in step 4).
                            We should recieve no notification for step 7).
 */
void CTDeviceRotation::TestAutoOrientation()
    {
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientationAuto);
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    IsOrientationCorrect(iCurrentThemeServerOrientation);
    // Now flip it upside down to ensure that nothing assumes this will only be updated incrementally
    SimulateThemeServerOrientation(EDisplayOrientation180);
    IsOrientationCorrect(iCurrentThemeServerOrientation);
    SimulateThemeServerOrientation(EDisplayOrientation270CW);
    IsOrientationCorrect(iCurrentThemeServerOrientation);
    
    // And check that wserv doesn't publish if the theme server publishes the existing orientation
    SimulateThemeServerOrientation(EDisplayOrientation270CW);
    iWaitForPublishOnNextTest = EFalse;    
    IsOrientationCorrect(iCurrentThemeServerOrientation);
    }

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0004
 @SYMTestCaseDesc           Test Ignore Orientation
 @SYMPREQ                   460936 Tracking Device Rotation
 @SYMTestPriority           1
 @SYMTestPurpose            To test that we can return the correct orientation value from 
                            the windwoserver after we indicate the front applications 
                            orientation as ignore.  
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            An RProperty is set up to publish Theme server Orientation changes.
                            We have set up two RWindowGroups in the ordinal positions 0 and 1. 
                            WindowServer orinetation is not set to EDisplayOrientationNormal.

 @SYMTestActions            1) Set Theme Server orientation to EDisplayOrientation90CW.
                            2) Set the RWindowGroup in ordinal position 0 to a ignore orientation.
                            3) Set the RWindowgroup in ordinal position 1 to a fixed orientation.
                            4) Check the published orinetation value in windowserver.
                            5) Repeat steps 2-4) varying the fixed orientation set in step 2).
                            6) Set Theme Server orientation to EDisplayOrientation90CW.
                            7) Set the RWindowGroup in ordinal position 0 to a ignore orientation.
                            8) Set the RWindowgroup in ordinal position 1 to a fixed orientation.
                            9) Check the published orinetation value in windowserver.
                            10) Set Theme Server orientation to EDisplayOrientationNormal.
                            11) Check the published orinetation value in windowserver.

 @SYMTestExpectedResults    We should get expected notifications where orientations have changed.
                            All orientations checked in step 4) should be the fixed orientations 
                            of the windows in ordinal position 1, as set in step 3).
                            The orientation checked in step 9) should be the theme server 
                            orientation set in step 6).
                            The orientation checked in step 11) should be the theme server 
                            orientation set in step 10).
 */
void CTDeviceRotation::TestIgnoreOrientation()
    {
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientationIgnore, EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientationIgnore, EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientationIgnore, EDisplayOrientation180);
    TestIndicateAppOrientation(EDisplayOrientationIgnore, EDisplayOrientation270CW);
    
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientationIgnore, EDisplayOrientationAuto);
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    IsOrientationCorrect(iCurrentThemeServerOrientation);
    }

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0005
 @SYMTestCaseDesc           Test Fixed Orientations On Swap
 @SYMPREQ                   460936 Tracking Device Rotation
 @SYMTestPriority           1
 @SYMTestPurpose            To test that we get notifactation of a change in orientation and can 
                            return the correct orientation value from the windwoserver after we 
                            swap windows with varying fixed orienations.  
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            We have set up two RWindowGroups in the ordinal positions 0 and 1. 

 @SYMTestActions            1) Set the RWindowGroup in ordinal position 0 to a fixed orientation.
                            2) Set the RWindowgroup in ordinal position 1 to a fixed orientation.
                            3) Check the published orinetation value in windowserver.
                            4) Move the RWindowGroup ordinal position 1 to the ordinal position 0.
                            5) Check the published orinetation value in windowserver.
                            6) Repeat steps 1-5 but vary the values the fixed orientation of the 
                               RWindows set in steps 1) and 2)

 @SYMTestExpectedResults    We should get expected notifications where orientations have changed.
                            All orientations checked in step 3) should be the fixed orientations 
                            of the windows in ordinal position 0, as set in step 1).
                            The orientations checked in step 5) should be the fixed orientations 
                            of the windows which was in ordinal position 1 before each swap, as 
                            set in step 2).
 */
void CTDeviceRotation::TestFixedOrientationsOnWindowSwap()
    {
    iWaitForPublishOnNextTest = EFalse;
    TestIndicateAppOrientation(EDisplayOrientationNormal, EDisplayOrientationNormal);
    iWaitForPublishOnNextTest = EFalse;
    TestAppOrientationOnSwap(EDisplayOrientationNormal, EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientationNormal, EDisplayOrientation90CW);
    TestAppOrientationOnSwap(EDisplayOrientationNormal, EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientation90CW, EDisplayOrientationNormal);
    TestAppOrientationOnSwap(EDisplayOrientation90CW, EDisplayOrientationNormal);
    }

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0006
 @SYMTestCaseDesc           Test Auto Orientations On Swap
 @SYMPREQ                   460936 Tracking Device Rotation
 @SYMTestPriority           1
 @SYMTestPurpose            To test that we get notifactation of a change in orientation and can 
                            return the correct orientation value from the windwoserver after we 
                            swap windows with auto orienations.  
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            An RProperty is set up to publish Theme server Orientation changes.
                            We have set up two RWindowGroups in the ordinal positions 0 and 1.
                            WindowServer orinetation is not set to EDisplayOrientation270CW. 

 @SYMTestActions            1) Set the theme server orientation.
                            2) Set the RWindowGroup in ordinal position 0 to a fixed orientation.
                            3) Set the RWindowGroup in ordinal position 1 to auto orientation.
                            4) Check the published orinetation value in windowserver.
                            5) Move the RWindowGroup ordinal position 1 to the ordinal position 0. 
                            6) Check the published orinetation value in windowserver.
                            7) Repeat steps 1-6 but vary the values of the theme server 
                               orientation in step 1) and the fixed orientation of the frontwindow
                               in 2)
 @SYMTestExpectedResults    We should get expected notifications where orientations have changed.
                            All orientations checked in step 4) should be the fixed orientations 
                            of the front windows, which have been set in step 2).
                            All orientations checked in step 6) should be the theme server 
                            orientations set in step 1)
 */
void CTDeviceRotation::TestAutoOrientationOnWindowSwap()
    {
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientation270CW, EDisplayOrientationAuto);
    TestAppOrientationOnSwap(EDisplayOrientation270CW, EDisplayOrientationAuto);
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientationAuto, EDisplayOrientation180);
    TestAppOrientationOnSwap(EDisplayOrientationAuto, EDisplayOrientation180);
    
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientation90CW, EDisplayOrientationAuto);
    iWaitForPublishOnNextTest = EFalse;
    TestAppOrientationOnSwap(EDisplayOrientation90CW, EDisplayOrientationAuto);
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientationAuto, EDisplayOrientationNormal);
    iWaitForPublishOnNextTest = EFalse;
    TestAppOrientationOnSwap(EDisplayOrientationAuto, EDisplayOrientationNormal);
    }

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0007
 @SYMTestCaseDesc           Test Swap Orientations with Ignore Orientation
 @SYMPREQ                   460936 Tracking Device Rotation
 @SYMTestPriority           1
 @SYMTestPurpose            To test that we get notifactation of a change in orientation and can 
                            return the correct orientation value from the windwoserver after we 
                            swap windows with ignore orienations.  
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            An RProperty is set up to publish Theme server Orientation changes.
                            We have set up two RWindowGroups in the ordinal positions 0 and 1. 

 @SYMTestActions            1) Set the theme server orientation.
                            2) Set the RWindowGroup in ordinal position 0 to ignore orientation.
                            3) Set the RWindowGroup in ordinal position 1 to a fixed orientation.
                            4) Check the published orinetation value in windowserver.
                            5) Move the RWindowGroup ordinal position 1 to the ordinal position 0. 
                            6) Check the published orinetation value in windowserver.
                            7) Repeat steps 1-6 but vary the values of the theme server 
                               orientation in step 1) and the fixed orientation of the frontwindow
                               in 3)
                            8) Set the theme server orientation.
                            9) Set the RWindowGroup in ordinal position 0 to a fixedorientation.
                            10) Set the RWindowGroup in ordinal position 1 to ignore orienation.
                            11) Repeat steps 4-6) 
                            12) Repeat steps 8-11) but vary the values of the theme server 
                               orientation in step 8) and the fixed orientation of the 
                               RwindowGroup in 9)                            
                               
 @SYMTestExpectedResults    We should get expected notifications where orientations have changed.
                            All orientations checked in step 4) and 6) should be the fixed 
                            orientations RWindowGroups which have been set in step 3).
                            All orientations checked in step 11) should be the fixed orientations 
                            of the front RWindowGroups which have been set in step 9).
*/
void CTDeviceRotation::TestIgnoreOrientationOnWindowSwap()
    {
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientationIgnore, EDisplayOrientation270CW);
    TestAppOrientationOnSwap(EDisplayOrientationIgnore, EDisplayOrientation180);
    
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientationIgnore, EDisplayOrientationNormal);
    TestAppOrientationOnSwap(EDisplayOrientationIgnore, EDisplayOrientation90CW);
    
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    TestIndicateAppOrientation(EDisplayOrientation270CW, EDisplayOrientationIgnore);
    TestAppOrientationOnSwap(EDisplayOrientation180, EDisplayOrientationIgnore);    
    
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientationNormal, EDisplayOrientationIgnore);
    TestAppOrientationOnSwap(EDisplayOrientation90CW, EDisplayOrientationIgnore);
    }

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0008
 @SYMTestCaseDesc           Test Swap Orientations with Auto and Ignore Orientations
 @SYMPREQ                   460936 Tracking Device Rotation
 @SYMTestPriority           1
 @SYMTestPurpose            To test that we get notifactation of a change in orientation and can 
                            return the correct orientation value from the windwoserver after we 
                            swap windows with auto and ignore orienations.  
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            An RProperty is set up to publish Theme server Orientation changes.
                            We have set up two RWindowGroups in the ordinal positions 0 and 1. 
 @SYMTestActions            1) Set the theme server orientation to EDisplayOrientationNormal.
                            2) Set the RWindowGroup in ordinal position 0 to auto orientation.
                            3) Set the RWindowGroup in ordinal position 1 to ignore orientation.
                            4) Check the published orinetation value in windowserver.
                            5) Set the theme server orientation to EDisplayOrientation90CW.
                            6) Check the published orinetation value in windowserver.
                            7) Set the theme server orientation to EDisplayOrientationNormal.
                            8) Move the RWindowGroup in ordinal position 1 to ordinal position 0.
                            9) Check the published orinetation value in windowserver.
                            10) Set the theme server orientation to EDisplayOrientation90CW.
                            11) Check the published orinetation value in windowserver.

 @SYMTestExpectedResults    We should get expected notifications where orientations have changed.
                            The orientations checked in step 4), 6), 9) and  11) should all be 
                            the orientations set to the themeserver in steps 1), 5), 8) and 10)
                            respecvtively. 
*/
void CTDeviceRotation::TestIgnoreAutoOrientationOnWindowSwap()
    {
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    TestIndicateAppOrientation(EDisplayOrientationAuto, EDisplayOrientationIgnore);
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    IsOrientationCorrect(iCurrentThemeServerOrientation);
    
    SimulateThemeServerOrientation(EDisplayOrientationNormal);
    iSecondWindowGroup.SetOrdinalPosition(0);
    IsOrientationCorrect(iCurrentThemeServerOrientation);
    SimulateThemeServerOrientation(EDisplayOrientation90CW);
    IsOrientationCorrect(iCurrentThemeServerOrientation);
    }

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0009
 @SYMTestCaseDesc           Test Invalid App Orientation
 @SYMPREQ                   ###TrackingDeviceRotation### TODO replace me
 @SYMTestPriority           1
 @SYMTestPurpose            To test that an invalid orientation is ignored.
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            An RProperty is set up to publish Theme server Orientation changes.
                            We have set up an RWindowGroup in ordinal position 0.
 @SYMTestActions            1) Set the theme server orientation to EDisplayOrientationNormal.
                            2) Set the RWindowGroup in ordinal position 0 to 
                               EDisplayOrientation90CW.
                            4) Check the published orinetation value in windowserver.
                            5) Set the RWindowGroup in ordinal position 0 to an invalid 
                               orientation.
                            6) Check the published orinetation value in windowserver.

 @SYMTestExpectedResults    Wserv should publish an orientation change for 2) but not for 5).
                            The orientations checked in steps 4) and 6) should both be the 
                            orientation set in step 2).
*/
void CTDeviceRotation::TestInvalidAppOrientation()
	{
	SimulateThemeServerOrientation(EDisplayOrientationNormal);
	TestIndicateAppOrientation(EDisplayOrientation90CW);
	iWs.IndicateAppOrientation(static_cast<TRenderOrientation>(1000));
	iWaitForPublishOnNextTest = EFalse;
	IsOrientationCorrect(EDisplayOrientation90CW);
	}

/**
 @SYMTestCaseID             GRAPHICS-WSERV-DEVICEROTATION-0010
 @SYMTestCaseDesc           Test Invalid Theme Server Orientation
 @SYMPREQ                   ###TrackingDeviceRotation### TODO replace me
 @SYMTestPriority           1
 @SYMTestPurpose            To test that an invalid theme server orientation is ignored when
                            the app orientation has been set to auto.
 @SYMPrerequisites          An RProperty is set up to subscribe to notifications of 
                            windowserver orientation changes.
                            An RProperty is set up to publish Theme server Orientation changes.
                            We have set up an RWindowGroup in ordinal position 0.
 @SYMTestActions            1) Set the theme server orientation to EDisplayOrientationNormal.
                            2) Set the RWindowGroup in ordinal position 0 to 
                               EDisplayOrientationAuto.
                            4) Check the published orinetation value in windowserver.
                            5) Set the theme server orientation to an invalid orientation.
                            6) Check the published orinetation value in windowserver.

 @SYMTestExpectedResults    Wserv should publish an orientation change for 2) but not for 5).
                            The orientations checked in steps 4) and 6) should both be the 
                            theme server orientation set in step 1).
*/
void CTDeviceRotation::TestInvalidThemeServerOrientation()
	{
	SimulateThemeServerOrientation(EDisplayOrientationNormal);
	TestIndicateAppOrientation(EDisplayOrientationAuto);
	SimulateThemeServerOrientation(static_cast<TRenderOrientation>(5000));
	iWaitForPublishOnNextTest = EFalse;
	IsOrientationCorrect(EDisplayOrientationNormal);
	}

__CONSTRUCT_STEP__(DeviceRotation)

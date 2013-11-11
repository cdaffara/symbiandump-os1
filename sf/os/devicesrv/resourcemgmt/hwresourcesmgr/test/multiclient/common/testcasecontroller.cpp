// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testcasecontroller.h"
#include <centralrepository.h>
#include <e32debug.h>
#include <e32math.h> 
#include <hwrmvibrasdkcrkeys.h>
#include "TestScheduler.h"


const TInt KSDKAPIAllowedTargets(
		CHWRMExtendedLight::EPrimaryDisplay |
		CHWRMExtendedLight::EPrimaryKeyboard | 
		CHWRMExtendedLight::ESecondaryDisplay |
		CHWRMExtendedLight::ESecondaryKeyboard |
		CHWRMExtendedLight::ECustomTarget1 |
		CHWRMExtendedLight::ECustomTarget2 |
		CHWRMExtendedLight::ECustomTarget3 |
		CHWRMExtendedLight::ECustomTarget4 
		);
		
const TInt KExtendedAPIAllowedTargets(
		CHWRMExtendedLight::EPrimaryDisplay |
		CHWRMExtendedLight::EPrimaryKeyboard | 
		CHWRMExtendedLight::ESecondaryDisplay |
		CHWRMExtendedLight::ESecondaryKeyboard |
		CHWRMExtendedLight::ECustomTarget1 |
		CHWRMExtendedLight::ECustomTarget2 |
		CHWRMExtendedLight::ECustomTarget3 |
		CHWRMExtendedLight::ECustomTarget4 |
		CHWRMExtendedLight::ECustomTarget5 |
		CHWRMExtendedLight::ECustomTarget6 |
		CHWRMExtendedLight::ECustomTarget7 |
		CHWRMExtendedLight::ECustomTarget8 |
		CHWRMExtendedLight::ECustomTarget9 |
		CHWRMExtendedLight::ECustomTarget10 |
		CHWRMExtendedLight::ECustomTarget11 |
		CHWRMExtendedLight::ECustomTarget12 |
		CHWRMExtendedLight::ECustomTarget13 |
		CHWRMExtendedLight::ECustomTarget14 |
		CHWRMExtendedLight::ECustomTarget15 |
		CHWRMExtendedLight::ECustomTarget16 |
		CHWRMExtendedLight::ECustomTarget17 |
		CHWRMExtendedLight::ECustomTarget18 |
		CHWRMExtendedLight::ECustomTarget19 |
		CHWRMExtendedLight::ECustomTarget20 |
		CHWRMExtendedLight::ECustomTarget21 |
		CHWRMExtendedLight::ECustomTarget22 |
		CHWRMExtendedLight::ECustomTarget23 |
		CHWRMExtendedLight::ECustomTarget24 |
		CHWRMExtendedLight::ECustomTarget25 |
		CHWRMExtendedLight::ECustomTarget26 |
		CHWRMExtendedLight::ECustomTarget27 		
		);		

const TInt KEnhancedAPIAllowedTargets = KExtendedAPIAllowedTargets;

//Delay in milliseconds.
const TUint KHwrmtestsLightMoreDelay = 2000;
const TUint KHwrmtestsVibraMoreDelay = 2000;
const TInt KTestCategory = 0x101FB3E7;
const TUint32 KHWRMColorStatus  = 0x00001000;
const TUint8 KHWRMRedShift             = 16;
const TUint8 KHWRMGreenShift           = 8;


CHwrmTestCaseController* CHwrmTestCaseController::NewL(MHwrmTestCaseObserver& aObserver)
	{
	CHwrmTestCaseController* self = new (ELeave) CHwrmTestCaseController(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHwrmTestCaseController::CHwrmTestCaseController(MHwrmTestCaseObserver& aObserver) :
	iObserver(aObserver)
	,iApiType(EHWRMApiTypeNoAPI)
	{
	}

void CHwrmTestCaseController::ConstructL()
	{
	iSession = CRepository::NewL(KCRUidVibraCtrl);
	}
	
CHwrmTestCaseController::~CHwrmTestCaseController()
	{
	delete iSession;
	delete iMyLight;
	delete iVibra;
	delete iMcFramework;
	}

void CHwrmTestCaseController::SetApiType(THWRMLightTestFAPIType aApiType)
	{
	iApiType = aApiType;
	}
	
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::RunTestCases
// Run test cases
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::RunBasicTestCases(THWRMLightTestFAPIType aApiType)
	{
	SetApiType(aApiType);
	UpdateSupportedTargets();
	
	RunTestCases( EBasicTestIdFirst, EBasicTestIdLast );
	}

void CHwrmTestCaseController::RunMulticlientTestCases()
	{
	if (iMcFramework == NULL)
		{
		TRAPD(err,iMcFramework = CMcFramework::NewL());
		if (err)
			{
			_LIT(KErrorMsg,"Failed to instantiate CMcFramework. Aborting multiclient cases");
			iObserver.LogMessage(KErrorMsg);
			return;
			}
		}
		
	RunTestCases( EMulticlientTestIdFirst, EMulticlientTestIdLast );
	}
	
void CHwrmTestCaseController::UpdateSupportedTargets()
	{
    if( iApiType == EHWRMApiTypeXAPI )
    	{
    	CHWRMExtendedLight* tmpLight = CHWRMExtendedLight::NewL();
    	iSupportedTargets = tmpLight->SupportedTargets();
    	delete tmpLight;
    	}
    else if( iApiType == EHWRMApiTypeSAPI )
    	{
    	CHWRMLight* tmpLight = CHWRMLight::NewL();
    	iSupportedTargets = tmpLight->SupportedTargets();
    	delete tmpLight;
    	}	
    else if( iApiType == EHWRMApiTypeEAPI )
    	{
    	CHWRMEnhancedLight* tmpLight = CHWRMEnhancedLight::NewL();
    	iSupportedTargets = tmpLight->SupportedTargets();
    	delete tmpLight;
    	}	
	}
	
void CHwrmTestCaseController::RunTestCases(TInt aCase)
	{
	RunTestCases( aCase, aCase );
	}

void CHwrmTestCaseController::RunTestCases(TInt aFirst, TInt aLast)
	{
 	_LIT(KLogMessage1,"Running test cases from %d to %d..");
 	_LIT(KLogMessage2,"Using API: %d.");
 	
 	TBuf<256> logMessage;
 	logMessage.Format(KLogMessage1,aFirst,aLast);
 	iObserver.LogMessage(logMessage);
 	
 	logMessage.Format(KLogMessage2,iApiType);
 	iObserver.LogMessage(logMessage);
 	
	TInt i;
	
	for( i = aFirst; i <= aLast; i++ )
		{
		iErrCode = 1;

		iObserver.InfoMessage(_L("Executing test case..."));
		switch(i)
			{
			// Basic test cases start
			case EBasicTestIdFirst:
				FormatMessageL(EHwrmMessageOpDebug, _L( "Start test case number %d" ), i );
				iObserver.InfoMessage( _L("Basic tests start...") );
				break;

			/** 
			* This case runs LightTest for all targets supported by the device defined in CenRep (10200C8C.txt)
			* Targets can be removed for this test by defining a mask to targetsToBeSkipped 
			* * Nokia Core OS *
			*/
			case EAllSupportedTargetsLightTest:
				{
				//define here targets not to be tested in this case
				TInt targetsToBeSkipped(0);
				
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug,_L( "All Supported Targets Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0002 . \nAll Supported Targets Light Test::SDK Light API used." ) );
					LightTestAllSupportedSDKTargets( targetsToBeSkipped );
					
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( "All Supported Targets Test, API Type: Extended Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0002 . \nAll Supported Targets Test::Extended Light API used." ) );
					LightTestAllSupportedExtendedTargets( targetsToBeSkipped );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( "All Targets Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0002 .\nAll Targets Test, API Type not determined. Skipping test..." ) );
					}
				
				break;
				}

			/** 
			* This case runs LightTest for all possible targets
			*/
			case EAllTargetsLightTest:
				{
				
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0003 . \nAll Targets Light Test::SDK Light API used." ) );
					LightTargetTest( KSDKAPIAllowedTargets );
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Test, API Type: Extended Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0003 .\nAll Targets Light Test::Extended Light API used." ) );
					LightTargetTest( KExtendedAPIAllowedTargets );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0003 . \nAll Targets Light Test, API Type not determined. Skipping test..." ) );
					}
									
				break;
				}

			// Possibility to test just one target. Main use could be System Target testing
			// Expected result is passed to LogResult function as 3rd parameter
			case ESingleTargetLightTest1:
				{
				
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightTargetTestSDK( CHWRMLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0004 . HWRM [LightOnWithSingleTarget] System target" ), iErrCode, KErrNone );  				
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightTargetTestExtended( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0004 . HWRM [LightOnWithSingleTarget] System target " ), iErrCode, KErrNone );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( "HWRM [LightOnWithSingleTarget], API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0004 . HWRM [LightOnWithSingleTarget]t, API Type not determined. Skipping test..." ) );
					}
				break;  
				}

            // In order to pass, this case needs EPrimaryDisplay to be supported
            // Expected result is passed to LogResult function as 3rd parameter
			case ECombinedTargetsLightTest1:
				{
				iErrCode = KErrNone;
				
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					CHWRMLight* myLight = CHWRMLight::NewLC();
					TRAP( iErrCode, myLight->LightOnL( CHWRMLight::EPrimaryDisplay | CHWRMLight::ESystemTarget ) );
					if( iErrCode == KErrNone )
					    {
					    iErrCode = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOn );
					    }
					LogResult( _L( "\n @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0005 . HWRM [LightOnWithCombinedTargets] PrimaryDisplay | SystemTarget " ), iErrCode );
					LogResult( _L( "\nHWRM [LightOnWithCombinedTargets] PrimaryDisplay | SystemTarget " ), iErrCode, KErrNone );					
					CleanupStack::PopAndDestroy( myLight );
					}
				else if( iApiType == EHWRMApiTypeXAPI ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					CHWRMExtendedLight* myLight = CHWRMExtendedLight::NewLC();
					TRAP( iErrCode, myLight->LightOnL( CHWRMExtendedLight::EPrimaryDisplay | CHWRMExtendedLight::ESystemTarget ) );
					if( iErrCode == KErrNone )
					    {
					    iErrCode = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOn );
					    }
					LogResult( _L( "\n @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0005 . HWRM [LightOnWithCombinedTargets] PrimaryDisplay | SystemTarget " ), iErrCode, KErrNone );					
					CleanupStack::PopAndDestroy( myLight );
					}
				else if( iApiType == EHWRMApiTypeEAPI ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					CHWRMEnhancedLight* myLight = CHWRMEnhancedLight::NewLC();
					TRAP( iErrCode, myLight->LightOnL( CHWRMExtendedLight::EPrimaryDisplay | CHWRMExtendedLight::ESystemTarget ) );
					if( iErrCode == KErrNone )
					    {
					    iErrCode = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOn );
					    }
					LogResult( _L( "\n @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0005 . HWRM [LightOnWithCombinedTargets] PrimaryDisplay | SystemTarget " ), iErrCode, KErrNone );					
					CleanupStack::PopAndDestroy( myLight );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Combined Targets Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0005 . Combined Targets Test, API Type not determined. Skipping test..." ) );
					}  				
				break;
				}

            //In order to pass, this case needs EPrimaryDisplayAndKeyboard and ECustomTarget1 to be supported 
			// Expected result is passed to LogResult function as 3rd parameter
			case ECombinedTargetsLightTest2:
				{
				iErrCode = KErrNone;
				
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					CHWRMLight* myLight = CHWRMLight::NewLC();
					TRAP( iErrCode, myLight->LightOnL( CHWRMLight::EPrimaryDisplayAndKeyboard | CHWRMLight::ECustomTarget1 ) );
				    LogResult( _L( "\n @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0006 . HWRM [LightOnWithCombinedTargets] PrimaryDisplayAndKeyboard | ECustomTarget1  " ), iErrCode, KErrNotSupported );					
					CleanupStack::PopAndDestroy( myLight );
					}
				else if( iApiType == EHWRMApiTypeXAPI ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					CHWRMExtendedLight* myLight = CHWRMExtendedLight::NewLC();
					TRAP( iErrCode, myLight->LightOnL( CHWRMExtendedLight::EPrimaryDisplayAndKeyboard | CHWRMExtendedLight::ECustomTarget1 ) );
					LogResult( _L( "\n @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0006 . HWRM [LightOnWithCombinedTargets] PrimaryDisplayAndKeyboard | ECustomTarget1 " ), iErrCode, KErrNotSupported );					
					CleanupStack::PopAndDestroy( myLight );
					}
				else if( iApiType == EHWRMApiTypeEAPI ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					CHWRMEnhancedLight* myLight = CHWRMEnhancedLight::NewLC();
					TRAP( iErrCode, myLight->LightOnL( CHWRMExtendedLight::EPrimaryDisplayAndKeyboard | CHWRMExtendedLight::ECustomTarget1 ) );
					LogResult( _L( "\n @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0006 . HWRM [LightOnWithCombinedTargets] PrimaryDisplayAndKeyboard | ECustomTarget1 " ), iErrCode, KErrNotSupported );					
					CleanupStack::PopAndDestroy( myLight );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Combined Targets Test, API Type not determined. Skipping test..." ) );
					LogResult(_L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0006 .Combined Targets Test, API Type not determined. Skipping test..." ) );
					}  				
				break;
				}
			
			/** 
			* This case runs LightOnDurationTest for all possible targets
			*/
			case EAllTargetsLightOnDurationTest:
				{			
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light on Duration Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0007 . All Targets Light on Duration Test::SDK Light API used." ) );
					LightOnDurationTest( KSDKAPIAllowedTargets );
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light on Duration Test, API Type: Extended Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0007 .All Targets Light on Duration Test::Extended Light API used." ) );
					LightOnDurationTest( KExtendedAPIAllowedTargets );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light on Duration Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0007 . All Targets Light on Duration Test, API Type not determined. Skipping test..." ) );
					}
									
				break;
				}
			
			//possibility to test just one target. Main use could be System Target testing	
			case ESingleTargetLightOnDurationTest1:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightOnDurationTestSDK( CHWRMLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0008 . HWRM [LightOnDurationWithSingleTarget] System target" ), iErrCode );  				
					}
				else if( iApiType == EHWRMApiTypeXAPI ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightOnDurationTestExtended( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0008 . HWRM [LightOnDurationWithSingleTarget] System target " ), iErrCode );
					}
				else if( iApiType == EHWRMApiTypeEAPI ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightOnDurationTestEnhanced( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0008 . HWRM [LightOnDurationWithSingleTarget] System target " ), iErrCode );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightOnDurationWithSingleTarget, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0008 . LightOnDurationWithSingleTarget, API Type not determined. Skipping test..." ) );
					}
				break;  
				}
	
	        /** 
			* This case runs LightOffDurationTest for all possible targets
			*/
			case EAllTargetsLightOffDurationTest:
				{			
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Off Duration Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0009 . All Targets Light Off Duration Test::SDK Light API used." ) );
					LightOffDurationTest( KSDKAPIAllowedTargets );
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeXAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Off Duration Test, API Type: Extended Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0009 . All Targets Light Off Duration Test::Extended Light API used." ) );
					LightOffDurationTest( KExtendedAPIAllowedTargets );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Off Duration Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0009 . All Targets Light Off Duration Test, API Type not determined. Skipping test..." ) );
					}
									
				break;
				}

			//possibility to test just one target. Main use could be System Target testing	
			case ESingleTargetLightOffDurationTest1:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightOffDurationTestSDK( CHWRMLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0010 . HWRM [LightOffDurationWithSingleTarget] System target" ), iErrCode );  				
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightOffDurationTestExtended( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0010 . HWRM [LightOffDurationWithSingleTarget] System target " ), iErrCode );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightOffDurationWithSingleTarget, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0010 . LightOffDurationWithSingleTarget, API Type not determined. Skipping test..." ) );
					}
				break;  
				}


			/** 
			* This case runs LightIntensityTest for all possible targets
			*/
			case EAllTargetsLightIntensityTest:
				{			
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Intensity Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0011 . All Targets Light Intensity Test::SDK Light API used." ) );
					LightIntensityTest( KSDKAPIAllowedTargets );
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Intensity Test, API Type: Extended Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0011 . All Targets Light Intensity Test::Extended Light API used." ) );
					LightIntensityTest( KExtendedAPIAllowedTargets );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Intensity Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0011 . All Targets Light Intensity Test, API Type not determined. Skipping test..." ) );
					}
									
				break;
				}
				
				
			//possibility to test just one target. Main use could be System Target testing
			case ESingleTargetLightIntensityTest1:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightIntensityTestSDK( CHWRMLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0012 . HWRM [LightIntensityWithSingleTarget] System target" ), iErrCode );  				
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightIntensityTestExtended( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0012 . HWRM [LightIntensityWithSingleTarget] System target " ), iErrCode );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightIntensityWithSingleTarget, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0012 .LightIntensityWithSingleTarget, API Type not determined. Skipping test..." ) );
					}
				break;
				}

			/** 
			* This case runs LightBlinkTest for all possible targets
			*/
			case EAllTargetsLightBlinkTest:
				{			
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0013 .All Targets Light Blink Test::SDK Light API used." ) );
					LightBlinkTest( KSDKAPIAllowedTargets );
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink Test, API Type: Extended Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0013 .All Targets Light Blink Test::Extended Light API used." ) );
					LightBlinkTest( KExtendedAPIAllowedTargets );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0013 .All Targets Light Blink Test, API Type not determined. Skipping test..." ) );
					}
									
				break;
				}

			//possibility to test just one target. Main use could be System Target testing
			case ESingleTargetLightBlinkTest1:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkTestSDK( CHWRMLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0014 .HWRM [LightBlinkWithSingleTarget] System target" ), iErrCode );  				
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkTestExtended( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0014 .HWRM [LightBlinkWithSingleTarget] System target " ), iErrCode );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightBlinkWithSingleTarget, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0014 .LightBlinkWithSingleTarget, API Type not determined. Skipping test..." ) );
					}
				break;
				}

			/** 
			* This case runs LightBlinkDurationTest for all possible targets
			*/
			case EAllTargetsLightBlinkDurationTest:
				{			
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink Duration Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0015 .All Targets Light Blink Duration Test::SDK Light API used." ) );
					LightBlinkDurationTest( KSDKAPIAllowedTargets );
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink Duration Test, API Type: Extended Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0015 .All Targets Light Blink Duration Test::Extended Light API used." ) );
					LightBlinkDurationTest( KExtendedAPIAllowedTargets );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink Duration Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0015 .All Targets Light Blink Duration Test, API Type not determined. Skipping test..." ) );
					}
									
				break;
				}

			//possibility to test just one target. Main use could be System Target testing
			case ESingleTargetLightBlinkDurationTest1:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkDurationTestSDK( CHWRMLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0016 .HWRM [LightBlinkDurationWithSingleTarget] System target" ), iErrCode );  				
					}
				else if( iApiType == EHWRMApiTypeXAPI ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkDurationTestExtended( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0016 .HWRM [LightBlinkDurationWithSingleTarget] System target " ), iErrCode );
					}
				else if( iApiType == EHWRMApiTypeEAPI ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkDurationTestEnhanced( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0016 .HWRM [LightBlinkDurationWithSingleTarget] System target " ), iErrCode );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightBlinkDurationWithSingleTarget, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0016 .LightBlinkDurationWithSingleTarget, API Type not determined. Skipping test..." ) );
					}
				break;
				}

			/** 
			* This case runs LightBlinkOnOffTest for all possible targets
			*/	
			case EAllTargetsLightBlinkOnOffTest:
				{			
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink On/Off Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0017 .All Targets Light Blink On/Off Test::SDK Light API used." ) );
					LightBlinkOnOffTest( KSDKAPIAllowedTargets );
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink On/Off Testt, API Type: Extended Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0017 .All Targets Light Blink On/Off Test::Extended Light API used." ) );
					LightBlinkOnOffTest( KExtendedAPIAllowedTargets );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink On/Off Test, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0017 .All Targets Light Blink On/Off Test, API Type not determined. Skipping test..." ) );
					}
									
				break;
				}
			
			//possibility to test just one target. Main use could be System Target testing
			case ESingleTargetLightBlinkOnOffTest1:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkOnOffTestSDK( CHWRMLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0018 .HWRM [LightBlinkOnOffWithSingleTarget] System target" ), iErrCode );  				
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkOnOffTestExtended( CHWRMExtendedLight::ESystemTarget );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0018 .HWRM [LightBlinkOnOffWithSingleTarget] System target " ), iErrCode );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightBlinkOnOffWithSingleTarget, API Type not determined. Skipping test..." ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0018 .LightBlinkOnOffWithSingleTarget, API Type not determined. Skipping test..." ) );
					}
				break;
				}

			/** 
			* This case runs LightOnDurationRGBTest for all possible targets
			*/
			case EAllTargetsLightOnDurationRGBTest:
				{			
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light on Duration RGB Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0018A . All Targets Light on Duration RGB Test::SDK Light API used." ) );
					LightOnDurationTest( KSDKAPIAllowedTargets, ETrue, 0xFF, 0, 0 );
					}
				else if( iApiType == EHWRMApiTypeEAPI ) // Enhanced Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light on Duration RGB Test, API Type: Enhanced Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0018A .All Targets Light on Duration RGB Test::Enhanced Light API used." ) );
					LightOnDurationTest( KEnhancedAPIAllowedTargets, ETrue, 0, 0xff, 0 );
					}									
				break;
				}
			
			//possibility to test just one target. Main use could be System Target testing	
			case EAllTargetsLightOnDurationRGBTest1:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightOnDurationTestSDK( CHWRMLight::ESystemTarget, ETrue, 0, 0, 0xff );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-00018B . HWRM [LightOnDurationWithSingleTarget] System target" ), iErrCode );  				
					}
				else if( iApiType == EHWRMApiTypeEAPI ) // Enhanced Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightOnDurationTestEnhanced( CHWRMExtendedLight::ESystemTarget, ETrue, 0xff, 0, 0 );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-00018B . HWRM [LightOnDurationWithSingleTarget] System target " ), iErrCode );
					}
				break;  
				}
	
			/** 
			* This case runs LightBlinkDurationRGBTest for all possible targets
			*/
			case EAllTargetsLightBlinkDurationRGBTest:
				{			
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink Duration RGB Test, API Type: SDK Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0018C . All Targets Light Blink Duration RGB Test::SDK Light API used." ) );
					LightBlinkDurationTest( KSDKAPIAllowedTargets, ETrue, 0, 0xff, 0 );
					}
				else if( iApiType == EHWRMApiTypeEAPI ) // Enhanced Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " All Targets Light Blink Duration RGB Test, API Type: Enhanced Light API" ) );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0018C .All Targets Light Blink Duration RGB Test::Enhanced Light API used." ) );
					LightBlinkDurationTest( KEnhancedAPIAllowedTargets, ETrue, 0, 0, 0xff);
					}									
				break;
				}
			
			//possibility to test just one target. Main use could be System Target testing	
			case EAllTargetsLightBlinkDurationRGBTest1:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkDurationTestSDK( CHWRMLight::ESystemTarget, ETrue, 0xff, 0, 0);
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-00018D . HWRM [LightOnDurationWithSingleTarget] System target" ), iErrCode );				
					}
				else if( iApiType == EHWRMApiTypeEAPI ) // Enhanced Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightBlinkDurationTestEnhanced( CHWRMExtendedLight::ESystemTarget, ETrue, 0, 0xff, 0 );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-00018D . HWRM [LightOnDurationWithSingleTarget] System target " ), iErrCode );
					}
				break;	
				}
			
			
			/** 
			* This case runs LightTests(On/Off/Blink) for multiple hardcoded targets
			*/	
			case ELightDifferentTargetsTest:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightWithDifferentTargetsSDK( );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0019 .HWRM [LightWithDifferentTargets]" ), iErrCode, KErrNone );  				
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightWithDifferentTargetsExtended();
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0019 .HWRM [LightWithDifferentTargets]" ), iErrCode, KErrNone );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightWithDifferentTargets, API Type not determined. Skipping test..." ) );
					LogResult( _L(" @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0019 .LightWithDifferentTargets, API Type not determined. Skipping test..." ) );
					}
				break;
				}

			/** 
			* This case runs LightTests(On/Off/Blink) for multiple hardcoded targets with
			* invalid duration
			*/	
			case ELightInvalidDurationTest:
				{
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightWithInvalidDurationSDK( );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0020 .HWRM [LightWithInvalidDuration]" ), iErrCode, KErrNone );  				
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
					iErrCode = LightWithInvalidDurationExtended();
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0020 .HWRM [LightWithInvalidDuration]" ), iErrCode, KErrNone );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightWithInvalidDuration, API Type not determined. Skipping test %d..." ) ,i );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0020 .LightWithInvalidDuration, API Type not determined. Skipping test..." ) );
					}
				break;
				}
			

			case ELightIntensityTest2:
				{
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				if( iApiType == EHWRMApiTypeSAPI ) // SDK Light API
					{
					iErrCode = LightIntensitySDK( );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-SDKAPI-L-0021 .HWRM [LightWithIntensity]" ), iErrCode, KErrNone );  				
					}
				else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) ) // Extended Light API
					{
					iErrCode = LightIntensityExtended();
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-EXTAPI-L-0021 .HWRM [LightWithIntensity]" ), iErrCode, KErrNone );
					}
				else // No API selected, skip the test
					{
					FormatMessageL(EHwrmMessageOpDebug, _L( " LightWithIntensity, API Type not determined. Skipping test %d..." ), i );
					LogResult( _L( " @SYMTestCaseID:DEVSRVS-HWRMMC-NOAPI-L-0021 .LightWithIntensity, API Type not determined. Skipping test..." ) );
					}
				break;
				}


			case EBasicTestIdLast:
				iObserver.InfoMessage( _L("Basic tests end...") );
				break;
				
			// End of Basic test cases


			
			// Multiclient cases start

			case EMulticlientTestIdFirst:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iObserver.InfoMessage( _L("Multiclient tests start...") );
				iErrCode = Verify_MCFramework_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-L-0023 .HWRM [Verify_MCFramework_M] "), iErrCode);
				break;

			case EMCPrimaryDisplayLight:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_Primary_display_light_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-L-0024 .HWRM [MC_Primary_display_light_M] "), iErrCode);				
				break;

			case EMCDisplayOnKbBlink:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_Display_on_and_keyboard_blink_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-L-0025 .HWRM [MC_Display_on_and_keyboard_blink_M] "), iErrCode);				
				break;

			case EMCDisplayOnKbReserved:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_Display_on_and_keyboard_reserved_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}				
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-L-0026 .HWRM [MC_Display_on_and_keyboard_reserved_M] "), iErrCode);
				break;

			case EMCDisplayAndKbBlink:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_Display_and_keyboard_blink_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-L-0027 .HWRM [MC_Display_and_keyboard_blink_M] "), iErrCode);
				break;

			case EMCDisplayRGBColor:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_Display_RGB_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-L-0027A .HWRM [MC_Display_RGB_M1] "), iErrCode);
				break;

			case EMCVibraBasicUsage:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_Vibra_Basic_Usage_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-V-0028 .HWRM [MC_Vibra_Basic_Usage_M] "), iErrCode);
				break;															

			case EMCCombinedLightAndVibra1:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_Combined_light_and_vibra_usage_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-LV-0029 .HWRM [MC_Combined_light_and_vibra_usage_M] "), iErrCode);
				break;		

			case EMCCombinedLightAndVibra2:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_Combined_light_and_vibra_usage_2_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-LV-0030 .HWRM [MC_Combined_light_and_vibra_usage_2_M] "), iErrCode);				
				break;		
				
			case EMCFmTxBasicUsage:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iErrCode = MC_FmTx_Basic_Usage_M();
				if (iErrCode != KErrNone)
				{
					iMcFramework->StepExecuted(KErrGeneral);
				}
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-F-0031 .HWRM [MC_FmTx_Basic_Usage_M] "), iErrCode);
				break;			
			
			case EMulticlientTestIdLast:
				FormatMessageL(EHwrmMessageOpDebug, _L( " Start test case number %d" ), i );
				iMcFramework->StopMultiClientTestInSlave( );
				break;				

			// End of Multiclient cases
					
			
			// Repeat invalid parameters
			// Expected to return KErrArgument
			case ERepeatInvalidParamsTest:
				FormatMessageL(EHwrmMessageOpDebug,_L(" Start test case %d [%d]"), i, iErrCode);
				iErrCode = RunRepeatedlyInvalidValues();
				FormatMessageL(EHwrmMessageOpDebug,_L(" End test case %d [%d]"), i, iErrCode );
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-LV-0034 .HWRM [Repeated Invalid Values] "), iErrCode, KErrArgument);				
				break;
				
			// Vibra test (30 min)	
			case EMaliciousVibraTest:
				FormatMessageL(EHwrmMessageOpDebug,_L(" Start test case %d [%d]"), i, iErrCode);
				iErrCode = MaliciousVibra();
				FormatMessageL(EHwrmMessageOpDebug,_L(" End test case %d [%d]"), i, iErrCode);
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-V-0035 .HWRM [Malicious Vibra Usage] "), iErrCode);	
				break;
			
			// Wait charging notify (5 min) 	
			case EWaitForChargingNotify:
				FormatMessageL(EHwrmMessageOpDebug,_L(" Start test case %d [%d]"), i, iErrCode);
				iErrCode = ChargingNotify();
				FormatMessageL(EHwrmMessageOpDebug,_L(" End test case %d [%d]"), i, iErrCode);
				LogResult(_L(" @SYMTestCaseID:DEVSRVS-HWRMMC-V-0036 .HWRM [Wait Charging Notify] "), iErrCode);
				
				break;
												
			default:
				break;
			

			}
	
		
		CTestScheduler::StartAndStopL(2000); // Short delay after each test case
							
		//iErrCode set to 1 in the beginning. If not changed, no case has been executed
		if( !iErrCode )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" End test case %d, error code [%d]"), i, iErrCode);
			FormatMessageL(EHwrmMessageOpDebug,_L(""));
			FormatMessageL(EHwrmMessageOpDebug,_L("#############################################################"));
			FormatMessageL(EHwrmMessageOpDebug,_L(""));
		
			if(iErrCode == KErrNone)
				{
				iObserver.InfoMessage(_L("Test case PASSED"));	
				}
			else
				{
				iObserver.InfoMessage(_L("Test case FAILED"));
				}			
			}
		
		}
		
	}



// -----------------------------------------------------------------------------
// CHwrmTestCaseController::TargetSupported
// -----------------------------------------------------------------------------
//
TBool CHwrmTestCaseController::TargetSupported( TInt aTarget )
	{
    return( iSupportedTargets & aTarget);  
	}


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightTestAllSupportedTargets
// This method 
//  -runs LightTest for all targets supported by the device defined in CenRep (10200C8C.txt)
//   Targets can be removed for this test by defining a mask to targetsToBeSkipped 
//  -logs results
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightTestAllSupportedSDKTargets( TInt aTargetsToBeSkipped )
{
		
	TBuf<100> msg( KNullDesC );	

	_LIT( KMessageText, "HWRM [All supported targets test]: target 0x%x" );
		
	TInt counter(0);
	
	TInt testTargets(0);
	
	testTargets = iSupportedTargets & ( ~aTargetsToBeSkipped );
		
	//loop through supported targets mask and execute LightTargetTest if current target is supported
	while( testTargets )
		{
		
		TInt currentTarget = ( 1 << counter );
		
		if( testTargets & 1 ) 
			{							
			FormatMessageL(EHwrmMessageOpDebug, _L( " All Supported Targets Test, target 0x%x" ), currentTarget );
			iErrCode = LightTargetTestSDK( currentTarget );
			msg.Format( KMessageText,  currentTarget );
			LogResult( msg, iErrCode );
			}
		else
		 	{
		 	FormatMessageL(EHwrmMessageOpDebug, _L( " All Supported Targets Test, target 0x%x SKIPPED. Target not supported" ), (TInt)currentTarget );
		 	msg.Format( KMessageText,  currentTarget );
		 	msg.Append(_L(" - SKIPPED. Target not supported" ) );
			LogResult( msg );
		 	}
			
			
			counter++;
			
			//remove tested target from the mask 
			testTargets >>= 1;
		
		}	
}

void CHwrmTestCaseController::LightTestAllSupportedExtendedTargets( TInt aTargetsToBeSkipped )
{
		
	TBuf<100> msg( KNullDesC );	

	_LIT( KMessageText, "HWRM [All supported targets test]: target 0x%x" );
		
	TInt counter(0);
	
	TInt testTargets(0);
	
	testTargets = iSupportedTargets & ( ~aTargetsToBeSkipped );
		
	//loop through supported targets mask and execute LightTargetTest if current target is supported
	while( testTargets )
		{
		
		TInt currentTarget = ( 1 << counter );
		
		if( testTargets & 1 ) 
			{							
			FormatMessageL(EHwrmMessageOpDebug, _L( " All Supported Targets Test, target 0x%x" ), currentTarget );
			iErrCode = LightTargetTestExtended( currentTarget );
			msg.Format( KMessageText,  currentTarget );
			LogResult( msg, iErrCode );
			}
		else
		 	{
		 	FormatMessageL(EHwrmMessageOpDebug, _L( " All Supported Targets Test, target 0x%x SKIPPED. Target not supported" ), (TInt)currentTarget );
		 	msg.Format( KMessageText, currentTarget );
		 	msg.Append(_L(" - SKIPPED. Target not supported" ) );
			LogResult( msg );
		 	}
			
			
			counter++;
			
			//remove tested target from the mask 
			testTargets >>= 1;
		
		}	
}


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::TestLightOn
// This method 
// - call LightOnL method with given parameters
// - Run 100ms delay
// - Print parameters to log 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::TestLightOn(TInt aTarget, TInt aDur, TInt aIntens, TBool aFadeIn)
{
	TInt errorStatus(KErrNone);			
	TRAP(errorStatus, iMyLight->LightOnL(aTarget, aDur, aIntens, aFadeIn));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" LightONL ->error:%d"), errorStatus);
	return errorStatus;
}

TInt CHwrmTestCaseController::TestLightOn(TInt aTarget)
{
	TInt errorStatus(KErrNone);			
	TRAP(errorStatus, iMyLight->LightOnL(aTarget));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" LightONL ->error:%d"), errorStatus);
	return errorStatus;
}

TInt CHwrmTestCaseController::TestLightOnColor(TInt aTarget, TInt aDur, THWRMLightColor& aColor)
{
	TInt errorStatus(KErrNone);			
	TRAP(errorStatus, iMyLight->LightOnL(aTarget, aDur, aColor));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" LightONL ->error:%d"), errorStatus);
	return errorStatus;
}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::TestLightOff
// This method 
// - call LightOffL method with given parameters
// - Run 100ms delay
// - Print parameters to log 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::TestLightOff(TInt aTarget, TInt aDuration, TBool aFadeOut)
{
	TInt errorStatus(KErrNone);			
	TRAP(errorStatus, iMyLight->LightOffL(aTarget, aDuration, aFadeOut));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" LightOffL ->error:%d"), errorStatus);
	return errorStatus;
}

TInt CHwrmTestCaseController::TestLightOff(TInt aTarget)
{
	TInt errorStatus(KErrNone);			
	TRAP(errorStatus, iMyLight->LightOffL(aTarget));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" LightOffL ->error:%d"), errorStatus);
	return errorStatus;
}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::TestLightBlink
// This method 
// - call LightBlinkL method with given parameters
// - Run 100ms delay
// - Print parameters to log 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::TestLightBlink(TInt aTarget, 
		                         		  TInt aDuration, 
		                         		  TInt aOnDuration, 
		                         		  TInt aOffDuration, 
		                         		  TInt aIntensity)
{
	TInt errorStatus(KErrNone);			
	TRAP(errorStatus, iMyLight->LightBlinkL(aTarget, aDuration, aOnDuration, aOffDuration, aIntensity));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" LightBlinkL ->error:%d"), errorStatus);
	return errorStatus;
}

TInt CHwrmTestCaseController::TestLightBlink(TInt aTarget, 
		                         		  TInt aDuration)
{
	TInt errorStatus(KErrNone);			
	TRAP(errorStatus, iMyLight->LightBlinkL(aTarget, aDuration));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" LightBlinkL ->error:%d"), errorStatus);
	return errorStatus;
}

TInt CHwrmTestCaseController::TestLightBlink(TInt aTarget)
{
	TInt errorStatus(KErrNone);			
	TRAP(errorStatus, iMyLight->LightBlinkL(aTarget));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" LightBlinkL ->error:%d"), errorStatus);
	return errorStatus;
}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::RunTestVibra
// This method 
// - call LightBlinkL method with given parameters
// - Run 100ms delay
// - Print parameters to log 
// -----------------------------------------------------------------------------
//

TInt CHwrmTestCaseController::RunTestVibra( TInt howLong, TInt whatIntensity)
{
	TInt errorStatus(KErrNone);	
	TRAP(errorStatus, iVibra->StartVibraL(howLong, whatIntensity));
	CTestScheduler::StartAndStopL(100);
	return errorStatus;
}

TInt CHwrmTestCaseController::RunTestVibra( TInt howLong )
{
	TInt errorStatus(KErrNone);	
	TRAP(errorStatus, iVibra->StartVibraL(howLong));
	CTestScheduler::StartAndStopL(100);
	return errorStatus;
}


TInt CHwrmTestCaseController::StopTestVibra()
{
	TInt errorStatus(KErrNone);		
	TRAP(errorStatus, iVibra->StopVibraL());
	CTestScheduler::StartAndStopL(100);
	return errorStatus; 
}


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::RunTestFmTx
// This method 
// - call EnableL method with given parameters
// - Run 100ms delay
// - Print parameters to log 
// -----------------------------------------------------------------------------
//

TInt CHwrmTestCaseController::RunTestFmTx( TInt aFrequency )
{
	TInt errorStatus(KErrNone);	
	TRAP(errorStatus, iFmTx->EnableL(aFrequency));
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" EnableL ->error:%d"), errorStatus);
	return errorStatus;
}


TInt CHwrmTestCaseController::StopTestFmTx()
{
	TInt errorStatus(KErrNone);		
	TRAP(errorStatus, iFmTx->DisableL());
	CTestScheduler::StartAndStopL(100);
	FormatMessageL(EHwrmMessageOpDebug,_L(" DisableL ->error:%d"), errorStatus);
	return errorStatus; 
}

TInt CHwrmTestCaseController::Verify_RGBColorValue(THWRMLightColor& aColor)
{
    TInt colorval;

    RProperty::Get( TUid::Uid( KTestCategory ), KHWRMColorStatus, colorval );

    if(((colorval>>KHWRMRedShift)    & 0xFF) != aColor.iRed)
        {
        FormatMessageL(EHwrmMessageOpDebug,_L("Red value not correct. Expected=0x%x, Received=0x%x"), ((colorval>>KHWRMRedShift)    & 0xFF), aColor.iRed);
        return KErrArgument;
        }
    if(((colorval>>KHWRMGreenShift)  & 0xFF) != aColor.iGreen)
        {
        FormatMessageL(EHwrmMessageOpDebug,_L("Green value not correct. Expected=0x%x, Received=0x%x"), ((colorval>>KHWRMGreenShift)  & 0xFF), aColor.iGreen);
        return KErrArgument;
        }
     if((colorval & 0xFF) != aColor.iBlue)
        {
        FormatMessageL(EHwrmMessageOpDebug,_L("Blue value not correct. Expected=0x%x, Received=0x%x"), (colorval & 0xFF), aColor.iBlue);
        return KErrArgument;
        }
	return KErrNone;
}

/*******************************************************************************/
/* M U L T I C L I E N T	C A S E S			                               */
//
//
TInt CHwrmTestCaseController::MC_Display_RGB_S()
{
	TInt mcResult;
	mcResult = KErrNone;
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Display_RGB_S()"));
    iObserver.InfoMessage(_L("MC_Display_RGB_S"));
    CHWRMLight::TLightStatus currStatus;

	TRAP( mcResult, iMyLight = CHWRMLight::NewL(this) );

	if( mcResult == KErrNone )
		{
		//Reserve Primary display, Restore current state after reservation, Don't force CoeEnv "on" */
		TRAP(mcResult, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplay, ETrue, ETrue));

		if( mcResult == KErrNone )
			{
			THWRMLightColor red;
			red.iRed = 0xff;
			red.iGreen = 0;
			red.iBlue = 0;

			TRAP( mcResult, iMyLight->SetLightColorL(CHWRMLight::EPrimaryDisplay, red));

			if( mcResult == KErrNone )
				{
					mcResult = Verify_RGBColorValue(red);

					if( mcResult == KErrNone )
						{
						THWRMLightColor green;
						green.iRed = 0;
						green.iGreen = 0xff;
						green.iBlue = 0;
						mcResult = TestLightOnColor(CHWRMLight::EPrimaryDisplay, 10000, green);

						if( mcResult == KErrNone)
							{
							currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);

							if( currStatus == CHWRMLight::ELightOn )
								{								
								CTestScheduler::StartAndStopL(5000);

								mcResult = Verify_RGBColorValue(green);

								if( mcResult == KErrNone )
									{
									CTestScheduler::StartAndStopL(8000);

									mcResult = Verify_RGBColorValue(red); // base state color

									if( mcResult == KErrNone )
										{
										iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplay);
										iMcFramework->StepExecuted(KErrNone);
										mcResult = KErrNone;
										}
									}
								}
							else
								{
								FormatMessageL(EHwrmMessageOpDebug,_L(" Expected light status failed. error [%d]"), mcResult);
								FormatMessageL(EHwrmMessageOpLog, _L("Expected light status failed. error [%d]"), mcResult);
								}
							}
						}
					else
						{
						FormatMessageL(EHwrmMessageOpDebug,_L(" Verify_RGBColorValue returned error [%d]"), mcResult);
						FormatMessageL(EHwrmMessageOpLog, _L("Verify_RGBColorValue returned error [%d]"), mcResult);
						}
					
				}
			else
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" SetLightColorL returned error [%d]"), mcResult);
				FormatMessageL(EHwrmMessageOpLog, _L("SetLightColorL returned error [%d]"), mcResult);
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ReserveLightL returned error [%d]"), mcResult);
			FormatMessageL(EHwrmMessageOpLog, _L("ReserveLightL returned error [%d]"), mcResult);
			}
		}

	delete iMyLight;
	return mcResult;
	
}

TInt CHwrmTestCaseController::MC_Display_RGB_M()
	{
	TInt mcResult;
	mcResult = KErrNone;
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Display_RGB_M()"));
    iObserver.InfoMessage(_L("MC_Display_RGB_M"));
    CHWRMLight::TLightStatus currStatus;

	TRAP( mcResult, iMyLight = CHWRMLight::NewL(this) );

	if( mcResult == KErrNone )
		{
		//Reserve Primary display, Restore current state after reservation, Don't force CoeEnv "on" */
		TRAP(mcResult, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplay, ETrue, EFalse));

		if( mcResult == KErrNone )
			{
			THWRMLightColor blue;
			blue.iRed = 0;
			blue.iGreen = 0;
			blue.iBlue = 0xff;

			TRAP( mcResult, iMyLight->SetLightColorL(CHWRMLight::EPrimaryDisplay, blue));

			if( mcResult == KErrNone )
				{
					mcResult = Verify_RGBColorValue(blue);

					if( mcResult == KErrNone )
						{
						mcResult = TestLightOff(CHWRMLight::EPrimaryDisplay);

						if( mcResult == KErrNone)
							{
							currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);

							if( currStatus == CHWRMLight::ELightOff )
								{
								mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase14);

								if( mcResult == KErrNone )
									{

									mcResult = Verify_RGBColorValue(blue);

									if( mcResult == KErrNone )
										{

										iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplay);
										
										iMcFramework->StepExecuted(KErrNone);
										mcResult = KErrNone;
										}

									}
								}
							else
								{
								FormatMessageL(EHwrmMessageOpDebug,_L(" Expected light status failed. error [%d]"), mcResult);
								FormatMessageL(EHwrmMessageOpLog, _L("Expected light status failed. error [%d]"), mcResult);
								}
							}
						}
					else
						{
						FormatMessageL(EHwrmMessageOpDebug,_L(" Verify_RGBColorValue returned error [%d]"), mcResult);
						FormatMessageL(EHwrmMessageOpLog, _L("Verify_RGBColorValue returned error [%d]"), mcResult);
						}
					
				}
			else
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" SetLightColorL returned error [%d]"), mcResult);
				FormatMessageL(EHwrmMessageOpLog, _L("SetLightColorL returned error [%d]"), mcResult);
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ReserveLightL returned error [%d]"), mcResult);
			FormatMessageL(EHwrmMessageOpLog, _L("ReserveLightL returned error [%d]"), mcResult);
			}
		}

	delete iMyLight;
	return mcResult;
	
	}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Primary_display_light_M
// Light usage with two applications / This application is on front
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Primary_display_light_M()
    {
	TInt mcResult;
	mcResult = KErrNone;
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Primary_display_light_M()"));
    iObserver.InfoMessage(_L("MC_Primary_display_light_M"));
    CHWRMLight::TLightStatus currStatus;
    //CHWRMLight::TLightStatus origStatus;    
    
    TRAP(mcResult, iMyLight = CHWRMLight::NewL(this));	
	
	if(mcResult == KErrNone)
	{
		
	  //Reserve Primary display, Restore current state after reservation, Don't force CoeEnv "on" */
	  TRAP(mcResult, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplay, ETrue, EFalse));
	  
	  if (mcResult == KErrNone)
	  {
	  	//CTestScheduler::StartAndStopL(1000);
	  	
	  	//origStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  	
	  	//FormatMessageL(EHwrmMessageOpDebug,_L(" Light reserved while Status:%d"),origStatus);
	  	/********************/
	  	/* L I G H T  O F F */
	  	/********************/
	  	
	  	mcResult = TestLightOff(CHWRMLight::EPrimaryDisplay);
	  	
	  	if(mcResult == KErrNone)
	  	{
	  		/* Setting light off succeeded */
	  		
	  		currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  		if (currStatus == CHWRMLight::ELightOff)
	  		{	
	  			/*****************/
	  			/* Step executed */
	  			/*****************/
	  			
	  			mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase1);
	  			
	  			/* Peer (background application) does following:	
	  			 * --------------------------------------------------
	  			 * - Reserve Primary display with higher priority 
	  			 * - Set Primary Display light blinking for 10 seconds 
	  			 */
	  			
	  			if (mcResult == KErrNone)
	  			{
	  				// check light status
	  				currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  				
	  				if(currStatus == CHWRMLight::ELightBlink)
	  				{
	  					/******************/
	  					/* L I G H T  O N */
	  					/******************/
	  					mcResult = TestLightOn(CHWRMLight::EPrimaryDisplay, 5000, 75, ETrue);	
	  					
	  					if(mcResult == KErrNone) /* Reserved for Slave!!! */
	  					{  						
	  						// check light status
	  						currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  						
	  						if(currStatus == CHWRMLight::ELightBlink)
	  						{
	  							/*****************/
	  							/* Step executed */
	  							/*****************/
	  							iMcFramework->StepExecuted(KErrNone);
	  							mcResult = iMcFramework->WaitNextTestStep();
	  							
	  							/* SLAVE (background application) does following:
	  							 * --------------------------------------------------
	  							 * - Idle 10 seconds 
	  							 * - Release primary display light reservation
	  							 * - End test case execution
	  							 */
	  							
	  							if (mcResult == KErrNone)
	  							{
	  								/* Light reservation has returned back to this application */
	  								
	  								currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  								
	  								if(currStatus == CHWRMLight::ELightOff)
	  								{
	  									/* Light status should now be OFF because it was the original setting before peer application reserved it */

	  									/******************/
	  									/* L I G H T  O N */
	  									/******************/	  									
	  									mcResult = TestLightOn(CHWRMLight::EPrimaryDisplay, 5000, 100, ETrue);
	  									
	  									if (mcResult == KErrNone)
	  									{
	  										/* Light on succeeded */
	  										currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  										if (currStatus == CHWRMLight::ELightOn)
	  										{
	  											CTestScheduler::StartAndStopL(5000);						
	  											
	  											currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);	
	  											if(currStatus == CHWRMLight::ELightOff)
	  											{
	  												/*****************/
	  												/* Release light */
	  												/*****************/
	  												iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplay);	
	  												
  													mcResult = KErrNone;	

	  											}
	  											else
	  											{
	  												FormatMessageL(EHwrmMessageOpDebug,_L(" wrong light status %d expected ELightOff"), currStatus);
	  												//iObserver.LogMessage(_L(" Wrong light status [%d]. Expected [ELightOff]"), currStatus);
	  												FormatMessageL(EHwrmMessageOpLog, _L("Wrong light status [%d]. Expected [ELightOff]"), currStatus);
	  												mcResult = KErrGeneral;
	  											}
	  										}
	  										else
	  										{
	  											// Wrong status
	  											FormatMessageL(EHwrmMessageOpDebug,_L(" wrong light status %d expected ELightOn"), currStatus);
	  											FormatMessageL(EHwrmMessageOpLog, _L("Wrong light status [%d]. Expected [ELightOn]"), currStatus);
	  											mcResult = KErrGeneral;
	  										}
	  									}
	  									else
	  									{
	  										FormatMessageL(EHwrmMessageOpDebug,_L(" error:%d"), mcResult);
	  										FormatMessageL(EHwrmMessageOpLog, _L("TestLightOn returned error [%d]"), mcResult);
	  										mcResult = KErrGeneral;
	  									}		
	  								}
	  								else
	  								{
	  									FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong light status [%d]. Expected [ELightOff]"), currStatus);
	  									FormatMessageL(EHwrmMessageOpLog, _L("Wrong light status [%d]. Expected [ELightOff]"), currStatus);
	  									mcResult = KErrGeneral;
	  								}
	  								
	  							}
	  							else
	  							{
	  								// Error happened peer. End test, report result and clean situation
	  								// Peer should clean itself and be ready for next test case
	  								FormatMessageL(EHwrmMessageOpDebug,_L(" peer error:%d"), mcResult);
	  								FormatMessageL(EHwrmMessageOpLog, _L("error from peer [%d]"), mcResult);
	  							}
	  							
	  						}
	  						else
	  						{
	  							FormatMessageL(EHwrmMessageOpDebug,_L(" wrong light status [%d]. Expected [ELightBlink]"), currStatus);
	  							FormatMessageL(EHwrmMessageOpLog, _L("wrong light status [%d]. Expected [ELightBlink]"), currStatus);
	  							mcResult = KErrGeneral;
	  						}
	  					}
	  					else
	  					{
	  						FormatMessageL(EHwrmMessageOpDebug,_L(" TestLightOn returned wrong error:%d"), mcResult);
	  						FormatMessageL(EHwrmMessageOpLog, _L("TestLightOn returned wrong error [%d]"), mcResult);
	  						mcResult = KErrGeneral;
	  					}
	  				}
	  				else
	  				{
	  					FormatMessageL(EHwrmMessageOpDebug,_L(" wrong light status [%d]. Expected [ELightBlink]"), currStatus);
	  					FormatMessageL(EHwrmMessageOpLog, _L("wrong light status [%d]. Expected [ELightBlink]"), currStatus);
	  					mcResult = KErrGeneral;
	  				}
	  			}
	  			else
	  			{
	  				// Error happened peer. End test, report result and clean situation
	  				// Peer should clean itself and be ready for next test case
	  				FormatMessageL(EHwrmMessageOpDebug,_L(" peer returned error: [%d]"), mcResult);
	  				FormatMessageL(EHwrmMessageOpLog, _L("peer returned error: [%d]"), mcResult);				
	  			}
	  		}
	  		else
	  		{
	  			FormatMessageL(EHwrmMessageOpDebug,_L(" wrong light status [%d]. Expected [ELightOff]"), currStatus);
	  			FormatMessageL(EHwrmMessageOpLog, _L("wrong light status [%d], Expected [ELightOff]"), currStatus);
	  			mcResult = KErrGeneral;
	  		}
	  	}
	  	else
	  	{
	  		FormatMessageL(EHwrmMessageOpDebug,_L(" TestLightOff returned error [%d]"), mcResult);
	  		FormatMessageL(EHwrmMessageOpLog, _L("TestLightOff returned error [%d]"), mcResult);	
	  		mcResult = KErrGeneral;
	  	}
	  }
	  else
	  {
	  	FormatMessageL(EHwrmMessageOpDebug,_L(" ReserveLightL returned error [%d]"), mcResult);
	  	FormatMessageL(EHwrmMessageOpLog, _L("ReserveLightL returned error [%d]"), mcResult);
	  }
	  
	  // Q: Mitä tapahtuu jos light-instanssi tuhotaan vapauttamatta targettia?
	}
	
    delete iMyLight;    	
    return mcResult;
    
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Display_on_and_keyboard_blink_M()
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Display_on_and_keyboard_blink_M()
    {
	TInt mcResult;
	mcResult = KErrNone;
	      
	iObserver.InfoMessage(_L("MC_Display_on_and_keyboard_blink_M"));
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Display_on_and_keyboard_blink_M()"));

    CHWRMLight::TLightStatus keybStatus;
    CHWRMLight::TLightStatus dispStatus;    
    //CHWRMLight::TLightStatus origDispStatus;
    
    TRAP(mcResult, iMyLight = CHWRMLight::NewL(this));
    
    if(mcResult == KErrNone)
    {
        
	    /*****************/
	    /* Step executed */
	    /*****************/
	    
	    TInt mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase2);
	    
	    if (mcResult == KErrNone)
	    {
	    	/* Step executed succesfully by peer */
	    	
	    	/* Query keyboard status */
	    	keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	    	dispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	    	
	    	if (keybStatus == CHWRMLight::ELightBlink)
	    	{
		    	/* OK Keyboard status is Blink (set in peer application) */
		    	
		    	/* Reserve Primary Display light */
		    	TRAP(mcResult, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplay, ETrue, ETrue));
		    	
		    	if ( mcResult == KErrNone)
		    	{	
			    	/*******************/
			    	/* L I G H T   ON  */
			    	/*******************/
			    	mcResult = TestLightOn(CHWRMLight::EPrimaryDisplay, 5000, 1, ETrue);
			    	
			    	/* Check light status */
			    	dispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
			    	keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
			    	
			    	if (mcResult == KErrNone && dispStatus == CHWRMLight::ELightOn && keybStatus == CHWRMLight::ELightBlink)
			    		{
				    	/* Pause 5s */
				    	CTestScheduler::StartAndStopL(5000);
				    	
				    	/* Release primary display light reservation */
				    	iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplay);

						/*****************/
						/* Step executed */
						/*****************/
						iMcFramework->StepExecuted(KErrNone);    		
			    		
			    		mcResult = iMcFramework->WaitNextTestStep();
			    			    		
			    		if (mcResult != KErrNone)
			    			{			    			
			    			FormatMessageL(EHwrmMessageOpDebug,_L("peer returned error [%d]"), mcResult);
			    			FormatMessageL(EHwrmMessageOpLog, _L("Slave returned error: [%d]"), mcResult);
			    			}
			    		//*************************
			    		// else -> Case passed OK
			    		//*************************		    			
			    		}
			    	else
			    		{
			    		/* Light status was wrong or setting of light failed */
			    		FormatMessageL(EHwrmMessageOpDebug,_L(" display status:%d Keyboard status:%d, Error:%d"), dispStatus, keybStatus, mcResult);
			    		FormatMessageL(EHwrmMessageOpLog, _L("display status:%d Keyboard status:%d, Error:%d"),dispStatus, keybStatus, mcResult);
			    		mcResult = KErrGeneral;
			    		}
		    		}
		    	else
		    		{
		    		FormatMessageL(EHwrmMessageOpDebug,_L(" reservation failed. Error:%d"), mcResult);
		    		FormatMessageL(EHwrmMessageOpLog, _L("reservation failed. Error:%d"), mcResult);
		    		}	
	    	}
	    	else
	    	{
	    		// Wrong keyboard status
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" wrong keyboard status:%d"),keybStatus);
	    		FormatMessageL(EHwrmMessageOpLog, _L("wrong keyboard status:%d"),keybStatus);
	    		mcResult = KErrGeneral;
	    	}
	    }
    }
    delete iMyLight;    	
    return mcResult;
        
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::
// 
// -----------------------------------------------------------------------------
//

TInt CHwrmTestCaseController::MC_Display_on_and_keyboard_reserved_M()
    {
  	TInt mcResult;
	mcResult = KErrNone;  
    iObserver.InfoMessage(_L("MC_Display_on_and_keyboard_reserved_M"));
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Display_on_and_keyboard_reserved_M()"));
    //CHWRMLight::TLightStatus currStatus;    
    CHWRMLight::TLightStatus displayStatus;
    CHWRMLight::TLightStatus keybStatus;    
    CHWRMLight::TLightStatus originalKStatus;
    CHWRMLight::TLightStatus originalDStatus;
    
    TRAP(mcResult, iMyLight = CHWRMLight::NewL(this));	
	
	if(mcResult == KErrNone)
	{
        
	  //Reserve Primary display and keyboard
 	  TRAP(mcResult, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard, ETrue, EFalse));
	  
	  if (mcResult == KErrNone)
	  {
		
		CTestScheduler::StartAndStopL(1000);
		
    	// Valojen statusten kysely testin alussa:
    	originalDStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
    	originalKStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);	
	
	  	mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase3);
	  	
	  	if (mcResult == KErrNone)
	  	{
	  		// Tarkista että valojen statukset ei ole muuttuneet alkutilanteesta
	      	displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	      	keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);		
	  		
	  		if(displayStatus == originalDStatus && keybStatus == originalKStatus)
	  		{
	  				
	  			mcResult = TestLightOn(CHWRMLight::EPrimaryDisplayAndKeyboard);
	  			
	  			displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  			keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);						
	  			
	  			if(mcResult == KErrNone &&  displayStatus == CHWRMLight::ELightOn && keybStatus == CHWRMLight::ELightOn)
	  			{
	  				CTestScheduler::StartAndStopL(5000);
	  				
	  				mcResult = TestLightOff(CHWRMLight::EPrimaryDisplay);
	  				
	  				displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  				keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);	
	  				
	  				if (mcResult == KErrNone && displayStatus == CHWRMLight::ELightOff && keybStatus == CHWRMLight::ELightOn)
	  				{
	  					/*****************************/
	  					/* Test executed succesfully */
	  					/*****************************/
	  					mcResult = KErrNone;
	  				}
	  				else
	  				{
	  					// Wrong status or error
	  					FormatMessageL(EHwrmMessageOpDebug,_L(" light off failed. Error:%d, Display status:[%d] Keyboard status:[%d]"),mcResult, displayStatus, keybStatus);
	  					FormatMessageL(EHwrmMessageOpLog, _L("light off failed. Error:%d, Display status:[%d] Keyboard status:[%d]"), mcResult, displayStatus, keybStatus);
	  					mcResult = KErrGeneral;
	  				}			
	  			}
	  			else
	  			{
	  				FormatMessageL(EHwrmMessageOpDebug,_L(" light on failed. Error:%d, Display status:[%d] Keyboard status:[%d]"), mcResult, displayStatus, keybStatus);
	  				FormatMessageL(EHwrmMessageOpLog, _L("light on failed. Error:%d, Display status:[%d] Keyboard status:[%d]"), mcResult, displayStatus, keybStatus);	
	  				mcResult = KErrGeneral;
	  			}	
	  		}
	  		else
	  		{			
	  			FormatMessageL(EHwrmMessageOpDebug,_L(" original light status did not remain. Display:%d,%d, Keyboard:%d,%d"),displayStatus,originalDStatus, keybStatus, originalKStatus);	
	  			FormatMessageL(EHwrmMessageOpLog, _L("original light status did not remain. Display:%d,%d, Keyboard:%d,%d"),displayStatus,originalDStatus, keybStatus, originalKStatus);	
	  			mcResult = KErrGeneral;
	  		}			
	  	}
	  	else
	  	{
	  		// Testi feilasi tausta-aplikaatiossa
	  		FormatMessageL(EHwrmMessageOpDebug,_L(" execution failed peer. Error:%d"), mcResult);
	  		FormatMessageL(EHwrmMessageOpLog, _L("execution failed peer. Error:%d"), mcResult);
	  		mcResult = KErrGeneral;
	  	}
	  	
	  	iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);
	  }	
	  else
	  {
	  	FormatMessageL(EHwrmMessageOpDebug,_L(" reservation failed. Error:%d"), mcResult);
	  	FormatMessageL(EHwrmMessageOpLog, _L("reservation failed. Error:%d"), mcResult);
	  	mcResult = KErrGeneral;
	  }
    
	}
    delete iMyLight;    	
    return mcResult;
    
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Display_and_keyboard_blink_M()
// 
// -----------------------------------------------------------------------------
//

TInt CHwrmTestCaseController::MC_Display_and_keyboard_blink_M()
    {
	TInt mcResult;
	mcResult = KErrNone;
	
    iObserver.InfoMessage(_L("MC_Display_and_keyboard_blink_M"));
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Display_and_keyboard_blink_M()"));
    CHWRMLight::TLightStatus displayStatus;
    CHWRMLight::TLightStatus keybStatus;    
    //CHWRMLight::TLightStatus origDispStatus;
    //CHWRMLight::TLightStatus origKeybStatus;    
    
    TRAP(mcResult, iMyLight = CHWRMLight::NewL(this));	
	
	if(mcResult == KErrNone)
	{
    
	  //Reserve Primary display and keyboard
 	  TRAP(mcResult, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard, ETrue, EFalse));
    
	  //origDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	  //origKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);			
    
	  if (mcResult == KErrNone)
	  {
	  	/*****************/
	  	/* Step executed */
	  	/*****************/
	  	
	  	mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase4);
	  	
	  	if (mcResult == KErrNone)
	  	{
	  		/************************/
	  		/* L I G H T  B L I N K */
	  		/************************/
	  		
	  		mcResult = TestLightBlink(CHWRMLight::EPrimaryDisplayAndKeyboard, 5000, 1000, 1000, 66);
	  		
	  		displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	      	keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);			
	  		
	  		if (mcResult == KErrNone && displayStatus == CHWRMLight::ELightBlink && keybStatus == CHWRMLight::ELightBlink)
	  		{
	  			
	  			CTestScheduler::StartAndStopL(5000);
	  			
	  			mcResult = TestLightOn(CHWRMLight::EPrimaryDisplayAndKeyboard);
	  			
	  			displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	      		keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);					
    
	  			if( displayStatus == CHWRMLight::ELightOn && keybStatus == CHWRMLight::ELightOn )
	  			{
	  				/*****************/
	  				/* Step executed */
	  				/*****************/
	  				iMcFramework->StepExecuted(KErrNone);    		
	  			    		
	  			    mcResult = iMcFramework->WaitNextTestStep();
	  			    
	  			    if (mcResult != KErrNone)
	  			    {
	  			    	// Tapaus feilasi toisessa aplikaatiossa	
	  			    	FormatMessageL(EHwrmMessageOpDebug,_L(" test execution failed in peer application. Error:%d"), mcResult);
	  			    	FormatMessageL(EHwrmMessageOpLog, _L("test execution failed in peer application. Error:%d"), mcResult);
	  			    }					
	  			}
	  			else
	  			{
	  				FormatMessageL(EHwrmMessageOpDebug,_L(" wrong light status. Display:%d Keyboard:%d"), displayStatus, keybStatus);
	  				FormatMessageL(EHwrmMessageOpLog, _L("wrong light status. Display:%d Keyboard:%d"), displayStatus, keybStatus);
	  				mcResult = KErrGeneral;
	  			}			
	  		}
	  		else
	  		{
	  			FormatMessageL(EHwrmMessageOpDebug,_L(" light blink failed Display:%d Keyboard:%d Error:%d"), displayStatus, keybStatus, mcResult);
	  			FormatMessageL(EHwrmMessageOpLog, _L("light blink failed Display:%d Keyboard:%d Error:%d"), displayStatus, keybStatus, mcResult);
	  			mcResult = KErrGeneral;
	  		}			
	  	}
	  	else 
	  	{
	  		FormatMessageL(EHwrmMessageOpDebug,_L(" test failed in peer application. Error:%d"), mcResult);
	  		FormatMessageL(EHwrmMessageOpLog, _L("test failed in peer application. Error:%d"), mcResult);
	  	}
	  	
	  	iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);
	  		
	  }
	}
    delete iMyLight;    	
    return mcResult;
    }
        
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::Verify_MCFramework_M
// Master side test case to verify McFramework functionality 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::Verify_MCFramework_M()
{
	
	TInt mcResult;
	mcResult = KErrNone;
	iObserver.InfoMessage(_L("Verify_MCFramework_M"));
	FormatMessageL(EHwrmMessageOpDebug,_L("Verify_MCFramework_M() starting"));
	iObserver.InfoMessage(_L("Verify_MCFramework_M"));
	
	CTestScheduler::StartAndStopL(1000);
	
	mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase6); 
	/* <- Käynnistää testin uosritukset taustalla ja jää odottamaan vastausta (stepin tulosta) */
	
	CTestScheduler::StartAndStopL(1000); // ootellaan sekunti
	
	iMcFramework->StepExecuted(KErrNone);  // käynnistää suorituksen taustalla
	mcResult = iMcFramework->WaitNextTestStep(); // menee odottamaan stepin tulosta
	
	FormatMessageL(EHwrmMessageOpDebug,_L("Verify_MCFramework_M() done"));
	
	return mcResult;

}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Vibra_Basic_Usage_M
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Vibra_Basic_Usage_M()
{
	TInt mcResult;
	mcResult = KErrNone;
	
    iObserver.InfoMessage(_L("MC_Vibra_Basic_Usage_M"));
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Vibra_Basic_Usage_M()"));
    // CRepository* iSession;
    CHWRMVibra::TVibraStatus vibraStatus;
    
    // iSession = CRepository::NewL(KCRUidVibraCtrl);
    
    TRAP(mcResult, iVibra = CHWRMVibra::NewL());
    if(mcResult == KErrNone)
    {
    	
	    iSession->Set(KVibraCtrlProfileVibraEnabled, 1);
	    
	    /* Reserve Vibra */
	    TRAP(mcResult, iVibra->ReserveVibraL(ETrue, EFalse));
	    
	    if (mcResult == KErrNone)
	    {
	    	/*****************/
	    	/* Step executed */
	    	/*****************/

	    	mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase5);
	    	
	    	if (mcResult == KErrNone)
	    	{
	    		/******************/
	    		/* V I B R A  O N */
	    		/******************/
	    		
	    		mcResult = RunTestVibra(5000); // Run vibra 5s
	    		if (mcResult == KErrNone)
	    		{
	    			// Vibra start OK
	    			CTestScheduler::StartAndStopL(100);
	    			// Check status
	    			vibraStatus = iVibra->VibraStatus();
	    			/* Should be stopped because vibra is reserved to another application */
	    			if(vibraStatus == CHWRMVibra::EVibraStatusStopped) 
	    			{
	    				CTestScheduler::StartAndStopL(5000);
	    				
	    				// Check status
	    				vibraStatus = iVibra->VibraStatus();
						if(vibraStatus == CHWRMVibra::EVibraStatusStopped)
	    				{
	    					/*****************/
	    					/* Step executed */
	    					/*****************/
	    					iMcFramework->StepExecuted(KErrNone);
		    				mcResult = iMcFramework->WaitNextTestStep(); 
							
							if( mcResult == KErrNone)
							{
								
								CTestScheduler::StartAndStopL(500);
								
								vibraStatus = iVibra->VibraStatus();
								if(vibraStatus == CHWRMVibra::EVibraStatusOn)
			    				{
			    					/*****************/
			    					/* Step executed */
			    					/*****************/
			    					iMcFramework->StepExecuted(KErrNone);
				    				mcResult = iMcFramework->WaitNextTestStep(); 		    					
									
									if( mcResult == KErrNone)
									{
										// Test succeeded peer
										// Vibra is no longer suspended

							    		/******************/
							    		/* V I B R A  O N */
							    		/******************/
							    		mcResult = RunTestVibra(5000); // Run vibra 5s
							    		if (mcResult == KErrNone)
							    		{
							    			CTestScheduler::StartAndStopL(100);
							    			
							    			vibraStatus = iVibra->VibraStatus();
							    			
							    			if(vibraStatus != CHWRMVibra::EVibraStatusOn)
							    			{
							    				// wrong status
							    				FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong vibra status (%d), expected ON"),vibraStatus);
							    				mcResult = KErrGeneral;
							    			}
							    		}
										else
										{
											// Vibra start failed 
											FormatMessageL(EHwrmMessageOpDebug,_L(" Vibra start failed. Error(%d)"),mcResult);
										}
									}
									else
									{
										// Test failed peer
										FormatMessageL(EHwrmMessageOpDebug,_L(" Test failed peer. Reason(%d)"),mcResult);	
									}
			    					
			    				}
			    				else
			    				{
			    					// wrong status
			    					FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong vibra status (%d), expected ON "), vibraStatus);
			    					mcResult = KErrGeneral;
			    				}
							}
							else
							{
								// step failed peer
								FormatMessageL(EHwrmMessageOpDebug,_L(" Test failed @ peer. Reason(%d)"),mcResult);
							}
		    				
	    				}
	    				else
	    				{
	    					// wrong vibra status
	    					FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong vibra status (%d), expected STOPPED"),vibraStatus);
	    					mcResult = KErrGeneral;
	    				}	    			
	    			}
	    			else
	    			{
	    				// wrong status
	    				FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong vibra status (%d), expected ON"),vibraStatus);
	    				mcResult = KErrGeneral;
	    			}			
	    		}
	    		else
	    		{
	    			// Start vibra failed
	    			FormatMessageL(EHwrmMessageOpDebug,_L(" Vibra start failed. Reason(%d)"),mcResult);
	    		}
	    	}
	    	else
	    	{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" Test failed @ peer. Reason(%d)"),mcResult);   	
	    	}
	    	
	    	iVibra->ReleaseVibra();
	    	
	    }
	    else
	    {
	    	// Varaus epäonnistui
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Reservation failed"), mcResult);
	    }

    }
	    
    delete iVibra; iVibra = NULL;
    return mcResult;
    
}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Combined_light_and_vibra_usage_M
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Combined_light_and_vibra_usage_M()
{
	TInt mcResult;
	mcResult = KErrNone;	
	iObserver.InfoMessage(_L("MC_Combined_light_and_vibra_usage_M"));   
    // CRepository* iSession;
    CHWRMVibra::TVibraStatus vibraStatus;
    CHWRMLight::TLightStatus displayStatus;
    CHWRMLight::TLightStatus keybStatus;    
    
    // iSession = CRepository::NewL(KCRUidVibraCtrl);
    iSession->Set(KVibraCtrlProfileVibraEnabled, 1);
    
    TRAP(mcResult, iVibra = CHWRMVibra::NewL());    
    {
    	//
    	// OG 13/03/07
    	if (mcResult != KErrNone)
    		{
    		return mcResult;
    		}
    	//
    		
	    TRAP(mcResult, iMyLight = CHWRMLight::NewL(this));
		if(mcResult == KErrNone)
		{
	    
			//Reserve Primary display and keyboard
		 	TRAP(mcResult, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard, ETrue, EFalse));
			//origDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
			//origKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);	    
		    
		    /* Reserve Vibra */
		    TRAP(mcResult, iVibra->ReserveVibraL(ETrue, EFalse));    
		    if (mcResult == KErrNone)
		    {
		    	TRAP(mcResult, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard, ETrue, ETrue));	
		    	
		    	if (mcResult == KErrNone)
		    	{		
		    		/******************/
		    		/* V I B R A  O N */
		    		/******************/
		    		
		    		mcResult = RunTestVibra(0,75);
		    		
		    		if (mcResult == KErrNone)
		    		{	
		    			vibraStatus = iVibra->VibraStatus();
		    			
		    			if (vibraStatus == CHWRMVibra::EVibraStatusOn )
		    			{			
			    			/************************/
			    			/* L I G H T  B L I N K */
			    			/************************/
			    			mcResult = TestLightBlink(CHWRMLight::EPrimaryDisplayAndKeyboard);
			    			
			    			if(mcResult == KErrNone)
			    			{
			    				
			    				displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
			      				keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);		
			    				
			    				if( displayStatus == CHWRMLight::ELightBlink && keybStatus == CHWRMLight::ELightBlink)
			    				{
			    				
				    				CTestScheduler::StartAndStopL(2000);		
				    				/*****************/
				    				/* Step executed */
				    				/*****************/
				    				mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase7);
				    				
				    				if (mcResult == KErrNone)
				    				{
				    					
				    					if(vibraStatus == CHWRMVibra::EVibraStatusOn)
				    					{
					    					/**********************/
					    					/* S T O P  V I B R A */
					    					/**********************/
					    					mcResult = StopTestVibra();
					    					
					    					vibraStatus = iVibra->VibraStatus();				
						    				displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
			      							keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);	

					    					if(vibraStatus == CHWRMVibra::EVibraStatusStopped &&
					    					   displayStatus == CHWRMLight::ELightBlink &&
					    					   keybStatus == CHWRMLight::ELightBlink)
					    					{
						    					if(mcResult == KErrNone)
						    					{
						    						/********************/
						    						/* L I G H T  O F F */
						    						/********************/	
						    						mcResult = TestLightOff(CHWRMLight::EPrimaryDisplayAndKeyboard);
						    										    						
						    						if(mcResult == KErrNone)
						    						{
						    							displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
			      										keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);						    						
						    						
						    							if(displayStatus==CHWRMLight::ELightOff && keybStatus==CHWRMLight::ELightOff)
						    							{
						    								
						    							}
						    							else
						    							{
						    								// wrong light status
						    								FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong light status(%d)(%d) Expected(OFF)(OFF)"),displayStatus, keybStatus);
						    								mcResult = KErrGeneral;
						    							}
						    						}
						    						else
						    						{
						    							//light off epäonnistui
						    							FormatMessageL(EHwrmMessageOpDebug,_L(" Set light off failed. Error(%d)"),mcResult);
						    						}		
						    						
						    					}
						    					else
						    					{
						    						//vibra stop epäonnistui
						    						FormatMessageL(EHwrmMessageOpDebug,_L(" Vibra stop failed. Error(%d)"),mcResult);
						    					}
					    					}
					    					else
					    					{
					    						// wrong vibra or light status
					    						FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong status(%d)(%d)(%d) Expected(Stopped)(Blink)(Blink)"),vibraStatus, displayStatus, keybStatus);
					    						mcResult = KErrGeneral;
					    					}
				    					}
				    					else
				    					{
				    						// wrong vibra status
				    						FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong vibra status(%d) Expected(ON)"),vibraStatus); 	
				    						mcResult = KErrGeneral;
				    					}
				    				}
				    				else
				    				{
				    					//suoritus epäonnistui
				    					FormatMessageL(EHwrmMessageOpDebug,_L(" Test case failed peer. Error(%d)"),mcResult);
				    				} 								
			    				}
			    				else
			    				{
			    					// wrong light status
			    					FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong light status(%d)(%d) Expected(BLINK)(BLINK)"),displayStatus, keybStatus);
			    					mcResult = KErrGeneral;
			    				}
			    			}	
			    			else
			    			{
			    				//blink epäonnistui
			    				FormatMessageL(EHwrmMessageOpDebug,_L(" TestLightBlink failed. Error(%d)"),mcResult);
			    			}
		    			}
		    			else
		    			{
		    				// Väärä vibra status
		    				FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong vibra status(%d) Expected ON"),vibraStatus);
		    				mcResult = KErrGeneral;
		    			}
		    		}
		    		else
		    		{
		    			//vibra start epäonnistui
		    			FormatMessageL(EHwrmMessageOpDebug,_L(" Vibra start failed. Error(%d)"),mcResult);
		    		}
		    		
		    		iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);
		    	}
		    	else
		    	{
		    		//valon varaus epäonnistui
		    		FormatMessageL(EHwrmMessageOpDebug,_L(" Light reservation failed. Error(%d)"),mcResult);
		    	}
		    	
		    	iVibra->ReleaseVibra();
		    }
		    else
		    {
		    	//vibran varaus epäonnistui
		    	FormatMessageL(EHwrmMessageOpDebug,_L(" Vibra reservation failed. Error(%d)"),mcResult);
		    }
		}
		
    }
    
    delete iVibra; iVibra = NULL;
    delete iMyLight; iMyLight = NULL;
    return mcResult;

}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Combined_light_and_vibra_usage_2_M
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Combined_light_and_vibra_usage_2_M()
{
	TInt mcResult;
	mcResult = KErrNone;	
	iObserver.InfoMessage(_L("MC_Combined_light_and_vibra_usage_2_M"));  
    // CRepository* iSession;
    CHWRMVibra::TVibraStatus vibraStatus;
    CHWRMLight::TLightStatus displayStatus;
    CHWRMLight::TLightStatus keybStatus;    
    
    // iSession = CRepository::NewL(KCRUidVibraCtrl);
    iSession->Set(KVibraCtrlProfileVibraEnabled, 1);
    
    TRAP(mcResult, iVibra = CHWRMVibra::NewL());
    //
    // OG 13/03/07
    if (mcResult != KErrNone)
    	{
    	return mcResult;
    	}
    //
    
 
    
    TRAP(mcResult, iMyLight = CHWRMLight::NewL(this));
	if (mcResult == KErrNone)
	{
		
		/******************/
		/* V I B R A  O N */
		/******************/
		mcResult = RunTestVibra(5000,-75);	    
	    
	    if (mcResult == KErrNone)
	    {
	    	/************************/
	    	/* L I G H T  B L I N K */
	    	/************************/
	    	mcResult = TestLightBlink(CHWRMLight::EPrimaryDisplayAndKeyboard);
	    	
	    	if (mcResult == KErrNone)
	    	{
	    		vibraStatus = iVibra->VibraStatus();
	    		displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
				keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	    		
	    		if (displayStatus == CHWRMLight::ELightBlink && keybStatus == CHWRMLight::ELightBlink && vibraStatus == CHWRMVibra::EVibraStatusOn)
	    		{
	    			/*****************/
	    			/* Step executed */
	    			/*****************/
		    		mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase8);
		    		
		    		if (mcResult == KErrNone)
		    		{
			    		// Check light		
		    			displayStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
						keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
						
		    			if (displayStatus != CHWRMLight::ELightOff || keybStatus != CHWRMLight::ELightOff )
		    			{
		    				// Wrong status
		    				FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong status (%d)(%d) Expected(OFF)(OFF)"),displayStatus, keybStatus);
		    				mcResult = KErrGeneral;
		    			}	
		    		}
		    		else
		    		{
		    			// failed peer
		    			FormatMessageL(EHwrmMessageOpDebug,_L(" Failed in peer application. Error(%d)"),mcResult);
		    		}	    		
	    		}
	    		else
	    		{
	    			// wrong status
				    FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong status(%d)(%d)(%d) Expected(ON)(BLINK)(BLINK)"),vibraStatus, displayStatus, keybStatus);
				    mcResult = KErrGeneral;    			
	    		}
	    	}
	    	else
	    	{
	    		// light blink failed
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" Blink light failed. Error(%d)"),mcResult);
	    	}    		
	    }
	    else
	    {
	    	// Start vibra failed
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Start vibra failed. Error(%d)"),mcResult);
	    }
	}
    
    
    delete iVibra; iVibra = NULL;
    delete iMyLight; iMyLight = NULL;
    return mcResult;	
	
}


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_FmTx_Basic_Usage_M
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_FmTx_Basic_Usage_M()
	{
	TInt mcResult;
	mcResult = KErrNone;
	
    iObserver.InfoMessage(_L("MC_FmTx_Basic_Usage_M"));
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_FmTx_Basic_Usage_M()"));
    TFmTxState fmTxState;
    TInt fmTxFrequency;
    
    TRAP(mcResult, iFmTx = CHWRMFmTx::NewL(this));
    if(mcResult == KErrNone)
    	{    		    
    	//Reserve FmTx, Don't force CoeEnv "on" 	
	    TRAP(mcResult, iFmTx->ReserveL(EFalse)); 
	    
	    if (mcResult == KErrNone)
	    	{
	    	mcResult = StopTestFmTx();
		  	/********************/
		  	/* F M T X    O F F */
		  	/********************/
	    	
		    if (mcResult == KErrNone)
			    {		    
			    /* Disabling FmTx succeeded */
			    fmTxState = iFmTx->Status();
			    
			    if( fmTxState == EFmTxStateOff)
			    	{
			    	/*****************/
			    	/* Step executed */
			    	/*****************/
		
			    	mcResult = iMcFramework->StartTestCaseInSlave(EMultiClientCase9);
			    	
		  			/* Peer (background application) does following:	
		  			 * --------------------------------------------------
		  			 * - Reserve FmTx with higher priority 
		  			 * - Enable FmTx with Frequency 89900 KHz
		  			 */
			    	
			    	if (mcResult == KErrNone)
			    		{
			    		fmTxState = iFmTx->Status();
			    		fmTxFrequency = iFmTx->Frequency();
			    		
			    		if( (fmTxState == EFmTxStateInactive 
			    		     || fmTxState == EFmTxStateActive)
			    		    && fmTxFrequency == 89900)
			    			{			    		
				    		/******************/
				    		/* F M T X   O N  */
				    		/******************/
			    			
				    		// try to set in another frequency (88100 KHZ)
				    		mcResult = RunTestFmTx(88100); 
				    		
				    		if (mcResult == KErrInUse ) /* Reserved for Slave!!! */
				    			{
				    			// check FmTx status
				    			fmTxState = iFmTx->Status();
				    			
				    			fmTxFrequency = iFmTx->Frequency();
				    			
				    			/* Should be Active or Inactive with frequency 89900 because fmtx is reserved to another application */
				    			if (fmTxState == EFmTxStateInactive 
				    				|| fmTxState == EFmTxStateActive)				    																	
				    				{
				    				if (fmTxFrequency == 89900)
					    				{					    				
						    			// Attemt to disable the FmTx
						    			mcResult = StopTestFmTx();					    				
						    				
						    			if ( mcResult == KErrInUse ) /* Reserved for Slave!!! */
						    				{
								    		// check FmTx status
									    	fmTxState = iFmTx->Status();
									    		
									    	fmTxFrequency = iFmTx->Frequency();
						    			
									    	if( (fmTxState == EFmTxStateInactive 
									    		   || fmTxState == EFmTxStateActive)
									   			 && fmTxFrequency == 89900)
						    					{	
					  							/*****************/
					  							/* Step executed */
					  							/*****************/
		
						    					iMcFramework->StepExecuted(KErrNone);
							    				mcResult = iMcFramework->WaitNextTestStep(); 
							    					
					  							/* SLAVE (background application) does following:
					  							 * --------------------------------------------------
					  							 * - Release primary display light reservation
					  							 * - End test case execution
					  							 */							    				
												
												if( mcResult == KErrNone)
													{
													/* FmTx reservation has returned back to this application */
													
													fmTxState = iFmTx->Status();
													fmTxFrequency = iFmTx->Frequency();
													
													if( ( fmTxState == EFmTxStateInactive 
														  || fmTxState == EFmTxStateActive)
														&& fmTxFrequency == 89900)
														{

														mcResult = RunTestFmTx(88100);
														
														/***********************/
														/* F M T X (88100 KHz) */
														/***********************/				
														
														if( mcResult == KErrNone)
															{
															mcResult = StopTestFmTx();
															
															/********************/
															/* F M T X    O F F */
															/********************/															    	
															
															if( mcResult == KErrNone)
																{					
																fmTxState = iFmTx->Status();
																fmTxFrequency = iFmTx->Frequency();
																
																if( fmTxState != EFmTxStateOff 
																	|| fmTxFrequency != 88100)
													    			{													    			
													    			// wrong status
													    			FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong FmTx status (%d), expected EFmTxStateInactive OR EFmTxStateActive"), fmTxState);
													    			mcResult = KErrGeneral;													    			
													    			}
																}
															else
																{
																// FmTx disable failed
																FormatMessageL(EHwrmMessageOpDebug,_L(" FmTx disable failed. Reason(%d)"),mcResult);																
																}											
															}														
														else
															{
															// Test failed peer
															FormatMessageL(EHwrmMessageOpDebug,_L(" FmTx enable(88100 KHz) failed. Reason(%d)"),mcResult);	
															}								    					
								    					}
								    				else
								    					{
								    					// wrong status
								    					FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong FmTx status (%d) Or frequency(%d), expected EFmTxStateInactive OR EFmTxStateActive and 89900"),fmTxState, fmTxFrequency);
								    					mcResult = KErrGeneral;
								    					}
													}
												else
													{
													// step failed peer
													FormatMessageL(EHwrmMessageOpDebug,_L(" Test failed @ peer. Reason(%d)"),mcResult);
													}							    					  			
							    				}
						    				else
						    					{
						    					// wrong status
						    					FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong FmTx status (%d) Or frequency(%d), expected EFmTxStateInactive OR EFmTxStateActive and 89900"),fmTxState, fmTxFrequency);
						    					mcResult = KErrGeneral;
						    					}
						    				}
						    			else
						    				{
						    				// wrong status
						    				FormatMessageL(EHwrmMessageOpDebug,_L(" FmTx disable did not leave with KErrInUse. Reason(%d)"),mcResult);
						    				mcResult = KErrGeneral;
						    				}
					    				}			
				    			    else
				    			    	{
				    			    	FormatMessageL(EHwrmMessageOpDebug,_L(" FmTx disable failed. Reason(%d)"),mcResult);
				    			    	}
				    				}
				    			else
				    				{
				    				// wrong status
				    				FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong FmTx status (%d), expected ON"),fmTxState);
				    				mcResult = KErrGeneral;
				    				}			
				    			}
				    		else
				    			{
				    			// Start FmTx failed
				    			FormatMessageL(EHwrmMessageOpDebug,_L(" FmTx enable did not leave with KErrInUse. Reason(%d)"),mcResult);
				    			}
			    			}
			    		else
			    			{
			    			// wrong status ot frequency
			    			FormatMessageL(EHwrmMessageOpDebug,_L(" Wrong FmTx status (%d) OR frequency (%d), expected ON and 89900"),fmTxState, fmTxFrequency);
			    			mcResult = KErrGeneral;
			    			}		
			    		}			    		
			    	else
			    		{
			    		FormatMessageL(EHwrmMessageOpDebug,_L(" Test failed @ peer. Reason(%d)"),mcResult);   	
			    		}
			    	}
			    else
			    	{
			    	FormatMessageL(EHwrmMessageOpDebug,_L(" FmTx disable failed. Reason(%d)"),mcResult);
			    	}
			    }
		    else
		   		{		    			    	
		   		FormatMessageL(EHwrmMessageOpDebug,_L(" FmTx enable failed. Reason(%d)"),mcResult);   	
		   		}
			    
	    	iFmTx->Release();
	    	
	    	}
	    else
	    	{
	    	// Varaus epäonnistui
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Reservation failed"), mcResult);
	    	}

    	}
	    
    delete iFmTx; 
    iFmTx = NULL;
    return mcResult;
    
	}

/*******************************************************************************/
/* S E C U R I T Y 	/ S P E C I A L		C A S E S                              */
//
// These cases might require some special arrangements, manual testing etc...
//
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MaliciousVibra
//
// Run  vibra 5 seconds in 6 seconds periods VERY LONG time
// WARNING! This is MALICIOS vibra usage. It might not be good for phones
// health to run vibra this long.
//
// Powerstate notifications are printed to trace. Also dislay light blinks every
// time when notifications is received
//  
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MaliciousVibra()
{
	TInt mcResult;
	mcResult = KErrNone;
	
	iObserver.InfoMessage(_L("MaliciousVibra"));    
    // CRepository* iSession;
    TInt i;
    CTestCasePropertyWatch* iBatteryLevelWatch;
    CTestCasePropertyWatch* iBatteryStatusWatch;
    CTestCasePropertyWatch* iChargingStatusWatch;    
    
    iBatteryLevelWatch = CTestCasePropertyWatch::NewL(EPSBatteryLevel);
    CleanupStack::PushL( iBatteryLevelWatch );
    iBatteryStatusWatch = CTestCasePropertyWatch::NewL(EPSBatteryStatus);
    CleanupStack::PushL( iBatteryStatusWatch );
    iChargingStatusWatch = CTestCasePropertyWatch::NewL(EPSChargingStatus);
    CleanupStack::PushL( iChargingStatusWatch );
    
    // iSession = CRepository::NewL(KCRUidVibraCtrl);
    iSession->Set(KVibraCtrlProfileVibraEnabled, 1);
    
    TRAP(mcResult, iVibra = CHWRMVibra::NewL());    
    if(mcResult == KErrNone)
    {
    
	    TRAP(mcResult, iMyLight = CHWRMLight::NewL(this));
		if (mcResult == KErrNone)
		{
			for(i=0;i<300;i++)
			{	
				TRAP(mcResult, iVibra->StartVibraL(5000, 99));
                
                if (mcResult == KErrLocked)
                {
                //Wait for vibra to cool off.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Vibra Locked. Wait for more time "));
                CTestScheduler::StartAndStopL(KHwrmtestsVibraMoreDelay);
                TRAP(mcResult, iVibra->StartVibraL(5000, 99));
                }

			    if (mcResult == KErrNone)
			    {
	   				iObserver.InfoMessage(_L("*  "));
	   				CTestScheduler::StartAndStopL(2000);	    		
	    			iObserver.InfoMessage(_L(" * "));
	    			CTestScheduler::StartAndStopL(2000);
	    			iObserver.InfoMessage(_L("  *"));
	    			CTestScheduler::StartAndStopL(2000);	
			    }
                else
			    {
			    	// Start vibra failed
			    	FormatMessageL(EHwrmMessageOpDebug,_L(" Start vibra failed. Error(%d) Loop(%d)"),mcResult, i);
			    	break;
			    }
			    
			    if(iBatteryLevelWatch->iStatusReceived)
			    {
			    	FormatMessageL(EHwrmMessageOpDebug,_L(" BatteryLevel changed ->%d"),iBatteryLevelWatch->iKeyResult);
			    	iBatteryLevelWatch->Start();		
			    	mcResult = TestLightBlink(CHWRMLight::EPrimaryDisplayAndKeyboard);
			    }

			    if(iBatteryStatusWatch->iStatusReceived)
			    {
			    	FormatMessageL(EHwrmMessageOpDebug,_L(" BatteryStatus changed ->%d"),iBatteryStatusWatch->iKeyResult);
			    	iBatteryStatusWatch->Start();
			    	mcResult = TestLightBlink(CHWRMLight::EPrimaryDisplayAndKeyboard);
			    }

			    if(iChargingStatusWatch->iStatusReceived)
			    {
			    	FormatMessageL(EHwrmMessageOpDebug,_L(" ChargingStatus changed ->%d"),iChargingStatusWatch->iKeyResult);
			    	iChargingStatusWatch->Start();
			    	mcResult = TestLightBlink(CHWRMLight::EPrimaryDisplayAndKeyboard);
			    }			    			    
			}
		}		
    }
    else
    {
    	FormatMessageL(EHwrmMessageOpDebug,_L(" CHWRMVibra::NewL() Returned:%d"),mcResult);	
    }
	CleanupStack::PopAndDestroy(3);
    delete iVibra; iVibra = NULL;
    return mcResult;	
	
}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::ChargingNotify
//
// Test case will wait 5 minutes for chargingStatus notification(s)
// Test is passed if at least one notification is received
// Notification should be received when charger is connected to phone
// Number of notifications is stored in counter that is printed to trace
//  
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::ChargingNotify()
{
	TInt mcResult;
	mcResult = KErrNone;
	
    iObserver.InfoMessage(_L("ChargingNotify"));   
    TInt i,j = 0;
    CTestCasePropertyWatch* iChargingStatusWatch;    
    
    iChargingStatusWatch = CTestCasePropertyWatch::NewL(EPSChargingStatus);
    CleanupStack::PushL( iChargingStatusWatch );
    
	for(i=0;i<100;i++)
	{	
	   	iObserver.InfoMessage(_L("*  "));
	   	CTestScheduler::StartAndStopL(500);	    		
	    iObserver.InfoMessage(_L(" * "));
	    CTestScheduler::StartAndStopL(500);
	    iObserver.InfoMessage(_L("  *"));
	    CTestScheduler::StartAndStopL(500);
	    
	    if(iChargingStatusWatch->iStatusReceived)
	    {
	    	j++;
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" ChargingStatus changed ->%d"),iChargingStatusWatch->iKeyResult);
	    	iChargingStatusWatch->Start();
	    	mcResult = KErrNone;
	    	break;
	    }			    			    
	}
    
    FormatMessageL(EHwrmMessageOpDebug,_L(" ChargingStatus changed %d times"),j);
    
    delete iVibra;
    iVibra = NULL;
    CleanupStack::PopAndDestroy( iChargingStatusWatch );
    
    return mcResult;	
	
}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::RunRepeatedlyInvalidValues
// 
// Tests unsupported values in quite fast loop. Returned test result is always PASS,
// Test purpose is to crash client/server/phone.
// 
// -----------------------------------------------------------------------------

TInt CHwrmTestCaseController::RunRepeatedlyInvalidValues()
{

	TInt mcResult;
	mcResult = KErrNone;
	
    iObserver.InfoMessage(_L("RunRepeatedlyInvalidValues"));   
    // CRepository* iSession;
    TInt i,iRandIntensity, iTargets;
    TInt64 iSeed;
    
    // iSession = CRepository::NewL(KCRUidVibraCtrl);
    iSession->Set(KVibraCtrlProfileVibraEnabled, 1);
    iSeed = 100;
    
    TRAP(mcResult, iVibra = CHWRMVibra::NewL());    
    if(mcResult == KErrNone)
    {
    
	    TRAP(mcResult, iMyLight = CHWRMLight::NewL(this));
		if (mcResult == KErrNone)
		{
			
			for(i=0;i<300;i++)
			{	
				TRAP(mcResult, iVibra->StartVibraL(5000, KMaxTInt));
				
				iTargets = iMyLight->SupportedTargets();
								
		    	mcResult = TestLightBlink(iTargets, KMaxTInt);
		    	
				mcResult = TestLightOn(10, KMaxTInt, KMaxTInt, ETrue);	    	
		    	
		    	mcResult = TestLightBlink(9, KMaxTInt, KMaxTInt, KMaxTInt, KMaxTInt);
		    	
		    	mcResult = TestLightOn(0, -400, KMaxTInt, ETrue);
		    	
		    	iRandIntensity = Math::Rand(iSeed);
		    	
		    	TRAP(mcResult, iVibra->StartVibraL(5000, iRandIntensity));
		    	iObserver.InfoMessage(_L("*  "));
		    	CTestScheduler::StartAndStopL(200);	    		
			    iObserver.InfoMessage(_L(" * "));
			    CTestScheduler::StartAndStopL(200);
			    iObserver.InfoMessage(_L("  *"));
			    CTestScheduler::StartAndStopL(200);
			}
		}		
    }

    delete iVibra;
    iVibra = NULL;
    
    return mcResult;	
	
}



// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LogResult
// Write test case result to log file
// -----------------------------------------------------------------------------
//

void CHwrmTestCaseController::LogResult( const TDesC& aText )
	{
	iObserver.LogMessage(aText);
	}
	
void CHwrmTestCaseController::LogResult(const TDesC& aTestCase, TInt aErrorCode)
	{
	TBuf16<256> caseName;
	caseName.Append(aTestCase);
	
	if(aErrorCode == KErrNone)
		{				
    	FormatMessageL(EHwrmMessageOpDebug,_L(" SUCCESS : %S "), &caseName);	
	    FormatMessageL(EHwrmMessageOpLog, _L(" SUCCESS : %S "), &caseName);	
		}
	else
		{				
		FormatMessageL(EHwrmMessageOpDebug,_L(" FAIL : %S  \nRTEST: Checkpoint-fail \n"), &caseName);	
    	FormatMessageL(EHwrmMessageOpLog, _L(" FAIL : %S  \nRTEST: Checkpoint-fail \n"), &caseName);
		FormatMessageL(EHwrmMessageOpDebug,_L(" Test case returned with error [%d]"), aErrorCode);	
		FormatMessageL(EHwrmMessageOpLog, _L("Test case returned with error [%d]"), aErrorCode);		
		}
	
//	iErrCode = KErrNone;	
	}

// TODO: PASS RESULT (PASS OR FAIL) AS PARAMETER TO THE OBSERVER FOR MORE FLEXIBLE FORMATTING
void CHwrmTestCaseController::LogResult(const TDesC& aTestCase, TInt aErrorCode, TInt aExpected)
	{
	TBuf16<256> caseName;
	caseName.Append(aTestCase);
	
	if( aErrorCode == aExpected )
		{	
		FormatMessageL(EHwrmMessageOpDebug, _L( " SUCCESS : %S " ), &caseName );	
		FormatMessageL(EHwrmMessageOpLog, _L( " SUCCESS : %S : (Returned: %d, expected: %d)"), &caseName, aErrorCode, aExpected );
		}	
	else
		{
	    FormatMessageL(EHwrmMessageOpDebug, _L( " FAIL : %S  \nRTEST: Checkpoint-fail \n" ), &caseName );	
		FormatMessageL(EHwrmMessageOpLog, _L( " FAIL : %S  : (Returned: %d, expected: %d) \nRTEST: Checkpoint-fail \n" ), &caseName, aErrorCode, aExpected );		
		FormatMessageL(EHwrmMessageOpDebug, _L( " Test case returned with error [%d]" ), aErrorCode );	
		FormatMessageL(EHwrmMessageOpLog,  _L( "Test case returned unexpected error code [%d] (expected %d)" ), aErrorCode, aExpected );		
		}
	
//	iErrCode = KErrNone;
	}



// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightStatusChanged
// 
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus)
{
	
	//iObserver.LogMessage(_L(""));
	//iMyTarget = aTarget;
	iMyStatus = aStatus;
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Light state changed for target: 0x%x"), aTarget);
        
    switch ( aStatus )
        {
        case CHWRMLight::ELightOn:
            TEST_TRACE(_L(" Light state changed: ELightOn"));
           
            break;
        case CHWRMLight::ELightOff:
            TEST_TRACE(_L(" Light state changed: ELightOff"));
           
            break;
        case CHWRMLight::ELightBlink:
            TEST_TRACE(_L(" Light state changed: ELightBlink"));
           
            break;
        case CHWRMLight::ELightStatusUnknown:
            TEST_TRACE(_L(" Light state changed: ELightStatusUnknown"));
           
            break;
        default:
            TEST_TRACE(_L(" Light state changed: UNDEFINED !!!"));
           
            break;
        }
}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::VibraStatusChanged
// 
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::VibraStatusChanged( CHWRMVibra::TVibraStatus aStatus)
	{
	iVibraStatus = aStatus;
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Vibra status changed to %d"), aStatus);
        
    switch ( aStatus )
        {
        case CHWRMVibra::EVibraStatusNotAllowed:
            TEST_TRACE(_L(" Vibra status changed: EVibraStatusNotAllowed"));
           
            break;
        case CHWRMVibra::EVibraStatusStopped:
            TEST_TRACE(_L(" Vibra status changed: EVibraStatusStopped"));
           
            break;
        case CHWRMVibra::EVibraStatusOn:
            TEST_TRACE(_L(" Vibra status changed: EVibraStatusOn"));
           
            break;
        case CHWRMVibra::EVibraStatusUnknown:
            TEST_TRACE(_L(" Vibra status changed: EVibraStatusUnknown"));
           
            break;
        default:
            TEST_TRACE(_L(" Vibra status changed: UNDEFINED !!!"));
           
            break;
        }
	}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::VibraModeChanged
// 
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::VibraModeChanged( CHWRMVibra::TVibraModeState aStatus)
	{
	iVibraMode = aStatus;
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Vibra mode changed to %d"), aStatus);
        
    switch ( aStatus )
        {
        case CHWRMVibra::EVibraModeUnknown:
            TEST_TRACE(_L(" Vibra mode changed: EVibraModeUnknown"));
           
            break;
        case CHWRMVibra::EVibraModeON:
            TEST_TRACE(_L(" Vibra mode changed: EVibraModeON"));
           
            break;
        case CHWRMVibra::EVibraModeOFF:
            TEST_TRACE(_L(" Vibra mode changed: EVibraModeOFF"));
           
            break;
        default:
            TEST_TRACE(_L(" Vibra mode changed: UNDEFINED !!!"));
           
            break;
        }
	}  

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::FmTxStatusChanged
// 
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::FmTxStatusChanged(TFmTxState aStatus)
	{
	FormatMessageL(EHwrmMessageOpDebug,_L("FmTx State Changed to %d "), aStatus);
	iFmTxState = aStatus;
	
	switch (aStatus)
        {
    case EFmTxStateUnknown:
    	TEST_TRACE(_L("FmTx state changed: EFmTxStateUnknown"));
        break;
    case EFmTxStateOff:
    	TEST_TRACE(_L("FmTx state changed: EFmTxStateOff"));
        break;
    case EFmTxStateActive:
    	TEST_TRACE(_L("FmTx state changed: EFmTxStateActive"));
        break;
    case EFmTxStateInactive:
    	TEST_TRACE(_L("FmTx state changed: EFmTxStateInactive"));
        break;
    case EFmTxStatePowerSaveInactivity:
    	TEST_TRACE(_L("FmTx state changed: EFmTxStatePowerSaveInactivity"));
    	break;
    case EFmTxStatePowerSaveAccessory:
    	TEST_TRACE(_L("FmTx state changed: EFmTxStatePowerSaveAccessory"));
		break;
    case EFmTxStateScanning:
    	TEST_TRACE(_L("FmTx state changed: EFmTxStateScanning"));
		break;
    default:
    	TEST_TRACE(_L("FmTx state changed: UNDEFINED !"));
        break;
        }
	}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::FmTxFrequencyChanged
// 
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::FmTxFrequencyChanged(TInt aFrequency)
 	{
 	FormatMessageL(EHwrmMessageOpDebug,_L("FmTx Frequency Changed. New Frequency = %d "), aFrequency);
 	}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::GetStatusForSystemTarget
// Check light status for SystemTarget
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::GetLightStatusForSystemTarget( CHWRMLight* aLight, CHWRMLight::TLightStatus aStatus )
	{
	
	TInt mask(0);
	
	mask = aLight->SupportedTargets();

	
	if( ( mask & CHWRMLight::EPrimaryDisplay ) && 
		( aLight->LightStatus( CHWRMLight::EPrimaryDisplay ) != aStatus ) )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" GetLightStatusForSystemTarget::EPrimaryDisplay. Status doesn't match"));
		return KErrGeneral;
		}	
	
	if( ( mask & CHWRMLight::EPrimaryKeyboard ) && 
		( aLight->LightStatus( CHWRMLight::EPrimaryKeyboard ) != aStatus  ) )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" GetLightStatusForSystemTarget::EPrimaryKeyboard. Status doesn't match"));
		return KErrGeneral;
		}
	
	if( ( mask & CHWRMLight::ESecondaryDisplay ) && 
		( aLight->LightStatus( CHWRMLight::ESecondaryDisplay ) != aStatus ) )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" GetLightStatusForSystemTarget::ESecondaryDisplay. Status doesn't match"));
		return KErrGeneral;
		}
	
	if( ( mask & CHWRMLight::ESecondaryKeyboard ) && 
		( aLight->LightStatus( CHWRMLight::ESecondaryKeyboard ) != aStatus ) )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" GetLightStatusForSystemTarget::ESecondaryKeyboard. Status doesn't match"));
		return KErrGeneral;
		}

	return KErrNone;
	
	}

TInt CHwrmTestCaseController::GetLightStatusForSystemTarget( CHWRMExtendedLight* aLight, CHWRMExtendedLight::TLightStatus aStatus )
	{
	
	TInt mask(0);
	
	mask = aLight->SupportedTargets();

	
	if( ( mask & CHWRMExtendedLight::EPrimaryDisplay ) && 
		( aLight->LightStatus( CHWRMExtendedLight::EPrimaryDisplay ) != aStatus ) )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" GetLightStatusForSystemTarget::EPrimaryDisplay. Status doesn't match"));
		return KErrGeneral;
		}	
	
	if( ( mask & CHWRMExtendedLight::EPrimaryKeyboard ) && 
		( aLight->LightStatus( CHWRMExtendedLight::EPrimaryKeyboard ) != aStatus  ) )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" GetLightStatusForSystemTarget::EPrimaryKeyboard. Status doesn't match"));
		return KErrGeneral;
		}
	
	if( ( mask & CHWRMExtendedLight::ESecondaryDisplay ) && 
		( aLight->LightStatus( CHWRMExtendedLight::ESecondaryDisplay ) != aStatus ) )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" GetLightStatusForSystemTarget::ESecondaryDisplay. Status doesn't match"));
		return KErrGeneral;
		}
	
	if( ( mask & CHWRMExtendedLight::ESecondaryKeyboard ) && 
		( aLight->LightStatus( CHWRMExtendedLight::ESecondaryKeyboard ) != aStatus ) )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" GetLightStatusForSystemTarget::ESecondaryKeyboard. Status doesn't match"));
		return KErrGeneral;
		}

	return KErrNone;
	
	}


	
//------------------------------ TEST CASES-------------------------------------

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::SDKLightTargetTest
// Call Light Target Test for multiple targets depending on selected API
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightTargetTest( TInt aTargetMask )
	{
	TInt currentTarget(1);
	TInt err( KErrNone );
	TInt expectedResult( KErrNone );
	
	TBuf<100> msg( KNullDesC );
	
	_LIT( KSupportedMessageText, "HWRM [Light target test]: Target 0x%x supported"  );	
	_LIT( KNotSupportedMessageText, "HWRM [Light target test]: Target 0x%x not supported"  );		

	//loop through all possible targets
	for( TInt i = 0; i < 31; i++ )
		{
		//if current target is not included in the given mask, or not defined in CenRep, KErrNot Supported is expected
		if( ( aTargetMask & currentTarget ) & iSupportedTargets )
			{
			expectedResult = KErrNone;
			msg.Format( KSupportedMessageText, currentTarget );
			}
		else
			{
			expectedResult = KErrNotSupported;
			msg.Format( KNotSupportedMessageText, currentTarget );
			}
		
		if( iApiType == EHWRMApiTypeSAPI  )
			{
			err = LightTargetTestSDK( currentTarget );
			}
		else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) )
			{
			err = LightTargetTestExtended( currentTarget );
			}
		
		LogResult( msg, err, expectedResult );

		// shift the target ready for next loop
        currentTarget <<= 1;
		}
	}

	
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightTargetTest
// Set primary display on and off
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightTargetTestSDK( TInt aTarget)
    {
    iObserver.InfoMessage(_L("LightTargetTest"));
    FormatMessageL(EHwrmMessageOpDebug, _L( " Test case: HWRM SDK [LightOnWithTarget] 0x%x" ), aTarget );
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	TRAP(errVal, myLight->LightOffL( aTarget ) );	
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}

 	//set light on for default intensity and infinite duration
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on"));

	TRAP(errVal, myLight->LightOnL( aTarget ) );
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}

	/*Light status can be checked for one target only. If System Target is included in current target, 
	  GetLightStatusForSystemTarget is called to check light status */
	if( aTarget & CHWRMLight::ESystemTarget )
		{
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOn );
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for default intensity and infinite duration"));
	    	TRAP(errVal, myLight->LightOffL( aTarget ));
	    	CTestScheduler::StartAndStopL(100);
	    	if ( errVal != KErrNone ) 
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
	    		}
	    	}
	    else
	    	{
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not ok"));
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Target light status: 0x%x"), iMyStatus);
	    	errVal = KErrGeneral;
	    	}
		}
	else
		{
		if((myLight->LightStatus( aTarget ) == CHWRMLight::ELightOn))
	    	{
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for default intensity and infinite duration"));
	    	TRAP(errVal, myLight->LightOffL( aTarget ));
	    	CTestScheduler::StartAndStopL(100);
	    	
	    	if (errVal != KErrNone) 
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
	    		}
	    	
	    	}
	    else
	    	{
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not ok"));
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Target light status: 0x%x"), iMyStatus);
	    	errVal = KErrGeneral;
	    	}
		
		}
	
	//delete light instance	
    CleanupStack::PopAndDestroy( myLight );  
    
    return errVal;
    
    }


TInt CHwrmTestCaseController::LightTargetTestExtended( TInt aTarget)
    {
    iObserver.InfoMessage(_L("LightTargetTest"));
    FormatMessageL(EHwrmMessageOpDebug, _L( " Test case: HWRM Extended [LightOnWithTarget] 0x%x" ), aTarget );
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMExtendedLight* myLight = NULL;

	if ( iApiType == EHWRMApiTypeEAPI )
    	{
		myLight = CHWRMEnhancedLight::NewLC();
    	}
	else
		{
    	myLight = CHWRMExtendedLight::NewLC();
		}
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	TRAP(errVal, myLight->LightOffL( aTarget ) );	
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}

 	//set light on for default intensity and infinite duration
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on"));

	TRAP(errVal, myLight->LightOnL( aTarget ) );
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}

	/*Light status can be checked for one target only. If System Target is included in current target, 
	  GetLightStatusForSystemTarget is called to check light status */
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOn );
		
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for default intensity and infinite duration"));
	    	TRAP(errVal, myLight->LightOffL( aTarget ));
	    	CTestScheduler::StartAndStopL(100);
	    	if ( errVal != KErrNone ) 
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
	    		}
	    	}
	    else
	    	{
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not ok"));
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Target light status: 0x%x"), iMyStatus);
	    	errVal = KErrGeneral;
	    	}
		}
	else
		{
		if((myLight->LightStatus( aTarget ) == CHWRMExtendedLight::ELightOn))
	    	{
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for default intensity and infinite duration"));
	    	TRAP(errVal, myLight->LightOffL( aTarget ));
	    	CTestScheduler::StartAndStopL(100);
	    	
	    	if (errVal != KErrNone) 
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
	    		}
	    	
	    	}
	    else
	    	{
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not ok"));
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Target light status: 0x%x"), iMyStatus);
	    	errVal = KErrGeneral;
	    	}
		
		}
	
	//delete light instance	
    CleanupStack::PopAndDestroy( myLight );
    
    return errVal;
    
    }

	
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightOnDurationTest
// Call Light Target Test for multiple targets depending on selected API
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightOnDurationTest( TInt aTargetMask, TBool aIsRGBTest, TInt aRed, TInt aGreen, TInt aBlue )
	{
	TInt currentTarget(1);
	TInt err( KErrNone );
	TInt expectedResult( KErrNone );
	
	TBuf<100> msg( KNullDesC );
	
	_LIT( KSupportedMessageText, "HWRM [Light duration test]: Target 0x%x supported"  );	
	_LIT( KNotSupportedMessageText, "HWRM [Light duration test]: Target 0x%x not supported"  );		

	//loop through all possible targets
	for( TInt i = 0; i < 31; i++ )
		{
		//if current target is not included in the given mask, or not defined in CenRep, KErrNot Supported is expected
		if( ( aTargetMask & currentTarget ) & iSupportedTargets )
			{
			expectedResult = KErrNone;
			msg.Format( KSupportedMessageText, currentTarget );
			}
		else
			{
			expectedResult = KErrNotSupported;
			msg.Format( KNotSupportedMessageText, currentTarget );
			}
		
		
		if( iApiType == EHWRMApiTypeSAPI  )
			{
			err = LightOnDurationTestSDK( currentTarget, aIsRGBTest, aRed, aGreen, aBlue );
			}
		else if( iApiType == EHWRMApiTypeXAPI )
			{
			err = LightOnDurationTestExtended( currentTarget );
			}
		else if( iApiType == EHWRMApiTypeEAPI )
			{
			err = LightOnDurationTestEnhanced( currentTarget, aIsRGBTest, aRed, aGreen, aBlue );
			}
			
			
		LogResult( msg, err, expectedResult );

		// shift the target ready for next loop
        currentTarget <<= 1;
		}
	}
	
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightOnDurationTestSDK
// Set target lights on for specified duration.
// -----------------------------------------------------------------------------
//

TInt CHwrmTestCaseController::LightOnDurationTestSDK( TInt aTarget, TBool aIsRGBTest, TInt aRed, TInt aGreen, TInt aBlue )
    {
    iObserver.InfoMessage(_L("LightOnDurationTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM SDK [LightOnWithDuration], 0x%x"), aTarget);
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target lights off"));
	//if light are on, set off for infinite duration
	TRAP(errVal, myLight->LightOffL( aTarget ));
	CTestScheduler::StartAndStopL(100);	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}	

	if( aIsRGBTest )
		{
		TRAP(errVal, myLight->SetLightDefaultColorL( aTarget ));
		if(errVal != KErrNone)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->SetLightColorL() failed [%d]"), errVal);
			CleanupStack::PopAndDestroy( myLight );
			return errVal;
			}	
		}

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target lights on"));
	
	//set light on for default intensity and 3 seconds
	if ( aIsRGBTest )
		{
		THWRMLightColor aRGBValue;
		aRGBValue.iRed = aRed;
		aRGBValue.iGreen = aGreen;
		aRGBValue.iBlue = aBlue;
		TRAP(errVal, myLight->LightOnL( aTarget, 3000, aRGBValue));
		}
	else
		{
		TRAP(errVal, myLight->LightOnL( aTarget, 3000));
		}
	CTestScheduler::StartAndStopL(100);

	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
		
	
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Check target light status"));
	/*Light status can be checked for one target only. If System Target is included in current target, 
	  GetLightStatusForSystemTarget is called to check light status */
	if( aTarget & CHWRMLight::ESystemTarget )
		{
		//check that display lights are on
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOn );
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOn"));
			//wait for light on duration to expire
			CTestScheduler::StartAndStopL(3100);
			FormatMessageL(EHwrmMessageOpDebug,_L(" Check display light status"));
			//lights should be off
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOff );
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOff );
                }
			if( errVal != KErrNone )
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			}
		}
	else
		{
		//check that display lights are on
		if (myLight->LightStatus( aTarget ) == CHWRMLight::ELightOn)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOn"));
			//wait for light on duration to expire
			CTestScheduler::StartAndStopL(3100);
			
			FormatMessageL(EHwrmMessageOpDebug,_L(" Check display light status"));
			//lights should be off
			if (myLight->LightStatus( aTarget ) != CHWRMLight::ELightOff)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;					
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			}
		}
	//delete light instance
    CleanupStack::PopAndDestroy( myLight );	
    
    return errVal;
        
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightOnDurationTestExtended
// Set target lights on for specified duration.
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightOnDurationTestExtended( TInt aTarget )
    {
    iObserver.InfoMessage(_L("LightOnDurationTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM Extended [LightOnWithDuration], 0x%x"), aTarget);
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMExtendedLight* myLight = CHWRMExtendedLight::NewLC();
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target lights off"));
	//if light are on, set off for infinite duration
	TRAP(errVal, myLight->LightOffL( aTarget ));
	CTestScheduler::StartAndStopL(100);	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}	

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target lights on"));
	
	//set light on for default intensity and 3 seconds
	TRAP(errVal, myLight->LightOnL( aTarget, 3000));
	CTestScheduler::StartAndStopL(100);

	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
		
	
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Check target light status"));
	/*Light status can be checked for one target only. If System Target is included in current target, 
	  GetLightStatusForSystemTarget is called to check light status */
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		//check that display lights are on
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOn );
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOn"));
			//wait for light on duration to expire
			CTestScheduler::StartAndStopL(3100);
			FormatMessageL(EHwrmMessageOpDebug,_L(" Check display light status"));
			//lights should be off
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOff );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOff );
                }
            
			if( errVal != KErrNone )
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			}
		}
	else
		{
		//check that display lights are on
		if (myLight->LightStatus( aTarget ) == CHWRMExtendedLight::ELightOn)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOn"));
			//wait for light on duration to expire
			CTestScheduler::StartAndStopL(3100);
			
			FormatMessageL(EHwrmMessageOpDebug,_L(" Check display light status"));
			//lights should be off
			if (myLight->LightStatus( aTarget ) != CHWRMExtendedLight::ELightOff)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;					
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			}
		}
	//delete light instance
    CleanupStack::PopAndDestroy( myLight );	
    
    return errVal;
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightOnDurationTestEnhanced
// Set target lights on for specified duration.
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightOnDurationTestEnhanced( TInt aTarget, TBool aIsRGBTest, TInt aRed, TInt aGreen, TInt aBlue )
    {
    iObserver.InfoMessage(_L("LightOnDurationTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM Enhanced [LightOnWithDuration], 0x%x"), aTarget);
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMEnhancedLight* myLight = CHWRMEnhancedLight::NewLC();
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target lights off"));
	//if light are on, set off for infinite duration
	TRAP(errVal, myLight->LightOffL( aTarget ));
	CTestScheduler::StartAndStopL(100);	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}	

	if ( aIsRGBTest )
		{
		TRAP(errVal, myLight->SetLightDefaultColorL( aTarget ));
		CTestScheduler::StartAndStopL(100);	
		if(errVal != KErrNone)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->SetLightColorL() failed [%d]"), errVal);
			CleanupStack::PopAndDestroy( myLight );
			return errVal;
			}	
		}

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target lights on"));
	
	//set light on for default intensity and 3 seconds
	if ( aIsRGBTest )
		{
		THWRMLightColor aRGBValue;
		aRGBValue.iRed = aRed;
		aRGBValue.iGreen = aGreen;
		aRGBValue.iBlue = aBlue;
		TRAP(errVal, myLight->LightOnL( aTarget, 3000, aRGBValue));
		}
	else
		{
		TRAP(errVal, myLight->LightOnL( aTarget, 3000));
		}
	CTestScheduler::StartAndStopL(100);

	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
		
	
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Check target light status"));
	/*Light status can be checked for one target only. If System Target is included in current target, 
	  GetLightStatusForSystemTarget is called to check light status */
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		//check that display lights are on
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOn );
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOn"));
			//wait for light on duration to expire
			CTestScheduler::StartAndStopL(3100);
			FormatMessageL(EHwrmMessageOpDebug,_L(" Check display light status"));
			//lights should be off
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOff );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOff );
                }
            
			if( errVal != KErrNone )
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			}
		}
	else
		{
		//check that display lights are on
		if (myLight->LightStatus( aTarget ) == CHWRMExtendedLight::ELightOn)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOn"));
			//wait for light on duration to expire
			CTestScheduler::StartAndStopL(3100);
			
			FormatMessageL(EHwrmMessageOpDebug,_L(" Check display light status"));
			//lights should be off
			if (myLight->LightStatus( aTarget ) != CHWRMExtendedLight::ELightOff)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;					
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			}
		}
	//delete light instance
    CleanupStack::PopAndDestroy( myLight );	
    
    return errVal;
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightOffDurationTest
// Call Light Off Duration Test for multiple targets depending on selected API
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightOffDurationTest( TInt aTargetMask )
	{
	TInt currentTarget(1);
	TInt err( KErrNone );
	TInt expectedResult( KErrNone );
	
	TBuf<100> msg( KNullDesC );
	
	_LIT( KSupportedMessageText, "HWRM [Light Off duration test]: Target 0x%x supported"  );	
	_LIT( KNotSupportedMessageText, "HWRM [Light Off duration test]: Target 0x%x not supported"  );		

	//loop through all possible targets
	for( TInt i = 0; i < 31; i++ )
		{
		//if current target is not included in the given mask, or not defined in CenRep, KErrNot Supported is expected
		if( ( aTargetMask & currentTarget ) & iSupportedTargets )
			{
			expectedResult = KErrNone;
			msg.Format( KSupportedMessageText, currentTarget );
			}
		else
			{
			expectedResult = KErrNotSupported;
			msg.Format( KNotSupportedMessageText, currentTarget );
			}
		
		
		if( iApiType == EHWRMApiTypeSAPI  )
			{
			err = LightOffDurationTestSDK( currentTarget );
			}
		else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) )
			{
			err = LightOffDurationTestExtended( currentTarget );
			}
			
			
		LogResult( msg, err, expectedResult );

		// shift the target ready for next loop
        currentTarget <<= 1;
		}
	}
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightOffDurationTestSDK
// Set target lights on and set lights off for 3 seconds
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightOffDurationTestSDK( TInt aTarget )
    {
    iObserver.InfoMessage(_L("LightOffDurationTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L( " Test case: HWRM SDK [LightOffWithDuration], 0x%x"), aTarget );
    TInt errVal = KErrNone;
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
   
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target (0x%x) lights on"), aTarget);
	//Set target light on for infinite duration
	TRAP(errVal, myLight->LightOnL( aTarget ));	
	CTestScheduler::StartAndStopL(100);	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL (0x%x) failed [%d]"), aTarget, errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target (0x%x) lights off"), aTarget);
	//set light off for default intensity and 3 seconds
	TRAP(errVal, myLight->LightOffL( aTarget, 3000));
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL (0x%x) failed [%d]"), aTarget, errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
	
	/*Light status can be checked for one target only. If System Target is included in current target, 
	  GetLightStatusForSystemTarget is called to check light status */
	if( aTarget & CHWRMLight::ESystemTarget )
		{
		//check that display lights are off
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOff );
		
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOff (0x%x)"), aTarget);
			CTestScheduler::StartAndStopL(3100);
			
			//should be back on baseline (lights on)
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOn );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOn );
                }
	    	if ( errVal != KErrNone ) 
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;
				}
	    	}
	    else
	    	{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
	    	}
		}
	else
		{
		//check that display lights are off
		if (myLight->LightStatus( aTarget ) == CHWRMLight::ELightOff)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOff (0x%x)"), aTarget);
			CTestScheduler::StartAndStopL(3100);
			
			//should be back on baseline (lights on)	
			if(myLight->LightStatus( aTarget ) != CHWRMLight::ELightOn)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;	
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;			
			}
    	}
    //delete light instance
    CleanupStack::PopAndDestroy( myLight );
    
    return errVal;
        
	}
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightOffDurationTestExtended
// Set target lights on and set lights off for 3 seconds
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightOffDurationTestExtended( TInt aTarget )
    {      
    iObserver.InfoMessage(_L("LightOffDurationTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L( " Test case: HWRM Extended [LightOffWithDuration], 0x%x"), aTarget );
    TInt errVal = KErrNone;
    //create light instance
    CHWRMExtendedLight* myLight = NULL;

	if ( iApiType == EHWRMApiTypeEAPI )
    	{
		myLight = CHWRMEnhancedLight::NewLC();
    	}
	else
		{
    	myLight = CHWRMExtendedLight::NewLC();
		}
   
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target (0x%x) lights on"), aTarget);
	//Set target light on for infinite duration
	TRAP(errVal, myLight->LightOnL( aTarget ));	
	CTestScheduler::StartAndStopL(100);	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL (0x%x) failed [%d]"), aTarget, errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set target (0x%x) lights off"), aTarget);
	//set light off for default intensity and 3 seconds
	TRAP(errVal, myLight->LightOffL( aTarget, 3000));
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL (0x%x) failed [%d]"), aTarget, errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
	
	/*Light status can be checked for one target only. If System Target is included in current target, 
	  GetLightStatusForSystemTarget is called to check light status */
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		//check that display lights are off
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOff );
		
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOff (0x%x)"), aTarget);
			CTestScheduler::StartAndStopL(3100);
			
			//should be back on baseline (lights on)
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOn );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOn );    
                }
	    	if ( errVal != KErrNone ) 
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;
				}
	    	}
	    else
	    	{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
	    	}
		}
	else
		{
		//check that display lights are off
		if (myLight->LightStatus( aTarget ) == CHWRMExtendedLight::ELightOff)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" ELightOff (0x%x)"), aTarget);
			CTestScheduler::StartAndStopL(3100);
			
			//should be back on baseline (lights on)	
			if(myLight->LightStatus( aTarget ) != CHWRMExtendedLight::ELightOn)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;	
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target:(0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;			
			}
    	}
    //delete light instance
    CleanupStack::PopAndDestroy( myLight );
    
    return errVal;

	}


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightIntensityTest
// Call LightIntensityTest for multiple targets depending on selected API
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightIntensityTest( TInt aTargetMask )
	{
	TInt currentTarget(1);
	TInt err( KErrNone );
	TInt expectedResult( KErrNone );
	
	TBuf<100> msg( KNullDesC );
	
	_LIT( KSupportedMessageText, "HWRM [Light intensity test]: Target 0x%x supported"  );	
	_LIT( KNotSupportedMessageText, "HWRM [Light intensity test]: Target 0x%x not supported"  );		

	//loop through all possible targets
	for( TInt i = 0; i < 31; i++ )
		{
		//if current target is not included in the given mask, or not defined in CenRep, KErrNot Supported is expected
		if( ( aTargetMask & currentTarget ) & iSupportedTargets )
			{
			expectedResult = KErrNone;
			msg.Format( KSupportedMessageText, currentTarget );
			}
		else
			{
			expectedResult = KErrNotSupported;
			msg.Format( KNotSupportedMessageText, currentTarget );
			}
		
		
		if( iApiType == EHWRMApiTypeSAPI  )
			{
			err = LightIntensityTestSDK( currentTarget );
			}
		else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) )
			{
			err = LightIntensityTestExtended( currentTarget );
			}
			
			
		LogResult( msg, err, expectedResult );

		// shift the target ready for next loop
        currentTarget <<= 1;
		}
	}
  
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightIntensityTest
// Set target lights on and with different intensity levels.
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightIntensityTestSDK( TInt aTarget )
    {
    iObserver.InfoMessage(_L("LightIntensityTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" HWRM SDK [LightOnWithIntensity]"));
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
	
	//set lights off for infinite duration
	TRAP(errVal, myLight->LightOffL( aTarget ));
	CTestScheduler::StartAndStopL(100);	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}

	//set lights on with different intensity values
	for(TInt i=0; i<=100; i+=10)
		{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on with %d intensity"), i);
			//set light on 
			TRAP(errVal, myLight->LightOnL( aTarget, 2000, i, ETrue));
			CTestScheduler::StartAndStopL(100);
			if(errVal != KErrNone)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL() failed [%d]"), errVal );
				CleanupStack::PopAndDestroy( myLight );
				return errVal;
				}
				
			//set timer to wait the duration 
			CTestScheduler::StartAndStopL(2100);
			
			
		if( aTarget & CHWRMLight::ESystemTarget )
			{
			//check that display lights are off
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOff );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOff );
                }
            
			if( errVal != KErrNone )
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target(0x%x"), aTarget);
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;
				}
			}
		else
			{
			//check that display lights are off again
			if(myLight->LightStatus( aTarget ) != CHWRMLight::ELightOff)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target(0x%x"), aTarget);
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;	
				}
			}
		}

	//delete light instance
    CleanupStack::PopAndDestroy( myLight );	
    
    return errVal;
   
    }

TInt CHwrmTestCaseController::LightIntensityTestExtended( TInt aTarget )
    {
    iObserver.InfoMessage(_L("LightIntensityTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" HWRM Extended [LightOnWithIntensity]"));
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMExtendedLight* myLight = NULL;

	if ( iApiType == EHWRMApiTypeEAPI )
    	{
		myLight = CHWRMEnhancedLight::NewLC();
    	}
	else
		{
    	myLight = CHWRMExtendedLight::NewLC();
		}
	
	//set lights off for infinite duration
	TRAP(errVal, myLight->LightOffL( aTarget ));
	CTestScheduler::StartAndStopL(100);	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}

	//set lights on with different intensity values
	for(TInt i=0; i<=100; i+=10)
		{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on with %d intensity"), i);
			//set light on 
			TRAP(errVal, myLight->LightOnL( aTarget, 2000, i, ETrue));
			CTestScheduler::StartAndStopL(100);
			if(errVal != KErrNone)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOnL() failed [%d]"), errVal );
				CleanupStack::PopAndDestroy( myLight );
				return errVal;
				}
				
			//set timer to wait the duration 
			CTestScheduler::StartAndStopL(2100);
			
			
		if( aTarget & CHWRMExtendedLight::ESystemTarget )
			{
			//check that display lights are off
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOff );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOff );
                }
            
			if( errVal != KErrNone )
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target(0x%x"), aTarget);
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;
				}
			}
		else
			{
			//check that display lights are off again
			if(myLight->LightStatus( aTarget ) != CHWRMExtendedLight::ELightOff)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK, target(0x%x"), aTarget);
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;	
				}
			}
		}

	//delete light instance
    CleanupStack::PopAndDestroy( myLight );	
    
    return errVal;
    }



// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkTest
// Call LightBlinkTest for multiple targets depending on selected API
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightBlinkTest( TInt aTargetMask )
	{
	TInt currentTarget(1);
	TInt err( KErrNone );
	TInt expectedResult( KErrNone );
	
	TBuf<100> msg( KNullDesC );
	
	_LIT( KSupportedMessageText, "HWRM [Light blink test]: Target 0x%x supported"  );	
	_LIT( KNotSupportedMessageText, "HWRM [Light blink test]: Target 0x%x not supported"  );		

	//loop through all possible targets
	for( TInt i = 0; i < 31; i++ )
		{
		//if current target is not included in the given mask, or not defined in CenRep, KErrNot Supported is expected
		if( ( aTargetMask & currentTarget ) & iSupportedTargets )
			{
			expectedResult = KErrNone;
			msg.Format( KSupportedMessageText, currentTarget );
			}
		else
			{
			expectedResult = KErrNotSupported;
			msg.Format( KNotSupportedMessageText, currentTarget );
			}
		
		
		if( iApiType == EHWRMApiTypeSAPI  )
			{
			err = LightBlinkTestSDK( currentTarget );
			}
		else if( iApiType == EHWRMApiTypeXAPI || (iApiType == EHWRMApiTypeEAPI) )
			{
			err = LightBlinkTestExtended( currentTarget );
			}
			
		LogResult( msg, err, expectedResult );

		// shift the target ready for next loop
        currentTarget <<= 1;
		}
	}

    
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkTestSDK
// Set target lights blink for infinite duration
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightBlinkTestSDK( TInt aTarget )
    {
    iObserver.InfoMessage(_L("LightBlinkTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM SDK [LightBlinkTest] target:0x%x"), aTarget );
    
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink"));
	
	//set lights to blink 
	TRAP(errVal, myLight->LightBlinkL( aTarget ));
	
	CTestScheduler::StartAndStopL(100);	
	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightBlinkL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
	
	
	if( aTarget & CHWRMLight::ESystemTarget )
		{
		//check that display lights are blinking
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightBlink );
	
		
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
		
			//set lights off for 3 sec
			TRAP(errVal, myLight->LightOffL( aTarget, 3000));
		
			CTestScheduler::StartAndStopL( 100 );	
		
			if(errVal != KErrNone)
				{
				FormatMessageL(EHwrmMessageOpDebug, _L(" iMyLight->LightOffL() (0x%x) failed [%d]"), aTarget, errVal );
				CleanupStack::PopAndDestroy( myLight );
				return errVal;
				}
			
			//wait 3+ seconds 
			CTestScheduler::StartAndStopL( 3100 );
			
			//check that lights are blinking again...
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightBlink );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightBlink );
                }
			
			if( errVal != KErrNone )	
				{
				FormatMessageL(EHwrmMessageOpDebug, _L(" Light status not OK, target: 0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug, _L(" Display light status: 0x%x"), iMyStatus );		
				errVal = KErrGeneral;
				}	
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug, _L(" Light status not OK, target: 0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug, _L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			}
		}
		else
			{
			if( myLight->LightStatus( aTarget ) == CHWRMLight::ELightBlink)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
			
				//set lights off for 3 sec
				TRAP(errVal, myLight->LightOffL( aTarget, 3000));
			
				CTestScheduler::StartAndStopL( 100 );	
			
				if(errVal != KErrNone)
					{
					FormatMessageL(EHwrmMessageOpDebug, _L(" iMyLight->LightOffL() failed [%d]"), errVal );
					CleanupStack::PopAndDestroy( myLight );
					return errVal;
					}
				
				//wait 3+ seconds
				CTestScheduler::StartAndStopL( 3100 );
				
				//check that lights are blinking again...
				if( myLight->LightStatus( aTarget ) != CHWRMLight::ELightBlink )	
					{
					FormatMessageL(EHwrmMessageOpDebug, _L(" Light status not OK, target: 0x%x"), aTarget );
					FormatMessageL(EHwrmMessageOpDebug, _L(" Display light status: 0x%x"), iMyStatus );		
					errVal = KErrGeneral;
					}
				}
			else
				{
				FormatMessageL(EHwrmMessageOpDebug, _L(" Light status not OK, target: 0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug, _L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;
				}
			}
			
	//delete light instance
	CleanupStack::PopAndDestroy( myLight );
	
    return errVal;
    
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkTestExtended
// Set target lights blink for infinite duration
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightBlinkTestExtended( TInt aTarget )
    {

    iObserver.InfoMessage(_L("LightBlinkTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM Extended [LightBlinkTest] target:0x%x"), aTarget );
    
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMExtendedLight* myLight = NULL;

	if ( iApiType == EHWRMApiTypeEAPI )
    	{
		myLight = CHWRMEnhancedLight::NewLC();
    	}
	else
		{
    	myLight = CHWRMExtendedLight::NewLC();
		}
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink"));
	
	//set lights to blink 
	TRAP(errVal, myLight->LightBlinkL( aTarget ));
	
	CTestScheduler::StartAndStopL(100);	
	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightBlinkL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
	
	
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		//check that display lights are blinking
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightBlink );
	
		
		if( errVal == KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
		
			//set lights off for 3 sec
			TRAP(errVal, myLight->LightOffL( aTarget, 3000));
		
			CTestScheduler::StartAndStopL( 100 );	
		
			if(errVal != KErrNone)
				{
				FormatMessageL(EHwrmMessageOpDebug, _L(" iMyLight->LightOffL() (0x%x) failed [%d]"), aTarget, errVal );
				CleanupStack::PopAndDestroy( myLight );
				return errVal;
				}
			
			//wait 3+ seconds 
			CTestScheduler::StartAndStopL( 3100 ); 
			
			//check that lights are blinking again...
			errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightBlink );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightBlink );
                }
			
			if( errVal != KErrNone )	
				{
				FormatMessageL(EHwrmMessageOpDebug, _L(" Light status not OK, target: 0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug, _L(" Display light status: 0x%x"), iMyStatus );		
				errVal = KErrGeneral;
				}	
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug, _L(" Light status not OK, target: 0x%x"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug, _L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			}
		}
		else
			{
			if( myLight->LightStatus( aTarget ) == CHWRMExtendedLight::ELightBlink)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
			
				//set lights off for 3 sec
				TRAP(errVal, myLight->LightOffL( aTarget, 3000));
			
				CTestScheduler::StartAndStopL( 100 );	
			
				if(errVal != KErrNone)
					{
					FormatMessageL(EHwrmMessageOpDebug, _L(" iMyLight->LightOffL() failed [%d]"), errVal );
					CleanupStack::PopAndDestroy( myLight );
					return errVal;
					}
				
				//wait 3+ seconds
				CTestScheduler::StartAndStopL( 3100 );
				
				//check that lights are blinking again...
				if( myLight->LightStatus( aTarget ) != CHWRMExtendedLight::ELightBlink )	
					{
					FormatMessageL(EHwrmMessageOpDebug, _L(" Light status not OK, target: 0x%x"), aTarget );
					FormatMessageL(EHwrmMessageOpDebug, _L(" Display light status: 0x%x"), iMyStatus );		
					errVal = KErrGeneral;
					}
				}
			else
				{
				FormatMessageL(EHwrmMessageOpDebug, _L(" Light status not OK, target: 0x%x"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug, _L(" Display light status: 0x%x"), iMyStatus);
				errVal = KErrGeneral;
				}
			}
			
	//delete light instance
	CleanupStack::PopAndDestroy( myLight );
	
    return errVal;   
    }
    
    
    
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkDurationTest
// Call LightBlinkTest for multiple targets depending on selected API
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightBlinkDurationTest( TInt aTargetMask, TBool aIsRGBTest, TInt aRed, TInt aGreen, TInt aBlue )
	{
	TInt currentTarget(1);
	TInt err( KErrNone );
	TInt expectedResult( KErrNone );
	
	TBuf<100> msg( KNullDesC );
	
	_LIT( KSupportedMessageText, "HWRM [Light blink duration test]: Target 0x%x supported"  );	
	_LIT( KNotSupportedMessageText, "HWRM [Light blink duration test]: Target 0x%x not supported"  );		

	//loop through all possible targets
	for( TInt i = 0; i < 31; i++ )
		{
		//if current target is not included in the given mask, or not defined in CenRep, KErrNot Supported is expected
		if( ( aTargetMask & currentTarget ) & iSupportedTargets )
			{
			expectedResult = KErrNone;
			msg.Format( KSupportedMessageText, currentTarget );
			}
		else
			{
			expectedResult = KErrNotSupported;
			msg.Format( KNotSupportedMessageText, currentTarget );
			}
		
		
		if( iApiType == EHWRMApiTypeSAPI  )
			{
			err = LightBlinkDurationTestSDK( currentTarget, aIsRGBTest, aRed, aGreen, aBlue );
			}
		else if( iApiType == EHWRMApiTypeXAPI )
			{
			err = LightBlinkDurationTestExtended( currentTarget );
			}
		else if( iApiType == EHWRMApiTypeEAPI )
			{
			err = LightBlinkDurationTestEnhanced( currentTarget, aIsRGBTest, aRed, aGreen, aBlue );
			}
			
			
		LogResult( msg, err, expectedResult );

		// shift the target ready for next loop
        currentTarget <<= 1;
		}
	}    
      
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkDurationTestSDK
// Set target lights to blink for defined duration
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightBlinkDurationTestSDK( TInt aTarget, TBool aIsRGBTest, TInt aRed, TInt aGreen, TInt aBlue )
    {
    
    //define duration for lights to blink
    TInt duration( 4000 );
    
   	iObserver.InfoMessage(_L("LightBlinkDurationTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM SDK [LightBlinkWithDuration], target: 0x%x"), aTarget );
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
    
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	TRAP(errVal, myLight->LightOffL( aTarget ));
	CTestScheduler::StartAndStopL(100);
	
	if(errVal != KErrNone)
	{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
	}

	if ( aIsRGBTest )
		{
		TRAP(errVal, myLight->SetLightDefaultColorL( aTarget ));
		
		if(errVal != KErrNone)
		{
			FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->SetLightDefaultColorL() failed [%d]"), errVal);
			CleanupStack::PopAndDestroy( myLight );
			return errVal;
		}
		}
    	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink"));
	
	//set lights to blink for <duration> seconds
	if ( aIsRGBTest )
		{
		THWRMLightColor aRGBValue;
		aRGBValue.iRed = aRed;
		aRGBValue.iGreen = aGreen;
		aRGBValue.iBlue = aBlue;
		TRAP(errVal, myLight->LightBlinkL( aTarget, duration, aRGBValue));
		}
	else
		{
		TRAP(errVal, myLight->LightBlinkL( aTarget, duration ));
		}
	
	CTestScheduler::StartAndStopL(100);	
	
	if(errVal != KErrNone)
		{
			FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightBlinkL() failed [%d]"), errVal);
			CleanupStack::PopAndDestroy( myLight );
			return errVal;
		}
		
	
	CHWRMLight::TLightStatus expectedStatus( CHWRMLight::ELightBlink );
		
	if( aTarget & CHWRMLight::ESystemTarget )
		{
		//check that display lights are blinking
		errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
		
		if( errVal == KErrNone )
			{
			CTestScheduler::StartAndStopL( duration + 100 );
			
			//check that target lights are off again
			expectedStatus = CHWRMLight::ELightOff;
			
			errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
                }
			
			if( errVal != KErrNone )
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x)no OK"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus );
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x) not OK"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus);
			errVal = KErrGeneral;	
			}
		}
	else
		{
		
		if( myLight->LightStatus( aTarget ) == expectedStatus )
			{
			CTestScheduler::StartAndStopL( duration + 100 );
			
			//check that target lights are off again
			expectedStatus = CHWRMLight::ELightOff;
			
			if(myLight->LightStatus( aTarget ) != expectedStatus)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status no OK"));
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus );
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x) not OK"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus);
			errVal = KErrGeneral;
			}
		}
		
		
	//delete light instance
    CleanupStack::PopAndDestroy( myLight );
    return errVal;
    
    }


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkDurationTestExtended
// Set target lights to blink for defined duration
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightBlinkDurationTestExtended( TInt aTarget )
    {

    //define duration for lights to blink
    TInt duration( 4000 );
    
   	iObserver.InfoMessage(_L("LightBlinkDurationTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM Extended [LightBlinkWithDuration], target: 0x%x"), aTarget );
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMExtendedLight* myLight = CHWRMExtendedLight::NewLC();
    
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	TRAP(errVal, myLight->LightOffL( aTarget ));
	CTestScheduler::StartAndStopL(100);
	
	if(errVal != KErrNone)
	{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
	}
    	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink"));
	
	//set lights to blink for <duration> seconds
	TRAP(errVal, myLight->LightBlinkL( aTarget, duration ));
	
	CTestScheduler::StartAndStopL(100);	
	
	if(errVal != KErrNone)
		{
			FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightBlinkL() failed [%d]"), errVal);
			CleanupStack::PopAndDestroy( myLight );
			return errVal;
		}
		
	
	CHWRMExtendedLight::TLightStatus expectedStatus( CHWRMExtendedLight::ELightBlink );
		
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		//check that display lights are blinking
		errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
		
		if( errVal == KErrNone )
			{
			CTestScheduler::StartAndStopL( duration + 100 );
			
			//check that target lights are off again
			expectedStatus = CHWRMExtendedLight::ELightOff;
			
			errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
                }
			
			if( errVal != KErrNone )
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x)no OK"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus );
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x) not OK"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus);
			errVal = KErrGeneral;	
			}
		}
	else
		{
		
		if( myLight->LightStatus( aTarget ) == expectedStatus )
			{
			CTestScheduler::StartAndStopL( duration + 100 );
			
			//check that target lights are off again
			expectedStatus = CHWRMExtendedLight::ELightOff;
			
			if(myLight->LightStatus( aTarget ) != expectedStatus)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status no OK"));
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus );
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x) not OK"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus);
			errVal = KErrGeneral;
			}
		}
		
		
	//delete light instance
    CleanupStack::PopAndDestroy( myLight );
    return errVal;
    
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkDurationTestEnhanced
// Set target lights to blink for defined duration
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightBlinkDurationTestEnhanced( TInt aTarget, TBool aIsRGBTest, TInt aRed, TInt aGreen, TInt aBlue )
    {

    //define duration for lights to blink
    TInt duration( 4000 );
    
   	iObserver.InfoMessage(_L("LightBlinkDurationTest"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM Extended [LightBlinkWithDuration], target: 0x%x"), aTarget );
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMEnhancedLight* myLight = CHWRMEnhancedLight::NewLC();
    
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	TRAP(errVal, myLight->LightOffL( aTarget ));
	CTestScheduler::StartAndStopL(100);
	
	if(errVal != KErrNone)
	{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
	}


	if ( aIsRGBTest )
		{
		TRAP(errVal, myLight->SetLightDefaultColorL( aTarget ));
		
		if(errVal != KErrNone)
		{
			FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->SetLightDefaultColorL() failed [%d]"), errVal);
			CleanupStack::PopAndDestroy( myLight );
			return errVal;
		}
		}
			
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink"));
	
	//set lights to blink for <duration> seconds
	if ( aIsRGBTest )
		{
		THWRMLightColor aRGBValue;
		aRGBValue.iRed = aRed;
		aRGBValue.iGreen = aGreen;
		aRGBValue.iBlue = aBlue;
		TRAP(errVal, myLight->LightBlinkL( aTarget, duration, aRGBValue));
		}
	else
		{
		TRAP(errVal, myLight->LightBlinkL( aTarget, duration ));
		}
	
	CTestScheduler::StartAndStopL(100);	
	
	if(errVal != KErrNone)
		{
			FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightBlinkL() failed [%d]"), errVal);
			CleanupStack::PopAndDestroy( myLight );
			return errVal;
		}
		
	
	CHWRMExtendedLight::TLightStatus expectedStatus( CHWRMExtendedLight::ELightBlink );
		
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		//check that display lights are blinking
		errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
		
		if( errVal == KErrNone )
			{
			CTestScheduler::StartAndStopL( duration + 100 );
			
			//check that target lights are off again
			expectedStatus = CHWRMExtendedLight::ELightOff;
			
			errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
            
            if(errVal != KErrNone)
                {
                //All targets might not be reset, so wait for some more time.
                FormatMessageL(EHwrmMessageOpDebug,_L(" Wait for more time"));
                CTestScheduler::StartAndStopL(KHwrmtestsLightMoreDelay);
                errVal = GetLightStatusForSystemTarget( myLight, expectedStatus );
                }
			
			if( errVal != KErrNone )
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x)no OK"), aTarget );
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus );
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x) not OK"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus);
			errVal = KErrGeneral;	
			}
		}
	else
		{
		
		if( myLight->LightStatus( aTarget ) == expectedStatus )
			{
			CTestScheduler::StartAndStopL( duration + 100 );
			
			//check that target lights are off again
			expectedStatus = CHWRMExtendedLight::ELightOff;
			
			if(myLight->LightStatus( aTarget ) != expectedStatus)
				{
				FormatMessageL(EHwrmMessageOpDebug,_L(" Light status no OK"));
				FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus );
				errVal = KErrGeneral;				
				}
			}
		else
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status (0x%x) not OK"), aTarget );
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x, expected: 0x%x"), iMyStatus, expectedStatus);
			errVal = KErrGeneral;
			}
		}
		
		
	//delete light instance
    CleanupStack::PopAndDestroy( myLight );
    return errVal;
    
    }




// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkDurationTest
// Call LightBlinkTest for multiple targets depending on selected API
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::LightBlinkOnOffTest( TInt aTargetMask )
	{
	TInt currentTarget(1);
	TInt err( KErrNone );
	TInt expectedResult( KErrNone );
	
	TBuf<100> msg( KNullDesC );
	
	_LIT( KSupportedMessageText, "HWRM [Light blink on/off test]: Target 0x%x supported"  );	
	_LIT( KNotSupportedMessageText, "HWRM [Light blink on/off test]: Target 0x%x not supported"  );		

	//loop through all possible targets
	for( TInt i = 0; i < 31; i++ )
		{
		//if current target is not included in the given mask, or not defined in CenRep, KErrNot Supported is expected
		if( ( aTargetMask & currentTarget ) & iSupportedTargets )
			{
			expectedResult = KErrNone;
			msg.Format( KSupportedMessageText, currentTarget );
			}
		else
			{
			expectedResult = KErrNotSupported;
			msg.Format( KNotSupportedMessageText, currentTarget );
			}
		
		
		if( iApiType == EHWRMApiTypeSAPI  )
			{
			err = LightBlinkOnOffTestSDK( currentTarget );
			}
		else if( (iApiType == EHWRMApiTypeXAPI) || (iApiType == EHWRMApiTypeEAPI) )
			{
			err = LightBlinkOnOffTestExtended( currentTarget );
			}
			
			
		LogResult( msg, err, expectedResult );

		// shift the target ready for next loop
        currentTarget <<= 1;
		}
	}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkOnOffTestSDK
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightBlinkOnOffTestSDK( TInt aTarget )
    {
    iObserver.InfoMessage(_L("LightBlinkOnOff"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM SDK [LightBlinkOnOff], target: 0x%x"), aTarget );
    
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
    
    FormatMessageL(EHwrmMessageOpDebug,_L(" Check light status"));
    
	if( aTarget & CHWRMLight::ESystemTarget )
		{
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightOff );
		if( errVal != KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	    	TRAP(errVal, myLight->LightOffL( aTarget ));
	    	CTestScheduler::StartAndStopL(100);
	    	
	    	if(errVal != KErrNone)
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
	    		CleanupStack::PopAndDestroy( myLight );
	    		return errVal;	
	    		}
	    	
			}
		
		}
	else
		{
	    if(myLight->LightStatus( aTarget ) != CHWRMLight::ELightOff)
	    	{
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	    	TRAP(errVal, myLight->LightOffL( aTarget ));
	    	CTestScheduler::StartAndStopL(100);
	    	
	    	if(errVal != KErrNone)
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
	    		CleanupStack::PopAndDestroy( myLight );
	    		return errVal;	
	    		}
	    	}
		}
    	
    	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink"));

	//set lights to blink for 5 sec
	TRAP(errVal, myLight->LightBlinkL( aTarget, 5000, 500, 300, 100));

	CTestScheduler::StartAndStopL(100);

	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightBlinkL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}


	//check that target lights are blinking
	if( aTarget & CHWRMLight::ESystemTarget )
		{
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMLight::ELightBlink );
		
		if(errVal != KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not Ok"));
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			//wait before returning
			CTestScheduler::StartAndStopL(5000);		
			}
		}
	else
		{
		if(myLight->LightStatus( aTarget ) != CHWRMLight::ELightBlink)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not Ok"));
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			//wait before returning
			CTestScheduler::StartAndStopL(5000);		
			}
		}

	// delete light instance	
    CleanupStack::PopAndDestroy( myLight );
    return errVal;        
    }
  
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightBlinkOnOffTestExtended
// 
// -----------------------------------------------------------------------------
//   
 TInt CHwrmTestCaseController::LightBlinkOnOffTestExtended( TInt aTarget )
    {
    iObserver.InfoMessage(_L("LightBlinkOnOff"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM Extended [LightBlinkOnOff], target: 0x%x"), aTarget );
    
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMExtendedLight* myLight = NULL;

	if ( iApiType == EHWRMApiTypeEAPI )
    	{
		myLight = CHWRMEnhancedLight::NewLC();
    	}
	else
		{
    	myLight = CHWRMExtendedLight::NewLC();
		}
    
    FormatMessageL(EHwrmMessageOpDebug,_L(" Check light status"));
    
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightOff );
		if( errVal != KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	    	TRAP(errVal, myLight->LightOffL( aTarget ));
	    	CTestScheduler::StartAndStopL(100);
	    	
	    	if(errVal != KErrNone)
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
	    		CleanupStack::PopAndDestroy( myLight );
	    		return errVal;	
	    		}
			}
		}
	else
		{
	    if(myLight->LightStatus( aTarget ) != CHWRMExtendedLight::ELightOff)
	    	{
	    	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off"));
	    	TRAP(errVal, myLight->LightOffL( aTarget ));
	    	CTestScheduler::StartAndStopL(100);
	    	
	    	if(errVal != KErrNone)
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightOffL() failed [%d]"), errVal);
	    		CleanupStack::PopAndDestroy( myLight );
	    		return errVal;	
	    		}
	    	}
		}
    	
    	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink"));

	//set lights to blink for 5 sec
	TRAP(errVal, myLight->LightBlinkL( aTarget, 5000, 500, 300, 100));

	CTestScheduler::StartAndStopL(100);

	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" iMyLight->LightBlinkL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}


	//check that target lights are blinking
	if( aTarget & CHWRMExtendedLight::ESystemTarget )
		{
		errVal = GetLightStatusForSystemTarget( myLight, CHWRMExtendedLight::ELightBlink );
		
		if(errVal != KErrNone )
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not Ok"));
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			//wait before returning
			CTestScheduler::StartAndStopL(5000);		
			}
		}
	else
		{
		if( myLight->LightStatus( aTarget ) != CHWRMExtendedLight::ELightBlink)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not Ok"));
			FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), iMyStatus);
			errVal = KErrGeneral;
			//wait before returning
			CTestScheduler::StartAndStopL(5000);		
			}
		}
		
	// delete light instance	
    CleanupStack::PopAndDestroy( myLight );
    return errVal;
        
    }
   
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightWithDifferentTargetsSDK
// Set primary display lights off and on with different targets
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightWithDifferentTargetsSDK( )
    {
    iObserver.InfoMessage( _L( "LightWithDifferentTargetsSDK" ) );
    FormatMessageL(EHwrmMessageOpDebug, _L( " Test case: HWRM SDK [LightWithDifferentTargets]" ) );
    
    TInt errVal( KErrNone );
    CHWRMLight::TLightStatus myStatus;
      
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
    
    //set lights off
   	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
   	TRAP( errVal, myLight->LightOffL( CHWRMLight::EPrimaryDisplayAndKeyboard ) );
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}

    //set lights on
    FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display and keyboard"));
	//set lights on for 3 sec
	TRAP(errVal, myLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, 3000));
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
		
	//check that display lights are on
	myStatus = myLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	if(myStatus != CHWRMLight::ELightOn)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug, _L( "LightWithDifferentTargetsSDK # Display Light status not OK!!! Status: 0x%x" ), myStatus );
		CleanupStack::PopAndDestroy( myLight );	
    	return KErrGeneral;
		}

	//check that keyboard lights are on
	myStatus = myLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	if(myStatus != CHWRMLight::ELightOn)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L("LightWithDifferentTargetsSDK # Keyboard Light status not OK!!! Status:0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return KErrGeneral;
		}
	
	//wait that lights are off again		
	CTestScheduler::StartAndStopL(3100);
    
    //set lights on for primary keyboard
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary keyboard"));
	TRAP(errVal, myLight->LightOnL(CHWRMLight::EPrimaryKeyboard, 5000));
	CTestScheduler::StartAndStopL(1000);
	if (errVal != KErrNone)
	    {
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return KErrGeneral;		
	    }
	
	//check that keyboard lights are on
	myStatus = myLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	
	if( myStatus == CHWRMLight::ELightOn )
		{
		//set lights to blink
		FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink for primary keyboard"));
		TRAP(errVal, myLight->LightBlinkL(CHWRMLight::EPrimaryKeyboard, 5000));
		CTestScheduler::StartAndStopL(1000);
		//check light status
		myStatus = myLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
		if(myStatus != CHWRMLight::ELightBlink)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
			FormatMessageL(EHwrmMessageOpDebug,_L("LightWithDifferentTargetsSDK # Keyboard light status: 0x%x"), myStatus);
			errVal = KErrGeneral;
			}
		else
		    {
		    errVal = KErrNone;
		    }
		CTestScheduler::StartAndStopL(4100);	
			
		}
	else
		{
		FormatMessageL(EHwrmMessageOpDebug, _L( " Light status not OK" ) );
		FormatMessageL(EHwrmMessageOpDebug, _L( " Keyboard light status: 0x%x" ), myStatus );
		errVal = KErrGeneral;
		}
   
    CleanupStack::PopAndDestroy( myLight ); 
     	
    return errVal;   
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightWithDifferentTargetsExtended
// Set primary display lights off and on with different targets
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightWithDifferentTargetsExtended( )
    {
    iObserver.InfoMessage( _L( "LightWithDifferentTargetsExtended" ) );
    FormatMessageL(EHwrmMessageOpDebug, _L( " Test case: HWRM Extended [LightWithDifferentTargets]" ) );
    
    TInt errVal( KErrNone );
    CHWRMExtendedLight::TLightStatus myStatus;
      
	  //create light instance
	  CHWRMExtendedLight* myLight = NULL;
	  
	  if ( iApiType == EHWRMApiTypeEAPI )
		  {
		  myLight = CHWRMEnhancedLight::NewLC();
		  }
	  else
		  {
		  myLight = CHWRMExtendedLight::NewLC();
		  }
    
    //set lights off
   	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
   	TRAP( errVal, myLight->LightOffL( CHWRMExtendedLight::EPrimaryDisplayAndKeyboard ) );
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOffL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}

    //set lights on
    FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display and keyboard"));
	//set lights on for 3 sec
	TRAP(errVal, myLight->LightOnL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, 3000));
	CTestScheduler::StartAndStopL(100);
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight );
		return errVal;
		}
		
	//check that display lights are on
	myStatus = myLight->LightStatus(CHWRMExtendedLight::EPrimaryDisplay);
	if(myStatus != CHWRMExtendedLight::ELightOn)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug, _L( "LightWithDifferentTargetsExtended # Display Light status not OK!!! Status: 0x%x" ), myStatus );
		CleanupStack::PopAndDestroy( myLight );	
    	return KErrGeneral;
		}

	//check that keyboard lights are on
	myStatus = myLight->LightStatus(CHWRMExtendedLight::EPrimaryKeyboard);
	if(myStatus != CHWRMExtendedLight::ELightOn)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L("LightWithDifferentTargetsExtended # Keyboard Light status not OK!!! Status:0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return KErrGeneral;
		}
	
	//wait that lights are off again		
	CTestScheduler::StartAndStopL(3100);
    
    //set lights on for primary keyboard
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary keyboard"));
	TRAP(errVal, myLight->LightOnL(CHWRMExtendedLight::EPrimaryKeyboard, 5000));
	CTestScheduler::StartAndStopL(1000);
	if (errVal != KErrNone)
	    {
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return KErrGeneral;		
	    }
	
	//check that keyboard lights are on
	myStatus = myLight->LightStatus(CHWRMExtendedLight::EPrimaryKeyboard);
	
	if( myStatus == CHWRMExtendedLight::ELightOn )
		{
		//set lights to blink
		FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights to blink for primary keyboard"));
		TRAP(errVal, myLight->LightBlinkL(CHWRMExtendedLight::EPrimaryKeyboard, 5000));
		CTestScheduler::StartAndStopL(1000);
		//check light status
		myStatus = myLight->LightStatus(CHWRMExtendedLight::EPrimaryKeyboard);
		if(myStatus != CHWRMExtendedLight::ELightBlink)
			{
			FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
			FormatMessageL(EHwrmMessageOpDebug,_L("LightWithDifferentTargetsSDK # Keyboard light status: 0x%x"), myStatus);
			errVal = KErrGeneral;
			}
		else
		    {
		    errVal = KErrNone;
		    }
		CTestScheduler::StartAndStopL(4100);	
			
		}
	else
		{
		FormatMessageL(EHwrmMessageOpDebug, _L( " Light status not OK" ) );
		FormatMessageL(EHwrmMessageOpDebug, _L( " Keyboard light status: 0x%x" ), myStatus );
		errVal = KErrGeneral;
		}
   
    CleanupStack::PopAndDestroy( myLight ); 
     	
    return errVal;
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightWithInvalidDurationSDK()
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightWithInvalidDurationSDK( )
    {
    iObserver.InfoMessage(_L("LightWithInvalidDurationSDK"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM SDK [LightWithInvalidDuration]"));
    
    CHWRMLight::TLightStatus myStatus;
    TInt retVal1(KErrNone);
    TInt retVal2(KErrNone);
    TInt retVal3(KErrNone);
    TInt retVal4(KErrNone);
    TInt maxDuration(KHWRMLightMaxDuration);
    TInt infDuration(KHWRMInfiniteDuration);
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
    
    FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
    //Set lights off for infinite duration
   	TRAP(errVal, myLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard));
   	CTestScheduler::StartAndStopL(100);

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display and keyboard"));
	//set lights on for too long duration KHWRMLightMaxDuration+1 //KErrArgument
	TRAP(retVal1, myLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, (maxDuration+1)));
	CTestScheduler::StartAndStopL(100);

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display and keyboard"));	
	//set lights on for infinite duration KHWRMInfiniteDuration//KErrArgument
	TRAP(retVal2, myLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, (infDuration-1)));
	CTestScheduler::StartAndStopL(100);

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display and keyboard"));	
	myLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, KHWRMInfiniteDuration);
	CTestScheduler::StartAndStopL(100);
	
	//check light status for primary display
	myStatus = myLight->LightStatus( CHWRMLight::EPrimaryDisplay );				
	if( myStatus != CHWRMLight::ELightOn )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Primary display light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight );
		return KErrGeneral;
		}
	//check light status for primary keyboard
	myStatus = myLight->LightStatus( CHWRMLight::EPrimaryKeyboard );				
	if( myStatus != CHWRMLight::ELightOn )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Primary keyboard light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight );
		return KErrGeneral;
		}
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
	//set lights off for too long duration KHWRMInfiniteDuration-1 //KErrArgument
	TRAP(retVal3, myLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard, (infDuration-1)));
	CTestScheduler::StartAndStopL(100);
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
	//set lights off for too long duration KHWRMLightMaxDuration+1 //KErrArgument
	TRAP(retVal4, myLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard, (maxDuration+1)));
	CTestScheduler::StartAndStopL(100);
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" ret1[%d], ret2[%d], ret3[%d], ret4[%d]"), retVal1, retVal2, retVal3, retVal4);		
	
	if (retVal1 != KErrArgument || retVal2 != KErrArgument || retVal3 != KErrArgument || retVal4 != KErrArgument)		
		{	
		errVal = KErrGeneral;	
		}
	else
		{			
		errVal = KErrNone;   
		}
	  
    CleanupStack::PopAndDestroy( myLight );
     	
    return errVal;	
    
    }
    
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightWithInvalidDurationExtended()
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightWithInvalidDurationExtended()
    {
    iObserver.InfoMessage(_L("LightWithInvalidDurationExtended"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM Extended [LightWithInvalidDuration]"));
    
    CHWRMExtendedLight::TLightStatus myStatus;
    TInt retVal1(KErrNone);
    TInt retVal2(KErrNone);
    TInt retVal3(KErrNone);
    TInt retVal4(KErrNone);
    TInt maxDuration(KHWRMLightMaxDuration);
    TInt infDuration(KHWRMInfiniteDuration);
    TInt errVal = KErrNone;
    
    //create light instance
    CHWRMExtendedLight* myLight = NULL;

	if ( iApiType == EHWRMApiTypeEAPI )
    	{
		myLight = CHWRMEnhancedLight::NewLC();
    	}
	else
		{
    	myLight = CHWRMExtendedLight::NewLC();
		}
    
    FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
    //Set lights off for infinite duration
   	TRAP(errVal, myLight->LightOffL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard));
   	CTestScheduler::StartAndStopL(100);

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display and keyboard"));
	//set lights on for too long duration KHWRMLightMaxDuration+1 //KErrArgument
	TRAP(retVal1, myLight->LightOnL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, (maxDuration+1)));
	CTestScheduler::StartAndStopL(100);

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display and keyboard"));	
	//set lights on for infinite duration KHWRMInfiniteDuration//KErrArgument
	TRAP(retVal2, myLight->LightOnL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, (infDuration-1)));
	CTestScheduler::StartAndStopL(100);

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display and keyboard"));	
	myLight->LightOnL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, KHWRMInfiniteDuration);
	CTestScheduler::StartAndStopL(100);
	
	//check light status for primary display
	myStatus = myLight->LightStatus( CHWRMExtendedLight::EPrimaryDisplay );				
	if( myStatus != CHWRMExtendedLight::ELightOn )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Primary display light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight );
		return KErrGeneral;
		}
	//check light status for primary keyboard
	myStatus = myLight->LightStatus( CHWRMExtendedLight::EPrimaryKeyboard );				
	if( myStatus != CHWRMExtendedLight::ELightOn )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Primary keyboard light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight );
		return KErrGeneral;
		}
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
	//set lights off for too long duration KHWRMInfiniteDuration-1 //KErrArgument
	TRAP(retVal3, myLight->LightOffL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, (infDuration-1)));
	CTestScheduler::StartAndStopL(100);
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
	//set lights off for too long duration KHWRMLightMaxDuration+1 //KErrArgument
	TRAP(retVal4, myLight->LightOffL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, (maxDuration+1)));
	CTestScheduler::StartAndStopL(100);
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" ret1[%d], ret2[%d], ret3[%d], ret4[%d]"), retVal1, retVal2, retVal3, retVal4);		
	
	if (retVal1 != KErrArgument || retVal2 != KErrArgument || retVal3 != KErrArgument || retVal4 != KErrArgument)		
		{	
		errVal = KErrGeneral;	
		}
	else
		{			
		errVal = KErrNone;   
		}
	  
    CleanupStack::PopAndDestroy( myLight );
     	
    return errVal;	
    }
    
   
    
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightIntensitySDK()
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightIntensitySDK( )
    {
    //iObserver.InfoMessage(_L("LightIntensity"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM SDK [LightWithIntensity]"));
    
    CHWRMLight::TLightStatus myStatus;
    TInt defIntensity( KHWRMDefaultIntensity );
    TInt maxIntensity( KHWRMLightMaxIntensity );
    TInt minIntensity( KHWRMLightMinIntensity );
    
    TInt errVal( KErrNone );
    
    //create light instance
    CHWRMLight* myLight = CHWRMLight::NewLC();
    
    FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
    //Set lights off for infinite duration
   	TRAP(errVal, myLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard));
   	CTestScheduler::StartAndStopL(100);

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display"));
	
	TRAP( errVal, myLight->LightOnL( CHWRMLight::EPrimaryDisplay, 2000, defIntensity, ETrue ) );
	CTestScheduler::StartAndStopL(100);
	if( errVal != KErrNone )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return errVal;	
		}

    myStatus = myLight->LightStatus( CHWRMLight::EPrimaryDisplay );		
	if ( myStatus != CHWRMLight::ELightOn )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight ); 	
		return KErrGeneral;
		}
	
	CTestScheduler::StartAndStopL(2000);
	
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary keyboard"));	
	
	TRAP( errVal, myLight->LightOnL( CHWRMLight::EPrimaryKeyboard, 2000, maxIntensity, ETrue ) );
	CTestScheduler::StartAndStopL(100);
	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return errVal;		
		}

    myStatus = myLight->LightStatus( CHWRMLight::EPrimaryKeyboard );		
	if(myStatus != CHWRMLight::ELightOn)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Keyboard light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight ); 	
		return KErrGeneral;
		}
	
	//wait that lights are off
	CTestScheduler::StartAndStopL(2000);
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display"));	
	
	TRAP(errVal, myLight->LightOnL(CHWRMLight::EPrimaryDisplay, 2000, minIntensity, ETrue));
	CTestScheduler::StartAndStopL(100);
	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return errVal;	
		}

    myStatus = myLight->LightStatus( CHWRMLight::EPrimaryDisplay );	
	if( myStatus != CHWRMLight::ELightOn )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight ); 	
		return KErrGeneral;
		}
		
	CTestScheduler::StartAndStopL(2000);
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for keyboard display (-100)")); //KErrArgument	
	
	TRAP(errVal, myLight->LightOnL(CHWRMLight::EPrimaryDisplay, 2000, -100, ETrue));
	CTestScheduler::StartAndStopL(100);
	if( errVal == KErrArgument )
	    {
		errVal = KErrNone;
	    }
			
    CleanupStack::PopAndDestroy( myLight ); 
     	
    return errVal;	
 
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightIntensityExtended()
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::LightIntensityExtended( )
    {
    iObserver.InfoMessage(_L("LightIntensityExtended"));
    FormatMessageL(EHwrmMessageOpDebug,_L(" Test case: HWRM Extended [LightWithIntensity]"));
    
    CHWRMExtendedLight::TLightStatus myStatus;
    TInt defIntensity( KHWRMDefaultIntensity );
    TInt maxIntensity( KHWRMLightMaxIntensity );
    TInt minIntensity( KHWRMLightMinIntensity );
    
    TInt errVal( KErrNone );
    
    //create light instance
    CHWRMExtendedLight* myLight = NULL;

	if ( iApiType == EHWRMApiTypeEAPI )
    	{
		myLight = CHWRMEnhancedLight::NewLC();
    	}
	else
		{
    	myLight = CHWRMExtendedLight::NewLC();
		}
    
    FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights off for primary display and keyboard"));
    //Set lights off for infinite duration
   	TRAP(errVal, myLight->LightOffL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard));
   	CTestScheduler::StartAndStopL(100);

	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display"));
	
	TRAP( errVal, myLight->LightOnL( CHWRMExtendedLight::EPrimaryDisplay, 2000, defIntensity, ETrue ) );
	CTestScheduler::StartAndStopL(100);
	if( errVal != KErrNone )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return errVal;	
		}

    myStatus = myLight->LightStatus( CHWRMExtendedLight::EPrimaryDisplay );		
	if ( myStatus != CHWRMExtendedLight::ELightOn )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight ); 	
		return KErrGeneral;
		}
	
	CTestScheduler::StartAndStopL(2000);
	
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary keyboard"));	
	
	TRAP( errVal, myLight->LightOnL( CHWRMExtendedLight::EPrimaryKeyboard, 2000, maxIntensity, ETrue ) );
	CTestScheduler::StartAndStopL(100);
	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return errVal;		
		}

    myStatus = myLight->LightStatus( CHWRMExtendedLight::EPrimaryKeyboard );		
	if(myStatus != CHWRMExtendedLight::ELightOn)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Keyboard light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight ); 	
		return KErrGeneral;
		}
	
	//wait that lights are off
	CTestScheduler::StartAndStopL(2000);
	
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for primary display"));	
	
	TRAP(errVal, myLight->LightOnL(CHWRMExtendedLight::EPrimaryDisplay, 2000, minIntensity, ETrue));
	CTestScheduler::StartAndStopL(100);
	
	if(errVal != KErrNone)
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" myLight->LightOnL() failed [%d]"), errVal);
		CleanupStack::PopAndDestroy( myLight ); 	
    	return errVal;	
		}

    myStatus = myLight->LightStatus( CHWRMExtendedLight::EPrimaryDisplay );	
	if( myStatus != CHWRMExtendedLight::ELightOn )
		{
		FormatMessageL(EHwrmMessageOpDebug,_L(" Light status not OK"));
		FormatMessageL(EHwrmMessageOpDebug,_L(" Display light status: 0x%x"), myStatus);
		CleanupStack::PopAndDestroy( myLight ); 	
		return KErrGeneral;
		}
		
	CTestScheduler::StartAndStopL(2000);
		
	FormatMessageL(EHwrmMessageOpDebug,_L(" Set lights on for keyboard display (-100)")); //KErrArgument	
	
	TRAP(errVal, myLight->LightOnL(CHWRMExtendedLight::EPrimaryDisplay, 2000, -100, ETrue));
	CTestScheduler::StartAndStopL(100);
	if( errVal == KErrArgument )
	    {
		errVal = KErrNone;
	    }
			
    CleanupStack::PopAndDestroy( myLight ); 
     	
    return errVal;	
    }
    
    
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::LightWithInvalidTarget
// 
// -----------------------------------------------------------------------------
//
/*TInt CHwrmTestCaseController::LightWithInvalidTarget( )
    {
    iObserver.InfoMessage(_L("LightWithInvalidTarget"));
    TInt retVal1(0), retVal2(0);
    CHWRMLight::TLightTarget invalidTarget = 20; 
    
    errVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
    
    //Set lights off for infinite duration
   	iMyLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	
	//try to set lights on for invalid target //KErrNotSupported
	TRAP(retVal1, iMyLight->LightOnL(invalidTarget, KHWRMLightMaxDuration));
		
	iMyLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, KHWRMInfiniteDuration);
	CTestScheduler::StartAndStopL(1000);
		
	if(iMyStatus != ELightOn)
		{
			delete iMyLight;
			return (errVal = KErrGeneral);
		}
		
	//try to set lights off for invalid target //KErrNotSupported
	TRAP(retVal2, iMyLight->LightOffL(invalidTarget, KHWRMInfiniteDuration);
			
	if (retVal1 == KErrNotSupported && retVal2 == KErrNotSupported)		
		{
			errVal = KErrNone;	
		}
	else
		{
			errVal = KErrGeneral;	
		}
	  
    delete iMyLight; 	
    return errVal;	    
    
    }*/

void CHwrmTestCaseController::FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage)
	{
	SendMessage(aOp,aMessage);
	}

void CHwrmTestCaseController::FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, TInt aArg1)
	{
	HBufC* message = HBufC::NewLC(aMessage.Length() + 10);
	message->Des().Format(aMessage,aArg1);
	SendMessage(aOp,*message);
	CleanupStack::PopAndDestroy(message);
	}

void CHwrmTestCaseController::FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, TInt aArg1, TInt aArg2)
	{
	HBufC* message = HBufC::NewLC(aMessage.Length() + 20);
	message->Des().Format(aMessage,aArg1,aArg2);
	SendMessage(aOp,*message);
	CleanupStack::PopAndDestroy(message);
	}

void CHwrmTestCaseController::FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, TInt aArg1, TInt aArg2, TInt aArg3)
	{
	HBufC* message = HBufC::NewLC(aMessage.Length() + 30);
	message->Des().Format(aMessage,aArg1,aArg2,aArg3);
	SendMessage(aOp,*message);
	CleanupStack::PopAndDestroy(message);
	}

void CHwrmTestCaseController::FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, TInt aArg1, TInt aArg2, TInt aArg3, TInt aArg4)
	{
	HBufC* message = HBufC::NewLC(aMessage.Length() + 40);
	message->Des().Format(aMessage,aArg1,aArg2,aArg3,aArg4);
	SendMessage(aOp,*message);
	CleanupStack::PopAndDestroy(message);
	}

void CHwrmTestCaseController::FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, const TDesC* aArg1)
	{
	HBufC* message = HBufC::NewLC(aMessage.Length() + aArg1->Length() + 10);
	message->Des().Format(aMessage,aArg1);
	SendMessage(aOp,*message);
	CleanupStack::PopAndDestroy(message);
	}

void CHwrmTestCaseController::FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, const TDesC* aArg1, TInt aArg2, TInt aArg3)
	{
	HBufC* message = HBufC::NewLC(aMessage.Length() + aArg1->Length() + 20);
	message->Des().Format(aMessage,aArg1,aArg2,aArg3);
	SendMessage(aOp,*message);
	CleanupStack::PopAndDestroy(message);
	}

// NOTE: this is not very elegant, but it's just test code... improve in future if needed
void CHwrmTestCaseController::SendMessage(THwrmMessageOp aOp, const TDesC& aMessage)
	{
	switch(aOp)
		{
	case EHwrmMessageOpInfo:
		iObserver.InfoMessage(aMessage);
		break;
	case EHwrmMessageOpLog:
		iObserver.LogMessage(aMessage);
		break;
	case EHwrmMessageOpDebug:
		iObserver.DebugMessage(aMessage);
		break;
	default:
		break;	
		}	
	}
	
//
//
//

/* TeK -> */

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::RunSlaveMode
//
// This method is called when application is started in SLAVE-mode
// 
// -----------------------------------------------------------------------------
//
void CHwrmTestCaseController::RunSlaveMode()
{
	if (iMcFramework == NULL)
		{
		TRAPD(err,iMcFramework = CMcFramework::NewL());
		if (err)
			{
			_LIT(KErrorMsg,"Failed to instantiate CMcFramework. Aborting multiclient cases (slave mode)");
			iObserver.LogMessage(KErrorMsg);
			return;
			}
		}
		
	TInt tcase = 0;
	TInt recStat = KErrNone;	  
	
	while (tcase != EHWRMQuitTest)
	{
		tcase = iMcFramework->WaitTestToStart();
		
		if (tcase != EHWRMQuitTest)
		{
			switch(tcase)
			{
				case (EMultiClientCase1):
					recStat = MC_Primary_display_light_S();
					iMcFramework->StepExecuted(recStat);					
					break;
				case (EMultiClientCase2):
					recStat = MC_Display_on_and_keyboard_blink_S();
					iMcFramework->StepExecuted(recStat);					
					break;
				case (EMultiClientCase3):
					recStat = MC_Display_on_and_keyboard_reserved_S();	
					iMcFramework->StepExecuted(recStat);					
					break;
				case (EMultiClientCase4):
					recStat = MC_Display_and_keyboard_blink_S();
					iMcFramework->StepExecuted(recStat);
					break;
				case (EMultiClientCase5):
					recStat = MC_Vibra_Basic_Usage_S();
					iMcFramework->StepExecuted(recStat);
					break;
				case (EMultiClientCase6):
					recStat = Verify_MCFramework_S();
					iMcFramework->StepExecuted(recStat);
					break;
				case (EMultiClientCase7):
					recStat = MC_Combined_light_and_vibra_usage_S();
					iMcFramework->StepExecuted(recStat);
					break;
				case (EMultiClientCase8):
					recStat = MC_Combined_light_and_vibra_usage_2_S();
					iMcFramework->StepExecuted(recStat);
					break;
				case (EMultiClientCase9):
					recStat = MC_FmTx_Basic_Usage_S();
					iMcFramework->StepExecuted(recStat);
					break;		
				case (EMultiClientCase14):
					recStat = MC_Display_RGB_S();
					iMcFramework->StepExecuted(recStat);
					break;		
				default:
					iMcFramework->StepExecuted(KErrGeneral);	
			}
		}
				
	}
}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::Verify_MCFramework_S
// Slave side test case to verify McFramework functionality 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::Verify_MCFramework_S()
{
	
	FormatMessageL(EHwrmMessageOpDebug,_L("Verify_MCFramework_S() starting"));

	CTestScheduler::StartAndStopL(1000); // Testin suoritusta

	iMcFramework->StepExecuted(KErrNone); // palautetaan testistepin tulos
	TInt execStep = iMcFramework->WaitNextTestStep(); // mennään odottamaan seuraavaa steppiä
	
	CTestScheduler::StartAndStopL(1000); // Testin suoritus jatkuu
	
	//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()
		  								
	FormatMessageL(EHwrmMessageOpDebug,_L("Verify_MCFramework_S() done"));
	
	return KErrNone;

}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Primary_display_light_S
// Light usage with two applications / This application is on the background
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Primary_display_light_S()
    {
    
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Primary_display_light_S()"));
    TInt iErrVal = KErrNone;
    CHWRMLight::TLightStatus currStatus;
    CHWRMLight::TLightStatus origStatus;
       
    //create light instance
    TRAP(iErrVal, iMyLight = CHWRMLight::NewL(this));	    
    
    if (iErrVal == KErrNone)
    {
    
	    origStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	    
	    //reserve light, Restore state after release = True, Force CoeNoeEnvOn = True
		TRAP(iErrVal, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplay, ETrue, ETrue));
		
		if (iErrVal == KErrNone)
		{
			// Reservation was OK
			
			// Set primary display light blinking
			iErrVal = TestLightBlink(CHWRMLight::EPrimaryDisplay, 10000, 1000, 1000, 100);
			
			if (iErrVal == KErrNone)
			{
				// Check light status
				currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);	
					
				if (currStatus == CHWRMLight::ELightBlink )
				{
					/* Light status is OK */
					
					iMcFramework->StepExecuted(KErrNone);
					iErrVal = iMcFramework->WaitNextTestStep();
					
					if (iErrVal == KErrNone)
					{			
						/* Test execution was OK in peer-application */
						
						/* Check current light status */
						currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
						
						if (currStatus == CHWRMLight::ELightBlink)
						{
							/* OK, light is still blinking */
							
							/* Wait 10 seconds */
							CTestScheduler::StartAndStopL(10000);
							
							// Release light, then status should return in state as it was before reservation
							iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplay);
							
							CTestScheduler::StartAndStopL(2000);
							/* Check light status */
							currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);			
							
							if(currStatus == origStatus)
							{
								/* OK, light is not blinking anymore */
								iErrVal = KErrNone;	
								//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()
							}
							else
							{
								FormatMessageL(EHwrmMessageOpDebug,_L("Wrong light status %d. Expected original: %d"), currStatus, origStatus);
								iErrVal = KErrGeneral; 
							}
							
						}
						else
						{
							// wrong light status
							FormatMessageL(EHwrmMessageOpDebug,_L("Wrong light status:%d. Expected [Blink]"), currStatus);
							iErrVal = KErrGeneral; 
						}
					}
					else
					{
						// execution failed in peer-application
						FormatMessageL(EHwrmMessageOpDebug,_L("Execution failed peer. Error:%d"), iErrVal); 	
					}
				}
				else
				{
					FormatMessageL(EHwrmMessageOpDebug,_L("Wrong light status [%d]. Expected [Blink]"), currStatus);
					iErrVal = KErrGeneral; 
				}			
			}
			else
			{
				// Set light blinking failed
				FormatMessageL(EHwrmMessageOpDebug,_L("Set light blink Error:%d"),iErrVal);
			}
		}
		else
		{
			FormatMessageL(EHwrmMessageOpDebug,_L("ReserveLightL returned error %d"), iErrVal);	
		}
    }
    delete iMyLight;    	
    return iErrVal;
    
    }
    
    
// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Display_on_and_keyboard_blink_S
// Light usage with two applications
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Display_on_and_keyboard_blink_S()
    {
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Display_on_and_keyboard_blink_S()"));
    TInt iErrVal = KErrNone;
    CHWRMLight::TLightStatus currKeybStatus;
    CHWRMLight::TLightStatus currDispStatus;    
    CHWRMLight::TLightStatus origKeybStatus;
    CHWRMLight::TLightStatus origDispStatus;   
        
    TRAP(iErrVal, iMyLight = CHWRMLight::NewL(this));	    
    
    if (iErrVal == KErrNone)
    {
    
	    origKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
        if(origKeybStatus == CHWRMLight::ELightStatusUnknown)
            {
            origKeybStatus = CHWRMLight::ELightOff;
            }
        
	    origDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);        
        if(origDispStatus == CHWRMLight::ELightStatusUnknown)
            {
            origDispStatus = CHWRMLight::ELightOff;
            }
	    
	    //reserve light
		TRAP(iErrVal, iMyLight->ReserveLightL(CHWRMLight::EPrimaryKeyboard, ETrue, ETrue));
		
		if (iErrVal == KErrNone)
		{
			iErrVal = TestLightBlink(CHWRMLight::EPrimaryKeyboard, 10000, 1000, 1000, 100);
			
			if (iErrVal == KErrNone)
			{
				currKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
				currDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
				
				if(currKeybStatus == CHWRMLight::ELightBlink && currDispStatus == origDispStatus)
				{
					/* Step executed succesfully */
					
					iMcFramework->StepExecuted(KErrNone);
					iErrVal = iMcFramework->WaitNextTestStep();			
				
					if (iErrVal == KErrNone)
					{
						
						currKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
						currDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);					
						
						if(currKeybStatus == CHWRMLight::ELightBlink /*&& currDispStatus == CHWRMLight::ELightOn*/) //SELVITÄ ONKO OK?
						{
									
							CTestScheduler::StartAndStopL(5000);
							
							iMyLight->ReleaseLight(CHWRMLight::EPrimaryKeyboard); // Muutos 22.6. TeK
												
							currKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
							currDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);									
							
							if(currKeybStatus == origKeybStatus /*&& currDispStatus == origDispStatus*/)
							{				
								
								//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()
								iErrVal = KErrNone;
							}
							else
							{
								FormatMessageL(EHwrmMessageOpDebug,_L("Wrong status Display:%d, Keyboard:%d"),currDispStatus, currKeybStatus);
								iErrVal = KErrGeneral;
							}
							
							
						}
						else
						{
							FormatMessageL(EHwrmMessageOpDebug,_L("H\xd6\xd6\xd6Wrong status. Expected:%d, received:%d"),CHWRMLight::ELightBlink, currKeybStatus);
							iErrVal = KErrGeneral;
						}
						
					}
					else
					{
						// testi feilasi masterissa
						FormatMessageL(EHwrmMessageOpDebug,_L("Error from peer application:%d"),iErrVal);
					}	
				}
				else
				{
					// väärä status
					FormatMessageL(EHwrmMessageOpDebug,_L("Wrong status! Display:%d, Keyboard:%d"),currDispStatus, currKeybStatus);
					iErrVal = KErrGeneral;
				}		
			}
			else
			{
				// Blink failed
				FormatMessageL(EHwrmMessageOpDebug,_L("Set keyboard light blink failed:%d"),iErrVal);
			}		
		}
		else
		{
			// valon varaus epäonnistui
			FormatMessageL(EHwrmMessageOpDebug,_L("Light reservation failed"),iErrVal);
		}
    }
    
    delete iMyLight;    	
    return iErrVal;
    }


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Display_on_and_keyboard_reserved_S
// Light usage with two applications
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Display_on_and_keyboard_reserved_S()
    {
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Display_on_and_keyboard_reserved_S()"));
    TInt iErrVal = KErrNone;
    CHWRMLight::TLightStatus currKeyStatus;    
    CHWRMLight::TLightStatus currDispStatus;
    CHWRMLight::TLightStatus origKeybStatus;
    CHWRMLight::TLightStatus origDispStatus;
    
    TRAP(iErrVal, iMyLight = CHWRMLight::NewL(this));	    
    
    if (iErrVal == KErrNone)
    {
    
		origKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	    origDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	    
	    //reserve light
	    TRAP(iErrVal, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard, ETrue, EFalse));
	    
	    if(iErrVal != KErrNone /*== KErrNotReady*/)
	    {
	    	// Reservation cannot succeed because trying to reserve while on background
	    	
	    	iErrVal = TestLightBlink(CHWRMLight::EPrimaryDisplayAndKeyboard);
	    	
	    	if(iErrVal != KErrNone /*== KErrInUse*/ )
	    	{
	    		// Cannot set light because foreground application has reserved display and keyboard
	    		
				currKeyStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	    		currDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	    		
	    		if ( origKeybStatus == currKeyStatus && origDispStatus == currDispStatus ) 
	    		{
	    			// Status has not changed -> OK
	    			
		    		CTestScheduler::StartAndStopL(2000);
		    		iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);
	    			//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()			
	    			iErrVal = KErrNone;
	    		}
	    		else
	    		{
	    			// Status has changed -> error
	    			FormatMessageL(EHwrmMessageOpDebug,_L("Unexpected status change :From:%d,%d To:%d,%d"),origKeybStatus, origDispStatus, currKeyStatus, currDispStatus);
	    			iErrVal = KErrGeneral;	
	    		}
	    	}
	    	else
	    	{
	    		// Test blink antoi väärän errorin
	    		FormatMessageL(EHwrmMessageOpDebug,_L("Wrong error from peer application:%d"),iErrVal);	
	    		
	    	}	
	    }
	    else
	    {
	    	// varaus epäonnistui
	    	FormatMessageL(EHwrmMessageOpDebug,_L("Reservation failed reason:%d"),iErrVal);
	    }
    }
    
    delete iMyLight;    	
    return iErrVal;
    }

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Display_and_keyboard_blink_S
// Light usage with two applications
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Display_and_keyboard_blink_S()
	{
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Display_and_keyboard_blink_S()"));
    TInt iErrVal = KErrNone;
    CHWRMLight::TLightStatus currKeyStatus(CHWRMLight::ELightBlink);    
    CHWRMLight::TLightStatus currDispStatus(CHWRMLight::ELightBlink);
    CHWRMLight::TLightStatus origKeyStatus;    
    CHWRMLight::TLightStatus origDispStatus;
    
    TRAP(iErrVal, iMyLight = CHWRMLight::NewL(this));	    
    
    if (iErrVal == KErrNone)
    {
		
		CTestScheduler::StartAndStopL(1000);
		
	    origKeyStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	    origDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	    
	    //reserve light
	    TRAP(iErrVal, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard, ETrue, EFalse));
	    
	    if(iErrVal == KErrNotReady) // was KErrone
	    {
	    	iErrVal = TestLightOn(CHWRMLight::EPrimaryDisplayAndKeyboard, 5000, 75, EFalse);
	    	
	    	if(iErrVal == KErrInUse)
	    	{
	    		CTestScheduler::StartAndStopL(2000);
	    		
	    		// Tarkista valo
	    		currKeyStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	    		currDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	    		
	    		/* Light should be same as at the beginning */
	    		
	    		if (origKeyStatus == currKeyStatus && origDispStatus == currDispStatus)
	    		{
		    		iMcFramework->StepExecuted(KErrNone);
		    		
		    		iErrVal = iMcFramework->WaitNextTestStep();
		    		
		    		if (iErrVal == KErrNone)
		    		{
		    			// Light status should be now ON
		    		
		    			currKeyStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	    				currDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	    				
	    				if (currKeyStatus == CHWRMLight::ELightOn && currDispStatus == CHWRMLight::ELightOn)
	    				{
	    					// status is OK
		    				iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);
		    				//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()
		    				iErrVal = KErrNone;	    				
	    				}
	    				else
	    				{
	    					// Display status has not restóred to original values
	    					FormatMessageL(EHwrmMessageOpDebug,_L("Unexpected status. Keyboard:%d Display:%d"),currKeyStatus, currDispStatus);
	    					iErrVal = KErrGeneral;
	    					
	    				}
		    		
		    		}
		    		else
		    		{
		    		    // Test failed peer
		    		    FormatMessageL(EHwrmMessageOpDebug,_L("Test failed failed peer:%d"),iErrVal);
		    		}    			
	    		}
	    		else
	    		{
	    			// light status has changed for some reason???
	    			// it should not because this client is not allowed to do changes in light
	    			// and client that has reserved light has not ordered any change in light  
	    			FormatMessageL(EHwrmMessageOpDebug,_L("Unexpected status. Keyboard:%d Display:%d"),currKeyStatus, currDispStatus);  			
	    			iErrVal = KErrGeneral;
	    		}
	    	}
	    	else
	    	{
	    		// Light on epäonnistui
	    		FormatMessageL(EHwrmMessageOpDebug,_L("Unexpected status. Keyboard:%d Display:%d"),currKeyStatus, currDispStatus); 	
	    		iErrVal = KErrGeneral;
	    	}	
	    }
	    else
	    {
	    	// varaus epäonnistui
	    	FormatMessageL(EHwrmMessageOpDebug,_L("Reservation failed:%d"),iErrVal);
	    }
    }
    
    delete iMyLight;    	
    return iErrVal;
	}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Vibra_Basic_Usage_S
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Vibra_Basic_Usage_S()
{
    
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_Vibra_Basic_Usage_S()"));
    TInt iErrVal = KErrNone;
    CHWRMVibra::TVibraStatus vibraStatus;

    TRAP(iErrVal, iVibra = CHWRMVibra::NewL());

    if (iErrVal == KErrNone)
    {
	    /*****************/
	    /* Reserve Vibra */
	    /*****************/
	    TRAP(iErrVal, iVibra->ReserveVibraL(ETrue, ETrue));
	    
	    if (iErrVal == KErrNone)
	    {   	
	    	/*************************/
	    	/* S T A R T   V I B R A */
	    	/*************************/
	    	iErrVal = RunTestVibra(2000); // Run vibra 2s
	    	
	    	if (iErrVal == KErrNone)
	    	{	    	
	    		vibraStatus = iVibra->VibraStatus();
	    			    			
	    		if(vibraStatus == CHWRMVibra::EVibraStatusOn /* || vibraStatus == CHWRMVibra::EVibraStatusUnknown*/)
	    		{
	    			CTestScheduler::StartAndStopL(2000);	
	 				
	 				vibraStatus = iVibra->VibraStatus();
	 				
	 				if(vibraStatus == CHWRMVibra::EVibraStatusStopped)
	 				{
	 					/*****************/
	 					/* Step executed */
	 					/*****************/
		    			iMcFramework->StepExecuted(KErrNone);
		    			iErrVal = iMcFramework->WaitNextTestStep(); 					

	    				/*************************/
	    				/* S T A R T   V I B R A */
	    				/*************************/
			    		iErrVal = RunTestVibra(5000); // Run vibra 5s
			    		if (iErrVal == KErrNone)
			    		{
			    			// Vibra start OK
			    			// Check status
			    			vibraStatus = iVibra->VibraStatus();
			    			if(vibraStatus == CHWRMVibra::EVibraStatusOn)
			    			{
	 							/*****************/
	 							/* Step executed */
	 							/*****************/
			    				iMcFramework->StepExecuted(KErrNone);
				    			iErrVal = iMcFramework->WaitNextTestStep(); 
				    			
				    			if (iErrVal == KErrNone)
				    			{
				    				// Check status
									vibraStatus = iVibra->VibraStatus();
			    					
			    					if(vibraStatus == CHWRMVibra::EVibraStatusOn)
			    					{
			    						//OK
			    						CTestScheduler::StartAndStopL(5000);
			    						
			    						vibraStatus = iVibra->VibraStatus();
			    						if(vibraStatus == CHWRMVibra::EVibraStatusStopped)
			    						{
			    							// Test is over --> release resources
	 										/*****************/
	 										/* Step executed */
	 										/*****************/
			    							//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()
			    									
			    							iErrVal = KErrNone;
			    						}
			    						else
			    						{
			    							// Wrong vibra status
			    							FormatMessageL(EHwrmMessageOpDebug,_L("Wrong vibra status5 (%d), expected STOPPED"),vibraStatus);				
			    							iErrVal = KErrGeneral;
			    						}
			    					}
			    					else
			    					{
			    						// Wrong status
			    						FormatMessageL(EHwrmMessageOpDebug,_L("Wrong vibra status4 (%d), expected ON"),vibraStatus);
			    						iErrVal = KErrGeneral;
			    					}
				    			}
				    			else
				    			{
				    				// test failed in master
				    				FormatMessageL(EHwrmMessageOpDebug,_L("Error from peer (%d)"),iErrVal);
				    			}
				    			
			    			}
			    			else
			    			{	
			    				// wrong status
			    				FormatMessageL(EHwrmMessageOpDebug,_L("Wrong vibra status3 (%d), expected ON"),vibraStatus);
			    				iErrVal = KErrGeneral;
			    			}
			    			
			    		}
			    		else
			    		{
			    			// Start vibra failed
			    			FormatMessageL(EHwrmMessageOpDebug,_L("Vibra start failed (%d)"),iErrVal);	
			    			
			    		}			
	 				}
	 				else
	 				{
	 					// Wrong status
	 					FormatMessageL(EHwrmMessageOpDebug,_L("Wrong vibra status2 (%d), expected STOPPED"),vibraStatus);
	 					iErrVal = KErrGeneral;
	 				}
	    		}
	    		else
	    		{
	    			// Wrong vibra status
	    			FormatMessageL(EHwrmMessageOpDebug,_L("Wrong vibra status1 (%d), expected ON"),vibraStatus);
	    			iErrVal = KErrGeneral;	
	    		}
	    	}
	    	else
	    	{
	    		FormatMessageL(EHwrmMessageOpDebug,_L("Vibra start failed (%d)"),iErrVal);
	    
	    	}
	    	
	    	/*****************/
	    	/* Release vibra */
	    	/*****************/
	    	iVibra->ReleaseVibra();	
	    }
	    else
	    {
	    	// Reserve failed
	    	FormatMessageL(EHwrmMessageOpDebug,_L("Vibra reservation failed (%d)"),iErrVal);
	    	
	    }
    }
    delete iVibra; iVibra = NULL;
    return iErrVal;
    
}


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Combined_light_and_vibra_usage_S
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Combined_light_and_vibra_usage_S()
{

    TInt iErrVal = KErrNone;
    
    // CRepository* iSession;
	CHWRMVibra::TVibraStatus vibraStatus;

    // iSession = CRepository::NewL(KCRUidVibraCtrl);
    iSession->Set(KVibraCtrlProfileVibraEnabled, 1);
    
    TRAP(iErrVal, iVibra = CHWRMVibra::NewL());
    
    if (iErrVal == KErrNone)
    {
	    // Check status
	    vibraStatus = iVibra->VibraStatus();
	    
	    if(vibraStatus == CHWRMVibra::EVibraStatusOn)
	    {
	      /*************************/
	      /* S T A R T   V I B R A */
	      /*************************/
	      iErrVal = RunTestVibra(2000);
	      
	      if(iErrVal == KErrInUse)
	      {
	      	/* Error is expected because another application has reserved vibra */
	      	
	      	CTestScheduler::StartAndStopL(2000);
	      	
	      	// Check status
	      	vibraStatus = iVibra->VibraStatus();
	      	
	      	if(vibraStatus == CHWRMVibra::EVibraStatusOn)
	      	{
	 			/*****************/
	 			/* Step executed */
	 			/*****************/
	      		//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()
	      		iErrVal = KErrNone;	
	      	}
	      	else
	      	{
	      	  FormatMessageL(EHwrmMessageOpDebug,_L("Wrong vibra status (%d)"),vibraStatus);
	      	  iErrVal = KErrGeneral;
	      	}
	      	
	      }
	      else
	      {
	      	// Wrong status
	      	FormatMessageL(EHwrmMessageOpDebug,_L("Wrong status(%d). Expected KErrInUse"),iErrVal);
	      }
	    }
	    else
	    {
	      FormatMessageL(EHwrmMessageOpDebug,_L("Wrong vibra status (%d)"),vibraStatus);  
	      iErrVal = KErrGeneral;
	    }
    }
    delete iVibra; iVibra = NULL;

    return iErrVal;

}

// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_Combined_light_and_vibra_usage_2_S
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_Combined_light_and_vibra_usage_2_S()
{
    TInt iErrVal = KErrNone;   
    // CRepository* iSession;
    CHWRMVibra::TVibraStatus vibraStatus;
    CHWRMLight::TLightStatus dispStatus;
    CHWRMLight::TLightStatus keybStatus;    
    //CHWRMLight::TLightStatus origDispStatus;
    //CHWRMLight::TLightStatus origKeybStatus;

    // iSession = CRepository::NewL(KCRUidVibraCtrl);
    iSession->Set(KVibraCtrlProfileVibraEnabled, 1);
    
    TRAP(iErrVal, iVibra = CHWRMVibra::NewL());
    if(iErrVal == KErrNone)
    {
    
	    TRAP(iErrVal, iMyLight = CHWRMLight::NewL(this));
		
		if(iErrVal == KErrNone)
		{
		
			//Reserve Primary display and keyboard

			//origDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
			//origKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
			vibraStatus = iVibra->VibraStatus();
			
			TRAP(iErrVal, iVibra->ReserveVibraL(ETrue, ETrue));
		    
		    if (iErrVal == KErrNone)
		    {	
				TRAP(iErrVal, iMyLight->ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard, ETrue, ETrue));
			
				if(iErrVal == KErrNone)
				{
					CTestScheduler::StartAndStopL(5000);
					
					/*******************/
					/* L I G H T   O N */
					/*******************/
					
					iErrVal = TestLightOn(CHWRMLight::EPrimaryDisplayAndKeyboard);
					
					if (iErrVal == KErrNone)
					{
						keybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
		    			dispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
		    			
		    			if (keybStatus == CHWRMLight::ELightOn && dispStatus == CHWRMLight::ELightOn)
		    			{
		    				CTestScheduler::StartAndStopL(5000);
		    				
		    				/*******************/
		    				/* V I B R A   O N */
		    				/*******************/
		    				
		    				iErrVal = RunTestVibra(5000,75);
		    				
		    				if(iErrVal == KErrNone)
		    				{
		    					vibraStatus = iVibra->VibraStatus();
		    					
		    					if (vibraStatus == CHWRMVibra::EVibraStatusOn)
		    					{
		    						CTestScheduler::StartAndStopL(5000);
		    						
		    						vibraStatus = iVibra->VibraStatus();
		    						
		    						if(vibraStatus == CHWRMVibra::EVibraStatusStopped)
		    						{
		    							
		    							/*****************/
		    							/* Release vibra */
		    							/*****************/
		    							iVibra->ReleaseVibra();
		    							
		    							/*****************/
		    							/* Release light */
		    							/*****************/
		    							iMyLight->ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);	    							
		    							
		    							/*****************/
		    							/* Step executed */
		    							/*****************/							    							
		    							//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()
		    							
		    							iErrVal = KErrNone;
		    						}
		    						else
		    						{
		    							// Wrong vibra status
		    							FormatMessageL(EHwrmMessageOpDebug,_L("Wrong status(%d). Expected EVibraStatusStopped"), vibraStatus);
		    							iErrVal = KErrGeneral;
		    						}	
		    					}
		    					else
		    					{
		    						// wrong vibra status
		    						FormatMessageL(EHwrmMessageOpDebug,_L("Wrong status(%d). Expected EVibraStatusOn"), vibraStatus);
		    						iErrVal = KErrGeneral;
		    					}    							
		    				}
		    				else
		    				{
		    					// start vibra failed
		    					FormatMessageL(EHwrmMessageOpDebug,_L("Vibra start failed(%d)."), iErrVal);
		    				}
		    			}
		    			else
		    			{
		    				// wrong status
		    				FormatMessageL(EHwrmMessageOpDebug,_L("Wrong status keyboard(%d) display(%d)"), keybStatus, dispStatus);
		    				iErrVal = KErrGeneral;
		    			}
					}
					else
					{
						// Set light on failed
						FormatMessageL(EHwrmMessageOpDebug,_L("Light on failed(%d)"), iErrVal);
					}
				}
				else
				{
					// Reserve light failed
					FormatMessageL(EHwrmMessageOpDebug,_L("Reserve light failed(%d)"), iErrVal);		
				}
		    }
		    else
		    {
		    	// Reserve vibra failed
		    	FormatMessageL(EHwrmMessageOpDebug,_L("Reserve vibra failed(%d)"), iErrVal);
		    }
		}
    }
    delete iVibra; iVibra = NULL;
    delete iMyLight; iMyLight = NULL;
    return iErrVal;	
	
}


// -----------------------------------------------------------------------------
// CHwrmTestCaseController::MC_FmTx_Basic_Usage_S
// 
// -----------------------------------------------------------------------------
//
TInt CHwrmTestCaseController::MC_FmTx_Basic_Usage_S()
{    
    FormatMessageL(EHwrmMessageOpDebug,_L("MC_FmTx_Basic_Usage_S()"));
    TInt iErrVal = KErrNone;
    TFmTxState fmTxState;

    TRAP(iErrVal, iFmTx = CHWRMFmTx::NewL(this));

    if (iErrVal == KErrNone)
    	{	    
    	//Reserve FmTx, Don't force CoeEnv "on" 
	    TRAP(iErrVal, iFmTx->ReserveL(ETrue));
	    
	    if (iErrVal == KErrNone)
	    	{   
			// Reservation was OK
	    	
	    	// Enable FmTx
	    	iErrVal = RunTestFmTx(89900); //89900 KHz
	    		    	
	    	if (iErrVal == KErrNone)
	    		{	    	
				// Check FmTx status
	    		fmTxState = iFmTx->Status();
	    			    			
	    		if(fmTxState == EFmTxStateActive || fmTxState == EFmTxStateInactive)
	    			{
	    			/* FmTx status is OK */
	    			
	    			iMcFramework->StepExecuted(KErrNone);
	    			iErrVal = iMcFramework->WaitNextTestStep(); 					

		    		if (iErrVal == KErrNone)
		    			{
		    			/* Test execution was OK in peer-application */
		    			
		    			/* Check current FmTx status */
		    			fmTxState = iFmTx->Status();
		    			
		    			if( fmTxState == EFmTxStateInactive
		    			   || fmTxState == EFmTxStateActive)
		    				{
		    				/* OK, status is Active or Inactive */
		    				/*****************/
		    				/* Step executed */
		    				/*****************/
		    				iFmTx->Release();		    				
			    			
		    				}
		    			else
		    				{	
		    				// wrong status
		    				FormatMessageL(EHwrmMessageOpDebug,_L("Wrong FmTx status2 (%d), expected ON"),fmTxState);
		    				iErrVal = KErrGeneral;
		    				}
		    			
		    			}
		    		else
		    			{
		    			// Start FmTx failed
		    			FormatMessageL(EHwrmMessageOpDebug,_L("FmTx enable failed (%d)"),iErrVal);			    			
		    			}			
 					}
	    		else
	    			{
	    			// Wrong FmTx status
	    			FormatMessageL(EHwrmMessageOpDebug,_L("Wrong FmTx status1 (%d), expected ON"),fmTxState);
	    			iErrVal = KErrGeneral;	
	    			}
	    		}
	    	else
	    		{
	    		FormatMessageL(EHwrmMessageOpDebug,_L("FmTx enable failed (%d)"),iErrVal);	    
	    		}
	    	}
	    else
	    	{
	    	// Reserve failed
	    	FormatMessageL(EHwrmMessageOpDebug,_L("FmTx reservation failed (%d)"),iErrVal);
	    	
	    	}
    	}
    delete iFmTx; 
    iFmTx = NULL;
    return iErrVal;
    
	}

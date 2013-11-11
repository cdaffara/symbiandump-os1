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

#ifndef TESTCASECONTROLLER_H
#define TESTCASECONTROLLER_H

#include <e32base.h>
#include <e32def.h>
#include <eikenv.h>
#include <hwrmlight.h>
#include <hwrm/hwrmextendedlight.h>
#include <hwrm/hwrmenhancedlight.h>
#include <hwrmvibra.h>
#include <hwrm/hwrmfmtx.h>
#include "McFramework.h"
#include "tc_log.h"
#include "testcaseids.h"

#define TEST_TRACE( a ) RDebug::Print a 

class CMcFramework;
class CRepository;

class MHwrmTestCaseObserver
	{
public:
	virtual void InfoMessage(const TDesC& aMessage) =0;
	virtual void LogMessage(const TDesC& aMessage) 	=0;
	virtual void DebugMessage(const TDesC& aMessage) =0;
	};
	
class CHwrmTestCaseController : public CBase, 
								public MHWRMLightObserver,
								public MHWRMFmTxObserver
    {
public:
	static CHwrmTestCaseController* NewL(MHwrmTestCaseObserver& aObserver);
	~CHwrmTestCaseController();	
	
	
public:
	void SetApiType(THWRMLightTestFAPIType aApiType);
	void RunTestCases(TInt aCase);
    void RunTestCases(TInt aCasesFrom, TInt aCasesTo);
	void RunBasicTestCases(THWRMLightTestFAPIType aApiType);
	void RunMulticlientTestCases();
	
	void RunSlaveMode(); // background
	
private:
	CHwrmTestCaseController(MHwrmTestCaseObserver& aObserver);
	void ConstructL();
	
	enum THwrmMessageOp
		{
		EHwrmMessageOpInfo = 0,
		EHwrmMessageOpLog,
		EHwrmMessageOpDebug
		};
		 
private:
	void UpdateSupportedTargets();
	
	void LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus);
	void VibraStatusChanged( CHWRMVibra::TVibraStatus aStatus);
	void VibraModeChanged( CHWRMVibra::TVibraModeState aStatus);
	void FmTxStatusChanged( TFmTxState aState);
	void FmTxFrequencyChanged( TInt aFrequency);
	

    void LightTargetTest( TInt aTargetMask );
    TInt LightTargetTestSDK( TInt aTarget );
    TInt LightTargetTestExtended( TInt aTarget );

    void LogResult( const TDesC& aText );
	void LogResult(const TDesC& aTestCase, TInt aErrorCode);
	void LogResult( const TDesC& aTestCase, TInt aErrorCode, TInt aExpected );
	
	TBool TargetSupported( TInt aTarget );

	void LightTestAllSupportedSDKTargets( TInt aTargetsToBeSkipped );
	void LightTestAllSupportedExtendedTargets( TInt aTargetsToBeSkipped );



   	TInt GetLightStatusForSystemTarget( CHWRMLight* aLight, CHWRMLight::TLightStatus aStatus );
	TInt GetLightStatusForSystemTarget( CHWRMExtendedLight* aLight, CHWRMExtendedLight::TLightStatus aStatus );


	TInt MC_Display_RGB_M();
    TInt MC_Primary_display_light_M();
    TInt MC_Display_on_and_keyboard_blink_M();
    TInt MC_Display_on_and_keyboard_reserved_M();
    TInt MC_Display_and_keyboard_blink_M();
    TInt MC_Vibra_Basic_Usage_M();
    TInt MC_Combined_light_and_vibra_usage_M();
    TInt MC_Combined_light_and_vibra_usage_2_M();
    TInt MC_FmTx_Basic_Usage_M();
    TInt MaliciousVibra();
    TInt ChargingNotify();
    TInt RunRepeatedlyInvalidValues();
    TInt TestLightOn(TInt aTarget, TInt aDur, TInt aIntens, TBool aFadeIn);
    TInt TestLightOn(TInt aTarget);	
    TInt TestLightOnColor(TInt aTarget, TInt aDur, THWRMLightColor& aColor);
	TInt Verify_RGBColorValue(THWRMLightColor& aColor);
    TInt TestLightOff(TInt aTarget, TInt aDuration, TBool aFadeOut);	
    TInt TestLightOff(TInt aTarget);		
    TInt TestLightBlink(TInt aTarget,TInt aDuration,TInt aOnDuration,TInt aOffDuration,TInt aIntensity);
    TInt TestLightBlink(TInt aTarget,TInt aDuration);
    TInt TestLightBlink(TInt aTarget);
    TInt Verify_MCFramework_M();
    TInt RunTestVibra(TInt howLong, TInt whatIntensity);
	TInt RunTestVibra(TInt howLong);
	TInt StopTestVibra();
	TInt RunTestFmTx( TInt aFrequency );
	TInt StopTestFmTx();

//
// BACKGROUND
	
	TInt MC_Display_RGB_S();
	TInt MC_Primary_display_light_S();
	TInt MC_Display_on_and_keyboard_blink_S();
	TInt MC_Display_on_and_keyboard_reserved_S();
	TInt MC_Display_and_keyboard_blink_S();
	TInt MC_Vibra_Basic_Usage_S();	
    TInt MC_Combined_light_and_vibra_usage_S();
    TInt MC_Combined_light_and_vibra_usage_2_S();
    TInt MC_FmTx_Basic_Usage_S();
	//TInt TestLightOn(TInt aTarget, TInt aDur, TInt aIntens, TBool aFadeIn);
	//TInt TestLightOn(TInt aTarget);	
	//TInt TestLightOff(TInt aTarget, TInt aDuration, TBool aFadeOut);	
	//TInt TestLightOff(TInt aTarget);		
	//TInt TestLightBlink(TInt aTarget,TInt aDuration,TInt aOnDuration,TInt aOffDuration,TInt aIntensity);
	//TInt TestLightBlink(TInt aTarget,TInt aDuration);
	//TInt TestLightBlink(TInt aTarget);
	TInt Verify_MCFramework_S();	
    //TInt RunTestVibra(TInt howLong, TInt whatIntensity);
	//TInt RunTestVibra(TInt howLong);
	//TInt StopTestVibra();
// END BACKGROUND
//
   	void LightOnDurationTest( TInt aTargetMask, TBool aIsRGBTest = EFalse, TInt aRed = 0, TInt aGreen = 0, TInt aBlue = 0 );
    TInt LightOnDurationTestSDK( TInt aTarget, TBool aIsRGBTest = EFalse, TInt aRed = 0, TInt aGreen = 0, TInt aBlue = 0 );
	TInt LightOnDurationTestExtended( TInt aTarget );
	TInt LightOnDurationTestEnhanced( TInt aTarget, TBool aIsRGBTest = EFalse, TInt aRed = 0, TInt aGreen = 0, TInt aBlue = 0 );

   	void LightOffDurationTest( TInt aTargetMask );
    TInt LightOffDurationTestSDK( TInt aTarget );
    TInt LightOffDurationTestExtended( TInt aTarget );

    void LightIntensityTest( TInt aTargetMask );
    TInt LightIntensityTestSDK( TInt aTarget );
    TInt LightIntensityTestExtended( TInt aTarget );

    void LightBlinkTest( TInt aTargetMask );
    TInt LightBlinkTestSDK( TInt aTarget );
    TInt LightBlinkTestExtended( TInt aTarget );

    void LightBlinkDurationTest( TInt aTargetMask, TBool aIsRGBTest = EFalse, TInt aRed = 0, TInt aGreen = 0, TInt aBlue = 0 );
    TInt LightBlinkDurationTestSDK( TInt aTarget, TBool aIsRGBTest = EFalse, TInt aRed = 0, TInt aGreen = 0, TInt aBlue = 0 );
    TInt LightBlinkDurationTestExtended( TInt aTarget );
    TInt LightBlinkDurationTestEnhanced( TInt aTarget, TBool aIsRGBTest = EFalse, TInt aRed = 0, TInt aGreen = 0, TInt aBlue = 0 );
	
    void LightBlinkOnOffTest( TInt aTargetMask );
    TInt LightBlinkOnOffTestSDK( TInt aTarget );
    TInt LightBlinkOnOffTestExtended( TInt aTarget );
	TInt LightWithDifferentTargetsSDK();
    TInt LightWithDifferentTargetsExtended();
	TInt LightWithInvalidDurationSDK();
    TInt LightWithInvalidDurationExtended();
 	
 	TInt LightIntensitySDK();
 	TInt LightIntensityExtended();

    //TInt LightWithInvalidTarget();
   	 		 
   	// logging
   	void FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage);
   	void FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, TInt aArg1);
   	void FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, TInt aArg1, TInt aArg2);
   	void FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, TInt aArg1, TInt aArg2, TInt aArg3);
   	void FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, TInt aArg1, TInt aArg2, TInt aArg3, TInt aArg4);
   	void FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, const TDesC* aArg1);
   	void FormatMessageL(THwrmMessageOp aOp, const TDesC& aMessage, const TDesC* aArg1, TInt aArg2, TInt aArg3);
   	void SendMessage(THwrmMessageOp aOp, const TDesC& aMessage);
   	
private:
	MHwrmTestCaseObserver& iObserver;
	
	//variable to store type of API (Extended or SDK) used in cases
	THWRMLightTestFAPIType iApiType;
	
	CRepository* iSession;
	TInt iErrCode;
	CHWRMLight* iMyLight;
	CHWRMLight::TLightStatus iMyStatus;
	TInt iSupportedTargets;
	
    CMcFramework* iMcFramework;
    CHWRMVibra* iVibra;
    CHWRMVibra::TVibraStatus iVibraStatus;
    CHWRMVibra::TVibraModeState iVibraMode;
    
	CHWRMFmTx* iFmTx;
	TFmTxState iFmTxState;        
    };
    
#endif

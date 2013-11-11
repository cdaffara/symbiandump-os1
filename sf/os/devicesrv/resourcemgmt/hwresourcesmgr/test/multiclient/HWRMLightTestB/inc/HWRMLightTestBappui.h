/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/




#ifndef __HWRMLightTestB_APPUI_H__
#define __HWRMLightTestB_APPUI_H__

#include <eikappui.h>
//#include <hwrmlight.h>
//#include <hwrmvibra.h>
#include "TestScheduler.h"
#include "tc_log.h"
//#include "McFramework.h"
#include <centralrepository.h>
//#include <hwrmvibrasdkcrkeys.h>
#include "testcasecontroller.h"

class CHWRMLightTestBAppView;

/*! 
  @class CHWRMLightTestBAppUi
  
  @discussion An instance of class CHWRMLightTestBAppUi is the UserInterface part of the AVKON
  application framework for the HWRMLightTestB example application
  * * Nokia Core OS *
  */
class CHWRMLightTestBAppUi : public CEikAppUi, public MHwrmTestCaseObserver
    {
public:
	~CHWRMLightTestBAppUi();
	CHWRMLightTestBAppUi();
	
private:	
    void ConstructL();
    
private: // from CAknAppUi
    void HandleCommandL(TInt aCommand);

	//void LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus);

	//void VibraStatusChanged( CHWRMVibra::TVibraStatus aStatus);
	
	//void VibraModeChanged( CHWRMVibra::TVibraModeState aStatus);
	
	//CRepository* iSession;

private: // from MHwrmTestCaseObserver
	void InfoMessage(const TDesC& aMessage);
	void LogMessage(const TDesC& aMessage);
	void DebugMessage(const TDesC& aMessage);
	
private:
	CHwrmTestCaseController* iTestCaseController;

    CHWRMLightTestBAppView* iAppView;

    //TInt iMyTarget;

    //CHWRMLight::TLightStatus iMyStatus;
    
    //CHWRMLight* iMyLight;
       
    //TInt iErrVal;
    
    Log iLog;
    
    void InitStatus();
    
//    void RunTestCases(TInt aCasesFrom, TInt aCasesTo);
		void LogResult(const TDesC& aTestCase, TInt aErrorCode);

	/* TeK -> */
//	CMcFramework* iMcFramework;
//	CHWRMVibra* iVibra;
//    CHWRMVibra::TVibraStatus iVibraStatus;
//    CHWRMVibra::TVibraModeState iVibraMode;
    	
   /* 	
	void RunSlaveMode();
	TInt MC_Primary_display_light_S();
	TInt MC_Display_on_and_keyboard_blink_S();
	TInt MC_Display_on_and_keyboard_reserved_S();
	TInt MC_Display_and_keyboard_blink_S();
	TInt MC_Vibra_Basic_Usage_S();	
	TInt MC_FmTx_Basic_Usage_S();
    TInt MC_Combined_light_and_vibra_usage_S();
    TInt MC_Combined_light_and_vibra_usage_2_S();
	TInt TestLightOn(TInt aTarget, TInt aDur, TInt aIntens, TBool aFadeIn);
	TInt TestLightOn(TInt aTarget);	
	TInt TestLightOff(TInt aTarget, TInt aDuration, TBool aFadeOut);	
	TInt TestLightOff(TInt aTarget);		
	TInt TestLightBlink(TInt aTarget,TInt aDuration,TInt aOnDuration,TInt aOffDuration,TInt aIntensity);
	TInt TestLightBlink(TInt aTarget,TInt aDuration);
	TInt TestLightBlink(TInt aTarget);
	TInt Verify_MCFramework_S();	
    TInt RunTestVibra(TInt howLong, TInt whatIntensity);
	TInt RunTestVibra(TInt howLong);
	TInt StopTestVibra();
*/	
//TEST CASES
/*
    TInt LightTargetDisplayTest();
    TInt LightTargetKeyboardTest();
    TInt LightOnDurationTest();
    TInt LightOffDurationTest();
    TInt LightIntensityTest();
    TInt LightBlinkTargetTest( );
    TInt LightBlinkDurationTest( );
    TInt LightBlinkOnOff();
	TInt LightWithDifferentTargets();
	TInt LightWithInvalidDuration();

    //TInt LightWithInvalidTarget();
*/		
    };


#endif // __HWRMLightTestB_APPUI_H__


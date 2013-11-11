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



#include <techview/eikon.hrh>
#include <e32property.h> 

#include <e32base.h>
#include <eikenv.h>
#include <eikdialg.h>

#include "HWRMLightTestB.pan"
#include "HWRMLightTestBappui.h"
#include "HWRMLightTestBappview.h"
#include "HWRMLightTestB.hrh"

#include "testcasecontroller.h"

_LIT(KDebugPrefix,"HWRMLightTestB # ");

void CHWRMLightTestBAppUi::ConstructL()
    {
    BaseConstructL();
	//iSession = CRepository::NewL(KCRUidVibraCtrl);
    iAppView = CHWRMLightTestBAppView::NewL(ClientRect());
    AddToStackL(iAppView);
    
    iTestCaseController = CHwrmTestCaseController::NewL(*this);
    }

CHWRMLightTestBAppUi::CHWRMLightTestBAppUi()                              
    {
    }

CHWRMLightTestBAppUi::~CHWRMLightTestBAppUi()
    {
    if (iAppView)
        {
        iEikonEnv->RemoveFromStack(iAppView);
        delete iAppView;
        iAppView = NULL;
        
        }
    //delete iSession;
    delete iTestCaseController;
    }

void CHWRMLightTestBAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
       // case EAknSoftkeyExit:
            Exit();
            break;

        case EHWRMLightTestBCommand1:
            {

			
			// Create multiclient test framework instance, create P&S-properties and attach to it 			
			//iMcFramework = iMcFramework->NewL(); 
			
           
            iTestCaseController->RunSlaveMode();
            

            //_LIT(message,"TEST EXECUTED (Slave)");
            //CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            //CEikDialog* informationNote = new (ELeave) CEikDialog;
            //informationNote->ExecuteLD(message);

            }
            break;		

        default:
            Panic(EHWRMLightTestBUi);
            break;
        }
    }
    
void CHWRMLightTestBAppUi::InfoMessage(const TDesC& aMessage)
	{
	iEikonEnv->InfoMsg(aMessage);
	}

// FUTURE TASK: Refactor duplicate code in LogMessage and DebugMessage (could be a good refresher on descriptors...)
void CHWRMLightTestBAppUi::LogMessage(const TDesC& aMessage)
	{
	HBufC* dbgMsg(NULL);
	TRAPD(err,	dbgMsg = HBufC::NewL(KDebugPrefix().Length() + aMessage.Length()));
	if (err != KErrNone)
		return;
	
	CleanupStack::PushL(dbgMsg);
	
	dbgMsg->Des().Append(KDebugPrefix);
	dbgMsg->Des().Append(aMessage);
	
	iLog.Printf(*dbgMsg);
	
	CleanupStack::PopAndDestroy(dbgMsg);
	}
	
void CHWRMLightTestBAppUi::DebugMessage(const TDesC& aMessage)
	{

	HBufC* dbgMsg(NULL);
	TRAPD(err,	dbgMsg = HBufC::NewL(KDebugPrefix().Length() + aMessage.Length()));
	if (err != KErrNone)
		return;
	
	CleanupStack::PushL(dbgMsg);
	
	dbgMsg->Des().Append(KDebugPrefix);
	dbgMsg->Des().Append(aMessage);
	
	RDebug::Print(*dbgMsg);
	
	CleanupStack::PopAndDestroy(dbgMsg);
	}
    

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::RunTestCases
// Run test cases
// -----------------------------------------------------------------------------
//
#ifdef COMMENTED_OUTXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void CHWRMLightTestBAppUi::RunTestCases(TInt aCasesFrom, TInt aCasesTo)
{
	
	//iLog.Printf(_L("Running test cases from %d to %d.."), aCasesFrom, aCasesTo);
	TInt i;
	TInt errCode;
	
	for (i = aCasesFrom; i < aCasesTo; i++)
	{
		errCode = KErrNone;
		switch(i)
		{
			case 1:
			
				errCode = LightTargetDisplayTest( );
				LogResult(_L("HWRM [LightOnWithDisplayTarget] "), errCode);
  			
				break;
			
			case 2:
			
				errCode = LightTargetKeyboardTest();
				LogResult(_L("HWRM [LightOnWithKeyboardTarget] "), errCode);
			
				break;
			
			case 3:
			
				errCode = LightOnDurationTest();
				LogResult(_L("HWRM [LightOnWithDuration] "), errCode);
			
				break;
			
			case 4:
			
				errCode = LightOffDurationTest();
				LogResult(_L("HWRM [LightOffWithDuration] "), errCode);
			
				break;

			case 5:
			
				errCode = LightIntensityTest();
				LogResult(_L("HWRM [LightOnWithIntensity] "), errCode);
			
				break;

			case 6:
			
				errCode = LightBlinkTargetTest();
				LogResult(_L("HWRM [LightBlinkWithTarget] "), errCode);
			
				break;

			case 7:
			
				errCode = LightBlinkDurationTest();
				LogResult(_L("HWRM [LightBlinkWithDuration] "), errCode);
			
				break;
			case 8:
			
				errCode = LightBlinkOnOff();
				LogResult(_L("HWRM [LightBlinkOnOff] "), errCode);
			
				break;
			case 9:
			
				errCode = LightWithDifferentTargets();
				LogResult(_L("HWRM [LightWithDifferentTargets] "), errCode);
			
				break;
			
			case 10:
			
				errCode = LightWithInvalidDuration();
				LogResult(_L("HWRM [LightWithInvalidDuration] "), errCode);
			
				break;
			
			default:
				break;
			

		}
	}
	
	
}

/* TeK -> */

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::RunSlaveMode
//
// This method is called when application is started in SLAVE-mode
// 
// -----------------------------------------------------------------------------
//
void CHWRMLightTestBAppUi::RunSlaveMode()
{
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
				default:
					iMcFramework->StepExecuted(KErrGeneral);	
			}
		}
				
	}
}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::Verify_MCFramework_S
// Slave side test case to verify McFramework functionality 
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::Verify_MCFramework_S()
{
	
	RDebug::Print(_L("Verify_MCFramework_S() starting"));

	CTestScheduler::StartAndStopL(1000); // Testin suoritusta

	iMcFramework->StepExecuted(KErrNone); // palautetaan testistepin tulos
	TInt execStep = iMcFramework->WaitNextTestStep(); // mennään odottamaan seuraavaa steppiä
	
	CTestScheduler::StartAndStopL(1000); // Testin suoritus jatkuu
	
	//iMcFramework->StepExecuted(KErrNone); last step execution is reported in RunSlaveMode()
		  								
	RDebug::Print(_L("Verify_MCFramework_S() done"));
	
	return KErrNone;

}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::TestLightOn
// This method 
// blaa blaa
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::TestLightOn(TInt aTarget, TInt aDur, TInt aIntens, TBool aFadeIn)
{
	TInt errorStatus;			
	TRAP(errorStatus, iMyLight->LightOnL(aTarget, aDur, aIntens, aFadeIn));
	CTestScheduler::StartAndStopL(100);
	//iLog.Printf(_L("HWRMLightTestB ***Light ON*** Target:%d, Duration:%ds, Intensity %d, Fade:%d"), aTarget, aDur/1000, aIntens, aFadeIn);
	RDebug::Print(_L("HWRMLightTestB ***Light ON*** Target:%d, Duration:%ds, Intensity %d, Fade:%d"), aTarget, aDur/1000, aIntens, aFadeIn);
	return errorStatus;
}

TInt CHWRMLightTestBAppUi::TestLightOn(TInt aTarget)
{
	TInt errorStatus;			
	TRAP(errorStatus, iMyLight->LightOnL(aTarget));
	CTestScheduler::StartAndStopL(100);
	//iLog.Printf(_L("HWRMLightTestB ***Light ON*** Target:%d, Duration:infinite"), aTarget);
	RDebug::Print(_L("HWRMLightTestB ***Light ON*** Target:%d, Duration:infinite"), aTarget);
	return errorStatus;
}


// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::TestLightOff
// This method 
// blaa blaa
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::TestLightOff(TInt aTarget, TInt aDuration, TBool aFadeOut)
{
	TInt errorStatus;			
	TRAP(errorStatus, iMyLight->LightOffL(aTarget, aDuration, aFadeOut));
	CTestScheduler::StartAndStopL(100);
	//iLog.Printf(_L("HWRMLightTestB ***Light OFF*** Target:%d, Duration:%ds, Fadeout:%d"), aTarget, aDuration/1000, aFadeOut);
	RDebug::Print(_L("HWRMLightTestB ***Light OFF*** Target:%d, Duration:%ds, Fadeout:%d"), aTarget, aDuration/1000, aFadeOut);
	return errorStatus;
}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::TestLightBlink
// This method 
// blaa blaa
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::TestLightBlink(TInt aTarget, 
		                         		  TInt aDuration, 
		                         		  TInt aOnDuration, 
		                         		  TInt aOffDuration, 
		                         		  TInt aIntensity)
{
	TInt errorStatus;			
	TRAP(errorStatus, iMyLight->LightBlinkL(aTarget, aDuration, aOnDuration, aOffDuration, aIntensity));
	CTestScheduler::StartAndStopL(100);
	//iLog.Printf(_L("HWRMLightTestB ***Light BLINK*** Target:%d, Duration:%ds, On:%d Off:%d, Intensity:%d"), aTarget, aDuration/1000, aOnDuration, aOffDuration, aIntensity);
	RDebug::Print(_L("HWRMLightTestB ***Light BLINK*** Target:%d, Duration:%ds, On:%d Off:%d, Intensity:%d"), aTarget, aDuration/1000, aOnDuration, aOffDuration, aIntensity);
	return errorStatus;
}

TInt CHWRMLightTestBAppUi::TestLightBlink(TInt aTarget, TInt aDuration)
{
	TInt errorStatus;			
	TRAP(errorStatus, iMyLight->LightBlinkL(aTarget, aDuration));
	CTestScheduler::StartAndStopL(100);
	//iLog.Printf(_L("HWRMLightTestB ***Light BLINK*** Target:%d, Duration:%ds"), aTarget, aDuration/1000);
	RDebug::Print(_L("HWRMLightTestB ***Light BLINK*** Target:%d, Duration:%ds"), aTarget, aDuration/1000);
	return errorStatus;
}

TInt CHWRMLightTestBAppUi::TestLightBlink(TInt aTarget)
{
	TInt errorStatus;			
	TRAP(errorStatus, iMyLight->LightBlinkL(aTarget));
	CTestScheduler::StartAndStopL(100);
	//iLog.Printf(_L("HWRMLightTestB ***Light BLINK*** Target:%d"), aTarget);
	RDebug::Print(_L("HWRMLightTestB ***Light BLINK*** Target:%d"), aTarget);
	return errorStatus;
}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::RunTestVibra
// This method 
// - call LightBlinkL method with given parameters
// - Run 100ms delay
// - Print parameters to log 
// -----------------------------------------------------------------------------
//

TInt CHWRMLightTestBAppUi::RunTestVibra( TInt howLong, TInt whatIntensity)
{
	TInt errorStatus;	
	TRAP(errorStatus, iVibra->StartVibraL(howLong, whatIntensity));
	//iLog.Printf(_L("CHWRMLightTestBAppUi Vibra start for %d seconds with intensity %d Error:%d"), howLong/1000, whatIntensity, errorStatus);
	RDebug::Print(_L("CHWRMLightTestBAppUi Vibra start for %d seconds with intensity %d Error:%d"), howLong/1000, whatIntensity, errorStatus);
	CTestScheduler::StartAndStopL(100);
	return errorStatus;
}

TInt CHWRMLightTestBAppUi::RunTestVibra( TInt howLong)
{
	TInt errorStatus;	
	TRAP(errorStatus, iVibra->StartVibraL(howLong));
	//iLog.Printf(_L("CHWRMLightTestBAppUi Vibra start for %d seconds with default intensity Error:%d"), howLong/1000, errorStatus);
	RDebug::Print(_L("CHWRMLightTestBAppUi Vibra start for %d seconds with default intensity Error:%d"), howLong/1000, errorStatus);
	CTestScheduler::StartAndStopL(100);
	return errorStatus;
}


TInt CHWRMLightTestBAppUi::StopTestVibra()
{
	TInt errorStatus;		
	TRAP(errorStatus, iVibra->StopVibraL());
	//iLog.Printf(_L("CHWRMLightTestBAppUi Vibra stop called Error:%d"),errorStatus);
	RDebug::Print(_L("CHWRMLightTestBAppUi Vibra stop called Error:%d"),errorStatus);
	CTestScheduler::StartAndStopL(100);
	return errorStatus; 
}



// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::MC_Primary_display_light_S
// Light usage with two applications / This application is on the background
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::MC_Primary_display_light_S()
    {
    
    RDebug::Print(_L("MC_Primary_display_light_S()"));
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
								RDebug::Print(_L("HWRMLightTestB # Wrong light status %d. Expected original: %d"), currStatus, origStatus);
								iErrVal = KErrGeneral; 
							}
							
						}
						else
						{
							// wrong light status
							RDebug::Print(_L("HWRMLightTestB # Wrong light status:%d. Expected [Blink]"), currStatus);
							iErrVal = KErrGeneral; 
						}
					}
					else
					{
						// execution failed in peer-application
						RDebug::Print(_L("HWRMLightTestB # Execution failed peer. Error:%d"), iErrVal); 	
					}
				}
				else
				{
					RDebug::Print(_L("HWRMLightTestB # Wrong light status [%d]. Expected [Blink]"), currStatus);
					iErrVal = KErrGeneral; 
				}			
			}
			else
			{
				// Set light blinking failed
				RDebug::Print(_L("HWRMLightTestB # Set light blink Error:%d"),iErrVal);
			}
		}
		else
		{
			RDebug::Print(_L("HWRMLightTestB # ReserveLightL returned error %d"), iErrVal);	
		}
    }
    delete iMyLight;    	
    return iErrVal;
    
    }
    
    
// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::MC_Display_on_and_keyboard_blink_S
// Light usage with two applications
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::MC_Display_on_and_keyboard_blink_S()
    {
    RDebug::Print(_L("MC_Display_on_and_keyboard_blink_S()"));
    TInt iErrVal = KErrNone;
    CHWRMLight::TLightStatus currKeybStatus;
    CHWRMLight::TLightStatus currDispStatus;    
    CHWRMLight::TLightStatus origKeybStatus;
    CHWRMLight::TLightStatus origDispStatus;   
        
    TRAP(iErrVal, iMyLight = CHWRMLight::NewL(this));	    
    
    if (iErrVal == KErrNone)
    {
    
	    origKeybStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	    origDispStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	    
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
								RDebug::Print(_L("HWRMLightTestB # Wrong status Display:%d, Keyboard:%d"),currDispStatus, currKeybStatus);
								iErrVal = KErrGeneral;
							}
							
							
						}
						else
						{
							RDebug::Print(_L("HWRMLightTestB # HÖÖÖWrong status. Expected:%d, received:%d"),CHWRMLight::ELightBlink, currKeybStatus);
							iErrVal = KErrGeneral;
						}
						
					}
					else
					{
						// testi feilasi masterissa
						RDebug::Print(_L("HWRMLightTestB # Error from peer application:%d"),iErrVal);
					}	
				}
				else
				{
					// väärä status
					RDebug::Print(_L("HWRMLightTestB # Wrong status! Display:%d, Keyboard:%d"),currDispStatus, currKeybStatus);
					iErrVal = KErrGeneral;
				}		
			}
			else
			{
				// Blink failed
				RDebug::Print(_L("HWRMLightTestB # Set keyboard light blink failed:%d"),iErrVal);
			}		
		}
		else
		{
			// valon varaus epäonnistui
			RDebug::Print(_L("HWRMLightTestB # Light reservation failed"),iErrVal);
		}
    }
    
    delete iMyLight;    	
    return iErrVal;
    }


// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::MC_Display_on_and_keyboard_reserved_S
// Light usage with two applications
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::MC_Display_on_and_keyboard_reserved_S()
    {
    RDebug::Print(_L("MC_Display_on_and_keyboard_reserved_S()"));
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
	    			RDebug::Print(_L("HWRMLightTestB # Unexpected status change :From:%d,%d To:%d,%d"),origKeybStatus, origDispStatus, currKeyStatus, currDispStatus);
	    			iErrVal = KErrGeneral;	
	    		}
	    	}
	    	else
	    	{
	    		// Test blink antoi väärän errorin
	    		RDebug::Print(_L("HWRMLightTestB # Wrong error from peer application:%d"),iErrVal);	
	    		
	    	}	
	    }
	    else
	    {
	    	// varaus epäonnistui
	    	RDebug::Print(_L("HWRMLightTestB # Reservation failed reason:%d"),iErrVal);
	    }
    }
    
    delete iMyLight;    	
    return iErrVal;
    }

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::MC_Display_and_keyboard_blink_S
// Light usage with two applications
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::MC_Display_and_keyboard_blink_S()
	{
    RDebug::Print(_L("MC_Display_and_keyboard_blink_S()"));
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
	    					RDebug::Print(_L("HWRMLightTestB # Unexpected status. Keyboard:%d Display:%d"),currKeyStatus, currDispStatus);
	    					iErrVal = KErrGeneral;
	    					
	    				}
		    		
		    		}
		    		else
		    		{
		    		    // Test failed peer
		    		    RDebug::Print(_L("HWRMLightTestB # Test failed failed peer:%d"),iErrVal);
		    		}    			
	    		}
	    		else
	    		{
	    			// light status has changed for some reason???
	    			// it should not because this client is not allowed to do changes in light
	    			// and client that has reserved light has not ordered any change in light  
	    			RDebug::Print(_L("HWRMLightTestB # Unexpected status. Keyboard:%d Display:%d"),currKeyStatus, currDispStatus);  			
	    			iErrVal = KErrGeneral;
	    		}
	    	}
	    	else
	    	{
	    		// Light on epäonnistui
	    		RDebug::Print(_L("HWRMLightTestB # Unexpected status. Keyboard:%d Display:%d"),currKeyStatus, currDispStatus); 	
	    		iErrVal = KErrGeneral;
	    	}	
	    }
	    else
	    {
	    	// varaus epäonnistui
	    	RDebug::Print(_L("HWRMLightTestB # Reservation failed:%d"),iErrVal);
	    }
    }
    
    delete iMyLight;    	
    return iErrVal;
	}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::MC_Vibra_Basic_Usage_S
// 
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::MC_Vibra_Basic_Usage_S()
{
    
    RDebug::Print(_L("MC_Display_on_and_keyboard_reserved_S()"));
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
			    							RDebug::Print(_L("HWRMLightTestB # Wrong vibra status5 (%d), expected STOPPED"),vibraStatus);				
			    							iErrVal = KErrGeneral;
			    						}
			    					}
			    					else
			    					{
			    						// Wrong status
			    						RDebug::Print(_L("HWRMLightTestB # Wrong vibra status4 (%d), expected ON"),vibraStatus);
			    						iErrVal = KErrGeneral;
			    					}
				    			}
				    			else
				    			{
				    				// test failed in master
				    				RDebug::Print(_L("HWRMLightTestB # Error from peer (%d)"),iErrVal);
				    			}
				    			
			    			}
			    			else
			    			{	
			    				// wrong status
			    				RDebug::Print(_L("HWRMLightTestB # Wrong vibra status3 (%d), expected ON"),vibraStatus);
			    				iErrVal = KErrGeneral;
			    			}
			    			
			    		}
			    		else
			    		{
			    			// Start vibra failed
			    			RDebug::Print(_L("HWRMLightTestB # Vibra start failed (%d)"),iErrVal);	
			    			
			    		}			
	 				}
	 				else
	 				{
	 					// Wrong status
	 					RDebug::Print(_L("HWRMLightTestB # Wrong vibra status2 (%d), expected STOPPED"),vibraStatus);
	 					iErrVal = KErrGeneral;
	 				}
	    		}
	    		else
	    		{
	    			// Wrong vibra status
	    			RDebug::Print(_L("HWRMLightTestB # Wrong vibra status1 (%d), expected ON"),vibraStatus);
	    			iErrVal = KErrGeneral;	
	    		}
	    	}
	    	else
	    	{
	    		RDebug::Print(_L("HWRMLightTestB # Vibra start failed (%d)"),iErrVal);
	    
	    	}
	    	
	    	/*****************/
	    	/* Release vibra */
	    	/*****************/
	    	iVibra->ReleaseVibra();	
	    }
	    else
	    {
	    	// Reserve failed
	    	RDebug::Print(_L("HWRMLightTestB # Vibra reservation failed (%d)"),iErrVal);
	    	
	    }
    }
    delete iVibra;
    return iErrVal;
    
}


// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::MC_Combined_light_and_vibra_usage_S
// 
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::MC_Combined_light_and_vibra_usage_S()
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
	      	  RDebug::Print(_L("HWRMLightTestB # Wrong vibra status (%d)"),vibraStatus);
	      	  iErrVal = KErrGeneral;
	      	}
	      	
	      }
	      else
	      {
	      	// Wrong status
	      	RDebug::Print(_L("HWRMLightTestB # Wrong status(%d). Expected KErrInUse"),iErrVal);
	      }
	    }
	    else
	    {
	      RDebug::Print(_L("HWRMLightTestB # Wrong vibra status (%d)"),vibraStatus);  
	      iErrVal = KErrGeneral;
	    }
    }
    delete iVibra;

    return iErrVal;

}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::MC_Combined_light_and_vibra_usage_2_S
// 
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::MC_Combined_light_and_vibra_usage_2_S()
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
		    							RDebug::Print(_L("HWRMLightTestB # Wrong status(%d). Expected EVibraStatusStopped"), vibraStatus);
		    							iErrVal = KErrGeneral;
		    						}	
		    					}
		    					else
		    					{
		    						// wrong vibra status
		    						RDebug::Print(_L("HWRMLightTestB # Wrong status(%d). Expected EVibraStatusOn"), vibraStatus);
		    						iErrVal = KErrGeneral;
		    					}    							
		    				}
		    				else
		    				{
		    					// start vibra failed
		    					RDebug::Print(_L("HWRMLightTestB # Vibra start failed(%d)."), iErrVal);
		    				}
		    			}
		    			else
		    			{
		    				// wrong status
		    				RDebug::Print(_L("HWRMLightTestB # Wrong status keyboard(%d) display(%d)"), keybStatus, dispStatus);
		    				iErrVal = KErrGeneral;
		    			}
					}
					else
					{
						// Set light on failed
						RDebug::Print(_L("HWRMLightTestB # Light on failed(%d)"), iErrVal);
					}
				}
				else
				{
					// Reserve light failed
					RDebug::Print(_L("HWRMLightTestB # Reserve light failed(%d)"), iErrVal);		
				}
		    }
		    else
		    {
		    	// Reserve vibra failed
		    	RDebug::Print(_L("HWRMLightTestB # Reserve vibra failed(%d)"), iErrVal);
		    }
		}
    }
    delete iVibra;
    delete iMyLight;
    return iErrVal;	
	
}


/* <- TeK */


// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LogResult
// Write test case result to log file
// -----------------------------------------------------------------------------
//
void CHWRMLightTestBAppUi::LogResult(const TDesC& aTestCase, TInt aErrorCode)
{
	TBuf16<256> caseName;
	caseName.Append(aTestCase);
	
	if(aErrorCode == KErrNone)
		{
		//iLog.Printf(_L("%S -- PASSED"), &caseName);
		}
	else
		{
		//iLog.Printf(_L("%S -- FAILED"), &caseName);
		//iLog.Printf(_L("Test case returned with error [%d]"), aErrorCode);
		
		}
	
	//iLog.Printf(_L(" "));
	
}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightStatusChanged
// 
// -----------------------------------------------------------------------------
//
void CHWRMLightTestBAppUi::LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus)
{
	//iLog.Printf(_L("Light status changed"));
	//InitStatus();
	
	//iLog.Printf(_L(""));
	iMyTarget = aTarget;
	iMyStatus = aStatus;
	
	//RDebug::Print(_L("Light target %d - LightStatusChanged to: %d"), aTarget, aStatus);
	
	RDebug::Print(_L("### Light state changed for target: 0x%x"), aTarget);
    //iLog.Printf(_L("### Light state changed for target: 0x%x"), aTarget);
    
    switch ( aStatus )
        {
        case CHWRMLight::ELightOn:
            RDebug::Print(_L("### Light state changed: ELightOn"));
            //iLog.Printf(_L("### Light state changed: ELightOn"));
            break;
        case CHWRMLight::ELightOff:
            RDebug::Print(_L("### Light state changed: ELightOff"));
            //iLog.Printf(_L("### Light state changed: ELightOff"));
            break;
        case CHWRMLight::ELightBlink:
            RDebug::Print(_L("### Light state changed: ELightBlink"));
            //iLog.Printf(_L("### Light state changed: ELightBlink"));
            break;
        case CHWRMLight::ELightStatusUnknown:
            RDebug::Print(_L("### Light state changed: ELightStatusUnknown"));
            //iLog.Printf(_L("### Light state changed: ELightStatusUnknown"));
            break;
        default:
            RDebug::Print(_L("### Light state changed: UNDEFINED !!!"));
            //iLog.Printf(_L("### Light state changed: UNDEFINED !!!"));
            break;
        }
}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::VibraStatusChanged
// 
// -----------------------------------------------------------------------------
//
void CHWRMLightTestBAppUi::VibraStatusChanged( CHWRMVibra::TVibraStatus aStatus)
{
	
	//iLog.Printf(_L(""));
	iVibraStatus = aStatus;
	
	RDebug::Print(_L("HWRMLightTestB # Vibra status changed to %d"), aStatus);
        
    switch ( aStatus )
        {
        case CHWRMVibra::EVibraStatusNotAllowed:
            RDebug::Print(_L("HWRMLightTestB # Vibra status changed: EVibraStatusNotAllowed"));
           
            break;
        case CHWRMVibra::EVibraStatusStopped:
            RDebug::Print(_L("HWRMLightTestB # Vibra status changed: EVibraStatusStopped"));
           
            break;
        case CHWRMVibra::EVibraStatusOn:
            RDebug::Print(_L("HWRMLightTestB # Vibra status changed: EVibraStatusOn"));
           
            break;
        case CHWRMVibra::EVibraStatusUnknown:
            RDebug::Print(_L("HWRMLightTestB # Vibra status changed: EVibraStatusUnknown"));
           
            break;
        default:
            RDebug::Print(_L("HWRMLightTestB # Vibra status changed: UNDEFINED !!!"));
           
            break;
        }
}

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::VibraModeChanged
// 
// -----------------------------------------------------------------------------
//
void CHWRMLightTestBAppUi::VibraModeChanged( CHWRMVibra::TVibraModeState aStatus)
{
	
	//iLog.Printf(_L(""));
	iVibraMode = aStatus;
	
	RDebug::Print(_L("HWRMLightTestB # Vibra mode changed to %d"), aStatus);
        
    switch ( aStatus )
        {
        case CHWRMVibra::EVibraModeUnknown:
            RDebug::Print(_L("HWRMLightTestB # Vibra mode changed: EVibraModeUnknown"));
           
            break;
        case CHWRMVibra::EVibraModeON:
            RDebug::Print(_L("HWRMLightTestB # Vibra mode changed: EVibraModeON"));
           
            break;
        case CHWRMVibra::EVibraModeOFF:
            RDebug::Print(_L("HWRMLightTestB # Vibra mode changed: EVibraModeOFF"));
           
            break;
        default:
            RDebug::Print(_L("HWRMLightTestB # Vibra mode changed: UNDEFINED !!!"));
           
            break;
        }
}  

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::InitStatus()
// Initialize target and light status
// -----------------------------------------------------------------------------
//
void CHWRMLightTestBAppUi::InitStatus()
{
	//iLog.Printf(_L("Init status"));
	iMyTarget = CHWRMLight::ENoTarget;
	iMyStatus = CHWRMLight::ELightStatusUnknown;	
}

  
//------------------------------ TEST CASES-------------------------------------

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightTargetPrimaryTest
// Set primary display on and off
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightTargetDisplayTest( )
    {
    
    RDebug::Print(_L("LightTargetDisplayTest()"));
    iErrVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
	
	//iLog.Printf(_L("check current light status"));		
	
	//check current light status
	//CHWRMLight::TLightStatus currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	RDebug::Print(_L("Set lights off"));
	//iLog.Printf(_L("Set lights off"));
	TRAP(iErrVal, iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay));	
	//iLog.Printf(_L("errValue %d"), iErrVal);	
	RDebug::Print(_L("errValue %d"), iErrVal);

	CHWRMLight::TLightStatus currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	
	if(currStatus == CHWRMLight::ELightOff)		
		{
		RDebug::Print(_L("currStatus==ELightOff"));
		//iLog.Printf(_L("set light on for default intensity and infinite duration"));
		//set light on for default intensity and infinite duration
		RDebug::Print(_L("set light on for default intensity and infinite duration"));
		TRAP(iErrVal, iMyLight->LightOnL(CHWRMLight::EPrimaryDisplay));
		

		if(iErrVal != KErrNone)
			{
			RDebug::Print(_L("Error when setting lights on, errValue %d"), iErrVal);
			//iLog.Printf(_L("Error when setting lights on, errValue %d"), iErrVal);	
			return iErrVal;
			
			}

		}
	
	if((iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) == CHWRMLight::ELightOn))
	    	{
	    	TRAP(iErrVal, iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay));
	    	//iLog.Printf(_L("Light Off -- EPrimaryDisplay, errValue %d"), iErrVal);		
	    	RDebug::Print(_L("Light Off -- EPrimaryDisplay, errValue %d"), iErrVal);
	    	}
    	
    delete iMyLight;    	
    return iErrVal;
    
    }

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightTargetKeyboardTest
// Set primary keyboard light on and off
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightTargetKeyboardTest( )
    {
    
  iErrVal = KErrNone;
  //create light instance
  iMyLight = CHWRMLight::NewL(this);
	RDebug::Print(_L("check current light status"));
	//check current light status
	CHWRMLight::TLightStatus currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard);
	
	if(currStatus == CHWRMLight::ELightOn)		
		{
		RDebug::Print(_L("currStatus==ELightOn"));
		//if light are on, set off
		
		RDebug::Print(_L("set lights off"));
		TRAP(iErrVal, iMyLight->LightOffL(CHWRMLight::EPrimaryKeyboard));	
		
		if(iErrVal != KErrNone)
		{
			RDebug::Print(_L("error in setting lights off: errValue == %d"), iErrVal);
			return iErrVal;
		}
			
		}
	else if(currStatus == CHWRMLight::ELightStatusUnknown)
		{
		RDebug::Print(_L("currStatus==ELightStatusUnknown"));
		//invalid target
		return KErrGeneral;		
		}
	
	//set light on for default intensity and infinite duration
	TRAP(iErrVal, iMyLight->LightOnL(CHWRMLight::EPrimaryKeyboard));
	if(iErrVal != KErrNone)
	{
		
		RDebug::Print(_L("error in setting lights on: errValue == %d"), iErrVal);
		return iErrVal;
	}
		
	
    if((iMyLight->LightStatus(CHWRMLight::EPrimaryKeyboard) == CHWRMLight::ELightOn))
    	{
    	TRAP(iErrVal, iMyLight->LightOffL(CHWRMLight::EPrimaryKeyboard));		
    	if (iErrVal != KErrNone)
    		RDebug::Print(_L("error in setting lights off: errValue == %d"), iErrVal);
    	}
    
    delete iMyLight; 	
    return iErrVal;
    
    }

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightDurationTest
// Set primary display lights on for 5 seconds
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightOnDurationTest( )
    {
    
    iErrVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
	//check current light status
	RDebug::Print(_L("check current light status"));
	CHWRMLight::TLightStatus currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	
	if(currStatus == CHWRMLight::ELightOn)		
		{
		RDebug::Print(_L("currStatus == CHWRMLight::ELightOn"));
		//if light are on, set off for infinite duration
		TRAP(iErrVal, iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay));	
		if(iErrVal != KErrNone)
			{
			RDebug::Print(_L("error when setting lights off - errValue: %d"), iErrVal);
			delete iMyLight;
			return iErrVal;
			}	
		}
	else if(currStatus == CHWRMLight::ELightStatusUnknown)
		{
		RDebug::Print(_L("currStatus == CHWRMLight::ELightStatusUnknown"));
		delete iMyLight;
		//invalid target
		return KErrGeneral;		
		}
	
	RDebug::Print(_L("set light on for default intensity and 5 seconds"));
	//set light on for default intensity and 5 seconds
	TRAP(iErrVal, iMyLight->LightOnL(CHWRMLight::EPrimaryDisplay, 5000));
	
	if(iErrVal != KErrNone)
	{
		
		RDebug::Print(_L("error when setting lights on, errValue:"), iErrVal);
		return iErrVal;
	}
	//check that display lights are on
	currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	if (currStatus == CHWRMLight::ELightOn)
		{
		RDebug::Print(_L("currStatus == CHWRMLight::ELightOn"));
		RDebug::Print(_L("set lights off"));
		iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay);
		//set timer to wait the duration 
		CTestScheduler::StartAndStopL(5000);
		//should be back on baseline (lights off)	
		if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOff)
		{
			
			RDebug::Print(_L("light status != CHWRMLight::ELightOff (errValue %d)"), iErrVal);
			iErrVal = KErrGeneral;	
			
		}
		}
	else
		{
			RDebug::Print(_L("currStatus =! CHWRMLight::ELightOn"));
			iErrVal = KErrGeneral;
		}
		
    delete iMyLight; 	
    return iErrVal;
        
    }

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightOffDurationTest
// Set primary display lights on and set lights off for 3 seconds
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightOffDurationTest( )
    {
    
    iErrVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
	RDebug::Print(_L("check current light status"));
	//check current light status
	CHWRMLight::TLightStatus currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	
	if(currStatus == CHWRMLight::ELightOff)		
		{
		RDebug::Print(_L("currStatus == CHWRMLight::ELightOff"));
		//if light are off, set on for infinite duration
		TRAP(iErrVal, iMyLight->LightOnL(CHWRMLight::EPrimaryDisplay));	
		if(iErrVal != KErrNone)
			{
			RDebug::Print(_L("error setting lights on, errValue = %d"), iErrVal);
			delete iMyLight;
			return iErrVal;
			}
			
		}
	else if(currStatus == CHWRMLight::ELightStatusUnknown)
		{
		RDebug::Print(_L("CHWRMLight::ELightStatusUnknown"));
		delete iMyLight;
		//invalid target
		return KErrGeneral;		
		}
	
	RDebug::Print(_L("set light off for default intensity and 3 seconds"));
	//set light off for default intensity and 3 seconds
	TRAP(iErrVal, iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay, 3000));
	if(iErrVal != KErrNone)
	{
		
		RDebug::Print(_L("set lights off failed, errValue = %d"), iErrVal);
		return iErrVal;
	}
	RDebug::Print(_L("check that display lights are off"));
	//check that display lights are off
	currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	if (currStatus == CHWRMLight::ELightOff)
		{
		RDebug::Print(_L("currStatus == CHWRMLight::ELightOff"));
		RDebug::Print(_L("set lights on"));
		iMyLight->LightOnL(CHWRMLight::EPrimaryDisplay);	
		//set timer to wait the duration 
		CTestScheduler::StartAndStopL(3000);
		//should be back on baseline (lights on)	
		if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOn)
		{
			
			RDebug::Print(_L("light status != CHWRMLight::ELightOn"));
			iErrVal = KErrGeneral;	
		}
		}
	else
		{
			RDebug::Print(_L("currStatus == CHWRMLight::ELightOff"));
			iErrVal = KErrGeneral;
		}
		
    delete iMyLight; 	
    return iErrVal;
        
	}

  
// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightIntensityTest
// Set primary display lights on and with different intensity levels.
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightIntensityTest( )
    {
    
    iErrVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
	//check current light status
	CHWRMLight::TLightStatus currStatus = iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay);
	
	if(currStatus == CHWRMLight::ELightOn)		
		{
		//if light are on, set off for infinite duration
		TRAP(iErrVal, iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay));	
		if(iErrVal != KErrNone)
			{
			delete iMyLight;
			return iErrVal;
			}
			
		}
	else if(currStatus == CHWRMLight::ELightStatusUnknown)
		{
		delete iMyLight;
		//invalid target
		return KErrGeneral;		
		}
	
	//set lights on with different intensity values
	for(TInt i=0; i<=100; i+=10)
		{
		//set light on 
		TRAP(iErrVal, iMyLight->LightOnL(CHWRMLight::EPrimaryDisplay, 2000, i, ETrue));
		if(iErrVal != KErrNone)
			{
			delete iMyLight;
			return iErrVal;
			}
			
		//set timer to wait the duration 
		CTestScheduler::StartAndStopL(2000);
		
		}
		
	//check that display lights are off again
	if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOff)
			iErrVal = KErrGeneral;	
	
    delete iMyLight; 	
    return iErrVal;
   
    }

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightBlinkTargetTest
// Set primary display lights blink for infinite duration
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightBlinkTargetTest( )
    {
    
    iErrVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
	
	//set lights to blink 
	TRAP(iErrVal, iMyLight->LightBlinkL(CHWRMLight::EPrimaryDisplay));
	if(iErrVal != KErrNone)
		{
		delete iMyLight;
		return iErrVal;
		}

	//check that display lights are off again
	if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightBlink)
			iErrVal = KErrGeneral;	
	
	//set lights off for 3 sec
	TRAP(iErrVal, iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay, 3000));
	if(iErrVal != KErrNone)
	{
		CTestScheduler::StartAndStopL(3000);
		if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightBlink)
			iErrVal = KErrGeneral;				
	}
	
	//set lights off 
	iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay);
	
    delete iMyLight; 	
    return iErrVal;
    
    }

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightBlinkDurationTest
// Set primary display lights to blink 5 seconds
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightBlinkDurationTest( )
    {
    
    iErrVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
    if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOff)
    	iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay);
	
	//set lights to blink for 5 sec
	TRAP(iErrVal, iMyLight->LightBlinkL(CHWRMLight::EPrimaryDisplay, 5000 ));
	if(iErrVal != KErrNone)
		{
		delete iMyLight;
		return iErrVal;
		}

	//check that display lights are blinking
	if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightBlink)
		{
		CTestScheduler::StartAndStopL(5000);
		if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOff)
			iErrVal = KErrGeneral;				
		}
	
    delete iMyLight; 	
    return iErrVal;
    
    }


// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightBlinkOnOff
// 
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightBlinkOnOff( )
    {
    
    iErrVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
    if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOff)
    	iMyLight->LightOffL(CHWRMLight::EPrimaryDisplay);
	
	//set lights to blink for 10 sec
	TRAP(iErrVal, iMyLight->LightBlinkL(CHWRMLight::EPrimaryDisplay, 5000, 500, 300, 100));
	if(iErrVal != KErrNone)
		{
		delete iMyLight;
		return iErrVal;
		}

	//check that display lights are blinking
	if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightBlink)
		{
		CTestScheduler::StartAndStopL(10000);
		if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOff)
			iErrVal = KErrGeneral;				
		}
	
    delete iMyLight; 	
    return iErrVal;
        
    }
    
   
// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightWithDifferentTargets
// Set primary display lights off and on with different targets
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightWithDifferentTargets( )
    {
    
    iErrVal = KErrNone;
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
    if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplayAndKeyboard) != CHWRMLight::ELightOff)
    	iMyLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	
	//set lights on for 3 sec
	TRAP(iErrVal, iMyLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, 3000));
	if(iErrVal != KErrNone)
		{
		delete iMyLight;
		return iErrVal;
		}
	//check that display lights are on
	if(iMyLight->LightStatus(CHWRMLight::EPrimaryDisplayAndKeyboard) != CHWRMLight::ELightOn)
		{
		CTestScheduler::StartAndStopL(3500);
		if((iMyStatus != CHWRMLight::ELightOff) && (iMyTarget != CHWRMLight::EPrimaryDisplayAndKeyboard))
			iErrVal = KErrGeneral;				
		}

	iMyLight->LightOnL(CHWRMLight::EPrimaryDisplay, 5000);
	CTestScheduler::StartAndStopL(2000);
	
	if(iMyStatus == CHWRMLight::ELightOn && CHWRMLight::EPrimaryDisplay)
		{
		
		iMyLight->LightBlinkL(CHWRMLight::EPrimaryKeyboard, 3000);
		CTestScheduler::StartAndStopL(1000);
		if(iMyStatus != CHWRMLight::ELightBlink && iMyTarget != CHWRMLight::EPrimaryKeyboard)
			{
			iErrVal = KErrGeneral;
			}
			
		}
		else
		{
		iErrVal = KErrGeneral;
		}
   
    delete iMyLight; 	
    return iErrVal;
   
    }


// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightWithInvalidDuration()
// 
// -----------------------------------------------------------------------------
//
TInt CHWRMLightTestBAppUi::LightWithInvalidDuration( )
    {
    
    TInt retVal1(0), retVal2(0), retVal3(0), retVal4(0);
    TInt maxDuration(KHWRMLightMaxDuration);
    TInt infDuration(KHWRMInfiniteDuration);
    iErrVal = KErrNone;
    
    //create light instance
    iMyLight = CHWRMLight::NewL(this);
    
    //Set lights off for infinite duration
   	iMyLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	
	//set lights on for too long duration KHWRMLightMaxDuration+1 //KErrArgument
	TRAP(retVal1, iMyLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, (maxDuration+1)));
		
	//set lights on for infinite duration KHWRMInfiniteDuration//KErrArgument
	TRAP(retVal2, iMyLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, (infDuration-1)));
		
	iMyLight->LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, KHWRMInfiniteDuration);
	CTestScheduler::StartAndStopL(1000);
		
	if(iMyStatus != CHWRMLight::ELightOn)
		{
			delete iMyLight;
			return (iErrVal = KErrGeneral);
		}

	//set lights off for too long duration KHWRMInfiniteDuration-1 //KErrArgument
	TRAP(retVal3, iMyLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard, (infDuration-1)));

	//set lights off for too long duration KHWRMLightMaxDuration+1 //KErrArgument
	TRAP(retVal4, iMyLight->LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard, (maxDuration+1)));
			
	if (retVal1 == KErrArgument && retVal2 == KErrArgument && retVal3 == KErrArgument && retVal4 == KErrArgument)		
		{
			iErrVal = KErrNone;	
		}
	else
		{
			iErrVal = KErrGeneral;	
		}
	  
    delete iMyLight; 	
    return iErrVal;	
    
    }
    

// -----------------------------------------------------------------------------
// CHWRMLightTestBAppUi::LightWithInvalidTarget
// 
// -----------------------------------------------------------------------------
//
/*TInt CHWRMLightTestBAppUi::LightWithInvalidTarget( )
    {
   
    TInt retVal1(0), retVal2(0);
    CHWRMLight::TLightTarget invalidTarget = 20; 
    
    iErrVal = KErrNone;
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
			return (iErrVal = KErrGeneral);
		}
		
	//try to set lights off for invalid target //KErrNotSupported
	TRAP(retVal2, iMyLight->LightOffL(invalidTarget, KHWRMInfiniteDuration);
			
	if (retVal1 == KErrNotSupported && retVal2 == KErrNotSupported)		
		{
			iErrVal = KErrNone;	
		}
	else
		{
			iErrVal = KErrGeneral;	
		}
	  
    delete iMyLight; 	
    return iErrVal;	    
    
    }*/

#endif // COMMENTEDOUT

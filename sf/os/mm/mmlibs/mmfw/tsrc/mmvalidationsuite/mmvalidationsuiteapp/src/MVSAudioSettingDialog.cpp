// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Part of the MVS Application for TechView
//


#include "MVSAudioSettingDialog.h"
#include "MVSApp.hrh"


void CMVSAudioSettingDialog::SetupDialogLD(TInt aNoRepeats, 
                          TTimeIntervalMicroSeconds aTrailingSilence,TInt aAudioPriority,
                          TMdaPriorityPreference aPriorityPreference, TInt& aMaxFileBuf,
                          CMVSAppUi* aAppUi)
	{
    CMVSAudioSettingDialog* dialog = new (ELeave) CMVSAudioSettingDialog(aNoRepeats,
                                                       aTrailingSilence,aAudioPriority, aPriorityPreference, aMaxFileBuf, aAppUi);
    dialog->ExecuteLD(R_NEW_MVS_DIALOG_AUDIOSETTING);
    }

void CMVSAudioSettingDialog::PreLayoutDynInitL()
    {
    CEikNumberEditor* repeatControl = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetRepeats));
    CEikNumberEditor* myTrSilenceControl = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetTrailingSilence));
    iMaxFileSizeCtrl = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetMaxFileSizeNum));
        
    if(iMaxFileBuf>0)
    	{
    	iMaxFileSizeCtrl->SetNumber(iMaxFileBuf);
    	}
    else
    	{
    	iMaxFileSizeCtrl->SetNumber(0);
     	}
    //Now seed the controls with appropriate values
    if(repeatControl)
	    {
	    repeatControl->SetNumber(iNoRepeats);
	    }
    if(myTrSilenceControl)
    	{
    	myTrSilenceControl->SetNumber(I64INT(iTrailingSilence.Int64()/1000000));	
    	}
    // Get a downcasted pointer to the controls
    CEikChoiceList* audioPriority = static_cast<CEikChoiceList*>(Control(EMVSCmdSetAudioPriority));

    CEikChoiceList* audioPriorityPref = static_cast<CEikChoiceList*>(Control(EMVSCmdSetAudioPriorityPreference));

    
    if(audioPriority)
	    {
	    //Set the current item based on the argument passed to the object
	    switch (iAudioPriority)
	    	{
	        case(EMdaPriorityMin):
	        	audioPriority->SetCurrentItem(0);
	          	break;

	        case(EMdaPriorityMax):
	        	audioPriority->SetCurrentItem(2);
	          	break;

	    	case(EMdaPriorityNormal):
	        default:
	        	audioPriority->SetCurrentItem(1);
	          	break;
	        }	
	    }
     if(audioPriorityPref)
	     {
	     //Set the priority preference based on the argument passed to the object
	     switch (iPriorityPreference)
	     	{
	        case(EMdaPriorityPreferenceTime):
	        	audioPriorityPref->SetCurrentItem(1);
	          	break;
	          
	        case(EMdaPriorityPreferenceQuality):
	        	audioPriorityPref->SetCurrentItem(2);
	          	break;
	          
	     	case(EMdaPriorityPreferenceTimeAndQuality):
	        	audioPriorityPref->SetCurrentItem(3);
	          	break;
	        
	     	case(EMdaPriorityPreferenceNone):
	     	default:
	     		audioPriorityPref->SetCurrentItem(0);
	        }	
	     }
    }


CMVSAudioSettingDialog::CMVSAudioSettingDialog(TInt aNoRepeats, 
                             TTimeIntervalMicroSeconds aTrailingSilence,TInt aAudioPriority,
                      TMdaPriorityPreference aPriorityPreference, TInt& aMaxFileBuf,
                      CMVSAppUi* aAppUi)
: iAppUi(aAppUi),iAudioPriority(aAudioPriority), iPriorityPreference (aPriorityPreference),
iNoRepeats(aNoRepeats),iTrailingSilence(aTrailingSilence), iMaxFileBuf(aMaxFileBuf)
	{
    
    }

TKeyResponse CMVSAudioSettingDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if(aKeyEvent.iCode == EKeyEnter && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonUpdate);
		}
	else if(aKeyEvent.iCode == EKeyEscape && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonCancel);
		}
	return CEikDialog::OfferKeyEventL(aKeyEvent,aType);	
	}

TBool CMVSAudioSettingDialog::OkToExitL(TInt aButtonId)
    {
    // Get a pointer to the progress bar control.
    // Downcast the returned CCoeControl* pointer to the correct type.
    CEikChoiceList* audioPriority = static_cast<CEikChoiceList*>(Control(EMVSCmdSetAudioPriority));

    CEikChoiceList* audioPriorityPref = static_cast<CEikChoiceList*>(Control(EMVSCmdSetAudioPriorityPreference));
    
    iMaxFileBuf = iMaxFileSizeCtrl->Number();
	iAppUi->SetMaxFileSize(iMaxFileBuf);	
    //Identify current Audio Priority item, and set that
    if(audioPriority)
	    {
	    switch (audioPriority->CurrentItem())
	    	{
	        case 0:
	        	iAudioPriority = EMdaPriorityMin;
	          	break;

	        case 1:
	        	iAudioPriority = EMdaPriorityNormal;
	          	break;

	        case 2:
	        	iAudioPriority = EMdaPriorityMax;
	         	break;

	        default:
	        	iAudioPriority = EMdaPriorityNormal;
	         	break;
	        }	
	    }
    
    //Now do the same for the Priority Preference.
    if(audioPriorityPref)
	    {
	    switch (audioPriorityPref->CurrentItem())
	    	{
	        case 1:
	        	iPriorityPreference = EMdaPriorityPreferenceTime;
	          	break;
	          
	        case 2:
	        	iPriorityPreference = EMdaPriorityPreferenceQuality;
	          	break;
	          
	        case 3:
	        	iPriorityPreference = EMdaPriorityPreferenceTimeAndQuality;
	          	break;

	    	case 0:
	        default:
	        	iPriorityPreference = EMdaPriorityPreferenceNone;
	          	break;
	        }
	    }
    iNoRepeats = static_cast<CEikNumberEditor*>
                        (Control(EMVSCmdSetRepeats))->Number();

    //Update the trailing silence from the control.
    TInt64 silence64 = (static_cast<CEikNumberEditor*>
                        (Control(EMVSCmdSetTrailingSilence))->Number());  
    TInt silence = I64INT(silence64*1000000);
    TTimeIntervalMicroSeconds theSilence(silence);
    
    switch(aButtonId)
    	{
     	case(EMVSButtonUpdate):
        	iAppUi->SetPriorityL(iAudioPriority, iPriorityPreference);
            iAppUi->SetRepeats(iNoRepeats, theSilence);
            return ETrue;
            
    	case(EMVSButtonCancel):
    		return ETrue;
        default:
           	break;
     	} 
    return ETrue;
    }

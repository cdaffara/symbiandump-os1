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

#include "MVSSelectPluginsDialog.h"
#include "MVSApp.hrh"
#include <techview/eikmfne.h>
#include <coecntrl.h>
#include "MVSConstants.h"

#include <techview/eikhopbt.h>



void CMVSSelectPluginsDialog::SetupDialogLD(CDesCArrayFlat* aPluginArray,
		                     CDesCArrayFlat* aExtArray, CDesCArrayFlat* aCodecArray, 
                               CMVSAppUi* aAppUi)
	{
     
    CMVSSelectPluginsDialog* dialog = new (ELeave) CMVSSelectPluginsDialog(
                                                       aPluginArray, 
                                                       aExtArray, aCodecArray,
                                                       aAppUi);
     
    CleanupStack::PushL(dialog); 
    dialog->ConstructL(aCodecArray);
    CleanupStack::Pop(dialog);
    dialog->ExecuteLD(R_MVS_DIALOG_SELECTPLUGINS);
    }



void CMVSSelectPluginsDialog::ConstructL(CDesCArrayFlat* aCodecArray)
    {
    //Create new arrays
    iExtArray = new(ELeave) CDesCArrayFlat(8);
    iPluginArray = new(ELeave) CDesCArrayFlat(8);
	iCodecArray = new(ELeave) CDesCArrayFlat(8);
    iAppUi->GetExtensionListL(iExtArray,iMediaType);
    iAppUi->GetPluginListL(0,iExtArray,iPluginArray,iMediaType);
    for(TInt counter = 0; counter< aCodecArray->Count(); ++counter)
    	{
        iCodecArray->AppendL((*aCodecArray)[counter]);
        }
    }


void CMVSSelectPluginsDialog::PreLayoutDynInitL()
    {
    CEikChoiceList* myPluginArray = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectControllerPlugin));
    CEikChoiceList* myExt = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectFormat));
    if(iExtArray->Count()==0)
    	{
		User::InfoPrint(KNotsupported);    	
    	}
    else
    	{
    	if(myPluginArray)
			{
			myPluginArray->SetArrayL(iPluginArray);
    		myPluginArray->SetArrayExternalOwnership(ETrue);	
			myPluginArray->SetObserver(this);
			iPluginCtl =  myPluginArray; 
			}
    	if(myExt)
    		{
    		myExt->SetArrayL(iExtArray);
    		myExt->SetArrayExternalOwnership(ETrue);	
    		myExt->SetObserver(this); 
    		iExtCtl = myExt;
    		}	
    	}
    //Get a pointer to the Option Button List control
    CEikHorOptionButtonList* pRadioGroup = static_cast<CEikHorOptionButtonList*>(Control(EMVSCmdSelectMediaType));
    //set audio as the default button to be selected
    if(pRadioGroup)
    	{
    	pRadioGroup->SetButtonById(EMVSHorOptButtonAudio);
    	pRadioGroup->SetObserver(this);
    	}
    }

	
CMVSSelectPluginsDialog::CMVSSelectPluginsDialog(CDesCArrayFlat* aPluginArray,
		                      CDesCArrayFlat* aExtArray, CDesCArrayFlat* aCodecArray,  
                              CMVSAppUi* aAppUi)
: iPluginArray(aPluginArray), iExtArray(aExtArray), iCodecArray(aCodecArray), iAppUi(aAppUi)
    {     
    iPluginCtl = 0;
    iExtCtl = 0;
    iMediaType = 0;
    }


TBool CMVSSelectPluginsDialog::OkToExitL(TInt aButtonId)
	{
    TInt indx = 0;
    //button options are Next> and Cancel
    if(aButtonId == EMVSButtonNext)
    	{
        //Get a downcasted pointer to the controls
        CEikChoiceList* myPluginArray = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectControllerPlugin));
        CEikChoiceList* myExt = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectFormat));
		iExt.Zero(); //zero the buffer, and append the extension
        if(myPluginArray && myExt)
	        {
	        CDesCArray *pDescArr = myExt->DesCArray();
	        CDesCArray *pControllerArr = myPluginArray->DesCArray();
	        if(pDescArr->MdcaCount() > 0)
		    	{
		        indx = myExt->CurrentItem();
				iExt.Append((*iExtArray)[indx]);		  	          	
		        } 
		    indx = myPluginArray->CurrentItem();
			iAppUi->SelectedExtension(iExt);
		    iAppUi->SelectedPluginIdx(indx);
		    iAppUi->SelectedMedia(iMediaType);	
	        }
        }
    else if(aButtonId == EMVSButtonCancel)
    	{
    	//to indicate that a suitable controller has to be selected automatically
    	//by the system based on the file name entered by the user.
    	indx= KControllerNotSelected;
    	iAppUi->SelectedPluginIdx(indx);
    	}
    return ETrue; //return
    }


void CMVSSelectPluginsDialog::HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType)
	{
	CEikDialog::HandleControlEventL(aControl,aEventType);
	CEikChoiceList* myPluginArray = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectControllerPlugin));
	CEikChoiceList* myExt = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectFormat));		
	if(myPluginArray && myExt)
		{
		TInt8 idx  = iExtCtl->CurrentItem();
		if(iExtCtl == aControl && aEventType == EEventStateChanged)
			{
			iPluginArray->Reset();
			TInt8 temp = iAppUi->GetPluginListL(idx,iExtArray,iPluginArray,iMediaType);
			TInt n = iPluginArray->Count();
			myPluginArray->SetArrayL(iPluginArray);
			myPluginArray->SetArrayExternalOwnership(ETrue);
			myPluginArray->SetCurrentItem(0);
			myPluginArray->DrawDeferred();
			return;
			}
		
		//Get a pointer to the Option Button List control
	    CEikHorOptionButtonList* pRadioGroup = 
	    	static_cast<CEikHorOptionButtonList*>(Control(EMVSCmdSelectMediaType)); 	
	    if(pRadioGroup == aControl && aEventType == EEventStateChanged)
	    	{
	     	TInt nButtonId = pRadioGroup->LabeledButtonId();
	     	if(nButtonId == EMVSHorOptButtonAudio) //if selected media - audio
	     		{
	     		iMediaType = 0;
	     		}
	     	else if(nButtonId == EMVSHorOptButtonVideo) //if selected media - video
	     		{
	     		iMediaType = 1;
	     		}
	       	iPluginArray->Reset();
	   		iExtArray->Reset();
	   		idx=0;  	
	      	iAppUi->GetExtensionListL(iExtArray,iMediaType);
	      	if(iExtArray->Count()==0)
	      		{
	      		User::InfoPrint(KNotsupported);
	      		}
	      	else
	      		{
	      		iAppUi->GetPluginListL(idx,iExtArray,iPluginArray,iMediaType);
	      		myPluginArray->SetArrayL(iPluginArray);
	     		myPluginArray->SetCurrentItem(0);
	     		myPluginArray->DrawDeferred();
	     		myExt->SetArrayL(iExtArray);
				myExt->SetArrayExternalOwnership(ETrue);
				myExt->SetCurrentItem(0);
				myExt->DrawDeferred();	
	      		}		
	      	}	
		}
	}


TKeyResponse CMVSSelectPluginsDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	//In this case iScanCode is used in preference to iCode, 
	//as using iCode in list selection doesn't seem to work well
	//with shortkeys,gives a panic
	if (aKeyEvent.iScanCode == EStdKeyEscape && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonCancel);	
		}
	if(aKeyEvent.iScanCode == EStdKeyEnter && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonNext);
		}
	return CEikDialog::OfferKeyEventL(aKeyEvent,aType);
	}


CMVSSelectPluginsDialog::~CMVSSelectPluginsDialog()
	{
	iPluginArray->Reset(); //reset the arrays
    iExtArray->Reset();
    iCodecArray->Reset();
    delete iExtArray; //and delete them both
    delete iPluginArray;
	delete iCodecArray;
	}

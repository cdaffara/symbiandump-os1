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


#include "MVSConfigAudioFormatDialog.h"
#include "MVSApp.hrh"
#include <badesca.h>


#include "MVSAppUI.h" 

void CMVSConfigAudioFormatDialog::SetupDialogLD(CMVSAppUi* aAppUi,TUid aControllerId,CSettingsManager* aSettingsManager)
	{
    CMVSConfigAudioFormatDialog* dialog = new (ELeave) CMVSConfigAudioFormatDialog(aAppUi,aControllerId); 
    CleanupStack::PushL(dialog);
    dialog->ConstructL();
    aSettingsManager->ReadAudioDataL(dialog,aControllerId);
    CleanupStack::Pop(dialog);
    dialog->ExecuteLD(R_MVS_DIALOG_CONFIGUREDATAFORMAT);
    }


void CMVSConfigAudioFormatDialog::ConstructL() 
	{
	iArrBitRates = new(ELeave) CDesCArrayFlat(8);	
	iArrSampleRates = new(ELeave) CDesCArrayFlat(8);	
	iArrChannels = new(ELeave) CDesCArrayFlat(8);	
	iArrCodecs = new(ELeave) CDesCArrayFlat(8);	
	}


void CMVSConfigAudioFormatDialog::PreLayoutDynInitL()
	{
	// Get a downcasted pointer to each of the controls
	CEikChoiceList* bitrateList = static_cast<CEikChoiceList*>(Control(EMVSSetBitrate));
	 
	CEikChoiceList* codecList = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectCodec));

	CEikChoiceList* channelsList = static_cast<CEikChoiceList*>(Control(EMVSSetNoOfChannels));

	CEikChoiceList* sampleratesList = static_cast<CEikChoiceList*>(Control(EMVSSetSamplerate));
    
    CEikCheckBox* restoresettings = static_cast<CEikCheckBox*>(Control(EMVSRestoreAudioSettings));	
	iSettings = restoresettings;
	TBuf<10> td;
	TInt err = 0;
	CDesCArray*	tempArray = NULL;
	if(restoresettings)
		{
		restoresettings->SetObserver(this); 
		}
	if(bitrateList)
		{
		bitrateList->SetArrayL(iArrBitRates);
		bitrateList->SetArrayExternalOwnership(ETrue);
		tempArray = bitrateList->DesCArray(); 	   		
		//get supported bit rates
		TRAP(err,iAppUi->GetSupportedBitRatesL(iBitRatesArray));	
		if(err == KErrNotSupported)	
			{
			_LIT(KBitRateSupport, "Bitrates not supported");
			User::InfoPrint(KBitRateSupport);
			iBitRateIdx = -1;			
			}
		else
			{
			//copy from array to  CDescCArray 
		    for(TInt i = 0; i < iBitRatesArray.Count();i++) 
			    {
				td.Num(iBitRatesArray[i]);
				tempArray->AppendL(td);
			    }
			}	
		}
	
    if(sampleratesList)
	    {
	    sampleratesList->SetArrayL(iArrSampleRates);
	    sampleratesList->SetArrayExternalOwnership(ETrue);
	    tempArray = sampleratesList->DesCArray();
		//get supported sample rates
		TRAP(err,iAppUi->GetSupportedSampleRatesArrayL(iSampleRatesArray));
		if(err == KErrNotSupported)
			{
			_LIT(KSampleRateSupport, "Samplerates not supported");
			User::InfoPrint(KSampleRateSupport);			
			iSampleRateIdx = -1;
			}
		else
			{
			//copy from array to  CDescCArray 
		    for(TInt i = 0; i < iSampleRatesArray.Count();i++) 
		    	{
		    	td.Num(iSampleRatesArray[i]);
		    	tempArray->AppendL(td);
		    	}
			}	
	    }
    
    if(channelsList)
	    {
	    channelsList->SetArrayL(iArrChannels);
	    channelsList->SetArrayExternalOwnership(ETrue);
	    tempArray = channelsList->DesCArray();
	    //get supported channels 	
		TRAP(err,iAppUi->GetSupportedNoChannelsArrayL(iChannelsArray));
		if(err==KErrNotSupported)
			{
			_LIT(KChannelSupport, "Channels not supported");
			User::InfoPrint(KChannelSupport);			
			iChannelListIdx = -1;
			}
		else
			{
			//copy from array to  CDescCArray
			for(TInt i = 0; i < iChannelsArray.Count();i++) 
		    	{
		    	td.Num(iChannelsArray[i]);
		    	tempArray->AppendL(td);
		    	}
			}	
	    }
    
	if(codecList)
		{
		codecList->SetArrayL(iArrCodecs);	
		codecList->SetArrayExternalOwnership(ETrue);
		tempArray = codecList->DesCArray();
		
		//get supported codecs
		char *pch = NULL;
		TUint32 unValue;
		TRAP(err,iAppUi->GetSupportedCodecsArrayL(iCodecsArray));
		if(err==KErrNotSupported)
			{
			_LIT(KCodecSupport, "Codecs not supported");
			User::InfoPrint(KCodecSupport);			
			iCodecListIdx = -1;
			}
		else
			{
			tempArray->Reset();
			//copy from array to  CDescCArray 
		    for(TInt i = 0; i < iCodecsArray.Count(); i++)
		    	{
		    	unValue = iCodecsArray[i].FourCC();
		    	pch =reinterpret_cast<char*>(&unValue);
		    	td.Zero();
		    	td.Append(pch[0]);
		    	td.Append(pch[1]);
		    	td.Append(pch[2]);
		    	td.Append(pch[3]);
		    	tempArray->AppendL(td);
		    	}
			}	
		}
	//Seed the controls with their appropriate value.
    if(iBitRateIdx >= 0)
    	{
    	bitrateList->SetCurrentItem(iBitRateIdx);
    	}
    if(iSampleRateIdx >= 0)
    	{
    	sampleratesList->SetCurrentItem(iSampleRateIdx);	
    	}
    if(iChannelListIdx >= 0)
    	{
    	channelsList->SetCurrentItem(iChannelListIdx);
    	}
    if(iCodecListIdx >= 0)
    	{
    	codecList->SetCurrentItem(iCodecListIdx);
    	}
    }



CMVSConfigAudioFormatDialog::CMVSConfigAudioFormatDialog(CMVSAppUi* aAppUi,TUid aControllerId)
	:iContollerUid(aControllerId) , iAppUi(aAppUi)
     {
     }



TBool CMVSConfigAudioFormatDialog::OkToExitL(TInt aButtonId)
     {
     if(aButtonId == EMVSButtonCancel)
     	{
     	return ETrue;
     	}
	 //the values have to go to store
 	 // Get a downcasted pointer to each of the controls
     CEikChoiceList* bitrateList = static_cast<CEikChoiceList*>(Control(EMVSSetBitrate));
     
     CEikChoiceList* codecList = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectCodec));

     CEikChoiceList* channelsList = static_cast<CEikChoiceList*>(Control(EMVSSetNoOfChannels));

     CEikChoiceList* sampleratesList = static_cast<CEikChoiceList*>(Control(EMVSSetSamplerate));
     
     if(bitrateList && codecList && channelsList && sampleratesList)
	     {
	     if(iBitRateIdx != -1)
	     	{
	     	iBitRateIdx	= bitrateList->CurrentItem();	
	     	}
	     if(iSampleRateIdx != -1)
	     	{
	     	iSampleRateIdx = sampleratesList->CurrentItem();	
	     	}
	     if(iChannelListIdx != -1)
	     	{
	     	iChannelListIdx = channelsList->CurrentItem();	
	     	}
	     if(iCodecListIdx != -1)
	     	{
	     	iCodecListIdx	= codecList->CurrentItem();	
	     	}
	     iAppUi->WriteAudioDataL(this,iContollerUid);
	 	 if(aButtonId == EMVSButtonUpdate)
		 	{
		 	if(iBitRateIdx >= 0)
		 		{
		 	 	iAppUi->SetBitRateL(iBitRatesArray[iBitRateIdx]);	
		 	 	}
		 	if(iSampleRateIdx >= 0) 	 
			 	{
			 	iAppUi->SetSampleRateL(iSampleRatesArray[iSampleRateIdx]);	
			 	}
		 	if(iChannelListIdx >= 0)
			 	{
			 	iAppUi->SetChannelsL(iChannelsArray[iChannelListIdx]);		
			 	}
			if(iCodecListIdx >= 0)
			 	{
				iAppUi->SetCodecsL(iCodecsArray[iCodecListIdx]);			
				}
		 	}	
	     }
	 return ETrue;
     }


TKeyResponse CMVSConfigAudioFormatDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if(aKeyEvent.iCode == EKeyEnter && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonUpdate);
		}
	return CEikDialog::OfferKeyEventL(aKeyEvent,aType);
	}



void CMVSConfigAudioFormatDialog::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteInt8L(iBitRateIdx);
	aStream.WriteInt8L(iSampleRateIdx);
	aStream.WriteInt8L(iChannelListIdx);
	aStream.WriteInt8L(iCodecListIdx);
	}


void CMVSConfigAudioFormatDialog::InternalizeL(RReadStream& aStream)
	{
	iBitRateIdx = aStream.ReadInt8L();
    iSampleRateIdx = aStream.ReadInt8L();
    iChannelListIdx = aStream.ReadInt8L();
    iCodecListIdx = aStream.ReadInt8L();
	}


void CMVSConfigAudioFormatDialog::HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType)
	{
	CEikChoiceList* bitrateList = static_cast<CEikChoiceList*>(Control(EMVSSetBitrate));
	 
	CEikChoiceList* codecList = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectCodec));

	CEikChoiceList* channelsList = static_cast<CEikChoiceList*>(Control(EMVSSetNoOfChannels));

	CEikChoiceList* sampleratesList = static_cast<CEikChoiceList*>(Control(EMVSSetSamplerate));
    
	CEikDialog::HandleControlEventL(aControl,aEventType);
	if(bitrateList && codecList && channelsList && sampleratesList)
		{
		if(iSettings == aControl && aEventType == EEventStateChanged)
			{
			if(iRestoreSettings == EFalse)
				{
				iRestoreSettings=ETrue;
				}
			else
				{
				iRestoreSettings=EFalse;	
				}
			if(iRestoreSettings)
				{
				if(iBitRateIdx >= 0)
					{
					bitrateList->SetCurrentItem(iBitRateIdx);
	    			bitrateList->DrawDeferred();
					}
	    	
	    		if(iSampleRateIdx >= 0)
	    			{
	    			sampleratesList->SetCurrentItem(iSampleRateIdx);	
	    			sampleratesList->DrawDeferred();
	    			}
	    		
	       		if(iChannelListIdx >= 0)
	       			{
	       			channelsList->SetCurrentItem(iChannelListIdx);
	       			channelsList->DrawDeferred();
	       			}
	    		
	    		if(iCodecListIdx >= 0)
	    			{
	    			codecList->SetCurrentItem(iCodecListIdx);
	    			codecList->DrawDeferred();
	    			}	
				}
			else
				{
				if(iBitRateIdx >= 0)
					{
					bitrateList->SetCurrentItem(0);
	    			bitrateList->DrawDeferred();
					}
	    	
	    		if(iSampleRateIdx >= 0)
	    			{
	    			sampleratesList->SetCurrentItem(0);	
	    			sampleratesList->DrawDeferred();
	    			}
	    		
	       		if(iChannelListIdx >= 0)
	       			{
	       			channelsList->SetCurrentItem(0);
	       			channelsList->DrawDeferred();
	       			}
	    		
	    		if(iCodecListIdx >= 0)
	    			{
	    			codecList->SetCurrentItem(0);
	    			codecList->DrawDeferred();
	    			}
				}
	    	return;
			}	
		}
	}


CMVSConfigAudioFormatDialog::~CMVSConfigAudioFormatDialog()
	{
	iBitRatesArray.Close();
	iSampleRatesArray.Close();
	iChannelsArray.Close();
	iCodecsArray.Close();
	delete iArrSampleRates; 	  	          
	delete iArrBitRates;
	delete iArrChannels;
	delete iArrCodecs;
	}

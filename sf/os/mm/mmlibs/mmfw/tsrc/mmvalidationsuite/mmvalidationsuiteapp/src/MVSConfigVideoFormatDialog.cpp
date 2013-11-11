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

#include "MVSConfigVideoFormatDialog.h"
#include "MVSApp.hrh"
#include <badesca.h>

#include "MVSAppUI.h" 

void CMVSConfigVideoFormatDialog::SetupDialogLD(CMVSAppUi* aAppUi,TUid aControllerId,CSettingsManager* aSettingsManager)
	{
	CMVSConfigVideoFormatDialog* dialog = new (ELeave) CMVSConfigVideoFormatDialog(aAppUi,aControllerId); 
    CleanupStack::PushL(dialog);
    dialog->ConstructL();
    aSettingsManager->ReadVideoDataL(dialog,aControllerId);
    CleanupStack::Pop(dialog);
    dialog->ExecuteLD(R_MVS_DIALOG_VIDEOPLUGINS);
    }


void CMVSConfigVideoFormatDialog::ConstructL() 
	{
	iMimeTypeArray = new(ELeave) CDesC8ArrayFlat(8);
	iArrFrameRate = new(ELeave) CDesCArrayFlat(8);	
	iArrFrameSize = new(ELeave) CDesCArrayFlat(8);
	iArrVideoMimeType = new(ELeave) CDesCArrayFlat(8);	
 	}

void CMVSConfigVideoFormatDialog::PreLayoutDynInitL()
    {
   	CEikChoiceList* frameRateCtl = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectVideoFrameRate));	
	 
	CEikChoiceList* frameSizeCtl = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectVideoFrameSize));	
	
	CEikChoiceList* mimeTypeCtl = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectVideoMimeCodec));	
		  		
    CEikCheckBox* restoresettings = static_cast<CEikCheckBox*>(Control(EMVSRestoreVideoSettings));	
    
    CEikCheckBox* audioenable = static_cast<CEikCheckBox*>(Control(EMVSEnableAudio));	
    
    TBuf<10> td;
    TBuf<20> sd;
 	TInt err = 0;
	CDesCArray*	tempArray = NULL;
	if(frameRateCtl)
		{
		frameRateCtl->SetArrayL(iArrFrameRate);
		frameRateCtl->SetArrayExternalOwnership(ETrue);
		tempArray = frameRateCtl->DesCArray(); 	   		
		//get supported frame rates
		TRAP(err,iAppUi->GetSupportedFrameRatesL(iFrameRateArray));	
		if(err == KErrNotSupported)	
			{
			_LIT(KFrameRateSupport, "Frame Rates not supported");
			User::InfoPrint(KFrameRateSupport);			
			iFrameRateIdx = -1;
			}
		else
			{
			//copy from array to  CDescCArray 
		    iFrameRateIdx = 0;
		    for(TInt i = 0; i < iFrameRateArray.Count();i++) 
			    {
				td.Num(iFrameRateArray[i]);
				tempArray->AppendL(td);
			    }
			}	
		}
		
	if(frameSizeCtl)
	    {
	    _LIT(comma,",");
	    frameSizeCtl->SetArrayL(iArrFrameSize);
	    frameSizeCtl->SetArrayExternalOwnership(ETrue);
	    tempArray = frameSizeCtl->DesCArray();
		//get supported sample rates
		TRAP(err,iAppUi->GetSupportedFrameSizeArrayL(iFrameSizeArray));
		if(err == KErrNotSupported)
			{
			_LIT(KSampleRateSupport, "Sample rates not supported");
			User::InfoPrint(KSampleRateSupport);			
			iFrameSizeIdx = -1;
			}
		else
			{
			//copy from array to  CDescCArray
			iFrameSizeIdx = 0; 
		    for(TInt i = 0; i < iFrameSizeArray.Count();i++) 
		    	{
		    	//code to append both TWidth and THeight into single cell of the Array
		    	TSize temp=iFrameSizeArray[i];
		    	TInt width=temp.iWidth;
		    	TInt height=temp.iHeight;
		    	td.Num(width);
		    	TInt8 size=td.Length()+1;
		    	sd.Insert(0,td);
		    	sd.Append(comma);
		    	td.Num(height);
		    	sd.Append(td);
		    	tempArray->AppendL(sd);
		    	td.Delete(0,size+5);//the Delete()function adjusts this value to ensure that no data
		    	sd.Delete(0,size+5);//beyond the end of the descriptor data area is deleted.
		     	}
			}	
	    }
	    
    if(mimeTypeCtl)
		{
	  	//get supported frame rates
		mimeTypeCtl->SetArrayL(iArrVideoMimeType);
		mimeTypeCtl->SetArrayExternalOwnership(ETrue);
		TRAP(err,iAppUi->GetSupportedVideoTypesL(*iMimeTypeArray));	
		TInt counter=iMimeTypeArray->Count();
		for(TInt i=0; i<counter; i++)
			{
			TBuf16<16> temp;
			temp.Copy(iMimeTypeArray->MdcaPoint(i)); 		
			iArrVideoMimeType->AppendL(temp);
			}
		if(err == KErrNotSupported)	
			{
			_LIT(KFrameRateSupport, "Frame Rates not supported");
			User::InfoPrint(KFrameRateSupport);			
			}
		}

    if(restoresettings)
    	{
    	iSettings = restoresettings;
    	restoresettings->SetObserver(this);	
    	}
    if(audioenable)
    	{
    	iAudioEnableCtl = audioenable;
    	iAudioEnable = EFalse;
    	audioenable->SetObserver(this);
    	}
    }


CMVSConfigVideoFormatDialog::CMVSConfigVideoFormatDialog(CMVSAppUi* aAppUi,TUid aControllerId)
	:iContollerUid(aControllerId) , iAppUi(aAppUi)
    {
    } 


TKeyResponse CMVSConfigVideoFormatDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if(aKeyEvent.iCode == EKeyEnter && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonUpdate);
		}
	return CEikDialog::OfferKeyEventL(aKeyEvent,aType);
	}


TBool CMVSConfigVideoFormatDialog::OkToExitL(TInt aButtonId)
    {
    if(aButtonId == EMVSButtonCancel)
    	{
    	return ETrue;
    	}   
    CEikChoiceList* frameRateCtl = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectVideoFrameRate));	
	 
	CEikChoiceList* frameSizeCtl = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectVideoFrameSize));
	
	CEikChoiceList* mimeTypeCtl = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectVideoMimeCodec));

	if(frameRateCtl && frameSizeCtl && mimeTypeCtl)
		{
		if(iFrameRateIdx != -1)
			{
			iFrameRateIdx = frameRateCtl->CurrentItem();	
			}
		if(iFrameSizeIdx != -1)
			{
			iFrameSizeIdx = frameSizeCtl->CurrentItem();	
			}
	    iMimeTypeIdx = mimeTypeCtl->CurrentItem();
		iAppUi->WriteVideoDataL(this,iContollerUid);	
		}
	if(aButtonId == EMVSButtonUpdate)
		{
		if(iFrameRateIdx >= 0)
			{
		 	iAppUi->SetVideoFrameRateL(iFrameRateArray[iFrameRateIdx]);	
		 	}
		if(iFrameSizeIdx >= 0) 	 
			{
		 	iAppUi->SetVideoFrameSizeL(iFrameSizeArray[iFrameSizeIdx]);	
		 	}
		if(iAudioEnable)
			{
			iAppUi->SetAudioEnabledL(iAudioEnable);
			}
		if(iMimeTypeIdx)
			{
			iAppUi->SetVideoTypeL((*iMimeTypeArray)[iMimeTypeIdx]);
			}
		}
	return ETrue;
    }


void CMVSConfigVideoFormatDialog::InternalizeL(RReadStream& aStream)
	{
	iFrameRateIdx = aStream.ReadInt8L();
    iFrameSizeIdx = aStream.ReadInt8L();
	}


void CMVSConfigVideoFormatDialog::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteInt8L(iFrameRateIdx);
	aStream.WriteInt8L(iFrameSizeIdx);	
	}

void CMVSConfigVideoFormatDialog::HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType)
	{
	CEikChoiceList* frameRateCtl = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectVideoFrameRate));	
	 
	CEikChoiceList* frameSizeCtl = static_cast<CEikChoiceList*>(Control(EMVSCmdSelectVideoFrameSize));	
	    
	CEikDialog::HandleControlEventL(aControl,aEventType);
	if(frameRateCtl && frameSizeCtl)
		{
		if(iSettings == aControl && aEventType == EEventStateChanged)
			{
			if(iRestoreSettings == EFalse)
				{
				iRestoreSettings = ETrue;
				}
			else
				{
				iRestoreSettings = EFalse;	
				}
			if(iRestoreSettings)
				{
				if(iFrameRateIdx>0)
					{
					frameRateCtl->SetCurrentItem(iFrameRateIdx);
					frameRateCtl->DrawDeferred();
					}
				if(iFrameSizeIdx>0)
					{
					frameSizeCtl->SetCurrentItem(iFrameSizeIdx);	
					frameSizeCtl->DrawDeferred();
					}
				}
			else
				{
				if(iFrameRateIdx >= 0)
					{
					frameRateCtl->SetCurrentItem(0);
	    			frameRateCtl->DrawDeferred();
					}
	    	
	    		if(iFrameSizeIdx >= 0)
	    			{
	    			frameSizeCtl->SetCurrentItem(0);	
	    			frameSizeCtl->DrawDeferred();
	    			}
				}
			return;
	    	}
	    if(iAudioEnableCtl == aControl && aEventType == EEventStateChanged)
	    	{
	    	if(iAudioEnable == EFalse)
	    		{
	    		iAudioEnable = ETrue;
	    		}
	    	else
	    		{
	    		iAudioEnable = EFalse;
	    		}
	    	return;
	    	}
		}
	}


CMVSConfigVideoFormatDialog::~CMVSConfigVideoFormatDialog()
	{
	iFrameRateArray.Close();
	iFrameSizeArray.Close();
	delete iArrFrameRate; 	  	          
	delete iArrFrameSize;
	delete iMimeTypeArray;
	delete iArrVideoMimeType;
	}

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

#include "MVSSystemInfoDialog.h"
#include "MVSApp.hrh"
#include "MVSAppUI.h"
#include <techview/eikedwin.h>

const TInt KControllerCaptionLength = 23;
const TInt KCodecCaptionLength = 8;

void CMVSSystemInfoDialog::SetupDialogLD(CMVSAppUi* aAppUi)
	{
    CMVSSystemInfoDialog* dialog = new (ELeave) CMVSSystemInfoDialog(aAppUi);
    CleanupStack::PushL(dialog);
    dialog->ConstructL();
    CleanupStack::Pop(dialog);
    dialog->ExecuteLD(R_NEW_MVS_DIALOG_SYSTEMINFO);
    }


void CMVSSystemInfoDialog::PreLayoutDynInitL()
    {
    // Get a downcasted pointer to the controls
    CDesCArrayFlat* arrCtlrCodec = NULL;
    arrCtlrCodec = new(ELeave)CDesCArrayFlat(8);
    CEikEdwin* controlPtr = static_cast<CEikEdwin*>(Control(EMVSCmdSystemInfo));
    if(controlPtr)
	    {
	    controlPtr->SetReadOnly(TRUE);
	    controlPtr->SetBackgroundColorL(KRgbGray);
	    TInt len = 0;
	    TInt counter = 0;
	    //get the array of plugins - audio
		iAppUi->GetSystemInfoL(arrCtlrCodec);
		for(counter=0; counter < arrCtlrCodec->MdcaCount(); counter++)
			{
			len =len + (*arrCtlrCodec)[counter].Length();//getting the total length required to be assigned to HBufC
			}
		_LIT(KControllerCaption,"AVAILABLE CONTROLLERS");
		TInt finalLength =len + counter + KControllerCaptionLength;//adding space for delimiter,
																   //so that every controller is displayed in a newline.	
		HBufC* controllerList = HBufC::NewLC(finalLength);
		controllerList->Des().Append(KControllerCaption);
		controllerList->Des().Append(CEditableText::EParagraphDelimiter);
		for(TInt i=0; i < arrCtlrCodec->MdcaCount(); i++)     
			{
			TBuf<256> data((*arrCtlrCodec)[i]);
			controllerList->Des().Append(data);			
		 	controllerList->Des().Append(CEditableText::EParagraphDelimiter);
		 	}
		//get the array of plugins - video
	    arrCtlrCodec->Reset();
	    RArray<TFourCC> nArrCodecs;
		char *pch = NULL;
		TUint32 unValue;
		TBuf<40> td;
		TInt err = 0;
		TRAP(err,iAppUi->GetSupportedCodecsArrayL(nArrCodecs));
	    if(err==KErrNotSupported)
			{
			TPtrC infoPtr = controllerList->Des();
			controlPtr->SetTextL(&infoPtr);
			CleanupStack::PopAndDestroy(controllerList);
			}
		else
			{
			_LIT(KCodecCaption,"CODECS");
			TInt availableCodecCount = nArrCodecs.Count();
			TInt codecLength = availableCodecCount * 4;//4 becuase thats the size of each codec
			TInt codecDataFinalLength = codecLength + finalLength + availableCodecCount +KCodecCaptionLength;//Final length of the HBuf
			HBufC* newList = controllerList->ReAllocL(codecDataFinalLength);
			CleanupStack::Pop(controllerList);//This is the old pointer which was pushed into the cleanupstack before ReAlloc.
			CleanupStack::PushL(newList);//The new pointer pointing to the new memory allocated after ReAlloc.
		    newList->Des().Append(CEditableText::EParagraphDelimiter);
		    newList->Des().Append(KCodecCaption);
		    newList->Des().Append(CEditableText::EParagraphDelimiter);
		    for(TInt i = 0; i < nArrCodecs.Count(); i++)
		    	{
			   	unValue = nArrCodecs[i].FourCC();
		    	pch =(char*)&unValue;
		    	td.Zero();
		    	td.Append(pch[0]);
		    	td.Append(pch[1]);
		    	td.Append(pch[2]);
		    	td.Append(pch[3]);
		    	newList->Des().Append(td);
		      	newList->Des().Append(CEditableText::EParagraphDelimiter);
		      	}
			TPtrC infoPtr = newList->Des();
			controlPtr->SetTextL(&infoPtr);
			CleanupStack::PopAndDestroy(newList);
			}
	    //set the contents of CEikEdwin - system info control
	    nArrCodecs.Close();
	    delete arrCtlrCodec; 	
	    }
    }



CMVSSystemInfoDialog::CMVSSystemInfoDialog(CMVSAppUi* aAppUi)
: iAppUi(aAppUi)
	{
    //Nothing to do here - all done in initialisation list
    }


CMVSSystemInfoDialog::~CMVSSystemInfoDialog()
	{
	}

void CMVSSystemInfoDialog::ConstructL()
	{
	}



TBool CMVSSystemInfoDialog::OkToExitL(TInt/* aButtonId*/)
    {
    //Nothing to be taken from the user here,
	//Just display the information.
	return ETrue;
	}

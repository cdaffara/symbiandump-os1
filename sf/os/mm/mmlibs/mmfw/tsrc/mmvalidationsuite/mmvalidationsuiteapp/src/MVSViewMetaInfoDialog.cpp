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


#include "MVSViewMetaInfoDialog.h"
#include "MVSApp.hrh"
#include <techview/eiklabel.h>
#include "MVSAppUI.h"
#include "MVSConstants.h"



void CMVSViewMetaInfoDialog::SetupDialogLD(RPointerArray<CMMFMetaDataEntry> aMetaArray,
                                        CMVSAppUi* aAppUi)
     {
     CMVSViewMetaInfoDialog* dialog = new (ELeave) CMVSViewMetaInfoDialog(aAppUi);
     CleanupStack::PushL(dialog); 
     dialog->ConstructL(aMetaArray);
     CleanupStack::Pop(dialog);
     dialog->ExecuteLD(R_MVS_DIALOG_SETMETAINFO);
     }


void CMVSViewMetaInfoDialog::PreLayoutDynInitL()
	{
    CEikEdwin* myEdwin1 = static_cast<CEikEdwin*>(Control(EMVSMeta1));
    //author
    if(myEdwin1)
    	{
    	myEdwin1->SetReadOnly(TRUE);
    	myEdwin1->SetBackgroundColorL(KRgbGray);
    	}
    
    CEikEdwin* myEdwin2 = static_cast<CEikEdwin*>(Control(EMVSMeta2));
    if(myEdwin2)
    	{
    	myEdwin2->SetReadOnly(TRUE);
    	myEdwin2->SetBackgroundColorL(KRgbGray);
    	}
    
    //copyright
    CEikEdwin* myEdwin3 = static_cast<CEikEdwin*>(Control(EMVSMeta3));
    if(myEdwin3)
    	{
    	myEdwin3->SetReadOnly(TRUE);
    	myEdwin3->SetBackgroundColorL(KRgbGray);
    	}
    
    //revision
    CEikEdwin* myEdwin4 = static_cast<CEikEdwin*>(Control(EMVSMeta4));
    if(myEdwin4)
    	{
    	myEdwin4->SetReadOnly(TRUE);
    	myEdwin4->SetBackgroundColorL(KRgbGray);
    	}
    
    //category
    CEikEdwin* myEdwin5 = static_cast<CEikEdwin*>(Control(EMVSMeta5));
    if(myEdwin5)
    	{
    	myEdwin5->SetReadOnly(TRUE);
    	myEdwin5->SetBackgroundColorL(KRgbGray);
    	}
    
    //comments
    CEikEdwin* myEdwin6 = static_cast<CEikEdwin*>(Control(EMVSMeta6));
	if(myEdwin6)
    	{
    	myEdwin6->SetReadOnly(TRUE);
    	myEdwin6->SetBackgroundColorL(KRgbGray);
    	}
    
    TRAPD(err,iAppUi->GetMetaDataL(iMetaArray));
	TInt metaArrayCount = iMetaArray.Count();
	if(err == KErrNone && metaArrayCount > 0)
		{
		for(TInt counter = 0; counter< iMetaArray.Count(); counter++)
	    	{
	        TBufC<256> temp(iMetaArray[counter]->Name());
	        TBufC<256> tempval(iMetaArray[counter]->Value());
	        if((iMetaArray[counter]->Name()).CompareF(KTitle)==0)
	        	{
	            myEdwin1->SetTextL(&(iMetaArray[counter]->Value()));
	            iTitlePos = counter;
	            }
	        if((iMetaArray[counter]->Name()).CompareF(KAuthor)==0)
	            {
	            myEdwin2->SetTextL(&(iMetaArray[counter]->Value()));
	            iAuthorPos = counter;
	            }
	        if((iMetaArray[counter]->Name()).CompareF(KCopyrt)==0)
	            {
	            myEdwin3->SetTextL(&(iMetaArray[counter]->Value()));
	            iCopyrightPos = counter;
	            }
	        if((iMetaArray[counter]->Name()).CompareF(KRevision)==0)
	            {
	            myEdwin4->SetTextL(&(iMetaArray[counter]->Value()));
	            iRevisionPos = counter;
	            }
	        if((iMetaArray[counter]->Name()).CompareF(KCategory)==0)
	            {
	            myEdwin5->SetTextL(&(iMetaArray[counter]->Value()));
	            iCategoryPos = counter;
	            }
	        if((iMetaArray[counter]->Name()).CompareF(KComments)==0)
	            {
	            myEdwin6->SetTextL(&(iMetaArray[counter]->Value()));
	            iCommentsPos = counter;
	            }
	        }	
		}
    
     
    if(err == KErrNotSupported || metaArrayCount == 0)
		{
		User::InfoPrint(KNotSupported);
		}
     }




CMVSViewMetaInfoDialog::CMVSViewMetaInfoDialog(CMVSAppUi* aAppUi)
: iTitlePos(-1), iAuthorPos(-1), iCopyrightPos(-1), iRevisionPos(-1),\
iCategoryPos(-1), iCommentsPos(-1), iAppUi(aAppUi)
    {
     
    }
    
    
void CMVSViewMetaInfoDialog::ConstructL(RPointerArray<CMMFMetaDataEntry> aMetaArray)
    {
    iMetaArray.Reset();
    TInt noElements = aMetaArray.Count();
    for(TInt counter = 0; counter < noElements; counter++)
    	{
        iMetaArray.Append(aMetaArray[counter]);
        }
    }

TBool CMVSViewMetaInfoDialog::OkToExitL(TInt /*aButtonId*/)
     {
     //Delete the metadata entries populated in the array
     iMetaArray.ResetAndDestroy();
     return ETrue;
     }

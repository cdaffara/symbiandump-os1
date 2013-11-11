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

#include "MVSEditMetaInfoDialog.h"
#include "MVSApp.hrh"
#include <techview/eiklabel.h>
#include "MVSConstants.h"

void CMVSEditMetaInfoDialog::SetupDialogLD(RPointerArray<CMMFMetaDataEntry> aMetaArray,
                                        CMVSAppUi* aAppUi)
	{
    CMVSEditMetaInfoDialog* dialog = new (ELeave) CMVSEditMetaInfoDialog(aAppUi);
    CleanupStack::PushL(dialog); 
    dialog->ConstructL(aMetaArray);
    CleanupStack::Pop(dialog);
    dialog->ExecuteLD(R_MVS_DIALOG_SETMETAINFO);
    }


void CMVSEditMetaInfoDialog::ConstructL(RPointerArray<CMMFMetaDataEntry> aMetaArray)
    {
    iMetaArray.Reset();
    TInt noElements = aMetaArray.Count();
    for(TInt counter = 0; counter < noElements; counter++)
    	{
        iMetaArray.Append(aMetaArray[counter]);
        }
    }


CMVSEditMetaInfoDialog::CMVSEditMetaInfoDialog(CMVSAppUi* aAppUi)
: iTitlePos(-1), iAuthorPos(-1), iCopyrightPos(-1), iRevisionPos(-1),\
iCategoryPos(-1), iCommentsPos(-1), iAppUi(aAppUi)
    {
     
    }


TBool CMVSEditMetaInfoDialog::OkToExitL(TInt aButtonId)
    {
    if(aButtonId == EMVSButtonCancel)
    	{
       	iMetaArray.Close();
       	return ETrue;
       	}
    //title
    CEikEdwin* myEdwin1 = static_cast<CEikEdwin*>(Control(EMVSMeta1));
     
    //author
    CEikEdwin* myEdwin2 = static_cast<CEikEdwin*>(Control(EMVSMeta2));
     
    //copyright
    CEikEdwin* myEdwin3 = static_cast<CEikEdwin*>(Control(EMVSMeta3));
     
    //revision
    CEikEdwin* myEdwin4 = static_cast<CEikEdwin*>(Control(EMVSMeta4));
     
    //category
    CEikEdwin* myEdwin5 = static_cast<CEikEdwin*>(Control(EMVSMeta5));
     
    //comments
    CEikEdwin* myEdwin6 = static_cast<CEikEdwin*>(Control(EMVSMeta6));
	
	TBuf<256> myBuf;
    CMMFMetaDataEntry* newMetaEntry = CMMFMetaDataEntry::NewL();
	if(iTitlePos != -1)
    	{
        myEdwin1->GetText(myBuf);
        iMetaArray[iTitlePos]->SetValueL(myBuf);
        }
    else
        {
        newMetaEntry->SetNameL(KTitle);
        myEdwin1->GetText(myBuf);
        newMetaEntry->SetValueL(myBuf);
        iMetaArray.Append(newMetaEntry);
        }
    if(iAuthorPos != -1)
        {
        myEdwin2->GetText(myBuf);
        iMetaArray[iAuthorPos]->SetValueL(myBuf);
        }
    else
        {
        newMetaEntry->SetNameL(KAuthor);
        myEdwin1->GetText(myBuf);
        newMetaEntry->SetValueL(myBuf);
        iMetaArray.Append(newMetaEntry);
        }
    if(iCopyrightPos != -1)
        {
        myEdwin3->GetText(myBuf);
        iMetaArray[iCopyrightPos]->SetValueL(myBuf);
        }
    else
        {
        newMetaEntry->SetNameL(KCopyrt);
        myEdwin3->GetText(myBuf);
        newMetaEntry->SetValueL(myBuf);
        iMetaArray.Append(newMetaEntry);
        }
    if(iRevisionPos != -1)
        {
        myEdwin4->GetText(myBuf);
        iMetaArray[iRevisionPos]->SetValueL(myBuf);
        }
    else
        {
        newMetaEntry->SetNameL(KRevision);
        myEdwin4->GetText(myBuf);
        newMetaEntry->SetValueL(myBuf);
        iMetaArray.Append(newMetaEntry);
        }
    if(iCategoryPos != -1)
        {
        myEdwin5->GetText(myBuf);
        iMetaArray[iCategoryPos]->SetValueL(myBuf);
        }
    else
        {
        newMetaEntry->SetNameL(KCategory);
        myEdwin5->GetText(myBuf);
        newMetaEntry->SetValueL(myBuf);
        iMetaArray.Append(newMetaEntry);
        }
    if(iCommentsPos != -1)
        {
        myEdwin6->GetText(myBuf);
        iMetaArray[iCommentsPos]->SetValueL(myBuf);
        }
    else
        {
        newMetaEntry->SetNameL(KComments);
        myEdwin6->GetText(myBuf);
        newMetaEntry->SetValueL(myBuf);
        iMetaArray.Append(newMetaEntry);
        }
    TInt err;
    TInt count=iMetaArray.Count();
    TRAP(err,iAppUi->SetMetaDataL(iMetaArray));
    if(err==KErrNotSupported)
      	{
      	User::InfoPrint(KNotSupported);
      	}
    delete newMetaEntry;
    return ETrue; 	    
    }

CMVSEditMetaInfoDialog::~CMVSEditMetaInfoDialog()
	{
	iMetaArray.Close();
	}

// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_LoopbackCwrapgetsmsplist.h"

 
CTestGetSmspList* CTestGetSmspList::NewLC(RMobileSmsMessaging& aSmsMessaging)
	{
    CTestGetSmspList* r=new(ELeave) CTestGetSmspList(aSmsMessaging);
    CleanupStack::PushL(r);
    r->ConstructL();
	CleanupStack::Pop();
    return r;
    }
      
CTestGetSmspList::CTestGetSmspList(RMobileSmsMessaging& aSmsMessaging)
    : CActive(EPriorityNormal), iSmsMessaging(aSmsMessaging)
    {
	}
      
void CTestGetSmspList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneSmspList::NewL(iSmsMessaging);
    }
         
CTestGetSmspList::~CTestGetSmspList()
    {
	iRetrieve->Cancel();
	delete iRetrieve;
	}
      
void CTestGetSmspList::Start()
    {
    iRetrieve->Start(iStatus);
    SetActive();
    }       

CMobilePhoneSmspList* CTestGetSmspList::RetrieveList()
	{
	CMobilePhoneSmspList* ptr=NULL;
	TRAP_IGNORE(ptr=iRetrieve->RetrieveListL());		// trap but ignore leaves
	return ptr;
	}
          
void CTestGetSmspList::RunL()
    {
	TInt ret=iStatus.Int();
    User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}

void CTestGetSmspList::DoCancel()
    {
    iRetrieve->Cancel();
    }

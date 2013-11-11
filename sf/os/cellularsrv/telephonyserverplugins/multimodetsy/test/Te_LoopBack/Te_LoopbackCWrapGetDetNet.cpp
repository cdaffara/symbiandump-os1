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

#include "Te_LoopbackCWrapGetDetNet.h"

// Wrapper object for CRetrieveMobilePhoneDetectedNetworks.
// The CRetrieveMobilePhoneDetectedNetworks active object needs
// both a CActiveScheduler installed and for it to have been started. 
// As a result of starting the CActiveScheduler, another AO must be 
// implemented to stop the CActiveScheduler when the
// CRetrieveMobilePhoneDetetctedNetworks active object completes.
// Can extend for other classes too.
CWrapperGetDetectedNetworks* CWrapperGetDetectedNetworks::NewL(RMobilePhone& aPhone)
	{
    CWrapperGetDetectedNetworks* r = new(ELeave) CWrapperGetDetectedNetworks(aPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
	CleanupStack::Pop();
    return r;
    }

CWrapperGetDetectedNetworks::CWrapperGetDetectedNetworks(RMobilePhone& aPhone)
    : CActive(EPriorityNormal), iPhone3(aPhone)
    {
	}

void CWrapperGetDetectedNetworks::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone3);
    }
         
CWrapperGetDetectedNetworks::~CWrapperGetDetectedNetworks()
    {
	if (iRetrieve) iRetrieve->Cancel();
	delete iRetrieve;
	}

CMobilePhoneNetworkList* CWrapperGetDetectedNetworks::RetrieveList()
	{
	CMobilePhoneNetworkList* ptr=NULL;
	TRAP_IGNORE(ptr=iRetrieve->RetrieveListL());		
	return(ptr);
	}

void CWrapperGetDetectedNetworks::Start()
    {
    iRetrieve->Start(iStatus);

    SetActive();
    }       

void CWrapperGetDetectedNetworks::RunL()
    {
	TInt ret = iStatus.Int();
    User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}

void CWrapperGetDetectedNetworks::DoCancel()
    {
	iRetrieve->Cancel();
    }       

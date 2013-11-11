// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <networking/panuiinterfaces.h>

#include "napuplinkauthorisationhelper.h"


CNapUplinkAuthorisationHelper::CNapUplinkAuthorisationHelper(MNapUplinkAuthorisationRequester& aNapUplinkAuthorisationRequester)
  : CActive(CActive::EPriorityStandard),
    iNapUplinkAuthorisationRequester(aNapUplinkAuthorisationRequester)
    {
	CActiveScheduler::Add(this);
    }
    
CNapUplinkAuthorisationHelper::~CNapUplinkAuthorisationHelper()
	{
	Cancel();
	iNotifier.Close();
	}

void CNapUplinkAuthorisationHelper::PerformNapUplinkAuthorisationL()
	{
	User::LeaveIfError(iNotifier.Connect());

	iNotifier.StartNotifierAndGetResponse(iStatus, KBTPanNapUplinkAuthorisationNotifierUid, iConnectionListPckg, iResultBuf);
	SetActive();
	}
	

void CNapUplinkAuthorisationHelper::RunL()
	{
	iNapUplinkAuthorisationRequester.MnuarNapUplinkAuthorisationHelperComplete(iStatus.Int(), iResultBuf());
	}
	
	
void CNapUplinkAuthorisationHelper::DoCancel()
	{
	// Cancel the notifier.
	iNotifier.CancelNotifier(KBTPanNapUplinkAuthorisationNotifierUid);
	iNapUplinkAuthorisationRequester.MnuarNapUplinkAuthorisationHelperComplete(KErrCancel, EDisallowNewNapConnection);
	}

TPanConnectionList& CNapUplinkAuthorisationHelper::PanConnectionList()
	{
	return iConnectionListPckg();
	}

// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "DialogProcessor.h"
#include "DbAccess.h"
//#include <comms-infras/dbaccess.h>

CDialogProcessor* CDialogProcessor::NewL(TInt)
	{
	CDialogProcessor* self = new(ELeave)CDialogProcessor();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CDialogProcessor::CDialogProcessor()
	{
	}

void CDialogProcessor::ConstructL()
	{
	CCommsDbAccess *aDatabase = CCommsDbAccess::NewL();
	CleanupStack::PushL( aDatabase );
	aDatabase->GetCurrentSettingsL(iSettings, ECommDbConnectionDirectionOutgoing, 1);
	CleanupStack::PopAndDestroy( aDatabase );

	TCallBack callback(DialogCb, this);
	iDialogCb = new(ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);
	}

CDialogProcessor::~CDialogProcessor()
	{

	delete iDialogCb;
	}

TInt CDialogProcessor::DialogCb(TAny* aThisPtr)
	{
	CDialogProcessor* self = static_cast<CDialogProcessor*>(aThisPtr);
	self->DialogComplete();
	return KErrNone;
	}

void CDialogProcessor::DialogComplete()
	{

	switch(iAction)
		{
		case ESelectConnection:
			iObserver->MDPOSelectComplete(KErrNone, iSettings);
			break;
		case ESelectModemAndLocation:
			iObserver->MDPOSelectModemAndLocationComplete(KErrNone, iSettings);
			break;
		case EReconnect:
			iObserver->MDPOReconnectComplete(KErrNone);
			break;
		case EWarnNewConnection:
			iObserver->MDPOWarnComplete(KErrNone, ETrue);
			break;
		default:
			User::Invariant();
		}
	}

void CDialogProcessor::SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs&)
	{

	iAction = ESelectConnection;
	iObserver = &aObserver;
	iDialogCb->CallBack();
	}

void CDialogProcessor::SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs&, TInt)
	{

	iAction = ESelectConnection;
	iObserver = &aObserver;
	iDialogCb->CallBack();
	}

void CDialogProcessor::SelectModemAndLocation(MDialogProcessorObserver& aObserver)
	{

	iAction = ESelectModemAndLocation;
	iObserver = &aObserver;
	iDialogCb->CallBack();
	}

void CDialogProcessor::Reconnect(MDialogProcessorObserver& aObserver)
	{

	iAction = EReconnect;
	iObserver = &aObserver;
	iDialogCb->CallBack();
	}

void CDialogProcessor::WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs&, const TDesC*, const TIspConnectionNames*, TInt)
	{

	iAction = EWarnNewConnection;
	iObserver = &aObserver;
	iDialogCb->CallBack();
	}


//
//  Default implementation of MDialogProcessorObserver
//

void MDialogProcessorObserver::MDPOSelectComplete(TInt, const TConnectionSettings&)
	{
	User::Invariant();
	}

void MDialogProcessorObserver::MDPOSelectModemAndLocationComplete(TInt, const TConnectionSettings&)
	{
	User::Invariant();
	}

void MDialogProcessorObserver::MDPOWarnComplete(TInt, TBool)
	{
	User::Invariant();
	}

void MDialogProcessorObserver::MDPOLoginComplete(TInt)
	{
	User::Invariant();
	}

void MDialogProcessorObserver::MDPOAuthenticateComplete(TInt)
	{
	User::Invariant();
	}

void MDialogProcessorObserver::MDPOReconnectComplete(TInt)
	{
	User::Invariant();
	}

void MDialogProcessorObserver::MDPOReadPctComplete(TInt)
	{
	User::Invariant();
	}

void MDialogProcessorObserver::MDPODestroyPctComplete(TInt)
	{
	User::Invariant();
	}

void MDialogProcessorObserver::MDPOQoSWarningComplete(TInt, TBool)
	{
	User::Invariant();
	}


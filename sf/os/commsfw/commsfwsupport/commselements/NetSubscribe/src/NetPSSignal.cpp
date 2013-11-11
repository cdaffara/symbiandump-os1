// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "NetPSSignal.h"

using namespace NetSubscribe;

void CPSSubscribe::RegisterNewSignalL(TEventClientData& aData, const SSignalId& aSignalId )
	{
	CPSSignal* pSignal = CPSSignal::NewLC(aSignalId);
	pSignal->RegisterClientL( aData );
	iSignals.AppendL(pSignal);
	CleanupStack::Pop(pSignal);
	}

CPSSignal* CPSSignal::NewLC( const SSignalId& aSignalId )
	{
	CPSSignal* pSignal = new(ELeave)CPSSignal(aSignalId);
	CleanupStack::PushL(pSignal);
	pSignal->ConstructL();
	return pSignal;
	}

void CPSSignal::ConstructL()
/**
 * subscribes to the property
 */
	{
	User::LeaveIfError(iProperty.Attach(TUid::Uid(iSignalId.iTypeId.iType), iSignalId.iHandle ));

	iProperty.Subscribe(iStatus);
	SetActive();
	}

CPSSignal::~CPSSignal()
	{
	Cancel();
	iProperty.Close();
	delete iUsrData;
	}

void CPSSignal::DoCancel()
	{
	iProperty.Cancel();
	}

void CPSSignal::RunL()
	{
	TInt nErr = iStatus.Int();
	//set active first since a leave is trapped by RunError and subscription continues
	iProperty.Subscribe(iStatus);
	SetActive();
	//get data
	TBuf8<KMaxLength> buf;
	User::LeaveIfError( iProperty.Get(buf) );
	TPtrC8 ptr( buf );
	if ( iUsrData )
		{
		User::LeaveIfError( iSignalId.iTypeId.Check( ptr ) );
		User::LeaveIfError(iUsrData->Load( ptr ));
		}
	else
		{
		iUsrData = Meta::SMetaDataECom::LoadL(ptr);
		User::LeaveIfError( iSignalId.iTypeId == iUsrData->GetTypeId() ? KErrNone : KErrArgument );
		}
	iData.DispatchL( iUsrData, nErr );
	}


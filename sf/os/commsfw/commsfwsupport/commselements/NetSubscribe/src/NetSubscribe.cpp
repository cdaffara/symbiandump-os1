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

/**
 * @internalTechnology
 */
EXPORT_C void TEvent::SubscribeL(CNetSubscribe& aSubscribe, const SSignalId& aSignalId )
	{
	TEventClientData data( *this );
	aSubscribe.RegisterSignalL( data, aSignalId );
	};

/**
 * @internalTechnology
 */
EXPORT_C void TEvent::Cancel(CNetSubscribe& aSubscribe)
	{
	aSubscribe.DeRegisterSignal( this );
	}

/**
 * @internalTechnology
 */
EXPORT_C CNetSubscribe* CNetSubscribe::NewL( const Meta::STypeId& aTransportId )
	{
	if ( aTransportId.iUid.iUid != KTransportUid )
		{
		User::Leave(KErrNotFound);
		}
	
	CNetSubscribe* newInst = NULL;
		
	switch (aTransportId.iType)
		{
		case EPublishSubscribe:
			newInst = new(ELeave)CPSSubscribe();
			break;
		default:
			User::Leave(KErrNotFound);
		}
	return newInst;
	}

/**
 * @internalTechnology
 */
CNetSubscribe::CNetSubscribe()
	{
	}

/**
 * @internalTechnology
 */
EXPORT_C CNetSubscribe::~CNetSubscribe()
	{
	while ( iSignals.Count() )
		{
		TInt n = iSignals.Count() - 1;
		XSignal* pSignal = iSignals[n];
		delete pSignal;
		iSignals.Remove(n);
		}
	iSignals.Close();
	}

/**
 * @internalTechnology
 */
void CNetSubscribe::DeRegisterSignal(TEventBase* aEvent)
	{
	TInt count = iSignals.Count();
	for (TInt n = count - 1; n >= 0; n--)
		{
		XSignal* pSignal = iSignals[n];
		if ( pSignal->DeRegisterClient( aEvent ) == 0 )
			{
			iSignals.Remove(n);
			count--;
			delete pSignal;
			}
		}
	}

/**
 * @internalTechnology
 */
void CNetSubscribe::RegisterSignalL(TEventClientData& aData, const SSignalId& aSignalId )
	{
	TInt count = iSignals.Count();
	for (TInt n = 0; n < count;n++)
		{
		XSignal* pSignal = iSignals[n];
		if ( aSignalId == pSignal->GetSignalId() )
			{
			pSignal->RegisterClientL( aData );
			return;
			}
		}
	RegisterNewSignalL(aData, aSignalId);
	}



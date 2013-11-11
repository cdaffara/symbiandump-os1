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

void TEventClientData::SignalErrorL( TInt aError )
	{
	if ( Event()->iErrHandler )
		{
		(Event()->iErrHandler)(Event()->iThis, aError);
		}
	};

void TEventDataArray::DispatchL( const Meta::SMetaData* aData, TInt aErr ) 
	{
	for (TInt n = 0; n < Count(); n++)
		{
		TEventClientData& data = (*this)[n];
		if ( aErr == KErrNone )
			{
			data.SignalL( aData );
			}
		else
			{
			data.SignalErrorL( aErr );
			}
		}
	}

TInt TEventDataArray::RegisterL( const TEventClientData& aData ) 
	{
	if (Find( aData.Event() ) != KErrNotFound)
		{
		User::Leave(KErrAlreadyExists);
		}
	AppendL( aData );
	return Count();
	}

TInt TEventDataArray::DeRegister( const TEventBase* aEvent )
	{
	TInt n = Find( aEvent );
	if ( n != KErrNotFound )
		{
		Remove(n);
		}
	return Count();
	}

TInt TEventDataArray::Find( const TEventBase* aEvent )
	{
	TInt n = 0;
	TInt count = Count();
	for (; n < count && (*this)[n].Event() != aEvent; n++)
		{
		}
	return n < Count() ? n : KErrNotFound;
	}
	
XSignal::~XSignal()
	/**
	virtual destructor so that the right instance destructor can be reached
	*/
	{
	iData.Close();
	}
	
TInt CSignalActive::RunError(TInt aError)
	{
	for (TInt n = 0; n < iData.Count(); n++)
		{
		TRAP_IGNORE(iData[n].SignalErrorL( aError ));
		}
	return KErrNone;
	}


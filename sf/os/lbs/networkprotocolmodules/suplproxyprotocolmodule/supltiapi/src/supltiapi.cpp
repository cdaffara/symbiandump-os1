// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SuplTiApi.cpp

#include <ecom/ecom.h>

#include <lbs/supltiapi.h>


// CLbsSuplTiApi
//

EXPORT_C CLbsSuplTiApi* CLbsSuplTiApi::NewL(MLbsSuplTiObserver& aObserver, TUid aEcomModuleId)
	{
	TAny* ptr = REComSession::CreateImplementationL(aEcomModuleId,
													_FOFF(CLbsSuplTiApi, iDtor_ID_Key),
													reinterpret_cast<TAny*>(&aObserver));

	return reinterpret_cast<CLbsSuplTiApi*>(ptr);
	}

EXPORT_C CLbsSuplTiApi::CLbsSuplTiApi(MLbsSuplTiObserver& aObserver)
	:iSuplTiObserver(aObserver)
	{
	}

EXPORT_C void CLbsSuplTiApi::ConstructL()
	{
	}

EXPORT_C CLbsSuplTiApi::~CLbsSuplTiApi()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

void CLbsSuplTiApi::RequestLocation(const TLbsNetSessionId& /*aSessionId*/,
									const TLbsNetPosRequestOptionsBase& /*aOptions*/,
									const TLbsNetPosRequestMethod& /*aMethod*/)
	{
	}

TInt CLbsSuplTiApi::CancelRequest(const TLbsNetSessionId& /*aSessionId*/)
	{
	return KErrNotSupported;
	}

void CLbsSuplTiApi::CloseSession()
	{
	}

TInt CLbsSuplTiApi::GetPosition(const TLbsNetSessionId& /*aSessionId*/,
								TPositionInfoBase& /*aPositionInfo*/)
	{
	return KErrNotSupported;
	}


// MLbsSuplTiObserver
//

void MLbsSuplTiObserver::RequestComplete(	TInt /*aReason*/,
											const TLbsNetSessionId& /*aSessionId*/)
	{
	}


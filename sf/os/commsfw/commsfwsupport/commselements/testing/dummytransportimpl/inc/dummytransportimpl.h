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

/**
 @file 
 @internalTechnology
*/

#ifndef SYMBIAN_DUMMYTRANSPORT_IMPL_H
#define SYMBIAN_DUMMYTRANSPORT_IMPL_H

#include <elements/nm_transport.h>

namespace DummyTransport
{

class CDummyTransportEntry;
NONSHARABLE_CLASS(CDummyTransportImpl) : public CActive,
                                         public Messages::MTransportSender,
                                         public Messages::MTransportReceiver
	{
public:
	IMPORT_C static CDummyTransportImpl* NewL(const Meta::CMetaDataVirtualCtorInPlace& aVirtCtor);
	IMPORT_C ~CDummyTransportImpl();

private:
	//From MTransportSender
	virtual void PostMessage(const Messages::TRuntimeCtxId& aPostFrom, const Messages::TRuntimeCtxId& aPostTo, const Meta::SMetaData& aMessage);

	//From MTransportReceiver
	virtual void RegisterAddress(Messages::TRuntimeCtxId& aCookie);
	virtual void DeregisterAddress(Messages::TRuntimeCtxId& aCookie);
	virtual TInt RegisteredCount() const;
	virtual TInt VerifyDestination(const Messages::TRuntimeCtxId& aDestination);

private:
	CDummyTransportImpl(const Meta::CMetaDataVirtualCtorInPlace& aVirtCtor);

private:
	// CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	const Meta::CMetaDataVirtualCtorInPlace& iVirtCtor;
	RPointerArray<CDummyTransportEntry> iEntries; //It is not efficient to use table as fifo queue but in this implementation we do not care really
	CDummyTransportEntry* iCurrentEntry;
	TInt iRegisteredCount;
	TUint8 iPointerSalt;
	};

} //namespace DummyTransport

#endif
//SYMBIAN_DUMMYTRANSPORT_IMPL_H


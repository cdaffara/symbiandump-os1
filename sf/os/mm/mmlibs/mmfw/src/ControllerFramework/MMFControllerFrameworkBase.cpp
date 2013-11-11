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

#include "mmfcontrollerframeworkbase.h"


// TMMFEvent
EXPORT_C TMMFEvent::TMMFEvent(TUid aEventType, TInt aErrorCode)
	: iEventType(aEventType), iErrorCode(aErrorCode)
	{
	}

EXPORT_C TMMFEvent::TMMFEvent()
	: iEventType(KNullUid), iErrorCode(KErrNone)
	{
	}


// CMMFMetaDataEntry
EXPORT_C CMMFMetaDataEntry* CMMFMetaDataEntry::NewL(const TDesC& aName, const TDesC& aValue)
	{
	CMMFMetaDataEntry* s = new(ELeave) CMMFMetaDataEntry;
	CleanupStack::PushL(s);
	s->ConstructL(aName, aValue);
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CMMFMetaDataEntry* CMMFMetaDataEntry::NewL(const CMMFMetaDataEntry& aOther)
	{
	CMMFMetaDataEntry* s = new(ELeave) CMMFMetaDataEntry;
	CleanupStack::PushL(s);
	s->ConstructL(aOther.Name(), aOther.Value());
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CMMFMetaDataEntry* CMMFMetaDataEntry::NewL()
	{
	CMMFMetaDataEntry* s = new(ELeave) CMMFMetaDataEntry;
	CleanupStack::PushL(s);
	s->ConstructL(KNullDesC, KNullDesC);
	CleanupStack::Pop(s);
	return s;
	}

CMMFMetaDataEntry::CMMFMetaDataEntry()
	{
	}

void CMMFMetaDataEntry::ConstructL(const TDesC& aName, const TDesC& aValue)
	{
	iName = aName.AllocL();
	iValue = aValue.AllocL();
	}

EXPORT_C CMMFMetaDataEntry::~CMMFMetaDataEntry()
	{
	delete iName;
	delete iValue;
	}

EXPORT_C void CMMFMetaDataEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iName;
	aStream << *iValue;
	}

EXPORT_C void CMMFMetaDataEntry::InternalizeL(RReadStream& aStream)
	{
	delete iName;
	iName = NULL;
	iName = HBufC::NewL(aStream, KMaxTInt);//Max length of HBuf is taken from stream - KMaxTInt just used as upper limit

	delete iValue;
	iValue = NULL;
	iValue = HBufC::NewL(aStream, KMaxTInt);
	}

EXPORT_C void CMMFMetaDataEntry::SetNameL(const TDesC& aName)
	{
	delete iName;
	iName = NULL;
	iName = aName.AllocL();
	}

EXPORT_C void CMMFMetaDataEntry::SetValueL(const TDesC& aValue)
	{
	delete iValue;
	iValue = NULL;
	iValue = aValue.AllocL();
	}

EXPORT_C const TDesC& CMMFMetaDataEntry::Name() const
	{
	return *iName;
	}

EXPORT_C const TDesC& CMMFMetaDataEntry::Value() const
	{
	return *iValue;
	}





EXPORT_C CMMFUrlParams* CMMFUrlParams::NewL(const TDesC& aUrl, TInt aIAPId)
	{
	CMMFUrlParams* s = CMMFUrlParams::NewLC(aUrl, aIAPId);
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CMMFUrlParams* CMMFUrlParams::NewLC(const TDesC& aUrl, TInt aIAPId)
	{
	CMMFUrlParams* s = new(ELeave) CMMFUrlParams;
	CleanupStack::PushL(s);
	s->ConstructL(aUrl, aIAPId);
	return s;
	}

EXPORT_C CMMFUrlParams* CMMFUrlParams::NewLC(RReadStream& aStream)
	{
	CMMFUrlParams* s = new(ELeave) CMMFUrlParams;
	CleanupStack::PushL(s);
	s->ConstructL(aStream);
	return s;
	}

EXPORT_C void CMMFUrlParams::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt16L(iUrl->Length());
	aStream.WriteL(*iUrl);
	aStream.WriteInt32L(iIAPId);
	}

EXPORT_C CBufFlat* CMMFUrlParams::ExternalizeToCBufFlatLC() const
	{
	CBufFlat* buffer = CBufFlat::NewL(32);
	CleanupStack::PushL(buffer);
	RBufWriteStream s;
	s.Open(*buffer);
	CleanupClosePushL(s);
	ExternalizeL(s);
	CleanupStack::PopAndDestroy();//s
	// Leave buffer on the CleanupStack
	return buffer;
	}


CMMFUrlParams::~CMMFUrlParams()
	{
	delete iUrl;
	}

CMMFUrlParams::CMMFUrlParams()
	{
	}

void CMMFUrlParams::ConstructL(const TDesC& aUrl, TInt aIAPId)
	{
	iUrl = aUrl.AllocL();
	iIAPId = aIAPId;
	}

void CMMFUrlParams::ConstructL(RReadStream& aStream)
	{
	TInt size = aStream.ReadInt16L();
	iUrl = HBufC::NewL(size);
	TPtr temp (iUrl->Des());
	aStream.ReadL(temp, size);
	iIAPId = aStream.ReadInt32L();
	}

EXPORT_C const TDesC& CMMFUrlParams::Url() const
	{
	return *iUrl;
	}

EXPORT_C TInt CMMFUrlParams::IAPId() const
	{
	return iIAPId;
	}

EXPORT_C TBool CMMFUrlParams::UseIAPId() const
	{
	return (iIAPId != KUseDefaultIap);
	}


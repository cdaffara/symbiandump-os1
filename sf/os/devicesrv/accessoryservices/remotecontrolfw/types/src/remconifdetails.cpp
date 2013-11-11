// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include <remcon/remconifdetails.h>
#include <s32strm.h>


EXPORT_C CRemConInterfaceDetailsArray* CRemConInterfaceDetailsArray::NewL()
	{
	CRemConInterfaceDetailsArray* self = new(ELeave) CRemConInterfaceDetailsArray();
	return self;
	}

CRemConInterfaceDetailsArray::CRemConInterfaceDetailsArray()
	{
	}

CRemConInterfaceDetailsArray::~CRemConInterfaceDetailsArray()
	{
	iArray.ResetAndDestroy();
	}

EXPORT_C TArray<CRemConInterfaceDetails*> CRemConInterfaceDetailsArray::Array() const
	{
	return iArray.Array();
	}
	
EXPORT_C CRemConInterfaceDetailsArray* CRemConInterfaceDetailsArray::InternalizeL(RReadStream& aStream)
	{
	CRemConInterfaceDetailsArray* self = new(ELeave) CRemConInterfaceDetailsArray();
	CleanupStack::PushL(self);
	TInt count = aStream.ReadInt32L();
	for(TInt ix=0; ix<count; ++ix)
		{
		CRemConInterfaceDetails* details = CRemConInterfaceDetails::InternalizeL(aStream);
		CleanupStack::PushL(details);
		self->iArray.AppendL(details);
		CleanupStack::Pop(details);
		}
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CRemConInterfaceDetailsArray::ExternalizeL(RWriteStream& aStream, TRemConClientType aTypeFilter)
	{
	TInt arrayCount = iArray.Count();
	TInt count = arrayCount;
#ifdef _DEBUG
	TInt checkCount = arrayCount;
#endif //_DEBUG
	
	if(aTypeFilter != ERemConClientTypeUndefined)
		{
		count = 0;
		for(TInt ix=0; ix<arrayCount; ++ix)
			{
			if(iArray[ix]->Type() == aTypeFilter)
				{
				++count;
				}
			}
#ifdef _DEBUG
		checkCount = count;
#endif // _DEBUG
		}
	aStream.WriteInt32L(count);
	for(TInt ix=0; ix<arrayCount; ++ix)
		{
		if(aTypeFilter == ERemConClientTypeUndefined || aTypeFilter == iArray[ix]->Type())
			{
			iArray[ix]->ExternalizeL(aStream);
#ifdef _DEBUG
			--checkCount;
#endif // _DEBUG
			}
		}
	ASSERT(checkCount == 0);
	}
	
EXPORT_C void CRemConInterfaceDetailsArray::AppendL(CRemConInterfaceDetails* aDetails)
	{
	iArray.AppendL(aDetails);
	}



EXPORT_C CRemConInterfaceDetails* CRemConInterfaceDetails::NewLC(TUid aUid, TRemConClientType aType, TBool aIsBulk, CRemConInterfaceBase* aInterface, const TDesC8& aFeatures)
	{
	CRemConInterfaceDetails* self = new(ELeave) CRemConInterfaceDetails(aUid, aType, aIsBulk, aInterface);
	CleanupStack::PushL(self);
	self->ConstructL(aFeatures);
	return self;
	}

CRemConInterfaceDetails::CRemConInterfaceDetails(TUid aUid, TRemConClientType aType, TBool aIsBulk, CRemConInterfaceBase* aInterface)
	: iInterface(aInterface)
	, iUid(aUid)
	, iType(aType)
	, iBulk(aIsBulk)
	{
	}

CRemConInterfaceDetails::~CRemConInterfaceDetails()
	{
	ASSERT(!iInterface);
	iFeatures.Close();
	}

void CRemConInterfaceDetails::ConstructL(const TDesC8& aFeatures)
	{
	if(aFeatures.Length() % sizeof(TUint) != 0)
		{
		User::Leave(KErrCorrupt);
		}
	iFeatures.Assign(aFeatures.AllocL());
	}

void CRemConInterfaceDetails::AddFeatures(RBuf8& aFeatures)
	{
	iFeatures.Close();
	iFeatures.Swap(aFeatures);
	}

EXPORT_C void CRemConInterfaceDetails::DeleteFeatures()
	{
	iFeatures.Close();
	}

EXPORT_C CRemConInterfaceBase*& CRemConInterfaceDetails::Interface()
	{
	return iInterface;
	}

EXPORT_C TUid CRemConInterfaceDetails::Uid() const
	{
	return iUid;
	}

EXPORT_C TBool CRemConInterfaceDetails::IsBulk() const
	{
	return iBulk;
	}

EXPORT_C TRemConClientType CRemConInterfaceDetails::Type() const
	{
	return iType;
	}

EXPORT_C void CRemConInterfaceDetails::GetRemConInterfaceFeaturesL(RArray<TUint>& aRemConInterfaceFeatures) const
	{
	aRemConInterfaceFeatures.Reset();
	CleanupClosePushL(aRemConInterfaceFeatures);

	TInt numFeatures = iFeatures.Length() / sizeof(TUint);
	for(TInt i=0; i<numFeatures; ++i)
		{
		TPtrC8 ptr = iFeatures.Mid(i*sizeof(TUint), sizeof(TUint));
		TPckgC<TUint>& val = *reinterpret_cast<TPckgC<TUint>*>(&ptr);
		aRemConInterfaceFeatures.AppendL(val());
		}

	CleanupStack::Pop(&aRemConInterfaceFeatures);
	}


CRemConInterfaceDetails* CRemConInterfaceDetails::InternalizeL(RReadStream& aStream)
	{
	TUid uid = TUid::Uid(aStream.ReadInt32L());
	TRemConClientType type = static_cast<TRemConClientType>(aStream.ReadInt32L());
	TBool bulk = aStream.ReadUint8L();
	TInt featureSize = aStream.ReadInt32L();
	RBuf8 features;
	features.CreateL(featureSize);
	features.CleanupClosePushL();
	aStream.ReadL(features);
	CRemConInterfaceDetails* self = new(ELeave) CRemConInterfaceDetails(uid, type, bulk, NULL);
	self->AddFeatures(features);
	CleanupStack::PopAndDestroy(&features);
	return self;
	}

void CRemConInterfaceDetails::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteInt32L(iUid.iUid);
	aStream.WriteInt32L(iType);
	aStream.WriteUint8L(!!iBulk);
	aStream.WriteInt32L(iFeatures.Size());
	aStream.WriteL(iFeatures);
	}

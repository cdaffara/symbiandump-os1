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
// Graphics Resource - logical channel implementation
//

#include <kernel/kern_priv.h>
#include <sgresource/sgextension.h>
#include "sgdeviceimpl.h"

TInt DSgChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& aVersion)
	{
	TVersion version = SgExtension::Version();
	if (aVersion.iMajor != version.iMajor || aVersion.iMinor > version.iMinor)
		{
		return KErrNotSupported;
		}
	return Kern::MutexCreate(iMutex, KNullDesC, KMutexOrdGeneral7);
	}

DSgChannel::~DSgChannel()
	{
	if (iMutex)
		{
		iMutex->Close(NULL);
		}
	for (TInt i = 0; i < iResources.Count(); ++i)
		{
		iResources[i].iResource->Close();
		}
	iResources.Close();
	}

TInt DSgChannel::Request(TInt aFunction, TAny* a1, TAny* a2)
	{
	switch (aFunction)
		{
	case RSgDevice::EControlCreateResource:
		{
		RSgDevice::TCreateResourceArgs args;
		kumemget32(&args, a1, sizeof(RSgDevice::TCreateResourceArgs));
		TInt metaDataSize, maxMetaDataSize;
		Kern::KUDesInfo(*args.iMetaData, metaDataSize, maxMetaDataSize);
		__ASSERT_ALWAYS(metaDataSize <= KSgMaxMetaDataSize, Panic(ESgPanicMetaDataSizeTooBig));
		__ASSERT_ALWAYS(args.iDataSize >= 0, Panic(ESgPanicDataSizeNegative));
		TBuf8<KSgMaxMetaDataSize> metaData;
		Kern::KUDesGet(metaData, *args.iMetaData);
		TUint64 id;
		TInt err = CreateResource(args.iAttributes, metaData, args.iDataSize, id);
		kumemput32(a2, &id, sizeof(TUint64));
		return err;
		}
	case RSgDevice::EControlOpenResource:
		{
		TUint64 id;
		kumemget32(&id, a1, sizeof(TUint64));
		return OpenResource(id);
		}
	case RSgDevice::EControlCloseResource:
		{
		TUint64 id;
		kumemget32(&id, a1, sizeof(TUint64));
		return CloseResource(id);
		}
	case RSgDevice::EControlResourceAttributes:
		{
		TUint64 id;
		kumemget32(&id, a1, sizeof(TUint64));
		return static_cast<TInt>(ResourceAttributes(id));
		}
	case RSgDevice::EControlGetResourceMetaData:
		{
		TUint64 id;
		kumemget32(&id, a1, sizeof(TUint64));
		TBuf8<KSgMaxMetaDataSize> metaData;
		TInt err = GetResourceMetaData(id, metaData);
		if (err == KErrNone)
			{
			Kern::InfoCopy(*static_cast<TDes8*>(a2), metaData);
			}
		return err;
		}
	case RSgDevice::EControlResourceDataAddress:
		{
		TUint64 id;
		kumemget32(&id, a1, sizeof(TUint64));
		return reinterpret_cast<TInt>(ResourceDataAddress(id));
		}
	case RSgDevice::EControlResourceDataSize:
		{
		TUint64 id;
		kumemget32(&id, a1, sizeof(TUint64));
		return ResourceDataSize(id);
		}
	case RSgDevice::EControlGlobalResourceCount:
		return GlobalResourceCount();
	case RSgDevice::EControlLocalGraphicsMemoryUsed:
		return LocalGraphicsMemoryUsed();
	case RSgDevice::EControlGlobalGraphicsMemoryUsed:
		return GlobalGraphicsMemoryUsed();
	default:
		return KErrNotSupported;
		}
	}

TInt DSgChannel::CreateResource(TUint32 aAttribs, const TDesC8& aMetaData, TInt aDataSize, TUint64& aId)
	{
	aId = 0;
	NKern::ThreadEnterCS();
	DSgResource* resource;
	TInt err = SgExtension::CreateResource(aAttribs, aMetaData, aDataSize, resource);
	if (err != KErrNone)
		{
		NKern::ThreadLeaveCS();
		return err;
		}
	TInt handle = Kern::MakeHandleAndOpen(NULL, resource->DataChunk(), EOwnerProcess);
	if (handle < 0)
		{
		resource->Close();
		NKern::ThreadLeaveCS();
		return handle;
		}
	Kern::MutexWait(*iMutex);
	err = iResources.InsertInOrder(TResourceListItem(resource, handle), Compare);
	Kern::MutexSignal(*iMutex);
	if (err != KErrNone)
		{
		(void)Kern::CloseHandle(NULL, handle);
		resource->Close();
		NKern::ThreadLeaveCS();
		return err;
		}
	NKern::ThreadLeaveCS();
	aId = resource->Id();
	return KErrNone;
	}

TInt DSgChannel::OpenResource(TUint64 aId)
	{
	if (aId == 0)
		{
		return KErrArgument;
		}
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iMutex);
	TInt i = iResources.FindInOrder(aId, Compare);
	if (i >= 0)
		{
		Kern::MutexSignal(*iMutex);
		NKern::ThreadLeaveCS();
		return KErrAlreadyExists;
		}
	DSgResource* resource;
	TInt err = SgExtension::FindAndOpenResource(aId, resource);
	if (err != KErrNone)
		{
		Kern::MutexSignal(*iMutex);
		NKern::ThreadLeaveCS();
		return err;
		}
	TInt handle = Kern::MakeHandleAndOpen(NULL, resource->DataChunk(), EOwnerProcess);
	if (handle < 0)
		{
		resource->Close();
		Kern::MutexSignal(*iMutex);
		NKern::ThreadLeaveCS();
		return handle;
		}
	err = iResources.InsertInOrder(TResourceListItem(resource, handle), Compare);
	if (err != KErrNone)
		{
		(void)Kern::CloseHandle(NULL, handle);
		resource->Close();
		Kern::MutexSignal(*iMutex);
		NKern::ThreadLeaveCS();
		return err;
		}
	Kern::MutexSignal(*iMutex);
	NKern::ThreadLeaveCS();
	return KErrNone;
	}

TInt DSgChannel::CloseResource(TUint64 aId)
	{
	if (aId == 0)
		{
		return KErrArgument;
		}
	TInt err = KErrNotFound;
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iMutex);
	TInt i = iResources.FindInOrder(aId, Compare);
	if (i >= 0)
		{
		(void)Kern::CloseHandle(NULL, iResources[i].iChunkHandle);
		iResources[i].iResource->Close();
		iResources.Remove(i);
		err = KErrNone;
		}
	Kern::MutexSignal(*iMutex);
	NKern::ThreadLeaveCS();
	return err;
	}

TUint32 DSgChannel::ResourceAttributes(TUint64 aId) const
	{
	if (aId == 0)
		{
		return 0;
		}
	TUint32 attribs = 0;
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iMutex);
	TInt i = iResources.FindInOrder(aId, Compare);
	if (i >= 0)
		{
		attribs = iResources[i].iResource->Attributes();
		}
	Kern::MutexSignal(*iMutex);
	NKern::ThreadLeaveCS();
	return attribs;
	}

TInt DSgChannel::GetResourceMetaData(TUint64 aId, TDes8& aMetaData) const
	{
	if (aId == 0)
		{
		return KErrArgument;
		}
	TInt err = KErrNotFound;
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iMutex);
	TInt i = iResources.FindInOrder(aId, Compare);
	if (i >= 0)
		{
		err = iResources[i].iResource->GetMetaData(aMetaData);
		}
	Kern::MutexSignal(*iMutex);
	NKern::ThreadLeaveCS();
	return err;
	}

TAny* DSgChannel::ResourceDataAddress(TUint64 aId) const
	{
	if (aId == 0)
		{
		return NULL;
		}
	TAny* addr = NULL;
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iMutex);
	TInt i = iResources.FindInOrder(aId, Compare);
	if (i >= 0)
		{
		addr = Kern::ChunkUserBase(iResources[i].iResource->DataChunk(), &Kern::CurrentThread());
		}
	Kern::MutexSignal(*iMutex);
	NKern::ThreadLeaveCS();
	return addr;
	}

TInt DSgChannel::ResourceDataSize(TUint64 aId) const
	{
	if (aId == 0)
		{
		return KErrArgument;
		}
	TInt ret = KErrNotFound;
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iMutex);
	TInt i = iResources.FindInOrder(aId, Compare);
	if (i >= 0)
		{
		ret = iResources[i].iResource->DataSize();
		}
	Kern::MutexSignal(*iMutex);
	NKern::ThreadLeaveCS();
	return ret;
	}

TInt DSgChannel::GlobalResourceCount() const
	{
	NKern::ThreadEnterCS();
	TInt ret = SgExtension::GlobalResourceCount();
	NKern::ThreadLeaveCS();
	return ret;
	}

TInt DSgChannel::LocalGraphicsMemoryUsed() const
	{
	TInt ret = 0;
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iMutex);
	TInt n = iResources.Count();
	for (TInt i = 0; i < n; ++i)
		{
		ret += iResources[i].iResource->DataChunk()->Size();
		}
	Kern::MutexSignal(*iMutex);
	NKern::ThreadLeaveCS();
	return ret;
	}

TInt DSgChannel::GlobalGraphicsMemoryUsed() const
	{
	NKern::ThreadEnterCS();
	TInt ret = SgExtension::GlobalGraphicsMemoryUsed();
	NKern::ThreadLeaveCS();
	return ret;
	}

DSgChannel::TResourceListItem::TResourceListItem(DSgResource* aResource, TInt aChunkHandle)
	: iResource(aResource), iChunkHandle(aChunkHandle)
	{
	}

TInt DSgChannel::Compare(const TUint64* aId, const TResourceListItem& aResourceListItem)
	{
	return DSgResource::Compare(aId, *aResourceListItem.iResource);
	}

TInt DSgChannel::Compare(const TResourceListItem& aResourceListItem1, const TResourceListItem& aResourceListItem2)
	{
	return DSgResource::Compare(*aResourceListItem1.iResource, *aResourceListItem2.iResource);
	}

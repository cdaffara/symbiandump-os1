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
// Graphics Resource - logical device driver internal functionality
//

#ifndef SGDEVICEIMPL_H
#define SGDEVICEIMPL_H

#include <kernel/kernel.h>
#include <sgresource/sgdevice.h>

enum TSgDevicePanicReason
	{
	ESgPanicMetaDataSizeTooBig = 1,
	ESgPanicDataSizeNegative = 2
	};

void Panic(TSgDevicePanicReason aReason);

class DSgDevice: public DLogicalDevice
	{
public:
	// From DLogicalDevice
	TInt Install();
	void GetCaps(TDes8& aDes) const;
	TInt Create(DLogicalChannelBase*& aChannel);
	};

class DSgResource;

class DSgChannel: public DLogicalChannelBase
	{
public:
	~DSgChannel();
	// From DLogicalChannelBase
	TInt DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVersion);
	TInt Request(TInt aFunction, TAny* a1, TAny* a2);
private:
	TInt CreateResource(TUint32 aAttribs, const TDesC8& aMetaData, TInt aDataSize, TUint64& aId);
	TInt OpenResource(TUint64 aId);
	TInt CloseResource(TUint64 aId);
	TUint32 ResourceAttributes(TUint64 aId) const;
	TInt GetResourceMetaData(TUint64 aId, TDes8& aMetaData) const;
	TAny* ResourceDataAddress(TUint64 aId) const;
	TInt ResourceDataSize(TUint64 aId) const;
	TInt GlobalResourceCount() const;
	TInt LocalGraphicsMemoryUsed() const;
	TInt GlobalGraphicsMemoryUsed() const;
private:
	class TResourceListItem
		{
	public:
		TResourceListItem(DSgResource* aResource, TInt aChunkHandle);
	public:
		DSgResource* iResource;
		TInt iChunkHandle;
		};
private:
	static TInt Compare(const TUint64* aId, const TResourceListItem& aResourceListItem);
	static TInt Compare(const TResourceListItem& aResourceListItem1, const TResourceListItem& aResourceListItem2);
private:
	DMutex* iMutex;
	RArray<TResourceListItem> iResources;
	};

#endif // SGDEVICEIMPL_H

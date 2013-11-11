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
// Graphics Resource - logical device driver inline functions
//

#ifndef SGDEVICE_INL
#define SGDEVICE_INL

inline TVersion SgDeviceVersionRequired()
	{
	return TVersion(1, 1, 0);
	}

#ifndef __KERNEL_MODE__

inline TInt RSgDevice::Connect()
	{
	return DoCreate(KSgDeviceName, SgDeviceVersionRequired(), KNullUnit, NULL, NULL);
	}

inline TInt RSgDevice::CreateResource(TUint32 aAttribs, const TDesC8& aMetaData, TInt aDataSize, TUint64& aId)
	{
	TCreateResourceArgs args;
	args.iAttributes = aAttribs;
	args.iMetaData = &aMetaData;
	args.iDataSize = aDataSize;
	return DoControl(EControlCreateResource, &args, &aId);
	}

inline TInt RSgDevice::OpenResource(TUint64 aId)
	{
	return DoControl(EControlOpenResource, &aId);
	}

inline TInt RSgDevice::CloseResource(TUint64 aId)
	{
	return DoControl(EControlCloseResource, &aId);
	}

inline TUint32 RSgDevice::ResourceAttributes(TUint64 aId)
	{
	return static_cast<TUint32>(DoControl(EControlResourceAttributes, &aId));
	}

inline TInt RSgDevice::GetResourceMetaData(TUint64 aId, TDes8& aMetaData)
	{
	return DoControl(EControlGetResourceMetaData, &aId, &aMetaData);
	}

inline TAny* RSgDevice::ResourceDataAddress(TUint64 aId)
	{
	return reinterpret_cast<TAny*>(DoControl(EControlResourceDataAddress, &aId));
	}

inline TInt RSgDevice::ResourceDataSize(TUint64 aId)
	{
	return DoControl(EControlResourceDataSize, &aId);
	}

inline TInt RSgDevice::GlobalResourceCount()
	{
	return DoControl(EControlGlobalResourceCount);
	}

inline TInt RSgDevice::LocalGraphicsMemoryUsed()
	{
	return DoControl(EControlLocalGraphicsMemoryUsed);
	}

inline TInt RSgDevice::GlobalGraphicsMemoryUsed()
	{
	return DoControl(EControlGlobalGraphicsMemoryUsed);
	}

#endif // __KERNEL_MODE__

#endif // SGDEVICE_INL

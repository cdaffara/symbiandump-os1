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
// Graphics Resource - logical device driver
//

#ifndef SGDEVICE_H
#define SGDEVICE_H

#include <e32des8.h>
#include <sgresource/sgcommon.h>

_LIT(KSgDeviceName, "sgdevice");

inline TVersion SgDeviceVersionRequired();

class RSgDevice: public RBusLogicalChannel
	{
	friend class DSgChannel;
public:
	inline TInt Connect();
	inline TInt CreateResource(TUint32 aAttribs, const TDesC8& aMetaData, TInt aDataSize, TUint64& aId);
	inline TInt OpenResource(TUint64 aId);
	inline TInt CloseResource(TUint64 aId);
	inline TUint32 ResourceAttributes(TUint64 aId);
	inline TInt GetResourceMetaData(TUint64 aId, TDes8& aMetaData);
	inline TAny* ResourceDataAddress(TUint64 aId);
	inline TInt ResourceDataSize(TUint64 aId);
	inline TInt GlobalResourceCount();
	inline TInt LocalGraphicsMemoryUsed();
	inline TInt GlobalGraphicsMemoryUsed();
private:
	enum TControl
		{
		EControlCreateResource,
		EControlOpenResource,
		EControlCloseResource,
		EControlResourceAttributes,
		EControlGetResourceMetaData,
		EControlResourceDataAddress,
		EControlResourceDataSize,
		EControlGlobalResourceCount,
		EControlLocalGraphicsMemoryUsed,
		EControlGlobalGraphicsMemoryUsed
		};
	class TCreateResourceArgs
		{
	public:
		TUint32 iAttributes;
		const TDesC8* iMetaData;
		TInt iDataSize;
		};
	};

#include <sgresource/sgdevice.inl>

#endif // SGDEVICE_H

// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Surface Manager Implementation
// 
//

/**
@file
@internalComponent
@prototype
*/

#ifndef __SURFACEMANAGERDRIVER_H__
#define __SURFACEMANAGERDRIVER_H__

#include <graphics/surfacemanager.h>

/**
RSurface Manager driver. This is a simple user side interface class derived from RBusLogicalChannel
with each surface manager function implemented with a call to DoControl
*/
class RSurfaceManagerDriver : public RBusLogicalChannel
	{
public:
	
	/** Package class to exchange data between the client interface of the driver (RSurfaceManagerDriver)
	 * and the kernel extension which implement it (DSurfaceManger)*/
	class TDeviceParam
		{
	public:
		TDeviceParam(){}
		TDeviceParam(TAny* aSurfaceId, TAny* aBuffer) : iSurfaceId(aSurfaceId), iBuffer(aBuffer) {}
		/** Generic object used to pass as a parameter to device driver*/
		TAny* iBuffer;
		/** Generic object used to pass as a parameter to device driver*/
		TAny* iSurfaceId;
		};

	class TCaps
		{
	public:
		TVersion iVersion;
		};

public:
#ifndef __KERNEL_MODE__
	inline TInt CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId);
	inline TInt CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId, const RChunk& aChunkHandle);
	inline TInt OpenSurface(const TSurfaceId& aSurfaceId);
	inline TInt CloseSurface(const TSurfaceId& aSurfaceId);
	inline TInt MapSurface(const TSurfaceId& aSurfaceId, RChunk& aHandle);
	inline TInt SurfaceInfo(const TSurfaceId& aSurfaceId, RSurfaceManager::TInfoBuf& aInfo);
	inline TInt Open();
	inline TInt SynchronizeCache(const TSurfaceId& aSurfaceId, TInt aBuffer, RSurfaceManager::TSyncOperation aOperation);
	inline TInt GetSurfaceHint(const TSurfaceId& aSurfaceId, RSurfaceManager::THintPair& aHint);
	inline TInt SetSurfaceHint(const TSurfaceId& aSurfaceId, const RSurfaceManager::THintPair& aHint);
	inline TInt AddSurfaceHint(const TSurfaceId&aSurface, const RSurfaceManager::THintPair& aHint);
	inline TInt GetBufferOffset(const TSurfaceId& aSurfaceId, TInt aBuffer, TInt& aOffset);
	inline TInt GetSurfaceManagerAttrib(RSurfaceManager::TSurfaceManagerAttrib aAttrib, TInt& aValue);
#endif	//__KERNEL_MODE__

	inline static const TDesC& Name();
	inline static TVersion VersionRequired();	

private:
	enum TSurfaceManagerControl
		{
		EControlCreateSurface,
		EControlOpenSurface,
		EControlCloseSurface,
		EControlAccessSurfaceData,
		EControlSurfaceInfo,
		EControlCreateSurfaceEx,
		EControlSynchronizeCache,
		EControlGetSurfaceHint,
		EControlSetSurfaceHint,
		EControlAddSurfaceHint,
		EControlGetBufferOffset,
		EControlGetSurfaceManagerAttrib
		};

	friend class DSurfaceManagerChannel;

	};


#include "surfacemanagerdriver.inl"

#endif	//__SURFACEMANAGERDRIVER_H__

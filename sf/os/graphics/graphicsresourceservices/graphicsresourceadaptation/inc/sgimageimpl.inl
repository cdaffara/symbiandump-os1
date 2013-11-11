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

#ifndef SGIMAGEIMPL_INL
#define SGIMAGEIMPL_INL


// XSgImageImplBase

inline XSgImageImplBase::XSgImageImplBase(XSgDriverImpl& aDriverImpl)
	: XSgBase(aDriverImpl)
	{}


inline XSgImageImplBase::XSgImageImplBase(XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId)
	: XSgBase(aDriverImpl), iId(aId)
	{}


inline const TSgImageMetaData& XSgImageImplBase::MetaData() const
	{
	return iState->MetaData();
	}


// XSgImageStateBase

inline XSgImageStateBase::XSgImageStateBase(XSgDriverImpl& aDriverImpl)
	: XSgBase(aDriverImpl)
	{}


inline XSgImageStateBase::XSgImageStateBase(XSgDriverImpl& aDriverImpl, TInt aDataStride)
	: XSgBase(aDriverImpl), iDataStride(aDataStride)
	{}


inline TInt XSgImageStateBase::DataStride() const
	{
	return iDataStride;
	}


#ifndef SYMBIAN_GRAPHICS_USE_GPU

// TSgImageId_SwLocal

inline TBool TSgImageId_SwLocal::IsMatch(const TSgDrawableId& aId)
	{
	return reinterpret_cast<const TSgImageId_SwLocal&>(aId).iMinusOne == KErrNotFound;
	}


// XSgImageImpl_SwLocal

inline XSgImageImpl_SwLocal::XSgImageImpl_SwLocal(XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId)
	: XSgImageImplBase(aDriverImpl, aId)
	{}


inline XSgImageImpl_SwLocal::XSgImageImpl_SwLocal(const XSgImageImpl_SwLocal& aImage, TUint32 aFlags)
	: XSgImageImplBase(aImage, aFlags)
	{}

#endif


// TSgImageId_SurfaceManager

inline TBool TSgImageId_SurfaceManager::IsMatch(const TSgDrawableId& aId)
	{
	return reinterpret_cast<const TSgImageId_SurfaceManager&>(aId).iBufferIndex >= 0
	       && reinterpret_cast<const TSgImageId_SurfaceManager&>(aId).iMetaDataIndex >= 0;
	}


// XSgImageImpl_SurfaceManager

inline XSgImageImpl_SurfaceManager::XSgImageImpl_SurfaceManager(XSgDriverImpl& aDriverImpl)
	: XSgImageImplBase(aDriverImpl)
	{}


inline XSgImageImpl_SurfaceManager::XSgImageImpl_SurfaceManager(XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId)
	: XSgImageImplBase(aDriverImpl, aId)
	{}


inline XSgImageImpl_SurfaceManager::XSgImageImpl_SurfaceManager(const XSgImageImpl_SurfaceManager& aImage, TUint32 aFlags)
	: XSgImageImplBase(aImage, aFlags)
	{}


// XSgImageState_SurfaceManager

inline XSgImageState_SurfaceManager::XSgImageState_SurfaceManager(XSgDriverImpl& aDriverImpl)
	: XSgImageStateBase(aDriverImpl)
	{}


inline const TSurfaceId& XSgImageState_SurfaceManager::SurfaceId() const
	{
	return iSurfaceId;
	}


inline const RChunk& XSgImageState_SurfaceManager::DataChunk() const
	{
	return iDataChunk;
	}


inline TInt XSgImageState_SurfaceManager::DataOffset() const
	{
	return iDataOffset;
	}


#endif // SGIMAGEIMPL_INL

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

#include "sgimagecollectionimpl.h"
#include "sgimageimpl.h"


TInt XSgImageCollectionImpl::New(XSgImageCollectionImpl*& aPtr, XSgDriverImpl& aDriverImpl,
                                 const TSgImageInfo& aInfo, TInt aImageCount, TBool aIsCached,
                                 TInt aStride, TInt aOffsetToFirstBuffer, TInt aOffsetBetweenBuffers,
                                 TInt aMetaDataIndex, const XSgImageCollectionImpl* aCollection)
	{
	aPtr = static_cast<XSgImageCollectionImpl*>(aDriverImpl.Alloc(sizeof(XSgImageCollectionImpl)));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageCollectionImpl(aDriverImpl);
	TInt err = aPtr->Construct(aInfo, aImageCount, aIsCached, aStride, aOffsetToFirstBuffer, aOffsetBetweenBuffers, aMetaDataIndex, aCollection);
	if (err != KErrNone)
		{
		aPtr->Delete();
		aPtr = NULL;
		}
	return err;
	}


TInt XSgImageCollectionImpl::Construct(const TSgImageInfo& aInfo, TInt aImageCount, TBool aIsCached,
                                       TInt aStride, TInt aOffsetToFirstBuffer, TInt aOffsetBetweenBuffers,
                                       TInt aMetaDataIndex, const XSgImageCollectionImpl* aCollection)
	{
	TInt maxNumberOfHints;
	TInt err;
	err=iDriverImpl.GetSurfaceManagerAttrib(RSurfaceManager::EMaxNumberOfHints,maxNumberOfHints);
	if (err != KErrNone)
	    {
	    return err;
	    } 
			
	if (aInfo.iUserAttributeCount > maxNumberOfHints)
		{
		return KErrOverflow;
		}
	RSurfaceManager::THintPair* hints = new RSurfaceManager::THintPair[aInfo.iUserAttributeCount];
	if(!hints)
		{
		return KErrNoMemory;
		}

	RSurfaceManager::TSurfaceCreationAttributesBuf reqs;
	reqs().iSize = aInfo.iSizeInPixels;
	reqs().iBuffers = aImageCount;
	reqs().iPixelFormat = aInfo.iPixelFormat;
	reqs().iStride = aStride;
	reqs().iOffsetToFirstBuffer = aOffsetToFirstBuffer;
	reqs().iAlignment = 4;
	reqs().iContiguous = EFalse;
	reqs().iCacheAttrib = aIsCached ? RSurfaceManager::ECached : RSurfaceManager::ENotCached;
	reqs().iMappable = ETrue;
	reqs().iOffsetBetweenBuffers = aOffsetBetweenBuffers;
	reqs().iSurfaceHints = hints;
	reqs().iHintCount = aInfo.iUserAttributeCount;
	for (TInt i = 0; i < aInfo.iUserAttributeCount; ++i)
		{
		reqs().iSurfaceHints[i].iKey = aInfo.iUserAttributes[i].iUid;
		reqs().iSurfaceHints[i].iValue = aInfo.iUserAttributes[i].iValue;
		reqs().iSurfaceHints[i].iMutable = EFalse;
		}	
	TSurfaceId surfaceId;
	if (!aCollection)
		{
		err = iDriverImpl.CreateSurface(reqs, surfaceId);
		}
	else
		{
		err = iDriverImpl.CreateSurface(reqs, surfaceId, aCollection->iDataChunk);
		}
	delete[] hints;
	hints = NULL;
	reqs().iSurfaceHints = NULL;
	if (err != KErrNone)
		{
		return err;
		}
	iSurfaceId = surfaceId;
	iCount = aImageCount;
	RChunk chunk;
	if (!aCollection)
		{
		err = iDriverImpl.MapSurface(surfaceId, chunk);
		}
	else
		{
		chunk = aCollection->iDataChunk;
		err = chunk.Duplicate(RThread()); // Get a process-wide handle
		}
	if (err != KErrNone)
		{
		return err;
		}
	iDataChunk = chunk;
	iMetaDataIndex = aMetaDataIndex;
	new(chunk.Base() + aMetaDataIndex * sizeof(TSgImageMetaData)) TSgImageMetaData(aInfo, iDriverImpl.PixelFormatTable(), aIsCached);
	return KErrNone;
	}


XSgImageCollectionImpl::~XSgImageCollectionImpl()
	{
	if (!iSurfaceId.IsNull())
		{
		iDriverImpl.CloseSurface(iSurfaceId);
		iDataChunk.Close();
		}
	}


void XSgImageCollectionImpl::Close()
	{
	XSgDriverImpl& driverImpl = iDriverImpl;
	driverImpl.Wait();
	if (DecRefCount() == 0)
		{
		driverImpl.DeleteImageCollection(this);
		}
	driverImpl.Signal();
	}


const TSurfaceId& XSgImageCollectionImpl::SurfaceId() const
	{
	return iSurfaceId;
	}


TInt XSgImageCollectionImpl::GetInfo(TSgImageInfo& aInfo) const
	{
	reinterpret_cast<TSgImageMetaData*>(iDataChunk.Base() + iMetaDataIndex * sizeof(TSgImageMetaData))->GetInfo(aInfo);
	for (TInt i = 0; i < aInfo.iUserAttributeCount; ++i)
		{
		RSurfaceManager::THintPair hint;
		hint.iKey = aInfo.iUserAttributes[i].iUid;
		TInt err = iDriverImpl.GetSurfaceHint(iSurfaceId, hint);
		if (err != KErrNone)
			{
			return err;
			}
		aInfo.iUserAttributes[i].iValue = hint.iValue;
		}
	return KErrNone;
	}


TInt XSgImageCollectionImpl::Count() const
	{
	return iCount;
	}


TInt XSgImageCollectionImpl::OpenImage(TInt aIndex, MSgDrawableAdapter*& aResult)
	{
	if (aIndex < 0 || aIndex >= iCount)
		{
		return KErrArgument;
		}
	union
		{
		TSgDrawableId id;
		TSgImageId_SurfaceManager id_SurfaceManager;
		};
	id_SurfaceManager.iSurfaceId = iSurfaceId;
	id_SurfaceManager.iBufferIndex = aIndex;
	id_SurfaceManager.iMetaDataIndex = iMetaDataIndex;
	id_SurfaceManager.iFlags = 0;
	iDriverImpl.Wait();
	TInt err = iDriverImpl.OpenImage(id, KSgDefaultOpenMode, aResult);
	iDriverImpl.Signal();
	return err;
	}

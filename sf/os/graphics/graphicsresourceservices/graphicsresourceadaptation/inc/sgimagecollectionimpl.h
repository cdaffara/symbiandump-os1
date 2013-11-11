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
 @internalComponent
*/

#ifndef SGIMAGECOLLECTIONIMPL_H
#define SGIMAGECOLLECTIONIMPL_H

#include "sgimagecollectionadapter.h"
#include "sgdriverimpl.h"


/**
@internalComponent

An adapter object representing an image collection. Image collections are always
stored as surfaces. All the images in a collection have the same metadata, which
is stored at the beginning of the underlying memory chunk.

If several image collections share a single memory chunk then the metadata is stored
in an array with as many elements as image collections, while the pixel data of
each image in any of the collections shares the surface buffer with the corresponding
images in the other collections.
*/
class XSgImageCollectionImpl: public XSgBase, public MSgImageCollectionAdapter
	{
public:
	static TInt New(XSgImageCollectionImpl*& aPtr, XSgDriverImpl& aDriverImpl,
	                const TSgImageInfo& aInfo, TInt aImageCount, TBool aIsCached,
	                TInt aStride, TInt aOffsetToFirstBuffer, TInt aOffsetBetweenBuffers,
	                TInt aMetaDataIndex = 0, const XSgImageCollectionImpl* aCollection = NULL);
	~XSgImageCollectionImpl();
	// From MSgResourceAdapter
	void Close();
	// From MSgImageCollectionAdapter
	const TSurfaceId& SurfaceId() const;
	TInt GetInfo(TSgImageInfo& aInfo) const;
	TInt Count() const;
	TInt OpenImage(TInt aIndex, MSgDrawableAdapter*& aResult);
private:
	inline XSgImageCollectionImpl(XSgDriverImpl& aDriverImpl);
	TInt Construct(const TSgImageInfo& aInfo, TInt aImageCount, TBool aIsCached,
	               TInt aStride, TInt aOffsetToFirstBuffer, TInt aOffsetBetweenBuffers,
	               TInt aMetaDataIndex, const XSgImageCollectionImpl* aCollection);
private:
	/** The surface identifier of the image collection. */
	TSurfaceId iSurfaceId;
	/** The number of images in the collection. */
	TInt iCount;
	/** Handle to the underlying memory chunk. */
	RChunk iDataChunk;
	/** Index into the metadata array. */
	TInt iMetaDataIndex;
	};


#include "sgimagecollectionimpl.inl"


#endif // SGIMAGECOLLECTIONIMPL_H

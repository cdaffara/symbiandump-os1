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
// Graphics Resource - kernel extension internal functionality
//

#ifndef SGEXTENSIONIMPL_H
#define SGEXTENSIONIMPL_H

#include <sgresource/sgextension.h>

NONSHARABLE_CLASS(DSgExtensionImpl): public DBase
	{
public:
	TInt Construct();
	TInt CreateResource(TUint32 aAttribs, const TDesC8& aMetaData, TInt aDataSize, DSgResource*& aResource);
	TInt FindAndOpenResource(TUint64 aId, DSgResource*& aResource);
	void DeleteResource(DSgResource* aResource);
	TInt GlobalResourceCount() const;
	TInt GlobalGraphicsMemoryUsed() const;
private:
	DMutex* iMutex;
	TUint64 iLastResourceId;
	RPointerArray<DSgResource> iResources;
	};

NONSHARABLE_CLASS(DSgResourceImpl): public DSgResource
	{
public:
	DSgResourceImpl(DSgExtensionImpl& aExtensionImpl, TUint64 aId, TUint32 aAttribs,
	                const TDesC8& aMetaData, DChunk* aDataChunk, TInt aDataSize);
	// From DSgResource
	TInt Open();
	void Close();
	TUint64 Id() const;
	TUint32 Attributes() const;
	TInt GetMetaData(TDes8& aMetaData) const;
	DChunk* DataChunk() const;
	TInt DataSize() const;
private:
	~DSgResourceImpl();
private:
	DSgExtensionImpl& iExtensionImpl;
	TInt iRefCount;
	TUint64 iId;
	TUint32 iAttributes;
	TInt iMetaDataSize;
	DChunk* iDataChunk;
	TInt iDataSize;
	};

#endif // SGEXTENSIONIMPL_H

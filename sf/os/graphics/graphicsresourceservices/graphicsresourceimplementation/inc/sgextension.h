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
// Graphics Resource - kernel extension
//

#ifndef SGEXTENSION_H
#define SGEXTENSION_H

#include <kernel/kernel.h>
#include <sgresource/sgcommon.h>

class DSgResource: public DBase
	{
public:
	virtual TInt Open() = 0;
	virtual void Close() = 0;
	virtual TUint64 Id() const = 0;
	virtual TUint32 Attributes() const = 0;
	virtual TInt GetMetaData(TDes8& aMetaData) const = 0;
	virtual DChunk* DataChunk() const = 0;
	virtual TInt DataSize() const = 0;
	inline static TInt Compare(const TUint64* aId, const DSgResource& aResource);
	inline static TInt Compare(const DSgResource& aResource1, const DSgResource& aResource2);
	};

class SgExtension
	{
public:
	IMPORT_C static TVersion Version();
	IMPORT_C static TInt CreateResource(TUint32 aAttribs, const TDesC8& aMetaData, TInt aDataSize, DSgResource*& aResource);
	IMPORT_C static TInt FindAndOpenResource(TUint64 aId, DSgResource*& aResource);
	IMPORT_C static TInt GlobalResourceCount();
	IMPORT_C static TInt GlobalGraphicsMemoryUsed();
	};

#include <sgresource/sgextension.inl>

#endif // SGEXTENSION_H

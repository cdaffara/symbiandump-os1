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
// Graphics Resource - kernel resource implementation
//

#include <kernel/kern_priv.h>
#include "sgextensionimpl.h"

DSgResourceImpl::DSgResourceImpl(DSgExtensionImpl& aExtensionImpl, TUint64 aId, TUint32 aAttribs,
	                             const TDesC8& aMetaData, DChunk* aDataChunk, TInt aDataSize)
	: iExtensionImpl(aExtensionImpl), iRefCount(1), iId(aId), iAttributes(aAttribs),
	  iMetaDataSize(aMetaData.Size()), iDataChunk(aDataChunk), iDataSize(aDataSize)
	{
	memcpy(this + 1, aMetaData.Ptr(), aMetaData.Size());
	}

DSgResourceImpl::~DSgResourceImpl()
	{
	if (iDataChunk)
		{
		Kern::ChunkClose(iDataChunk);
		}
	}

TInt DSgResourceImpl::Open()
	{
	return NKern::SafeInc(iRefCount) > 0 ? KErrNone : KErrNotFound;
	}

void DSgResourceImpl::Close()
	{
	if (NKern::SafeDec(iRefCount) == 1)
		{
		iExtensionImpl.DeleteResource(this);
		}
	}

TUint64 DSgResourceImpl::Id() const
	{
	return iId;
	}

TUint32 DSgResourceImpl::Attributes() const
	{
	return iAttributes;
	}

TInt DSgResourceImpl::GetMetaData(TDes8& aMetaData) const
	{
	if (aMetaData.MaxSize() < iMetaDataSize)
		{
		return KErrOverflow;
		}
	if (iMetaDataSize == 0)
		{
		aMetaData.Zero();
		}
	else
		{
		aMetaData.Copy(TPtrC8(reinterpret_cast<const TUint8*>(this + 1), iMetaDataSize));
		}
	return KErrNone;
	}

DChunk* DSgResourceImpl::DataChunk() const
	{
	return iDataChunk;
	}

TInt DSgResourceImpl::DataSize() const
	{
	return iDataSize;
	}

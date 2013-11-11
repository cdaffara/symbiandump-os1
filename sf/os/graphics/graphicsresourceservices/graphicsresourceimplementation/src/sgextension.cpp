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
// Graphics Resource - kernel extension implementation
//

#include <kernel/kern_priv.h>
#include "sgextensionimpl.h"

DSgExtensionImpl* TheSgExtensionImpl = NULL;

DECLARE_STANDARD_EXTENSION()
	{
	__KTRACE_OPT(KBOOT, Kern::Printf("Starting Graphics Extension"));
	TInt err;
	TheSgExtensionImpl = new DSgExtensionImpl;
	if (TheSgExtensionImpl)
		{
		err = TheSgExtensionImpl->Construct();
		if (err != KErrNone)
			{
			delete TheSgExtensionImpl;
			TheSgExtensionImpl = NULL;
			}
		}
	else
		{
		err = KErrNoMemory;
		}
	if (err != KErrNone)
		{
		__KTRACE_OPT(KBOOT, Kern::Printf("Error: Graphics Extension failed to start"));
		}
	return err;
	}

EXPORT_C TVersion SgExtension::Version()
	{
	return TVersion(1, 1, 1);
	}

EXPORT_C TInt SgExtension::CreateResource(TUint32 aAttribs, const TDesC8& aMetaData, TInt aDataSize, DSgResource*& aResource)
	{
	if (!TheSgExtensionImpl)
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("Error: Graphics Extension not ready"));
		return KErrNotReady;
		}
	return TheSgExtensionImpl->CreateResource(aAttribs, aMetaData, aDataSize, aResource);
	}

EXPORT_C TInt SgExtension::FindAndOpenResource(TUint64 aId, DSgResource*& aResource)
	{
	if (!TheSgExtensionImpl)
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("Error: Graphics Extension not ready"));
		return KErrNotReady;
		}
	return TheSgExtensionImpl->FindAndOpenResource(aId, aResource);
	}

EXPORT_C TInt SgExtension::GlobalResourceCount()
	{
	if (!TheSgExtensionImpl)
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("Error: Graphics Extension not ready"));
		return KErrNotReady;
		}
	return TheSgExtensionImpl->GlobalResourceCount();
	}

EXPORT_C TInt SgExtension::GlobalGraphicsMemoryUsed()
	{
	if (!TheSgExtensionImpl)
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("Error: Graphics Extension not ready"));
		return KErrNotReady;
		}
	return TheSgExtensionImpl->GlobalGraphicsMemoryUsed();
	}

TInt DSgExtensionImpl::Construct()
	{
	return Kern::MutexCreate(iMutex, KNullDesC8, KMutexOrdGeneral0);
	}

TInt DSgExtensionImpl::CreateResource(TUint32 aAttribs, const TDesC8& aMetaData, TInt aDataSize, DSgResource*& aResource)
	{
	if (aDataSize <= 0)
		{
		return KErrArgument;
		}
	DSgResourceImpl* resource = static_cast<DSgResourceImpl*>(Kern::Alloc(sizeof(DSgResourceImpl) + aMetaData.Size()));
	if (!resource)
		{
		return KErrNoMemory;
		}
    TChunkCreateInfo info;
	info.iType = TChunkCreateInfo::ESharedKernelMultiple;
	info.iMaxSize = aDataSize;
	info.iOwnsMemory = ETrue;
#ifdef __WINS__
	info.iMapAttr = aAttribs, 0; // suppress compiler warning: cache attribute does not apply to WINS
#else
	info.iMapAttr = aAttribs & ESgResourceAttrCpuCached ? EMapAttrCachedMax : EMapAttrL1Uncached;
#endif
	DChunk* chunk;
	TLinAddr kernAddr;
	TUint32 mapAttr;
	TInt err = Kern::ChunkCreate(info, chunk, kernAddr, mapAttr);
	if (err != KErrNone)
		{
		Kern::Free(resource);
		return err;
		}
	err = Kern::ChunkCommit(chunk, 0, aDataSize);
	if (err != KErrNone)
		{
		Kern::ChunkClose(chunk);
		Kern::Free(resource);
		return err;
		}
	Kern::MutexWait(*iMutex);
	new(resource) DSgResourceImpl(*this, ++iLastResourceId, aAttribs, aMetaData, chunk, aDataSize);
	err = iResources.InsertInOrder(resource, DSgResource::Compare);
	Kern::MutexSignal(*iMutex);
	if (err != KErrNone)
		{
		resource->Close();
		}
	aResource = (err == KErrNone ? resource : NULL);
	return err;
	}

TInt DSgExtensionImpl::FindAndOpenResource(TUint64 aId, DSgResource*& aResource)
	{
	if (aId == 0)
		{
		return KErrArgument;
		}
	Kern::MutexWait(*iMutex);
	TInt i = iResources.FindInOrder(aId, DSgResource::Compare);
	if (i < 0)
		{
		Kern::MutexSignal(*iMutex);
		return KErrNotFound;
		}
	DSgResource* resource = iResources[i];
	TInt err = resource->Open();
	Kern::MutexSignal(*iMutex);
	aResource = (err == KErrNone ? resource : NULL);
	return err;
	}

void DSgExtensionImpl::DeleteResource(DSgResource* aResource)
	{
	Kern::MutexWait(*iMutex);
	TInt i = iResources.FindInOrder(aResource, DSgResource::Compare);
	if (i >= 0)
		{
		iResources.Remove(i);
		}
	else
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("Error: internal state of Graphics Extension is inconsistent"));
		}
	Kern::MutexSignal(*iMutex);
	delete aResource;
	}

TInt DSgExtensionImpl::GlobalResourceCount() const
	{
	return iResources.Count();
	}

TInt DSgExtensionImpl::GlobalGraphicsMemoryUsed() const
	{
	TInt ret = 0;
	Kern::MutexWait(*iMutex);
	TInt n = iResources.Count();
	for (TInt i = 0; i < n; ++i)
		{
		ret += iResources[i]->DataChunk()->Size();
		}
	Kern::MutexSignal(*iMutex);
	return ret;
	}

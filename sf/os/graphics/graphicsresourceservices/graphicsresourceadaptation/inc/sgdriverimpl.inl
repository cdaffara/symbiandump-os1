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

#ifndef SGDRIVERIMPL_INL
#define SGDRIVERIMPL_INL


inline void Panic(TSgResourceAdapterPanicReason aReason)
	{
	User::Panic(KSgResourceAdapterPanicCategory, aReason);
	}


// XSgBase

inline XSgBase::XSgBase(XSgDriverImpl& aDriverImpl)
	: iDriverImpl(aDriverImpl)
	{}


inline void XSgBase::IncRefCount()
	{
	__ASSERT_DEBUG(iDriverImpl.IsMutexHeld(), Panic(ESgPanicMutexNotHeld));
	__ASSERT_DEBUG(iRefCount >= 0, Panic(ESgPanicBadReferenceCount));
	++iRefCount;
	}


inline TInt XSgBase::DecRefCount()
	{
	__ASSERT_DEBUG(iDriverImpl.IsMutexHeld(), Panic(ESgPanicMutexNotHeld));
	__ASSERT_DEBUG(iRefCount > 0, Panic(ESgPanicBadReferenceCount));
	return --iRefCount;
	}


/**
@internalComponent
@test
*/
inline TInt XSgBase::RefCount() const
	{
	return iRefCount;
	}


inline TAny* XSgBase::operator new(TUint aSize, TAny* aBase)
	{
	Mem::FillZ(aBase, aSize);
	return aBase;
	}


// XSgDriverImpl

inline XSgDriverImpl::XSgDriverImpl(RHeap* aHeap)
	: iHeap(aHeap)
	{}


inline TAny* XSgDriverImpl::operator new(TUint aSize, TAny* aBase)
	{
	Mem::FillZ(aBase, aSize);
	return aBase;
	}


inline void XSgDriverImpl::Wait()
	{
	iMutex.Wait();
	}


inline void XSgDriverImpl::Signal()
	{
	iMutex.Signal();
	}


inline TBool XSgDriverImpl::IsMutexHeld() const
	{
	return iMutex.IsHeld();
	}


inline TAny* XSgDriverImpl::Alloc(TInt aSize)
	{
	return iHeap->Alloc(aSize);
	}


inline void XSgDriverImpl::Free(TAny* aCell)
	{
	iHeap->Free(aCell);
	}


inline TInt XSgDriverImpl::CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId)
	{
	return iSurfaceManager.CreateSurface(aReqs, aSurfaceId);
	}


inline TInt XSgDriverImpl::CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId, const RChunk& aChunk)
	{
	return iSurfaceManager.CreateSurface(aReqs, aSurfaceId, aChunk);
	}


inline TInt XSgDriverImpl::OpenSurface(const TSurfaceId& aSurfaceId)
	{
	return iSurfaceManager.OpenSurface(aSurfaceId);
	}


inline TInt XSgDriverImpl::CloseSurface(const TSurfaceId& aSurfaceId)
	{
	return iSurfaceManager.CloseSurface(aSurfaceId);
	}


inline TInt XSgDriverImpl::SurfaceInfo(const TSurfaceId& aSurfaceId, RSurfaceManager::TInfoBuf& aInfo)
	{
	return iSurfaceManager.SurfaceInfo(aSurfaceId, aInfo);
	}


inline TInt XSgDriverImpl::SynchronizeCache(const TSurfaceId& aSurfaceId, TInt aBuffer, RSurfaceManager::TSyncOperation aOperation)
	{
	return iSurfaceManager.SynchronizeCache(aSurfaceId, aBuffer, aOperation);
	}


inline TInt XSgDriverImpl::GetSurfaceHint(const TSurfaceId& aSurfaceId, RSurfaceManager::THintPair& aHint)
	{
	return iSurfaceManager.GetSurfaceHint(aSurfaceId, aHint);
	}


#endif // SGDRIVERIMPL_INL

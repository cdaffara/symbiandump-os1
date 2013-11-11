// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Thread knowledge
// 
//

inline TBool TWorkerThreadDataBase::IsValid() const
	{
	return iHeap != 0;
	}

inline CWorkerThreadDataGlobalsBase::CWorkerThreadDataGlobalsBase()
	{
	}

inline CWorkerThreadDataGlobalsBase::~CWorkerThreadDataGlobalsBase()
	{
	User::Free(iWorkers);
	}

inline void CWorkerThreadDataGlobalsBase::ConstructL(TInt aWorkerDataSize, TWorkerId TUpperThreadIdBound)
	{
	iWorkers = reinterpret_cast<TWorkerThreadDataBase*>(User::AllocZL(aWorkerDataSize * (TUpperThreadIdBound + 1)));
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>* CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>::NewL()
	{
	CWorkerThreadDataGlobals* self = new(ELeave) CWorkerThreadDataGlobals();
	CleanupStack::PushL(self);
	self->ConstructL(sizeof(TWTD), TUpperThreadIdBound);
	CleanupStack::Pop(self);
	return self;
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline void CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>::ConstructL(TInt aWorkerDataSize, TWorkerId aUpperBoundId)
	{
	CWorkerThreadDataGlobalsBase::ConstructL(aWorkerDataSize, aUpperBoundId);
	for(TInt i = 0; i <= TUpperThreadIdBound; ++i)
		{
		new(reinterpret_cast<TUint8*>(iWorkers) + i * aWorkerDataSize) TWTD;
		}
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline TWorkerId CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>::UpperBoundWorkerId() const
	{
	return TUpperThreadIdBound;
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline TWTD* CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>::GetWorkerGlobals(TWorkerId aWorker) const
	{
	__ASSERT_DEBUG(aWorker <= UpperBoundWorkerId(), Panic(ECFTransInvalidWorkerId));
	return reinterpret_cast<TWTD*>(reinterpret_cast<TUint8*>(iWorkers) + sizeof(TWTD) * aWorker);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline TBool CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>::WorkerPresent(TWorkerId aId) const
	{
	return GetWorkerGlobals(aId)->IsValid();
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline RAllocator& CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>::WorkerHeap(TWorkerId aWorkerId) const
	{
	return *(GetWorkerGlobals(aWorkerId)->iHeap);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline void CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>::PanicWorker(TWorkerId aWorkerId, const TDesC& aCategory, TInt aReason) const
	{
	GetWorkerGlobals(aWorkerId)->iThread.Panic(aCategory, aReason);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>::CWorkerThreadDataGlobals()
: CWorkerThreadDataGlobalsBase()
	{
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline CWorkerThreadRegister<TWTD, TUpperThreadIdBound>* CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::NewL(TWorkerId aSelfId, CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>* aGlobalThreadRegister)
	{
	return new(ELeave) CWorkerThreadRegister(aSelfId, aGlobalThreadRegister);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline TWTD* CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::GetWorkerGlobals(TWorkerId aWorker) const
	{
	__ASSERT_DEBUG(iGlobals != NULL, Panic(ECFTransThreadRegisterUnspecified));
	__ASSERT_DEBUG(aWorker <= TUpperThreadIdBound, Panic(ECFTransInvalidWorkerId));
	return iGlobals->GetWorkerGlobals(aWorker);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline TWorkerId CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::UpperBoundWorkerId() const
	{
	return TUpperThreadIdBound;
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline TBool CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::WorkerPresent(TWorkerId aWorker) const
	{
	__ASSERT_DEBUG(iGlobals != NULL, Panic(ECFTransThreadRegisterUnspecified));
	__ASSERT_DEBUG(aWorker <= TUpperThreadIdBound, Panic(ECFTransInvalidWorkerId));
	return iGlobals->WorkerPresent(aWorker);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline RAllocator& CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::WorkerHeap(TWorkerId aWorker) const
	{
	__ASSERT_DEBUG(iGlobals != NULL, Panic(ECFTransThreadRegisterUnspecified));
	__ASSERT_DEBUG(aWorker <= TUpperThreadIdBound, Panic(ECFTransInvalidWorkerId));
	return iGlobals->WorkerHeap(aWorker);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline void CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::PanicWorker(TWorkerId aWorkerId, const TDesC& aCategory, TInt aReason) const
	{
	GetWorkerGlobals(aWorkerId)->iThread.Panic(aCategory, aReason);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline TWorkerId CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::SelfWorkerId() const
	{
	return iSelfId;
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline TWTD* CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::GetSelfWorkerGlobals() const
	{
	__ASSERT_DEBUG(iGlobals != NULL, Panic(ECFTransThreadRegisterUnspecified));
	return iGlobals->GetWorkerGlobals(iSelfId);
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline void CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::SetGlobalThreadRegister(CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>* aGlobalThreadRegister)
	{
	iGlobals = aGlobalThreadRegister;
	}

template<class TWTD, TInt TUpperThreadIdBound>
inline CWorkerThreadRegister<TWTD, TUpperThreadIdBound>::CWorkerThreadRegister(TWorkerId aSelfId, CWorkerThreadDataGlobals<TWTD, TUpperThreadIdBound>* aGlobalThreadRegister)
: iSelfId(aSelfId),
iGlobals(aGlobalThreadRegister)
	{
	}



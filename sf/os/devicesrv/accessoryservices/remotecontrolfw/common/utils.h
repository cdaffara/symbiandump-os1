// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Utilities for Rem Con server.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef REMCONUTILS_H
#define REMCONUTILS_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <s32strm.h>
#include <s32buf.h>

// In debug, using checking forms of CleanupStack::Pop. In release builds, 
// use the non-checking form to save a little bit of ROM.
#ifdef _DEBUG
#define CLEANUPSTACK_POP1(a)		CleanupStack::Pop(a);
#define CLEANUPSTACK_POP2(a, b) 	CleanupStack::Pop(a, b);
#else
#define CLEANUPSTACK_POP1(a)		CleanupStack::Pop();
#define CLEANUPSTACK_POP2(a, b) 	CleanupStack::Pop(2);
#endif // _DEBUG

// Used for cleanup stack-based cleanup of RImplInfoPtrArrays.
void CleanupResetAndDestroyPushL(RImplInfoPtrArray& aArray);
// Used for cleanup stack-based cleanup of temporary heaps.
void CleanupSwitchHeapPushL(RHeap& aHeap);


template <class T>
class CleanupDeleteAndNull
	{
public:
	inline static void PushL(T*& aRef) {CleanupStack::PushL(TCleanupItem(&DeleteAndNull,&aRef));};
private:
	static void DeleteAndNull(TAny *aPtr) {T*& ptr = *static_cast<T**>(aPtr); delete ptr; ptr = NULL;};
	};
template <class T>
inline void CleanupDeleteAndNullPushL(T*& aRef)
	{CleanupDeleteAndNull<T>::PushL(aRef);}

template <class T>
class CleanupReset
	{
public:
	inline static void PushL(T& aRef) {CleanupStack::PushL(TCleanupItem(&Reset,&aRef));};
private:
	static void Reset(TAny *aPtr) {(static_cast<T*>(aPtr))->Reset();};
	};
template <class T>
inline void CleanupResetPushL(T& aRef)
	{CleanupReset<T>::PushL(aRef);}

template <class T>
class CleanupSignal
	{
public:
	inline static void PushL(T& aRef) {CleanupStack::PushL(TCleanupItem(&Signal,&aRef));};
private:
	static void Signal(TAny *aPtr) {(static_cast<T*>(aPtr))->Signal();};
	};
template <class T>
inline void CleanupSignalPushL(T& aRef)
	{CleanupSignal<T>::PushL(aRef);}

template <class T>
class CleanupNull
	{
public:
	inline static void PushL(T*& aRef) {CleanupStack::PushL(TCleanupItem(&Null,&aRef));};
private:
	static void Null(TAny *aPtr) {T*& ptr = *static_cast<T**>(aPtr); ptr = NULL;};
	};
template <class T>
inline void CleanupNullPushL(T*& aRef)
	{CleanupNull<T>::PushL(aRef);}



/**
A wrapper class around an RFastLock that enables locks to be nested.
*/
NONSHARABLE_CLASS(RNestableLock)
	{
public:
	RNestableLock();
	TInt CreateLocal();
	void Close();
	
	void Wait();
	void Signal();
	
private:
	static const TUint32 KInvalidThreadId = ~0u;
	
private:
	RFastLock	iLock;
	RFastLock	iMetaLock;
	TThreadId	iThreadId;
	TInt		iRefCount;
	};


class CSpecificThreadCallBackBody;

NONSHARABLE_CLASS(RSpecificThreadCallBack)
	{
public:
	RSpecificThreadCallBack();
	
	TInt Create(const TCallBack& aCallBack, TInt aPriority);
	void Close();
	
	TInt Start();
	TInt CallBack();
	void Cancel();
	
private:
	CSpecificThreadCallBackBody*	iBody;
	};


NONSHARABLE_CLASS(RCountSizeWriteStream)
	: public RWriteStream
	, public MStreamBuf
	{
public:
	using RWriteStream::Close;
	RCountSizeWriteStream();
	TInt Size() const;
	void Reset();

private: // MStreamBuf
	void DoRelease();
	void DoSynchL();
	TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer);
	void DoWriteL(const TAny* aPtr,TInt aLength);
	TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer);
	TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
	
private:
	TInt iSize;
	};

#endif // REMCONUTILS_H

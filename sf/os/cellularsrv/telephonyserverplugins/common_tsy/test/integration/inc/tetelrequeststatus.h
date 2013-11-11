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
 @internalTechnology
*/


#ifndef TETELREQUEST_H
#define TETELREQUEST_H

#include <e32base.h>
#include <etel.h>
#include <e32cons.h>

/**
Base class to store a request status and the require information to cancel the request
Object of this call can be push on the cleanup stack to ensure that a request is canceled
on leave.
*/
class TEtelRequestBase
	{
public:
	virtual void Cancel()=0;
public:
	inline operator TCleanupItem();
	inline operator TRequestStatus&();
	inline TInt Int();
protected:
	static void DoCleanup(TAny* aPtr);
	inline void Cleanup();
protected:
	TRequestStatus 		iReqStatus;
	};


/**
TEtelRequestBase for core etel functions
*/
template<class SubSess>
class TCoreEtelRequestStatus : public TEtelRequestBase
	{
public:
	typedef void (SubSess::*TConstCancelFnPtr)(void) const;
	typedef void (SubSess::*TCancelFnPtr)(void);
public:
	inline TCoreEtelRequestStatus(SubSess& aSubSess, TCancelFnPtr aCancelFnPtr);
	inline TCoreEtelRequestStatus(SubSess& aSubSess, TConstCancelFnPtr aCancelFnPtr);
	void Cancel();
protected:
	SubSess&			iSubSess;
	TConstCancelFnPtr	iConstCancelFnPtr;
	TCancelFnPtr		iCancelFnPtr;
	};


/**
TEtelRequestBase for extended etel functions
*/
class TExtEtelRequestStatus : public TEtelRequestBase
	{
public:
	inline TExtEtelRequestStatus(RTelSubSessionBase& aSubSess, TInt aCancelIpc);
	void Cancel();
protected:
	RTelSubSessionBase&	iSubSess;
	TInt				iCancelIpc;
	};
	
	
/**
*/
inline TEtelRequestBase::operator TCleanupItem()
	{
	return TCleanupItem(DoCleanup,this);
	}

/**
*/
inline TEtelRequestBase::operator TRequestStatus&()
	{
	return iReqStatus;
	}

/**
 */
inline TInt TEtelRequestBase::Int()
	{
	return iReqStatus.Int();
	}

/**
*/
inline void TEtelRequestBase::Cleanup()
	{
	if (iReqStatus == KRequestPending)
		{
		Cancel();
		}
	}

/**
*/
template<class SubSess>
inline TCoreEtelRequestStatus<SubSess>::TCoreEtelRequestStatus(SubSess& aSubSess, TCancelFnPtr aCancelFnPtr)
	:iSubSess(aSubSess), iConstCancelFnPtr(NULL), iCancelFnPtr(aCancelFnPtr)
	{
	}

/**
*/
template<class SubSess>
inline TCoreEtelRequestStatus<SubSess>::TCoreEtelRequestStatus(SubSess& aSubSess, TConstCancelFnPtr aCancelFnPtr)
	:iSubSess(aSubSess), iConstCancelFnPtr(aCancelFnPtr), iCancelFnPtr(NULL)
	{
	}

/**
*/
template<class SubSess>
void TCoreEtelRequestStatus<SubSess>::Cancel()
	{
	if (iConstCancelFnPtr)
		(iSubSess.*iConstCancelFnPtr)();
	else
		(iSubSess.*iCancelFnPtr)();
	User::WaitForRequest(iReqStatus);
	}

/**
*/
inline TExtEtelRequestStatus::TExtEtelRequestStatus(RTelSubSessionBase& aSubSess, TInt aCancelIpc)
	:iSubSess(aSubSess), iCancelIpc(aCancelIpc)
	{
	}
	
/**
TConsoleReadRequestBase for CConsole::Read function
*/
class TConsoleReadRequestStatus : public TEtelRequestBase
	{
public:
	inline TConsoleReadRequestStatus(CConsoleBase& aSubSess);
	void Cancel();
protected:
	CConsoleBase&	iSubSess;
	};
	
/**
*/
inline TConsoleReadRequestStatus::TConsoleReadRequestStatus(CConsoleBase& aSubSess)
	:iSubSess(aSubSess)
	{
	}


#endif

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
//

/**
 @file
 @internalTechnology
 @released
*/

#include <elements/interfacetable.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNetIntfIntrf, "ElemNetIntfIntrf");
#endif

using namespace NetInterfaces;
_LIT(KNetInterfacesPanic, "NetInterfaces");
enum TNetInterfacesPanic
	{
	EInterfaceNotFound = 0,
	};
	
EXPORT_C TAny* TInterfaceControl::FetchInterfaceL(TInt aInterfaceId)
	{
	TAny* itf = DoFetchInterface(aInterfaceId);
	if(itf == NULL)
		{
		//Since we do not have control over 3rd party code we only check
		//and mask this error as KErrNotSupported rather than panic ESockSvr
		//(we panic in UDEB, please see DoFetchInterface);
		User::Leave(KErrInterfaceNotSupported);
		}
		
	return itf;
	}

EXPORT_C TAny* TInterfaceControl::FetchInterface(TInt aInterfaceId)
	{
	TAny* itf = DoFetchInterface(aInterfaceId);
	if(itf == NULL)
		{
		// This is fatal.  Either an TInterfaceControl was returned for a
		// particular interface id and the TInterfaceControl was inconsistent, 
		// or a bad interface id was passed into FetchInterface.
		User::Panic(KNetInterfacesPanic, EInterfaceNotFound);
		}
		
	return itf;
	}
	
TAny* TInterfaceControl::DoFetchInterface(TInt aInterfaceId)
	{
	const TFIEntry* head = iHead;

	// Find the interface implementer
	while((head != NULL) && (head->iIfId != aInterfaceId))
		{
		head = head->iNextFn();
		}

	// If not found, aInterfaceId is not valid, leave with KErrArgument
	TAny* iface = NULL;
	if(head != NULL)
		{
		// Ask the implementer for the interface reference (pointer)
		iface = head->iFetchFnL(this);
		//The returned pointer cannot be NULL because it has been returned from a leaving function.
		//This may be the case in one scenario, where the API extension implementer (TInterfaceControl
		//derived object) has returned NULL from ReturnInterfacePtrL() which is a bug.
		__ASSERT_DEBUG(iface,User::Panic(KNetInterfacesPanic, EInterfaceNotFound));
		}

	return iface;
	}


EXPORT_C TInterfaceIter::TInterfaceIter(const TInterfaceControl& aItfControl)
:	iItfControl(aItfControl),
	iCurrent(aItfControl.iHead)
	{
	}

EXPORT_C TAny* TInterfaceIter::operator[](TInt aInd)
	{
	//aInd>0 not yet supported!
	__ASSERT_ALWAYS(aInd==0, User::Panic(KNetInterfacesPanic, EInterfaceNotFound));
	const TInterfaceControl::TFIEntry* fetcher = iItfControl.iHead;
	return fetcher? fetcher->iFetchFnL(const_cast<TInterfaceControl*>(&iItfControl)) : NULL;
	}

EXPORT_C TAny* TInterfaceIter::Next()
	{
	// Initialise current to head or grab next interface in list
	if (iCurrent == NULL)
		return NULL;

	iCurrent = iCurrent->iNextFn();
	return iCurrent? iCurrent->iFetchFnL(const_cast<TInterfaceControl*>(&iItfControl)) : NULL;
	}

EXPORT_C TBool AApiExtBase::SupportsExtInterface(TInt aInterfaceId) const
    {
    TRAPD(ret,const_cast<AApiExtBase*>(this)->FetchExtInterfaceL(aInterfaceId));
    return ret == KErrNone;
    }

EXPORT_C TInterfaceControl* AApiExtBase::FetchExtInterfaceControlL(TInt aInterfaceId)
	{
	return DoFetchInterfaceControlL(aInterfaceId);
	}
	
EXPORT_C TAny* AApiExtBase::FetchExtInterfaceL(TInt aInterfaceId)
    {
    TInterfaceControl* tic = FetchExtInterfaceControlL(aInterfaceId);
    __ASSERT_DEBUG(tic, User::Panic(KSpecAssert_ElemNetIntfIntrf, 1));
    return tic->FetchInterfaceL(aInterfaceId);
    }

EXPORT_C TAny* AApiExtBase::FetchExtInterface(TInt aInterfaceId)
    {
    TInterfaceControl* tic = NULL;
    TRAP_IGNORE(tic = FetchExtInterfaceControlL(aInterfaceId));
    return tic? tic->FetchInterface(aInterfaceId) : NULL;
    }

EXPORT_C TInterfaceControl* AApiExtBase::DoFetchInterfaceControlL(TInt aInterfaceId)
	{
	(void)aInterfaceId;
	User::Leave(KErrInterfaceNotSupported);
	return NULL;
	}



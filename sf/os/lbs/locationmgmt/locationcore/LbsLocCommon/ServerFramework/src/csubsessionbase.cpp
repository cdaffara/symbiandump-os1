// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "csubsessionbase.h"
#include "lbsmessageenums.h"

/** 
inline function to push the item needed to be released when destroy, via a TCleanupItem object,
which includes a MRelease function to release the resource.

@param aRef A reference MRelease function
@internalTechnology
@released
 */	
inline void CleanupVirtualRelease::PushL(MRelease& aRef)
	{
	CleanupStack::PushL(TCleanupItem(&Release,&aRef));
	}
		
/** 
release the resource by calling the defined MRelease function

@param aPrt A reference of MRelease function
@internalTechnology
@released
 */	
void CleanupVirtualRelease::Release(TAny* aPtr)
	{
	MRelease* ptr = reinterpret_cast<MRelease*>(aPtr);
	TReleaseFunc func = &MRelease::VirtualRelease;
	(ptr->*func)();
	}
	
	
/** 
function to push the item needed to be released by calling CleanupVirtualRelease class,
which includes a MRelease function to release the resource.

@param aRef A reference MRelease function
@internalTechnology
@released
 */	
void CleanupVirtualReleasePushL(MRelease& aRef)
	{
	CleanupVirtualRelease::PushL(aRef);
	}
	
/** 
Default constructor
Nothing here

@internalTechnology
@released
 */	
CSubSessionBase::CSubSessionBase()
	{
	// does nothing
	}
	
/** 
Default destructor
Nothing here

@internalTechnology
@released
 */	
CSubSessionBase::~CSubSessionBase()
	{
	// nuke the impl object
	if(iImpl)
		{
		iImpl->VirtualRelease();
		iImpl=NULL;
		}
	}

/** 
Set the generic subsession implement pointer

@param aImpl A generic interface pointer to MSubSessionImpl
@see MSubSessionImpl
@internalTechnology
@released
 */	
void CSubSessionBase::SetImpl(MSubSessionImpl* aImpl)
	{
	iImpl = aImpl;
	}
	
/** 
Get the generic subsession implement pointer

@return the generic interface pointer to MSubSessionImpl
@see MSubSessionImpl
@internalTechnology
@released
 */	
MSubSessionImpl* CSubSessionBase::Impl()
	{
	return iImpl;
	}

/** 
Invoke user panic by setting the category as CSubSessionBase

@param aPanic A constant panic category TSubSessionBasePanic
@internalTechnology
@released
 */	
void CSubSessionBase::Panic(const TSubSessionBasePanic aPanic)
	{
	_LIT(KSubSessionBase, "CSubSessionBase");
	User::Panic(KSubSessionBase, aPanic);
	}

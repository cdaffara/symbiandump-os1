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
// LbsLocSourceGpsBase.cpp
// 
//

#include <ecom/ecom.h>
#include <e32debug.h>

#include <lbs/lbsreflocdatasourcebase.h>
#include "lbsdevloggermacros.h"

/**
 Two-phase constructor that loads the right ECOM implementation of the interface and provides a pointer 
 to it.
 
 @param aObserver Reference to the class that should receive all the callbacks
 @param aEcomModuleId The UID of the ECOM implementation that should be loaded
 
 @return Pointer to the newly loaded implementation
 */
EXPORT_C CLbsRefLocationSourceBase* CLbsRefLocationSourceBase::NewL
	(
	MLbsRefLocationObserver& aObserver,
	TUid aEcomModuleId
	)
	{
	LBSLOG(ELogP1, "CLbsRefLocationSourceBase::NewL()");
	LBSLOG2(ELogP2, "Loading the ref location module plugin. Id: 0x%X", aEcomModuleId.iUid);
	TAny* ptr = REComSession::CreateImplementationL(aEcomModuleId,
													_FOFF(CLbsRefLocationSourceBase, iDtor_ID_Key),
													reinterpret_cast<TAny*>(&aObserver));
	LBSLOG2(ELogP2, "Ref location module plugin loaded.  Id: 0x%X", aEcomModuleId.iUid);
	return reinterpret_cast<CLbsRefLocationSourceBase*>(ptr);
	}

/**
 Constructor
 
 @param aObserver Reference to the class that should receive all the callbacks
 */
EXPORT_C CLbsRefLocationSourceBase::CLbsRefLocationSourceBase(MLbsRefLocationObserver& aObserver)
	:iObserver(aObserver), iVersion(1,0,0)
	{
	}

/**
 Base 2nd stage constructor. The implementer of the interface needs to call this method as 
 part of the construction procedure to ensure the object is build fully.
 */
EXPORT_C void CLbsRefLocationSourceBase::BaseConstructL()
	{	
	}

/**
 Destructor
 */
EXPORT_C CLbsRefLocationSourceBase::~CLbsRefLocationSourceBase()
	{
	LBSLOG(ELogP1, "CLbsRefLocationSourceBase::~CLbsRefLocationSourceBase()");
	LBSLOG(ELogP2, "Destroying the ref location module plugin.");
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

/**
 Method which shows which version of the interface the implementation is using
 
 @return The version of the interface implemented
 */
EXPORT_C TVersion CLbsRefLocationSourceBase::Version() const
    {
    return iVersion;
    }

/**
This method is used to allow polymorphic extensions to the API without breaking
BC.

Reserved for future expansion.

@param aFunctionNumber contains the Id of the function to be invoked internally.
@param aPtr1 a pointer to any data.
@param aPtr2 a pointer to any data.
@return TAny* a pointer to any function.
 */
EXPORT_C TAny* CLbsRefLocationSourceBase::ExtendedInterface(TInt /*aFunctionNumber*/, 
							TAny* /*aPtr1*/, TAny* /*aPtr2*/)
	{
	return NULL;
	}

/**
 Method which shows which version of the callback interface the implementation is using
 
 @return The version of the callback interface implemented 
 */
EXPORT_C TVersion MLbsRefLocationObserver::Version() const
	{
	return TVersion(1,0,0);
	}

/**
This method is used to allow polymorphic extensions to the API without breaking
BC.

Reserved for future expansion.

@param aFunctionNumber contains the Id of the function to be invoked internally.
@param aPtr1 a pointer to any data.
@param aPtr2 a pointer to any data.
@return TAny* a pointer to any function.
 */
EXPORT_C TAny* MLbsRefLocationObserver::ExtendedInterface(TInt /*aFunctionNumber*/, 
							TAny* /*aPtr1*/, TAny* /*aPtr2*/)
	{
	return NULL;
	}

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

// System
#include <ecom/ecom.h>

// Lbs
#include "lbsdevloggermacros.h"

// Component
#include <lbs/gpssetclockbase.h>
#include "GPSSetClockDefault.h"


/** Static constructor.
@param aImplementationId An Uid of a customer specific plug-in to be loaded
@return A pointer to the licensee specific (plug-in) or a default implementation.
*/
EXPORT_C CGPSSetClockBase* CGPSSetClockBase::NewL(TUid aImplementationId)
	{
	LBSLOG(ELogP1, "CGPSSetClockBase::NewL()\n");
	CGPSSetClockBase* ret;
	
	if (aImplementationId.iUid == 0)
		{
		// Use the default, internal implementation
		ret = CGPSSetClockDefault::NewL();
		}
	else
		{
		// Use a licensee specific plug-in
		ret = reinterpret_cast<CGPSSetClockBase*>
			  (REComSession::CreateImplementationL(aImplementationId, _FOFF(CGPSSetClockBase, iDtorIDKey)));
		}
	
	return ret;
	}

/** Default destructor.
*/
EXPORT_C CGPSSetClockBase::~CGPSSetClockBase()
	{
	LBSLOG(ELogP1, "CGPSSetClockBase::~CGPSSetClockBase()\n");
	REComSession::DestroyedImplementation(iDtorIDKey);
	}

/** Default constructor.
*/
EXPORT_C CGPSSetClockBase::CGPSSetClockBase()
	{
	LBSLOG(ELogP1, "CGPSSetClockBase::CGPSSetClockBase()\n");
	}

/** A licensee specific implementation of setting the system UTC time.
The method must NOT allocate memory or leave.
Capabilities of the process must be checked (usually the WriteDeviceData is required)
*/
TInt CGPSSetClockBase::SetUTCTime(const TTime& /*aUTCTime*/)
	{
	LBSLOG(ELogP1, "CGPSSetClockBase::SetUTCTime()\n");
	return KErrNotSupported;
	}

/**
This method is used to allow polymorphic extensions to the API without breaking
BC. See documentation for explanation.
Reserved for future expansion - derived classes should see documentation
on how this is to be used.
 
@param aFunctionNumber contains the Id of the function to be invoked.
@param aPtr1 a pointer to any data
@param aPtr2 a pointer to any data.
@return TAny* a pointer to any function
*/
EXPORT_C TAny* CGPSSetClockBase::ExtendedInterface(TInt /*aFunctionNumber*/, TAny* /*aPtr1*/, TAny* /*aPtr2*/)
	{
	LBSLOG(ELogP1, "CGPSSetClockBase::ExtendedInterface()\n");
	return NULL;
	}


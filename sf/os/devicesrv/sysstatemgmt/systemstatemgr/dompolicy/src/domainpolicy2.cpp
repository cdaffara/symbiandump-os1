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

#include "ssmdomaindefs.h"
#include "ssmsubstates.hrh"

#ifdef __WINS__
const TInt KStateTransitionTimeout = 30000000; /* 30 seconds */
#else
const TInt KStateTransitionTimeout = 10000000; /* 10 seconds */
#endif

/*
Domain specification and policy for the startup domain hierarchy
*/

static const TDmDomainSpec DomainHierarchy[] =
	{
		{ KSM2HALDomain3,				KDmIdNone,					_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined,	KStateTransitionTimeout},
		{ KSM2KernelServicesDomain3,	KSM2HALDomain3,				_INIT_SECURITY_POLICY_PASS, ESsmStartupSubStateUndefined,	KStateTransitionTimeout},
		{ KSM2OSServicesDomain3,		KSM2KernelServicesDomain3,	_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined,	KStateTransitionTimeout},
		{ KSM2GenMiddlewareDomain3,		KSM2OSServicesDomain3,		_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined,	KStateTransitionTimeout },
		{ KSM2GenMiddlewareDomain4,		KSM2GenMiddlewareDomain3,	_INIT_SECURITY_POLICY_PASS, ESsmStartupSubStateUndefined,	KStateTransitionTimeout },
		{ KSM2AppServicesDomain3,		KSM2GenMiddlewareDomain4,	_INIT_SECURITY_POLICY_PASS, ESsmStartupSubStateUndefined, 	KStateTransitionTimeout },
		{ KSM2AppServicesDomain4,		KSM2AppServicesDomain3,	    _INIT_SECURITY_POLICY_PASS, ESsmStartupSubStateUndefined, 	KStateTransitionTimeout },
		{ KSM2UiServicesDomain3,		KSM2AppServicesDomain4,		_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined, 	KStateTransitionTimeout },
		{ KSM2UiApplicationDomain3,		KSM2UiServicesDomain3,		_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined, 	KStateTransitionTimeout },
		// end of array marker
		{ KDmIdNone,	KDmIdNone,	_INIT_SECURITY_POLICY_PASS,				0,			0		}
	};


/*
Note that the _INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData) is not used for the root domain, since this refers to the capabilities of
the SSA components connecting to the Start-up Domain Hierarchy. No capabilities are required by SSA components to attach to the Start-up Domain Hierarchy.
*/

// Policy for traversal of the startup Domain Hierarchy. Only positive transitions should take place
static const TDmHierarchyPolicy HierarchyPolicyStartup	=
	{ETraverseParentsFirst, ETraverseParentsFirst, ETransitionFailureContinue};

/**
Gets access to the domain hierarchy specification.

The domain hierarchy specification is a simple array of TDmDomainSpec items.

The default implementation provided by Symbian OS just returns a pointer to
the domain hierarchy specification array.

@return A pointer to the domain hierarchy specification array.
*/
EXPORT_C const TDmDomainSpec* DmPolicy::GetDomainSpecs()
	{
	return (TDmDomainSpec*) DomainHierarchy;
	}




/**
Releases access to the specified domain hierarchy specification.

The domain hierarchy specification is a simple array of TDmDomainSpec items.

As the default Symbian OS implementation of GetDomainSpecs() just returns
a pointer to the domain hierarchy specification array, then the default
implementation of Release() is empty. The API is provided to permit
more complex implementations, if required.

@param aDomainSpec A pointer to the domain hierarchy specification array.
*/
EXPORT_C void DmPolicy::Release(const TDmDomainSpec* /*aDomainSpec*/)
	{
	}


/**
Retrieves the domain hierarchy policy.

@param	aPolicy a client-supplied policy which on exit
		will contain a copy of the policy for the requested domain hierarchy Id.

@return	KErrNone
*/
EXPORT_C TInt DmPolicy::GetPolicy(TDmHierarchyPolicy& aPolicy)
	{
	aPolicy = HierarchyPolicyStartup;
	return KErrNone;
	}


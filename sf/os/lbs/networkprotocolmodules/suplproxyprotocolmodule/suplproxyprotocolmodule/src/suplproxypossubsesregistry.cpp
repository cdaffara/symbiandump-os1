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



// INCLUDE FILES
#include <e32base.h>
#include "suplproxypossubsesregistry.h"
#include "suplproxypossubsession.h"
#include "lbsdevloggermacros.h"

// ================= MEMBER FUNCTIONS =======================

/**
* C++ default constructor.
*/
CPosSubsessionRegistry::CPosSubsessionRegistry()
    {
	LBSLOG(ELogP1, "CPosSubsessionRegistry::CPosSubsessionRegistry() Begin and End\n");

    }

/**
 * Symbian constructor
 */
void CPosSubsessionRegistry::ConstructL()
    {
	LBSLOG(ELogP1, "CPosSubsessionRegistry::ConstructL() Begin\n");

    iRegistryIndex = CObjectIx::NewL();
    iRegistryContainerIndex = CObjectConIx::NewL();
    iRegistryContainer = iRegistryContainerIndex->CreateL();

	LBSLOG(ELogP1, "CPosSubsessionRegistry::ConstructL() End\n");
    }

/**
 * Two-phased constructor
 *
 * @return a new instance of this class
 */
CPosSubsessionRegistry* CPosSubsessionRegistry::NewL()
    {
	LBSLOG(ELogP1, "CPosSubsessionRegistry::NewL() Begin\n");

    CPosSubsessionRegistry* self = new (ELeave) CPosSubsessionRegistry;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

	LBSLOG(ELogP1, "CPosSubsessionRegistry::NewL() End\n");

    return self;
    }

/**
 * C++ destructor
 */
CPosSubsessionRegistry::~CPosSubsessionRegistry()
    {

	LBSLOG(ELogP1, "CPosSubsessionRegistry::~CPosSubsessionRegistry() Begin\n");

    delete iRegistryIndex;
    delete iRegistryContainerIndex;
    iRegistryContainer = NULL; // deleted by index

	LBSLOG(ELogP1, "CPosSubsessionRegistry::~CPosSubsessionRegistry() End\n");

    }

/**
 * Retreives the appropriate subsession.
 *
 * @param aHandle the subsession handle
 * @return a subsession
 */
CSuplProxyPosSubSession* CPosSubsessionRegistry::SubSessionFromHandleL(TUint aHandle)
    {
	LBSLOG(ELogP1, "CPosSubsessionRegistry::SubSessionFromHandleL() Begin and End\n");

    return static_cast<CSuplProxyPosSubSession*> (iRegistryIndex->At(aHandle));
    }


/**
 * Returns the maximum number of reference counting 
	* objects that the object index can hold before 
	* internal buffers need to be expanded.
 * @return the maximum number of reference counting objects.
 */
TInt CPosSubsessionRegistry::Count()
    {
	LBSLOG(ELogP1, "CPosSubsessionRegistry::CountL() Begin and End\n");
    return iRegistryIndex->Count();
    }

/**
 * Closes a subsession.
 *
 * @param aHandle a handle to the subsession
 */
void CPosSubsessionRegistry::CloseSubSession(TUint aHandle)
    {
	LBSLOG(ELogP1, "CPosSubsessionRegistry::CloseSubSession() Begin\n");

    if (iRegistryIndex->At(aHandle))
        {
        // Remove on a CObjectIx does close on the CObject
        iRegistryIndex->Remove(aHandle);
        }

	LBSLOG(ELogP1, "CPosSubsessionRegistry::CloseSubSession() End\n");

    }

/**
 * Adds an object instance to the registry and
 * transfers object ownership to the registry.
 *
 * @param aSubSession an object instance
 * @return a registry handle to the object instance.
 */
TInt CPosSubsessionRegistry::AddInstanceL(CSuplProxyPosSubSession* aSubSession)
    {
	LBSLOG(ELogP1, "CPosSubsessionRegistry::AddInstanceL() Begin\n");

    iRegistryContainer->AddL(aSubSession);

    TInt result = iRegistryIndex->AddL(aSubSession);

	LBSLOG(ELogP1, "CPosSubsessionRegistry::AddInstanceL() End\n");

	return result;
    }

// End of File

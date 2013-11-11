/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


// INCLUDE FILES
#include <e32base.h>
#include "testEPos_CPosSubsessionRegistry.h"
#include "testEPos_CPosSubSession.h"

// ================= MEMBER FUNCTIONS =======================

/**
* C++ default constructor.
*/
CPosSubsessionRegistry::CPosSubsessionRegistry()
    {
    }

/**
 * Symbian constructor
 */
void CPosSubsessionRegistry::ConstructL()
    {
    iRegistryIndex = CObjectIx::NewL();
    iRegistryContainerIndex = CObjectConIx::NewL();
    iRegistryContainer = iRegistryContainerIndex->CreateL();
    }

/**
 * Two-phased constructor
 *
 * @return a new instance of this class
 */
CPosSubsessionRegistry* CPosSubsessionRegistry::NewL()
    {
    CPosSubsessionRegistry* self = new (ELeave) CPosSubsessionRegistry;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * C++ destructor
 */
CPosSubsessionRegistry::~CPosSubsessionRegistry()
    {
    delete iRegistryIndex;
    delete iRegistryContainerIndex;
    iRegistryContainer = NULL; // deleted by index
    }

/**
 * Retreives the appropriate subsession.
 *
 * @param aHandle the subsession handle
 * @return a subsession
 */
CPosSubSession* CPosSubsessionRegistry::SubSessionFromHandleL(TUint aHandle)
    {
    return static_cast<CPosSubSession*> (iRegistryIndex->At(aHandle));
    }

/**
 * Retreives the appropriate subsession.
 *
 * @param aIndex the index in the list
 * @return a subsession, NULL if no session at specified index.
 */
CPosSubSession* CPosSubsessionRegistry::SubSessionFromIndex(TInt aIndex)
    {
    return static_cast<CPosSubSession*> ((*iRegistryIndex)[aIndex]);
    }

/**
 * Returns the maximum number of reference counting 
	* objects that the object index can hold before 
	* internal buffers need to be expanded.
 * @return the maximum number of reference counting objects.
 */
TInt CPosSubsessionRegistry::Count()
    {
    return iRegistryIndex->Count();
    }

/**
 * Closes a subsession.
 *
 * @param aHandle a handle to the subsession
 */
void CPosSubsessionRegistry::CloseSubSession(TUint aHandle)
    {
    if (iRegistryIndex->At(aHandle))
        {
        // Remove on a CObjectIx does close on the CObject
        iRegistryIndex->Remove(aHandle);
        }
    }

/**
 * Adds an object instance to the registry and
 * transfers object ownership to the registry.
 *
 * @param aSubSession an object instance
 * @return a registry handle to the object instance.
 */
TInt CPosSubsessionRegistry::AddInstanceL(CPosSubSession* aSubSession)
    {
    iRegistryContainer->AddL(aSubSession);
    return iRegistryIndex->AddL(aSubSession);
    }

// End of File

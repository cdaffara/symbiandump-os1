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



#include <lbscommon.h>
#include <ecom/ecom.h>

#include "epos_modulesinternal.h"
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include "EPos_CPosModulesEngine.h"
#include <lbs/epos_cposmodules.h>


CPosModules::CPosModules()
    {
    }

/**
 * Two-phased constructor.
 *
 * @return A new instance of this class.
 */
EXPORT_C CPosModules* CPosModules::OpenL()
    {
    CPosModules* self = new (ELeave) CPosModules;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

/**
 * Second phase constructor
 */
void CPosModules::ConstructL()
    {
    iEngine = CPosModulesEngine::NewL();
    }

/** Destructor.
 * @panic LocationSettings EPosLocSetPanicObserverNotStopped If observer is not unset by calling RemoveObserver().
 * @see SetObserverL() 
 * @see RemoveObserver()
 */
EXPORT_C CPosModules::~CPosModules()
    {
    delete iEngine;
    }

/**
 * Sets listener for location settings changes.
 *
 * Listening can be stopped by calling RemoveObserver() .
 *
 * @param aObserver [in] The observing object.
 * @leave KErrNoMemory No memory available to install observer.
 * @panic LocationSettings EPosLocSetPanicObserverIsAlreadySet If observer is already set.
 * @see RemoveObserver()
 */
EXPORT_C void CPosModules::SetObserverL( MPosModulesObserver& aObserver )
    {
    iEngine->SetObserverL( aObserver );
    }

/**
* Unsets change events observer and stops notifications.
* @see SetObserverL()
*/
EXPORT_C void CPosModules::RemoveObserver()
    {
    iEngine->RemoveObserver();
    }

/**
 * Retrieves a list of the existing modules in priority order and leaves
 * it in cleanup stack. Client takes ownership of returned object.
 *
 * Modules marked as invisible are not included in this list.
 *
 * @return List of visible modules
 * @see ModuleIdListL()
 */
EXPORT_C CPosModuleIdList* CPosModules::ModuleIdListLC()
    {
    return iEngine->GetPriorityListLC();
    }

/**
 * Retrieves a list of the existing modules in priority order.
 * Client takes ownership of returned object.
 *
 * Modules marked as invisible are not included in this list.
 *
 * @return List of visible modules
 * @see ModuleIdListLC()
 */
EXPORT_C CPosModuleIdList* CPosModules::ModuleIdListL()
    {
    CPosModuleIdList* list = iEngine->GetPriorityListLC();
    CleanupStack::Pop( list );
    return list;
    }
    
/**
 * Retrieves the attributes of a module.
 *
 * If the supplied position module info class is not supported, this
 * method will leave with code KErrNotSupported. Currently only
 * TPositionModuleInfo is supported.
 *
 * @param aModuleId [in] The ID of the module to retrieve information for.
 * @param aInfo [out] On return contains the module information.
 *
 * @leave KErrNotSupported Not supported module info class object is
 *                         is supplied as parameter.
 * @leave KErrNotFound No module with given ID is installed.
 */
EXPORT_C void CPosModules::GetModuleInfoL(
    TPositionModuleId aModuleId,
    TPositionModuleInfoBase& aInfo)
    {
    if ( aInfo.PositionClassType() != EPositionModuleInfoClass )
        {
        User::Leave( KErrArgument );
        }

    TPositionModuleInfo* info =
            reinterpret_cast<TPositionModuleInfo*>( &aInfo );

    iEngine->GetModuleInfoL( aModuleId, *info );
    }
    
/**
 * Changes the priority of a module. The priority of the other modules
 * will be shifted so that each module has a unique priority between 0
 * and number of modules - 1. 0 is the highest priority.
 *
 * Note that modules with visibility attribute set to 0 have no priority.
 *
 * @param aModuleId The ID of the module to set priority for.
 * @param aNewPriority The new priority for the module. Priority must be
 *   a number between 0 and number of modules - 1.
 *
 * @leave KErrLocked Several clients are accessing location settings simultaneously.
 * @leave KErrNotFound No module with specified ID is found in priority list.
 * @leave KErrArgument Priority value is out of bounds.
 */
EXPORT_C void CPosModules::SetModulePriorityL(
    TPositionModuleId aModuleId,
    TInt aNewPriority )
    {
    iEngine->SetModulePriorityL( aModuleId, aNewPriority );
    }
    
/**
 * Changes the priority of a module. The priority of the other modules
 * will be shifted so that each module has a unique priority between 0
 * and number of modules - 1. 0 is the highest priority.
 *
 * @param aOldPriority The module's old priority. Priority must be a
 *   number between 0 and number of modules - 1.
 * @param aNewPriority The new priority for the module. Priority must be
 *   a number between 0 and number of modules - 1.
 *
 * @leave KErrLocked Several clients are accessing location settings simultaneously.
 * @leave KErrArgument Priority value is out of bounds.
 */
EXPORT_C void CPosModules::SetModulePriorityL(
    TInt aOldPriority,
    TInt aNewPriority)
    {
    iEngine->SetModulePriorityL( aOldPriority, aNewPriority );
    }

/**
 * Changes the priority of modules. The modules priorities will be set
 * to reflect the module order in the list.
 *
 * @param aModuleIdList [in] The module list including the new priority order
 *   List must have the same set of module IDs as would be returned by
 *   ModuleIdListL().
 *
 * @leave KErrLocked Several clients are accessing location settings simultaneously.
 * @leave KErrArgument Module ID list is missing some module IDs or
 *                     has module IDs not found in the system.
 */
EXPORT_C void CPosModules::SetModulePrioritiesL(
    const CPosModuleIdList& aModuleIdList )
    {
    iEngine->SetModulePrioritiesL( aModuleIdList );
    }

/**
 * Retrieves the priority of a positioning module. 0 is the highest
 * priority.
 *
 * Note that modules with visibility attribute set to 0 have no priority.
 *
 * @param aModuleId The ID of the module to retrieve priority for.
 * @return The module priority.
 * @leave KErrNotFound No module with specified ID is found in priority list.
 */
EXPORT_C TInt CPosModules::PriorityL( TPositionModuleId aModuleId )
    {
    return iEngine->PriorityL( aModuleId );
    }

/**
 * Updates changeable attributes of a positioning module.
 *
 * @param aModuleId [in] The ID of the module, which attributes are going to be updated.
 * @param aUpdate [in] The attributes to update.
 *
 * @leave KErrLocked Several clients are accessing location settings simultaneously.
 * @leave KErrNotFound No module with given ID is installed.
 */
EXPORT_C void CPosModules::UpdateModuleL(
    TPositionModuleId aModuleId,
    const CPosModuleUpdate& aUpdate)
    {
    iEngine->UpdateModuleL( aModuleId, aUpdate );
    }

/** Verifies if a module is installed in the system.
 *
 *  @param aModuleId The ID of the module to check for existence
 *  @return ETrue if module exists, EFalse otherwise
 *
 */
EXPORT_C TBool CPosModules::IsModuleInstalled( TPositionModuleId aModuleId )
    {
    return iEngine->IsModuleInstalled( aModuleId );
    }

/** Returns visibility attribute of a module.
 *
 *  @param aModuleId The ID of the module,
 *                   which visibility attribute is to be checked.
 *  @return ETrue if module exists and visible, EFalse otherwise.
 *
 *  @leave KErrNotFound No module with given ID is found in the system.
 *  @see IsModuleInstalledL()
 */
EXPORT_C TBool CPosModules::IsModuleVisibleL( TPositionModuleId aModuleId )
    {
    return iEngine->IsModuleVisibleL( aModuleId );
    }

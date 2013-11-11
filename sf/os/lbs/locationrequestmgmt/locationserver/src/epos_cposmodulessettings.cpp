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

#include <lbs/epos_cposmodules.h>
#include "epos_mposmodulesettingslistener.h"
#include "epos_cposmodulessettings.h"

// ================= MEMBER FUNCTIONS =======================

/**
 * C++ default constructor.
 */
CPosModuleSettings::CPosModuleSettings()
    {
    }

/**
 * Symbian constructor.
 */
void CPosModuleSettings::ConstructL()
    {
    iModules = CPosModules::OpenL();
    iModules->SetObserverL( *this );
    }

/**
 * Two-phased constructor.
 */
CPosModuleSettings* CPosModuleSettings::NewL()
    {
    CPosModuleSettings* self = new ( ELeave ) 
        CPosModuleSettings();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


/**
 * Destructor.
 */
CPosModuleSettings::~CPosModuleSettings()
    {
    iListenerArray.Close();
    if ( iModules )
        {
        iModules->RemoveObserver();
        }
    delete iModules;
    }

/**
 * Get reference to CPosModules
 *
 * @return The reference to CPosModules.
 */
CPosModules& CPosModuleSettings::PosModules()
    {
    return *iModules;
    }


/**
 * From MPosModuleSettingsManager
 */
void CPosModuleSettings::AddListenerL(MPosModuleSettingsListener& aListener )
    {
    User::LeaveIfError( iListenerArray.Append( &aListener ) );
    }

/**
 * From MPosModuleSettingsManager
 */
void CPosModuleSettings::RemoveListener(MPosModuleSettingsListener& aListener)
    {
    TInt count = iListenerArray.Count();
    for ( TInt i = count-1; i >=0; i-- )
        {
        if ( iListenerArray[i] == &aListener )
            {
            iListenerArray.Remove( i );
            }
        }
    }

/**
 * From MPosModuleSettingsManager
 */
CPosModuleIdList* CPosModuleSettings::ModuleIdListL()
    {
    return iModules->ModuleIdListL();
    }

/**
 * From MPosModuleSettingsManager
 */
void CPosModuleSettings::GetModuleInfoL(TPositionModuleId aModuleId, TPositionModuleInfoBase& aInfo)
    {
    iModules->GetModuleInfoL(aModuleId, aInfo );
    }

/**
 * From MPosModuleSettingsManager
 */
TBool CPosModuleSettings::IsModuleVisibleL(TPositionModuleId aModuleId)
    {
    return iModules->IsModuleVisibleL(aModuleId );
    }

/**
 * From MPosModulesObserver
 */
void CPosModuleSettings::HandleSettingsChangeL( TPosModulesEvent aEvent )
    {
    TInt count = iListenerArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iListenerArray[i]->HandleModuleSettingsChangedL( aEvent );
        }
    }

//  End of File

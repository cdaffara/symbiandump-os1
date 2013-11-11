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



// INCLUDE FILES
#include <e32base.h>
#include "EPos_CProxyPositioner.h"
#include "epos_proxypositionerconstructparams.h"

//Panic category
#ifdef _DEBUG
_LIT(KEPosPluginFWPanicCategory, "EPos");
#endif

// ================= MEMBER FUNCTIONS =======================

// Destructor
EXPORT_C CProxyPositioner::~CProxyPositioner()
    {
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CProxyPositioner::CProxyPositioner()
    {
    }


// ---------------------------------------------------------
// CProxyPositioner::BaseConstructL
// ---------------------------------------------------------
//
EXPORT_C void CProxyPositioner::BaseConstructL( TAny* aConstructionParameters )
    {
    CPositioner::BaseConstructL( aConstructionParameters );

    TProxyPositionerConstructParams* params = 
        reinterpret_cast<TProxyPositionerConstructParams*>(aConstructionParameters);

    __ASSERT_DEBUG( params->iModuleStatusManager != NULL,
        User::Panic( KEPosPluginFWPanicCategory, KErrGeneral ) );
        
    __ASSERT_DEBUG( params->iModuleSettingsManager != NULL,
        User::Panic( KEPosPluginFWPanicCategory, KErrGeneral ) );

    iModuleStatusManager = params->iModuleStatusManager;
    iModuleSettingsManager = params->iModuleSettingsManager;
    }

// ---------------------------------------------------------
// CProxyPositioner::SetPositionCriteriaL
// ---------------------------------------------------------
//
EXPORT_C void CProxyPositioner::SetPositionCriteriaL(
    const TPositionCriteriaBase& /*aPositionCriteria*/)
    {
    }

// ---------------------------------------------------------
// CProxyPositioner::ModuleSettingsManager
// ---------------------------------------------------------
//
EXPORT_C MPosModuleSettingsManager& CProxyPositioner::ModuleSettingsManager() const
    {
    return *iModuleSettingsManager;
    }
        
// ---------------------------------------------------------
// CProxyPositioner::ModuleStatusManager
// ---------------------------------------------------------
//
EXPORT_C MPosModuleStatusManager& CProxyPositioner::ModuleStatusManager() const
    {
    return *iModuleStatusManager;
    }

//  End of File  

// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "HWRMFeatureMgr.h"
#include <featmgr/featurecontrol.h> 


// -----------------------------------------------------------------------------
// HWRMFeatureMgr::IsFeatureSupported
// Checks whether feature is supported
// -----------------------------------------------------------------------------

TBool HWRMFeatureMgr::IsFeatureSupported(const TUid aFeatureUid)
    {
    RFeatureControl featureControl;
    
    TInt err = featureControl.Open();

    TBool featureSupported(EFalse);

    if(err == KErrNone)
        {
        featureSupported = featureControl.FeatureSupported(aFeatureUid);
        }
    
    featureControl.Close();

    return featureSupported;
    }

// -----------------------------------------------------------------------------
// HWRMFeatureMgr::HWRMFeatureMgr
// C++ constructor
// -----------------------------------------------------------------------------
//

HWRMFeatureMgr::HWRMFeatureMgr()
    {
    }



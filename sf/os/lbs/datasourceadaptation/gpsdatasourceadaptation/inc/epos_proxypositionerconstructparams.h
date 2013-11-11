/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef EPOS_PROXYPOSITIONERCONSTRUCTPARAMS_H
#define EPOS_PROXYPOSITIONERCONSTRUCTPARAMS_H

#include "EPos_PositionerConstructParams.h"

// FORWARD DECLARATIONS
class MPosModuleStatusManager;
class MPosModuleSettingsManager;

// DATA TYPES
struct TProxyPositionerConstructParams : public TPositionerConstructParams
    {
    //Pointer to module status manager
    MPosModuleStatusManager* iModuleStatusManager;

    //Pointer to module settings manager
    MPosModuleSettingsManager* iModuleSettingsManager;
    };

#endif  // EPOS_PROXYPOSITIONERCONSTRUCTPARAMS_H

// End of File

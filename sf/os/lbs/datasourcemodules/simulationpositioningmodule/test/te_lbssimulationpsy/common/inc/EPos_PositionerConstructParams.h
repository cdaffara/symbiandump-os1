/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//
//  EPos_MPosParameterObserver.h
//
// Copyright (c) Symbian Software Ltd 2004.  All rights reserved.
//

#ifndef __EPOS_POSITIONERCONSTRUCTPARAMS_H__
#define __EPOS_POSITIONERCONSTRUCTPARAMS_H__

#include <e32def.h>

// FORWARD DECLARATIONS
class MPosStatusObserver;
class MPosParameterObserver;

/** Required construction parameters for @see CPositioner.

@internalComponent
@released
*/
struct TPositionerConstructParams
    {
    TUid iImplementationUid;
    MPosStatusObserver* iStatusObserver;
    MPosParameterObserver* iParamObserver;
    };


#endif  // __EPOS_POSITIONERCONSTRUCTPARAMS_H__

// End of File

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



#ifndef POSITIONERCONSTRUCTPARAMS_H
#define POSITIONERCONSTRUCTPARAMS_H

// FORWARD DECLARATIONS
class MPosStatusObserver;
class MPosParameterObserver;

// DATA TYPES
struct TPositionerConstructParams
    {
    TUid iImplementationUid;
    MPosStatusObserver* iStatusObserver;
    MPosParameterObserver* iParamObserver;
    };


#endif  // POSITIONERCONSTRUCTPARAMS_H

// End of File

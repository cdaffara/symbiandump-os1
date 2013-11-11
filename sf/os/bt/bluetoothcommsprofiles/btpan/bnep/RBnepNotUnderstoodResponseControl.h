// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef __RBNEPNOTUNDERSTOODRESPONSECONTROL_H
#define __RBNEPNOTUNDERSTOODRESPONSECONTROL_H 

#include "RBnepControl.h"

NONSHARABLE_CLASS(RBnepNotUnderstoodResponseControl) : public RBnepControl  
    {
public:
    typedef enum
        {
        KUnknownControlTypeResponseFieldOffset = KControlResponseOffset,
        KMaxSizeOfCommandNotUnderstoodResponse = KSizeOfBnepResponseCode // response code (2)
        } ControlCommandNotUnderstoodResponseConstants;

    void InitL ();
    void SetUnknownControlType (TUint8 aUnknownControlType);
    };

#endif

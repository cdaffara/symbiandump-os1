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

#ifndef __RBNEPFILTERMULTIADDRRESPONSECONTROL_H
#define __RBNEPFILTERMULTIADDRRESPONSECONTROL_H 

#include "BnepTypes.h"
#include "RBnepControl.h"

NONSHARABLE_CLASS(RBnepFilterMultiAddrResponseControl) : public RBnepControl  
    {
public:
    typedef enum
        {
        KMultiAddrResponseFieldOffset = KControlResponseOffset,
        KMaxSizeOfMultiAddrResponse = KSizeOfBnepControlType + KSizeOfBnepResponseCode // control type (1) + response code (2)	
        } MultiAddrResponseConstants;
    void InitL ();
    void SetMultiAddrResponseL (TFilterMultiAddrResponse aResponse);
    };
#endif

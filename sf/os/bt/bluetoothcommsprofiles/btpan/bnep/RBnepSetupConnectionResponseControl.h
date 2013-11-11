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

#ifndef __RBNEPSETUPCONNECTIONRESPONSECONTROL_H
#define __RBNEPSETUPCONNECTIONRESPONSECONTROL_H 

#include "RBnepControl.h"
#include "pancommon.h"

NONSHARABLE_CLASS(RBnepSetupConnectionResponseControl) : public RBnepControl  
    {
    typedef enum
        {
        KSetupResponseFieldOffset = KControlResponseOffset,
        KMaxSizeOfSetupResponse = KSizeOfBnepControlType + KSizeOfBnepResponseCode	// control type (1) + response code (2)
        } SetupResponseConstants;
public:
    void ConnectionSetupResponseL (TBnepSetupConnectionResponseMessage& aResponse);
    void InitL ();
    void SetConnectionSetupResponseL (TBnepSetupConnectionResponseMessage aResponse);
    };

#endif

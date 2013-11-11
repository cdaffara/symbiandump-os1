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

#ifndef __RBNEPFILTERNETTYPERESPONSECONTROL_H
#define __RBNEPFILTERNETTYPERESPONSECONTROL_H 

#include "RBnepControl.h"

NONSHARABLE_CLASS(RBnepFilterNetTypeResponseControl) : public RBnepControl  
    {
    typedef enum
        {
        KFilterNetTypeResponseFieldOffset = KControlResponseOffset,
        KMaxSizeOfFilterNetTypeResponse = KSizeOfBnepControlType + KSizeOfBnepResponseCode // control type (1) + response code (2)
        } NetTypeResponseConstants;

public:
    void InitL ();
    void SetNetTypeResponseL (TFilterNetTypeResponse aResponse);
    };
#endif

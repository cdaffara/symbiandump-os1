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

#ifndef __RBNEPCONTROL_H
#define __RBNEPCONTROL_H 

#include <es_mbuf.h>
#include "BnepTypes.h"

class TSglQueLink;

/**
   A BNEP Command.
   Used to encapsulate a BNEP control message of any type.
*/
NONSHARABLE_CLASS(RBnepControl) : public RMBufChain  
    {
public:
    typedef enum 
        { 
        KControlTypeOffset = 0,
        KControlResponseOffset = 1
        } Constants;

    TBnepControlType ControlType ();

public:
    TSglQueLink iLink;
    };

inline TBnepControlType RBnepControl::ControlType ()
    {
    return static_cast<TBnepControlType>(First()->Get(0));
    }


#endif

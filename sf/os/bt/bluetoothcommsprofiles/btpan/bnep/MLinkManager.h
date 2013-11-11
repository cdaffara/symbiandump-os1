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

#ifndef __MLINKMANAGER_H
#define __MLINKMANAGER_H 

#include <e32def.h>

class RBnepControl;

/**
   Interface between the BNEP link object and the BNEP channel manager object.
*/
class MLinkManager 
    {
public:
    virtual void Execute (RBnepControl* aCommand) = 0;
    virtual void RemoteDeviceDisconnect (TInt aError) = 0;
    TBool AwaitingResponse () const;
    void SetAwaitingResponse (TBool value);
private: 
    TBool iAwaitingResponse;
    };

inline TBool MLinkManager::AwaitingResponse () const
    {
    return iAwaitingResponse;
    }

inline void MLinkManager::SetAwaitingResponse (TBool value)
    {
    iAwaitingResponse = value;
    }


#endif

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

#ifndef __MPANDEVICE_H
#define __MPANDEVICE_H 


#include <bttypes.h>
#include "MPanDeviceOwner.h"
#include "RBnepFrame.h"

class MPanDevice 
    {
public:
    const TBTDevAddr& Addr () const;
    TBool ForThisDevice (const TBTDevAddr& aAddr) const;
    const TBTDevAddr& LocalAddr () const;
    virtual void Process (RBnepFrame& aPayload) = 0;
    void SetOwner (MPanDeviceOwner& aBridge);
    virtual void Stop () = 0;
    void SetOwnerCountPtr(TBnepBytesTransferred* aOwnerCountPtr);
public:
    TBTDevAddr iAddr;
protected:
    MPanDeviceOwner *iOwner;
    TBnepBytesTransferred* iOwnerCountPtr;
    };

inline const TBTDevAddr& MPanDevice::Addr () const
    {
    return iAddr;
    }

inline TBool MPanDevice::ForThisDevice (const TBTDevAddr& aAddr) const
    {
    return (iAddr == aAddr);
    }

inline const TBTDevAddr& MPanDevice::LocalAddr () const
    {
    return iOwner->DefaultAddress();
    }

inline void MPanDevice::SetOwner (MPanDeviceOwner& aBridge)
    {
    iOwner = &aBridge;
    }

inline void MPanDevice::SetOwnerCountPtr(TBnepBytesTransferred* aOwnerCountPtr)
	{
 	iOwnerCountPtr = aOwnerCountPtr;
	}
 


#endif

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

#ifndef __MPANDEVICEOWNER_H
#define __MPANDEVICEOWNER_H 

#include <bttypes.h>
#include "pancommon.h"

class MPanDevice;
class RBnepFrame;

class MPanDeviceOwner : public MBnepConnectionManager  
    {
    /**
       Upcall interface from MPanDevice.
       Concrete implementation is generally only by the bridge, but can
       be used for testing purposes.
    */

public:
    virtual TInt AttachDevice (MPanDevice& aDevice) = 0;
    virtual const TBTDevAddr& DefaultAddress () const = 0;
    virtual void LinkGoingDown (MPanDevice& aDevice) = 0;
	virtual void RemoteDeviceReady() = 0;
	virtual TInt Process (RBnepFrame& aPacket, const TBTDevAddr& aSourceIfAddr) = 0;
    virtual void StopLinks () = 0;
    virtual void DataCountUpdated() = 0;
    };


#endif

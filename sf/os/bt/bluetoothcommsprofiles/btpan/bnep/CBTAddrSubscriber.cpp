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

#include <bluetooth/logger.h>
#include <es_prot.h>
#include <bt_subscribe.h>
#include "CBTAddrSubscriber.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

CBTAddrSubscriber::CBTAddrSubscriber (TBTDevAddr& aAddr, CBnepLocalDevice& aLocal)
        : CActive(EPriorityStandard), iAddr(aAddr), iOwner(aLocal)
    {
    LOG_FUNC
    CActiveScheduler::Add(this);
    }

CBTAddrSubscriber::~CBTAddrSubscriber()
    {
    LOG_FUNC
    Cancel();
    }

/**
   Create an address subscriber and queue the first request.
   @param aAddr The address to update with published data.
   @param aLocal The local device node which will be informed when the device
   address changes.
*/
CBTAddrSubscriber* CBTAddrSubscriber::NewL (TBTDevAddr& aAddr, CBnepLocalDevice& aLocal)
    {
    LOG_STATIC_FUNC
    CBTAddrSubscriber* self = new(ELeave) CBTAddrSubscriber(aAddr, aLocal);
    CleanupStack::PushL(self);
    User::LeaveIfError(self->iDevAddrProperty.Attach(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetLocalDeviceAddress));

    // Attempt to fetch the device address property and activate the AO subscription on 
    // the property so that the upper layers can be notified when it is initialised by 
    // the BT stack. If the property hasn't been initialised yet, SyncFetch will 
    // return KErrUnderflow. In this context it is alright to ignore that return value.
    self->SyncFetch(ETrue);
    LOG(_L8("CBTAddrSubscriber requesting notifications"));
    CleanupStack::Pop(self);
    return self;
    }

/**
   Having been notified of a BDADDR change, notify owner if we
   have an address at the moment.
*/
void CBTAddrSubscriber::RunL ()
    {
    LOG_FUNC
    // Don't plan to restart the active object since the device address
    // is unlikely to change after this runs once. However, the AO
    // will reactivate if the property could not be fetched
    // properly (i.e. err != KErrNone)
    TInt err = SyncFetch(); 
    if(KErrNone == err)
        {
        // Notify the upper layers of the address update.
        iOwner.BDADDRChanged();
        }
    else
        {
        iDevAddrProperty.Subscribe(iStatus);
        SetActive();
        }
    }

void CBTAddrSubscriber::DoCancel ()
    {
    LOG_FUNC
    iDevAddrProperty.Cancel();
    }

/**
   Fetches the device address property synchronously if it exists.
   Conditionally queue a request to be notified of future BDADDR changes.
   By default, a request is not queued automatically.
   @param aRestart specify whether to queue request and schedule the active object. Default is EFalse.
   @return KErrUnderflow The property has zero length (i.e. is uninitialised)
   @internalComponent
*/
TInt CBTAddrSubscriber::SyncFetch (TBool aRestart)
    {
    LOG_FUNC
    TInt retval = KErrNone;
    TBuf8<KBTDevAddrSize> addr;
    iDevAddrProperty.Get(addr);
    if (addr.Length())
        {
        iAddr = TBTDevAddr(addr);
        LOG_BT(_L8("Setting address to %S"), iAddr);
        }
    else
        {
        // The property hasn't been initialised yet.
        retval = KErrUnderflow;
        }

    if(aRestart)
        {
		if(retval == KErrNone)
			{
			// The BT stack was already up when BNEP started.  Call the
			// BDADDRChanged method to inform the NIF that the link layer 
			// is up.
			iOwner.BDADDRChanged();
			}
		else
			{
	        // Watch out for any future updates on the property.
	        iDevAddrProperty.Subscribe(iStatus);
	        SetActive();
			}
        }
    return retval;
    }

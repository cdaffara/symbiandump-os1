// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PANCTRL_H
#define PANCTRL_H

#include <es_sock.h>
#include <es_sock_partner.h>

/**
@file
@note Constant definitions for clients using PAN agent.
Includes constants for controlling the state of the PAN network
*/

/**
Base value for constants for use with Control() call
@publishedPartner
@released
@see RConnection::Control
@note KCO* values for use with PAN agent
*/
const TInt KCOAgentPanBase = 0x1000;


/**
@publishedPartner
@released
@see RConnection::Control

Bring a new device into the PAN network.  The Control() call will complete as
soon as the PAN agent has started the process, so it may be several seconds
before the device is addressable.  It is also possible for the connection
attempt to fail even if the Control() method returns KErrNone, due to the
actual processing occuring asynchronously.

@code
RConnection connection;
TBTDevAddr remoteDeviceToAdd;
...
<Connect RConnection object, and find the remote device to add to the PAN>
...
TPtr8 ptr = remoteDeviceToAdd.Des();
User::LeaveIfError(connection.Control(KCOLAgent, KCOAgentPanConnectDevice, ptr));
@endcode

The use of a temporary TPtr is safe, as the Control method is synchronous.

@note KCO* value for use with PAN agent
*/
const TInt KCOAgentPanConnectDevice = KCOAgentPanBase | KConnReadUserDataBit;


/**
@publishedPartner
@released
@see RConnection::Control

Attempt to remove a connected device from the PAN network.

@code
RConnection connection;
TBTDevAddr remoteDeviceToRemove;
...
<Connect RConnection object, and find the remote device to remove from the PAN>
...
TPtr8 ptr = remoteDeviceToRemove.Des();
User::LeaveIfError(connection.Control(KCOLAgent, KCOAgentPanDisconnectDevice, ptr));
@endcode

The use of a temporary TPtr is safe, as the Control method is synchronous.

@note KCO* value for use with PAN agent
*/
const TInt KCOAgentPanDisconnectDevice = KCOAgentPanBase + 1 | KConnReadUserDataBit;



/**
@publishedPartner
@released
@see RConnection::Control

Return a list of connected devices.  If the buffer supplied is too small to
hold all of the device addresses, as many as will fit will be returned.  The
descriptor length will be adjusted to reflect the total connected device count.

@note Devices which are still in the process of connecting to the PAN network
will not be included.

@code
RConnection connection;
...
<Connect RConnection object>
...
const TUint KMaxDevicesForSimultaneousSelection = 7;
   
TFixedArray<TBTDevAddr, KMaxDevicesForSimultaneousSelection> deviceAddrList;
deviceAddrList.Reset();
  
TPckg<TFixedArray<TBTDevAddr, KMaxDevicesForSimultaneousSelection> > pckg(deviceAddrList);
  
User::LeaveIfError(connection.Control(KCOLAgent, KCOAgentPanEnumerateDevices, pckg));
    
for(TInt i=0; i < pckg().Count(); ++i)
   {
   if (pckg()[i]!=0)
   	  {
	  const TBTDevAddr devAddr = pckg().At(i);
	  //......
	  //......  <-Implmentation to use the BT address
	  //......
      }
   }
@endcode

@note KCO* value for use with PAN agent
*/
const TInt KCOAgentPanEnumerateDevices = KCOAgentPanBase + 2 | KConnWriteUserDataBit;


/**
@publishedPartner
@released
@see RConnection::Control

Return a list of device addresses that are allowed to use the uplink
@code
RConnection connection;
...
<Connect RConnection object>
...
const TUint KMaxDevicesForSimultaneousSelection = 7;
       
TFixedArray<TBTDevAddr, KMaxDevicesForSimultaneousSelection> deviceAddrList;
devices.Reset();
       
TPckg<TFixedArray<TBTDevAddr, KMaxDevicesForSimultaneousSelection> > pckg(deviceAddrList);
       
connection.Control(KCOLAgent, KCOAgentPanEnumerateUplinkAccessDevices, pckg);
   
for(TInt i=0; i < pckg().Count(); ++i)
   {
   if (pckg()[i]!=0)
   	  {
	  const TBTDevAddr devAddr = pckg().At(i);
	  //......
	  //......  <-Implmentation to use the BT address
	  //......
      }
   }
@endcode 
*/
const TInt KCOAgentPanEnumerateUplinkAccessDevices = KCOAgentPanBase + 3 | KConnWriteUserDataBit;


#endif // PANCTRL_H
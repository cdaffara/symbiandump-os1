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

#ifndef __CBNEPLOCALDEVICE_H
#define __CBNEPLOCALDEVICE_H 

#include <networking/pktdrv.h>
#include "MPanDevice.h"

class CBTAddrSubscriber;
class CLANLinkCommon;


/**
Flow control constants for CPktDrvBase::Send() method
*/
const TUint KFlowOff = 0;
const TUint KReadyForMoreData = 1;

/**
   The local device
   There will always be one (and only one) of these objects owned by the bridge
   It acts as a link between the ethernet nif and the PAN network.  This is
   mainly of relevance when the local role is as a GN, as the bridge will be
   routing packets between the PANUs.
*/
NONSHARABLE_CLASS(CBnepLocalDevice) : public CPktDrvBase, 
                         			  public MPanDevice  
    {

public:
    ~CBnepLocalDevice();
    void BDADDRChanged ();
	void RemoteDeviceReady ();
	void AllRemoteDevicesDisconnected ();
    TInt Control (TUint UNUSEDPARAMETER1, TUint UNUSEDPARAMETER2, TDes8& UNUSEDPARAMETER3, TAny* UNUSEDPARAMETER4 = 0);
    TUint8* GetInterfaceAddress () const;
    TInt GetMulticastList (const THWAddr* UNUSEDPARAMETER1, TInt& UNUSEDPARAMETER2) const;
    TInt GetRxMode () const;
    TInt InterfacePowerDown ();
    TInt InterfacePowerUp ();
    TInt InterfaceResume ();
    TInt InterfaceSleep ();
    static CBnepLocalDevice* NewL (CPktDrvFactory& aFactory, CLANLinkCommon* aParent);
    TInt Notification (TAgentToNifEventType aEvent, TAny* aInfo = 0);
    virtual void Process (RBnepFrame& aFrame);
    TInt ResetInterface ();
    TInt Send (RMBufChain& aPkt);
    TInt SetInterfaceAddress (const THWAddr& UNUSEDPARAMETER1);
    TInt SetMulticastList (const THWAddr* UNUSEDPARAMETER1, TInt UNUSEDPARAMETER2);
    TInt SetRxMode (TRxMode UNUSEDPARAMETER1);
    TInt StartInterface ();
    TInt StopInterface ();
    void Stop ();
    CBTAddrSubscriber *iSubscriber;
private:
    CBnepLocalDevice (CPktDrvFactory& aFactory, CLANLinkCommon* aParent);
    void ConstructL (CLANLinkCommon* UNUSEDPARAMETER1);
private: 
    TBool iInterfaceStarted;
	TBool iBDAddressAvailable;
	TBool iLinkLayerUp;

    };
#endif

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

#ifndef __CBNEPLINK_H
#define __CBNEPLINK_H 

#include <e32base.h>
#include <e32def.h>
#include <comms-infras/eintsock.h>
#include "MLinkManager.h"
#include "MPanDevice.h"
#include "RBnepFrame.h"
#include "bneputils.h"

class CSocketReader;
class CSocketWriter;
class CNetTypeFilterTable;
class CMultiAddrFilterTable;


const TInt KBnepMTU = 1691;
const TInt KErrLinkFlowedOff = 1;



/**
   A remote device connected to the bridge by BNEP.
   There will be zero or more of these linked to the bridge, plus the local device.
*/
NONSHARABLE_CLASS(CBnepLink) : public CBase, 
                  public MPanDevice  
    {
public:
    ~CBnepLink();
    static CBnepLink* NewL (MPanDeviceOwner& aBridge, RInternalSocket& aSocket, MLinkManager& aParser);
    void Process (RBnepFrame& aFrame);
    void QueueOnOutput (RBnepControl* aCommand);
    void ReadComplete (RMBufChain& aChain);
    void RemoteDeviceDisconnect (TInt aError);
    void ResumeDataTransfer ();
	void RemoteDeviceReady();
    TInt SendFrame (RBnepFrame& aFrame);
    void SuspendDataTransfer ();
    void Install (CMultiAddrFilterTable* aFilterTable);
    void Install (CNetTypeFilterTable* aFilterTable);
    void Proceed ();
    void Stop ();
    void Invalidate ();
	void UpdateOwnerCount(TInt aReceivedCount, TInt aSentCount);

	void SetUplinkAccessAllowedForBnepLink(TBool aAllowed);
	
private:
    CBnepLink (MPanDeviceOwner& aBridge, RInternalSocket& aSocket, MLinkManager& aParser);
    void ConstructL ();

private:
    // Data Members for Class Attributes
    MLinkManager& iLinkManager;
    TBool iNegotiating;
    
private: 
    // Data Members for Has Relationships
    RInternalSocket iSocket;
    CSocketReader* iReader;
    CSocketWriter* iWriter;
    CMultiAddrFilterTable* iMultiAddrFilters;
    CNetTypeFilterTable* iNetTypeFilters;
    RBnepFrame iOutgoing;
    RBnepFrame iIncoming;
    TBool iInvalid;
    
	TBool iUplinkAccessAllowed;
    };


#endif

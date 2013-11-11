// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// btgpsdiscovery.h
// 
//

#ifndef BTGPSDISCOVERY_H
#define BTGPSDISCOVERY_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <btdevice.h>	// For CBTDevice
#include <btsdp.h>		// For KSerialPortUUID

class CSdpAgentSim;

/**
 *  MBTGPSSdpResultReceiver
 * 
 */
class MBTGPSSdpResultReceiver
    {
public: 
    /**
    * 
    */
    virtual void ServiceAttributeSearchComplete(TInt aErr ) = 0;
    };

class CBTGPSDiscovery : public CBase, MSdpAgentNotifier, MSdpAttributeValueVisitor
    {
public:
    // Cancel and destroy
    ~CBTGPSDiscovery ();
    
    // Two-phased constructor.
    
    // Two-phased constructor.
    static CBTGPSDiscovery* NewL (MBTGPSSdpResultReceiver* aBTEngSdpResultReceiver);
    
    // Two-phased constructor.
    static CBTGPSDiscovery* NewLC (MBTGPSSdpResultReceiver* aBTEngSdpResultReceiver);

public:
    // New functions
    TInt RemoteProtocolChannelQueryL(const TBTDevAddr& aAddr, TInt aService);
    TInt ParseRfcommChannel(TInt& aType);
    
    private:
    // C++ constructor
    CBTGPSDiscovery (MBTGPSSdpResultReceiver* aBTEngSdpResultReceiver);

private:
    // MSdpAgentNotifier methods
    void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
    void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
    void AttributeRequestComplete(TSdpServRecordHandle, TInt /*aError*/);
    
    // MSdpAttributeValueVisitor methods
    void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
    void StartListL(CSdpAttrValueList& /*aList*/){};
    void EndListL(){};
    
private:
    MBTGPSSdpResultReceiver* iBTSdpResultReceiver;
    TSdpServRecordHandle* iHandle;
    CSdpAgent* iSdpAgent;
    TInt iChannel;
    TBool iValid;
    TInt iMatchesFound;
    
#ifdef	LBS_BTGPSPSY_SIM
    // Simulated CSdpAgent, used for simulated devices
    CSdpAgentSim* iSdpAgentSim;
#endif
    };

#endif // BTGPSDISCOVERY_H

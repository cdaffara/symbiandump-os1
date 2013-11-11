// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32std.h>
#include "BTGPSSdpHandler.h"
#include "BTGPSConnectManagerExt.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSHandlerFactory.h"
#include "BTGPSLogging.h"
#include "BTGPSLogging.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSSdpHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSSdpHandler* CBTGPSSdpHandler::NewL(MBTGPSConnectManagerExt& aManagerExt)
    {
    CBTGPSSdpHandler* self = new (ELeave) CBTGPSSdpHandler(
        aManagerExt);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSSdpHandler::~CBTGPSSdpHandler
// -----------------------------------------------------------------------------
CBTGPSSdpHandler::~CBTGPSSdpHandler()
    {
    delete iBtGPSDiscovery;
    }


// -----------------------------------------------------------------------------
// CBTGPSSdpHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSSdpHandler::ConstructL()
    {
    TRACESTRING("CBTGPSSdpHandler::ConstructL start...")

    CBTDevice* btDevice = &(iManagerExt.DeviceManager().BtDevice());

    iChannelNumberErr = KErrNotFound;
    iBtGPSDiscovery = CBTGPSDiscovery::NewL( this );
    iBtGPSDiscovery->RemoteProtocolChannelQueryL(btDevice->BDAddr(), KSerialPortUUID );
        
    TRACESTRING("CBTGPSSdpHandler::ConstructL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSSdpHandler::CBTGPSSdpHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSSdpHandler::CBTGPSSdpHandler(MBTGPSConnectManagerExt& aManagerExt)
    : iManagerExt(aManagerExt)
    {
    }


// -----------------------------------------------------------------------------
// CBTGPSSdpHandler::RunL
// -----------------------------------------------------------------------------
//void CBTGPSSdpHandler::ServiceSearchComplete(const RSdpRecHandleArray& /*aResult*/, TUint /*aTotalRecordsCount*/, TInt /*aErr */)
//    {
    //do nothing
//    }
                                                         
// -----------------------------------------------------------------------------
// CBTGPSSdpHandler::AttributeSearchComplete
// -----------------------------------------------------------------------------
//void CBTGPSSdpHandler::AttributeSearchComplete( TSdpServRecordHandle /*aHandle*/, const RSdpResultArray& /*aAttr*/, TInt /*aErr*/ )
//	{
    //do nothing
 //   }
            
// -----------------------------------------------------------------------------
// CBTGPSSdpHandler::ServiceAttributeSearchComplete
// -----------------------------------------------------------------------------
void CBTGPSSdpHandler::ServiceAttributeSearchComplete(TInt aErr )
    {
    TRACESTRING2("CBTGPSSdpHandler::ServiceAttributeSearchComplete start...%d", 
        aErr )
    if( aErr == KErrNone || aErr == KErrEof)
        {
        TInt channel = KErrNotFound;
        TInt err = iBtGPSDiscovery->ParseRfcommChannel(channel );
        if( err == KErrNone )
            {
            TRACESTRING2("channel number:%d", channel)
            iManagerExt.DeviceManager().SetPort(channel);
            iChannelNumberErr = KErrNone;
            }
        }
    // If the bluetooth device needs to be reinitialised it aErr will be EPageTimedOut (-6004)
    if ((aErr < KErrNone ))
        {
        iManagerExt.HandlerComplete( 
            ESdpHandler,
            iChannelNumberErr );
        }
    TRACESTRING("CBTGPSSdpHandler::ServiceAttributeSearchComplete end")
    }
            
// -----------------------------------------------------------------------------
// CBTGPSSdpHandler::DeviceSearchComplete
// -----------------------------------------------------------------------------
//void CBTGPSSdpHandler::DeviceSearchComplete( CBTDevice* /*aDevice*/, TInt /*aErr*/ )
//    {
    //do nothing
 //   }

//  End of File




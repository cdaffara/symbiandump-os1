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

#include "BTGPSHandlerFactory.h"
#include "BTGPSPanic.h"
#include "BTGPSDeviceSelector.h"
#include "BTGPSSdpHandler.h"
#include "BTGPSPairingHandler.h"
#include "BTGPSInitHandler.h"
#include "BTGPSSocketHandler.h"
#include "BTGPSDeviceInfoHandler.h"
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
	#include "btgpsgetnextdevice.h"
	#include "btgpscheckdevicepairing.h"
#endif


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TBTGPSHandlerFactory::TBTGPSHandlerFactory
// -----------------------------------------------------------------------------
TBTGPSHandlerFactory::TBTGPSHandlerFactory(
        MBTGPSConnectManagerExt& aManagerExt)
    : iManagerExt(aManagerExt)
    {
    }

// -----------------------------------------------------------------------------
// TBTGPSHandlerFactory::ConstructHandlerL
// -----------------------------------------------------------------------------
CBase* TBTGPSHandlerFactory::ConstructHandlerL(TInt aId)
    {
    CBase* handler = NULL;
    switch(aId)
        {
        case EDeviceSelector:
            handler = CBTGPSDeviceSelector::NewL(iManagerExt);
            break;
        case EPairingHandler:
            handler = CBTGPSPairingHandler::NewL(iManagerExt);
            break;
        case ESdpHandler:
            handler = CBTGPSSdpHandler::NewL(iManagerExt);
            break;
        case EInitHandler:
            handler = CBTGPSInitHandler::NewL(iManagerExt);
            break;
        case ESocketHandler:
            handler = CBTGPSSocketHandler::NewL(iManagerExt);
            break;
        case EDeviceInfoHandler:
            handler = CBTGPSDeviceInfoHandler::NewL(iManagerExt);
            break;
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
        case EGetNextDevice:
        	handler = CBTGPSGetNextDevice::NewL(iManagerExt);
        	break;
        case ECheckDevicePairing:
        	handler = CBTGPSCheckDevicePairing::NewL(iManagerExt);
        	break;
#endif
        default:
            __ASSERT_DEBUG(EFalse, Panic(EPanicInvalidCase));
        }
    return handler;
    }

//  End of File




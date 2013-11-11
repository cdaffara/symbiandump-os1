/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/




#ifndef BTGPSSDPHANDLER_H
#define BTGPSSDPHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "btgpsdiscovery.h"

// FORWARD DECLARATIONS
class MBTGPSConnectManagerExt;

// CLASS DECLARATION

/**
*  Connection handler that do SDP search. The RFCOMM channel id will be stored
*  to device manager when the it is found.
*
*/
class CBTGPSSdpHandler: public CBase, public MBTGPSSdpResultReceiver
    {
    public: 

        /**
        * Two-phase construction.
		* @param aManagerExt Reference to connect manager.
        */
        static CBTGPSSdpHandler* NewL(MBTGPSConnectManagerExt& aManagerExt);

        /**
        * Destructor
        */
        virtual ~CBTGPSSdpHandler();

    private:   
        /**
        * From MBTGPSSdpResultReceiver
        */
        virtual void ServiceAttributeSearchComplete(TInt aErr );
        
    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSSdpHandler(MBTGPSConnectManagerExt& aManagerExt);
        
    private:
        //Manager
        MBTGPSConnectManagerExt& iManagerExt;
        
        //BT Engine
        CBTGPSDiscovery* iBtGPSDiscovery;

        //Channel number server result
        TInt iChannelNumberErr;
    };
#endif
// End of File


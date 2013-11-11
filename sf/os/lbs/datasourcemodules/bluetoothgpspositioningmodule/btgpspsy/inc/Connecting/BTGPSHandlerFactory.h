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




#ifndef BTGPSHANDLERFACTORY_H
#define BTGPSHANDLERFACTORY_H

//  INCLUDES
#include <e32base.h> 

// CLASS DECLARATION
class MBTGPSConnectManagerExt;

//Connecting handler id
enum TConnectingHandlerId
    {
    EDeviceSelector,   		///Device selection handler id
    EPairingHandler,   		///Device pairing handler id
    ESdpHandler,       		///Sdp search handler id
    EBtOnQueryHandler, 		///Bt On query handler id
    ESocketHandler,    		///Socket connection handler id
    EInitHandler,      		///Initialisation complete handler id
    EDeviceInfoHandler,		///Device bonded information handler
    EGetNextDevice,	   		///Get the next device from Device Config API
    ECheckDevicePairing,	///Check the pairing of the current device
    };

/**
*  Factory class to construct all connection handlers.
*
*/
class TBTGPSHandlerFactory
    {
    public: 
        /**
        * Constructor
		* @param aManagerExt Reference to connect manager.
        */
        TBTGPSHandlerFactory(
            MBTGPSConnectManagerExt& aManagerExt);

    public: //Factory function
        /**
        * Construct connection handler by handler Id
		* @param aId The handler id that should be constructed
		* @return Pointer to the handler that has been constructed.
        */
        CBase* ConstructHandlerL(TInt aId);

    private:
        //Manager ext
        MBTGPSConnectManagerExt& iManagerExt;
        
    };
#endif
// End of File


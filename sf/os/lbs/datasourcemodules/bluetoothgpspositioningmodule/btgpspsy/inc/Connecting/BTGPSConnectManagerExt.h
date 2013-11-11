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




#ifndef BTGPSCONNECTMANAGEREXT_H
#define BTGPSCONNECTMANAGEREXT_H

//  INCLUDES

#include <e32base.h> 

// FORWARD DECLARATIONS
class CBTGPSDeviceManager;
class CBTGPSConstantsManager;
class CBTGPSSettingManager;
class CBTGPSDeviceListManager;
class RSocketServ;
class RSocket;

// CLASS DECLARATION

/**
*  Interface class for BT connection handlers. BT connection handlers will
*  use reference of this interface to get connection to device manager or 
*  other global objects. 
*
*/
class MBTGPSConnectManagerExt
    {
    public: 

        /**
        * Get reference to device manager.
		* @return Reference to device manager.
        */
        virtual CBTGPSDeviceManager& DeviceManager() const = 0;
        
        /**
        * Get reference to constants manager
        * @return Reference to contants manager
        */
        virtual CBTGPSConstantsManager& ConstantsManager() const = 0;

        /**
        * Get reference to setting manager
        * @return Reference to setting manager
        */
        virtual CBTGPSSettingManager& SettingManager() const = 0;

        /**
        * Get reference to RSocketServ and Socket.
		* @param aSocketServ On return, contains reference of socket server, which
		*                    is owned by connect manager.
		* @param aSocket On return, contains reference of RSocket, which is
		*                owned by connect manager.
        */
        virtual void GetSocket(
            RSocketServ*& aSocketServ,
            RSocket*& aSocket) = 0;
            
        /**
        * Disconnect BT device
        */
        virtual void DisconnectBtDevice() = 0;

        /**
        * Interface for handler to report operation result.
		* @param aId The id of current handler
		* @param aErr The operation err code. KErrNone if operation
		*        has been executed successfully.
        */
        virtual void HandlerComplete(TInt aId, TInt aErr) = 0;
        
        /**
         * Get reference to Device List Manager
         * @return Reference to Device List Manager
         */
        virtual CBTGPSDeviceListManager& DeviceListManager() const = 0; 
    };
#endif
// End of File


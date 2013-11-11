/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef BTGPSDEVICEINFOHANDLER_H
#define BTGPSDEVICEINFOHANDLER_H

//  INCLUDES
 
#include "btgpsdevman.h"
#include "BTGPSConnectManagerExt.h"
#include <bt_sock.h>

// FORWARD DECLARATIONS
class MBTGPSConnectManagerExt;

// CLASS DECLARATION

/**
*  The handler that handles finding bonded BT devices. This class is responsible
*  to find if the device in device manager is paired. If the device is paired, 
*  KErrNone is returned through MBTGPSConnectManagerExt interface. If it's not
*  paired or any other failure, KErrNotFound is returned.
*
*/
class CBTGPSDeviceInfoHandler: public CBase, private MBTGPSDevManObserver
    {
    public: 

        /**
        * Two-phase construction.
        */
        static CBTGPSDeviceInfoHandler* NewL(
            MBTGPSConnectManagerExt& aManagerExt);

        /**
        * Destructor
        */
        virtual ~CBTGPSDeviceInfoHandler();

    private:
        /**
        * from MBTGPSDevManObserver
        */
        virtual void HandleGetDevicesComplete(TInt aErr, RBTDeviceArray* aDeviceArray);
        
        /**
        * from MBTGPSDevManObserver
        */
        virtual void HandleAddDeviceComplete(TInt aErr);

        /**
        * from MBTGPSDevManObserver
        */
        virtual void HandleDeleteDevicesComplete(TInt aErr);

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        * @param aManagerExt Reference to manager ext
        */
        CBTGPSDeviceInfoHandler(
            MBTGPSConnectManagerExt& aManagerExt);
            
        /**
        * Handler complete static callback function
        * @param aAny Pointer to this
        */
        static TInt HandlerCompleteCallback(TAny* aAny);
        
        /**
        * Handler complete notify function
        */
        void HandlerCompleteNotify();
        
    private:
        //Manager Ext
        MBTGPSConnectManagerExt& iManagerExt;

        //BT Device Manager
        CBTGPSDevMan* iBtDevMan;
        
        // result array needed when using CBTGPSDevMan
        RBTDeviceArray* iDeviceArray;
        
        //complet idle object
        CIdle* iIdle;
        

        //BT Scoket addr
        TBTSockAddr iBtSockAddr;
        
        //BT address validity
        TInt iValid;
        
        //Operation error code
        TInt iErrCode;

    };
#endif

// End of File


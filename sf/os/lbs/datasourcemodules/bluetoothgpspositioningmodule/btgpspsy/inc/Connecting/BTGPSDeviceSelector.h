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




#ifndef BTGPSDEVICESELECTOR_H
#define BTGPSDEVICESELECTOR_H

//  INCLUDES
#include <e32std.h> 
#include <btextnotifiers.h>

// FORWARD DECLARATIONS
class MBTGPSConnectManagerExt;

// CLASS DECLARATION

/**
*  The handler that handles device selection. This class is responsible
*  to show device selection query note to end user. Selected device address
*  is stored to device manager. If user cancel the selection or other reason
*  that a device can't be selected, error code is returned through 
*  MBTGPSConnectManagerExt interface.
*
*/
class CBTGPSDeviceSelector: public CActive
    {
    public: 

        /**
        * Two-phase construction.
        */
        static CBTGPSDeviceSelector * NewL(
            MBTGPSConnectManagerExt& aManagerExt);

        /**
        * Destructor
        */
        virtual ~CBTGPSDeviceSelector();

    private:
        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        TInt RunError(TInt aError);

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        * @param aId Timer id.
        * @param aListener The timer event listener.
        */
        CBTGPSDeviceSelector(
            MBTGPSConnectManagerExt& aManagerExt);
        
    private:
        //Manager Ext
        MBTGPSConnectManagerExt& iManagerExt;
        
        //Notifier
        RNotifier iNotifier;

        //BT selection response
        TBTDeviceResponseParamsPckg iBTSelectionResponse;
        
    };
#endif
// End of File


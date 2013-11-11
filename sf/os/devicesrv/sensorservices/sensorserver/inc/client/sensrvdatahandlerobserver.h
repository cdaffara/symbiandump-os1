/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel data observer.
*
*/


#ifndef MSENSRVDATAHANDLEROBSERVER_H
#define MSENSRVDATAHANDLEROBSERVER_H


#include <e32base.h>
#include "sensrvtypes.h"


/**
 *  Channel data observer. Callback interface to indicate when a new data is 
 *  available.
 * 
 *  @lib SensrvClient.lib
 *  @since S60 5.0  
 */

class MSensrvDataHandlerObserver
    {
      public:
        /**
        * Indicates data is available in the receiving buffer
        *
        * @since S60 5.0
        * @param aCount Data count in receiving buffer
        * @param aDataLost Data lost count
        * @return void
        */  
        virtual void DataReceived( TInt aCount, TInt aDataLost ) = 0;
        
        /**
        * Data listening failed. 
        * If error was fatal, channel has also been closed and sensor server 
        * session terminated.
        * If error was minor, some data has potentially been lost.
        *
        * @since S60 5.0        
        * @param aError Error code.
        * @return void 
        */
        virtual void DataError( TSensrvErrorSeverity aError ) = 0;
        
        /**
        * Reopen channel.
        * Property listening and condition listening need restarting
        * 
        * @since S60 5.0        
        * @param aMaxBufferCount New maximum client buffer count
        * @return void 
        */
        virtual void ReopenChannelForDataListeningL( TInt& aMaxBufferCount ) = 0;
    };

#endif //MSENSRVDATAHANDLEROBSERVER_H

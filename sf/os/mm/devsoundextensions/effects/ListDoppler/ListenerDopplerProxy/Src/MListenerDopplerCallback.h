/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  interface listner doppler call back 
*
*/




#ifndef LISTENERDOPPLERCALLBACK_H
#define LISTENERDOPPLERCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from Doppler Event Observer.
*
*  @since 3.0
*/
class MListenerDopplerCallback
    {
    public:

        /**
        * Invoked by the Doppler Event Observer when the doppler object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the doppler data
        */
        //virtual void DopplerEvent( TEfDopplerDataPckg& aDataPckg ) = 0;
        virtual void ListenerDopplerEvent( TDesC8& aDataPckg ) = 0;


    };



#endif      // LISTENERDOPPLERCALLBACK_H

// End of File

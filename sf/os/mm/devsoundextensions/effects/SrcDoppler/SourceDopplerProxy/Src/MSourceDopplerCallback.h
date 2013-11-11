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
* Description:  interface for source doppler call back 
*
*/




#ifndef SOURCEDOPPLERCALLBACK_H
#define SOURCEDOPPLERCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from Doppler Event Observer.
*
*  @since 3.0
*/
class MSourceDopplerCallback
    {
    public:

        /**
        * Invoked by the Doppler Event Observer when the doppler object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the doppler data
        */
        //virtual void DopplerEvent( TEfDopplerDataPckg& aDataPckg ) = 0;
        virtual void SourceDopplerEvent( TDesC8& aDataPckg ) = 0;


    };



#endif      // SOURCEDOPPLERCALLBACK_H

// End of File

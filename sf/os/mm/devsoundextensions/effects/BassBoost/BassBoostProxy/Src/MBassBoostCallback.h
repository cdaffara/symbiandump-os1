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
* Description:  Interface for Bass Boost call back 
*
*/




#ifndef BASSBOOSTCALLBACK_H
#define BASSBOOSTCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from BassBoost Event Observer.
*
*  @since 3.0
*/
class MBassBoostCallback
    {
    public:

        /**
        * Invoked by the BassBoost Event Observer when the bassboost object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the bassboost data
        */
        //virtual void BassBoostEvent( TEfBassBoostDataPckg& aDataPckg ) = 0;
        virtual void BassBoostEvent( TDesC8& aDataPckg ) = 0;


    };



#endif      // BASSBOOSTCALLBACK_H

// End of File

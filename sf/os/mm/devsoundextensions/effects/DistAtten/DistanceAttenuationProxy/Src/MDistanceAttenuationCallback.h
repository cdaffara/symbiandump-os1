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
* Description:  Interface for distnace attenuation callback
*
*/




#ifndef MDISTANCEATTENUATIONCALLBACK_H
#define MDISTANCEATTENUATIONCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from DistanceAttenuation Event Observer.
*
*  @since 3.0
*/
class MDistanceAttenuationCallback
    {
    public:

        /**
        * Invoked by the Distance Attenuation Event Observer when the DistanceAttenuation object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the distance attenuation data
        */
        virtual void DistanceAttenuationEvent( TDesC8& aDataPckg ) = 0;
    };



#endif      // MDISTANCEATTENUATIONCALLBACK_H

// End of File

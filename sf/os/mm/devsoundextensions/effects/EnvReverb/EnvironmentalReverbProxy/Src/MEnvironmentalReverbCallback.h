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
* Contributors: Interface for Env Reverb call back 
*
* Description:  interface for env reverb call back
*
*/




#ifndef MENVIRONMENTALREVERBCALLBACK_H
#define MENVIRONMENTALREVERBCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from Environmental Reverb Event Observer.
*
*  @since 3.0
*/
class MEnvironmentalReverbCallback
    {
    public:

        /**
        * Invoked by the Environmental Reverb Event Observer when the Environmental Reverb object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the Environmental Reverb data
        */
        virtual void EnvironmentalReverbEvent( TDesC8& aDataPckg ) = 0;


    };



#endif      // MENVIRONMENTALREVERBCALLBACK_H

// End of File

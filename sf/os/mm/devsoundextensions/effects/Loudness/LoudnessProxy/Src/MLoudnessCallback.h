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
* Description:  Interface for loudness call back 
*
*/




#ifndef MLOUDNESSCALLBACK_H
#define MLOUDNESSCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from Loudness Event Observer.
*
*  @since 3.0
*/
class MLoudnessCallback
    {
    public:

        /**
        * Invoked by the Loudness Event Observer when the Loudness object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the loudness data
        */
        virtual void LoudnessEvent( TDesC8& aDataPckg ) = 0;
    };



#endif      // MLOUDNESSCALLBACK_H

// End of File

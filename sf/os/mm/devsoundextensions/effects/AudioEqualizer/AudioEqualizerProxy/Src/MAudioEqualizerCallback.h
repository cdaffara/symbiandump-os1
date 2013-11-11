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
* Description:  interface for audio equalizer call back 
*
*/




#ifndef MAUDIOEQUALIZERCALLBACK_H
#define MAUDIOEQUALIZERCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from equalizer Event Observer.
*
*  @since 3.0
*/
class MAudioEqualizerCallback
    {
    public:

        /**
        * Invoked by the Audio Equalizer Event Observer when the equalizer object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the equalizer data
        */
        virtual void AudioEqualizerEventL( TDesC8& aDataPckg ) = 0;


    };



#endif      // MAUDIOEQUALIZERCALLBACK_H

// End of File

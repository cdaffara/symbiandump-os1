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
* Description:  Interface class to be implemented by objects that receives callbacks from StereoWidening Event Observer.
*
*/




#ifndef MSTEREOWIDENINGCALLBACK_H
#define MSTEREOWIDENINGCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from StereoWidening Event Observer.
*
*  @since 3.0
*/
class MStereoWideningCallback
    {
    public:

        /**
        * Invoked by the Stereo Widening Event Observer when the StereoWidening object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the stereo widening data
        */
        virtual void StereoWideningEvent( TDesC8& aDataPckg ) = 0;
    };



#endif      // MSTEREOWIDENINGCALLBACK_H

// End of File

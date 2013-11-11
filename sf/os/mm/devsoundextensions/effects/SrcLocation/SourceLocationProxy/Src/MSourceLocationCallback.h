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
* Description:  Interface for source location call back 
*
*/




#ifndef SOURCELOCATIONCALLBACK_H
#define SOURCELOCATIONCALLBACK_H

//  INCLUDES

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that receives callbacks from SourceLocation Event Observer.
*
*  @since 3.0
*/
class MSourceLocationCallback
    {
    public:

        /**
        * Invoked by the SourceLocation Event Observer when the bassboost object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the bassboost data
        */
        //virtual void SourceLocationEvent( TEfSourceLocationDataPckg& aDataPckg ) = 0;
        virtual void SourceLocationEvent( TDesC8& aDataPckg ) = 0;


    };



#endif      // SOURCELOCATIONCALLBACK_H

// End of File

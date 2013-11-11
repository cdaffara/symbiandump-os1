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
* Description:   This file contains definitions of the queue item class.
*
*/




#ifndef EFFECTDATAQUEITEM_H
#define EFFECTDATAQUEITEM_H

//  INCLUDES
#include <e32base.h>


// DATA TYPES

// CLASS DECLARATION


/**
*  Class to encapsulate a queue item.
*
*  @lib SourceLocationMessageHandlerlib
*  @since 2.0
*/
class CEffectDataQueItem : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CEffectDataQueItem* NewL( TDesC8* aDataBuffer );

        /**
        * Destructor.
        */
        virtual ~CEffectDataQueItem();

        TDesC8& EffectData();

    private:

        /**
        * C++ default constructor.
        */
        CEffectDataQueItem();

       /**
        * Construct a queue item object and initialize it with
        * @param aDataBuffer Buffer containing data
        */
        void ConstructL( TDesC8* aDataBuffer );

    public:
    	// next item
        TSglQueLink* iLink;

    private:    // Data

        // Data buffer
        HBufC8* iData;

    };


#endif      // EFFECTDATAQUEITEM_H
// End of File

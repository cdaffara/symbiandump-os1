/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


#ifndef DATA_DEALER_PARAMS_H
#define DATA_DEALER_PARAMS_H

#include <e32base.h>

// Class for data dealer paramaters
class TDataDealerParams
    {
    public:
    /**
    Constructor
  	@param TBool aProducer If aProducer is set ETrue Data dealer is a producer
                            Otherwise data dealer is a consumer
    @param TUint aKey identifier
    */
    TDataDealerParams( const TBool aProducer, const TUint aKey ):
        iProducer( aProducer ), iKey( aKey )
        {};

    /**
    Either created data dealer produces or consumes data
    */
    TBool iProducer;

    /**
    Unique identificator for DataDealer
    */
    TUint iKey;
    };

#endif	// DATA_DEALER_PARAMS_H

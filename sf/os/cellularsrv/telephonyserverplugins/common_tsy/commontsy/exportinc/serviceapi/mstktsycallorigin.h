/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @publishedPartner
 @released
*/
#ifndef M_MSTKTSYCALLORIGINH_
#define M_MSTKTSYCALLORIGINH_

#include <e32base.h>

/**
*  MStkTsyCallOrigin interface
*  This interface should be implemented by an object in the SIM ATK TSY 
*  to allow the CTSY Dispatcher to query whether a number/address originates
*  from a proactive command.
*/
class MStkTsyCallOrigin
	{
public:
    
    /**
     * This function should be implemented by an object in
     * the SAT TSY to allow the CTSY Dispatcher to ascertain
     * whether a call was requested by a Proactive Command.
     *          
     * @param aAddress The number whose origin is required.
     * @return ETrue if call originates from a Proactive Command
     */
    virtual TBool IsSimOriginatedCall( const TDesC8& aAddress ) = 0;

    
	};

#endif      //  M_MSTKTSYCALLORIGINH_   
            
// End of File

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
#ifndef M_MCTSYSATSERVICEH_
#define M_MCTSYSATSERVICEH_

// INCLUDE FILES
#include <e32base.h>

// Forward declarations
class MStkTsySatService;

/**
 *  CommonTsy SAT service interface. 
 *  This interface should be implemented by the TSY object which provides 
 *  a method to allow an SMS message originating from the SAT TSY to be sent.
 *
 */
class MCtsySatService
	{
public:
    
    /**
     * Makes a request to send an SMS message.
     *          
     * @param aStkTsySatService Reference to a MStkTsySatService
     * @param aSmsTpdu Pointer to the descriptor holding PDU to be sent
     * @param aScAddress The service centre address
     * @param aMobileTON The type of service centre number
     * @param aMobileNPI The service centre numbering plan
     * @param aMoreToSend Flag indicating continuity
     * @param aTsyReqHandle The TSY request handle
     * @return Error value
     */
    virtual TInt SendSatMessage( 
        MStkTsySatService& aStkTsySatService, 
        TDes8* aSmsTpdu, 
        TDes16* aScAddress, 
        RMobilePhone::TMobileTON* aMobileTON, 
        RMobilePhone::TMobileNPI* aMobileNPI, 
        TBool aMoreToSend, 
        TTsyReqHandle aTsyReqHandle ) = 0;
	};

#endif      //  M_MCTSYSATSERVICEH_   
            
// End of File

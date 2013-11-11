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
#ifndef M_MSTKTSYSATSERVICEH_
#define M_MSTKTSYSATSERVICEH_

#include <e32base.h>

/**
*  MStkTsySatService interface
*  This interface should be implemented by an object in the SIM ATK TSY 
*  to allow the CTSY object to complete the message send. 
*/
class MStkTsySatService
	{
public:
    
    /**
     * This function should be implemented by an object in
     * the SAT TSY to allow the CTSY object which has a
     * handle to it to complete a request made by the SAT
     * TSY to send an SMS message.
     *          
     * @param aError The error code relating to the send SMS message
     * return KErrNone if no error; another error code otherwise.
     */
    virtual TInt CompleteSendSmsMessage( TInt aError ) = 0;
    
    /**
     * This function should be implemented to allow the caller to 
     * check the activation status of MO-SMS control.
     *          
     * return The status of control activated
     */
    virtual TBool IsMoSmControlBySimActivated() = 0;
    
	};

#endif      //  M_MSTKTSYSATSERVICEH_   
            
// End of File

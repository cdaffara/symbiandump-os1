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
#ifndef M_MCUSTOMVENDOREXTCOMPLETE_H_
#define M_MCUSTOMVENDOREXTCOMPLETE_H_

// INCLUDES
#include <e32base.h>
#include <et_phone.h>

/**
 * This interface provide a function which allows an object in the
 * LTSY to ask the CTSY to complete a request back to the client.
 */
class MCustomVendorExtComplete
    {
public:

    /**
     * This function should be called by a function in the LTSY
     * to complete a request back to the client.
     *          
     * @param aTsyReqHandle TSY request handle 
     * @param aError The error code the request is completing with.
     */
    virtual void ReqCompleted( const TTsyReqHandle aTsyReqHandle,
        const TInt aError ) = 0;
    };

#endif      //  M_MCUSTOMVENDOREXTCOMPLETE_H_   
            
// End of File

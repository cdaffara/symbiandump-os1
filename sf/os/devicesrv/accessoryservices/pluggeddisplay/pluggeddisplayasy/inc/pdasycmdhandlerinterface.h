/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * MPDAsyCmdHandlerInterface declaration.
 *
 */

#ifndef PDASYCMDHANDLERINTERFACE_H
#define PDASYCMDHANDLERINTERFACE_H

// SYSTEM INCLUDE FILES
#include <e32std.h>
#include <AsyCmdTypes.h>

// USER INCLUDE FILES

// FORWARD DECLARATIONS
class CAccPolObjectCon;

// DATA TYPES

// CLASS DECLARATION

/**
 * MPDAsyCmdHandlerInterface provides the needed services through
 * which the Plugged Display Engine can process command responses.
 *
 * @lib None.
 * @since S60 TB9.2
 */
NONSHARABLE_CLASS( MPDAsyCmdHandler )
    {
public:

    /**
     * Process response to boolean command.
     * 
     * @since TB9.2.
     * @param aCmdResponse Command response.
     * @return None.
     */
    virtual void ProcessResponse( const TAccValueTypeTBool aCmdResponse,
        TInt aErrCode ) = 0;

    /**
     * Process response to integer command.
     * 
     * @param aCmdResponse Response value.
     * @param aErrorCode Response error. 
     * @return None.
     */
    virtual void ProcessResponse( const TAccValueTypeTInt aCmdResponse,
        TInt aErrCode ) = 0;

    /**
     * Process response to descriptor command.
     * 
     * @param aCmdResponse Response value.
     * @param aErrorCode Response error. 
     * @return None.
     */
    virtual void ProcessResponse( const TDesC8& aCmdResponse,
        TInt aErrCode ) = 0;

    /**
     * Process response to object command.
     * 
     * @param aCmdResponse Response value.
     * @param aErrorCode Response error. 
     * @return None.
     */
    virtual void ProcessResponse( const CAccPolObjectCon& aObjectCon,
        TInt aErrCode ) = 0;

    /**
     * Get object container from proxy stream.
     * 
     * @param aObjectCon Object container where the data is streamed.
     * @return None.
     */
    virtual void ObjectConDataFromProxy(
        CAccPolObjectCon& aObjectCon ) const = 0;
    };

#endif // PDASYCMDHANDLERINTERFACE_H

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
 *
 */

#ifndef PDASYCMDHANDLER_H
#define PDASYCMDHANDLER_H

// SYSTEM INCLUDE FILES
#include <AsyCommandHandlerBase.h>

// USER INCLUDE FILES
#include "pdasycmdhandlerinterface.h"

// FORWARD DECLARATIONS
class CPDAsyMainService;

// DATA TYPES

// CLASS DECLARATION

/**
 * CPDAsyCmdHandler is responsible for handling commands sent by Acc FW.
 *
 * @lib None.
 * @since S60 TB9.2
 */
NONSHARABLE_CLASS( CPDAsyCmdHandler ):
public CASYCommandHandlerBase,
public MPDAsyCmdHandler
    {
public:

    /**
     * Symbian two phased constructors.
     *
     * @since S60 TB9.2
     * @param None.
     * @return CPDAsyCmdHandler*
     */
    static CPDAsyCmdHandler* NewL();
    static CPDAsyCmdHandler* NewLC();

    /**
     * Symbian two phased constructor.
     * 
     * NOTE: These are only used for testing purposes!
     *
     * @since S60 TB9.2
     * @param aCmdHandler Enternal response handler.
     * @param aMainServiceBase Main service.
     * @return CPDAsyCmdHandler*
     */
    static CPDAsyCmdHandler* CreateTestInstanceL(
        MPDAsyCmdHandler* aCmdHandler,
        CASYMainServiceBase* aMainServiceBase );

    /**
     * C++ destructor.
     */
    virtual ~CPDAsyCmdHandler();

protected: // From base classes
    
    // @see CASYCommandHandlerBase
    virtual void ProcessCommandL( const TProcessCmdId aCommand,
        const TASYCmdParams& aCmdParams );
    
    // @see MPDAsyCmdHandler
    virtual void ProcessResponse( const TAccValueTypeTBool aCmdResponse,
        TInt aErrCode );
    
    // @see MPDAsyCmdHandler
    virtual void ProcessResponse( const TAccValueTypeTInt aCmdResponse,
        TInt aErrCode );
    
    // @see MPDAsyCmdHandler
    virtual void ProcessResponse( const TDesC8& aCmdResponse,
        TInt aErrCode );
    
    // @see MPDAsyCmdHandler
    virtual void ProcessResponse( const CAccPolObjectCon& aObjectCon,
        TInt aErrCode );
    
    // @see MPDAsyCmdHandler
    virtual void ObjectConDataFromProxy(
        CAccPolObjectCon& aObjectCon ) const;
    
private:
    
    // Helper function to get pointer to Plugged Display ASY main service
    CPDAsyMainService* MainService();
    
private:

    CPDAsyCmdHandler();
    void ConstructL( MPDAsyCmdHandler* aCmdHandler = NULL,
        CASYMainServiceBase* aMainServiceBase = NULL );

private: // Data
    
    /** ASY command handler interface, not owned */
    MPDAsyCmdHandler* iCmdHandler;
    
    /** ASY main service, not owned */
    CASYMainServiceBase* iMainService;
    };

#endif // PDASYCMDHANDLER_H

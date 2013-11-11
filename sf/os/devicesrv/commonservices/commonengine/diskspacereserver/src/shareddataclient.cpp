/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     This module offers an interface for applications to access other
*     applications' settings which are meant to have shared access.
*
*
*/


// INCLUDE FILES
#include "shareddataclient.h"
#include "dosclishareddata.h"
#include <s32file.h>
#include "tracemacros.h"
// CONSTANTS

// CLASS DECLARATION

class CDosWrapper : public CBase
    {
    public:
        CDosWrapper() {};
        ~CDosWrapper() {};

    public: // data
        RDosServer iDosSession;
        RDosSharedData iDosSubsession;
};


// ================= MEMBER FUNCTIONS ==========================================

EXPORT_C RSharedDataClient::RSharedDataClient()
    {
    iDosWrapper = NULL;
    }

// -----------------------------------------------------------------------------
// RSharedDataClient::Connect()
// Connects to the  server - default number of message slots = 2
// -----------------------------------------------------------------------------

EXPORT_C TInt RSharedDataClient::Connect()
    {
    COMMONENGINE_RDEBUG(_L("RSharedDataClient::Connect()"));
    TInt err(KErrNone);
    
    if( !iDosWrapper )
        {
        iDosWrapper = new CDosWrapper();
        if ( !iDosWrapper )
            {
            COMMONENGINE_RDEBUG(_L("RSharedDataClient::Connect() - Could not create iDosWrapper, return KErrCouldNotConnect"));
            return KErrCouldNotConnect;
            }
        COMMONENGINE_RDEBUG(_L("RSharedDataClient::Connect() - iDosWrapper created"));
        }

    err = iDosWrapper->iDosSession.Connect();
    COMMONENGINE_RDEBUG_INT(_L("RSharedDataClient::Connect() - iDosSession.Connect() returned %d"), err);
    if ( err != KErrNone )
        {
        COMMONENGINE_RDEBUG(_L("RSharedDataClient::Connect() - Could not connect iDosSession, return KErrCouldNotConnect"));
        return KErrCouldNotConnect;
        }
    
    err = iDosWrapper->iDosSubsession.Open(iDosWrapper->iDosSession);
    COMMONENGINE_RDEBUG_INT(_L("RSharedDataClient::Connect() - iDosSubSession.Open() returned %d"), err);
    
    if ( err != KErrNone )
        {
        COMMONENGINE_RDEBUG(_L("RSharedDataClient::Connect() - Could not open iDosSubSession, return KErrCouldNotConnect"));
        return KErrCouldNotConnect;
        }
    
    COMMONENGINE_RDEBUG(_L("RSharedDataClient::Connect() - return"));
       
    return err;
    }

// -----------------------------------------------------------------------------
// RSharedDataClient::Close()
// Closes a client session.
// -----------------------------------------------------------------------------

EXPORT_C void RSharedDataClient::Close()
    {
    COMMONENGINE_RDEBUG(_L("RSharedDataClient::Close()"));
    if ( iDosWrapper )
        {
        iDosWrapper->iDosSubsession.Close();
        iDosWrapper->iDosSession.Close();
        delete iDosWrapper;
        iDosWrapper = NULL;
        }
    COMMONENGINE_RDEBUG(_L("RSharedDataClient::Close() - return"));
    }

// -----------------------------------------------------------------------------
// RSharedDataClient::RequestFreeDiskSpaceLC()
// -----------------------------------------------------------------------------

EXPORT_C void RSharedDataClient::RequestFreeDiskSpaceLC( const TInt aAmount ) 
    const
    {
    COMMONENGINE_RDEBUG_INT(_L("RSharedDataClient::RequestFreeDiskSpaceLC( %d )"), aAmount);
    CleanupStack::PushL( 
        TCleanupItem( CleanupFreeDiskSpaceRequest, 
                      CONST_CAST( RSharedDataClient*, this ) ) );
                      
    COMMONENGINE_RDEBUG_INT(_L("RSharedDataClient::RequestFreeDiskSpaceLC - calling RequestFreeDiskSpace( %d )"), 
                                aAmount);
    RequestFreeDiskSpace( aAmount );
    COMMONENGINE_RDEBUG(_L("RSharedDataClient::RequestFreeDiskSpaceLC() - return"));
    }

// -----------------------------------------------------------------------------
// RSharedDataClient::RequestFreeDiskSpace()
// -----------------------------------------------------------------------------

EXPORT_C void RSharedDataClient::RequestFreeDiskSpace( const TInt aAmount ) 
    const
    {
    COMMONENGINE_RDEBUG_INT(_L("RSharedDataClient::RequestFreeDiskSpace( %d )"), aAmount);
    TInt amount(aAmount);
    if ( iDosWrapper )
        {
        iDosWrapper->iDosSubsession.RequestFreeDiskSpace( amount ); //Use the subsession.
        COMMONENGINE_RDEBUG(_L("RSharedDataClient::RequestFreeDiskSpace() - return"));
        }
    else
        {
        COMMON_RDEBUG(_L("RSharedDataClient::RequestFreeDiskSpace() - Not connected to the  server, return!"));
        }
    }

// -----------------------------------------------------------------------------
// RSharedDataClient::CancelFreeDiskSpaceRequest()
// -----------------------------------------------------------------------------

EXPORT_C void RSharedDataClient::CancelFreeDiskSpaceRequest() const
    {
    COMMONENGINE_RDEBUG(_L("RSharedDataClient::CancelFreeDiskSpaceRequest()"));
    if ( iDosWrapper )
        {
        iDosWrapper->iDosSubsession.RequestFreeDiskSpaceCancel(); //Use the subsession.    
        COMMONENGINE_RDEBUG(_L("RSharedDataClient::CancelFreeDiskSpaceRequest() - return"));
        }
    else
        {
        COMMON_RDEBUG(_L("RSharedDataClient::RequestFreeDiskSpace() - Not connected to the  server, return!"));
        }
    }
    
// -----------------------------------------------------------------------------
// RSharedDataClient::CleanupFreeDiskSpaceRequest()
// -----------------------------------------------------------------------------

void RSharedDataClient::CleanupFreeDiskSpaceRequest( TAny* aAny )
    {
    COMMONENGINE_RDEBUG(_L("RSharedDataClient::CleanupFreeDiskSpaceRequest()"));
    REINTERPRET_CAST( RSharedDataClient*, aAny )->CancelFreeDiskSpaceRequest();
    COMMONENGINE_RDEBUG(_L("RSharedDataClient::CleanupFreeDiskSpaceRequest() - return"));
    }


// End of File

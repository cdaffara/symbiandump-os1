/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics-Out-Of-Memory plugin for Font and Bitmap server.
*
*/

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <graphics/fbsoogmmessage.h>
#include <w32std.h>
#include <FBS.H>

#include "fbsoogmplugin.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "fbsoogmpluginTraces.h"
#endif


/**
 Called by the GOoM monitor framework in the case of low GPU memory.

 Note: In the case of failure, this framework method will do so silently.

 @param aBytesToFree The amount of GPU memory to be freed.
 */
void CFbsOogmMonitorPlugin::FreeRam( TInt aBytesToFree, TInt aFlags )
    {
    OstTrace0( TRACE_NORMAL, CFBSOOGMMONITORPLUGIN_FREERAM, "> " );

    if ( (NULL == RFbsSession::GetSession()) )
         {
         if ( KErrNone != RFbsSession::Connect() )
             {
             OstTrace0( TRACE_NORMAL, DUP2_CFBSOOGMMONITORPLUGIN_FREERAM, "< No FbsSession" );

             return;
             }

         }

    TFbsOogmMessage oogmMessage( TFbsOogmMessage::EFbsOogmLowNotification, aBytesToFree, aFlags );
    (void)RFbsSession::GetSession()->ConveyOogmMessage( oogmMessage );

    OstTrace0( TRACE_NORMAL, DUP1_CFBSOOGMMONITORPLUGIN_FREERAM, "< " );
    }



/**
 Called by the GOoM monitor framework in the case of normal GPU memory-usage being permissible.

 Note: In the case of failure, this framework method will do so silently.
 */
void CFbsOogmMonitorPlugin::MemoryGood( TInt aFlags )
    {
    OstTrace0( TRACE_NORMAL, CFBSOOGMMONITORPLUGIN_MEMORYGOOD, ">" );

    if ( (NULL == RFbsSession::GetSession()) )
         {
        if ( KErrNone != RFbsSession::Connect() )
            {
            OstTrace0( TRACE_NORMAL, DUP2_CFBSOOGMMONITORPLUGIN_MEMORYGOOD, "< No FbsSession" );

            return;
            }

         }

    const TInt KOogmDummyIntegerArgument = 0;
    TFbsOogmMessage oogmMessage( TFbsOogmMessage::EFbsOogmOkayNotification, KOogmDummyIntegerArgument, aFlags );
    (void)RFbsSession::GetSession()->ConveyOogmMessage( oogmMessage );

	OstTrace0( TRACE_NORMAL, DUP1_CFBSOOGMMONITORPLUGIN_MEMORYGOOD, "<" );
    }



/**

 */
CFbsOogmMonitorPlugin::CFbsOogmMonitorPlugin()
    {
    }



/**

 */
CFbsOogmMonitorPlugin::~CFbsOogmMonitorPlugin()
    {
    RFbsSession::Disconnect();
    }



/**

 */
void CFbsOogmMonitorPlugin::ConstructL()
    {
    CGOomMonitorPluginBase::ConstructL();
    }



/**

 */
TAny* CreatePlugin()
    {
    CFbsOogmMonitorPlugin* self = new CFbsOogmMonitorPlugin;
    if ( self )
        {
        TRAPD( err, self->ConstructL() );
        if ( err != KErrNone )
            {
            delete self;
            self = NULL;
            }
        }

    return self;
    }



/**

 */
const TImplementationProxy ImplementationTable[] =
    {
    { {0x2002B638}, ::CreatePlugin }
    };



/**

 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    OstTrace0( TRACE_NORMAL, _IMPLEMENTATIONGROUPPROXY, "> ::ImplementationGroupProxy" );
    
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy ) ;
    return ImplementationTable;
    }




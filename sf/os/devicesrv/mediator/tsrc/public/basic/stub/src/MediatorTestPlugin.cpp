/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Source file for Mediator Test plugin
*
*/


// INCLUDE FILES
#include <e32def.h>
#include <w32std.h>
#include <e32property.h>

#include "MediatorTestPlugin.h"
#include "debug.h"
#include "mediatortestdata.h"


// CONSTANTS

const TInt KDefaultTimeOut = 3000; // 3 seconds
const TInt KDefaultUid = 0x999101;
const TInt KDefaultId = 1;
// -----------------------------------------------------------------------------
// CUPnPAVControlImplementation::NewL
// Creates an instance of the implementation.
// -----------------------------------------------------------------------------
CMediatorTestPlugin* CMediatorTestPlugin::NewL()
	{
    LOG(_L("CMediatorTestPlugin::NewL()"));
	CMediatorTestPlugin* self = new (ELeave) CMediatorTestPlugin;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMediatorTestPlugin::CMediatorTestPlugin
// First phase construction.
// -----------------------------------------------------------------------------
CMediatorTestPlugin::CMediatorTestPlugin()
	{
    // Nothing to do here
	}

// -----------------------------------------------------------------------------
// CMediatorTestPlugin::ConstructL
// Second phase construction.
// -----------------------------------------------------------------------------
void CMediatorTestPlugin::ConstructL()
	{
    LOG(_L("CMediatorTestPlugin::ConstructL()"));
    
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CMediatorTestPlugin::~CMediatorTestPlugin()
	{
    if ( iMediatorNotifications )
        {
        iMediatorNotifications->UnregisterNotificationObserver();
        }
    delete iMediatorNotifications;
    delete iEventConsumer;
	}
	
// -----------------------------------------------------------------------------
// StartL.
// -----------------------------------------------------------------------------
void CMediatorTestPlugin::StartL()
	{
    LOG(_L("CMediatorTestPlugin::StartL()"));
    
    // LEAVE TEST
    //User::Leave( KErrNotFound );
    
    iMediatorNotifications = CMediatorNotifications::NewL();
    iMediatorNotifications->RegisterNotificationObserver( this );
    iEventConsumer = CMediatorEventConsumer::NewL( this );
    iCommandResponder = CMediatorCommandResponder::NewL( this );
    
    RCommandList commandList;
    TInt status(KErrNone);
    TUid myUid = TUid::Uid(KDefaultUid);
    TCapabilitySet caps;
    caps.SetEmpty();
    TVersion version( 1,0,0);

    status = iCommandResponder->RegisterCommand( myUid, 
    	                                         myUid, 
                                                 KDefaultId, 
                                                 version,
                                                 caps, 
                                                 KDefaultTimeOut );
	}
	
// -----------------------------------------------------------------------------
// StartL.
// -----------------------------------------------------------------------------
void CMediatorTestPlugin::MediatorEventL( TUid aDomain,
                             TUid aCategory, 
                             TInt aEventId, 
                             const TDesC8& aData )
    {
    RDebug::Print( _L("CMediatorTestPlugin::Event %d received !!!\n"), aEventId );
    if ( aData.Length() >= 256 )
        {
        return;
        }
    HBufC* newBuffer = ToUnicodeL( aData );
	TBuf<256> text( *newBuffer );
	delete newBuffer; newBuffer = NULL;
    RDebug::Print( _L("CMediatorTestPlugin::Data: %S\n"), &text );
    }

void WriteTestListResponseL( TUid aDomain, TUid aCategory, TInt aFirstItemId, TInt aLastItemId )
    {
    TInt currentListId( KErrNotSupported );
    
    TInt err = RProperty::Get( KMediatorStifTestPsKeys, KCurrentTestList, currentListId );
    
    if ( currentListId == KErrNotSupported || err != KErrNone )
        {
        RDebug::Print( _L("CMediatorTestPlugin::Test list not supported\n") );
        return;
        }
   
    // first read currently stored data, if any, as test can contain several phases were items are added
    TTestItemListSetPckgBuf eventsPckgBuf;
    eventsPckgBuf().iCount = 0;
    
    User::LeaveIfError( RProperty::Get( KMediatorStifTestPsKeys, KReceivedTestLists, eventsPckgBuf ) );
    
    TInt count =  eventsPckgBuf().iCount;
    
    __ASSERT_ALWAYS( eventsPckgBuf().iCount < KMaxCountOfLists, User::Invariant() ); // if asserts here, modify tests or increase the max count
    // add the new received the list and update to P&S
    
    TTestItemList* newList = &(eventsPckgBuf().iTestLists[ eventsPckgBuf().iCount ]);
    eventsPckgBuf().iCount++;
        
    newList->iDomain = aDomain;
    newList->iCategory = aCategory;
    newList->iFirst = aFirstItemId;
    newList->iLast = aLastItemId;
    
    User::LeaveIfError( RProperty::Set( KMediatorStifTestPsKeys, KReceivedTestLists, eventsPckgBuf ) );
    
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorTestPlugin::MediatorEventsAddedL( TUid aDomain, 
                                   TUid aCategory, 
                                   const REventList& aEvents )
    {
    RDebug::Print( _L("CMediatorTestPlugin::MediatorEventsAddedL:Domain 0x%x, category 0x%x, count=%d\n"), 
                      aDomain.iUid, aCategory.iUid, aEvents.Count() );
    
    if ( aEvents.Count() )
        {
        WriteTestListResponseL( aDomain, 
                                aCategory, 
                                aEvents[0].iEventId,
                                aEvents[aEvents.Count()-1].iEventId );
        }
    
    
    TInt status = iEventConsumer->SubscribeEvent( aDomain, aCategory, aEvents );
    RDebug::Print( _L("CMediatorTestPlugin::MediatorEventsAddedL: events subscribed with status %d ###\n"), status ); 
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorTestPlugin::MediatorCommandsAddedL( TUid aDomain, 
                                     TUid aCategory,
                                     const RCommandList& aCommands )
    {
    RDebug::Print( _L("CMediatorTestPlugin::MediatorCommandsAddedL:Domain %d, category %d, count=%d\n"), 
                      aDomain.iUid, aCategory.iUid, aCommands.Count() );
    
    if ( aCommands.Count() )
        {
        WriteTestListResponseL( aDomain, 
                                aCategory, 
                                aCommands[0].iCommandId,
                                aCommands[aCommands.Count()-1].iCommandId );
        }
    }
 
// -----------------------------------------------------------------------------
// ?classname::?member_function
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorTestPlugin::MediatorCategoryRemovedL( TUid aDomain, TUid aCategory )
    {
    RDebug::Print( _L("CMediatorTestPlugin::MediatorCategoryRemovedL: Category removed\n"));
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorTestPlugin::MediatorEventsRemovedL( TUid aDomain, 
                                     TUid aCategory,
                                     const REventList& aEvents )
    {
    RDebug::Print( _L("CMediatorTestPlugin::MediatorEventsRemovedL: %d new events removed ###\n"), aEvents.Count() );
    RDebug::Print( _L("CMediatorTestPlugin::MediatorEventsRemovedL: Domain %d, category %d\n"), aDomain.iUid, aCategory.iUid );
    }
                                     
// -----------------------------------------------------------------------------
// ?classname::?member_function
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorTestPlugin::MediatorCommandsRemovedL( TUid aDomain, 
                                       TUid aCategory,
                                       const RCommandList& aCommands )
    {
    RDebug::Print( _L("CMediatorTestPlugin::MediatorCommandsRemovedL: %d commands removed ###\n"), aCommands.Count() );
    RDebug::Print( _L("CMediatorTestPlugin::MediatorCommandsRemovedL: Domain %d, category %d\n"), aDomain.iUid, aCategory.iUid );
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC16* CMediatorTestPlugin::ToUnicodeL(const TDesC8& aBuffer)
		{
		TInt length = aBuffer.Length();
		HBufC16* str = HBufC16::NewL(length);

        str->Des().Copy( aBuffer );
        
        return str;
		}


// -----------------------------------------------------------------------------
// ?classname::?member_function
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorTestPlugin::MediatorCommandL( TUid aDomain,
                                            TUid aCategory, 
                                            TInt aCommandId,
                                            TVersion aVersion, 
                                            const TDesC8& aData )
    {

    RDebug::Print( _L("CMediatorTestPlugin::MediatorCommandL: command id:%d received ###\n"), aCommandId);
    RDebug::Print( _L("CMediatorTestPlugin::MediatorCommandL: Domain %d, category %d\n"), aDomain.iUid, aCategory.iUid );

    _LIT8(KTestR, "Response data here");
    HBufC8* replyData = KTestR().AllocLC();
    TInt status = iCommandResponder->IssueResponse( aDomain, 
                                               aCategory, 
                                               aCommandId, 
                                               KErrNone,
                                               *replyData );       

	CleanupStack::PopAndDestroy( replyData );
	
		//iDomain = aDomain;
        //iCategory = aCategory; 
        //iCommandId = aCommandId;			

        //_LIT8(KTest, "Command response here");
    	//HBufC8* data = KTest().AllocLC();
        //TInt status = iCommandResponder->IssueResponse( aDomain, 
        //                                  aCategory, 
        //                                  aCommandId, 
        //                                  KErrNone,
        //                                  *data );                                      
        //CleanupStack::PopAndDestroy( data );        

    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorTestPlugin::CancelMediatorCommand( TUid aDomain,
                                                    TUid aCategory, 
                                                    TInt aCommandId )
    {
		
    }  

// End of File

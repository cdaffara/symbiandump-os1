/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   DRM Play Session
*
*/



#include "DRMCustomCommandAsyncAO.h"
#include "DRMPlaySession.h"

#ifdef _DEBUG
#include <e32debug.h>

#define TRACEPRNFNIN RDebug::Printf( "%s @ %d, %s>ENTER", __FILE__, __LINE__,  __PRETTY_FUNCTION__ );
#define TRACEPRNFNOUT RDebug::Printf( "%s @ %d, %s>EXIT", __FILE__, __LINE__,  __PRETTY_FUNCTION__ );
#define DEBPRN2(s, v1) RDebug::Print(s, v1);
#define DEBPRN3(s, v1, v2) RDebug::Print(s, v1, v2);
#else
#define TRACEPRNFNIN
#define TRACEPRNFNOUT
#define DEBPRN2(s, v1)
#define DEBPRN3(s, v1, v2)
#endif


CDRMCustomCommandAsyncAO::CDRMCustomCommandAsyncAO( TCustomCommandType aType, CDRMPlayServerSession& aParent )
: CActive(CActive::EPriorityStandard),
 iCustomCommandType(aType),
 iAmCompleted(EFalse),
 iParent(aParent),
 iData1FromClient(NULL),
 iData2FromClient(NULL),
 iDataToClient(NULL)
    {
    TRACEPRNFNIN;
    CActiveScheduler::Add(this);
    TRACEPRNFNOUT;
    }

CDRMCustomCommandAsyncAO::~CDRMCustomCommandAsyncAO()
    {
    TRACEPRNFNIN;
    Cancel();
    if ( !iAmCompleted && iMessage)
        {
        iMessage->Complete( KErrCancel );
        }

    // Delete the descriptors created on heap
    delete iData1FromClient;
    delete iData2FromClient;
    delete iDataToClient;

    delete iMessage;

    TRACEPRNFNOUT;
    }

CDRMCustomCommandAsyncAO* CDRMCustomCommandAsyncAO::NewL( const RMessage2& aMessage,
                                                          TCustomCommandType aType,
                                                          CDRMPlayServerSession& aParent )
    {
    TRACEPRNFNIN;
    CDRMCustomCommandAsyncAO* self = new ( ELeave )CDRMCustomCommandAsyncAO( aType, aParent );
    CleanupStack::PushL( self );
    self->ConstructL(aMessage);
    CleanupStack::Pop( self );
    TRACEPRNFNOUT;
    return self;
    }

void CDRMCustomCommandAsyncAO::ConstructL( const RMessage2& aMessage )
    {
    TRACEPRNFNIN;

    //Read mmf message destination information
    TPckgCustomCommand thePckg;
    User::LeaveIfError( aMessage.Read(0, thePckg) );

    iMMFMessageDestinationPckg = thePckg().iDestination;
    iMMFMessageFunction = thePckg().iFunction;

/*
    RDebug::Print(_L("CDRMCustomCommandAsyncAO::ConstructL:InterfaceId[%x]DestinationHandle[%d]"), \
                                iMMFMessageDestinationPckg().InterfaceId(),
                                iMMFMessageDestinationPckg().DestinationHandle() );
*/

    // Create descriptors to hold data1, data2

    // Get Data 1 from client
    TInt dataLen = aMessage.GetDesLengthL( 1 );
    iData1FromClient = HBufC8::NewL( dataLen );
    TPtr8 data1Ptr = iData1FromClient->Des();
    aMessage.ReadL( 1, data1Ptr );

    // Get Data 2 from client
    dataLen = aMessage.GetDesLengthL( 2 );
    iData2FromClient = HBufC8::NewL( dataLen );
    TPtr8 data2Ptr = iData2FromClient->Des();
    aMessage.ReadL( 2, data2Ptr );

    TRACEPRNFNOUT;
    }

TMMFMessageDestinationPckg& CDRMCustomCommandAsyncAO::GetMMFMessageDestinationPckg()
    {
    return iMMFMessageDestinationPckg;
    }

TInt CDRMCustomCommandAsyncAO::GetMMFMessageFunction()
    {
    return iMMFMessageFunction;
    }

HBufC8* CDRMCustomCommandAsyncAO::GetData1FromClient()
    {
    return iData1FromClient;
    }

HBufC8* CDRMCustomCommandAsyncAO::GetData2FromClient()
    {
    return iData2FromClient;
    }

HBufC8* CDRMCustomCommandAsyncAO::GetDataToClient()
    {
    return iDataToClient;
    }

TInt CDRMCustomCommandAsyncAO::SetActive()
    {
    TRACEPRNFNIN;
    TInt status(KErrNotReady);
    if ( !IsActive() && iMessage )
        {
        CActive::SetActive();
        status = KErrNone;
        }
    TRACEPRNFNOUT;
    return status;
    }

void CDRMCustomCommandAsyncAO::TransferOwnershipL( const RMessage2& aMessage )
    {
    // If this is a async custom command with result back to client
    // Descriptor place holder is passed in the async message 'aMessage'.
    if ( iCustomCommandType == ECustomCommandWithResult )
        {
        // Create buffer to hold the result back
        TInt dataLen = aMessage.GetDesMaxLengthL( 0 );
        iDataToClient = HBufC8::NewL( dataLen );
        }

    iMessage = new (ELeave) RMessage2(aMessage);
    }

void CDRMCustomCommandAsyncAO::RunL()
    {
    TRACEPRNFNIN;
    TInt status(KErrNone);
    // Write the result back if client is expecting one
    TPtr8 dataPtr = iDataToClient->Des();
    if ( ( iCustomCommandType == ECustomCommandWithResult ) &&
         (dataPtr.Length() > 0 ) )
        {
        status = iMessage->Write( 0, dataPtr );
#ifdef _DEBUG
        if ( status != KErrNone )
            {
            RDebug::Print(_L("CDRMCustomCommandAsyncAO::RunL:iMessage.Write()->Len[%d]MaxLen[%d]Status[%d]"),
                            dataPtr.Length(),
                            dataPtr.MaxLength(),
                            status);
            }
#endif // _DEBUG
        }
    // Complete the message
    status = iStatus.Int();
    DEBPRN2(_L("CDRMCustomCommandAsyncAO::RunL[%d]"), status );
    iMessage->Complete( status );
    iAmCompleted = ETrue;

    // Signal the parent
    iParent.AsyncCustomCommandCompleted( this );

    TRACEPRNFNOUT;
    }

void CDRMCustomCommandAsyncAO::DoCancel()
    {
    TRACEPRNFNIN;
    iMessage->Complete( KErrCancel );
    iAmCompleted = ETrue;
    TRACEPRNFNOUT;
    }

TInt CDRMCustomCommandAsyncAO::RunError( TInt /*aError*/ )
    {
    TRACEPRNFNIN;
    // There was some error completing message....
    TRACEPRNFNOUT;
    return KErrNone;
    }

// End of File

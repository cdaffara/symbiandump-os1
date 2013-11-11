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
* Description:   Message handler for AddedDevSoundControl CI.
*
*/



// INCLUDE FILES
#include "AddedDevSoundControlCI.h"
#include "AddedDevSoundControlMsgHdlr.h"
#include "AddedDevSoundControlMsgs.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

/**
 * CAddedDevSoundControlMsgHdlr::CAddedDevSoundControlMsgHdlr
 * C++ default constructor can NOT contain any code, that might leave.
 */
CAddedDevSoundControlMsgHdlr::CAddedDevSoundControlMsgHdlr(
                              MAddedDevSoundControl* aAddedDSControlCI) :
	CMMFObject(KUidAddedDevSoundControlInterface)
    {
    iAddedDSControlCI = aAddedDSControlCI;
    }

/**
 * CAddedDevSoundControlMsgHdlr::ConstructL
 * Symbian 2nd phase constructor can leave.
 */
void CAddedDevSoundControlMsgHdlr::ConstructL()
    {
    }

/**
 * CAddedDevSoundControlMsgHdlr::NewL
 * Two-phased constructor.
 */
EXPORT_C CAddedDevSoundControlMsgHdlr*
CAddedDevSoundControlMsgHdlr::NewL(TAny* aAddedDSControlCI)
    {
    MAddedDevSoundControl* addedDevSoundControlCI =
            (MAddedDevSoundControl*)aAddedDSControlCI;
    CAddedDevSoundControlMsgHdlr* self =
            new (ELeave) CAddedDevSoundControlMsgHdlr(addedDevSoundControlCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

/**
 * Destructor
 */
CAddedDevSoundControlMsgHdlr::~CAddedDevSoundControlMsgHdlr()
	{
	delete iAddedDSControlCI;
	}

/**
 * CAddedDevSoundControlMsgHdlr::HandleRequest
 * Handles messages from the proxy.
 * Calls a subfunction, which determines what custom interface to call.
 * A subfunction is used to contain multiple leaving functions for a single
 * trap.
 * (other items were commented in a header).
 */
void CAddedDevSoundControlMsgHdlr::HandleRequest(TMMFMessage& aMessage)
	{
	ASSERT(
	aMessage.Destination().InterfaceId() == KUidAddedDevSoundControlInterface);

	TRAPD(error, DoHandleRequestL(aMessage));
	if (error)
		{
		aMessage.Complete(error);
		}
	}

/**
 * CAddedDevSoundControlMsgHdlr::DoHandleRequestL
 * Determines which custom interface to call.
 * (other items were commented in a header).
 */
void CAddedDevSoundControlMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EAddedDSControlSetHwAwareness:
			{
    		DoSetHwAwarenessL(aMessage);
			break;
			}
		case EAddedDSControlPauseAndFlush:
			{
    		DoPauseAndFlushL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

/**
 * CAddedDevSoundControlMsgHdlr::DoSetHwAwarenessL
 * Handles EAddedDSControlSetHwAwareness message from the proxy and calls 
 * custom interface method.
 *
 * (other items were commented in a header).
 */
void CAddedDevSoundControlMsgHdlr::DoSetHwAwarenessL(TMMFMessage& aMessage)
    {
	TPckgBuf<TBool> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt status = iAddedDSControlCI->SetHwAwareness(pckg());
    aMessage.Complete(status);
    }

/**
 * CAddedDevSoundControlMsgHdlr::DoPauseAndFlushL
 * Handles EAddedDSControlPauseAndFlush message from the proxy and calls 
 * custom interface method.
 *
 * (other items were commented in a header).
 */
void CAddedDevSoundControlMsgHdlr::DoPauseAndFlushL(TMMFMessage& aMessage)
    {
	TInt status = iAddedDSControlCI->PauseAndFlush();
    aMessage.Complete(status);
    }

// End of File

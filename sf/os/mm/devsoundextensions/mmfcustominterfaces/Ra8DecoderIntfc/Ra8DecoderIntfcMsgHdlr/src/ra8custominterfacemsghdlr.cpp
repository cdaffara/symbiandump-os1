/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Message handler for RA8 decoder custom interface.
*
*/



// INCLUDE FILES
#include    <Ra8CustomInterface.h>
#include    "Ra8CustomInterfaceMsgHdlr.h"
#include    "Ra8CustomInterfaceMsgs.h"

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

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceMsgHdlr::CRa8CustomInterfaceMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRa8CustomInterfaceMsgHdlr::CRa8CustomInterfaceMsgHdlr(
                            CRa8CustomInterface* aRa8CustomInterface) :
	CMMFObject(KUidRa8DecHwDeviceCI)
    {
    iRa8CustomInterface = aRa8CustomInterface;
    }

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRa8CustomInterfaceMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRa8CustomInterfaceMsgHdlr*
    CRa8CustomInterfaceMsgHdlr::NewL(TAny*  aRa8CustomInterface)
    {
    CRa8CustomInterface* custominterface = 
                         (CRa8CustomInterface*)aRa8CustomInterface;
    CRa8CustomInterfaceMsgHdlr* self = 
            new (ELeave) CRa8CustomInterfaceMsgHdlr(custominterface);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CRa8CustomInterfaceMsgHdlr::~CRa8CustomInterfaceMsgHdlr()
	{
	}

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRa8CustomInterfaceMsgHdlr::HandleRequest(TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidRa8DecHwDeviceCI);
	TRAPD(error,DoHandleRequestL(aMessage));
	if (error)
		{
		aMessage.Complete(error);
		}
	}

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRa8CustomInterfaceMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch (aMessage.Function())
		{
		case ERa8CiFrameNumber:
			{
    		DoFrameNumberL(aMessage);
			break;
			}
		case ERa8CiSetInitString:
			{
    		DoSetInitStringL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceMsgHdlr::DoFrameNumberL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRa8CustomInterfaceMsgHdlr::DoFrameNumberL(TMMFMessage& aMessage)
    {
	TInt frame = iRa8CustomInterface->FrameNumber();
	TPckgBuf<TInt> pckg(frame);
	aMessage.WriteDataToClientL(pckg);
    aMessage.Complete(KErrNone);
    }
    
// -----------------------------------------------------------------------------
// CRa8CustomInterfaceMsgHdlr::DoSetInitStringL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRa8CustomInterfaceMsgHdlr::DoSetInitStringL(TMMFMessage& aMessage)
    {
	TInt size = aMessage.SizeOfData1FromClient();
	HBufC8* dataBuf = HBufC8::NewL(size);
	CleanupStack::PushL( dataBuf );
	TPtr8 dataPtr = dataBuf->Des();
    aMessage.ReadData1FromClientL(dataPtr);
	TInt status = iRa8CustomInterface->SetInitString(dataPtr);
	CleanupStack::Pop(dataBuf);
	delete dataBuf;
    aMessage.Complete(status);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File

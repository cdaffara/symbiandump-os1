// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the active object that controls the Read() requests.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ReceiverTraces.h"
#endif

#include "Receiver.h"
#include "Constants.h"
#include <es_ini.h>

const TUint KBufferIncreaseStep=500;
const TUint K64k=65535;

CReceiver::CReceiver(CBcaIoController& aObserver, TUint aMaxPacketSize)
/**
 * Constructor. Performs standard active object initialisation.
 *
 * @param aObserver Reference to the observer of this state machine
 */
	: CActive(EPriorityHigh), 
	  iObserver(aObserver), 
	  iMaxPacketSize(aMaxPacketSize)
	{	
	CActiveScheduler::Add(this);
	}

CReceiver* CReceiver::NewL(CBcaIoController& aObserver, TUint aMaxPacketSize)
/**
 * Two-phase constructor. Creates a new CBcaIoController object, performs 
 * second-phase construction, then returns it.
 *
 * @param aObserver The observer, to which events will be reported
 * @return A newly constructed CBcaIoController object
 */
	{
	CReceiver* self = new (ELeave) CReceiver(aObserver, aMaxPacketSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CReceiver::ConstructL()
/**
 * Second-phase constructor. Creates all the state objects it owns.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_CONSTRUCTL_1, "CReceiver::ConstructL");
	iData.CreateL(iMaxPacketSize);
	}

CReceiver::~CReceiver()
/**
 * Destructor.
 */
    {
    Cancel();
    // iData is a shared bit of memory between raw ip and bca
    // you cannot delete it while bca might be using it otherwise
    // bad things may happen.
    iData.Close();
    }

void CReceiver::RunL()
/**
 *  Method called when read request completes. This will only be once the BCA has a full IP 
 *  packet in its buffer.
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_RUNL_1, "CReceiver::RunL [iStatus=%d]", iStatus.Int());

	if (iStatus != KErrNone)
		{
		if (iStatus == KErrNoMemory)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_RUNL_2, "WARNING! CReceiver: Read failed with KErrNoMemory. Increase buffer.");
				// Read operation failed!! Nif will re-issue the read request. Increase buffer.			
			if ((iMaxPacketSize + KBufferIncreaseStep) > K64k)
			    {
			    // In theory IP packet can't be bigger than 64k, so if we come here something is wrong so stop observer. 
                iObserver.Stop(KErrNoMemory);
			    }
			else
			    {
                iMaxPacketSize += KBufferIncreaseStep;
                TInt err = iData.ReAlloc(iMaxPacketSize);
                if (KErrNoMemory == err)
                    {                
                    iObserver.Stop(KErrNoMemory);
                    }
                else
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_RUNL_3, "Read request after increasing size of read buffer");
                    (iObserver.Bca())->Read(iStatus, iData);    
                    SetActive();
                    }
			    }
			}
		else 
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_RUNL_4, "WARNING! CReceiver: Read failed");
			iObserver.Stop(iStatus.Int());
			}
		return;
		}
	else
	    {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_RUNL_5, "CReceiver: Data Packet Received");
    
        iRMBufPacket.CreateL(iData);
        
        // Immediately execute new read request.
        (iObserver.Bca())->Read(iStatus, iData);
    
        SetActive();
        
        iRMBufPacket.Pack();
        
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
        TUint16 protocolCode = iObserver.RemoveHeader(iRMBufPacket);
#else
        TUint16 protocolCode = 0;
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
    
        // Process the packet
        iObserver.GetObserver().Process(iRMBufPacket, protocolCode);
        iRMBufPacket.Free();
	    }
	}

TInt CReceiver::RunError(TInt aError)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_RUNERROR_1, "WARNING! CReceiver::RunError Read failed");
    iObserver.Stop(aError); 
    return KErrNone;  
    }

void CReceiver::DoCancel()
/**
 *	Cancel active request
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_DOCANCEL_1, "CReceiver::DoCancel");

	(iObserver.Bca())->CancelRead(); 
	}

void CReceiver::StartListening()
/**
 *  This method issues a Read request. This is the API used to receive 
 *  packets from BCA.  
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVER_STARTLISTENING_1, "CReceiver::StartListening");

	// The BCA will complete this read once it has a full IP packet in its buffer.
	(iObserver.Bca())->Read(iStatus, iData);

	SetActive();
	}

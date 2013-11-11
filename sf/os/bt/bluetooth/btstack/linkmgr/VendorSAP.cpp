// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "VendorSAP.h"
#include "vendorspecific.h"
#include "linkmgr.h"
#include "hcifacade.h"
#include "BTSec.h"


//Diagnostic string for security check failures, in builds without platsec
//diagnostics this will be NULL.
const char* const KVENDOR_SAP_NAME_DIAG = __PLATSEC_DIAGNOSTIC_STRING("Vendor SAP");

// only does IOCTLs

CVendorSAP* CVendorSAP::NewL(CLinkMgrProtocol& aProtocol)
	{
	return new(ELeave) CVendorSAP(aProtocol);
	}

CVendorSAP::CVendorSAP(CLinkMgrProtocol& aProtocol)
: iDebugEventQueue(_FOFF(TVendorDebugEvent, iEventLink)),
  iProtocol(aProtocol)
	{
	iProtocol.iVendorSpecificSAP = this;
	}

CVendorSAP::~CVendorSAP()
	{
	iProtocol.iVendorSpecificSAP = NULL;
	
	//need to empty the queue.
	while (!(iDebugEventQueue.IsEmpty()))
		{
		TVendorDebugEvent* theEvent = iDebugEventQueue.First();
		theEvent->iEventLink.Deque();
		delete theEvent;
		}
	}

void CVendorSAP::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
/**
**/
	{
	__ASSERT_DEBUG(iIoctlLevel == 0, Panic(EVendorTwoIoctls));
	__ASSERT_DEBUG(iIoctlName == 0, Panic(EVendorTwoIoctls));
	
	iIoctlLevel = aLevel;
	iIoctlName = aName;
	
	//Do security check on following Ioctls
	TInt rerr = KErrNone;
	switch(aName)
		{
		case KHCIWaitForVendorSpecificDebugEventIoctl:
		case KHCIWriteVendorSpecificFrameIoctl:
		case KHCIWriteVendorSpecificFrameNoEventExpectedIoctl:
			{
			//The above Ioctl require additional security checking, NetworkControl
			//in addition to the LocalServices that was required to create the SAP
			__ASSERT_DEBUG(iSecurityChecker, User::Panic(KSECURITY_PANIC, EBTPanicNullSecurityChecker));
			rerr = iSecurityChecker->CheckPolicy(KNETWORK_CONTROL, KVENDOR_SAP_NAME_DIAG);
			if (rerr != KErrNone)
				{
				NotifyIoctlComplete(rerr,
									KSolBtHCI,
									aName,
									NULL);
				}								
			break;
			}
		default:
		//	Other Ioctls require just LocalServices - effectively policed when BT socket is created/transferred
		break;	
		}
	if (rerr != KErrNone)
		{
		return;				
		}
	
	// have passed security check so continue to handle ioctl		
	switch(aName)
		{
		case KHCIWaitForVendorSpecificDebugEventIoctl:
			{

			/*
			If VendorSpecificWaitForDebugEvent ioctl 
			See if we can complete it synchronously
			*/
			TryToCompleteVendorDebugEventIoctl();	
			break;
			}
		case KHCIWriteVendorSpecificFrameIoctl:
			{
			if(aOption->Length() != sizeof(THCIWriteVendorSpecificFrameIoctl))
				{
				NotifyIoctlComplete(KErrBadDescriptor,
						KSolBtHCI,
						KHCIWriteVendorSpecificFrameIoctl,
						NULL);
				return;
				}
			const THCIWriteVendorSpecificFrameIoctl* ioctl =
				reinterpret_cast<const THCIWriteVendorSpecificFrameIoctl*>(aOption->Ptr());
			
			TRAPD(err,iProtocol.HCIFacade().VendorSpecificCommandL(ioctl->iOpcode, ioctl->iParams));
			if (err)
				{
				NotifyIoctlComplete(err,
									KSolBtHCI,
									KHCIWriteVendorSpecificFrameIoctl,
									NULL);
				}
			// if no error wait for event before completing

			break;
			}
		case KHCIWriteVendorSpecificFrameNoEventExpectedIoctl:
			{
			if(aOption->Length() != sizeof(THCIWriteVendorSpecificFrameIoctl))
				{
				NotifyIoctlComplete(KErrBadDescriptor,
						KSolBtHCI,
						KHCIWriteVendorSpecificFrameNoEventExpectedIoctl,
						NULL);
				return;
				}
			const THCIWriteVendorSpecificFrameIoctl* ioctl =
				reinterpret_cast<const THCIWriteVendorSpecificFrameIoctl*>(aOption->Ptr());
			
			TRAPD(err,iProtocol.HCIFacade().VendorSpecificCommandL(ioctl->iOpcode, ioctl->iParams));
			// notify completion now though really could wait until delivered from q
			NotifyIoctlComplete(err,
								KSolBtHCI,
								KHCIWriteVendorSpecificFrameNoEventExpectedIoctl,
								NULL);
			break;
			}
		default:
			NotifyIoctlComplete(
				KErrNotSupported,
				iIoctlLevel,
				iIoctlName,
				NULL
			);
			break;
		} // END Switch
	}


void CVendorSAP::VendorCommandCompleteEvent(TInt aErr, const TDesC8* aEvent)
	{
	if(iIoctlName == KHCIWriteVendorSpecificFrameIoctl)
		{
		/*
		A client is waiting for this.
		*/
		NotifyIoctlComplete(aErr, iIoctlLevel, KHCIWriteVendorSpecificFrameIoctl, aEvent);
		}
	}

void CVendorSAP::VendorDebugEvent(TInt aErr, const TDesC8* aEvent)
	{
	// Some events will not contain an event descriptor.
	TPtrC8 eventDesc = aEvent?*aEvent:KNullDesC8; 
	
	// Holds the data from an *expected* event (expected as in conduit said so!).
	TVendorDebugEvent* newEvent = NewDebugEventData(eventDesc, aErr);	
	
	if(newEvent)
		{
		iDebugEventQueue.AddLast(*newEvent); // add this event to the end of the list of events already queued
		}
	else
		{
		NotifyIoctlComplete(KErrNoMemory,
							iIoctlLevel,
							iIoctlName,
							NULL);
		return;
		}
	TryToCompleteVendorDebugEventIoctl();
	}


TInt CVendorSAP::SecurityCheck(MProvdSecurityChecker *aSecurityChecker)
	{
	__ASSERT_ALWAYS(aSecurityChecker, User::Panic(KSECURITY_PANIC, EBTPanicNullSecurityChecker));
		
	iSecurityChecker = aSecurityChecker;
	return iSecurityChecker->CheckPolicy(KLOCAL_SERVICES, KVENDOR_SAP_NAME_DIAG);				
	}

void CVendorSAP::NotifyIoctlComplete(TInt aErr, TUint aLevel, TUint aName, const TDesC8* aBuffer)
	{
	if (iIoctlLevel == aLevel && iIoctlName == aName)
		{
		if (aErr == KErrNone)
			{
			if(iSocket)
				iSocket->IoctlComplete(const_cast<TDesC8*>(aBuffer));
			}
		else
			{
			if(iSocket)
				iSocket->Error(aErr, MSocketNotify::EErrorIoctl);
			}
		}
	
	iIoctlLevel=0;
	iIoctlName=0;
	}
	

CVendorSAP::TVendorDebugEvent* CVendorSAP::NewDebugEventData(const TDesC8& aBuf, TInt aError)
/**
@param aBuf The Vendor event buffer. Must not be NULL
**/
	{
	TVendorDebugEvent* event = new TVendorDebugEvent;
	if(!event)
		{
		return 0;
		}
	event->iEventBuf.Copy(aBuf.Left(Min(CHctlCommandFrame::KHCIMaxCommandLength, aBuf.Length())));
	event->iEventError = aError;
	return event;
	}

void CVendorSAP::TryToCompleteVendorDebugEventIoctl()
	{
	// check to see if anyone expecting a debug event completion
	if (iIoctlName != KHCIWaitForVendorSpecificDebugEventIoctl)
		{
		return;
		}

	// nothing to do
	if(iDebugEventQueue.IsEmpty())
		{
		return;
		}

	TVendorDebugEvent* theEvent = iDebugEventQueue.First();

	// belt and braces!
	if(theEvent == NULL)
		{
		return;
		}
	
	// De-queue the event, as it is being consumed
	theEvent->iEventLink.Deque();

	// Complete the waiting ioctl, passing the the first appropriate
	// event in the queue back up.
	NotifyIoctlComplete(
		theEvent->iEventError,
		KSolBtHCI,
		KHCIWaitForVendorSpecificDebugEventIoctl,
		&(theEvent->iEventBuf)
		);

	// Event has been consumed, may now be deleted
	delete theEvent;
		
	}


void CVendorSAP::Start()
	{
	// do nothing
	}

void CVendorSAP::LocalName(TSockAddr& /*aAddr*/) const
	{
	Panic(EVendorSAPBadCall);
	}

TInt CVendorSAP::SetLocalName(TSockAddr& /*aAddr*/)
	{
	Panic(EVendorSAPBadCall);
	return KErrNotSupported;
	}

void CVendorSAP::RemName(TSockAddr& /*aAddr*/) const
	{
	Panic(EVendorSAPBadCall);
	}

TInt CVendorSAP::SetRemName(TSockAddr& /*aAddr*/)
	{
	Panic(EVendorSAPBadCall);
	return KErrNotSupported;
	}

TInt CVendorSAP::GetOption(TUint /*aLevel*/,TUint /*aName*/,TDes8& /*aOption*/) const
	{
	Panic(EVendorSAPBadCall);
	return KErrNotSupported;
	}

void CVendorSAP::CancelIoctl(TUint __DEBUG_ONLY(aLevel),TUint __DEBUG_ONLY(aName))
	{
	__ASSERT_DEBUG(iIoctlLevel == aLevel, Panic(EVendorSAPBadIoctlCancel));
	__ASSERT_DEBUG(iIoctlName == aName, Panic(EVendorSAPBadIoctlCancel));
	
	iIoctlName = 0;
	iIoctlLevel = 0;
	}

TInt CVendorSAP::SetOption(TUint /*aLevel*/,TUint /*aName*/,const TDesC8& /*aOption*/)
	{
	Panic(EVendorSAPBadCall);
	return KErrNotSupported;
	}

void CVendorSAP::ActiveOpen()
	{
	Panic(EVendorSAPBadCall);
	}

void CVendorSAP::ActiveOpen(const TDesC8& /*aConnectionData*/)
	{
	Panic(EVendorSAPBadCall);
	}

TInt CVendorSAP::PassiveOpen(TUint /*aQueSize*/)
	{
	Panic(EVendorSAPBadCall);
	return KErrNotSupported;
	}

TInt CVendorSAP::PassiveOpen(TUint /*aQueSize*/,const TDesC8& /*aConnectionData*/)
	{
	Panic(EVendorSAPBadCall);
	return KErrNotSupported;
	}

void CVendorSAP::AutoBind()
	{
	Panic(EVendorSAPBadCall);
	}

TUint CVendorSAP::Write(const TDesC8& /*aDesc*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	Panic(EVendorSAPBadCall);
	return 0;
	}

void CVendorSAP::GetData(TDes8& /*aDesc*/,TUint /*aOptions*/,TSockAddr* /*aAddr*/)
	{
	Panic(EVendorSAPBadCall);
	}

void CVendorSAP::Shutdown(TCloseType aCloseType)
	{
	if (aCloseType == ENormal)
		{
		iSocket->CanClose();
		}
	}

void CVendorSAP::Shutdown(TCloseType /*aOption*/,const TDesC8& /*aDisconnectionData*/)
	{
	Panic(EVendorSAPBadCall);
	}


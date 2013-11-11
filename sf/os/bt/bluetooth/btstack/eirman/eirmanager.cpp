// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "eirmanager.h"
#include <bluetooth/hcicommandqueue.h>
#include <bttypes.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/commandstatusevent.h>
#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hciconsts.h>
#include <bluetooth/hci/writeextendedinquiryresponsecommand.h>
#include <bluetooth/logger.h>
#include "eirmansession.h"
#include "linkmgr.h"
#include "eirmanpanics.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_EIRMANAGER);
#endif

__ASSERT_COMPILE(KReservedNameBytes + KReservedUuid16Bytes + KReservedUuid128Bytes + KReservedManufacturerSpecificDataBytes + KReservedTxPowerLevelBytes == KEirPacketSize);

CEirManager* CEirManager::NewL(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol)
	{
	LOG_STATIC_FUNC
	
	CEirManager* self = new(ELeave) CEirManager(aCommandQueue, aLinkMgrProtocol);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CEirManager::~CEirManager()
	{
	LOG_FUNC
	
	// Delete all registered tags
	for(TInt8 i = static_cast<TInt8>(EEirTagName); i < static_cast<TInt8>(EEirTagRESERVED); i++)
		{
		__ASSERT_DEBUG(!iNotifiees[i].iNotifiee, EIR_MANAGER_PANIC(EEirManagerNotifieeNotCleared));
		}
	
	iCommandQueue.MhcqRemoveAllCommands(*this);
	delete iWriteEirTimer;
	}

CEirManager::CEirManager(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol)
	: iOffersStale(EFalse)
	, iCmdId(KInvalidCommandQueueItemId)
	, iCmdCount(0)
	, iCommandQueue(aCommandQueue)
	, iLinkMgrProtocol(aLinkMgrProtocol)
	{
	LOG_FUNC
	}

void CEirManager::ConstructL()
	{
	LOG_FUNC
	iWriteEirTimer = CWriteEirTimer::NewL(*this);
	}

void CEirManager::UnhookClient(TEirTag aTag)
	{
	LOG_FUNC
	if (iNotifiees[aTag].iNotifiee)
		{
		iNotifiees[aTag].iNotifiee = NULL;
		if(iNotifiees[aTag].iTagData)
			{
			delete iNotifiees[aTag].iTagData;
			iNotifiees[aTag].iTagData = NULL;
			}			
		}
	}

// A client register aTag with EirManager
TInt CEirManager::RegisterTag(TEirTag aTag, MEirManagerNotifiee& aNotifiee)
	{
	LOG_FUNC
	TInt ret = KErrNone;
	LOG1(_L("CEirManager::RegisterTag tag = %d"), aTag);

	if(!TagValid(aTag))
		{
		LOG1(_L("CEirManager::RegisterTag INVALID TAG: %d"), aTag);
		ret = KErrArgument;
		}
	else if(!TagAllowed(aTag))
		{
		LOG(_L("CEirManager::RegisterTag: IN USE"));
		ret = KErrInUse;
		}
	else
		{
		iNotifiees[aTag].iNotifiee = &aNotifiee;
		iNotifiees[aTag].iTagData = NULL;
		iNotifiees[aTag].iOfferedBytes = 0;
		iNotifiees[aTag].iEirDataMode = EEirDataComplete;
		iNotifiees[aTag].iState = 0x00;

		if (!OffersPending() && NeedToOffer())
			{
			// Do not consider this for insertion yet if there are offers pending.
			// Instead, once the offers have been responded to, this client will be dealt with
			NotifyClients();
			}
		}
	return ret;
	}

// A client deregister aTag with EirManager
void CEirManager::DeregisterTag(TEirTag aTag)
	{
	LOG_FUNC
	LOG1(_L("CEirManager::DeregisterTag tag = %d"), aTag);

	if(!TagValid(aTag))
		{
		LOG1(_L("CEirManager::DeregisterTag INVALID TAG: %d"), aTag);
		return;
		}

	if (iNotifiees[aTag].iNotifiee)
		{
		UnhookClient(aTag);	
		if(iNotifiees[aTag].iRequiredBytes > 0)
			{
			// It did require some bytes and now requires none, reevaluate offers
			iNotifiees[aTag].iState &= ~KNeedOffer;
			// A client could storm in a deregister before setting data - so we remove
			// any pending offer as a SetData after deregistering will be errored - any
			// previous offer has been voided.
			iNotifiees[aTag].iState &= ~KOfferPending;
			if(!OffersPending())
				{
				NotifyClients();
				}
			else if(iNotifiees[aTag].iOfferedBytes > 0)
				{
				iOffersStale = ETrue;
				}
			}
		if(!NeedToOffer() && !OffersPending())
			{
			TryToUpdateHostController();
			}
		// Now tidy up offer and requested values (they are no longer valid).
		iNotifiees[aTag].iRequiredBytes = 0;
		iNotifiees[aTag].iOfferedBytes = 0;
		}
	}

TInt CEirManager::SetData(TEirTag aTag, const TDesC8& aData, TEirDataMode aEirDataMode)
	{
	LOG_FUNC
	LOG1(_L("CEirManager::SetData tag = %d"), aTag);
	
	if(!iLinkMgrProtocol.IsExtendedInquiryResponseSupportedLocally())
		{
		return KErrNotSupported;
		}

	if(!TagValid(aTag))
		{
		LOG1(_L("CEirManager::SetData INVALID TAG: %d"), aTag);
		return KErrArgument;
		}

	if(!iNotifiees[aTag].iNotifiee)
		{
		// No client registered with this tag
		return KErrNotFound;
		}

	LOG2(_L("CEirManager::SetData data len = %d offered bytes: %d"), aData.Length(), iNotifiees[aTag].iOfferedBytes);

	if (!(iNotifiees[aTag].iState & KOfferPending))
		{
		// No offer pending for this tag
		return KErrNotReady;		
		}
	if(iNotifiees[aTag].iOfferedBytes < aData.Length())
		{
		// Client wants to publish more data than allowed to
		return KErrTooBig;
		}
		
	TInt ret = KErrNone;
	
	HBufC8** tagdata;
	
	FindTagData(aTag, tagdata);
	
	// Free up whatever there was before
	if (*tagdata)
		{
		delete *tagdata;
		}

	HBufC8* data = NULL;

	// If the descriptor is empty, mark the heap buffer pointer as NULL, 
	// meaning that the client is registered for this tag, but there is no currently no data available for it
	if(aData.Length() != 0x0)
		{
		data = aData.Alloc();
		if (!data)
			{
			LOG(_L("CEirManager::SetData: NO MEMORY "));
			ret = KErrNoMemory;
			}
		}

	*tagdata = data;

	// We no longer have a value pending.
	iNotifiees[aTag].iState &= ~KOfferPending;
	iNotifiees[aTag].iEirDataMode = aEirDataMode;

	if(NeedToOffer() && !OffersPending())
		{
		NotifyClients();
		}

	if(!NeedToOffer() && !OffersPending())
		{

		// Regardless of whether the allocation failed or not, check whether all offers have been accepted
		// and update the Host Controller accordingly
		TryToUpdateHostController();
		}
		
	LOG1(_L("CEirManager::SetData returning = %d"), ret);
	return ret;
	}

void CEirManager::MhcqcCommandEventReceived(const THCIEventBase& aEvent, 
											const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	if(aEvent.EventCode() == ECommandCompleteEvent)
		{
		const THCICommandCompleteEvent& completeEvent = static_cast<const THCICommandCompleteEvent&>(aEvent);
		if(completeEvent.CommandOpcode() == KWriteExtendedInquiryResponseOpcode)
			{
			--iCmdCount;
			}
		else
			{
			LOG1(_L("Warning!! Unexpected Command Complete Event Received (command opcode: 0x%04x)"), completeEvent.CommandOpcode());
			__ASSERT_DEBUG(EFalse, EIR_MANAGER_PANIC(EEirManagerUnexpectedCompleteEvent));
			}
		}
	else
		{
		LOG1(_L("Warning!! Unexpected Event Received (event code: 0x%02x)"), aEvent.EventCode());
		__ASSERT_DEBUG(EFalse, EIR_MANAGER_PANIC(EEirManagerUnexpectedEvent));
		}
	}

void CEirManager::MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand)
	{
	LOG_FUNC
	aErrorCode = aErrorCode;   // Avoid unused warning
	LOG1(_L("\taErrorCode = %d"), aErrorCode);
	if(aCommand && aCommand->Opcode() == KWriteExtendedInquiryResponseOpcode)
		{
		--iCmdCount;
		}
	else
		{
		LOG(_L("Warning!! Unexpected Error Received"));
		if(aCommand)
			{
			LOG1(_L("\taCommand->Opcode() = 0x%04x"), aCommand->Opcode());
			}
		}
	}

void CEirManager::WriteExtendedInquiryResponseL(TBool aFECRequired, const TDesC8& aEir)
	{
	LOG_FUNC
	if(iCmdCount > 0)
		{
		// There is at least 1 outstanding command in the queue
		TInt err = iCommandQueue.MhcqRemoveCommand(iCmdId, *this);
		if(err == KErrNone)
			{
			--iCmdCount;
			}
		// else if there was an error it is because the command cannot be removed
		// "at this time" so we just "leak" it.  This isn't a problem, this was a 
		// best effort optimisation.
		}
	CWriteExtendedInquiryResponseCommand* cmd = CWriteExtendedInquiryResponseCommand::NewL(aFECRequired ? 1 : 0, aEir);
	// Ownership of cmd transfered
	iCmdId = iCommandQueue.MhcqAddCommandL(cmd, *this);
	++iCmdCount;
	}

// Check whether a specific tag is allowed to be registered
TBool CEirManager::TagAllowed(TEirTag aTag) const
	{
	LOG_FUNC
	// We don't support 32 bit UUIDs
	return (aTag!=EEirTagSdpUuid32);
	}
	
// Check whether aTag is present
TBool CEirManager::TagPresent(TEirTag aTag) const
	{
	LOG_FUNC
	return (iNotifiees[aTag].iNotifiee != NULL);
	}

// Find the registered data for aTag
void CEirManager::FindTagData(TEirTag aTag, HBufC8**& aOutPointerToData)
	{
	LOG_FUNC
	aOutPointerToData = &iNotifiees[aTag].iTagData;
	}

// Count the number of registered tags
TInt CEirManager::TagCount() const
	{
	LOG_FUNC
	TInt count = 0;
	for (TInt i=0; i<EEirTagRESERVED; i++)
		{
		if (iNotifiees[i].iNotifiee)
			{
			count++;
			}
		}
	return count;
	}

TBool CEirManager::OffersPending() const
	{
	LOG_FUNC
	for (TInt i=0; i<EEirTagRESERVED; i++)
		{
		if(iNotifiees[i].iState & KOfferPending)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TBool CEirManager::NeedToOffer() const
	{
	LOG_FUNC
	if(iOffersStale)
		{
		return ETrue;
		}
	for (TInt i=0; i<EEirTagRESERVED; i++)
		{
		if (iNotifiees[i].iState & KNeedOffer)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// Notify EirManager there is a change of data from aTag client
TInt CEirManager::NewData(TEirTag aTag, TInt aRequiredBytes)
	{
	LOG_FUNC
	if(!iLinkMgrProtocol.IsExtendedInquiryResponseSupportedLocally())
		{
		return KErrNotSupported;
		}

	// including 2 bytes for length and tag
	TInt requiredBytes = 0;
	TInt currentBytes = iNotifiees[aTag].iRequiredBytes;
	if(aRequiredBytes > 0)
		{
		requiredBytes = aRequiredBytes + KEirLengthTagLength;
		}
	else if(aTag == EEirTagName && aRequiredBytes == 0)
		{
		// This is a special case for device without local name
		requiredBytes = KEirLengthTagLength;
		}
	else
		{
		requiredBytes = 0;
		}
	
	iNotifiees[aTag].iRequiredBytes = requiredBytes; // always record what we last asked for.
	
	if(!(iNotifiees[aTag].iState & KNeedOffer))
		{
		// We currently aren't marked as needing an offer so we will need to fix that...
		iNotifiees[aTag].iState |= KNeedOffer;
		if(requiredBytes == currentBytes && !(iNotifiees[aTag].iState & KOfferPending) && !iOffersStale)
			{
			// If the same then we handle locally, if we aren't already waiting.
			DoNotify(aTag);
			}
		else if(!OffersPending())
			{
			// If not waiting for anybody then lets kick off a round.
			NotifyClients();
			}
		}
	return KErrNone;
	}
	
// Notify all registered clients about their allocated data length
void CEirManager::NotifyClients()
	{

	LOG_FUNC

	// go around each notifiee and ask for values
	// our policy at the moment is: 
	// after allocating reserved bytes for each tag, we will prioritize tag with big granularity

	ComputeOfferLengths(); 

	for (TInt i = 0; i<EEirTagRESERVED; i++)
		{
		if(TagPresent((TEirTag) i))
			{
			DoNotify((TEirTag)i);
			}
		}
	}

// Notify each tag about the available data length
void CEirManager::DoNotify(TEirTag aTag)
	{
	LOG_FUNC

	iNotifiees[aTag].iState &= ~KNeedOffer; // We got an offer!
	
	// Only make valid offers: offers more than 0 and 0 offers with non 0 request
	if(iNotifiees[aTag].iOfferedBytes > 0)
		{
		// Notify client about offered bytes
		iNotifiees[aTag].iState |= KOfferPending;
		iNotifiees[aTag].iNotifiee->MemnEirBlockAvailable(aTag, iNotifiees[aTag].iOfferedBytes);
		}
	else if(iNotifiees[aTag].iOfferedBytes == 0)
		{
		if(iNotifiees[aTag].iRequiredBytes > 0)
			{
			// Notify client about 0 offer and not expecting client to call back
			iNotifiees[aTag].iState |= KOfferPending;
			iNotifiees[aTag].iNotifiee->MemnEirBlockAvailable(aTag, iNotifiees[aTag].iOfferedBytes);
			}
		else if(iNotifiees[aTag].iRequiredBytes == 0)
			{
			// request 0 and offered 0, delete client
			iNotifiees[aTag].iState &= ~KNeedOffer;
			delete iNotifiees[aTag].iTagData;
			iNotifiees[aTag].iTagData = NULL;
			}
		}
	}

void CEirManager::ComputeOfferLengths()
	{
	LOG_FUNC
	// count bytes available for EIR data, including length and tag
	// due to the low cost of Tx Power Level, it is always given a space in EIR
	TInt availableEirSize = EIRPacketSize();
	TInt reservedByteForName = KReservedNameBytes - (KEirPacketSize - availableEirSize);
	TInt totalSpare = 0;

	ComputeClientInitialOffer(EEirTagName, totalSpare, reservedByteForName);
	ComputeClientInitialOffer(EEirTagSdpUuid16, totalSpare, KReservedUuid16Bytes);
	ComputeClientInitialOffer(EEirTagSdpUuid128, totalSpare, KReservedUuid128Bytes);
	ComputeClientInitialOffer(EEirTagManufacturerSpecific, totalSpare, KReservedManufacturerSpecificDataBytes, ETrue);
	ComputeClientInitialOffer(EEirTagTxPowerLevel, totalSpare, KReservedTxPowerLevelBytes, ETrue);

	// if there is still bytes left after allocating the reserved bytes, 
	// we will try to allocate them by giving high priority to tag with bigger granularity
	/* tag			granularity (byte)
	   name				1
	  uuid16			2
	  uuid128			16
	  manufacturer		length
	*/
	LOG4(_L("RequiredBytes **** Name: %d, UUID16: %d, UUID128: %d, Manu: %d"), 
			iNotifiees[0].iRequiredBytes, iNotifiees[1].iRequiredBytes, iNotifiees[3].iRequiredBytes, iNotifiees[4].iRequiredBytes);
	LOG4(_L("OfferedBytes ----- Name: %d, UUID16: %d, UUID128: %d, Manu: %d"), 
			iNotifiees[0].iOfferedBytes, iNotifiees[1].iOfferedBytes, iNotifiees[3].iOfferedBytes, iNotifiees[4].iOfferedBytes);
	LOG1(_L("totalSpare: %d"), totalSpare);
	ComputeClientFinalOffer(EEirTagManufacturerSpecific, totalSpare, KReservedManufacturerSpecificDataBytes, ETrue);
	ComputeClientFinalOffer(EEirTagSdpUuid128, totalSpare, K128BitUuidGranularity);
	ComputeClientFinalOffer(EEirTagSdpUuid16, totalSpare, K16BitUuidGranularity);
	ComputeClientFinalOffer(EEirTagName, totalSpare, KDeviceNameGranularity);
	// Tx Power Level should be after the allocating the reserved bytes
	iOffersStale = EFalse; // We now know any "staleness" has been corrected.

	LOG(_L("After allocating reserved"));
	LOG4(_L("RequiredBytes **** Name: %d, UUID16: %d, UUID128: %d, Manu: %d"),
			iNotifiees[0].iRequiredBytes, iNotifiees[1].iRequiredBytes, iNotifiees[3].iRequiredBytes, iNotifiees[4].iRequiredBytes);
	LOG4(_L("OfferedBytes ----- Name: %d, UUID16: %d, UUID128: %d, Manu: %d"),
			iNotifiees[0].iOfferedBytes, iNotifiees[1].iOfferedBytes, iNotifiees[3].iOfferedBytes, iNotifiees[4].iOfferedBytes);
	LOG1(_L("totalSpare: %d"), totalSpare);
	}

/* compute offer based on client's request and the reserved bytes for this client*/
void CEirManager::ComputeClientInitialOffer(TEirTag aTag, TInt& aTotalSpare, TInt aReservedBytes,
											TBool aIsAllOrNothing)
	{
	if(TagPresent(aTag))
		{
		if(aIsAllOrNothing)
			{
			if(iNotifiees[aTag].iRequiredBytes <= aReservedBytes)
				{
				iNotifiees[aTag].iOfferedBytes = iNotifiees[aTag].iRequiredBytes;
				aTotalSpare += (aReservedBytes - iNotifiees[aTag].iRequiredBytes);
				iNotifiees[aTag].iEirDataMode = EEirDataComplete;
				}
			else
				{
				// the request exceeds reserved bytes for this client, the client is offered nothing
				iNotifiees[aTag].iOfferedBytes = 0;
				iNotifiees[aTag].iEirDataMode = EEirDataPartial;
				aTotalSpare += aReservedBytes;
				}
			}
		else
			{
			if(iNotifiees[aTag].iRequiredBytes <= aReservedBytes)
				{
				iNotifiees[aTag].iOfferedBytes = iNotifiees[aTag].iRequiredBytes;
				aTotalSpare += (aReservedBytes - iNotifiees[aTag].iRequiredBytes);
				iNotifiees[aTag].iEirDataMode = EEirDataComplete;
				}
			else
				{
				iNotifiees[aTag].iOfferedBytes = aReservedBytes;
				iNotifiees[aTag].iEirDataMode = EEirDataPartial;
				}
			}
		}
	else
		{
		aTotalSpare += aReservedBytes;
		}
	}

/* compute offer based on client's request and the spared bytes from the initial offers*/
void CEirManager::ComputeClientFinalOffer(TEirTag aTag, TInt& aTotalSpare, TUint aGranularity,
				  TBool aIsAllOrNothing)
	{
	if(iNotifiees[aTag].iRequiredBytes > iNotifiees[aTag].iOfferedBytes)
		{
		if(aIsAllOrNothing)
			{
			// this means we have more data to publish, which also indicates
			// it required bytes are more than reserved bytes for this tag
			if(iNotifiees[aTag].iRequiredBytes <= aTotalSpare)
				{
				aTotalSpare -= iNotifiees[aTag].iRequiredBytes;
				iNotifiees[aTag].iOfferedBytes = iNotifiees[aTag].iRequiredBytes;
				iNotifiees[aTag].iEirDataMode = EEirDataComplete;
				}
			}
		else
			{
			// this means we have more data to publish
			if((iNotifiees[aTag].iRequiredBytes - iNotifiees[aTag].iOfferedBytes) <= aTotalSpare)
				{
				aTotalSpare -= (iNotifiees[aTag].iRequiredBytes - iNotifiees[aTag].iOfferedBytes);
				iNotifiees[aTag].iOfferedBytes = iNotifiees[aTag].iRequiredBytes;
				iNotifiees[aTag].iEirDataMode = EEirDataComplete;
				}
			else
				{
				iNotifiees[aTag].iOfferedBytes += (aTotalSpare - aTotalSpare % aGranularity);
				aTotalSpare %= aGranularity;
				}
			}
		}
	}

// Calculate the valid size of EIR packet, excluding payload header	
TInt CEirManager::EIRPacketSize() const
	{
	LOG_FUNC
	TInt total = 0;
	// default value of valid EIR packet is for using DM1
	TInt ret = KEirPacketSize - KEirDM1PayloadHeaderSize;
	for (TInt i = 0; i<EEirTagRESERVED; i++)
		{
		if(TagPresent((TEirTag) i) && iNotifiees[i].iRequiredBytes > 0)
			{
			total += iNotifiees[i].iRequiredBytes;
			// Including 2 bytes for the length and tag
			total += KEirLengthTagLength;
			}
		}
		
	if(total > KEirDM1PacketSize)
		{
		// None DM1 packet has 2 bytes of payload header, recalculate...
		ret = KEirPacketSize - KEirNoneDM1PayloadHeaderSize;
		}

	return ret;
	}

void CEirManager::UpdateHostControllerL()
	{
	LOG_FUNC
	
	/* We don't handle the situation of handware swapping after stack is started
	 * when the device changed from 2.1 to 2.0 and EIR is no longer supports, 
	 * we'll quietly stop writing EIR and return KErrNotSupported for future requests.
	we
	 */
	__ASSERT_DEBUG(iLinkMgrProtocol.IsExtendedInquiryResponseSupportedLocally(), EIR_MANAGER_PANIC(EEirManagerEirNotSupported));
	TBuf8<KHCIExtendedInquiryResponseMaxLength> eir;

	for (TInt i = 0; i<EEirTagRESERVED; i++)
		{
		TExtendedInquiryResponseDataType dataType = EEirLocalNamePartial;

		// Skip non-registered and data-empty tags
		if(iNotifiees[i].iNotifiee)
			{
			if(i == EEirTagName || iNotifiees[i].iTagData)
				{
				// We'll always try to publish empty device name according to the spec
				switch (i)
					{
					// Name should go first
					case EEirTagName:
					dataType = iNotifiees[i].iEirDataMode == EEirDataPartial ? EEirLocalNamePartial : EEirLocalNameComplete;
					break;
					
					case EEirTagSdpUuid16:
					dataType = iNotifiees[i].iEirDataMode == EEirDataPartial ?  EEirUUID16Partial : EEirUUID16Complete;
					break;
									
					case EEirTagSdpUuid128:
					dataType = iNotifiees[i].iEirDataMode == EEirDataPartial ?  EEirUUID128Partial : EEirUUID128Complete;
					break;
					
					case EEirTagManufacturerSpecific:
					dataType = EEirVendorSpecific;
					break;
					
					case EEirTagTxPowerLevel:
					dataType = EEirTxPowerLevel;
					break;
					}
				if(iNotifiees[i].iTagData)
					{
					// Data Structure length, add a byte for the Data Type
					eir.Append(iNotifiees[i].iTagData->Length() + 1);
					// Data Structure data: Data Type
					eir.Append(dataType);
					// Data Structure data: EIR data
					eir.Append(*(iNotifiees[i].iTagData));
					}
				else
					{
					// Here is the case of empty device name again
					eir.Append(1);
					// Data Structure data: Data Type
					eir.Append(dataType);
					}
				}
			}
		}

	LOG1(_L("CEirManager::UpdateHostController about to send %d bytes of EIR data"), eir.Length());
	// Send this down HCI
	WriteExtendedInquiryResponseL(ETrue, eir);

	LOG(_L("------- EIR verbose display"));
	LOGHEXDESC(eir);
	}

void CEirManager::TryToUpdateHostController()
	{
	// In case there is an outstanding write eir command (a timer running) already, Cancel()
	iWriteEirTimer->Cancel();
	
	// Start the timer to gurad against from clients' updates flooding the controller
	iWriteEirTimer->Start();
	}


CWriteEirTimer* CWriteEirTimer::NewL(CEirManager& aEirManager)
	{
	CWriteEirTimer* timer = new (ELeave) CWriteEirTimer(aEirManager);
	CleanupStack::PushL(timer);
	timer->ConstructL();
	CleanupStack::Pop(timer);
	return timer;
	}


CWriteEirTimer::CWriteEirTimer(CEirManager& aEirManager)
: CTimer(EPriorityStandard), iEirManager(aEirManager)
	{
	}

void CWriteEirTimer::Start()
	{
	After(KWriteEirTimeout);
	}

void CWriteEirTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CWriteEirTimer::RunL()
	{
	// timer expires, which means no more write eir request within life of the time (1 second)
	// write eir here now
	iEirManager.UpdateHostControllerL();
	}

TInt CWriteEirTimer::RunError(TInt IF_FLOGGING(aError))
	{
	LOG_FUNC
	// The only reason the error occurs is that the WriteExtendedInquiryResponseL has failed
	// This may be caused by invalid EIR data, we'll do nothing here.
	LOG1(_L("CWriteEirTimer::RunError() %d: "), aError);
	return KErrNone;
	}

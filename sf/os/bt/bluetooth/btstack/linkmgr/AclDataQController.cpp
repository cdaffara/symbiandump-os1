// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Link Command Q Controlling internal methods 
// The command Q is used to store commands used by the stack
// The HCI is at liberty to use its own command Q for any other HC signalling 
// it requires.
// The HCI in such cases would then signal the stack with not all credits etc.
// 
//

#include <bluetooth/logger.h>
#include <bluetooth/hci/hciconsts.h>
#include "AclDataQController.h"
#include "linkmgr.h"
#include "AclDataQ.h"
#include "hcifacade.h"
#include "hostmbufpool.h"
#include "linkflowcontrol.h"
#include "linkconsts.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

const TInt KACLDataQConnectionRecordGranularity = 2;

/**
	Factory function.

	@return Ownership of a new CACLDataQController.
 */
CACLDataQController* CACLDataQController::NewL(CLinkMgrProtocol& aProtocol,
											   CLinkMuxer& aMuxer,
											   TUint16 aBufSize,
											   TUint16 aFrameOverhead,
											   TUint aNumBufs)
	{
	LOG_STATIC_FUNC
	LOG3(_L("CACLDataQController::NewL aBufSize = %d, aFramingOverhead = %d, aNumBufs = %d"),
		aBufSize, aFrameOverhead, aNumBufs);

	CACLDataQController* self = new(ELeave) CACLDataQController(aProtocol.HCIFacade(), aMuxer);
	CleanupStack::PushL(self);
	self->ConstructL(aProtocol, aBufSize, aFrameOverhead, aNumBufs);
	CleanupStack::Pop(self);
	return self;
	}

CACLDataQController::CACLDataQController(CHCIFacade& aHCIFacade,
										 CLinkMuxer& aMuxer)
 :	iAclConns(KACLDataQConnectionRecordGranularity),
	iLinkMuxer(aMuxer),
	iHCIFacade(aHCIFacade)
	{
	LOG_FUNC
	}

CACLDataQController::~CACLDataQController()
	{
	LOG_FUNC
	
#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
	delete iMBufPool;
#endif
	
	delete iDataQ;
	iAclConns.Reset();
	iAclConns.Close();
	}

void CACLDataQController::ConstructL(CLinkMgrProtocol& aProtocol,
									 TUint16 aBufSize,
									 TUint16 aFrameOverhead,
									 TUint aNumBufs)
	{
	LOG_FUNC

	iDataQ = CAclDataQ::NewL(aProtocol, aNumBufs, aBufSize, aFrameOverhead);
	

#ifdef PROXY_COMMUNICATES
	LOG(_L("\tPROXY_COMMUNICATES defined- reserving slots for broadcast channel"));

	// Reserve BC one now
	User::LeaveIfError(ACLLogicalLinkUp(KHCIBroadcastHandle, EFalse));
#endif
	
#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
	LOG(_L8("\tHOSTCONTROLLER_TO_HOST_FLOW_CONTROL defined- creating buffer pool"));
	iMBufPool = CHostMBufPool::NewL(aProtocol.HCIFacade().CommandQController());
#endif
	}

void CACLDataQController::InitialDataCredits(TUint16 aCredits)
/**
	Set the initial data credits to whatever number of buffers the HC has.
*/
	{
	LOG_FUNC
	LOG1(_L("CACLDataQController::InitialDataCredits aCredits = %d"), aCredits);

	iDataCredits = aCredits;
	iNumControllerBufs = aCredits;
	}

CACLDataItem* CACLDataQController::GetFreeItem()
/**
	Returns ownership of a data item, or NULL if there are none free.
 */
	{
	LOG_FUNC

	__ASSERT_DEBUG(iDataQ, Panic(EHCIBufferMgrBadState));
	CACLDataItem* ret = iDataQ->RemoveFirstSpareItem();
	
	LOG1(_L("CACLDataQController::GetFreeItem ret = 0x%08x"), ret);
	return ret;
	}

void CACLDataQController::AddItem(CACLDataItem& aAclItem)
	{
	LOG_FUNC
	LOG1(_L("CACLDataQController::AddItem &aAclItem = 0x%08x"), &aAclItem);

	// what Connection Handle is this frame for?
	__ASSERT_DEBUG(aAclItem.Frame(), Panic(EHCIBufferMgrBadState));
	THCIConnHandle connh = (*(aAclItem.Frame())).ConnectionHandle();
	
	// Get the connection record
	const TInt index = FindConnection(connh);
	__ASSERT_ALWAYS(index>=0, Panic(EHCIACLDataControllerConnectionListConnectionNotFound));

	++(iAclConns[index].iPacketsQueued);
	LOG3(_L("\tadded an item for connection handle %d: iPacketsQueued = %d, iPacketsPending = %d"), 
		connh, iAclConns[index].iPacketsQueued, iAclConns[index].iPacketsPending);

	__ASSERT_DEBUG(iDataQ, Panic(EHCIBufferMgrBadState));
	iDataQ->AddItem(aAclItem);
	iLinkMuxer.TryToSend();
	}

THCIConnHandle CACLDataQController::HighestPriority()
/**
 Get the highest priority connection, starting to look from the connection 
 which sent last, and ignoring connections with no queued data to send.
 If there's no clear highest priority connection, the connection handle of the 
 0th connection in our array will be returned, whether it has anything to send 
 or not (i.e. you always get a valid connection handle). 
 Assumes that there is at least 1 connection handle in our array.
 */
	{
	LOG_FUNC
	LOG1(_L("CACLDataQController::HighestPriority iIndexOfLastSendingConn = %d"), 
		iIndexOfLastSendingConn);

	const TUint count = iAclConns.Count();
	LOG1(_L("\tcount = %d"), count);
	// Should only be called if we have connections to judge the highest 
	// priority of.
	__ASSERT_DEBUG(count != 0, Panic(EHCIBufferMgrBadState));

	// Deal with the most likely case first, for speed...
	if ( count == 1 )
		{
		LOG1(_L("CACLDataQController::HighestPriority returning %d"), 
			iAclConns[0].iConnH);
		return iAclConns[0].iConnH;
		}

	// Now look through iAclConns from the one which sent last...
	__ASSERT_DEBUG(iIndexOfLastSendingConn < count, Panic(EHCIBufferMgrBadState));
	TInt highestpriority = -KMaxTInt;
	// NB If no connections have anything to send, connection index 0 will be 
	// indicated by the return value.
	TInt highestindex = 0;
	TUint index;
	if ( iIndexOfLastSendingConn + 1 == count )
		{
		index = 0;
		}
	else
		{
		index = iIndexOfLastSendingConn + 1;
		}

	FOREVER
		{
		__ASSERT_DEBUG(index < static_cast<TUint>(iAclConns.Count()), Panic(EHCIBufferMgrBadState));

		const TInt priority = -(iAclConns[index].iPacketsPending);
		const TInt queued = iAclConns[index].iPacketsQueued;

#ifdef __FLOG_ACTIVE
		const THCIConnHandle connh = iAclConns[index].iConnH;
		LOG3(_L("\tconnection handle %d: iPacketsQueued = %d, iPacketsPending = %d"), 
			connh, queued, -priority);
#endif

		// Only consider connections with some data queued.
		if ( queued > 0 && priority > highestpriority )
			{
			highestpriority = priority;
			highestindex = index;
			}

		LOG3(_L("\tindex = %d, highestpriority = %d, highestindex = %d"),
			index, highestpriority, highestindex);
		
		++index;
		// Have we gone all the way round?
		if ( index == iIndexOfLastSendingConn + 1 )
			{
			break;
			}
		// If we're at the end of the array, continue looking from the 
		// beginning.
		if ( index == count )
			{
			index = 0;
			}
		}

	LOG1(_L("CACLDataQController::HighestPriority connection handle = %d"), 
		iAclConns[highestindex].iConnH);
	return iAclConns[highestindex].iConnH;
	}

TBool CACLDataQController::AnotherPacketAllowed(TDataQConnectionInfo& aRecord)
/**	
	A small strategy to see if another packet is allowed on a given connection 
	handle.
	Always try to keep some spare host controller buffers for another 
	connection handle that might arise. This maxes out at 8.
*/
	{
	LOG_FUNC

	TBool allowAnotherPacket;
	
	if ( aRecord.iParked && aRecord.iConnH != KHCIBroadcastHandle )
		{
		// not allowed to send on non-broadcast handle when parked
		allowAnotherPacket = EFalse;
		}
	else
		{
		// the rest of the handles are in principle sendable, but need to 
		// check for room in HC. NB The '- 1' here is to make sure that one 
		// connection can't result in the whole device being locked up if the 
		// remote device stops responding- there'll still be one slot left 
		// over. 
		allowAnotherPacket = aRecord.iPacketsPending < iNumControllerBufs - 1 ? ETrue : EFalse;
		}

	LOG1(_L("CACLDataQController::AnotherPacketAllowed allowAnotherPacket = %d"), 
		allowAnotherPacket);
	return allowAnotherPacket;
	}

/**
 Called by the link muxer to do an actual send.
 Get the link which is entitled to send (on the basis of priority (which is 
 simply (0-number of packets waiting to be sent)) and recent history (if it 
 was the last to send, we check all the other links first for a higher or 
 equal priority one).
 Finally, get a packet belonging to the winning link and attempt to send it. 
 
 @return EFalse if no send occurred, ETrue otherwise.
 */
TBool CACLDataQController::IssueNextACLDataFragment()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iDataCredits>0,Panic(EHCIACLDataControllerDataCreditsZeroBeforeIssue));

	if ( !iAclConns.Count() )
		{
		LOG(_L("CACLDataQController::IssueNextACLDataFragment returning EFalse"));
		return EFalse;
		}

	THCIConnHandle connh = HighestPriority(); // find suggested connection handle to send on
	//ask the DataQ for the first item on this connection handle
	CACLDataItem* item = NULL;
	__ASSERT_DEBUG(iDataQ, Panic(EHCIBufferMgrBadState));
	item = iDataQ->FirstItemByConnectionHandle(iHCIFacade, connh);
	//if still NULL then there is no longer stuff on that connH on the Q so get the head item
	if ( !item )
		{
		item = iDataQ->FirstItem(iHCIFacade, connh); // connh will be set
		}

	TBool sent = EFalse;

	const TInt index = FindConnection(connh);
	LOG1(_L("\tFindConnection returned %d"), index);
	if ( index == KErrNotFound )
		{
		// Connection not found, because it went down between the link muxer 
		// realising it could try to send and it (asynchronously) actually 
		// asking us to send.
		LOG1(_L("CACLDataQController::IssueNextACLDataFragment sent = %d"), 
			sent);
		return sent;
		}

	if ( !AnotherPacketAllowed(iAclConns[index]) )
		{
		LOG1(_L("CACLDataQController::IssueNextACLDataFragment sent = %d"), 
			sent);
		return sent; // no more allowed, otherwise HC liable to clogging
		}

	__ASSERT_DEBUG(item, Panic(EHCIBufferMgrBadState));
	sent = SendItem(*item);

	// Regardless of whether the send succeeded, make a note that this 
	// connection handle was the last to 'have a go'...
	iIndexOfLastSendingConn = index;

	if ( sent )
		{
		--iDataCredits;
		--(iAclConns[index].iPacketsQueued);
		++(iAclConns[index].iPacketsPending);
		LOG3(_L("\tsent on connection handle %d: iPacketsQueued = %d, iPacketsPending = %d"),
			connh, iAclConns[index].iPacketsQueued, iAclConns[index].iPacketsPending);
		}
	
	LOG1(_L("CACLDataQController::IssueNextACLDataFragment sent = %d"), sent);
	return sent;
	}

TBool CACLDataQController::SendItem(CACLDataItem& aItem) 
/**
	Actually do the send - return true if successful
**/
	{
	LOG_FUNC

	TBool ret = EFalse;
	__ASSERT_DEBUG(aItem.Frame(), Panic(EHCIBufferMgrBadState));
	if ( iHCIFacade.WriteACLData(*aItem.Frame()) == KErrNone )
		{
		__ASSERT_DEBUG(iDataQ, Panic(EHCIBufferMgrBadState));
		iDataQ->PendingItem(aItem);
		ret = ETrue;
		}
	
	LOG1(_L("CACLDataQController::SendItem ret = %d"), ret);
	return ret;
	}

void CACLDataQController::GetBufferInfo(TUint16& aBufSize, TUint& aNumBufs)
/**
	Retrieves information about what the capabilities of the Data Q are.
	This information will be used in order to inform the HCI client about
	our MTU and maximum buffers so flow control can be possible.
*/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iDataQ, Panic(EHCIBufferMgrBadState));
	aBufSize = iDataQ->ItemSize();
	aNumBufs = iDataQ->Ceiling();

	LOG2(_L("CACLDataQController::GetBufferInfo aBufSize = %d, aNumBufs = %d"), 
		aBufSize, aNumBufs);
	}

void CACLDataQController::GetDataQRecords(TUint& aQFillLevel, TUint16& aCredits)
/**
	Returns the number of things ready to send, and the number of credits we 
	have to send
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iDataQ, Panic(EHCIBufferMgrBadState));
	aQFillLevel = iDataQ->FillLevel(); // items in the Q
	aCredits = iDataCredits;
	LOG2(_L("LL: CACLDataQController: Model: FillLevel %d, FillCeiling %d"),
		aQFillLevel, iDataQ->Ceiling());

	LOG2(_L("CACLDataQController::GetDataQRecords aQFillLevel = %d, aCredits = %d"), 
		aQFillLevel, aCredits);
	}

void CACLDataQController::FlushOccurred(THCIConnHandle /*aConnH*/)
	{
	LOG_FUNC
	// do nothing
	// we will receive a num_completed_packets event
	}

TInt CACLDataQController::ACLLogicalLinkUp(THCIConnHandle aConnH, TBool aIsParked)
/**
	This object cares more about *ACL* links
	Whether or not ACL comes up for free with physical channel, it
	is worth drawing the distinction

	If an error occurs we tell the caller - it's likely the ACL will need to 
	be torn down
**/
	{
	LOG_FUNC
	LOG2(_L("CACLDataQController::ACLLogicalLinkUp aConnH = %d, aIsParked = %d"), 
		aConnH, aIsParked);

	TDataQConnectionInfo tmpConnRecord;
	tmpConnRecord.iConnH = aConnH;
	tmpConnRecord.iPacketsQueued = 0;
	tmpConnRecord.iPacketsPending = 0;
	tmpConnRecord.iParked = aIsParked;

	TInt err = iAclConns.Append(tmpConnRecord);

	// We don't need to update iIndexOfLastSendingConn because the new 
	// connection will have been added 'after' its current position.

	LOG1(_L("CACLDataQController::ACLLogicalLinkUp err = %d"), err);
	return err;
	}

void CACLDataQController::ACLLogicalLinkDown(THCIConnHandle aConnH)
/**
	Updates the Q.
 */
	{
	LOG_FUNC
	LOG1(_L("CACLDataQController::ACLLogicalLinkDown aConnH = %d"), 
		aConnH);

#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
	if(iMBufPool)
		{
		iMBufPool->InvalidateByConnH(aConnH);
		// the packet completions should probably move to the iAclConns model
		// to clean up this code.
		}
#endif // HOSTCONTROLLER_TO_HOST_FLOW_CONTROL

	TInt connection = FindConnection(aConnH);
		
	if ( connection == KErrNotFound )
		{
		LOG(_L("CACLDataQController::ACLLogicalLinkDown (Connection not found)."));
		return;
		}

	const TUint index = static_cast<TUint>(connection);

	const TUint16 dropped = iAclConns[index].iPacketsPending;
	LOG1(_L("\treusing %d credits"), dropped);

	// Increase iDataCredits by how many packets this link had on the air (we 
	// won't receive a num_completed_packets event to do it the normal way)
	iDataCredits = static_cast<TUint16>(iDataCredits + dropped);

	__ASSERT_DEBUG(iDataQ, Panic(EHCIBufferMgrBadState));
	iDataQ->InvalidateByConnH(aConnH, iHCIFacade);
	iAclConns.Remove(index);

	// The array of connections has changed- if the removed connection is 
	// 'after' iIndexOfLastSendingConn, we don't need to bother changing it. 
	// If the removed connection IS the connection that sent last, reset it to 
	// zero. If the removed connection is before the one that sent last, just 
	// decrement iIndexOfLastSendingConn. 
	if ( index == iIndexOfLastSendingConn )
		{
		iIndexOfLastSendingConn = 0;
		}
	else if ( index < iIndexOfLastSendingConn )
		{
		--iIndexOfLastSendingConn;
		}

	__ASSERT_DEBUG(iDataCredits<=iNumControllerBufs, Panic(EHCIACLDataControllerMaxAllowedDataCreditsExceeded));
	}

void CACLDataQController::SetParked(THCIConnHandle aConnH, TBool aParked)
	{
	LOG_FUNC
	LOG2(_L("CACLDataQController::SetParked aConnH = %d, aParked = %d"), 
		aConnH, aParked);

	// Check if there are any data connections.
	if ( iAclConns.Count() > 1 )
		{
		const TInt index = FindConnection(aConnH);
		if(index != KErrNotFound)
			{
			iAclConns[index].iParked = aParked;
			}
		else
			{
			LOG(_L("\tConnection handle not found."));
			}
		}
	else
		{
		LOG(_L("\tcurrently no ACL connections"));
		}

	if ( aParked == EFalse )
		{
		// kick off draining of queue
		iLinkMuxer.TryToSend();
		}
	}

void CACLDataQController::CompletedPackets(THCIConnHandle aConnH, TUint16 aNo)
/**
	Called when some buffer slots become free in our HC (a 
	NumberOfCompletedPackets event)- in other words, we can now send aNo more 
	packets down if we wish.
 */
	{
	LOG_FUNC
	LOG2(_L("CACLDataQController::CompletedPackets aConnH = %d, aNo = %d"), 
		aConnH, aNo);

	// Adjust the connection that's just had packets put on the air
	const TInt index = FindConnection(aConnH);

	//We have to check pending packets because it is possible this completed packets event comes 
	//from a previous link and current link reuses the same connection handle
	if (index!=KErrNotFound && iAclConns[index].iPacketsPending > 0)
		{
		iAclConns[index].iPacketsPending = static_cast<TUint16>(iAclConns[index].iPacketsPending - aNo);
		
		LOG4(_L("\t%d completed packets for connection handle %d: iPacketsQueued = %d, iPacketsPending = %d"),
			aNo, aConnH, iAclConns[index].iPacketsQueued, iAclConns[index].iPacketsPending);

		iDataCredits = static_cast<TUint16>(iDataCredits + aNo);
		__ASSERT_DEBUG(iDataCredits<=iNumControllerBufs, Panic(EHCIACLDataControllerMaxAllowedDataCreditsExceeded));

		LOG3(_L("LinkMgr: CACLDataQController: Got %d DataCredits on ConnH 0x%04x: total %d DataCredts"), 
			aNo, aConnH, iDataCredits);
		LOG2(_L("LL: CACLDataQController: Got %d DataCredits; total %d DataCredts"),
			aNo,iDataCredits);
			
		// Notify sent items.
		iDataQ->ItemsSent(aNo);
		
		// now we have credits - have a go at using them
		iLinkMuxer.TryToSend();
		}
	else
		{
		LOG1(_L("LinkMgr: CACLDataQController: Got Completed Packets on lost ConnH(%d) [dropped?]"), aConnH);
		//The Hardware has sent num_completed_packets for a ConnH which we don't know about,
		//if this was a race with the connection being dropped we will already have reused the
		//credits.
		}
	}

#pragma warning (default:4244)// "conversion from 'int' to 'unsigned short', possible loss of data"

TInt CACLDataQController::FindConnection(THCIConnHandle aConnH)
/**
 Returns the index (in iAclConns) of the connection record with THCIConnHandle 
 aConnH, or error.
 */
	{
	LOG_FUNC
	LOG2(_L("CACLDataQController::FindConnection aConnH = %d, iAclConns.Count() = %d"), 
		aConnH, iAclConns.Count());

	TDataQConnectionInfo tmpInfo;
	tmpInfo.iConnH = aConnH;

	TIdentityRelation<TDataQConnectionInfo> relation(LinkMatch);

	const TInt position = iAclConns.Find(tmpInfo, relation);

	LOG1(_L("CACLDataQController::FindConnection position = %d"), position);
	return position;
	}

/*static*/ TBool CACLDataQController::LinkMatch(const TDataQConnectionInfo& aA,
									 const TDataQConnectionInfo& aB)
	{
	LOG_STATIC_FUNC
	return (aA.iConnH == aB.iConnH);
	}

void CACLDataQController::FlushComplete(TInt __DEBUG_ONLY(aErr), THCIConnHandle aConnH)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aErr == KErrNone || aErr == ECommandDisallowed, Panic(EHCIACLDataControllerFlushCompleteError));
		 
	TInt ix = FindConnection(aConnH);
	__ASSERT_DEBUG(ix != KErrNotFound, Panic(EHCIACLDataControllerFlushCompleteError));
	if(ix != KErrNotFound)
		{
		__ASSERT_DEBUG(iAclConns[ix].iFlushInProgress, Panic(EHCIACLDataControllerFlushCompleteError));
		iAclConns[ix].iFlushInProgress = EFalse;

		iLinkMuxer.TryToSend();
		}
	}
	
	
TInt CACLDataQController::SetFlushInProgress(THCIConnHandle aConnH)
	{
	LOG_FUNC
	// Check a flush is not currently outstanding against this
	// Connection handle.
	TInt rerr = KErrNone;
	TInt ix = FindConnection(aConnH);
	if(ix != KErrNotFound)
		{
		if(!iAclConns[ix].iFlushInProgress)
			{
			TRAP(rerr, iHCIFacade.FlushL(aConnH));
			if(rerr == KErrNone)
				{
				iAclConns[ix].iFlushInProgress = ETrue;
				}
			}
			
		if(rerr == KErrNone)
			{
			iDataQ->ProcessFlush(iHCIFacade, aConnH);
			}
		}
	else
		{
		rerr = ix;
		}

	return rerr;
	}

RMBufChain CACLDataQController::PopulateInboundBufferL(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData)
	{
	LOG_FUNC
	// make a new chain consisting of Flag(1st octet) followed by Data.
	RMBufChain aclData;
	static const TInt KFlagHeaderOffset = 0;
	
#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
	// Check what flow control mode is in operation
	TFlowControlMode flowControl = iLinkMuxer.FlowControlMode();
	TBool ctrlerToHost = (flowControl == ETwoWayFlowControlEnabled) || (flowControl == EFlowControlFromHostControllerOnly);
	if(ctrlerToHost)
		{
		__ASSERT_DEBUG(iMBufPool, Panic(ELinkMgrFlowControlChangeOfMind));
		aclData = iMBufPool->TakeBufferL(aConnH);
		aclData.CopyIn(aData, KLinkMgrIncomingBufferHeaderSize);
		// return the reserved MBufs we didn't need to the global pool
		aclData.TrimEnd(aData.Length() + KLinkMgrIncomingBufferHeaderSize);
		}
	else
#endif // HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
		{
		aclData.CreateL(aData, KLinkMgrIncomingBufferHeaderSize);
		}
	
	aclData.First()->Ptr()[KFlagHeaderOffset] = aFlag;
	
	return aclData;
	}

void CACLDataQController::NoExplicitInboundPoolNeeded()
	{
	LOG_FUNC
#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
	delete iMBufPool;
	iMBufPool = NULL;
#endif // HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
	}

//
// End of file

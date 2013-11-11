// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Link Autonomous ACL data packet Queue class implementation.
// This autonomous Q pre-allocates a number of CHCIACLPacketBuffers.
// It is at this point where the single pool of buffers in the HC is modelled 
// SW-side.
// This is the alternative chosen over having each baseband SAP have its own 
// Queue.
// 
//

#include <bluetooth/logger.h>
#include "AclDataQ.h"
#include "linkqitem.h"
#include "linkmgr.h"
#include "hcifacade.h"
#include "L2CapPDU.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

/**
	Instantiates a new ACL Data Q.

	If this is called with zero number of items, it emulates a 'NULL' pattern 
	for the data Q. This is necessary on init when we don't yet know what size 
	of packet buffers we're going to use.
*/
CAclDataQ* CAclDataQ::NewL(CLinkMgrProtocol& aProtocol, 
						   TUint aNumberOfItems, 
						   TUint16 aDataSize, 
						   TUint16 aFramingOverhead)
	{
	LOG_STATIC_FUNC
	LOG3(_L("CAclDataQ::NewL aNumberOfItems = %d, aDataSize = %d, aFramingOverhead = %d"),
		aNumberOfItems, aDataSize, aFramingOverhead);

	CAclDataQ* self = new(ELeave) CAclDataQ;
	CleanupStack::PushL(self);
	self->ConstructL(aProtocol, aNumberOfItems, aDataSize, aFramingOverhead);
	CleanupStack::Pop(self);

	LOG1(_L("CAclDataQ::NewL self = 0x%08x"), self);
	return self;
	}

CAclDataQ::CAclDataQ()
 : iPreHardwareBuffer(_FOFF(CACLDataItem,iLink)), 
   iSpareDataPool(_FOFF(CACLDataItem,iLink)),
   iPendingData(_FOFF(CACLDataItem,iLink))
	{
	LOG_FUNC
	}

CAclDataQ::~CAclDataQ() 
	{
	LOG_FUNC
	// delete the two Qs
	CACLDataItem* tmpItem;

	while ( ( tmpItem = RemoveFirstSpareItem() ) != NULL )
		{
		delete tmpItem;
		}

	while(!iPreHardwareBuffer.IsEmpty())
		{
		tmpItem=iPreHardwareBuffer.First();
		iPreHardwareBuffer.Remove(*tmpItem);
		delete tmpItem;
		}
	}

/**
	Pre-allocates all the Packet Buffer space as a pool.
	Client of this function decides the number of items - this function does 
	not provide any less than that, even in low memory conditions.
*/
void CAclDataQ::ConstructL(CLinkMgrProtocol& aProtocol, 
						   TUint aNumberOfItems, 
						   TUint16 aDataSize, 
						   TUint16 aFramingOverhead)
	{
	LOG_FUNC

	for ( TUint allocIdx = 0 ; allocIdx < aNumberOfItems ; allocIdx++ )
		{
		CACLDataItem* tmpPacketBuf = CACLDataItem::NewL();
		CleanupStack::PushL(tmpPacketBuf);

		tmpPacketBuf->Frame() = 
			aProtocol.HCIFacade().NewACLDataFrameL(static_cast<TUint16>(aDataSize+aFramingOverhead));
	
		CleanupStack::Pop(tmpPacketBuf);
		iSpareDataPool.AddLast(*tmpPacketBuf);
		}
	
	// we managed to allocate as much as we were asked to
	iCeiling = aNumberOfItems;
	iItemSize = aDataSize;
	}

/**
	Takes the first item off the spare item pool and returns ownership of it 
	to the caller, or NULL if there are no spare items.
*/
CACLDataItem* CAclDataQ::RemoveFirstSpareItem()
	{
	LOG_FUNC

	CACLDataItem* ret = NULL;
	if ( !iSpareDataPool.IsEmpty() )
		{
		ret = iSpareDataPool.First();
		iSpareDataPool.Remove(*ret);
		}

	LOG1(_L("CAclDataQ::RemoveFirstSpareItem ret = 0x%08x"), ret);
	return ret;
	}

void CAclDataQ::AddItem(CACLDataItem& aAclItem)
/** 
	Place the given item on the outbound Q. Takes ownership of it.
*/
	{
	LOG_FUNC
	LOG1(_L("CAclDataQ::AddItem &aAclItem = 0x%08x"), &aAclItem);

	iLevel++;
	__ASSERT_DEBUG(iLevel<=iCeiling,Panic(ELinkMgrFIFOLevelAboveCeiling));

	// Add to the outbound Q - when sent move to end of spare Q
	iPreHardwareBuffer.AddLast(aAclItem);
	}

/**
	Moves all Packet Buffers on the outbound Q, belonging to this connection, 
	to the spare item pool.
	Updates the iLevel of the Q.
*/
void CAclDataQ::InvalidateByConnH(THCIConnHandle aConnH, const CHCIFacade& /*aHCIFacade*/)
	{
	LOG_FUNC
	LOG1(_L("CAclDataQ::InvalidateByConnH aConnH = %d"), aConnH);

	CACLDataItem* item = 0;
	// Check both the pending and pre-hardware queues for frames
	// on the connection handle.  Inform the fragment sender that
	// the element has been sent-  this will ensure that all
	// resources are cleaned up, and no re-sends are attempted.
	TSglQueIter<CACLDataItem> pendingDataIter(iPendingData);
	while(pendingDataIter)
		{
		item = pendingDataIter++;
		if((*(item->Frame())).ConnectionHandle() == aConnH)
			{
			iPendingData.Remove(*item);
			TDataPlaneElementHandle* elementHandle = item->ElementHandle();
			elementHandle->DataPlaneElement().DataElementSent(elementHandle->ElementID());
			iSpareDataPool.AddLast(*item);
			}
		}			

	TSglQueIter<CACLDataItem> preHardwareIter(iPreHardwareBuffer);
	while(preHardwareIter)
		{
		item = preHardwareIter++;
		if((*(item->Frame())).ConnectionHandle() == aConnH)
			{
			iPreHardwareBuffer.Remove(*item);
			iLevel--;
			TDataPlaneElementHandle* elementHandle = item->ElementHandle();
			elementHandle->DataPlaneElement().DataElementSent(elementHandle->ElementID());
			iSpareDataPool.AddLast(*item);
			}
		}
	LOG1(_L("CAclDataQ::InvalidateByConnH iLevel = %d"), iLevel);
	}

void CAclDataQ::PendingItem(CACLDataItem& aItem)
/**
	An item has been sent to the host controller so move it from the PreHardware queue
	to the pending sent notification queue.
*/
	{
	LOG_FUNC
	LOG1(_L("CAclDataQ::InvalidateItem &aItem = 0x%08x"), &aItem);

	iPreHardwareBuffer.Remove(aItem);
	iPendingData.AddLast(aItem);
	iLevel--;
	 
	// if it went below zero it would become too big (because unsigned)
	__ASSERT_DEBUG(iLevel<iCeiling,Panic(EHCIFIFOLevelBelowZero));
	}

void CAclDataQ::ItemsSent(TUint aNumberOfItemsSent)
	{
	LOG_FUNC
	CACLDataItem* item = 0;
	// If a flush has occurred then a number of completed packets event can
	// be received when no data is in the pending queue.
	for(TUint i=0;i<aNumberOfItemsSent;i++)
		{
		if(!iPendingData.IsEmpty())
			{
			item = iPendingData.First();

			TDataPlaneElementHandle* elementHandle = item->ElementHandle();
			elementHandle->DataPlaneElement().DataElementSent(elementHandle->ElementID());

			iPendingData.Remove(*item);
			iSpareDataPool.AddLast(*item);
			}
		}
	}

void CAclDataQ::ProcessFlush(const CHCIFacade& /*aHCIFacade*/, THCIConnHandle aConnH)
	{
	LOG_FUNC
	CACLDataItem* item = 0;

	// The order of clearing flushed items from the queues must be
	// maintained.  The nearest to being sent must be reported first so
	// that if the PDU is to be re-sent it is placed at the start of
	// the re-sent queue. 
	TSglQueIter<CACLDataItem> pendingDataIter(iPendingData);
	while(pendingDataIter)
		{
		item = pendingDataIter++;
		if((*(item->Frame())).ConnectionHandle() == aConnH)
			{
			iPendingData.Remove(*item);
			TDataPlaneElementHandle* elementHandle = item->ElementHandle();
			elementHandle->DataPlaneElement().DataElementFlushed(elementHandle->ElementID());
			iSpareDataPool.AddLast(*item);
			}
		}			

	TSglQueIter<CACLDataItem> preHardwareIter(iPreHardwareBuffer);
	while(preHardwareIter)
		{
		item = preHardwareIter++;
		if((*(item->Frame())).ConnectionHandle() == aConnH)
			{
			iPreHardwareBuffer.Remove(*item);
			iLevel--;
			TDataPlaneElementHandle* elementHandle = item->ElementHandle();
			elementHandle->DataPlaneElement().DataElementFlushed(elementHandle->ElementID());
			iSpareDataPool.AddLast(*item);
			}
		}
	}


CACLDataItem* CAclDataQ::FirstItemByConnectionHandle(const CHCIFacade& /*aHCIFacade*/,
												   const THCIConnHandle aConnH)
/**
	Returns first packet on the data fifo on a given connection handle
*/
	{
	LOG_FUNC
	LOG1(_L("CAclDataQ::FirstItemByConnectionHandle aConnH = %d"), aConnH);

	CACLDataItem* item = NULL;
	TSglQueIter<CACLDataItem> iter(iPreHardwareBuffer);

	FOREVER
		{
		item = iter++;

		if ( item == NULL ) // ended the iterator
			{
			break;
			}

		if ((*(item->Frame())).ConnectionHandle() == aConnH)
			{
			break;
			}
		}
	LOG1(_L("CAclDataQ::FirstItemByConnectionHandle item = 0x%08x"), item);
	return item;
	}

/**
 Returns the first item on the outbound queue and sets aConnH to its 
 connection handle.
 */
CACLDataItem* CAclDataQ::FirstItem(const CHCIFacade& /*aHCIFacade*/,
								   THCIConnHandle& aConnH)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iLevel>0,Panic(ELinkMgrFIFOLevelZeroOrBelowZero));

	CACLDataItem* item = iPreHardwareBuffer.First();
	aConnH = (*(item->Frame())).ConnectionHandle();

	LOG2(_L("CAclDataQ::FirstItem aConnH = %d, item = 0x%08x"), aConnH, item);
	return item;
	}

/**
	Resets data items onto spare Q and sets the fill level to zero.
	Hence it acts like the buffer is empty, thus we can utilise it again in 
	full.
	This call must be made from the Data Controller ONLY when a Reset occurs.
	
	Note: It does not zero the contents of the buffers.
*/
void CAclDataQ::InvalidateAll()
	{
	LOG_FUNC

	CACLDataItem* item = 0;
	// Check both the pending and pre-hardware queues for frames
	// on the connection handle.  Inform the fragment sender that
	// the element has been sent-  this will ensure that all
	// resources are cleaned up, and no re-sends are attempted.
	TSglQueIter<CACLDataItem> pendingDataIter(iPendingData);
	while(pendingDataIter)
		{
		item = pendingDataIter++;
		iPendingData.Remove(*item);
		TDataPlaneElementHandle* elementHandle = item->ElementHandle();
		elementHandle->DataPlaneElement().DataElementSent(elementHandle->ElementID());
		iSpareDataPool.AddLast(*item);
		}			

	TSglQueIter<CACLDataItem> preHardwareIter(iPreHardwareBuffer);
	while(preHardwareIter)
		{
		item = preHardwareIter++;
		iPreHardwareBuffer.Remove(*item);
		TDataPlaneElementHandle* elementHandle = item->ElementHandle();
		elementHandle->DataPlaneElement().DataElementSent(elementHandle->ElementID());
		iSpareDataPool.AddLast(*item);
		}
	iLevel=0;

	iLevel = 0;
	}

//
// End of file

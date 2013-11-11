// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _ACLDATAQ_H
#define _ACLDATAQ_H

#include <e32base.h>
#include <bttypes.h>

class CACLDataItem;
class CHCIFacade;
class CLinkMgrProtocol;

/**
	HCI ACL Data Packet Buffer Q.

	This class will handle the Qing and issuing of ACL Data Packet Buffers.
	It pre-allocates Packet buffers on the heap on construction, hence the 
	number or size of preallocated buffers should not change after 
	construction because ESock doesn't like memory Leaves.

	For a UART HCTL implementation of the HCI, we need to be able to buffer 
	HCI Data buffers of at least 255+1+4 bytes, since the HC must guarantee to 
	be able to accept/buffer data packets of 255 bytes (EXCLUDING the headers).

	The general usage includes:
	1/ Obtaining items in the 'spare data pool'- for the client to populate 
	with data (RemoveFirstSpareItem),
	2/ Putting such items back on the 'data fifo' queue, for sending 
	(AddItem), and
	3/ When we do a send, getting an item to send (FirstItemByConnectionHandle 
	and FirstItem) and moving it back to the spare data pool (InvalidateItem).

	This class is not intended for derivation, to reuse aggregate.
*/
NONSHARABLE_CLASS(CAclDataQ) : public CBase
	{
public:
	static CAclDataQ* NewL(CLinkMgrProtocol& aProtocol, 
		TUint aNumberOfItems, 
		TUint16 aDataSize, 
		TUint16 aFramingOverhead);
	~CAclDataQ();

public:
	inline TUint	Ceiling() const;
	inline TUint	FillLevel() const;
	void			AddItem(CACLDataItem& aACLItem);
	inline TUint16	ItemSize() const;
	void			InvalidateAll();
	void			InvalidateByConnH(THCIConnHandle aConn, const CHCIFacade& aHCIFacade);
	CACLDataItem*	RemoveFirstSpareItem();
	CACLDataItem*	FirstItemByConnectionHandle(const CHCIFacade& aHCIFacade, const THCIConnHandle aConnH);
	CACLDataItem*	FirstItem(const CHCIFacade& aHCIFacade, THCIConnHandle& aConnH);
	void			PendingItem(CACLDataItem& aItem);
	void 			ItemsSent(TUint aNumberOfItemsSent);
	void 			ProcessFlush(const CHCIFacade& aHCIFacade, THCIConnHandle aConnH);
	
private:
	CAclDataQ();
	void ConstructL(CLinkMgrProtocol& aProtocol, 
		TUint aNumberOfItems, 
		TUint16 aDataSize, 
		TUint16 aFramingOverhead);

private: // owned
	// size of each item (HCI ACL Data buffer) in the FIFO
	TUint16 iItemSize;

	// max number of allocated items allowed
	TUint iCeiling;

	// how many packets are pending on the Q (iDataFifo)
	TUint iLevel;

	TSglQue<CACLDataItem>	iPreHardwareBuffer;	// A buffer for data prior to it being 
	                                            // sent to the hardware buffer.
	TSglQue<CACLDataItem>	iSpareDataPool;		// the 'empty' slots
	TSglQue<CACLDataItem>	iPendingData;		// Data on the hardware.
	};

inline TUint CAclDataQ::Ceiling() const
/**
	Returns the maximum number of packets this Q will hold.
*/
	{
	return iCeiling;
	}

inline TUint CAclDataQ::FillLevel() const
/**
	Returns the number of packets currently on the Q.
*/
	{
	return iLevel;
	}

inline TUint16 CAclDataQ::ItemSize() const
/**
	Returns the data size of the packets that can be handled by this Q.
*/
	{
	return iItemSize;
	}

#endif // _ACLDATAQ_H

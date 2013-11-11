// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MBuf Manager Extensions
//

/**
@file

MBuf Manager Extensions
*/


#ifndef __NIFMBUF_H__
#define __NIFMBUF_H__

#ifndef __ES_MBUF_H__
#include <es_mbuf.h>
#endif

#ifndef __ES_SOCK_H__
#include <es_sock.h>
#endif


//
// Support to use single MBufs for containing C++ objects
//

class RMBufCell
/**
Support to use single MBufs for containing C++ objects

@publishedPartner
@released
*/
	{
public:
	IMPORT_C static TAny* operator new(TUint aSize, TLeave aLeave, TUint aExtra);
	IMPORT_C static TAny* operator new(TUint aSize, TUint aExtra) __NO_THROW;
	IMPORT_C static void operator delete(TAny*);
	static void operator delete(TAny*, TLeave, TUint);
	static void operator delete(TAny*, TUint);
	IMPORT_C RMBufCell* CopyL();
	//
	inline RMBufCell() {}	// to prevent statement has no effect
	static inline TAny* operator new(TUint aSize, TLeave aLeave)
		{ return RMBufCell::operator new (aSize, aLeave, 0); }
	static inline TAny* operator new(TUint aSize)
		{ return RMBufCell::operator new (aSize, 0); }
	inline RMBuf* MBuf();
	//
private:
	RMBuf* iThisMBuf;
	};

inline RMBuf* RMBufCell::MBuf()
	{ return iThisMBuf; }

class RMBufStreamQ : public RMBufQ
/**
MBuf Stream Queue
@publishedPartner
@released
*/
	{
public:
	IMPORT_C RMBufStreamQ();
	IMPORT_C void TrimStart(TInt aOffset);
	IMPORT_C void CopyOut(TDes8& aDes);
	IMPORT_C void CopySegmentL(RMBufChain &aNewChain, TInt aOffset, TInt aLen);
	IMPORT_C void Append(RMBufChain& aPacket);
protected:
	inline RMBufChain& AsRMBufChain() { return *((RMBufChain*)this); }
protected:
	// Pointers cache
	TInt iPosition;
	RMBuf* iMBuf;
	TInt iOffset;
	TInt iLength;		
	};

//
// Inter-Protocol Information carried in header MBuf
//

class RMBufPktInfo : public RMBufCell
/**
Inter-Protocol Information carried in header MBuf

@publishedPartner
@released
*/
	{
public:
	TSockAddr iSrcAddr;	//< Source address of packet according to iProtocol
	TSockAddr iDstAddr;	//< Destination address of packet according to protocol
	TInt iProtocol;		//< ID of protocol who is giving you this
	TInt iLength;		//< Actual length of packet
	TUint iFlags;		//< Packet flags
	};

//
// Holder for MBuf Chain with inter-protocol header split off
//

class RMBufPacketBase : public RMBufChain
/**
@publishedPartner
@released
*/
	{
	__DECLARE_CLEANUP
public:
	IMPORT_C RMBufPacketBase();
	IMPORT_C void CopyInfoL(RMBufPacketBase& aNewPkt);
	IMPORT_C void CopyPackedL(RMBufChain& aPacket);
	IMPORT_C void Pack();
	IMPORT_C void SetInfo(RMBufPktInfo* iInfo);
	IMPORT_C void FreeInfo();
	IMPORT_C void Free();
	IMPORT_C void TrimStart(TInt aOffset);
	IMPORT_C void TrimEnd(TInt aOffset);
	IMPORT_C RMBufPktInfo* Unpack();
	IMPORT_C RMBufPktInfo* Info();
	IMPORT_C RMBufPktInfo* PeekInfo();
	IMPORT_C RMBufPktInfo* NewInfoL(TUint aMBufHdrSize);
	IMPORT_C RMBufPktInfo* NewInfo(TUint aMBufHdrSize);
	IMPORT_C static RMBufPktInfo* PeekInfoInChain(RMBufChain& aChain);
protected:
	IMPORT_C RMBufPktInfo* CreateL(RMBufQ& aPacket, TInt aLength, TUint aMBufHdrSize);
	IMPORT_C RMBufPktInfo* CreateL(TInt aLength, TUint aMBufHdrSize);
	IMPORT_C RMBufPktInfo* CreateL(const TDesC8 &aDesc, TInt aHdrSize, TUint aMBufHdrSize);
	IMPORT_C RMBufPktInfo* CreateCopyL(const RMBufQ& aPacket, TInt aOffset, TInt aLength, TInt aHdrSize, TUint aMBufHdrSize);
	IMPORT_C RMBufPktInfo* CreateCopyL(RMBufStreamQ& aStream, TInt aOffset, TInt aLength, TInt aHdrSize, TUint aMBufHdrSize);
	IMPORT_C RMBufPktInfo* CreateCopyL(const RMBufPktQ& aList, TInt aHdrSize, TUint aMBufHdrSize);
protected:
	RMBufPktInfo* iInfo;
	};

template <class T>
class RMBufInfoPacketBase : public RMBufPacketBase
/**
@publishedPartner
@released
*/
	{
public:
    /**
 	Splits off the info header
 	@return the packet info header
 	*/
 	inline T* Unpack()
		{ return (T*)RMBufPacketBase::Unpack(); }
	/**
    Return pointer to info header
    @return the info header
    */
	inline T* Info()
		{ return (T*)RMBufPacketBase::Info(); }
	/**
    Allows the packet info header to be accessed without unpacking
	@return the packet info header
	*/
	inline T* PeekInfo()
		{ return (T*)RMBufPacketBase::PeekInfo(); }
	/**
	Allows the packet info header to be accessed while still on a packet queue
	@param aChain the chain to be accessed
	@return the info header
	*/
	inline static T* PeekInfoInChain(RMBufChain& aChain)
		{ return (T*)RMBufPacketBase::PeekInfoInChain(aChain); }
	/**
	Create an info header 
	@return the info header
	*/
	inline T* NewInfoL()
		{ return (T*)RMBufPacketBase::NewInfoL(sizeof(T)); }
	/**
	Create an info header 
	@return the info header
	*/
	inline T* NewInfo()
		{ return (T*)RMBufPacketBase::NewInfo(sizeof(T)); }
	/**
 	Create an empty Class T chain of required header size
 	@param aHdrSize the header size
 	@return the info header
 	*/	
	inline T* CreateL(TInt aHdrSize)
		{ return (T*)RMBufPacketBase::CreateL(aHdrSize, sizeof(T)); };
	/**
 	Create a Class T chain and header from a descriptor
 	@param aDesc the buffer of data
 	@param aHdrSize the header size
 	@return the info header
 	*/
	inline T* CreateL(const TDesC8 &aDesc, TInt aHdrSize=0)
		{ return (T*)RMBufPacketBase::CreateL(aDesc, aHdrSize, sizeof(T)); };
	/**
 	Create a Class T chain from an received MBuf Queue
 	@param aPacket the MBuf Queue
 	@param aLength the chain length
 	@return the info header
 	*/
	inline T* CreateL(RMBufQ& aPacket, TInt aLength=0)
		{ return (T*)RMBufPacketBase::CreateL(aPacket, aLength, sizeof(T)); };
	/**
 	Create a Class T chain from an received MBuf Queue
 	@param aPacket the MBuf Queue
 	@param aOffset the offset 
 	@param aLength the length of the data to be copied
 	@param aHdrSize the header size
 	@param aMBufHdrSize the MBuf Header Size
 	@return the info header
 	*/
	inline T* CreateCopyL(const RMBufQ& aPacket, TInt aOffset, TInt aLength, TInt aHdrSize=0)
		{ return (T*)RMBufPacketBase::CreateCopyL(aPacket, aOffset, aLength, aHdrSize, sizeof(T)); };
	/**
  	Create a Class T chain by copying data from a stream queue (basically an RMBufQ)
  	aOffset and aLength specify location in stream queue.
 	@param aStream the stream queue
 	@param aOffset the offset
 	@param aLength the data length
 	@param aHdrSize the header size
 	@return the info header
 	*/	
	inline T* CreateCopyL(RMBufStreamQ& aStream, TInt aOffset, TInt aLength, TInt aHdrSize=0)
		{ return (T*)RMBufPacketBase::CreateCopyL(aStream, aOffset, aLength, aHdrSize, sizeof(T)); };
	/**
	Build a Class T chain by copying a packet queue list, leaving space on the front
	for a header. (Very heavily used in PPP option processing).
	@param aList the packet queue
	@param aHdrSize the header size
	@return the info header
	*/
	inline T* CreateCopyL(const RMBufPktQ& aList, TInt aHdrSize=0)
		{ return (T*)RMBufPacketBase::CreateCopyL(aList, aHdrSize, sizeof(T)); };
	};

/**
@publishedPartner
@released
*/
typedef class RMBufInfoPacketBase<RMBufPktInfo> RMBufPacket;

enum TMBufExtPanic
	{
	EMbExtPanic_MBufAllocTooBig,
	EMbExtPanic_HeaderTooBig,
	EMbExtPanic_PacketHeader,
	EMbExtPanic_PacketHeaderEmpty
	};

void MBufExtPanic(TMBufExtPanic aPanic);

#endif

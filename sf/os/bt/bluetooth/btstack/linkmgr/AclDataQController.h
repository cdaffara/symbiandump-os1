// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _ACLDATAQCONTROLLER_H
#define _ACLDATAQCONTROLLER_H

#include <e32base.h>
#include <bttypes.h>
#include <es_mbuf.h>

class CAclDataQ;
struct TDataQConnectionInfo;
class CACLDataItem;
class CLinkMgrProtocol;
class CLinkMuxer;
class CHCIFacade;
class CHostMBufPool;

/**
	Controller of the ACL data Q and of the pending packet list.
	This object holds in effect the model of how the HC operates its data 
	pool, knows when ACL data are ready to be send-off for transmission and 
	accounts for the ones pending transmission of the air interface.
*/
NONSHARABLE_CLASS(CACLDataQController) : public CBase
	{
public:
	static CACLDataQController* NewL(CLinkMgrProtocol& aProtocol,
		CLinkMuxer& aMuxer,
		TUint16 aBufSize,
		TUint16 aFrameOverhead,
		TUint aNumBufs);
	~CACLDataQController();

public:
	// Outbound aspects
	void InitialDataCredits(TUint16 aCredits);
	void AddItem(CACLDataItem& aACLFrame);
	TBool IssueNextACLDataFragment();
	void GetBufferInfo(TUint16& aBufSize, TUint& aNumBufs);
	void GetDataQRecords(TUint& aQFillLevel, TUint16& aCredits);
	CACLDataItem* GetFreeItem();

	TInt SetFlushInProgress(THCIConnHandle aConnH);
	void FlushComplete(TInt aErr, THCIConnHandle aConnH);
	void FlushOccurred(THCIConnHandle aConnH); // TODO: not actually called by HCI facade
	TInt ACLLogicalLinkUp(THCIConnHandle aConnH, TBool aIsParked);
	void ACLLogicalLinkDown(THCIConnHandle aConnH);
	void SetParked(THCIConnHandle aConnH, TBool aParked);
	void CompletedPackets(THCIConnHandle aConnH, TUint16 aNo);
	
	// Inbound buffer related aspects
	RMBufChain PopulateInboundBufferL(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData);
	void NoExplicitInboundPoolNeeded();
	

private:
	CACLDataQController(CHCIFacade& aHCIFacade, CLinkMuxer& aMuxer);
	void ConstructL(CLinkMgrProtocol& aProtocol, TUint16 aBufSize, TUint16 aFrameOverhead, TUint aNumBufs);
	
	TInt FindConnection(THCIConnHandle aConnH);
	TBool AnotherPacketAllowed(TDataQConnectionInfo& aRecord);
	TBool SendItem(CACLDataItem& aItem);
	static TBool LinkMatch(const TDataQConnectionInfo& aA, const TDataQConnectionInfo& aB);
	THCIConnHandle HighestPriority();

private: // owned
	CAclDataQ*	iDataQ;
	
	// from HC, how many ACL packets we can send at any one time (varies 
	// according to how many are pending NumberOfCompletedPackets)
	TUint16		iDataCredits;

	RArray<TDataQConnectionInfo> iAclConns;

	// This is the index into iAclConns of the connection which last sent an 
	// item. This is used when finding the next link to allow to send, to 
	// implement fairer sharing of the link (otherwise connections near the 
	// start of the array get precedence over connections near the end).
	TUint iIndexOfLastSendingConn;

	TUint16     iNumControllerBufs;
	
#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
	CHostMBufPool* iMBufPool;
#endif

private: // unowned
	CLinkMuxer&	iLinkMuxer;
	CHCIFacade& iHCIFacade;
	};

struct TDataQConnectionInfo
	{
public:
	THCIConnHandle	iConnH;

	// Number of packets that have been put on the queue but not yet sent over 
	// the air.
	TUint16			iPacketsQueued;

	// Number of packets on the air but not yet reported sent. 
	// (0-iPacketsPending) is the connection's priority when working out which 
	// connection to send from next.
	TUint16			iPacketsPending;

	TBool			iParked;
	TBool			iFlushInProgress;
	};

#endif // _ACLDATAQCONTROLLER_H

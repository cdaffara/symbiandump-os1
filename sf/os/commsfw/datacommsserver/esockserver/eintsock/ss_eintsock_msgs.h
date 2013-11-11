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
// EIntSock_Msgs.H
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__EINTSOCK_MSGS_H__)
#define __EINTSOCK_MSGS_H__

#include <cfmsgs.h>
#include <cfshared.h>
#include <cfutil.h>
#include <comms-infras/ss_threadtransport.h>
#include "ss_msgs.h"
#include <es_sock.h>
#include <es_mbuf.h>

#include <comms-infras/sockmes.h>

using namespace CommsFW;

const TInt32 KEIntSockTransportPluginInterfaceUid = 0x10206013;
const TInt32 KEIntSockTransportPluginImplementationUid = 0x10206014;

const TInt KSoOpenByName = 512; // Note: There is no equaivalent opcode for this in sockmes.h


class CInternalSocketImpl;

struct TEIntSockOpMsgWrapper
	{
	public:
		inline TEIntSockOpMsgWrapper();
		
		template < class T0 >
		inline TEIntSockOpMsgWrapper ( T0 a0 );
		template < class T0, class T1 >
		inline TEIntSockOpMsgWrapper ( T0 a0, T1 a1 );
		template < class T0, class T1, class T2 >
		inline TEIntSockOpMsgWrapper ( T0 a0, T1 a1, T2 a2 );
		template < class T0, class T1, class T2, class T3 >
		inline TEIntSockOpMsgWrapper ( T0 a0, T1 a1, T2 a2, T3 a3 );

		
		inline TInt ReadDes(TInt aIndex, TDes8& aDes, TInt aOffset) const;
		inline TInt ReadBufChain ( TInt aIndex, RMBufChain& aBufChain ) const;
		inline TInt ReadInt(TInt aIndex) const;
		inline void InitBufChain(TInt aIndex);
		TInt WriteDesc(TInt aIndex, const TDesC8& aDesc, TInt aOffset);
		inline TInt WriteBufChain(TInt aIndex, RMBufChain& aBufChain);
		inline void WriteInt ( TInt aIndex, TInt aValue );
		inline TInt GetDesLength ( TInt aIndex ) const;
		inline TInt GetDesMaxLength ( TInt aIndex ) const;		
		inline TInt GetMBufChainLength ( TInt aIndex ) const;
		
		inline const TDesC& GetDesc ( TInt aIndex ) const;
		
		inline void SetRequest (TRequestWrapper& aRequest);
		inline void CompleteMessage(TInt aError);
		
	private:
		TIpcArgs		iArgs;
		TRequestWrapper iRequest;		
	};


class TEIntSockMsg : public Den::TWorkerTransportMsg
	{
public:
	inline TEIntSockMsg(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl* aImplPtr, TRequestStatus& aStatus);
	inline TEIntSockMsg(TSockMess aOperation, TEIntSockOpMsgWrapper& iOpMsg, CInternalSocketImpl** aImplPtrPtr, TRequestStatus& aStatus);
	inline TEIntSockMsg(TSockMess aOperation, TEIntSockOpMsgWrapper& iOpMsg, CInternalSocketImpl* aImplPtr);
	inline TEIntSockMsg(TSockMess aOperation, TEIntSockOpMsgWrapper& iOpMsg, CInternalSocketImpl** aImplPtrPtr);	
	inline TSockMess Operation () const;
	inline TEIntSockOpMsgWrapper OpMsg () const;
	inline CInternalSocketImpl*& ImplPtrRef() const;
	inline CInternalSocketImpl* ImplPtr() const;
protected:
	struct TEIntSockMsgParams : public Den::TWorkerTransportMsg::TWorkerTransportParams 
		{	
		TSockMess				iOperation;
		TEIntSockOpMsgWrapper	iOpMsg;
		union
			{			
			CInternalSocketImpl** iImplPtrPtr;
			CInternalSocketImpl* iImplPtr;
			};
		};
	};

class TEIntSockMsgExt : public TEIntSockMsg
	{
public:
	inline TEIntSockMsgExt(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl*& aImplPtr, TRequestStatus& aStatus, CommsFW::MCommsTransportSender* aSender, const Messages::TNodeId& aImplCookie);
	inline TEIntSockMsgExt(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl*& aImplPtr, CommsFW::MCommsTransportSender* aSender, const Messages::TNodeId& aImplCookie);
	inline CommsFW::MCommsTransportSender* Sender() const;
	inline const Messages::TNodeId& ImplCookie() const;
protected:
	struct TParams : public TEIntSockMsg::TEIntSockMsgParams
		{
		CommsFW::MCommsTransportSender* iSender;
		Messages::TNodeId iImplCookie;
		};
	};
	

#include "ss_eintsock_msgs.inl"
#endif


// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// @internalTechnology
// 
//

inline TEIntSockOpMsgWrapper::TEIntSockOpMsgWrapper()
	{
		
	}

template < class T0 >
inline TEIntSockOpMsgWrapper::TEIntSockOpMsgWrapper ( T0 a0 )
	{
	iArgs.Set ( 0, a0 );
	iArgs.Set ( 1, 0 );
	iArgs.Set ( 2, 0 );
	iArgs.Set ( 3, 0 );
	}

template < class T0, class T1 >
inline TEIntSockOpMsgWrapper::TEIntSockOpMsgWrapper ( T0 a0, T1 a1 )
	{
	iArgs.Set ( 0, a0 );
	iArgs.Set ( 1, a1 );
	iArgs.Set ( 2, 0 );
	iArgs.Set ( 3, 0 );
	}

template < class T0, class T1, class T2 >
inline TEIntSockOpMsgWrapper::TEIntSockOpMsgWrapper ( T0 a0, T1 a1, T2 a2 )
	{
	iArgs.Set ( 0, a0 );
	iArgs.Set ( 1, a1 );
	iArgs.Set ( 2, a2 );
	iArgs.Set ( 3, 0 );
	}

template < class T0, class T1, class T2, class T3 >
inline TEIntSockOpMsgWrapper::TEIntSockOpMsgWrapper ( T0 a0, T1 a1, T2 a2, T3 a3 )
	{
	iArgs.Set ( 0, a0 );
	iArgs.Set ( 1, a1 );
	iArgs.Set ( 2, a2 );
	iArgs.Set ( 3, a3 );		
	}


inline TInt TEIntSockOpMsgWrapper::ReadDes(TInt aIndex, TDes8& aDes, TInt aOffset) const
	{
	const TDesC8* desc = ( const TDesC8* ) iArgs.iArgs[ aIndex ];
	aDes.Copy( desc->Mid ( aOffset ) );
	return KErrNone;
	}

inline TInt TEIntSockOpMsgWrapper::ReadBufChain ( TInt aIndex, RMBufChain& aBufChain ) const
	{
	RMBufChain* buf = ( RMBufChain* ) iArgs.iArgs [ aIndex ];	
	aBufChain = buf->First ();
	return KErrNone;
	}

inline TInt TEIntSockOpMsgWrapper::ReadInt(TInt aIndex) const
	{
	return iArgs.iArgs[aIndex];
	}

inline void TEIntSockOpMsgWrapper::InitBufChain(TInt aIndex)
	{
	RMBufChain* buf = ( RMBufChain* ) iArgs.iArgs [ aIndex ];
	buf->Init ();		
	}

inline TInt TEIntSockOpMsgWrapper::WriteBufChain( TInt aIndex, RMBufChain& aBufChain )
	{
	RMBufChain* buf = ( RMBufChain* ) iArgs.iArgs [ aIndex ];
	*buf = aBufChain.First ();	
	return KErrNone;
	}

inline void TEIntSockOpMsgWrapper::WriteInt ( TInt aIndex, TInt aValue )
	{
	iArgs.Set ( aIndex, aValue );
	}

const TDesC& TEIntSockOpMsgWrapper::GetDesc ( TInt aIndex ) const
	{
	return (*(( const TDesC* )iArgs.iArgs [ aIndex ]));		
	}

inline void TEIntSockOpMsgWrapper::SetRequest (TRequestWrapper& aRequest)
	{
	iRequest = aRequest;
	}

inline void TEIntSockOpMsgWrapper::CompleteMessage(TInt aError)
	{
	iRequest.RequestComplete(aError);	
	}

inline TInt TEIntSockOpMsgWrapper::GetDesLength ( TInt aIndex ) const
	{
	const TDesC8* desc = (const TDesC8*)iArgs.iArgs[ aIndex ];
	return desc->Length ();
	}

inline TInt TEIntSockOpMsgWrapper::GetDesMaxLength ( TInt aIndex ) const
	{
	TDes8* des = (TDes8*)iArgs.iArgs[ aIndex ];
	return des->MaxSize ();		
	}


inline TInt TEIntSockOpMsgWrapper::GetMBufChainLength ( TInt aIndex ) const
	{
	RMBufChain* buf = (RMBufChain*)iArgs.iArgs [ aIndex ];
	return buf->Length ();
	}

inline TEIntSockMsg::TEIntSockMsg(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl* aImplPtr, TRequestStatus& aStatus)
: Den::TWorkerTransportMsg(KEIntSockTransportPluginImplementationUid, aStatus)
	{
	TEIntSockMsgParams* params = reinterpret_cast<TEIntSockMsgParams*>(TCFMessage::ParamBlock());
	params->iOperation = aOperation;
	params->iOpMsg = aOpMsg;
	params->iImplPtr = aImplPtr;
	}

inline TEIntSockMsg::TEIntSockMsg(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl** aImplPtrPtr, TRequestStatus& aStatus) 
: Den::TWorkerTransportMsg(KEIntSockTransportPluginImplementationUid, aStatus)
	{
	TEIntSockMsgParams* params = reinterpret_cast<TEIntSockMsgParams*>(TCFMessage::ParamBlock());
	params->iOperation = aOperation;	
	params->iOpMsg = aOpMsg;
	params->iImplPtrPtr = aImplPtrPtr;
	}

inline TEIntSockMsg::TEIntSockMsg(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl* aImplPtr)
: Den::TWorkerTransportMsg(KEIntSockTransportPluginImplementationUid)
	{
	TEIntSockMsgParams* params = reinterpret_cast<TEIntSockMsgParams*>(TCFMessage::ParamBlock());
	params->iOperation = aOperation;
	params->iOpMsg = aOpMsg;
	params->iImplPtr = aImplPtr;
	}

inline TEIntSockMsg::TEIntSockMsg(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl** aImplPtrPtr) 
: Den::TWorkerTransportMsg(KEIntSockTransportPluginImplementationUid)
	{
	TEIntSockMsgParams* params = reinterpret_cast<TEIntSockMsgParams*>(TCFMessage::ParamBlock());
	params->iOperation = aOperation;	
	params->iOpMsg = aOpMsg;
	params->iImplPtrPtr = aImplPtrPtr;
	}


inline TSockMess TEIntSockMsg::Operation () const
	{
	const TEIntSockMsgParams* params = reinterpret_cast<const TEIntSockMsgParams*>(TCFMessage::ParamBlock());
	return params->iOperation;
	}

inline TEIntSockOpMsgWrapper TEIntSockMsg::OpMsg () const
	{
	const TEIntSockMsgParams* params = reinterpret_cast<const TEIntSockMsgParams*>(TCFMessage::ParamBlock());
	return params->iOpMsg;
	}

inline CInternalSocketImpl*& TEIntSockMsg::ImplPtrRef() const
	{
	const TEIntSockMsgParams* params = reinterpret_cast<const TEIntSockMsgParams*>(TCFMessage::ParamBlock());
	return *params->iImplPtrPtr;
	}

inline CInternalSocketImpl* TEIntSockMsg::ImplPtr() const
	{
	const TEIntSockMsgParams* params = reinterpret_cast<const TEIntSockMsgParams*>(TCFMessage::ParamBlock());
	return params->iImplPtr;
	}

inline TEIntSockMsgExt::TEIntSockMsgExt(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl*& aImplPtr, TRequestStatus& aStatus, CommsFW::MCommsTransportSender* aSender, const Messages::TNodeId& aImplCookie)
: TEIntSockMsg(aOperation, aOpMsg, &aImplPtr, aStatus)
	{
	TParams* params = reinterpret_cast<TParams*>(ParamBlock());
	params->iSender = aSender;
    
	// caution - TRuntimeCtxId assignment operator overloaded - it assumes a properly constructed destination, which it isn't here
	// so we in-place construct it
	new (&params->iImplCookie)Messages::TNodeId();
	params->iImplCookie = aImplCookie;
	}

inline TEIntSockMsgExt::TEIntSockMsgExt(TSockMess aOperation, TEIntSockOpMsgWrapper& aOpMsg, CInternalSocketImpl*& aImplPtr, CommsFW::MCommsTransportSender* aSender, const Messages::TNodeId& aImplCookie)
: TEIntSockMsg(aOperation, aOpMsg, &aImplPtr)
	{
	TParams* params = reinterpret_cast<TParams*>(ParamBlock());
	params->iSender = aSender;
	
	// caution - TRuntimeCtxId assignment operator overloaded - it assumes a properly constructed destination, which it isn't here
	// so we in-place construct it
	new (&params->iImplCookie)Messages::TNodeId();
	params->iImplCookie = aImplCookie;
	}

inline CommsFW::MCommsTransportSender* TEIntSockMsgExt::Sender() const
	{
	const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
	return params->iSender;
	}
	
inline const Messages::TNodeId& TEIntSockMsgExt::ImplCookie() const
	{
	const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
	return params->iImplCookie;
	}



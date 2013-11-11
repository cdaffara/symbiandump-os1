// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// include\mmf\common\mmfipc.inl
// 
//

#ifdef __MMF_USE_IPC_V2__

//
// CMmfIpcSession
//

inline void CMmfIpcSession::CreateL(const CMmfIpcServer& /*aServer*/)
	{
	// default does nothing
	} 
	
inline void CMmfIpcSession::CreateL() 
	{ 
	const CMmfIpcServer* server = static_cast<const CMmfIpcServer*>(Server());
	CreateL(*server); // simulate CSession::CreateL() call
	}

//
// CMmfIcpServer
//

inline CMmfIpcServer::CMmfIpcServer(TInt aPriority,TServerType aType):
	CServer2(aPriority,aType)
	{
	}

inline CSession2* CMmfIpcServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	return NewSessionL(aVersion);
	}

#endif //__MMF_USE_IPC_V2__
	
//
// MmfMessageUtil
//

inline void MmfMessageUtil::ReadL(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult)
	{
#ifdef __MMF_USE_IPC_V2__
	User::LeaveIfError(aMessage.Read(aParam, aResult));
#else
	// expect aParam to be a literal - so this code should optimise out
	if (aParam==0)
		aMessage.ReadL(aMessage.Ptr0(), aResult);
	else if (aParam==1)
		aMessage.ReadL(aMessage.Ptr1(), aResult);
	else if (aParam==2)
		aMessage.ReadL(aMessage.Ptr2(), aResult);
	else if (aParam==3)
		aMessage.ReadL(aMessage.Ptr3(), aResult);
	else
		User::Leave(KErrArgument);
#endif //__MMF_USE_IPC_V2__
	}

inline void MmfMessageUtil::WriteL(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue)
	{
#ifdef __MMF_USE_IPC_V2__
	User::LeaveIfError(aMessage.Write(aParam, aValue));
#else
	// expect aParam to be a literal - so this code should optimise out
	if (aParam==0)
		aMessage.WriteL(aMessage.Ptr0(), aValue);
	else if (aParam==1)
		aMessage.WriteL(aMessage.Ptr1(), aValue);
	else if (aParam==2)
		aMessage.WriteL(aMessage.Ptr2(), aValue);
	else if (aParam==3)
		aMessage.WriteL(aMessage.Ptr3(), aValue);
	else
		User::Leave(KErrArgument);
#endif //__MMF_USE_IPC_V2__
	}

inline TInt MmfMessageUtil::Read(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult)
	{
#ifdef __MMF_USE_IPC_V2__
	return aMessage.Read(aParam, aResult);
#else
	TRAPD(error, ReadL(aMessage, aParam, aResult));
	return error;
#endif //__MMF_USE_IPC_V2__
	}

inline TInt MmfMessageUtil::Write(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue)
	{
#ifdef __MMF_USE_IPC_V2__
	return aMessage.Write(aParam, aValue);
#else
	TRAPD(error, WriteL(aMessage, aParam, aValue));
	return error;
#endif //__MMF_USE_IPC_V2__
	}





/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __TESTFRAMEWORKIPC_INL__
#define __TESTFRAMEWORKIPC_INL__


//
// RMmfSessionBaseX
//

// Pass throughs - equiv to using...

inline TInt RMmfSessionBaseX::SendReceive(TInt aFunction) const
	{
	return RMmfSessionBase::SendReceive(aFunction);
	}
	
inline void RMmfSessionBaseX::SendReceive(TInt aFunction, TRequestStatus& aStatus) const
	{
	RMmfSessionBase::SendReceive(aFunction, aStatus);
	}
	
inline TInt RMmfSessionBaseX::SendReceive(TInt aFunction, TInt aParam0) const
	{
	return RMmfSessionBase::SendReceive(aFunction, aParam0);
	}
	
inline TInt RMmfSessionBaseX::SendReceive(TInt aFunction, TInt aParam0, TInt aParam1) const
	{
	return RMmfSessionBase::SendReceive(aFunction, aParam0, aParam1);
	}
	
inline void RMmfSessionBaseX::SendReceive(TInt aFunction, TInt aParam0, TRequestStatus& aStatus) const
	{
	RMmfSessionBase::SendReceive(aFunction, aParam0, aStatus);
	}
	
inline void RMmfSessionBaseX::SendReceive(TInt aFunction, TInt aParam0, TInt aParam1, TRequestStatus& aStatus) const
	{
	RMmfSessionBase::SendReceive(aFunction, aParam0, aParam1, aStatus);
	}
	
inline TInt RMmfSessionBaseX::SendReceive(TInt aFunction, const TDesC8& aParam0) const
	{
	return RMmfSessionBase::SendReceive(aFunction, aParam0);
	}
	
inline TInt RMmfSessionBaseX::SendReceive(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1) const
	{
	return RMmfSessionBase::SendReceive(aFunction, aParam0, aParam1);
	}
	
inline TInt RMmfSessionBaseX::SendReceive(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, const TDesC8& aParam2) const
	{
	return RMmfSessionBase::SendReceive(aFunction, aParam0, aParam1, aParam2);
	}
	
inline void RMmfSessionBaseX::SendReceive(TInt aFunction, const TDesC8& aParam0, TRequestStatus& aStatus) const
	{
	RMmfSessionBase::SendReceive(aFunction, aParam0, aStatus);
	}
	
inline void RMmfSessionBaseX::SendReceive(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, TRequestStatus& aStatus) const
	{
	RMmfSessionBase::SendReceive(aFunction, aParam0, aParam1, aStatus);
	}
	
inline void RMmfSessionBaseX::SendReceive(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, const TDesC8& aParam2, TRequestStatus& aStatus) const
	{
	RMmfSessionBase::SendReceive(aFunction, aParam0, aParam1, aParam2, aStatus);
	}

// New call

inline TInt RMmfSessionBaseX::SendReceive(TInt aFunction, const TDesC& aParam0, TInt aParam1) const
	{
#ifdef __MMF_USE_IPC_V2__
	return RSessionBase::SendReceive(aFunction, TIpcArgs(&aParam0,aParam1));
#else
	TAny* p[KMaxMessageArguments];
	p[0] = static_cast<TAny*>(const_cast<TDesC16*>(&aParam0));
	p[1] = reinterpret_cast<TAny*>(aParam1);
	p[2] = NULL;
	p[3] = NULL;
	return RSessionBase::SendReceive(aFunction, p);
#endif __MMF_USE_IPC_V2__
	}

//
// MmfMEssageUtilX
//

// Using calls

inline TInt MmfMessageUtilX::Read(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult)
	{
	return MmfMessageUtil::Read(aMessage, aParam, aResult);
	}

inline TInt MmfMessageUtilX::Write(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue)
	{
	return MmfMessageUtil::Write(aMessage, aParam, aValue);
	}

inline void MmfMessageUtilX::ReadL(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult)
	{
	MmfMessageUtil::ReadL(aMessage, aParam, aResult);
	}

inline void MmfMessageUtilX::WriteL(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue)
	{
	MmfMessageUtil::WriteL(aMessage, aParam, aValue);
	}


// New calls

inline void MmfMessageUtilX::ReadL(const RMmfIpcMessage& aMessage, TInt aParam, TDes16& aResult)
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
#endif __MMF_USE_IPC_V2__
	}

inline void MmfMessageUtilX::WriteL(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC16& aValue)
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
#endif __MMF_USE_IPC_V2__
	}

inline TInt MmfMessageUtilX::Read(const RMmfIpcMessage& aMessage, TInt aParam, TDes16& aResult)
	{
#ifdef __MMF_USE_IPC_V2__
	return aMessage.Read(aParam, aResult);
#else
	TRAPD(error, ReadL(aMessage, aParam, aResult));
	return error;
#endif __MMF_USE_IPC_V2__
	}

inline TInt MmfMessageUtilX::Write(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC16& aValue)
	{
#ifdef __MMF_USE_IPC_V2__
	return aMessage.Write(aParam, aValue);
#else
	TRAPD(error, WriteL(aMessage, aParam, aValue));
	return error;
#endif __MMF_USE_IPC_V2__
	}



#endif // __TESTFRAMEWORKIPC_INL__

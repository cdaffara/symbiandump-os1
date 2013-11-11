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
//

#if !defined(__RSSTD_INL__)
#define __RSSTD_INL__

inline RootServer::TRSModuleState CCommsProviderModule::GetState() const
   /** Returns state for a CPM
     */    
    {
    return iState;
    }

inline const CommsFW::TCFModuleNameF& CCommsProviderModule::GetName() const
   /** Returns name for a CPM
     */    
    {
    return iName;
    }

inline const TFileName& CCommsProviderModule::GetDll() const
   /** Returns Dll filename for a CPM
     */
    {
    return iDll;
    }
    
inline RHeap* CCommsProviderModule::GetHeapPtr() const
   /** Returns heap pointer for a CPM
     */
    {
	return iHeapPtr;
	}  
  
inline const CommsFW::RCFChannel& CCommsProviderModule::GetSendChannel() const
	{
	return iSend;
	}
	
inline const CommsFW::RCFChannel& CCommsProviderModule::GetRecvChannel() const 
	{
	return iRecv;
	}
  
inline TBool CCommsProviderModule::IsSticky() const
	{
	return iIsSticky;
	}

inline TBool CCommsProviderModule::IsServer() const
	{
	return iIsServer;
	}

#if defined(SYMBIAN_C32ROOT_API_V3)

inline TUint32 CCommsProviderModule::ControlFlags() const
	{
	return iControlFlags;
	}

inline void CCommsProviderModule::SetControlFlags(TUint32 aFlags)
	{
	iControlFlags = aFlags;
	}

#endif

inline TBool CNotifySuddenDeathContainer::Match(MNotifySuddenDeath* aNotifier) const
	{
	return iNotifier == aNotifier;
	}

#endif


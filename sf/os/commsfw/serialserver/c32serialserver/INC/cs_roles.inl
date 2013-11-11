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
// CC32WorkerDataGlobals
// 
//


inline CC32WorkerThread* CC32WorkerDataGlobals::WorkerThread(TWorkerId aWorker) const
	{
	return GetWorkerGlobals(aWorker)->iWorker;
	}

//
//  CCSYInfo
//


inline void CCSYInfo::SetWorkerId(TInt aThreadNum)  
	{ 
	iThreadNum=aThreadNum;
	}
	
inline TInt CCSYInfo::WorkerId() const
	{
	return iThreadNum;
	}
	
inline const HBufC8* CCSYInfo::GetCSYName() const
	{
	return iCSYName;
	}   // guaranteed to be non-null if NewL called.
	
inline CSerial* CCSYInfo::GetCSYHandle() const
	{
	return iCSYHandle;
	}
	
	
inline TPtrC CCSYInfo::GetPortName() const
	{
	return iCSYPortPrefix;
	}

inline void CCSYInfo::IncAccessCount() 
	{
	iAccessCount++;
	}
		
inline void CCSYInfo::DecAccessCount()
	{
	iAccessCount--;
	}
		
inline TInt CCSYInfo::AccessCount() const
	{
	return iAccessCount;
	}
		
inline TBool CCSYInfo::IsLoaded() const 
	{
	return iLoaded;
	}
	
inline void CCSYInfo::SetLoadState(const TBool aLoaded)
	{
	iLoaded=aLoaded;
	}
	
inline void CCSYInfo::SetLoadIndex(TInt aIndex)
	{
	iLoadIndex = aIndex;
	}
	
inline TInt CCSYInfo::LoadIndex() const
	{
	return iLoadIndex;
	}
	
//
// CC32ThreadInfo
//

inline const HBufC8* CC32ThreadManager::CC32ThreadInfo::ModuleName() const
	{
	return iModuleName;
	}
	
//	
// CC32ParsedIniData
//

inline void CC32ThreadManager::CC32ParsedIniData::SetWorkerId(const TInt aThreadNum)
	{
	iCMIWorkerId = aThreadNum;
	}

inline TInt CC32ThreadManager::CC32ParsedIniData::NumCSYs() const
 	{
 	return iCSYs.Count();
 	}  

inline TBool CC32ThreadManager::CC32ParsedIniData::IsPlayer() const
	{
	return iIsPlayer;
	}
	
inline TBool CC32ThreadManager::CC32ParsedIniData::IsDefaultThread() const
 	{
 	return iIsDefault;
 	}
 	
inline TInt CC32ThreadManager::CC32ParsedIniData::WorkerId() const
	{
	return iCMIWorkerId;
	}

//
// CC32Dealer
//


inline TBool CC32Dealer::WorkerDataGlobalsExist() const
	{
	return (iWorkerDataGlobals != NULL);
	}

inline CC32WorkerDataGlobals& CC32Dealer::WorkerDataGlobals() const
	{
	ASSERT(iWorkerDataGlobals);
	return *iWorkerDataGlobals;
	}

inline TBool CC32Dealer::StartupFailed() const
	{
	return iFailedStartup;
	}

/** 
Immediate shutdowns expose a number of additional race risks; in the face of one happening to
any thread at all we stop attempting certain kinds of cleanup. This should be acceptable
behaviour since at it will be the whole of C32 shutting down. 
*/
inline void CC32Dealer::SetImmediateShutdownPresent()
	{
	iImmediateShutdownMark = ETrue;
	}

inline TBool CC32Dealer::TestImmediateShutdownPresent() const
	{
	return iImmediateShutdownMark;
	}	
    	
//
// CC32Player
//

inline CC32WorkerThread& CC32Player::WorkerThread() const
	{
	return *iOwnerThread;
	}

inline CC32Dealer& CC32Player::CC32DealerByRef() const
	{
	return WorkerThread().DealerByRef();
	}

inline TWorkerId CC32Player::WorkerId() const
	{
	return WorkerThread().WorkerId();
	}

inline const RMessage2& CC32Player::Message() const
	{
	return *iCurrentMessage;
	}

inline CC32Player::TSubSessionContainer& CC32Player::SubSessions()
	{
	return iSubSessions;
	}




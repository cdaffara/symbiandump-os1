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
//

/**
 @file
 @internalComponent
*/

#include <cfextras.h>
#include <comms-infras/c32startcli.h>
#include <e32property.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cflog.h>
#endif
#include "cs_roles.h"
#include "C32LOG.H"
#include "CS_STD.H"
#include "cs_msgs.h"
#include "cs_glob.h"

using namespace RootServer;

#define MSG_PRM(prmIndex)		(prmIndex)		// subsession is passed in message as parameter 3

const TInt KIniDataBufSizeIncrement = 60;   // Initial buffer of 60 chars - after finding 
                                           // usual iniData section uses about 40 chars to describe workerId,Role and CSYlist

//
// CC32WorkerDataGlobals
//

CC32WorkerDataGlobals* CC32WorkerDataGlobals::NewL()
	{
	CC32WorkerDataGlobals* self = new(ELeave) CC32WorkerDataGlobals();
	CleanupStack::PushL(self);
	self->ConstructL(sizeof(TC32WorkerThreadRegister), KMaxWorkerThreadId);
	CleanupStack::Pop(self);
	return self;
	}

//
// CCSYInfo
//

CCSYInfo::CCSYInfo(TInt aThreadNum)
: iThreadNum(aThreadNum),
iLoaded(EFalse),
iLoadIndex(-1)
	{
	}

CCSYInfo* CCSYInfo::NewL(const TDesC8& aCSYName, TInt aThreadNum)
	{
	CCSYInfo* self = new (ELeave) CCSYInfo(aThreadNum);
	CleanupStack::PushL(self);
	self->ConstructL(aCSYName);
	CleanupStack::Pop(self);
	return self;
	}
	
	
void CCSYInfo::ConstructL(const TDesC8& aCSYName)
	{
	iCSYName=aCSYName.AllocL();
	}

CCSYInfo::~CCSYInfo()
	{
	delete iCSYName;
	iCSYPortPrefix.Close();
	}
	
	
TInt CCSYInfo::CompareCSYInfo(const CCSYInfo& aFirst,const CCSYInfo& aSecond)
// compares CSY records by comparing the csy filenames
    {
    //-ve if first is less than second
    return aFirst.iCSYName->CompareF(*(aSecond.iCSYName));
    }
    	
TInt CCSYInfo::SetPortNameSizeToMaximum()
	{
	__ASSERT_DEBUG(iCSYPortPrefix.Length()==0,Fault(EBadState,_L8("CCSYInfo: attempt to grow portname for CSY for a non-null string (ie, module looks loaded)")));
	return iCSYPortPrefix.ReAlloc(KMaxFullName);	
	}

    	
void CCSYInfo::SetPortName(const TDesC& aPortName)
	{
	// since this may be called at end of csy load process, we have no reasonable way to deal with OOM,
	// so before the csy load process started, the portname
	// was grown to max. After setting the string we resize the portname so that the remaining
	// memory is freed. An RBuf allows us to resize in-place.
	
	iCSYPortPrefix = aPortName;
	
#ifdef _DEBUG
	TInt ret = iCSYPortPrefix.ReAlloc(aPortName.Length());
	__ASSERT_DEBUG(ret == KErrNone,Fault(EBadState,_L8("CCSYInfo::SetPortName: failed to shrink port prefix. Ret: %d"),ret));
#else
	(void)iCSYPortPrefix.ReAlloc(aPortName.Length()); // on failure, continue with current size.
#endif
	
	}
	
	
	
void CCSYInfo::SetCSYHandle(CSerial* aSerialHandle)
//this handle is never actually used by CCSYInfo, just passed back to player later
	{
	iCSYHandle=aSerialHandle;
	}






//
// CC32ThreadManager::CC32ParsedIniData
//

CC32ThreadManager::CC32ParsedIniData::CC32ParsedIniData()
	{
	}


CC32ThreadManager::CC32ParsedIniData* CC32ThreadManager::CC32ParsedIniData::NewL(const TDesC8& aIniData, const TBool aIsDealer)

// aIsDealer specifies whether this data was sent to us from the configurator as it spawned the
// dealer thread
	{
	CC32ParsedIniData* self = new(ELeave) CC32ParsedIniData();
	CleanupStack::PushL(self);
	self->ConstructL(aIniData,aIsDealer);
	CleanupStack::Pop(self);
	return self;
	}
	

CC32ThreadManager::CC32ParsedIniData::~CC32ParsedIniData()
	{
	iCSYs.Close();
	}

void CC32ThreadManager::CC32ParsedIniData::ConstructL(const TDesC8& aIniData, const TBool aIsDealer)
// we could have had the NewL perform this function, but because that would men creating and destroying
// this object for each data read, we chose to save that by making this operation separate
// aIsDealer - true if this is the inidata for the dealer loading all the cmi files. This allows
//             us to know this is a dealer even if nothing of the sort is in the [iniData] section and
//             thus we can preserve backwards compat.


// on exit the data should be:
// workerID = always correct - if we are dealer it is automatically 0 regardless of what config says. If we
//            are player we leave unless this is provided. We leave if it is zero and we are player.
// isPlayer - is false if we know we are dealer (aIsDealer) regardless of aIniData. Is true if we
//            see Role=player. If role missing and we are not Dealer (!aIsDealer) then we don't try
//            any recovery since a workerthread assumes it is a dealer if no role.
// isWildcard - true if a wildcard is provided in the csy list
// iCSYs - list of the csys in CSYList. It is valid that a dealer may not have a CSY list
	{
	TPtrC8 roleValue;
	TPtrC8 workerIdValue;
	TPtrC8 csyListValue;
	
	if (&aIniData==NULL || aIniData.Length()==0)
		{
		if (!aIsDealer)
			{
			C32LOG(KC32Warning,_L8("CC32ParsedIniData: Corrupt CMI file that references C32.DLL: Missing [IniData] section."));
			C32LOG(KC32Warning,_L8("CC32ParsedIniData: Please migrate all *.cmi files that reference C32.DLL to include Group token and [IniData] section."));
			// we don't panic in this scenario since this could be an old ini file we've found
			// in future we will panic here once licencees have migrated any cmi files they need to
			User::Leave(KErrCorrupt);
			}
		else
			{
			// dealers are allowed to not have CSYLists and we know what the role & workerId should be so continue
			// in future this will be a config error but during migration we continue.
   		    C32LOG(KC32Detail,_L8("CC32ParsedIniData: Supplied inidata for dealer is empty - must be old CMI file. Skipping."));
			return;
			}
		}


	if (!GetVarFromIniData(aIniData, KNullDesC8, KWorkerIdLabel, workerIdValue) )
		{
		C32LOG(KC32Warning,_L8("CC32ParsedIniData: Corrupt [IniData] section - missing workerId value."));
		
		if (aIsDealer)
			{
			C32LOG(KC32Warning,_L8("CC32ParsedIniData: As we are dealer, ignoring corruption by setting workerId to 0 and continuing"));
			iCMIWorkerId = KMainThreadId;
			}
		else
			{
#ifdef _DEBUG
			Fault(EBadIniData); // panic
#endif
			User::Leave(KErrCorrupt);
			}
		}
	else
		{
		// we accept whatever we get as the workerId
		TLex8 lex(workerIdValue);
		(void)lex.Val(iCMIWorkerId);		
		}
		
	
		
	
	if (!GetVarFromIniData(aIniData, KNullDesC8, KRoleLabel, roleValue))
		{
		C32LOG(KC32Warning,_L8("CC32ParsedIniData: Corrupt [IniData] section - missing Role label."));
		if (!aIsDealer)
			{
#ifdef _DEBUG
		Fault(EBadIniData); // panic
#endif
		User::Leave(KErrCorrupt);
			}
		}
	else if(roleValue.CompareF(KPlayerRole)==0)
		{
		iIsPlayer = ETrue;
		}
	else if(roleValue.CompareF(KDealerRole)!=0)
		{
		C32LOG2(KC32Warning,_L8("CC32ParsedIniData: Corrupt [IniData] section: Specified Role not recognised; must be Dealer or Player: %d "),&roleValue);
		
#ifdef _DEBUG
		Fault(EBadIniData); // panic
#endif
		User::Leave(KErrCorrupt);
		}
		
	
	if (iIsPlayer && (iCMIWorkerId < 0 || iCMIWorkerId > KMaxWorkerThreadId))
		{
		C32LOG2(KC32Warning,_L8("CC32ParsedIniData: Corrupt [IniData] section: Role=Player and WorkerId invalid: %d"),iCMIWorkerId);
#ifdef _DEBUG
		Fault(EBadIniData); // panic
#endif
		User::Leave(KErrCorrupt);
		}
		
		
	if (!GetVarFromIniData(aIniData, KNullDesC8, KCSYListLabel, csyListValue))
		{
		if (iIsPlayer)
			{
			C32LOG(KC32Warning,_L8("CC32ParsedIniData: Corrupt [IniData] section - missing CSYList definition."));
#ifdef _DEBUG
		Fault(EBadIniData); // panic
#endif
			User::Leave(KErrCorrupt);
			}
		}
	else
		{
		
		TInt sectionLen = 0;	
		TPtrC8 section(csyListValue);

		
		while (section.Length() > 0)
			{
			// assume that a CSY name is at least 1 characters long
			sectionLen = section.Find(TPtrC8(_S8(",")));
			if (sectionLen > 0)
				{
				iCSYs.AppendL(section.Mid(0,sectionLen));
				
				if (sectionLen+1 < section.Length())
					{
					section.Set(section.Mid(sectionLen+1));
					} 
				else
					{
					section.Set(KNullDesC8);
					}
				} 
			else if ((sectionLen==0) && (section.Length() > 0))
				{
				//possibly a double comma - we'll be nice and ignore it
				section.Set(section.Mid(sectionLen+1));
				} 
			else if (section[0]=='*')
				{
				iIsDefault = ETrue;
				if (section.Length() > 1)
					{
					section.Set(section.Mid(1));
					} 
				else
					{
					section.Set(KNullDesC8);
					}
				}
			else
				{
				iCSYs.AppendL(section);
				section.Set(KNullDesC8);
				}
			}
		}		
		

	}









//
// CC32ThreadManager::CC32ThreadInfo
//

CC32ThreadManager::CC32ThreadInfo::CC32ThreadInfo()
	{
	}
	


CC32ThreadManager::CC32ThreadInfo* CC32ThreadManager::CC32ThreadInfo::NewL(const TDesC8& aModuleName)
	{
	CC32ThreadInfo* self = new (ELeave) CC32ThreadInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aModuleName);
	CleanupStack::Pop(self);
	return self;	
	}
	
	
void CC32ThreadManager::CC32ThreadInfo::ConstructL(const TDesC8& aModuleName)
	{
	iModuleName = aModuleName.AllocL();
	}
	
void CC32ThreadManager::CC32ThreadInfo::SetModuleNameL(const TDesC8& aModuleName)
	{
	delete iModuleName;
	iModuleName = NULL;
	iModuleName = aModuleName.AllocL();
	}
	
CC32ThreadManager::CC32ThreadInfo::~CC32ThreadInfo()
	{
	delete iModuleName;
	}



TBool CC32ThreadManager::CC32ThreadInfo::CompareThreadInfo(const CC32ThreadInfo& aFirst,const CC32ThreadInfo& aSecond)
// aFirst should always be non-empty. aSecond may be an empty string if it is the dealer entry
	{
	return (aFirst.iModuleName==aSecond.iModuleName);
	}








//
// CCPMLoader
//


CC32ThreadManager::CCPMLoader::CCPMLoader()
    : CActive(EPriorityStandard)
    {
    // server is at a super-high priority, so whatever priority we choose is not going to make any difference
    CActiveScheduler::Add(this);

    }
    
CC32ThreadManager::CCPMLoader* CC32ThreadManager::CCPMLoader::NewL()
	{
    CCPMLoader* self = new(ELeave) CC32ThreadManager::CCPMLoader();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);    
    return self;
	}

void CC32ThreadManager::CCPMLoader::ConstructL()
	{
	TInt ret = iCfgSvr.Connect();
	if (ret != KErrNone)
 		 {
 		 C32LOG2(KC32Dealer,_L8("CC32Dealer::LoadModule failed during connect to configurator. Code: %d"),ret);
 		 User::Leave(ret);
 		 }
	}

	
void CC32ThreadManager::CCPMLoader::DoCancel()
	{
	C32LOG(KC32Detail,_L8("CC32Dealer::CCPMLoader: DoCancel called"));
	
	iCfgSvr.CancelLoadCpm(iName);
	}
	
CC32ThreadManager::CCPMLoader::~CCPMLoader()
    {
    Cancel();
    iCfgSvr.Close();
    // deques on delete of parent
    }	
    
void CC32ThreadManager::CCPMLoader::RequestModuleLoad(CC32ThreadManager* aThreadMgr, CommsFW::TWorkerId aWorker, const TCFModuleName& aName)
// Begin the asynchronous process of loading a player
	{
	__ASSERT_DEBUG(!IsActive(),Fault(EBadState,_L8("CCPMLoader: Loader already busy with another request when loading module %S. Panicking"),&aName));
	
	iName = TCFModuleName(aName);    // store so that when we lose stack at this function exit, RRs server can still read data
	iThreadManager = aThreadMgr;
	iWorkerId = aWorker;

	iCfgSvr.LoadCpm(iName,iStatus);
	
	SetActive();
		
	C32LOG2(KC32Dealer,_L8("CCPMLoader: Load request for module %S sent to Configurator"),&iName);
	}

void CC32ThreadManager::CCPMLoader::RunL()
// this normally never runs in the success case since new player's response beats it and dealer kills the loader.
	{
	TInt res = iStatus.Int();

	C32LOG4(KC32Dealer,_L8("CC32Dealer: Configurator has completed LoadCPM (%S)(WorkId%d) request with %d"),&iName,iWorkerId,res);
	
	// assert_debug in here that status is never "already running"
	__ASSERT_DEBUG(res != KErrRSModuleAlreadyExist, Fault(EBadState,_L8("CC32Dealer: LoadCPM returned KErrRSModuleAlreadyExists - this is unexpected so Panicking.")));
	__ASSERT_DEBUG(res != KErrRSModuleUnknown, Fault(EBadState,_L8("CC32Dealer: LoadCPM returned KErrRSModuleUnknown - probably incorrect CMI file. Panicking.")));
	
	
	// on success there is nothing we need to do with this response - just capture it and log it.
	// on failure we clean up.
	
	if (res==KErrNone)
		{
		C32LOG2(KC32Detail,_L8("CC32Dealer::LoadCPM: Load for (%S) has been successful"),&iName);
		}
	else
		{
		__ASSERT_DEBUG(!iThreadManager->Dealer()->WorkerExists(iWorkerId),Fault(EBadState,_L8("CCPMLoader::RunL - WorkerExists(%d) but configurator returned a failure: %d"),iWorkerId,res));
		C32LOG2(KC32Dealer,_L8("CC32Dealer::LoadCPM (%S) has apparently failed so cleaning up pending requests and threadmanager"),&iName);

		// give threadmanager a chance to remove any memory we allocated before 
		// commencing the module load.
		iThreadManager->ProcessModuleLoadFailed(iWorkerId);
		
		// we now leave ourselves inactive but still valid since we will be needed on the next attempt to load this cpm
		// and there is no obvious place to delete ourselves
		}
	}
	



//
// CC32ThreadManager
//

CC32ThreadManager::CC32ThreadManager(CC32Dealer* aDealer)
	{
	iDealer = aDealer;
	}
	
	
CC32ThreadManager* CC32ThreadManager::NewL(CC32Dealer* aDealer)
	{
	CC32ThreadManager* self = new (ELeave) CC32ThreadManager(aDealer);
	return self;

	}

CC32ThreadManager::~CC32ThreadManager()
	{
	iCSYList.ResetAndDestroy();
	iC32ThreadList.ResetAndDestroy();
	
	for (TInt i=0;i <= KMaxWorkerThreadId ; i++)
		{
		delete iCPMLoader[i];
		}
	}
	
CC32Dealer* CC32ThreadManager::Dealer()
	{
	__ASSERT_DEBUG(iDealer!=NULL,Fault(EBadState,_L8("CC32ThreadManager: Dealer() - iDealer is NULL. Panicking.")));
	return iDealer;
	}


TWorkerId CC32ThreadManager::DefaultThread()
	{
	return iDefaultThreadIndex;
	}


TBool CC32ThreadManager::FindThreadByPortPrefix(const TDesC& aPortName, TWorkerId& aWorker) const
// returns EFalse if not found, ETrue otherwise
// unloaded csys or csys yet to be loaded have an empty port prefix so will not be found
// searching is done in folded way to avoid case problems.
	{
	TInt idx = 0;
	if (&aPortName==NULL)
		{
		return KErrNotFound;
		}
		
	if (aPortName.Length()==0)
		{
		return KErrNotFound;
		}
	
	while (idx < iCSYList.Count())
		{
		if (aPortName.CompareF(iCSYList[idx]->GetPortName())==0)
			{
			__ASSERT_DEBUG(iCSYList[idx]->IsLoaded(),Fault(EBadState,_L("Panicking due to finding CSY by PortName (%S) that is inactive at index: %d" ),&aPortName,idx));
			aWorker=iCSYList[idx]->WorkerId();
			return ETrue;
			}
		idx++;
		}
		
	return EFalse;
	}
	
	
	
TInt CC32ThreadManager::MapPortprefixToCSYFileName(const TDesC& aPortName, TDes& aCSYFileName)
// aPortName is the port prefix format name to search on.
// unloaded csys or csys yet to be loaded have an empty port prefix so will not be found.
// aCSYFileName - on return contains the filename for the csy with the portprefix
// Return: KErrNone if the csy was found, KErrNotFound otherwise.
// Panics:
//    if aCSYFileName is not long enough to take result.
// Returns KErrNotFound in the case of port prefix being null
// 
	{
	TInt idx = 0;
	if (&aPortName==NULL)
		{
		return KErrNotFound;
		}
	if (aPortName.Length()==0)
		{
		return KErrNotFound;
		}

	while (idx < iCSYList.Count())
		{
		if (aPortName.CompareF(iCSYList[idx]->GetPortName())==0)
			{
			__ASSERT_DEBUG(iCSYList[idx]->IsLoaded(),Fault(EBadState,_L("Panicking due to finding CSY by PortName (%S) that is inactive at index: %d" ),&aPortName,idx));
			aCSYFileName.Copy(*iCSYList[idx]->GetCSYName());
			return KErrNone;
			}
		idx++;
		}
		
	return KErrNotFound;
	}
	
	
	
TBool CC32ThreadManager::FindThreadByFileName(const TDesC8& aCSYFileName8, TWorkerId &aWorkerId) const
// Find thread that would house this CSY, if no match found, return EFalse
// CSYFilename - CSY name with extension
	{
	TInt idx = 0;
	while (idx < iCSYList.Count())
		{
		if (aCSYFileName8.CompareF(*iCSYList[idx]->GetCSYName())==0)
			{
			aWorkerId = iCSYList[idx]->WorkerId();
			return ETrue;
			}
		idx++;
		}
		
	return EFalse;	
	}

TBool CC32ThreadManager::FindThreadOfActiveCSYByName(const TDesC& aName, TWorkerId& aWorkerId) const	// GetPortInfo(aName) version, TWorkerId filled with worker
// find thread by filename of CSY, if not found try by portnam of CSY.
// only searches active CSYs
// aName - portprefix or filename of csy to find
// aWorkerId - on return contains workerId, unchanged if result is EFalse
// returns: ETrue if found, EFalse otherwise
	{
	TBuf8<KMaxFileName> nameWithExt8;
	nameWithExt8.Copy(aName);
	
	 TInt r=nameWithExt8.LocateReverse('.');
	 if (r==KErrNotFound)
 		 {
 		 if (nameWithExt8.Length()>KMaxFileName - (TInt)KCSYExtension8.iTypeLength)		 // valid filename is checked here
	 		 {
	 		 return EFalse;
	 		 }
 		 nameWithExt8.Append(KCSYExtension8);
 		 }
	
	
	TInt idx = 0;
	while (idx < iCSYList.Count())
		{
		if (iCSYList[idx]->IsLoaded() && ((aName.CompareF(iCSYList[idx]->GetPortName())==0) || (nameWithExt8.CompareF(*iCSYList[idx]->GetCSYName())==0)))
			{
			aWorkerId = iCSYList[idx]->WorkerId();
			return ETrue;
			}
		idx++;
		}

	return EFalse;
	}

TInt CC32ThreadManager::FindThreadByGlobalIndex(const TInt aIndex) const
// aIndex is the array position the CSY would be in if it were in the old portmanager structure
// This function returns zero or a positive integer if the global index refers to a loaded
// csy. If not, then KErrNotFound is returned.

	{
	__ASSERT_DEBUG(aIndex >= 0,Fault(EBadState));
	TInt absIdx = 0;
	while (absIdx < iCSYList.Count())
		{
		if (iCSYList[absIdx]->IsLoaded())
			{
			if (iCSYList[absIdx]->LoadIndex()==aIndex)
				{
				return iCSYList[absIdx]->WorkerId();
				}
			}
		absIdx++;
		}
		
	
	return KErrNotFound;
	}

CSerial* CC32ThreadManager::GetSerialObjectFromCSYFileName(const TDesC& aCSYFileName)
// We might be asked to do an update even for a CSY that has been loaded since all CSY load requests are
// passed through to the player. This is because player needs to record each session that loads the module.
	{
	TBuf8<KMaxFileName> fileName8;   // convert 16 to 8-bit string. huge stack usage, but no other way
	fileName8.Copy(aCSYFileName);
	TInt idx = 0;
	
	while (idx < iCSYList.Count())
		{
		if (fileName8.CompareF(*iCSYList[idx]->GetCSYName())==0)
			{
			C32LOG1(KC32Detail, _L8("CC32ThreadManager::GetSerialObjectFromCSYFileName()"));
			return iCSYList[idx]->GetCSYHandle();
			}
		else
			{
			idx++;
			}
		}	
	return NULL;
	}

CSerial* CC32ThreadManager::GetSerialL(const TDesC& aPortName)
	{
	TInt idx;
	User::LeaveIfError(GetIndexFromPortPrefix(aPortName, idx));
	return GetSerialObjectByIndex(idx);
	}

TInt CC32ThreadManager::GetIndexFromPortPrefix(const TDesC& aPortName, TInt& aIndex)
	{
	TInt idx = 0;
	
	while (idx < iCSYList.Count())
		{
		if (aPortName.CompareF(iCSYList[idx]->GetPortName())==0)
			{
			C32LOG1(KC32Detail, _L8("CC32ThreadManager::GetIndexFromPortPrefix()"));
			aIndex = idx;
			return KErrNone;
			}
		else
			{
			idx++;
			}
		}	
	return KErrNotFound;
	}

CSerial* CC32ThreadManager::GetSerialObjectByIndex(TInt aIndex)
	{
	return iCSYList[aIndex]->GetCSYHandle();
	}

TInt CC32ThreadManager::IncrementCountOnLoad(const TDesC& aCSYFileName)
	{
	TBuf8<KMaxFileName> fileName8;   // convert 16 to 8-bit string. 
	fileName8.Copy(aCSYFileName);
	TInt idx = 0;
	
	while (idx < iCSYList.Count())
		{
		if (fileName8.CompareF(*iCSYList[idx]->GetCSYName())==0)
			{
			iCSYList[idx]->IncAccessCount();
			C32LOG3(KC32Detail, _L8("CC32ThreadManager::IncrementCountOnCSYLoad() AccessCount for %S CSY = %d"), &fileName8, iCSYList[idx]->AccessCount() );
			return KErrNone;
			}
		else
			{
			idx++;
			}
		}	
	return KErrNotFound;
	}

TInt CC32ThreadManager::DecrementCountOnCSYUnLoad(const TDesC& aCSYFileName)
	{
	TBuf8<KMaxFileName> fileName8;   // convert 16 to 8-bit string. 
	fileName8.Copy(aCSYFileName);
	TInt idx = 0;
	
	while (idx < iCSYList.Count())
		{
		if (fileName8.CompareF(*iCSYList[idx]->GetCSYName())==0)
			{
			if(iCSYList[idx]->AccessCount()==1)
				{
				// unload CSY as all sesions have unloaded CSYs or closed now.
				UpdateThreadManagerOnCSYUnload(fileName8);
				}
			iCSYList[idx]->DecAccessCount();
			C32LOG3(KC32Detail, _L8("CC32ThreadManager::DecrementCountOnCSYUnLoad() AccessCount for %S CSY = %d"), &fileName8, iCSYList[idx]->AccessCount() );
			return KErrNone;
			}
		else
			{
			idx++;
			}
		}	
	return KErrNotFound;
	}


TInt CC32ThreadManager::GrowCSYPortNameToMaximumIfNotLoaded(const TDesC& aCSYFileName)
	{
	TBuf8<KMaxFileName> fileName8;   // convert 16 to 8-bit string. 
	fileName8.Copy(aCSYFileName);
	TInt idx = 0;
	
	while (idx < iCSYList.Count())
		{
		if (fileName8.CompareF(*iCSYList[idx]->GetCSYName())==0)
			{
			if (!iCSYList[idx]->IsLoaded())
				{
				return iCSYList[idx]->SetPortNameSizeToMaximum();
				}
			else
				{
				return KErrNone;
				}
			}
		else
			{
			idx++;
			}
		}	
	return KErrNotFound;
	}


TInt CC32ThreadManager::RequestLoadModule(CommsFW::TWorkerId aWorker)
	{
	TInt ret = KErrNone;
	
	__ASSERT_DEBUG(!Dealer()->WorkerExists(aWorker),Fault(EBadState,_L8("CC32ThreadManager::RequestLoadModule: Worker already Exists! Panicking")));

	
	// loader may already exist but be inactive if a previous load attempt failed
	if (iCPMLoader[aWorker] == NULL)
		{
		C32LOG3(KC32Detail,_L8("ThreadMgr::RequestLoadModule: Loader doesn't exist for worker %d (%S). Starting worker"),aWorker,GetModuleName(aWorker));
		TRAP(ret,iCPMLoader[aWorker] = CCPMLoader::NewL());
		if (ret != KErrNone)
			{
			C32LOG2(KC32Detail,_L8("CCPMLoader::NewL() leave occured (%d)"),ret);
			return ret;
			}
		iCPMLoader[aWorker]->RequestModuleLoad(this,aWorker,*GetModuleName(aWorker));
		}
	else if (!iCPMLoader[aWorker]->IsActive())
		{
		C32LOG3(KC32Detail,_L8("CC32ThreadManager: Inactive Loader already exists - reusing to start worker %d (%S)."),aWorker,GetModuleName(aWorker));
		
		iCPMLoader[aWorker]->RequestModuleLoad(this,aWorker,*GetModuleName(aWorker));
		}
	else
		{
		C32LOG2(KC32Detail,_L8("ThreadMgr: RequestLoadModule: Attempt to start worker %d ignored due to already outstanding load- dealer should queue this req."),aWorker);
 		// if timer is already running, we assume load still in progress so we just exit
 		// and caller can queue request
		ret=KErrInUse;
		}
	return ret;
	}


void CC32ThreadManager::ProcessModuleLoadFailed(CommsFW::TWorkerId aWorker)
// called from LoadCPM when configurator reports that player has failed to start
// Gives thread manager a chance to do any cleanup.
	{
	C32LOG2(KC32Dealer,_L8("CC32ThreadManager:ProcessModuleLoadFailed: Failed worker was %d. Cleaning up pending requests."),aWorker);
	
	// give dealer a chance to remove any pending requests for the module which did not start
	// and undo any memory allocations done as part of load
	Dealer()->ProcessFailedPlayerLoad(aWorker);

	}


void CC32ThreadManager::ProcessModuleLoadSuccess(CommsFW::TWorkerId aWorker)
// called when dealer receives introduction msg from player, regardless of how player started
	{
	C32LOG2(KC32Detail, _L8("CC32ThreadManager::ProcessModuleLoadSuccess for worker %d. Deleting CPMLoader."), aWorker);
	
	// Delete the load Active object. It turns out that normally by this point the configurator has yet to complete the request,
	// even though the player is already running, so by removing the loader here, we will be sending a Cancel to the configurator
	// for an operation we know has already completed successfully. This is safe (view a request as consisting of two parts
	// - 1) ask a server to make a request and 2) hope the server will let you know how it goes - we are
	// now simply cancelling part 2 since we know how part 1 went)
	
	delete iCPMLoader[aWorker];
	iCPMLoader[aWorker] = NULL;
	
	}



void CC32ThreadManager::UpdateThreadManagerOnCSYUnload(const TDesC8& aCSYFileName8)
// called when we predict Player will unload CSY, since the indexing of the CSYs and the auto-close on index 0 behaviour
// that the player uses (preserved legacy C32 behaviour) does not give an opportunity for this process to be intercepted
// on player side and a mesg sent to dealer.
	{
	TInt idx = 0;	
	
	// NULL CSerial* and Portname
	C32LOG2(KC32Detail, _L8("CC32ThreadManager::UpdateThreadManagerOnCSYUnLoad() for %S"), &aCSYFileName8);
	while (idx < iCSYList.Count())
		{
		if (aCSYFileName8.CompareF(*iCSYList[idx]->GetCSYName())==0)
			{
			__ASSERT_DEBUG(iCSYList[idx]->IsLoaded(),Fault(EBadState,_L8("UpdateThreadManagerOnCSYUnLoad: CSY marked as not loaded on call to update after unload. Panicking.")));
			iCSYList[idx]->SetPortName(KNullDesC16);
			iCSYList[idx]->SetCSYHandle(NULL);
			iCSYList[idx]->SetLoadState(EFalse);
			
			// run thru list and adjust index numbers now that this csy has unloaded
			for (TInt absIdx = 0; absIdx < iCSYList.Count(); ++absIdx)
				{
				if ((iCSYList[absIdx]->IsLoaded() && (iCSYList[absIdx]->LoadIndex() > iCSYList[idx]->LoadIndex())))
					{
					iCSYList[absIdx]->SetLoadIndex(iCSYList[absIdx]->LoadIndex()-1);
					}
				}
			
			iCSYList[idx]->SetLoadIndex(-1);
			--iNumLoadedModules;

			return;
			}
		else
			{
			idx++;
			}
		}	
	
	C32LOG2(KC32Warning,_L8("UpdateThreadManagerOnCSYUnload: Could not find CSY supposedly unloaded: %S. Will Panic in debug."),&aCSYFileName8);
	__ASSERT_DEBUG(0,Fault(EBadState));
	}

void CC32ThreadManager::UpdateThreadManagerOnCSYLoad(const TDesC& aCSYFileName, const TDesC& aPortPrefix, CSerial* aSerial)
// We might be asked to do an update even for a CSY that has been loaded since all CSY load requests are
// passed through to the player. This is because player needs to record each session that loads the module.
	{
	TBuf8<KMaxFileName> fileName8;   // convert 16 to 8-bit string. huge stack usage, but no other way
	fileName8.Copy(aCSYFileName);
	TInt idx = 0;
	

	// we update the csyinfo with the new serial and port prefix and mark it active

	while (idx < iCSYList.Count())
		{
		if (fileName8.CompareF(*iCSYList[idx]->GetCSYName())==0)
			{
			if (iCSYList[idx]->IsLoaded()==EFalse)
				{
				C32LOG5(KC32Detail, _L("C32ThrdMg::UpdateThreadManagerOnCSYLoad() for CSY[%d](%S).CSerial=%x. PortPrefix=%S"),idx,&aCSYFileName,aSerial,&aPortPrefix);
				iCSYList[idx]->SetPortName(aPortPrefix);
				iCSYList[idx]->SetCSYHandle(aSerial);
				iCSYList[idx]->SetLoadState(ETrue);
				
				iCSYList[idx]->SetLoadIndex(iNumLoadedModules);
				++iNumLoadedModules;
				}
			else
				{
				C32LOG3(KC32Detail, _L8("CC32ThreadManager::UpdateThreadManagerOnCSYLoad(): CSY[%d] already updated. Access count is %d"),idx,iCSYList[idx]->AccessCount());
				}
			return;
			}
		else
			{
			idx++;
			}
		}
		
		
	C32LOG2(KC32Warning,_L8("UpdateThreadManagerOnCSYLoad: Could not find CSY supposedly loaded: %S. Will panic in debug."),&fileName8);
	__ASSERT_DEBUG(0,Fault(EBadState));
	}

void CC32ThreadManager::UpdateThreadManagerOnCSYLoadFailure(const TDesC& aCSYFilename)
// called to allow thread manager to clean up any allocations made prior to attempting CSY load.
// aCSYFilename - name with extension of CSY that player failed to load
	{
	CommsFW::TWorkerId worker;
	
	TBuf8<KMaxFileName> fileName8;   // convert 16 to 8-bit string. huge stack usage, but no other way
	fileName8.Copy(aCSYFilename);

	
	TBool found = FindThreadByFileName(fileName8,worker);
	
	// it is possible we would not find it if two sessions had requested this csy load at same time and
	// this is now the completion of the second sessions load attempt and the first session cleanup
	// had destroyed the csyInfo.
	if (found)
		{
		// if this was default thread we might have
		// added an unknown CSY so in this case remove it. It does no harm if this CSY was specified in CMI
		// file since on next attempt thread manager will place the CSY in same thread anyway.
		
		if (worker == iDefaultThreadIndex)
			{
			for (TInt i=0; i < iCSYList.Count(); i++)
				{
				// comparing the worker Ids first is a bit quicker
				if (iCSYList[i]->WorkerId()==worker)
					{
					if (iCSYList[i]->GetCSYName()->Compare(fileName8)==0)
						{
						__ASSERT_DEBUG(!iCSYList[i]->IsLoaded(),Fault(EBadState,_L8("CC32ThreadManager::UpdateThreadManagerOnCSYLoadFailure: CSY[%d] is marked as loaded! Panicking."),i));
						C32LOG3(KC32Dealer,_L("CC32ThreadManager::UpdateThreadManagerOnCSYLoadFailure: Failed CSY (%S) was for default thread. Deleting CSY[%d]"),&aCSYFilename,i);
						delete iCSYList[i];
						iCSYList.Remove(i);
						return;
						}
					}
				}
			C32LOG2(KC32Dealer,_L("CC32ThreadManager::UpdateThreadManagerOnCSYLoadFailure: Failed CSY (%S) was for default thread but could not find. Will panic in debug."),&aCSYFilename);
			__ASSERT_DEBUG(0,User::Invariant());
			}
		}
	else
		{
		C32LOG2(KC32Dealer,_L("CC32ThreadManager::UpdateThreadManagerOnCSYLoadFailure: Failed CSY record (%S) was not found so assuming another request deleted it"),&aCSYFilename);
		}
	}


CSerial* CC32ThreadManager::FindSerialObjectByGlobalIndex(const TInt aIndex) const
// aIndex is the array position the CSY would be in if it were in the old portmanager structure
// this is now the position of the csy in the active array
// index should be valid so panic if not
	{
		
	TInt absIdx = 0;
	while (absIdx < iCSYList.Count())
		{
		if (iCSYList[absIdx]->IsLoaded())
			{
			if (iCSYList[absIdx]->LoadIndex()==aIndex)
				{
				return iCSYList[absIdx]->GetCSYHandle();
				}
			else
				{
				absIdx++;
				}
			}
		else
			{
			absIdx++;
			}
		}
		
	C32LOG2(KC32Warning,_L8("FindSerialObjectByGlobalIndex - Panicking due to index out of range: %d"),aIndex);
	__ASSERT_ALWAYS(0,Fault(EBadState));
	
	return NULL;
	}
	
const HBufC8* CC32ThreadManager::GetModuleName(TWorkerId aWorkerId)
// aWorkerId assumed to be valid
	{
	return iC32ThreadList[aWorkerId]->ModuleName();
	}
	

#ifdef __FLOG_ACTIVE
void CC32ThreadManager::DumpThreadInfoAndCSYLists()
	{
	//log in Detail mode only
	
	C32LOG(KC32Detail,_L8("---- Dump of C32 Thread Manager ----"));
	C32LOG2(KC32Detail,_L8("Default Thread: %d"),iDefaultThreadIndex);
	TBuf8<KMaxFullName> shortName8; // TPortName is, rather annoyingly given it probably never needs to be, a 16-bit descriptor
	
	for (TInt i=0;i < iCSYList.Count();i++)
		{
		shortName8.Copy(iCSYList[i]->GetPortName());
		C32LOG8(KC32Detail,_L8("CSY List [%d].CSY Name:%S,ShrtNm:%S,ThrdID:%d,PlyrHdl:%x,Active:%d,Idx:%d"),i,iCSYList[i]->GetCSYName(),&shortName8,iCSYList[i]->WorkerId(),iCSYList[i]->GetCSYHandle(),iCSYList[i]->IsLoaded(),iCSYList[i]->LoadIndex());
		}

	TBool globalsExist = Dealer()->WorkerDataGlobalsExist();
	if (globalsExist)
		{
		C32LOG(KC32Detail,_L8("Thread List start."));		
		}
	else
		{
		C32LOG(KC32Detail,_L8("Thread List start. Still booting so don't know player thread status yet."));
		}
		
	for (TInt k=0;k < iC32ThreadList.Count(); k++)
		{
		if (iC32ThreadList[k]==NULL)
			{
			C32LOG2(KC32Warning,_L8("DumpThreadInfo: Internal check failure: Found NULL record at WorkerId: %d"),k);
#ifdef _DEBUG
			Fault(EBadState); // panic
#endif
			}
		else if (iC32ThreadList[k]->ModuleName()->CompareF(KNullDesC8)==0)
			{
			if (k==0)
				{
				C32LOG(KC32Detail,_L8("Thread List[0]:Blank (This is normal)"));
				}
			else
				{
				C32LOG2(KC32Warning,_L8("DumpThreadInfo: Internal check failure: Found blank record for a player at WorkerId: %d"),k);
				C32LOG1(KC32Warning,_L8("DumpThreadInfo: This indicates missing cmi file for this workerId. Continuing since this could be a test scenario."));
				// there is no panic here since we allow blanks in case user is mix-and-matching csys to threads
				}
			}
		else
			{
			if (globalsExist)
				{
				C32LOG4(KC32Detail,_L8("Thread List[%d]:Name:%S. Running:%d"),k,iC32ThreadList[k]->ModuleName(),Dealer()->WorkerExists(k));
				}
			else
				{
				C32LOG3(KC32Detail,_L8("Thread List[%d]:Name:%S. Dealer still starting so Running Status unknown."),k,iC32ThreadList[k]->ModuleName());
				}
			}
		}

	C32LOG(KC32Detail,_L8("---- End Dump of C32 Thread Manager ----"));
	
	
	
	}
#endif	


void CC32ThreadManager::LoadCMIDataL(const TDesC8& aDealerIniData)
// load all the data from the CMI files.
// aCSYList provides us with the data for the dealer, but we have to query the Configurator for all 
// the data for the player cmis.
	{

	TInt ret;
	TRSIter iter;


	TCFModuleName moduleName;    // as specified in CMI file
	RArray<TInt>		numCSYsPerCMI;  // used if we find two CSYs the same. Filled in as each
	                                    // cmi processed so that index of element is threadnum, 
	                                    // and value is number of CSYs in that threadnum.
	CleanupClosePushL(numCSYsPerCMI);
	
	TBool skipThisCMI;    // the CMI is a dealer or somehow invalid beyond redemption
	
	CC32ParsedIniData* iniDataParser;

	RBuf8 iniData;      // to accept data from the configurator
	TInt iniBufSize;   // tracks size of ini data buffer as we grow it
	
	RRsConfigurator cfgSvr;
	
	User::LeaveIfError(cfgSvr.Connect());
	CleanupClosePushL(cfgSvr); 
	

	iniBufSize=KIniDataBufSizeIncrement;
	iniData.CreateL(iniBufSize);
	iniData.CleanupClosePushL();
	
	CC32ThreadInfo* newThreadRec;
	CC32ThreadInfo* blankThreadRec; // we used to insert NULLs, but inserting this blank is a bit safer/easier to work with.
	
	// KMaxTUint16 magic value indicates no default thread has been identified - need to use unsigned value because
	// TWorkerID is now an unsigned type.
	iDefaultThreadIndex = KMaxTUint16;
	
	// process the dealer's own cmi data. Will leave if the cmi data is bad, unless
	// we are the dealer.
	iniDataParser = CC32ParsedIniData::NewL(aDealerIniData,ETrue);
	CleanupStack::PushL(iniDataParser);
	

	// add a record to the threadlist so that we have index 0 - it is irrelevant what we put in here
	// so we will make the string an empty string since we won't ever use it - Dealer is always running!
	// It has to be a valid string (so not null) otherwise the comparison (CompareThreadInfo) fails the type check.
	blankThreadRec = CC32ThreadInfo::NewL(KNullDesC8);
	ret = iC32ThreadList.Append(blankThreadRec); //transfer ownership
	if (ret != KErrNone)
		{
		delete blankThreadRec;
		User::Leave(ret);
		}
	blankThreadRec=NULL;
	
	__ASSERT_DEBUG(iC32ThreadList.Count()==1,Fault(EBadState));
	
	if (iniDataParser->IsDefaultThread())
		{
		iDefaultThreadIndex = KMainThreadId;
		}
		
	
	
	for (TInt i=0 ; i < iniDataParser->NumCSYs(); i++)
		{
		TBuf8<KMaxFileName> csyFilename;
		csyFilename.Copy(iniDataParser->iCSYs[i]);
		TInt r=csyFilename.LocateReverse('.');
	 	if (r==KErrNotFound)
			{
			if (csyFilename.Length()>KMaxFileName - (TInt)KCSYExtension8.iTypeLength)
				{
				C32LOG2(KC32Warning,_L("LoadCMIDataL: Corrupt [IniData] section: Dealer has a CSY that is too big: %S. Will panic in debug."),&iniDataParser->iCSYs[i]);
				__ASSERT_DEBUG(0,Fault(EBadIniData));
				_LIT8(KCSYDudName, "BadCsy" );
				csyFilename.Copy(KCSYDudName());  // continue with a dud record
				}
			csyFilename.Append(KCSYExtension8);
			}
		
		CCSYInfo* csyInfoRec = CCSYInfo::NewL(csyFilename, KMainThreadId);
		CleanupStack::PushL(csyInfoRec);
		
		ret = iCSYList.InsertInOrder(csyInfoRec, TLinearOrder<CCSYInfo>(&CCSYInfo::CompareCSYInfo));
		
		if (ret == KErrAlreadyExists)
			{
			C32LOG2(KC32Warning,_L8("LoadCMIDataL: Corrupt [IniData] section: Dealer has the same CSY listed twice: %S"),&iniDataParser->iCSYs[i]);
#ifdef _DEBUG
			Fault(EBadIniData); // panic
#endif

			// ignore and carry on
			CleanupStack::PopAndDestroy(csyInfoRec); // dont need duplicate further
			}
		else if (ret != KErrNone)
			{
			User::Leave(ret);
			} 
		else
			{
			// ownership transferred
			CleanupStack::Pop(csyInfoRec);
			}
		}

	CleanupStack::PopAndDestroy(iniDataParser);

	
	// iterate through all the CMI files
	// Any CMI data sections that don't have "Role=Player" are ignored. Dealer CMIs can be ignored
	// since we processed our dealer's cmi above.

	// If we find two CMI files with the same module name we fault.
	// The order we find them is the order that Configurator supplies them to us - it is probably alphabetical.
	// If we see the wildcard twice, we process it only the first time.
	// If we do not see the wildcard we make the first player the wildcard thread, or the dealer if no players.
	// If a player has no CSYs, we ignore it.
	
	// We do this in one pass:
	// 1. Read records from each CMI file as they come and add them to the structure. If the thread has no number,
	//    or a duplicate number, or "0" (which should be the dealer) we fault.
	//    After each
	//    CMI file is complete we record the total number of CSYs added for that thread. Before adding
	//    each CSY we scan the list to ensure that CSY has not already been added. If so, then we
	//    move it to whichever of the two threads it is allocated to that has the least number of CSYs.
	//
	// As a small efficiency gain, since we may be searching the CSY names many times we order the 
	// CSY list by CSY name. This order is not preserved once we've finished parsing the CMI files - ie,
	// further discovered csys through loading are simply appended to the end.
	// An efficincy improvement would be to parse the ini data sequentially. At the moment we reuse the commsfw ini
	// file parsing routine but this would re-read the data each time.
	// Also, a bit of string copying goes on so this could potentially be reduced.
	
	
	while ( cfgSvr.EnumerateModules(_L8("C32SerComms"),iter,moduleName) == KErrNone)
		{
	
		//keep enlarging inidata buffer until we get it big enough
		ret=cfgSvr.GetModuleIniData(moduleName, iniData, iniBufSize);
		if (ret==KErrOverflow)
			{
			iniData.ReAllocL(iniBufSize);
			ret=cfgSvr.GetModuleIniData(moduleName, iniData, iniBufSize);
			}
		User::LeaveIfError(ret);

		skipThisCMI=EFalse;
		
		// if it leaves we check it was due to corrupt data and continue since we can cope
		// with all corrupt data cases
		C32LOG2(KC32Bootup,_L8("LoadCMIDataL: Parsing inidata for module %S"),&moduleName);
		TRAP(ret,iniDataParser = CC32ParsedIniData::NewL(iniData,EFalse));
		CleanupStack::PushL(iniDataParser);

		
		if ((ret != KErrNone) && (ret != KErrCorrupt))
			{
			User::Leave(ret);
			}
		else
			{
			__ASSERT_DEBUG(iniDataParser,Fault(ESvrStartServer));
			}

		if (iniDataParser->IsPlayer())
			{
			newThreadRec = CC32ThreadInfo::NewL(moduleName);
			CleanupStack::PushL(newThreadRec);
			
			if (iC32ThreadList.Find(newThreadRec, TIdentityRelation<CC32ThreadInfo>(&CC32ThreadInfo::CompareThreadInfo)) != KErrNotFound)
				{
				// unfortunately there is a rare circumstance where we may miss a duplicate. If the dealer
				// is not part of the group, and then a member in the group has
				// the same name, we won't spot this. only configurator can detect this.
				C32LOG(KC32Warning,_L8("LoadCMIDataL: Corrupt IniData found - duplicate module name."));
#ifdef _DEBUG
				Fault(EBadIniData); // panic
#endif
				User::Leave(KErrCorrupt);
				}
				
			if ((iniDataParser->NumCSYs() < 1) && !iniDataParser->IsDefaultThread())
				{
				C32LOG(KC32Warning,_L8("LoadCMIDataL: Corrupt [IniData] section: Player has no CSY list."));
#ifdef _DEBUG
				Fault(EBadIniData); // panic
#endif
				skipThisCMI=ETrue;
				C32LOG(KC32Bootup,_L8("Skipping CMI file due to no CSYs found"));
				}
			
			
		
			if (!skipThisCMI)
				{
				// workerID range is checked in parser so we don't re-check here
				
							
				if ((iDefaultThreadIndex <= KMainThreadId) && (iniDataParser->IsDefaultThread()))
					{
					// if dealer was default, we override here since we now have a default player
					iDefaultThreadIndex = iniDataParser->WorkerId();
					}
					
				if (iniDataParser->WorkerId() >= iC32ThreadList.Count())
					{
					// expand the array if necessary
					// these elements may be filled in later
					while (iniDataParser->WorkerId() > iC32ThreadList.Count())
						{
						blankThreadRec = CC32ThreadInfo::NewL(KNullDesC8);
						ret = iC32ThreadList.Append(blankThreadRec); //transfer ownership
						if (ret != KErrNone)
							{
							delete blankThreadRec;
							User::Leave(ret);
							}
						blankThreadRec=NULL;
						}
					iC32ThreadList.AppendL(newThreadRec);
					CleanupStack::Pop(newThreadRec); //ownership transferred
					} 
				else
					{
					// we are using up an existing element
					if (iC32ThreadList[iniDataParser->WorkerId()]->ModuleName()->CompareF(KNullDesC8)!=0)
						{
						C32LOG2(KC32Warning,_L8("LoadCMIDataL: Corrupt IniData found - player workerId is a duplicate: %d."),iniDataParser->WorkerId());
#ifdef _DEBUG
						Fault(EBadIniData); // panic
#endif
						User::Leave(KErrCorrupt);
						}
					else
						{
						delete iC32ThreadList[iniDataParser->WorkerId()]; //blank record
						iC32ThreadList[iniDataParser->WorkerId()] = newThreadRec;
						CleanupStack::Pop(newThreadRec); //ownership transferred
						}
					}
					
				if (iniDataParser->WorkerId() >= numCSYsPerCMI.Count())
					{
					// expand the array if neceesary
					while (iniDataParser->WorkerId() > numCSYsPerCMI.Count())
						{
						numCSYsPerCMI.AppendL(-1);
						}
					numCSYsPerCMI.AppendL(iniDataParser->NumCSYs());
					}
				else
					{
					if (numCSYsPerCMI[iniDataParser->WorkerId()]>0)
						{
						C32LOG4(KC32Warning,_L8("LoadCMIDataL: Internal check failure: attempted to add CSYs for workerID twice. WorkerId: %d, CurrNumCSYs: %d, NewNumCSYs: %d"),iniDataParser->WorkerId(),numCSYsPerCMI[iniDataParser->WorkerId()],iniDataParser->NumCSYs());
#ifdef _DEBUG
						Fault(EBadState); // panic
#endif
						User::Leave(KErrCorrupt);
						}
					else
						{
						numCSYsPerCMI[iniDataParser->WorkerId()]=iniDataParser->NumCSYs();
						}
					}
					
				for (TInt i=0 ; i < iniDataParser->NumCSYs(); i++)
					{
					TBuf8<KMaxFileName> csyFilename;
					csyFilename.Copy(iniDataParser->iCSYs[i]);
					TInt r=csyFilename.LocateReverse('.');
				 	if (r==KErrNotFound)
						{
						if (csyFilename.Length()>KMaxFileName - (TInt)KCSYExtension8.iTypeLength)
							{
							C32LOG2(KC32Warning,_L("LoadCMIDataL: Corrupt [IniData] section: Player has a CSY that is too big: %S. Will panic in debug."),&iniDataParser->iCSYs[i]);
							__ASSERT_DEBUG(0,Fault(EBadIniData));
							_LIT8(KCSYDudName, "BadCsy" );
							csyFilename.Copy(KCSYDudName());  // continue with a dud record
							}
						csyFilename.Append(KCSYExtension8);
						}
					
					CCSYInfo* csyInfoRec = CCSYInfo::NewL(csyFilename, iniDataParser->WorkerId());
					CleanupStack::PushL(csyInfoRec);
					
					ret = iCSYList.InsertInOrder(csyInfoRec, TLinearOrder<CCSYInfo>(&CCSYInfo::CompareCSYInfo));
					if (ret == KErrAlreadyExists)
						{
						// duplicate discovered
						ret = iCSYList.FindInOrder(csyInfoRec, TLinearOrder<CCSYInfo>(&CCSYInfo::CompareCSYInfo));
						if (numCSYsPerCMI[iniDataParser->WorkerId()] > numCSYsPerCMI[iCSYList[ret]->WorkerId()])
							{
							C32LOG4(KC32Bootup,_L8("Reallocating ThreadId for CSY %S [%d -> %d] due to duplicate appearance in CMI files"),iCSYList[ret]->GetCSYName(),iCSYList[ret]->WorkerId(),iniDataParser->WorkerId());
							numCSYsPerCMI[iCSYList[ret]->WorkerId()]--;
							iCSYList[ret]->SetWorkerId(iniDataParser->WorkerId());
							}
						else
							{
							C32LOG3(KC32Bootup,_L8("CSY %S appears in two CMI files. Leaving in current thread (%d) due to equal CSY balance across threads."),iCSYList[ret]->GetCSYName(),iCSYList[ret]->WorkerId());
							numCSYsPerCMI[iniDataParser->WorkerId()]--;
							}
						CleanupStack::PopAndDestroy(csyInfoRec); // dont need duplicate further
						}
					else if (ret != KErrNone)
						{
						User::Leave(ret);
						} 
					else
						{
						// ownership transferred
						CleanupStack::Pop(csyInfoRec);
						}
			
					}
				} // if skip
			else
				{
				CleanupStack::PopAndDestroy(&newThreadRec);
				}
			
			} //if is player
			CleanupStack::PopAndDestroy(iniDataParser);
	
		}//while enumerate modules
	
		
	if (iDefaultThreadIndex == KMaxTUint16)
		{
		// try to find any usable players for the default. otherwise, we use dealer.
		TInt k=TC32WorkerThreadPublicInfo::EFirstPlayerThread;  // 
		while (k < iC32ThreadList.Count())
			{
			if (iC32ThreadList[k]!=blankThreadRec)
				{
				iDefaultThreadIndex = k;
				}
			k++;
			}
		if (iDefaultThreadIndex == KMaxTUint16)
			{
			iDefaultThreadIndex = KMainThreadId;
			}
		}
				
	CleanupStack::PopAndDestroy(&iniData);
	CleanupStack::PopAndDestroy(&cfgSvr);
	CleanupStack::PopAndDestroy(); // numCSYsPerCMI
	 


#ifdef __FLOG_ACTIVE
	DumpThreadInfoAndCSYLists();
#endif

	}

void CC32ThreadManager::SetDefaults()
// called if the initialisation of the threadmanager goes wrong (LoadCMIDataL leaves), which is probably when
// there is no memory. So this function sets up the thread manager into a state so that it is still
// internally consistent and able to load modules so C32 can keep booting. If memory is available
// by the time a module load request is received, it can be loaded successfully.
	{
	iDefaultThreadIndex = KMainThreadId;
	
	// by destroying whatever is in the csylist, all CSYs will default to the main thread
	iCSYList.ResetAndDestroy();

	// we don't need the thread list anymore - all csy load attempts will go to default thread
	// so this list will never be checked. If a player is loading at boot as well,
	// the bind at that stage will see the list is destroyed and the player will just sit there unused.
	iC32ThreadList.ResetAndDestroy();
	
	// iCPMLoader will be empty at this point, so we leave it null.
	}


//
//	CC32Dealer
//


CC32Dealer* CC32Dealer::NewL(CC32WorkerThread* aOwnerThread, const TDesC8& aDealerIniData)
	{
	CC32Dealer* self = new(ELeave) CC32Dealer(aOwnerThread);
	CleanupStack::PushL(self);
	self->ConstructL(aDealerIniData);
	CleanupStack::Pop(self);
	return self;
	}
	
CC32Dealer::CC32Dealer(CC32WorkerThread* aOwnerThread) 
: CC32Server(aOwnerThread, EPriority), 
iOwnerThread(aOwnerThread)
	{
	}

// aCSYList is the CSY list as extracted from the ini data that the CWorkerThread receives from the
// configurator.
void CC32Dealer::ConstructL(const TDesC8& aDealerIniData)
	{
	// set up the thread manager, which will load the cmi data
	// if this fails we have two possible methods of recovery.
	// The first is to attempt to set some defaults, which at least allows c32 to provide service
	// If this fails too (possibly low memory) then we mark the Dealer as having failed to start
	// which then means it will not start its server.

	TRAPD(ret,iThreadManager = CC32ThreadManager::NewL(this));
	if (ret!=KErrNone)
		{
		C32LOG2(KC32Bootup,_L8("CC32Dealer::ConstructL: Could not create thread manager so setting iFailedStartup TRUE. Ret=%d"),ret);
		iFailedStartup=ETrue;
		} 
	else
		{
		
		TRAP(ret,iThreadManager->LoadCMIDataL(aDealerIniData));
		if (ret != KErrNone)
			{
			C32LOG2(KC32Bootup,_L8("CC32Dealer: CMI data load left with %d. Ignoring CMI files and defaulting to loading all CSYs into main thread."),ret);
			iThreadManager->SetDefaults();
			}
		
		}
	
	
	if(!iFailedStartup)
		{
		iWorkerDataGlobals = CC32WorkerDataGlobals::NewL();

		TC32WorkerThreadRegister& mainProperties = *WorkerDataGlobals().GetWorkerGlobals(TC32WorkerThreadPublicInfo::EMainThread);
		// players set their heap when they get introduction from main thread, but
		// as this does not happen for main thread we set main thread's heap here separately
		mainProperties.iHeap = &User::Heap();
		mainProperties.iDealer = this;
		if(!iOwnerThread->Player())
			{
			mainProperties.iPlayer = iOwnerThread->Player();	
			}

#ifdef _DEBUG
		if(iOwnerThread->AllocFailType() != RAllocator::ENone)
			{
			WorkerDataGlobals().GetWorkerGlobals(TC32WorkerThreadPublicInfo::EMainThread)->iHeap->__DbgSetAllocFail(iOwnerThread->AllocFailType(), iOwnerThread->AllocFailRate());
			}
#endif

		C32LOG1(KC32Dealer,_L8("calling inherited ConstructL()"));
		inherited::ConstructL(iThreadManager);
		}
	}


/**
When deleting a session it will be in response to a disconnect from the client, so we
signal that to the session. If this thread is in the process of shutting down we need to
check whether it was just waiting for this session (if this was the last one) and if this is
the case we can initiate the asynchronous process of shutting down the worker thread.
*/
void CC32Dealer::DeleteSession(CCommSession* aSession) 
	{ 
	// If we ever see evidence of shutdowns nested inside active calls (ie "this" deleted prematurely then consider
	// switching handling to use a CAsyncOneShot)
	aSession->CompleteDisconnect();
	if(WorkerThread().ShuttingDown() && CanShutdown())
		{
		WorkerThread().SetDealerShutdownComplete(ETrue);
		WorkerThread().MaybeTriggerThreadShutdownCallback();
		}
	}


/**
Iterate through all sessions and make sure they deploy the SubSessionProcessor on each owned sub-session.
It is not known here what the SubSessionProcessor actually does as it is implemented by the caller.
*/
void CC32Dealer::ProcessSubSessions(TWorkerId aPeerId, CCommSession::TSubSessionProcessor aSubSessionProcessor, TAny* aPtr)
	{
	iSessionIter.SetToFirst();
	CCommSession* sess;
	while((sess = static_cast<CCommSession*>(iSessionIter++)) != NULL)
		{
		sess->ProcessSubSessions(aPeerId, aSubSessionProcessor, aPtr);
		}
	}

TInt CC32Dealer::SubsessionCountInPlayer(TWorkerId aPeerId)
	{
	TInt numSubSessions = 0;
	ProcessSubSessions(aPeerId, CCommSession::CountSubSessions, &numSubSessions);
	return numSubSessions;
	}
	
CC32Dealer::~CC32Dealer()
	{
	C32LOG1(KC32Shutdown,_L8("CC32Dealer::~CC32Dealer()."));

#ifdef __FLOG_ACTIVE
	iSessionIter.SetToFirst();
	CCommSession* pSession;
	// we log any sessions remaining
	while((pSession=static_cast<CCommSession*>(iSessionIter++))!=NULL)
		{
		C32LOG2(KC32Shutdown, _L8("CC32Dealer::~CC32Dealer(): Session(%08x): Remaining. WARNING: this will cause objects to leak."), pSession);
		}
#endif
	
	iParkedRequests.Close();
	delete iThreadManager;
	iThreadManager = NULL;

	if (WorkerDataGlobalsExist())    //won't exist if we've OOM'd during construction and reached here during cleanup.
		{
		for(TWorkerId id = TC32WorkerThreadPublicInfo::EMaxWorkerThreadId; id >=  TC32WorkerThreadPublicInfo::EFirstPlayerThread; --id)
			{
			if(WorkerExists(id))
				{
				FreeWorkerReferences(id);
				}
			}
		}

		
	delete iWorkerDataGlobals;
	iWorkerDataGlobals = NULL;
	C32LOG(KC32Shutdown,_L8("CC32Dealer::~CC32Dealer() complete."));

	}

void CC32Dealer::ShutdownIfReady()
	{
	ASSERT(iOwnerThread->WorkerId() == TC32WorkerThreadPublicInfo::EMainThread); // protect against this being called for a dealerPlayer
	C32LOG2(KC32Shutdown, _L("CC32Dealer::ShutdownIfReady() - iSessionShutdownComplete = %d"), iSessionShutdownComplete );
	if(iSessionShutdownComplete)
		{
		iOwnerThread->TriggerThreadShutdownCallback();
		}
	}


/** 
Used during binding when the Dealer receives a introduction response message from a worker.
The Dealer will set-up housekeeping datastructures for the worker
@see TC32WorkerMsg::EMainIntroductionResp
*/

void CC32Dealer::ProcessWorkerIntroductionResponse(const TC32WorkerMainIntroductionRespMsg& aMsg)
	{
	const TC32WorkerThreadPublicInfo& msgInfo = aMsg.WorkerInfo();
	C32LOG2(KC32Dealer, _L("Process Worker Introduction Response for worker: %d"),msgInfo.iWorkerId);
	ASSERT(msgInfo.iWorkerId > TC32WorkerThreadPublicInfo::EMainThread && msgInfo.iWorkerId <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId);
	ASSERT(!WorkerDataGlobals().WorkerPresent(msgInfo.iWorkerId));
	TC32WorkerThreadRegister& workerReg = *WorkerDataGlobals().GetWorkerGlobals(msgInfo.iWorkerId);
	static_cast<TC32WorkerThreadPublicInfo&>(workerReg) = msgInfo;
	workerReg.iDealer = workerReg.iWorker->Dealer();
	workerReg.iPlayer = workerReg.iWorker->Player();
	RemovePendingIntroductionResponse();

	// process response message. Process parked request here
    ProcessParkedRequest();

#ifdef _DEBUG
  	// We only switch on the configured simulated allocation failures once the bindings are complete, because
  	// it's too hard to recover from them earlier. This is a regrettable but hopefully harmless limitation in
  	// practice, ie if we're OOM during boot then recovery strategies aren't obvious.
  	workerReg.iHasGlobalAllocFails = aMsg.FailType() != RAllocator::ENone;
  	if(workerReg.iHasGlobalAllocFails)
  		{
  		workerReg.iHeap->__DbgSetAllocFail(aMsg.FailType(), aMsg.FailRate());
  		}
#endif

	}

void CC32Dealer::DeleteCPMLoader(CommsFW::TWorkerId aWorkerId)
	{
	// remove waiting LoadCPM object
	iThreadManager->ProcessModuleLoadSuccess(aWorkerId);
	}

void CC32Dealer::RemoveParkedRequestsOnSessionClose(CCommSession* aSession)
	{
	const TInt parkedCount = iParkedRequests.Count();
	for(TInt i = parkedCount - 1; i >= 0; --i)
 		{
		const TSessionMessagePair& pair = iParkedRequests[i];
		if(pair.iSession == aSession)
			{
			C32LOG2(KC32Dealer, _L8("CC32Dealer::RemoveParkedRequest() - found parked for closing session %08x "), aSession);
			SafeComplete(pair.iMessage, KErrSessionClosed);		// session is being closed
			iParkedRequests.Remove(i);
		 	}
 		}
	}

void CC32Dealer::ProcessParkedRequest()
	{
	 const TInt parkedCount = iParkedRequests.Count();
	 C32LOG2(KC32Bootup, _L8("CC32Dealer::ProcessParkedRequestL() - %d parked messages to process"), parkedCount);
	 for(TInt i = parkedCount - 1; i >= 0; --i)
 		 {
 		 const TSessionMessagePair& pair = iParkedRequests[i];
 		 // Check that the session still exists
 		 iSessionIter.SetToFirst();
 		 CSession2* ss;
 		 while((ss = iSessionIter++) != NULL && ss != pair.iSession)
	 		 {
	 		 }
		 if(ss && !pair.iMessage.IsNull())
	 		 {
	 		 C32LOG2(KC32Bootup, _L8("CC32Dealer::ProcessParkedRequestL() - parked messages for sess %08x"), ss);
	 		 RMessage2 tempMsg = pair.iMessage;
	 		 iParkedRequests.Remove(i);		// remove parked request
		 	 TRAPD(res, ss->ServiceL(tempMsg));
		 	 if(res != KErrNone)
				{
		 	 	SafeComplete(tempMsg, res);
		 	 	}
	 		 }
 		 }
	 }

TInt CC32Dealer::ParkRequest(CCommSession* aSession, const RMessage2& aMessage)
	 {
	 __ASSERT_DEBUG(aMessage.Function()==ECommLoadCommModule,Fault(EBadState,_L8("CC32Dealer::ParkRequest - Attempt to park a non-moduleLoad request. Panicking. Session(%08x) Message(%08x)"), aSession, aMessage.Handle()));
	 
	 C32LOG3(KC32Bootup, _L8("CC32Dealer::ParkRequest Session(%08x) Message(%08x)"), aSession, aMessage.Handle());
	 return iParkedRequests.Append(TSessionMessagePair(aSession, aMessage));
	 }


TInt CC32Dealer::LoadCPMOnLoadCommModule(CommsFW::TWorkerId aWorker)
	 {
	 return iThreadManager->RequestLoadModule(aWorker);
	 }
	 
	 
void CC32Dealer::ProcessFailedPlayerLoad(CommsFW::TWorkerId aWorker)
// called from CPMLoader if configurator reported load failed.
// gives dealer a chance to do any cleanup.
	{
	// sweep parked requests array and remove any CSY load requests that were hoping to be loaded into the
	// failed player
	// They will be completed with appropriate KErrNoMemory (most likely reason for player load to fail)
	// error code. Transversing the array is somewhat expensive since we're reprocessing all queued messages.
	TFileName csyfilename;
	C32LOG2(KC32Dealer,_L8("CC32Dealer:ProcessFailedPlayerLoad: Sweeping parked requests array to complete(w/KErrNoMemory) and remove any waiting on failed player %d"),aWorker);
	const TInt parkedCount = iParkedRequests.Count();
	for (TInt i = parkedCount -1 ;i >= 0; --i)
		{
	    const TSessionMessagePair& pair = iParkedRequests[i];
		TInt res = pair.iMessage.Read(0,csyfilename);
		
		TBuf8<KMaxFileName> fileName8;
		fileName8.Copy(csyfilename);
		
		
		if (res==KErrNone)
			{
			TInt r=csyfilename.LocateReverse('.');
		 	if (r==KErrNotFound)
				{
				// length was checked when message first processed before queuing
				__ASSERT_DEBUG(csyfilename.Length() <= KMaxFileName - (TInt)KCSYExtension.iTypeLength, Fault(EBadState,_L("CC32Dealer::ProcessFailedPlayerLoad: Message(%08x) filename is too long. Panicking."),&pair.iMessage));
				csyfilename.Append(KCSYExtension);
				}

			CommsFW::TWorkerId worker;
			TBool found = iThreadManager->FindThreadByFileName(fileName8,worker);
			

			
			if (!found)
				{
				// CSY must have been destined for the default thread
				worker = iThreadManager->DefaultThread();
				}
			
			if (worker == aWorker)
				{
				C32LOG3(KC32Detail,_L8("CC32Dealer:ProcessFailedPlayerLoad: Completing and removing message(%08x) at index %d"),&pair.iMessage,i);
				SafeComplete(pair.iMessage,KErrNoMemory);
				iParkedRequests.Remove(i);
				}
			
			}
		else
			{
			__ASSERT_DEBUG(res==KErrNone,Fault(EBadState,_L8("CC32Dealer::ProcessFailedPlayerLoad: Could not read filename from Message at iParkedRequests[%d]. Res=%d Panicking."),i,res));
			// in release mode just ignore mesg and keep going
			}

		}
		
	}


/** ThreadManager is updated with CSerial* and PortPrefix. PortPrefix is mapped to CSYFileName 
* and this mapping then provides CSYFileName, which is searched in CSYcontainer in session to 
* test whether the CSY in question is loaded in this session or not - this mapping is used while
* closing Comm Module
*/
void CC32Dealer::ProcessLoadCommModuleSuccessResponse(const RMessage2& aMessage, CSerial* aSerial)
// called from workerthread dispatchMsg, or directly if load is for co-resident player
	{
	/** Map find CSYFileName in ThreadManager and update PortPrefix and CSerial* field against it. In case of duplication, 
	* the CSerial* will be updated again though it'll be point to the same physical address
	*/
	
	TFileName csyFilename;
	Read(0, aMessage, csyFilename, 0);
	TInt r=csyFilename.LocateReverse('.');
 	if (r==KErrNotFound)
		{
		csyFilename.Append(KCSYExtension);
		}
	C32LOG2(KC32Dealer, _L("CC32Dealer::ProcessLoadCommModuleSuccessResponse, insert CSerial* and PortPrefix for: %S"),&csyFilename);

	iThreadManager->UpdateThreadManagerOnCSYLoad(csyFilename, aSerial->Name(), aSerial);
	
	// complete message in dealer after update is done in ThreadManager
	SafeComplete(aMessage, KErrNone);
	}

void CC32Dealer::ProcessLoadCommModuleFailureResponse(const RMessage2& aMessage, TInt aFailReason)
	{
	TBool dummy;
	TFileName csyFilename;
	Read(0, aMessage, csyFilename, 0);
	
	TInt ret = AddCSYExtension(csyFilename,aMessage);
	__ASSERT_DEBUG(ret == KErrNone, Fault(EBadState,_L("CC32Dealer::ProcessLoadCommModuleFailureResponse: Could not add extension to CSY filename. This is inconsistent since should not get here in such case. Panicking.")));
		

	C32LOG1(KC32Dealer, _L("CC32Dealer::ProcessLoadCommModuleFailureResponse, remove CSY from Session csy container and de-allocate memory for this csy in ThreadManager"));
	static_cast<CCommSession*>(aMessage.Session())->RemoveCSYFromSession(csyFilename,dummy);
	// de-allocate memory on this call
	iThreadManager->UpdateThreadManagerOnCSYLoadFailure(csyFilename);
	// complete message after removing csy from session CSY container and de-allocating memory
	SafeComplete(aMessage, aFailReason);
	}
	

TInt CC32Dealer::Read(TInt aPos, const RMessagePtr2& aMessage , TDes16& aDes, TInt aOffset)
	{
	C32LOG3(KC32Dealer, _L8("CC32Dealer::Read(), Pos (%d), Offset (%d)"), aPos, aOffset);

	TInt ret = aMessage.Read(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		C32LOG1(KC32Dealer, _L8("Error at the time of reading data from client"));
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}

void CC32Dealer::FreeWorkerReferences(TWorkerId aWorkerId)
	{	
	C32LOG2(KC32Shutdown, _L("CC32Dealer::FreeWorkerReferences(%d)"), aWorkerId );
	TC32WorkerThreadRegister& properties(*WorkerDataGlobals().GetWorkerGlobals(aWorkerId));
#ifdef _DEBUG
	// The RootServer normally checks the heap for leaks when the module unloads
	// but for C32 CSY modules this is commonly too early, since the Dealer (acting as PitBoss) holds
	// its reference open until the cleanup completes. Hence here we check for 
	// remaining allocations if we hold the last reference and if no thread 
	// which used it died involuntarily
	RCFSharedHeap* heap = static_cast<RCFSharedHeap*>(properties.iHeap);
	C32LOG4(KC32Warning, _L8("~~~CC32Dealer::FreeWorkerReferences heap(%08x).AccessCount()==%d, cell count=%d"), (TUint) heap, heap->AccessCount(), heap->Count());
	if(heap->AccessCount() <= 2)
		{
		if(heap->Count() > 0)
			{
			//C32LOG2(KRSModule,KRSModuleLeak,_L8("~~~SerComms-CC32Dealer::FreeWorkerReferences.Following leaks are from Ser-Comms C32 (more info logged under: C32SerComms *)"));
			C32LOG1(KRSModule,KRSModuleLeak);
			heap->LogAllocatedCells(RootServer::KLogSubSysRSModule, RootServer::KLogRSLeakTag);
			RProperty pubsub;
			TInt res = pubsub.Attach(RootServer::KUidCommsProcess, RootServer::KUidCommsModuleLeakCounter);
			//No nead for cleanup stack, cannot leave before Close
			if (res == KErrNone)
				{
				TInt count;
				res =pubsub.Get(count);
				if (res == KErrNone)
					{
					count += heap->Count();
					res =pubsub.Set(count);
					}
				}
			pubsub.Close();
			if (res != KErrNone)
				{
				__CFLOG_1(RootServer::KLogSubSysRSModule, RootServer::KLogRSLeakTag, _L8("Unable to report leaks. Error: %d"), res);
				}
			// As much as anything this log line is here to make it apparent that the breakpoint above was hit
			C32LOG1(KC32Warning, _L8("--- end of leaked cell log. If leaks not shown enable logging for: RSModule *"));
			}
		}
#endif
	properties.Clear();
	}			

/** 
Check if we can unbind from a worker. This is only possible if the local Dealer doesn't 
have any outstanding sub-sessions terminating in the peer and it doesn't have any sessions 
with outstanding closes against the peer. Otherwise the channel is still needed. 
*/
TBool CC32Dealer::CanUnbindFromWorker(TWorkerId aWorker)
	{
	if (!WorkerThread().DealerByRef().Player(aWorker))
		{
		return ETrue;
		}
	if(SubsessionCountInPlayer(aWorker) == 0)
		{
		// Check for any sessions which have outstanding session closes against the Worker
		iSessionIter.SetToFirst();
		CCommSession* sess;
		while((sess = static_cast<CCommSession*>(iSessionIter++)) != NULL)
			{
			if(sess->IsPlayerInDisconnectList(aWorker))
				{
				return EFalse;
				}
			}
		return ETrue;
		}
	return EFalse;
	}
	

/** 
Check whether a worker Id is legal and a worker with that Id is installed.
*/
TBool CC32Dealer::WorkerExists(TWorkerId aId) const
	{
	return WorkerDataGlobals().WorkerPresent(aId);
	}

void CC32Dealer::SessionShutdownComplete()
	{
	iSessionShutdownComplete = ETrue;
	ShutdownIfReady();
	}


void CC32Dealer::PostMessage(TWorkerId aWorkerId, TCFMessage& aMessage)
	{
	iOwnerThread->PostMessage(aWorkerId, aMessage);
	}

#ifdef _DEBUG
  // If a heap has been configured from boot to have a failure mode then we don't override this here as
  // the lifetime failure testing is more important than the specific test case doing a SetFailNext
  void CC32Dealer::SetFailNextForAllHeaps(TInt aFailNext)
  	{
  	for(TWorkerId workerId = TC32WorkerThreadPublicInfo::EMainThread; workerId <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId; ++workerId)
  		{
  		const TC32WorkerThreadRegister& worker = *WorkerDataGlobals().GetWorkerGlobals(workerId);
  		if(WorkerExists(workerId) && !worker.iHasGlobalAllocFails)
  			{
  			worker.iHeap->__DbgSetAllocFail((aFailNext < 0)? RAllocator::EReset: RAllocator::EFailNext, aFailNext);
  			}
  		}
  	}
#endif

/** 
Given the "local" worker thread's id (normally but not necessarily the current worker
thread and that of another "foreign" worker thread, if they have different heaps then 
switch heaps and return the previous one, otherwise return NULL. 
*/
RAllocator* CC32Dealer::MaybeSwitchHeap(TWorkerId aForeignWorkerId)
	{
	
	const TC32WorkerThreadPublicInfo& foreignInfo = *WorkerDataGlobals().GetWorkerGlobals(aForeignWorkerId);
	ASSERT(foreignInfo.iHeap);
	RHeap* heap = &User::Heap();
	if(heap != foreignInfo.iHeap)
		{
		C32LOG2(KC32Detail, _L8("CC32Dealer::MaybeSwitchHeap - Switching heap to %08x."),foreignInfo.iHeap);
		return User::SwitchAllocator(foreignInfo.iHeap);
		}
	else
		{
		C32LOG2(KC32Detail, _L8("CC32Dealer::MaybeSwitchHeap - No heap switch happened - Heap %08x."),heap);
		}	
		
	return NULL;
	}

/**
Called by any Player/thread to add a sub-session to a session. It will switch the local heap to that 
of the peer before performing operations on the session pointer. It is essential that the session lock is used
around this call.
@see CC32SubSessionIx::Lock
@see CC32SubSessionIx::UnLock
*/
TInt CC32Dealer::AddSubSession(CCommSubSession* aSubSession, CCommSession* aSession, TInt& aHandle)
	{
	RAllocator* prevAllocator = MaybeSwitchHeap(aSession->WorkerId());
	TInt err = aSession->iSubSessions.Add(aSubSession, aHandle);
	C32LOG4(KC32Detail, _L8("CC32Dealer::AddSubSession(%08x, %08x, => %08x)"), aSubSession, aSession, aHandle);
	if(prevAllocator)
		{
		User::SwitchAllocator(prevAllocator);
		}
	return err;
	}

/**
Called by any Player/thread to remove a sub-session from a session. It will switch the local heap to that 
of the peer before performing operations on the session pointer. It is essential that the session lock is used
around this call.
@see CC32SubSessionIx::Lock
@see CC32SubSessionIx::UnLock
*/
void CC32Dealer::RemoveSubSession(TInt aHandle, CCommSession* aSession)
	{
	RAllocator* prevAllocator = MaybeSwitchHeap(aSession->WorkerId());
	C32LOG3(KC32Detail, _L8("CC32Dealer::RemoveSubSession(%08x, %08x"), aHandle, aSession);
	VERIFY(aSession->iSubSessions.Remove(aHandle) != NULL);
	if(prevAllocator)
		{
		User::SwitchAllocator(prevAllocator);
		}
	}

/**
Called by any Player/thread to find handle for a sub-session. It will switch the local heap to that 
of the peer before performing operations on the session pointer. It is essential that the session lock is used
around this call.
@see CC32SubSessionIx::Lock
@see CC32SubSessionIx::UnLock
*/
TInt CC32Dealer::FindSubSession(CCommSubSession* aSubSession, CCommSession* aSession, TInt& aHandle)
	{
	RAllocator* prevAllocator = MaybeSwitchHeap(aSession->WorkerId());
	TInt err = aSession->iSubSessions.Find(aSubSession, aHandle);
	C32LOG4(KC32Detail, _L8("CC32Dealer::FindSubSession(%08x, %08x, => %08x)"), aSubSession, aSession, aHandle);
	if(prevAllocator)
		{
		User::SwitchAllocator(prevAllocator);
		}
	return err;
	}

CC32Dealer* CC32Dealer::Dealer(TWorkerId aWorkerId) const
	{
	return WorkerDataGlobals().GetWorkerGlobals(aWorkerId)->iDealer;
	}

CC32Player* CC32Dealer::Player(TWorkerId aWorkerId) const
	{
	return WorkerDataGlobals().GetWorkerGlobals(aWorkerId)->iPlayer;
	}

void CC32Dealer::SendIntroductionToWorker(CommsFW::TWorkerId aWorkerId)
	{
	// Send the worker peer the Dealer introduction message
	TC32WorkerMainIntroductionMsg msg(this);
	PostMessage(aWorkerId, msg);
	AddPendingIntroductionResponse();
	}

void CC32Dealer::AddPendingIntroductionResponse()
	{
	++iPendingIntroResponses;
	}

void CC32Dealer::RemovePendingIntroductionResponse()
	{
	--iPendingIntroResponses;
	}

/**
This is a simple check shutting down is not possible if there are still open sessions, unless
shutdown type is EImmediate but then this method is not called. 
*/
TBool CC32Dealer::CanShutdown()
	{
	CC32Data* globals = CC32DataInTls();
    C32LOG2(KC32Detail, _L8("CC32Dealer::CanShutdown() \t iNumSessions = %d "), globals->iNumSessions);
	return globals->iNumSessions <= 0;
	}


void CC32Dealer::ProcessShutdownRequest(CommsFW::TCFShutdownType aType)
// Since the c32 framework mimmicks esock's but does not yet contain a DealerPlayer concept,
// this function will be called by whichever thread sees a shutdown first.
	{
	TBool commenceShutdown = CanShutdown();

	// If have to do now and there are still sessions then we exit anyway but suppress the heap check and log a rude message. We
	// used to delete the sessions but that isn't safe
	if(EImmediate==aType)
		{
    	commenceShutdown = ETrue;

#ifdef __FLOG_ACTIVE
		iSessionIter.SetToFirst();
		if(iSessionIter++ != NULL)
			{
			TInt cnt = 0;
			iSessionIter.SetToFirst();
			CSession2* ss;
			while((ss = iSessionIter++) != NULL)
				{
				C32LOG2(KC32Detail, _L8("<==Session(%08x): remaining"), ss);
				++cnt;
				}
			C32LOG2(KC32Detail, _L8("NB! Immediate shutdown commanded but #%d client sessions remaining (bad test code?)"), cnt);
			}
#endif
		}

	if(WorkerThread().IsMainThread())
		{
		// this section will always be called by each thread since no dealerPlayer yet
		
		// Even the Dealer (acting as pitboss) yields to the immediate shutdown
		if(commenceShutdown)
			{
			WorkerThread().DealerByRef().SessionShutdownComplete();
			}
		}
	WorkerThread().SetDealerShutdownComplete(commenceShutdown);
	}

//
// TC32WorkerThreadRegister
//

TC32WorkerThreadRegister::TC32WorkerThreadRegister()
	{
#ifdef _DEBUG
  	iHasGlobalAllocFails = EFalse;
#endif
	}

void TC32WorkerThreadRegister::Clear()
	{
	inherited::Clear();
	iDealer = NULL;
	iPlayer = NULL;
	}

//
// CC32SubSessionIx
//

void CC32SubSessionIx::InitialiseL()
	{
	User::LeaveIfError(iLock.CreateLocal());
	}

CC32SubSessionIx::~CC32SubSessionIx()
	{
	iLock.Close(); 
	User::Free(iIx);
	}

TInt CC32SubSessionIx::Find(CCommSubSession* aSubSession, TSubSessionHandle& aHandle) const
	{
	iLock.AssertLockHeld();
	for(TInt ix = 0; ix < iSize; ++ix)
		{
		TEntry& e = iIx[ix];
		if(e.iType != CCommSubSession::ENull && e.iObject == aSubSession)
			{
			aHandle = MakeHandle(ix, e.iMagic, e.iType);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

TInt CC32SubSessionIx::Add(CCommSubSession* aSubSession, TSubSessionHandle& aHandle)
	{
	iLock.AssertLockHeld();
	if(TUint(iFreeListHead) >= TUint(iSize))
		{
		TInt err = ExpandArray();
		if(err != KErrNone)
			{
			return err;
			}
		}
	TInt ix = iFreeListHead;
	TEntry& e = iIx[ix];
	ASSERT(e.iType == CCommSubSession::ENull);
	iFreeListHead = e.iNextFree;
	ASSERT(TUint(iFreeListHead) <= TUint(iSize));
	e.iObject = aSubSession;
	e.iType = TUint8(aSubSession->Type());
	TInt magic = (e.iMagic + 1) & KMagicMask;
	e.iMagic = TUint16(magic);
	aHandle = MakeHandle(ix, magic, e.iType);
	++iActiveCount;
	return KErrNone;
	}

CCommSubSession* CC32SubSessionIx::Remove(TSubSessionHandle aHandle)
	{
	iLock.AssertLockHeld();
	ASSERT(TUint(iFreeListHead) <= TUint(iSize));
	TEntry* e = At(aHandle);
	ASSERT(e);
	CCommSubSession* subSess = e->iObject;
	e->iType = TUint8(CCommSubSession::ENull);
	e->iNextFree = iFreeListHead;
	iFreeListHead = e - &iIx[0];
	ASSERT(TUint(iFreeListHead) < TUint(iSize));
	--iActiveCount;
	return subSess;
	}

CCommSubSession* CC32SubSessionIx::At(TInt aHandle, CCommSubSession::TSubSessionType aType) const
	{
	iLock.AssertLockHeld();
	TEntry* e = At(aHandle);
	if(e && (e->iType == aType || aType == CCommSubSession::EAny))
		{
		return e->iObject;
		}
	return NULL;
	}

CC32SubSessionIx::TEntry* CC32SubSessionIx::At(TSubSessionHandle aHandle) const
	{
	TEntry* e = 0;
	TInt ix = IndexFromHandle(aHandle);
	if(TUint(ix) < TUint(iSize))
		{
		e = &iIx[ix];
		if(MakeHandle(ix, e->iMagic, e->iType) != aHandle)
			{
			e = 0;
			}
		}
	return e;
	}

TInt CC32SubSessionIx::ExpandArray()
	{
	ASSERT(TUint(iFreeListHead) <= TUint(iSize));
	TInt size = iSize + EIndexGranularity;
	if(size > KIndexLimit)
		{
		return KErrNoMemory;
		}
	TEntry* ix = reinterpret_cast<TEntry*>(User::ReAlloc(iIx, size * sizeof(TEntry)));
	if(!ix)
		{
		return KErrNoMemory;
		}
	iIx = ix;
	iSize = size;
	for(TInt ii = iFreeListHead; ii < size;)
		{
		TEntry& e = ix[ii];
		e.iNextFree = ++ii;
		e.iType = TUint8(CCommSubSession::ENull);
		}
	return KErrNone;
	}

CC32SubSessionIx::TIter::TIter(CC32SubSessionIx& aContainer)
: iContainer(aContainer)
	{
	SetToFirst();
	}

void CC32SubSessionIx::TIter::SetToFirst()
	{
	iPos = 0;
	}

CCommSubSession* CC32SubSessionIx::TIter::operator++(TInt)
	{
	TSubSessionHandle dummyHandle;
	return Next(dummyHandle);
	}

CCommSubSession* CC32SubSessionIx::TIter::Next(TSubSessionHandle& aHandle)
	{
	iContainer.iLock.AssertLockHeld();
	TEntry* e = &iContainer.iIx[iPos];
	while(iPos < iContainer.iSize)
		{
		TInt oldPos = iPos++;
		if(e->iType != CCommSubSession::ENull)
			{
			aHandle = iContainer.MakeHandle(oldPos, e->iMagic, e->iType);
			return e->iObject;
			}
		++e;
		}
	return NULL;
	}


//
//	CC32Player class definitions
//


CC32Player* CC32Player::NewL(CC32WorkerThread* aOwnerThread)
	{
	CC32Player* self = new(ELeave) CC32Player(aOwnerThread);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CC32Player::CC32Player(CC32WorkerThread* aOwnerThread)
: iOwnerThread(aOwnerThread)
	{
	}

void CC32Player::ConstructL()
	{
	iPortManager = CPortManager::NewL();
	}

/** 
The Player destructor doesn't have much to do as a lot of the cleanup is done during the
normal shutdown routines. Here the Player merely deletes all sub-sessions it owns. 
*/
CC32Player::~CC32Player()
	{
	// The object container is stored as a packed array, so working backwards through it avoids invalidating
	// the iterator when removing entries (and as a bonus is more efficient)
	C32LOG1(KC32Shutdown, _L8("CC32Player::~CC32Player()"));
	for(TInt i = iSubSessions.Count() - 1; i >= 0; --i)
		{
		CCommSubSession* subSession = iSubSessions[i];
		C32LOG2(KC32Shutdown, _L8("-- destroying -- %08x"), subSession );
		// This subsession needs deletion, which we do by closing it until it disappears
		// (generally this should only require a single close, but it's possible that the
		// Dealer died with it additionally open)
		ASSERT(subSession);
		ASSERT(subSession->AccessCount() > 0);
		// Following logic presumes that each Close() decrements AccessCount() by one;
		// anything else is too broken for words
		for(TInt cnt = subSession->AccessCount(); cnt > 0; --cnt)
			{
			CPort* p = CPortFromSubSession(subSession);		// subSession->iPort
			p->Close();							// close CPort
			subSession->Close();				// close CCommSubSession
			}
		}
	iSubSessions.ResetAndDestroy();
	delete iPortManager;
	iPortManager = NULL;
	}

/**
The Player can unbind from another worker thread if it doesn't have any sub-sessions 
belonging to a session in the peer. 
*/
TBool CC32Player::CanUnbindFromWorker(TWorkerId aWorker)
	{
	C32LOG3(KC32Bootup, _L8("CC32Player::CanUnbindFromWorker(%d): %d subsess"), aWorker, iSubSessions.Count() );
	for(TInt idx = iSubSessions.Count() - 1; idx >= 0; --idx)
		{
		CCommSubSession* ss = iSubSessions[idx];
		C32LOG3(KC32Bootup, _L8("-- subsess %08x worker=%d"), ss, ss->Session()->WorkerId());
		if(iSubSessions[idx]->Session()->WorkerId() == aWorker)
			{
			return EFalse;
			}
		}
	return ETrue;
	}

/** 
Check whether the Player is ready to shut down and if so, tells the Worker Thread who owns it. 
*/
void CC32Player::MaybeSetPlayerShutdownComplete(TBool aForceShutdownNow)
	{
	TBool shutdownNow = aForceShutdownNow || (iSubSessions.Count() == 0);
	C32LOG4(KC32Bootup, _L8("CC32Player::MaybeSetPlayerShutdownComplete(), shutdownNow = %d [forced=%d, #subSess=%d]"), 
		shutdownNow, aForceShutdownNow, iSubSessions.Count() );
	WorkerThread().SetPlayerShutdownComplete(shutdownNow);
	}

/**
If an incoming shutdown request is of type EImmediate, informs the
Worker Thread that Player shutdown is complete, otherwise do nothing here. 
*/
void CC32Player::ProcessShutdownRequest(CommsFW::TCFShutdownType aType)
	{
	C32LOG2(KC32Bootup, _L8("CC32Player::ProcessShutdownRequest(%d)"), aType );
	WorkerThread().SetPlayerShutdownComplete(aType == EImmediate);
	}

void CC32Player::ProcessUnLoadCommModuleMsg(CSerial* aSerial)
	{
	__ASSERT_DEBUG(aSerial != NULL, Fault(EBadState, _L8("NULL CSerial* passed to player !")));
	__ASSERT_DEBUG(aSerial->AccessCount() > 0, Fault(EBadState, _L8("CC32Player::ProcessUnLoadCommModuleMsg - AccessCount < 0 before Close()")));
	if(aSerial->AccessCount() > 0)
		{
		aSerial->Close();
		}
	else
		{
		C32LOG2(KC32Warning, _L8("CC32Player::ProcessUnLoadCommModuleMsg - AccessCount negative  %d, Cannot call close()"), aSerial->AccessCount() );
		}
	return;
	}

/**
Write a handle back to Ptr3 of the current message
*/
TInt CC32Player::WriteSubSessionHandle(TInt aHandle)
	{
	TPckgC<TInt> pH(aHandle);
	return iSession->Write(MSG_PRM(3), Message(),pH);
	}

// CC32Player::CloseSubSession closes a CPort so CommCancel is not used.
void CC32Player::CloseSubSession(const RMessage2& aMessage, CCommSubSession* aSubSession)
	{
	ASSERT(aSubSession);
	CPort* p = CPortFromSubSession(aSubSession);
	C32LOG1(KC32Player, _L8("CC32Player::CloseSubSession"));
	p->FreeSession(aSubSession->Session());		// check if CPort can be freed in this session
	p->Close();								// close CPort
	if(aSubSession->AccessCount()==1)
	// the access count == 1 and subsession is being closed. Remove subsession from session's CC32SubSessionIx
		{
		aSubSession->Session()->SubSessions().Lock();
		TInt handle;
		TInt res = CC32DealerByRef().FindSubSession(aSubSession, aSubSession->Session(), handle); 
		if(res == KErrNone)
			{
			CC32DealerByRef().RemoveSubSession(handle, aSubSession->Session());
			}
		else
			{
			__ASSERT_DEBUG(EFalse, Fault(EFindSubSessionFailed, _L8("Could not find subsession in CC32SubSessionIx while subsession count is 1")));
			}
		aSubSession->Session()->SubSessions().Unlock();
		}
	aSubSession->Close();	// close CCommSubSession, removes from TSubSessionContainer if access count drops to zero
	SafeComplete(aMessage, KErrNone);
	}

/**
This method is uses for example when the session is closed by the client whilst still having
active sub-sessions. All outstanding requests on CPort are cancelled using CPort::CommCancel
@see CC32Player::CloseSession
*/
void CC32Player::CloseAllOwnedSubSessions(CCommSession* aSession)
	{
	// The object container is stored as a packed array, so working backwards through it avoids invalidating
	// the iterator when removing entries (and as a bonus is more efficient)
	C32LOG2(KC32Player, _L8("CC32Player::CloseAllOwnedSubSessions() subsessions in TSubSessionContainer = %d"), iSubSessions.Count() );
	for(TInt i = iSubSessions.Count() - 1; i >= 0; --i)
		{
		CCommSubSession* subSession = iSubSessions[i];
		if(subSession->Session() == aSession)		// close subsessions belonging to the closing session
			{
			C32LOG2(KC32Player, _L8("CC32Player::CloseAllOwnedSubSessions()-closing subsession %x"), iSubSessions[i] );
			// close all subSessions in 'this' aSession as session is being closed.
			// may not lead to ~CPort being called if CPort is being shared across sessions
			// AccessCount of all subsession = AccessCount of CPort
			while(subSession->AccessCount() >= 1)
				{
				CPort* p = CPortFromSubSession(subSession);		// subSession->iPort
				C32LOG3(KC32Player, _L8("CC32Player::CloseAllOwnedSubSessions()-Closing CPort %x for subsession %x"),p, iSubSessions[i] );
				p->CommCancel(0, aSession);	// cancel all outstanding requests
				p->Close();					// close all CPorts in this session
				if (subSession->AccessCount() == 1)
                    {
                    subSession->Close();        // close CCommSubSession. This also deletes object
                    break; //avoid accessing deleted subSession object 
                    }
                else
                    {
                    subSession->Close();        // close CCommSubSession
                    }
				}
			}
		}
	}

/** 
Used when the dealer signals it is closing a session and the Player needs to cleanup 
all resources related to/owned by the session.
@see TPlayerMsg::ESessionClose
*/
void CC32Player::CloseSession(CCommSession* aSession)
	{
	// Remove all subsessions in this aSession
	CloseAllOwnedSubSessions(aSession);
	}

#ifdef _DEBUG
// I'm worried that we'll have dangling session usage, hence this debug-build deliberate
// corruption of it
static void CorruptSessionPointer(TAny* aSessionPtr)
	{
	CCommSession** sessPtr = reinterpret_cast<CCommSession** >(aSessionPtr);
	*sessPtr = reinterpret_cast<CCommSession* >(-1);
	}
#endif

CPort* CC32Player::CPortFromSubSession(CCommSubSession* aSubSession)
	{
	return aSubSession->iPort;
	}

/** 
Process the client message forwarded from the Dealer. For subsession-Close commands
the Dealer provides the subsession pointer explicitly as it has already been removed from
the index. 
*/
void CC32Player::ProcessMessageL(const RMessage2& aMessage, CCommSubSession* aSubSession)
	{
	iSession = static_cast<CCommSession*>(aMessage.Session());
	iCurrentMessage = &aMessage;
	C32LOG4(KC32Player, _L8("CC32Player:\tProcessMessageL: session=%08x, subsess=%08x, Message(%08x) "), iSession, aSubSession, aMessage.Handle());	
	iComplete = ETrue;
#ifdef _DEBUG
	CleanupStack::PushL(TCleanupItem(CorruptSessionPointer, &iSession));
#endif

	switch (aMessage.Function())
        {
	case ECommLoadCommModule:
		LoadCommModule(aMessage);	
#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
	case ECommCloseCommModule:
		CloseCommModule(aMessage);
#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
	case ECommPortInfoByName:
		{
		TPortName name;
		Read(1,aMessage,name);
		PortInfo(aMessage,name);
#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
		}
	//case ECommPortInfoByNumber:	as the global index is not valid in player !
	// instead CSerial* is forwarded to player to obtain PortInfo through TC32PlayerGetPortInfoMsg msg
        }

	if((aMessage.Function()==ECommOpen)
	   ||
	   (aMessage.Function()==ECommOpenWhenAvailable))
		{
		NewPortL(aMessage);
#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
		}

	if (aMessage.Function()==ECommClose)
		{
		CloseSubSession(aMessage, aSubSession);
#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
		}
	
	CPort *p = CPortFromSubSession(aSubSession);
	
	if(p->SessionHasBeenPreempted(iSession))
		{
		SafeComplete(aMessage, KErrCancel);
#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
		}

	// These functions need to be dispatched before the check below is made
	if(aMessage.Function()==ECommSetAccess)
		{
		p->CommSetAccess(aMessage, *iSession);
#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
		}
	else if(aMessage.Function()==ECommOpenWhenAvailableCancel)
		{
		if (p->IsBlockedSetAccessWaiting(*iSession))
			{
			p->CommSetAccessCancel(0, iSession);
			SafeComplete(aMessage, KErrNone);
			}
		else
			{
			SafeComplete(aMessage, KErrNotFound);
			}

#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
		}

	// Any other requests will be denied until the port is fully open
	if(p->SessionIsAwaitingOpen(iSession))
		{
		SafeComplete(aMessage, KErrNotReady);
#ifdef _DEBUG
		CleanupStack::PopAndDestroy();
#endif
		return;
		}

	switch (aMessage.Function())
       {
    case ECommRead:
    	p->CommRead(aMessage, iSession);
    	break;
	case ECommReadCancel:
		p->CommReadCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);
		break;
	case ECommQueryReceiveBuffer:
		p->CommQueryReceiveBuffer(aMessage, *iSession);
		break;
	case ECommResetBuffers:
		p->CommResetBuffers(aMessage, *iSession);
		break;
	case ECommWrite:
		p->CommWrite(aMessage, iSession);
		break;
	case ECommWriteCancel:
		p->CommWriteCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);
		break;
	case ECommBreak:
		p->CommBreak(aMessage, iSession);
		break;
	case ECommBreakCancel:
		p->CommBreakCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);
		break;
	case ECommCancel:
		p->CommCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);
		break;
	case ECommConfig:
		p->CommConfig(aMessage, *iSession);
		break;
	case ECommSetConfig:
		p->CommSetConfig(aMessage, *iSession);
		break;
	case ECommCaps:
		p->CommCaps(aMessage, *iSession);
		break;
	case ECommSetMode:
		p->CommSetServerConfig(aMessage, *iSession);
		break;
	case ECommGetMode:
		p->CommGetServerConfig(aMessage, *iSession);
		break;
	case ECommSignals:
		p->CommSignals(aMessage, *iSession);
		break;
	case ECommSetSignalsToMark:
		p->CommSetSignalsToMark(aMessage, *iSession);
		break;
	case ECommSetSignalsToSpace:
		p->CommSetSignalsToSpace(aMessage, *iSession);
		break;
	case ECommReceiveBufferLength:
		p->CommReceiveBufferLength(aMessage, *iSession);
		break;
	case ECommSetReceiveBufferLength:
		p->CommSetReceiveBufferLength(aMessage, *iSession);
		break;
	case ECommSetAccess:
		p->CommSetAccess(aMessage, *iSession);
		break;

#ifdef _DEBUG
	case ECommDebugState:
		p->CommDebugState( aMessage, *iSession);
		break;
#endif

#ifdef _DEBUG_DEVCOMM
	case ECommDbgDoDumpDebugInfo:
		p->CommDumpDebugInfo(aMessage);
		break;
#endif
       }

	// Extensions to the CCommSession starts from here
   	switch (aMessage.Function())
        {
	case ECommNotifySignals:
		p->CommNotifySignalChange(aMessage, iSession);
		break;
	case ECommNotifySignalsCancel:
		p->CommNotifySignalChangeCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);
		break;
	case ECommNotifyFlowControl:
		p->CommNotifyFlowControlChange(aMessage, iSession);
		break;
	case ECommNotifyFlowControlCancel:
		p->CommNotifyFlowControlChangeCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);	
		break;
	case ECommGetFlowControl:
		p->CommGetFlowControlStatus(aMessage, iSession);
		break;			
	case ECommNotifyConfigChange:
		p->CommNotifyConfigChange(aMessage, iSession);
		break;
	case ECommNotifyConfigChangeCancel:
		p->CommNotifyConfigChangeCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);
		break;
	case ECommNotifyBreak:
		p->CommNotifyBreak(aMessage, iSession);
		break;
	case ECommNotifyBreakCancel:
		p->CommNotifyBreakCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);
		break;
	case ECommGetRole:
		p->CommGetRole(aMessage, iSession);
		break;
	case ECommNotifyDataAvailable:
		p->CommNotifyDataAvailable(aMessage, iSession);
		break;
	case ECommNotifyDataAvailableCancel:
		p->CommNotifyDataAvailableCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);
		break;
	case ECommNotifyOutputEmpty:
		p->CommNotifyOutputEmpty(aMessage, iSession);
		break;
	case ECommNotifyOutputEmptyCancel:
		p->CommNotifyOutputEmptyCancel(aMessage.Int3(), iSession);
		SafeComplete(aMessage, KErrNone);	
		break;
	// case default: 	// no default case here as it is dealt with in CCommSession::ServiceL
		}
	// Extensions to the CCommSession ends to here
#ifdef _DEBUG
	CleanupStack::PopAndDestroy();  // TCleanupItem(CorruptSessionPointer, &iSession));
#endif
	}

void CC32Player::SafeComplete(const RMessagePtr2& aMessage, TInt aCompletionCode)
	{
	if(iComplete)
		{
		::SafeComplete(aMessage, aCompletionCode);
		}
	}

void CC32Player::DontCompleteCurrentRequest()
	{
	iComplete = EFalse;
	}

void CC32Player::LoadCommModule(const RMessage2& aMessage)
/**
 * Load a comm module
 */
	{
	TFileName csyfilename;
	Read(0,aMessage,csyfilename);

	TInt r=csyfilename.LocateReverse('.');
 	if (r==KErrNotFound)
		{
		csyfilename.Append(KCSYExtension);
		}

	C32LOG3(KC32Player, _L("CC32Player::LoadCommModule : %S. aMessage:%08x"), &csyfilename,aMessage.Handle());
	CSerial *s=NULL;
	TRAPD(res,s=iPortManager->LoadCommModuleL(csyfilename)); // if you see a crash with (iPortManager == null) here, you might have mis-configured cmi files
	if(res == KErrNone)
		{
		// no leave occurred, library loaded successfully ! Send pointers to message, CsyFilename, and portPrefix
		// back to dealer, dealer updates the information in ThreadManager and completes the message
		CC32WorkerThread& owner = WorkerThread();
		if(owner.WorkerId()==TC32WorkerThreadPublicInfo::EMainThread)
			{
			// co-resident player, make direct function call
			owner.Dealer()->ProcessLoadCommModuleSuccessResponse(aMessage, s);
			DontCompleteCurrentRequest();
			}
		else if(owner.PeerReachable(TC32WorkerThreadPublicInfo::EMainThread))
			{
			// different thread, send via transports
			TC32PlayerLoadCommModuleSuccessResp respMsg(aMessage, s);
			owner.PostMessage(TC32WorkerThreadPublicInfo::EMainThread, respMsg);
			DontCompleteCurrentRequest();
			const RNullableMessage& forwardedMsg = static_cast<const RNullableMessage&>(aMessage);
			forwardedMsg.NullHandle();
			}
		else
			{
			C32LOG1(KC32Warning, _L8("CC32Player::LoadCommModule() Peer Thread Unreachable"));
			DontCompleteCurrentRequest();
			__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));
			}
		C32LOG1(KC32Player, _L("CC32Player::LoadCommModule : Load completed successfully - message sent to Dealer"));
			
		}
	else
		{
		// library load failure ! Mesg sent back to dealer to inform of failed load attempt, dealer 
		// removes CSY from session CSY container and de-allocates memory allocated to this to-be-loaded
		// CSY, and completes message with reason of load failure
		CC32WorkerThread& owner = WorkerThread();
		if(owner.WorkerId()==TC32WorkerThreadPublicInfo::EMainThread)
			{
			// co-resident player, make direct function call
			owner.Dealer()->ProcessLoadCommModuleFailureResponse(aMessage, res);
			DontCompleteCurrentRequest();
			}
		else if(owner.PeerReachable(TC32WorkerThreadPublicInfo::EMainThread))
			{
			// different thread, send via transports
			TC32PlayerLoadCommModuleFailureResp respMsg(aMessage, res);
			owner.PostMessage(TC32WorkerThreadPublicInfo::EMainThread, respMsg);
			DontCompleteCurrentRequest();
			const RNullableMessage& forwardedMsg = static_cast<const RNullableMessage&>(aMessage);
			forwardedMsg.NullHandle();
			}
		else
			{
			C32LOG1(KC32Warning, _L8("CC32Player::LoadCommModule() Peer Thread Unreachable"));
			DontCompleteCurrentRequest();
			__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));
			}
		}
	}

void CC32Player::CloseCommModule(const RMessage2& aMessage)
/**
 * Close a comm module
 */
	{
	TFullName name;      // not a TPortName for backwards compat reasons
	Read(0,aMessage,name);
	C32LOG2(KC32Player, _L("CC32Player::CloseCommModule() called, CommModule : %S"), &name);
	CSerial* s=NULL;
	TRAPD(res,s=iPortManager->GetSerialL(name));
	if (res==KErrNone)
		{
		s->Close();		// call Close() on CSerial object
		}
	SafeComplete(aMessage, res);
	}

void CC32Player::NewPortL(const RMessage2& aMessage)	// do we need to pass CCommSession* here ?
/**
 * Ask the port manager to open a port in the CSY which is then added
 * to this session's port list (CC32SubSessionIx). If another session 
 * has already opened the same port, the port manager will still give 
 * us a reference if the port is not being used exclusively.
 *
 * @param aMessage handle to the IPC message from the client
 */
	{
	C32LOG1(KC32Player, _L8("CC32Player::NewPortL()"));
	iSession = static_cast<CCommSession*>(aMessage.Session());
	C32LOG3(KC32Player, _L8("CC32Player:NewPort: session=%08x, Message(%08x) "), iSession, aMessage.Handle());	
	CPort *p=NULL;
	CSerial *s=NULL;
	TFullName name;
	TUint port;
	TInt len;
	TInt handle;

	// Extract port name and number
	VERIFY_RESULT(ExtractPortNameAndNumber(aMessage, name, port, len), KErrNone);	 // must be valid as it is already checked in dealer
	TRAPD(res,s=iPortManager->GetSerialL(name.Left(len)));
	/**
	We can't ASSERT here as the client should have loaded the CSY before opening the port.
	This will fail only in the circumstance when the other session unloads the Comm Module and
	by the  time message is received on player, the GetSerial fails due to module not present.
	*/
	if (res!=KErrNone)
		{
		SafeComplete(aMessage, res);
		return;
		}
	if(aMessage.Function()==ECommOpenWhenAvailable)
		{
		// This default role, as specified in aMessage.Int2(), will only be used
		// if the port is being opened for the first time
		TRAP(res,p=iPortManager->GetPortL(name,port,s,(TUint)EIntCommWaitUntilAvailable,
												(TUint)aMessage.Int2(), iSession));
		}
	else
		{
		TRAP(res,p=iPortManager->GetPortL(name,port,s,aMessage.Int1(),aMessage.Int2(), iSession));
		}

	if(res!=KErrNone)
		{
		SafeComplete(aMessage, res);
		return;
		}
	else
		{
		/** no leaves occurred, either new or existing CPort opened successfully
		* Iterate through TSubSessionContainer if (p==CPortFromSubSession() && subSess->Session() == iSession)
		* call subSess->Open() so that its AccessCount is incremented else 
		* create a "new" subsession. Iterating the TSubSessionContainer won't
		* be a performance bottleneck as RComm::Open() call is made only once
		*/
		TBool existingSubSess = EFalse;			// Set ETrue when existing subsession found
		TInt idxFound = 0;			// stores the index of existing sub-session from TSubSessionContainer
		for(TInt i = iSubSessions.Count() - 1; i >= 0; --i)
			{
			if(CPortFromSubSession(iSubSessions[i]) == p && iSubSessions[i]->Session() == iSession)	// check for existing CPort* && check for session
				{
				existingSubSess=ETrue;		// existing CPort* found which is this session
				idxFound = i;				// store index
				break;
				}
			}

		if(existingSubSess == EFalse)
			{
			/** new SubSession
			* create "new" subsession
			* Add CPort* (p) to this newly created subsession
			* Add CCommSubSession to TSubSessionContainer
			* Add CCommSubSession* to CC32SubSessionIx to obtain subsession handle
			* Write the handle back to client
			* if writing handle back to client fails, remove subsession from CC32SubSessionIx
			*/
			
			C32LOG1(KC32Player, _L8("CC32Player::NewPortL() - NEW SUBSESSION"));
			CCommSubSession* css = CCommSubSession::NewL(iSession, p, this); 	// create CCommSubSession	
			res = iSubSessions.Append(css);		// add to TSubSessionContainer
			if(res != KErrNone)
				{
				css->Close();		// Close() newly created sub-session (which will delete it as accesscount = 1) this could not be appended to TSubSessionContainer
				SafeComplete(aMessage, res);	// complete message with error returned from Append
				return;
				}
			// lock SubSessionIx container in dealer and obtain "new" TSubSessionHandle - TInt
			iSession->SubSessions().Lock();
			res = CC32DealerByRef().AddSubSession(css, iSession, handle);
			if(res == KErrNone)
				{
				// write subsession handle to client
				res = WriteSubSessionHandle(handle);
				if(res != KErrNone)	// if cannot write handle to client
					{
					// we can remove the index from CC32SubSessionIx for this session as its a new subSess
					CC32DealerByRef().RemoveSubSession(handle, iSession);
					}
				}
			iSession->SubSessions().Unlock();
			
			if(res != KErrNone)		// if cannot write handle to client
				{
				p->Close();			// close CPort
				css->Close();	// close CCommSubSession, removes from TSubSessionContainer if access count drops to zero.
				}
			
			SafeComplete(aMessage, res);
			return;
			}
		else
			{
			/** existing SubSession, 
			* call Open() on subSess to increment AccessCount
			* CC32Dealer::FindSubSession() to find subsession aHandle
			* write the aHandle back to client
			* if writing handle back to client, call Close() on subsession and CPort to decrement reference counts
			*/
			C32LOG1(KC32Player, _L8("CC32Player::NewPortL() - EXISTING SUBSESSION"));		
			iSubSessions[idxFound]->Open();		// increment AccessCount

			// lock SubSessionIx container in dealer
			iSession->SubSessions().Lock();
			// find existing sub-session from this session's CC32SubSessionIx
			res = CC32DealerByRef().FindSubSession(iSubSessions[idxFound], iSession, handle); // should not fail
			if(res != KErrNone)
				{
				C32LOG1(KC32Warning, _L8("CC32Player::NewPortL() - Failed to find existing sub-session in this session "));
				}
			__ASSERT_DEBUG(res == KErrNone, Fault(EFindSubSessionFailed));
			// write existing or new subsession handle back to client
			res = WriteSubSessionHandle(handle);

			iSession->SubSessions().Unlock();			

			if(res != KErrNone)		// if cannot write handle to client
				{
				p->Close();			// close CPort
				iSubSessions[idxFound]->Close();	// close CCommSubSession
				}
			
			SafeComplete(aMessage, res);
			return;
			}
		}
	}

TInt CC32Player::ExtractPortNameAndNumber(const RMessagePtr2& aMessage, TDes& aPortName, TUint& aPortNumber, TInt& aLength) 
/**
Extract the port name and number from RMessage
*/
	{
  	Read(0,aMessage, aPortName);
	
	_LIT(KDoubleColon, "::");
	aLength = aPortName.Find(KDoubleColon);
	if (aLength == KErrNotFound)
		{
		return KErrNotFound;
		}
	// extract the numeric value after ::
	TInt numPos = aLength + KDoubleColon.iTypeLength;
	TPtrC numPtr(&aPortName[numPos], aPortName.Length() - numPos);
	TLex lexer(numPtr);
	TInt ret = lexer.Val(aPortNumber);
	
	return ret;
	}

void CC32Player::PortInfo(const RMessage2& aMessage,const TPortName& aPortName)
/**
 * Write back the port info to the client for a specified port
 *
 * @param aPortName name of the port to get information about
 */
	{
	TSerialInfo port;
	TInt ret=0;
	if((ret=iPortManager->PortInfo(aPortName,port))==KErrNone)
		{
		TPckgC<TSerialInfo> p(port);
		Write(0,aMessage,p);
		}
	SafeComplete(aMessage, ret);
	}

void CC32Player::PortInfo(const RMessage2& aMessage, CSerial* aSerialPtr)
/**
 * Write back the port info to the client for a specified port
 *
 * @param aSerial CSerial Pointer obtained by lookup in CC32ThreadManager
 */
	{
	iComplete = ETrue;
	TFileName name;
	TSerialInfo port;
	TInt ret=0;
	ASSERT(aSerialPtr);
	aSerialPtr->Info(port);
	aSerialPtr->ModuleName(name);
	TPckgC<TSerialInfo> p(port);
	ret = Write(0,aMessage,p);
	if(ret == KErrNone)	//if not KErrNone then client will already be panic'd so do not atttempt further write as this will panic the server
		{
		Write(1,aMessage,name);	
		}
	SafeComplete(aMessage, ret);
	}

TInt CC32Player::Write(TInt aPos, const RMessagePtr2& aMessage , const TDesC8& aDes, TInt aOffset)
/**
 * Write and kill the client if it leaves.
 *
 * Copies data from an 8 bit descriptor in the server address space to the client
 * thread's address space. The target location must be a valid modifiable descriptor.
 * Data is copied from the source descriptor to the specified offset position within
 * the target descriptor data area. The length of data copied is the length of the
 * source descriptor. The length of the target descriptor is set to the length of
 * the source descriptor plus the value of the offset.
 *
 * @param aPtr    A pointer to a valid address within the client thread's address space.
 *                The data type at this location must be a modifiable descriptor, i.e. aTDes8 type.
 * @param aDes    An 8 bit descriptor in the server address space. This is the source of the copy operation.
 * @param aOffset The offset from the start of the target descriptor data area where copying is to begin.
 *                This value must be greater than or equal to zero.
 *
 * @panic This function will panic the client if the WriteL() leaves
 */	
	{
	//C32LOG4(KC32Player, _L8("CC32Player::Write() Data = (%s), Pos (%d) Offset (%d)"), aDes.Ptr(), aPos, aOffset);

	TInt ret = aMessage.Write(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		C32LOG1(KC32Player, _L8("CC32Player::Write \t Error at the time of writing data to client"));
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}


TInt CC32Player::Read(TInt aPos, const RMessagePtr2& aMessage , TDes8& aDes, TInt aOffset)
/**
 * Read and kill the client if it leaves.
 * 
 * Copies data from the client thread's address space into an 8 bit descriptor
 * in the server address space. The source data must be a valid descriptor.
 * Data is copied from the specified offset position within the source descriptor
 * data area. The length of data copied is the length of source descriptor data
 * minus the offset value. If the offset value is greater than the length of the
 * source descriptor, then no data is copied. The length of data copied is limited
 * to the maximum length of the target descriptor.
 *
 * @param aPtr    A pointer to a valid address within the client thread's address space.
 *                The data at this pointer must be a descriptor, i.e. a TDesC8 type.
 * @param aDes    An 8 bit descriptor in the server address space. This is the target
 *                of the copy operation.
 * @param aOffset The offset from the start of the source descriptor data area from where
 *                copying is to begin. This value must be greater than or equal to zero.
 *
 * @panic This function will panic the client if the ReadL() leaves
 */
	{
	C32LOG3(KC32Player, _L8("CC32Player::Read(), Pos (%d), Offset (%d)"), aPos, aOffset);

	TInt ret = aMessage.Read(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		C32LOG1(KC32Player, _L8("CC32Player::Read \t Error at the time of reading data from client"));
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}


TInt CC32Player::Write(TInt aPos, const RMessagePtr2& aMessage , const TDesC16& aDes, TInt aOffset)
/**
 * Write and kill the client if it leaves.
 *
 * (see CC32Player::Write() with 8-bit descriptor)
 *
 * @param aPtr    A pointer to a valid address within the client thread's address space.
 *                The data type at this location must be a modifiable descriptor, i.e. aTDes16 type.
 * @param aDes    A 16 bit descriptor in the server address space. This is the source of the copy operation.
 * @param aOffset The offset from the start of the target descriptor data area where copying is to begin.
 *                This value must be greater than or equal to zero.
 *
 * @panic This function will panic the client if the WriteL() leaves
 */	
	{
	//C32LOG4(KC32Player, _L8("CC32Player::Write(), Data = (%s), Pos (%d), Offset (%d)"), aDes.Ptr(), aPos, aOffset);

	TInt ret = aMessage.Write(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		C32LOG1(KC32Player, _L8("CC32Player::Write \t Error at the time of writing data to client"));
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}


TInt CC32Player::Read(TInt aPos, const RMessagePtr2& aMessage , TDes16& aDes, TInt aOffset)
/**
 * Read and kill the client if it leaves.
 *
 * (see CC32Player::Write() with 8-bit descriptor)
 *
 * @param aPtr    A pointer to a valid address within the client thread's address space.
 *                The data at this pointer must be a descriptor, i.e. a TDesC16 type.
 * @param aDes    A 16 bit descriptor in the server address space. This is the target
 *                of the copy operation.
 * @param aOffset The offset from the start of the source descriptor data area from where
 *                copying is to begin. This value must be greater than or equal to zero.
 *
 * @panic This function will panic the client if the ReadL() leaves
 */	
	{
	C32LOG3(KC32Player, _L8("CC32Player::Read(), Pos (%d), Offset (%d)"), aPos, aOffset);

	TInt ret = aMessage.Read(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		C32LOG1(KC32Player, _L8("CC32Player::Read \t Error at the time of reading data from client"));
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}


// EOF - CS_ROLES.CPP



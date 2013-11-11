// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ET_MANTraces.h"
#endif

#include <e32svr.h>
#include <f32file.h>
#include "ET_SSTD.H"

const TUint KColonChar=':';
_LIT(KDoubleColon, "::");
const TInt KDoubleColonLength = 2;

//
// CPhoneManager class
//

CPhoneManager* CPhoneManager::NewL()
//
// Create new instance of phone manager
//
	{
	CPhoneManager* phoneManager=new(ELeave) CPhoneManager();
	CleanupStack::PushL(phoneManager);
	phoneManager->ConstructL();
	CleanupStack::Pop();
	return phoneManager;
	}

CPhoneManager::CPhoneManager()
	{}

void CPhoneManager::ConstructL()
	{
	iContainerIx=CObjectConIx::NewL();
	iObjectCon=iContainerIx->CreateL();
	iTsyModulesCon =iContainerIx->CreateL();
	iDuplicatePhones = new (ELeave) CArrayPtrFlat<CDuplicatePhoneInfo>(3);
	}

CPhoneManager::~CPhoneManager()
//
//	Destroy all objects in global storage
//
	{
	if (iContainerIx)
		{
		iContainerIx->Remove(iObjectCon);
		iContainerIx->Remove(iTsyModulesCon);
		delete iContainerIx;
		}
	if(iDuplicatePhones)
		{
		iDuplicatePhones->ResetAndDestroy();
		delete iDuplicatePhones;
		}
	}

void CloseObject(TAny* aObj)
//
// Utility func for cleanup stack
//
	{
	CObject* obj=REINTERPRET_CAST(CObject*,aObj);
	obj->Close();
	}

CPhoneBase* CPhoneManager::OpenPhoneFromFactoryL(CPhoneFactoryBase* aParent,const TDesC& aName)
//
// Search for the name of this TelObject if found open it if not create new instances of the object
//
	{
	CPhoneBase* newObject=NULL;
	TInt handle=0;
	TFullName dummy;
	TInt res=iObjectCon->FindByFullName(handle,aName,dummy);
	switch (res)
		{
	case KErrNotFound:
			{
			TInt len=aName.LocateReverse(KColonChar);
			if (len==KErrNotFound)
				User::Leave(KErrBadName);
			TName name=aName;
			name=aName.Right(aName.Length()-len-1);
			TName originalName(name);
			ConvertPhoneNameToOriginal(originalName);	// this converts phone name to original if it has been previously converted

			newObject=aParent->NewPhoneL(originalName);

			TCleanupItem newObjectClose(CloseObject,newObject);
			CleanupStack::PushL(newObjectClose);
			newObject->SetNameL(&name); // MUST not be a full name. It must be the name recognised by clients
					// not necessarily what the TSY calls it, if another TSY has already been loaded
					// with an identical phone name
			newObject->SetOwner(aParent);
			newObject->SetPhoneOwner(newObject);
			iObjectCon->AddL(newObject);
			newObject->Init(); // Call Tsy Init
			CleanupStack::Pop();
			}
		break;
	case KErrNone:
		newObject=REINTERPRET_CAST(CPhoneBase*,iObjectCon->At(handle));
		newObject->Open();
		break;
	default:
		User::Leave(res);
		}
	return newObject;
	}

CTelObject* CPhoneManager::OpenSubSessionObjectL(CTelObject* aParent, TDes& aNewName)
//
//	Create a new instance of this TelObject, expecting to be given a name back
//
	{
	CTelObject* newObject=NULL;
	newObject=aParent->OpenNewObjectL(aNewName);
	
	__ASSERT_ALWAYS(newObject!=NULL,Fault(EEtelFaultBadPhonePointer));
	
	TCleanupItem newObjectClose(CloseObject,newObject);
	CleanupStack::PushL(newObjectClose);
	newObject->SetOwner(aParent);
	newObject->SetNameL(&aNewName); // MUST not be a full name	
	newObject->SetPhoneOwner(aParent->PhoneOwner());
	if(iObjectCon->CheckUniqueFullName(newObject)!=KErrAlreadyExists)		// If the TSY is allocating objects from a pool, this one may already be registered.
		iObjectCon->AddL(newObject);
	newObject->Init();
	CleanupStack::Pop();
	return newObject;
	}
	
CTelObject* CPhoneManager::OpenSubSessionObjectByNameL(CTelObject* aParent, const TDesC& aName/*,TTelObjectOpenSource aSource*/)
//
// Search for the name of this TelObject if found open it if not create new instances of the object
//
	{
	CTelObject* newObject=NULL;
	TInt handle=0;
	TFullName dummy;
	TInt res=iObjectCon->FindByFullName(handle,aName,dummy);
	switch (res)
		{
	case KErrNotFound:
			{
			// All parsing of the name string done so far has been from left to right
			// Cannot simply start parsing from right to left to extract the last name element
			// There is an, albiet unlikely, error case in which the string ":::" appears, which would cause a problem
			TInt len = 0;
			TName name = aName;
			// Loop through the name string, removing each element deliminated by two colons, until only the tail of the string is left
			do
				{
				len = name.Find(KDoubleColon);

				if ( (len != KErrNotFound) && (name.Length() > KDoubleColonLength) )
					{
					name.Copy(name.Mid(len + KDoubleColonLength));
					}
				else
					{
					// Length wasn't greater than KDoubleColonLength, so the double colon was at the end of the string
					// This shouldn't ever happen.
					break;
					}
				
				} while (len != KErrNotFound);

			if (name.Length())
				newObject=aParent->OpenNewObjectByNameL(name);
			else
				newObject=aParent->OpenNewObjectL(name);	// TSY will supply a name
				
			__ASSERT_ALWAYS(newObject!=NULL,Fault(EEtelFaultBadPhonePointer));				
			
			TCleanupItem newObjectClose(CloseObject,newObject);
			CleanupStack::PushL(newObjectClose);
			newObject->SetNameL(&name); // MUST not be a full name
			newObject->SetOwner(aParent);
			newObject->SetPhoneOwner(aParent->PhoneOwner());
			iObjectCon->AddL(newObject);
			newObject->Init();
			CleanupStack::Pop();
			break;
			}
	case KErrNone:
		newObject=REINTERPRET_CAST(CTelObject*,iObjectCon->At(handle));
		newObject->Open();
		break;
	default:
		User::Leave(res);
		}
	return newObject;
	}
	
void CloseLibrary(TAny* aLib)
//
// Close a library from the cleanup stack
//
	{
	RLibrary* lib=REINTERPRET_CAST(RLibrary*,aLib);
	lib->Close();
	}

CPhoneFactoryBase* CPhoneManager::OpenPhoneFactoryFromTsyL(const TDesC& aName)
//
// Find a previously opened phone by name
//
	{
	TInt handle=0;
	TName matching;
	User::LeaveIfError(iTsyModulesCon->FindByName(handle,aName,matching));
	return REINTERPRET_CAST(CPhoneFactoryBase*,iTsyModulesCon->At(handle));
	}

//
// Typedef required for returning PhoneFactoryBase instance
//
typedef CPhoneFactoryBase*(*TPhoneFactoryBaseNewL)();

CPhoneFactoryBase* CPhoneManager::LoadPhoneModuleL(const TDesC& aFileName)
//
// Load an Etel module (TSY) on user request
//
	{
	// Strip .TSY Off
	TName name(aFileName);
	TInt len=name.Locate('.');
	name.SetLength(len);

	TName foundName;
	TInt findHandle=0;
	TInt found=iTsyModulesCon->FindByName(findHandle,name,foundName);	// Is already loaded?
	if(found==KErrNone)
		{	// The TSY is already loaded, we'll just open, and bump up the reference count.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEMANAGER_LOADPHONEMODULEL_1, "LoadPhoneModuleL\tTSY already loaded - inc ref count");
		CPhoneFactoryBase* s=REINTERPRET_CAST(CPhoneFactoryBase*,iTsyModulesCon->At(findHandle));
		s->Open();
		return s;
		}

	RLibrary lib;
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEMANAGER_LOADPHONEMODULEL_2, "Loading %S", aFileName);
	TInt r=lib.Load(aFileName);
	if (r!=KErrNone)
		User::Leave(r);
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEMANAGER_LOADPHONEMODULEL_3, "Loaded %S", aFileName);
	TCleanupItem libClose(CloseLibrary,&lib);
	CleanupStack::PushL(libClose);

	// Check the Uid2
	if(lib.Type()[1]!=TUid::Uid(KUidUnicodeEtelServerModule))
		User::Leave(KErrBadLibraryEntryPoint);

	TPhoneFactoryBaseNewL libEntry=(TPhoneFactoryBaseNewL)lib.Lookup(1);
	if (libEntry==NULL)
		User::Leave(KErrBadLibraryEntryPoint);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEMANAGER_LOADPHONEMODULEL_4, "About to get CPhoneFactoryBase ptr");
	CPhoneFactoryBase* s=NULL;
	s=(*libEntry)();	// libEntry may leave.
	if(s==NULL)
		User::Leave(KErrNoMemory);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEMANAGER_LOADPHONEMODULEL_5, "Got CPhoneFactoryBase ptr");
	TRAPD(error, s->ConstructL(lib));
	if(error)
		{
		s->Close();
		User::Leave(error);
		}
	CleanupStack::Pop();

	if (DuplicatePhoneName(s))
		{
		s->Close();
		User::Leave(KErrEtelDuplicatePhoneName);
		}

	TInt errorName = s->SetName(&name);
	if(errorName!=KErrNone)
		{
		s->Close();
		User::Leave(errorName);
		}

	TRAPD(ret,iTsyModulesCon->AddL(s));
	if(ret!=KErrNone)
		{
		s->Close();
		User::Leave(ret);
		}

	if (iTsyModulesCon->Count()>1)	// only check for phone name clashes if have >1 TSYs loaded
		{
		TInt ret = RenameDuplicatePhoneName();
		if (ret!=KErrNone)
			{
			s->Close();
			User::Leave(ret);
			}
		}
	return s;
	}

TInt CPhoneManager::EnumeratePhones() const
//
// Return the number of phones in all loaded tsy
//
	{
	TInt tsyCount=0;
	TUint numPhones=0;
	while (tsyCount<iTsyModulesCon->Count())
		{
		CPhoneFactoryBase* phoneFac=REINTERPRET_CAST(CPhoneFactoryBase*,iTsyModulesCon->operator[](tsyCount));
		numPhones+=phoneFac->EnumeratePhones();
		tsyCount++;
		}
	return numPhones;
	}

TInt CPhoneManager::GetPhoneInfo(TUint aIndex,RTelServer::TPhoneInfo& aInfo) const
//
// Get info by number on a specified phone.
//
	{
	TInt tsyCount=0;
	TUint numPhones=0;
	TUint lastCount=0;
	while (tsyCount<iTsyModulesCon->Count())
		{
		CPhoneFactoryBase* phoneFac=REINTERPRET_CAST(CPhoneFactoryBase*,iTsyModulesCon->operator[](tsyCount));
		numPhones+=phoneFac->EnumeratePhones();
		if (aIndex<numPhones) // the phone index refers to is in this tsy
			{
			return phoneFac->GetPhoneInfo(aIndex-lastCount,aInfo);
			}
		tsyCount++;
		lastCount=numPhones;
		}
	return KErrNotFound;
	}

TInt CPhoneManager::GetTsyName(const TInt aIndexOfPhone,TDes& aTsyName) const
	{
	CPhoneFactoryBase* phoneFac = PhoneFactoryObjectFromPhoneIndex(aIndexOfPhone);
	if (phoneFac!=NULL)
		{
		aTsyName = phoneFac->Name();
		return KErrNone;
		}
	else
		return KErrNotFound;
	}

CPhoneFactoryBase* CPhoneManager::PhoneFactoryObjectFromPhoneIndex(TUint aIndex) const
//
// Return a factory handle for a given phone index.
//
	{
	TInt tsyCount=0;
	TUint numPhones=0;
	while (tsyCount<iTsyModulesCon->Count())
		{
		CPhoneFactoryBase* phoneFac=REINTERPRET_CAST(CPhoneFactoryBase*,iTsyModulesCon->operator[](tsyCount));
		numPhones+=phoneFac->EnumeratePhones();
		if (aIndex<numPhones) // the phone index refers to is in this tsy
			return phoneFac;
		tsyCount++;
		}
	return NULL;
	}

CPhoneFactoryBase* CPhoneManager::PhoneFactoryObjectFromPhoneName(const TDesC& aPhoneName) const
//
// Return a handle for phone factory base with given name
// NULL if not found in any tsy module
//
	{
	RTelServer::TPhoneInfo info;
	TUint numPhones=EnumeratePhones();
	TUint count=0;
	CPhoneFactoryBase* fact=NULL;
	
	while (count<numPhones)
		{
		GetPhoneInfo(count,info);
		fact = PhoneFactoryObjectFromPhoneIndex(count);
		ConvertPhoneNameFromOriginal(fact->Name(),info.iName);	// convert original phone name to munged if possible, 
					// because aPhoneName will be in that form
		if (info.iName.Compare(aPhoneName)==KErrNone) // found a match phone name
			return fact;
		count++;
		}
	return NULL;
	}

TBool CPhoneManager::DuplicatePhoneName(CPhoneFactoryBase* aPhoneFac) const
//
// Scan throu' phone names in this TSY and check that no two phones has the same name
//
	{
	RTelServer::TPhoneInfo infoToCompare;
	RTelServer::TPhoneInfo infoToMatch;
	TUint numPhones=aPhoneFac->EnumeratePhones();
	TUint indexToCompare=0;
	if (numPhones<=1)
		return EFalse;
	do
		{
		aPhoneFac->GetPhoneInfo(indexToCompare,infoToCompare);
		for (TUint i=indexToCompare+1;i<numPhones;i++)
			{
			aPhoneFac->GetPhoneInfo(i,infoToMatch);
			if (infoToMatch.iName.Compare(infoToCompare.iName)==KErrNone) // found a match
				return ETrue;
			}
		indexToCompare++;
		}
	while (indexToCompare<numPhones);
	return EFalse;
	}

TInt CPhoneManager::RenameDuplicatePhoneName() const
//
// Scan through phone names and check for identical phone names (because a new TSY has just been
// loaded for instance). 
// Assumes that for all TSYs loaded, if they have more than one phone, that the phone 
// names within that TSY are all different. This assumption should be OK since this is checked at load time.
// Make a note of which phone has an identical name
//
	{
	RTelServer::TPhoneInfo infoToCompare;
	RTelServer::TPhoneInfo infoToMatch;
	TUint numPhones=EnumeratePhones();
	TUint indexToCompare=0;
	if (numPhones<=1)
		return EFalse;
	do
		{
		GetPhoneInfo(indexToCompare,infoToCompare);
		for (TUint i=indexToCompare+1;i<numPhones;i++)
			{
			GetPhoneInfo(i,infoToMatch);
			if (infoToMatch.iName.Compare(infoToCompare.iName)==KErrNone) // found a match
				{
				TName tsyName;
				TInt err = GetTsyName(i,tsyName);
				if(err != KErrNone)
					return err;
				_LIT(KDash,"-");
				TName newName(tsyName);
				newName.Append(KDash);
				newName.Append(infoToMatch.iName);	// Create unique name
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEMANAGER_RENAMEDUPLICATEPHONENAME_1, "new name = %S", newName);
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEMANAGER_RENAMEDUPLICATEPHONENAME_2, "tsy name = %S", tsyName);
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEMANAGER_RENAMEDUPLICATEPHONENAME_3, "old name = %S", infoToMatch.iName);
				TRAPD(ret,StoreDuplicateNameL(tsyName,infoToMatch.iName,newName));// even if this returns with 
									//KErrAlreadyExists, carry on searching for another match
				if (ret!=KErrNone && ret!=KErrAlreadyExists)
					return ret;
				}
			}
		indexToCompare++;
		}
	while (indexToCompare<numPhones);
	return KErrNone;
	}

void CPhoneManager::StoreDuplicateNameL(const TDesC& aTsyName, const TDesC& aOriginalName, const TDesC& aNewName) const
	{
	for (TInt i=0; i<iDuplicatePhones->Count(); i++)
		{
		if (iDuplicatePhones->At(i)->iNewName->CompareF(aNewName)==KErrNone)
			User::Leave(KErrAlreadyExists);	// Leave if the munged name has already been chosen.
		}
	CDuplicatePhoneInfo* phoneInfo = CDuplicatePhoneInfo::NewLC(&aTsyName,&aOriginalName,&aNewName);
	iDuplicatePhones->AppendL(phoneInfo);
	CleanupStack::Pop();
	}

TInt CPhoneManager::ConvertPhoneNameToOriginal(TDes& aName) const
	{
	CDuplicatePhoneInfo* eachInfo;
	for (TInt i=0; i<iDuplicatePhones->Count();i++)
		{
		eachInfo = iDuplicatePhones->At(i);
		if (eachInfo->iNewName->Des()==aName)
			{
			aName=eachInfo->iOriginalName->Des();
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

TInt CPhoneManager::ConvertPhoneNameFromOriginal(const TDesC& aTsyName, TDes& aPhoneName) const
	{
	CDuplicatePhoneInfo* eachInfo;
	for (TInt i=0; i<iDuplicatePhones->Count();i++)
		{
		eachInfo = iDuplicatePhones->At(i);
		if (eachInfo->iOriginalName->Des()==aPhoneName && eachInfo->iTsyName->Des()==aTsyName)
			{
			aPhoneName=eachInfo->iNewName->Des();
			return KErrNone;
			}
		}
	return KErrNotFound;
	}
				
void CPhoneManager::RemoveDuplicatePhoneInfo(const TDesC& aTsyName)
//
//	If a session Closes a Phone Module, and it was the last session to have a handle open on the
//	Module, the Duplicate Phone Information needs to be removed.
//	So search currently loaded TSYs to see if the session's removed TSY has in fact been removed
//	altogether, and if so remove the duplicate phone info.
//
	{	
	TRAPD(ret,OpenPhoneFactoryFromTsyL(aTsyName));
	if (ret==KErrNone)	// a session still has a handle on it
		return;
	CDuplicatePhoneInfo* eachInfo;
	TInt max = iDuplicatePhones->Count()-1;
	for (TInt i=max; i>=0;--i)	// otherwise we need to remove it
		{
		eachInfo = iDuplicatePhones->At(i);
		if (eachInfo->iTsyName->Des()==aTsyName)
			{
			delete eachInfo;
			iDuplicatePhones->Delete(i);
			}
		}
	}
//
// CDuplicatePhoneInfo
//	

CDuplicatePhoneInfo* CDuplicatePhoneInfo::NewLC(const TDesC* aTsyName,const TDesC* aOriginalName, const TDesC* aNewName)
	{
	CDuplicatePhoneInfo* p = new (ELeave) CDuplicatePhoneInfo();
	CleanupStack::PushL(p);
	p->ConstructL(aTsyName,aOriginalName,aNewName);
	return p;
	}

CDuplicatePhoneInfo::CDuplicatePhoneInfo()
	{}

CDuplicatePhoneInfo::~CDuplicatePhoneInfo()
	{
	delete iTsyName;
	delete iNewName;
	delete iOriginalName;
	}

void CDuplicatePhoneInfo::ConstructL(const TDesC* aTsyName,const TDesC* aOriginalName,const TDesC* aNewName)
	{
	delete(iTsyName);
	iTsyName=NULL;

	if (aTsyName!=NULL)
		{
		iTsyName=aTsyName->AllocL();
		}

	delete(iNewName);
	iNewName=NULL;

	if (aNewName!=NULL)
		{
		iNewName=aNewName->AllocL();
		}

	delete(iOriginalName);
	iOriginalName=NULL;

	if (aOriginalName!=NULL)
		iOriginalName=aOriginalName->AllocL();
	}

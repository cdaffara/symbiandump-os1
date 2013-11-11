// objectlist.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/qr3dll.h>
#include "KernLbxModel.h"
#include "Utils.h"
#include <e32uid.h>
#include <fshell/common.mmh>
#include <apgcli.h>

EXPORT_C CKernelObjectList* CKernelObjectList::NewL(RMemoryAccess* aMemAccess)
	{
	CKernelObjectList* self = new(ELeave) CKernelObjectList(aMemAccess);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CKernelObjectList::SetCurrentType(TKernelObjectType aType)
	{
	iCurrent = aType;
	TRAP_IGNORE(iModel->SetCurrentListL(iCurrent));
	}

EXPORT_C void CKernelObjectList::RefreshDataL(TInt aIndex)
	{
	iModel->RefreshDataL(aIndex);
	}

EXPORT_C MQikListBoxModel* CKernelObjectList::GetQikListBoxModel()
	{
	return NULL;
	}

EXPORT_C TInt CKernelObjectList::Count() const
	{
	return iModel->Count();
	}

EXPORT_C void CKernelObjectList::GetInfoL(TInt aIndex, RBuf& aTitle, RBuf& aInfo)
	{
	GetInfoByIndexL(aIndex, EFalse, aTitle, aInfo);
	}

EXPORT_C void CKernelObjectList::GetVerboseInfoL(TInt aIndex, RBuf& aTitle, RBuf& aInfo)
	{
	GetInfoByIndexL(aIndex, ETrue, aTitle, aInfo);
	}

EXPORT_C void CKernelObjectList::GetInfoByIdL(TInt aId, TBool aVerbose, RBuf& aTitle, RBuf& aInfo)
	{
	TInt idx = iModel->ItemIdIndex(aId);
	User::LeaveIfError(idx);

	GetInfoByIndexL(idx, aVerbose, aTitle, aInfo);
	}

EXPORT_C TInt CKernelObjectList::GetInfoByIndexL(TInt aIndex, TBool aVerbose, RBuf& aTitle, RBuf& aInfo)
	{
	CKernListBoxData* data = static_cast<CKernListBoxData*>(iModel->RetrieveDataL(aIndex));
	CleanupClosePushL(*data);
	if (aVerbose)
		{
		data->InfoForDialogL(aTitle, aInfo, EFalse);
		}
	else
		{
		data->FormatL(data->iInfo, aTitle, aInfo);
		}
	TInt res = data->ItemId();
	CleanupStack::PopAndDestroy(data);
	return res;
	}

EXPORT_C void CKernelObjectList::DumpAllInfoL(RClogger& aClogger)
	{
	TInt n = Count();
	for (TInt i = 0; i < Count(); i++)
		{
		CKernListBoxData* data = static_cast<CKernListBoxData*>(iModel->RetrieveDataL(i));
		CleanupClosePushL(*data);
		data->DumpToCloggerL(aClogger, i, n);
		CleanupStack::PopAndDestroy(data);
		}
	}

CKernelObjectList::CKernelObjectList(RMemoryAccess* aMemAccess)
	: iMemAccess(aMemAccess)
	{
	}

void CKernelObjectList::ConstructL()
	{
	CDefaultProductPlugin::LoadProductPluginL(); // A bit nasty but reduces the amount of refactoring needed!
	iModel = new(ELeave) CKernListBoxModel(*iMemAccess);
	CleanupStack::PushL(iModel);
	iModel->ConstructL();
	CleanupStack::Pop(iModel);
	}

EXPORT_C void CKernelObjectList::SetInfoChangedCallback(TInt aIndex, const TCallBack& aCallback)
	{
	iModel->SetInfoChangedCallback(aIndex, aCallback);
	}

CKernelObjectList::~CKernelObjectList()
	{
	delete iModel;
	delete gPlugin;
	}


void GetNameFromApparcL(TUint aSid, RBuf& aName);
void GetNameFromFsL(TUint aSid, RBuf& aName);

EXPORT_C TInt ExeNameForSid(TUint aSid, RBuf& aName)
	{
	TInt err = KErrNone;
	if (aName.MaxLength() < 256) 
		{
		err = aName.Create(256);
		}
	if (err) return err;

	if (aSid == 0x10205c44)
		{
		// Special case this because sysstart isn't running & isn't an app, and appears
		// enough as parent to be worth special-casing
		aName.Copy(_L("SysStart"));
		return KErrNone;
		}

	TBuf<12> pattern;
	pattern.Num(aSid, EHex);
	pattern.Insert(0, _L("*["));
	pattern.Append(_L("]*"));
	TFindProcess findproc(pattern);
	TInt found = findproc.Next(*reinterpret_cast<TFullName*>(&aName));
	if (found == KErrNone)
		{
		aName.SetLength(aName.Length()-14); // Remove the [12345678]0001
		return found;
		}

	TRAP(err, GetNameFromApparcL(aSid, aName));
	if (err == KErrNone) return KErrNone;

	// Failing all that go to the file system directly
	TRAP(err, GetNameFromFsL(aSid, aName));
	return err;
	}

void GetNameFromApparcL(TUint aSid, RBuf& aName)
	{
#ifdef FSHELL_APPARC_SUPPORT
	// Didn't find the process in the process list. So not running. Try
	// apparc as a fallback - will only work for fully-fledged applications
	TApaAppInfo* appinfo = new (ELeave) TApaAppInfo;
	CleanupStack::PushL(appinfo);
	RApaLsSession ls;
	CleanupClosePushL(ls);
	User::LeaveIfError(ls.Connect());
	User::LeaveIfError(ls.GetAppInfo(*appinfo, TUid::Uid(aSid)));
	TInt slash = appinfo->iFullName.LocateReverse('\\');
	aName.Copy(appinfo->iFullName.Mid(slash+1));
	CleanupStack::PopAndDestroy(2, appinfo); // ls, appinfo
#else
	User::Leave(KErrNotSupported);
#endif
	}

void GetNameFromFsL(TUint aSid, RBuf& aName)
	{
	// Finally, fall back to the file system. This can be slow which is why we try the other options first
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	TUidType uids(KExecutableImageUid, KNullUid, TUid::Uid(aSid));
	CDir* dir = NULL;
	User::LeaveIfError(fs.GetDir(_L("Z:\\sys\\bin\\"), uids, ESortNone, dir));
	if (dir && dir->Count())
		{
		aName.Copy((*dir)[0].iName);
		delete dir;
		}
	else
		{
		delete dir;
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(&fs);
	}

// KernLbxModel.cpp
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
#include "KernLbxModel.h"
#include "Utils.h"
#include <fshell/memoryaccess.h>
#include "QResources3.hrh"
#include <HAL.h>
#include <fshell/common.mmh>
#include <fshell/clogger.h>

#ifdef FSHELL_APPARC_SUPPORT
#include <apgcli.h>
#include <APGWGNAM.H>
#include <APMREC.H>
#endif
#ifdef FSHELL_WSERV_SUPPPORT
#include <W32STD.H>
#endif

#include <fshell/ltkutils.h>
#include "sandbox.h"

using namespace LtkUtils;

CKernListBoxData::CKernListBoxData(CKernListBoxModel* aModel)
	: /*CQikListBoxData(),*/ iModel(aModel)
	{
	}

CKernListBoxData::~CKernListBoxData()
	{
	if (iDelegate)
		{
		iDelegate->DataObjectAboutToDestruct(this);
		}
	delete iInfo;
	}


CKernListBoxModel::CKernListBoxModel(RMemoryAccess& aMemoryAccess)
	: iSort(NULL), iMemAccess(aMemoryAccess)
	{
	}

void CKernListBoxModel::ConstructL()
	{
	iUnd = new(ELeave) CUndertaker(*this);
	iUnd->Register();
#ifdef FSHELL_WSERV_SUPPORT
	iWsSession = new (ELeave) RWsSession;
	TInt err = iWsSession->Connect();
	if (err != KErrNone)
		{
		delete iWsSession;
		iWsSession = NULL;
		// Null pointer is used to indicate wserv not present
		}
#endif
	iTempBuf1.CreateL(256);
	iTempBuf2.CreateL(256);
	}

TInt CKernListBoxModel::Count() const
	{
	return iItemData.Count();
	}

TInt CKernListBoxModel::ItemIdIndex(TInt aItemId) const
	{
	const TInt count = iItemData.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iItemData[i]->ItemId() == aItemId)
			return i;
		}
	return KErrNotFound;
	}

CKernListBoxData* CKernListBoxModel::RetrieveDataL(TInt aItemIndex)
	{
	iItemData[aItemIndex]->Open();
	return iItemData[aItemIndex];
	}

CKernListBoxData* CKernListBoxModel::RetrieveDataLC(TInt aItemIndex)
	{
	CKernListBoxData* data = iItemData[aItemIndex];
	data->Open();
	CleanupClosePushL(*data);
	return data;
	}

void CKernListBoxModel::RemoveAllDataL()
	{
	const TInt count = iItemData.Count();
	for (TInt i = 0; i < count; i++)
		{
		iItemData[i]->Close();
		}
	iItemData.Reset();
	}

void CKernListBoxModel::ModelBeginUpdateLC()
	{
	}

void CKernListBoxModel::ModelEndUpdateL()
	{
	}

void CKernListBoxModel::DataUpdatedL(TInt /*aIndex*/)
	{
	}

TInt CKernListBoxModel::Compare(TInt aLeft, TInt aRight) const
	{
	const CKernListBoxData* left = iItemData[aLeft];
	const CKernListBoxData* right = iItemData[aRight];
	return (*iSort)(left, right);
	}

void CKernListBoxModel::Swap(TInt aLeft,TInt aRight) const
	{
	const_cast<CKernListBoxModel*>(this)->WtfSwap(aLeft, aRight);
	}

void CKernListBoxModel::WtfSwap(TInt aLeft,TInt aRight)
	{
	CKernListBoxData* temp = iItemData[aLeft];
	iItemData[aLeft] = iItemData[aRight];
	iItemData[aRight] = temp;
	}

void CKernListBoxModel::RemoveDataL(TInt aItemIndex)
	{
	iItemData[aItemIndex]->Close();
	iItemData.Remove(aItemIndex);
	}

void CKernListBoxModel::SetCurrentListL(TInt aCurrentList)
	{
	if (iCurrentList != aCurrentList)
		{
		RemoveAllDataL();
		}
	iCurrentList = aCurrentList;
	}

TInt CKernListBoxModel::GetCurrentList() const
	{
	return iCurrentList;
	}

void CKernListBoxModel::NewKernDataL(TInt aType, TObjectKernelInfo* aInfo)
	{
	DoNewKernDataL(aType, aInfo, NULL);
	}

void CKernListBoxModel::DoNewKernDataL(TInt aType, TObjectKernelInfo* aInfo, MKernListBoxDataDelegate* aDelegate)
	{
	CKernListBoxData* data;
	switch (aType)
		{
	case EListOpenFiles:
		data = new(ELeave) COpenFilesListBoxData(this);
		break;
	case EListThread:
		data = new(ELeave) CThreadsListBoxData(this);
		break;
	case EListFeatureReg:
		data = new(ELeave) CFeatRegListBoxData(this);
		break;
	case EListServer:
		data = new(ELeave) CServerListBoxData(this);
		break;
	case EListHal:
		data = new(ELeave) CHalListBoxData(this);
		break;
	case EListWindowGroups:
#ifdef FSHELL_WSERV_SUPPORT
		data = new(ELeave) CWindowGroupListBoxData(this);
#else
		data = NULL; // Compiler shutter upper
		User::Leave(KErrNotSupported);
#endif
		break;
	case EListMessageQueue:
		data = new(ELeave) CMsgQueListBoxData(this);
		break;
	case EListMutex:
		data = new(ELeave) CMutexListBoxData(this);
		break;
	case EListSemaphore:
		data = new(ELeave) CSemaphoreListBoxData(this);
		break;
	case EListTimer:
		data = new(ELeave) CTimerListBoxData(this);
		break;
	default:
		data = new(ELeave) CKernListBoxData(this);
		break;
		}
	
	data->SetDelegate(aDelegate);
	CleanupStack::PushL(data);
	data->iType = aType;
	data->ConstructL();

	data->FormatL(aInfo, iTempBuf1, iTempBuf2);

	iItemData.AppendL(data);
	CleanupStack::Pop(data);
	data->Open(); // open for model

	//data->Open(); // open for return
	data->iInfo = aInfo;
	//return data;
	}


// This function is used for the list box items
void CKernListBoxData::FormatL(TObjectKernelInfo* aInfo, RBuf& aTempBuf1, RBuf& aTempBuf2)
	{
	RBuf& name = aTempBuf1;
	RBuf& more = aTempBuf2;
	name.Zero();
	more.Zero();

	TInt itemId = (TInt)aInfo; // By default use the ptr unless the type can suggest anything better
	DoFormatL(aInfo, aTempBuf1, aTempBuf2, itemId);
	SetItemId(itemId);

	if (iDelegate)
		{
		iDelegate->FormatChangedL(this, name, more);
		}
	}


void CKernListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId)
	{
	switch (iType)
		{
	case EListChunk:
		{
		TChunkKernelInfo& info = *(TChunkKernelInfo*)aInfo;
		name.Copy(info.iFullName);
		PrettyName(iType, name);
		TBuf<16> size, maxSize;
		HR(size, info.iSize);
		HR(maxSize, info.iMaxSize);
		more.Format(_L("Size %S / %S"), &size, &maxSize);
		break;
		}
	case EListProcess:
		{
		TProcessKernelInfo& info = *(TProcessKernelInfo*)aInfo;
		name.Copy(info.iFullName);
		PrettyName(iType, name);
		TUint pid = info.iProcessId;
		TUint sid = info.iProcessSecurityInfo.iSecureId;
		TExitType exitType = EExitPending;
		RProcess proc;
		if (proc.Open(pid) == KErrNone)
			{
			exitType = proc.ExitType();
			}
		if (exitType == EExitPending)
			{
			more.Format(_L("Pid %i Sid 0x%x"), pid, sid);
			}
		else
			{
			TExitCategoryName cat = proc.ExitCategory();
			more.Format(_L("Pid %i %S %i Sid 0x%x"), pid, &cat, proc.ExitReason(), sid);
			}
		proc.Close();
		itemId = pid;
		break;
		}
	case EListCodeSeg:
		{
		TCodeSegKernelInfo& info = *reinterpret_cast<TCodeSegKernelInfo*>(aInfo);
		name.Copy(info.iName);

		TBuf<16> size;
		HR(size, info.iSize);
		more.Format(_L("Size %S Ref count %i"), &size, info.iAccessCount);
		break;
		}
	case EListMimeTypes:
		{
		SDataType& info = *reinterpret_cast<SDataType*>(aInfo);
		name.Copy(info.iMime);
		more.Format(_L("From: %S"), &info.iRecog);
		break;
		}
	default:
		break;
		}
	
	}

void CKernListBoxModel::DumpToCloggerL(RClogger& clogger)
	{
	TInt count = Count();

#ifdef __WINS__
	clogger.Log("NOTE: This data is from a WINS[CW] build so is not representative of a real device!");
#endif

	for (TInt i = 0; i < count; i++)
		{
		CKernListBoxData* data = static_cast<CKernListBoxData*>(RetrieveDataLC(i));
		data->DumpToCloggerL(clogger, i, count);
		CleanupStack::PopAndDestroy(data);
		}
	}

void CKernListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt count)
	{
	_LIT8(KChunkDesc,"Chunk;FullName;Size;(SizeHumanReadable);MaxSize;(MaxSizeHumanReadable);CreatorPid;BaseAddr");
	_LIT8(KChunkFmt,"Chunk;%S;%i;(%S);%i;(%S);%i;%08x");

	_LIT8(KProcessDesc,"Process;FullName;Pid;Priority;(PriorityHumanReadable);Sid;Vid;CreatorSid;CreatorName;Capabilities;(CapsHumanReadable);ExitStatus;ExitCategory;ExitReason;FixedProcess;DProcessAddr");
	_LIT8(KProcessFmt,"Process;%S;%i;%i;(%S);%x;%x;%x;%S;%x;(%S);%S;%S;%i;%S;%08x");

	_LIT8(KCodesegDesc,"CodeSeg;FileName;Size;(SizeHumanReadable);RefCount;DepCount");
	_LIT8(KCodesegFmt,"CodeSeg;%S;%i;%S;%i;%i");

	_LIT(KMimeDesc,"MimeType;Name;Recogniser;AppUid;AppName");
	_LIT(KMimeFmt, "MimeType;%S;%S;%x;%S");

	CKernListBoxData* data = this;
	TObjectKernelInfo* aInfo = data->iInfo;

	switch (iType)
		{
	case EListChunk:
		{
		if (i == 0) clogger.Log(KChunkDesc);
		if (i == 0)
			{
			// Also log free at the start, so we can tally up mem losses
			TInt freeRam = -1, totalRam = -1;
			TBuf8<16> free, total;
			HAL::Get(HAL::EMemoryRAMFree, freeRam);
			HAL::Get(HAL::EMemoryRAM, totalRam);
			HR(free, freeRam);
			HR(total, totalRam);
			_LIT8(KFree, "FREE");
			clogger.Log(KChunkFmt, &KFree, freeRam, &free, totalRam, &total, 0);
			}

		TChunkKernelInfo& info = *(TChunkKernelInfo*)aInfo;
		TBuf8<16> size, maxSize;
		HR(size, info.iSize);
		HR(maxSize, info.iMaxSize);
		clogger.Log(KChunkFmt, &info.iFullName, info.iSize, &size, info.iMaxSize, &maxSize, info.iControllingOwnerProcessId, info.iBase);

		if (i == count-1)
			{
			// Also print out free RAM at end, so we can see if mem usage changed much during
			TInt freeRam = -1, totalRam = -1;
			TBuf8<16> free, total;
			HAL::Get(HAL::EMemoryRAMFree, freeRam);
			HAL::Get(HAL::EMemoryRAM, totalRam);
			HR(free, freeRam);
			HR(total, totalRam);
			_LIT8(KFree, "FREE");
			clogger.Log(KChunkFmt, &KFree, freeRam, &free, totalRam, &total, 0);
			}
		break;
		}
	case EListProcess:
		{
		if (i == 0) clogger.Log(KProcessDesc);
		TProcessKernelInfo& info = *(TProcessKernelInfo*)aInfo;

		TBuf<16> priority16 = ToString((TProcessPriority)info.iPriority);
		TPtr8 priority = priority16.Collapse();
		HBufC* caps16 = ToStringL(info.iProcessSecurityInfo.iCaps);
		TPtr8 caps(caps16->Des().Collapse());
		TPtrC exitStatus;
		TInt exitReason = 0;
		TExitCategoryName exitCategory;
		RProcess handle;
		if (handle.Open(info.iProcessId) == KErrNone)
			{
			exitStatus.Set(ToString(handle.ExitType()));
			exitCategory = handle.ExitCategory();
			exitReason = handle.ExitReason();
			handle.Close();
			}
		TBuf8<16> exitStatus8;
		exitStatus8.Copy(exitStatus);
		TPtrC8 exitCategory8 = exitCategory.Collapse();
		RBuf creatorName;
		data->ExeNameForSid(info.iProcessCreatorSecurityInfo.iSecureId, creatorName);
		TPtrC8 creatorName8 = creatorName.Collapse();
		TBool fixed = info.iAttributes & 1; // TMemModelProcessAttributes::EFixedAddress
		_LIT8(KFixed, "FIXED");
		TPtrC8 fixedPtr = fixed ? KFixed() : KNullDesC8();
		clogger.Log(KProcessFmt, &info.iFullName, info.iProcessId, info.iPriority, &priority, info.iProcessSecurityInfo.iSecureId, info.iProcessSecurityInfo.iVendorId, info.iProcessCreatorSecurityInfo.iSecureId, &creatorName8, info.iProcessSecurityInfo.iCaps[0], &caps, &exitStatus8, &exitCategory8, exitReason, &fixedPtr, info.iAddressOfKernelObject);
		creatorName.Close();
		delete caps16;
		break;
		}
	case EListCodeSeg:
		{
		if (i == 0) clogger.Log(KCodesegDesc);
		TCodeSegKernelInfo& info = *reinterpret_cast<TCodeSegKernelInfo*>(aInfo);
		TBuf8<16> size;
		HR(size, info.iSize);
		clogger.Log(KCodesegFmt, &info.iName, info.iSize, &size, info.iAccessCount, info.iDepCount);
		break;
		}
	case EListMimeTypes:
		{
		if (i == 0) clogger.Log(KMimeDesc);
		SDataType& info = *reinterpret_cast<SDataType*>(aInfo);
		clogger.Log(KMimeFmt, &info.iMime, &info.iRecog, info.iAppUid, &info.iApp);
		break;
		}
	default:
		break;
		}
	}

void CKernListBoxData::InfoForDialogL(RBuf& aTitle, RBuf& aText, TBool aRefresh)
	{
	if (aRefresh)
		{
		// Figure out our index in the model
		TInt index = iModel->ItemIdIndex(ItemId());
		iModel->RefreshDataL(index);
		}

	aTitle.Zero();
	aText.Zero();

	if (aTitle.MaxLength() < 256)
		{
		aTitle.ReAllocL(256);
		}
	if (aText.MaxLength() < 1024)
		{
		aText.ReAllocL(1024);
		}

	RBuf& inf = aText;
	TBuf<256>* name = new(ELeave) TBuf<256>;
	CleanupStack::PushL(name);
	DoInfoForDialogL(aTitle, inf, name);
	CleanupStack::PopAndDestroy(name);
	}

void AppendProcessFlags(TDes& aBuf, TUint32 aFlags)
	{
	TInt len = aBuf.Length();

	// 9.1/9.2 doesn't have these, it's easier to redefine them to the correct value than make all the code conditional
	const TUint KThreadFlagRealtime = 0x00000040;
	const TUint KThreadFlagRealtimeTest = 0x00000080;

#define IF_MATCH(aBuf, aFlags, aFlag) if (aFlags&aFlag) { aBuf.Append(_L( #aFlag )); aBuf.Append('|'); }
	IF_MATCH(aBuf, aFlags, KThreadFlagProcessCritical)
	IF_MATCH(aBuf, aFlags, KThreadFlagProcessPermanent)
	IF_MATCH(aBuf, aFlags, KThreadFlagSystemCritical)
	IF_MATCH(aBuf, aFlags, KThreadFlagSystemPermanent)
	IF_MATCH(aBuf, aFlags, KThreadFlagOriginal)
	IF_MATCH(aBuf, aFlags, KThreadFlagLastChance)
	IF_MATCH(aBuf, aFlags, KThreadFlagRealtime)
	IF_MATCH(aBuf, aFlags, KThreadFlagRealtimeTest)
	IF_MATCH(aBuf, aFlags, KProcessFlagPriorityControl)
	IF_MATCH(aBuf, aFlags, KProcessFlagJustInTime)
	if (aBuf.Length() > len)
		{
		// Remove trailing |
		aBuf.SetLength(aBuf.Length() - 1);
		}
	}

void CKernListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* aTemp)
	{
	TDes* name = aTemp;
	switch (iType)
		{
		case EListProcess:
			{
			TProcessKernelInfo& info = *(TProcessKernelInfo*)iInfo;
			RProcess process;
			TInt err = process.Open(info.iProcessId);
			CleanupClosePushL(process);

			_LIT(KInfo, "Process info");
			aTitle.Copy(KInfo);
			inf.Copy(iInfo->iFullName);
			if (!err)
				{
				inf.Append(Klf);
				inf.Append(process.FileName());
				}
			if (info.iCommandLine.Length())
				{
				inf.Append(' ');
				//aTemp->Copy(info.iCommandLine);
				// Command line args are generally 16-bit descriptors even though they're represented in 8-bit kernel-side
				TPtrC widePtr((const TUint16*)info.iCommandLine.Ptr(), info.iCommandLine.Size()/2);
				inf.Append(widePtr);
				}

			//_LIT8(KProcessDesc, "Process\tFullName\tPid\tPriority\t(PriorityHumanReadable)\tSid\tVid\tCapabilities\t(CapsHumanReadable)\tExitStatus\tExitCategory\tExitReason");

			_LIT(KProcess, "\n\nPid: %i %S\nSid: 0x%x Vid: 0x%x");
			_LIT(KPriority, "\nPriority: %i (%S)");
			_LIT(KOtherStuff, "\nCreator Sid: 0x%x (%S)\n\nCapabilities: %S");
			_LIT(KKern, "\nKernel object address: 0x%08x");
			_LIT(KFlags, "\nFlags: 0x%x (");
			_LIT(KDied, "\n\nDied with: %S %S %i");

			HBufC* caps = ToStringL(info.iProcessSecurityInfo.iCaps);
			TUint creatorSid = info.iProcessCreatorSecurityInfo.iSecureId;
			TBool fixed = info.iAttributes & 1; // TMemModelProcessAttributes::EFixedAddress
			_LIT(KFixed, "(fixed process)");
			TPtrC fixedPtr = fixed ? KFixed() : KNullDesC();
			inf.AppendFormat(KProcess, info.iProcessId, &fixedPtr, info.iProcessSecurityInfo.iSecureId, info.iProcessSecurityInfo.iVendorId);
			if (!err)
				{
				TPtrC priority = ToString(process.Priority());
				inf.AppendFormat(KPriority, process.Priority(), &priority);
				}
			inf.AppendFormat(KKern, info.iAddressOfKernelObject);
			if (info.iFlags)
				{
				inf.AppendFormat(KFlags, info.iFlags);
				AppendProcessFlags(inf, info.iFlags);
				inf.Append(')');
				}

			RBuf parentName;
			ExeNameForSid(creatorSid, parentName);

			inf.AppendFormat(KOtherStuff, creatorSid, &parentName, caps);
			delete caps;
			parentName.Close();

			if (!err && process.ExitType() != EExitPending)
				{
				TPtrC exitStatus;
				TInt exitReason = 0;
				TExitCategoryName exitCategory;
				exitStatus.Set(ToString(process.ExitType()));
				if (process.ExitType() == EExitPanic)
					{
					exitCategory = process.ExitCategory();
					}
				exitReason = process.ExitReason();
				inf.AppendFormat(KDied, &exitStatus, &exitCategory, exitReason);
				}
			CleanupStack::PopAndDestroy(&process);
			break;
			}
		case EListChunk:
			{
			TChunkKernelInfo& info = *(TChunkKernelInfo*)iInfo;
			_LIT(KInfo, "Chunk info");
			aTitle.Copy(KInfo);
			inf.Copy(iInfo->iFullName);
			inf.Append(Klflf);

			TBuf<16> size, maxSize;
			HR(size, info.iSize);
			HR(maxSize, info.iMaxSize);

			RProcess creator;
			TInt err = creator.Open(info.iControllingOwnerProcessId);
			if (!err) 
				{
				*name = creator.FullName();
				PrettyName(EListProcess, *name);
				creator.Close();
				}
			_LIT(KKern, "Kernel object address: 0x%08x\n");
			_LIT(KChunk, "Base: 0x%08x\nSize: %S\nMax Size: %S\nCreator pid: %i (%S)");
			inf.AppendFormat(KKern, info.iAddressOfKernelObject);
			inf.AppendFormat(KChunk, info.iBase, &size, &maxSize, info.iControllingOwnerProcessId, name);
			break;
			}
		case EListCodeSeg:
			{
			TCodeSegKernelInfo& info = *reinterpret_cast<TCodeSegKernelInfo*>(iInfo);
			_LIT(KInfo, "Code segment info");
			aTitle.Copy(KInfo);
			inf.Copy(info.iName);
			inf.Append(Klflf);

			TBuf<16> size;
			HR(size, info.iSize);
			_LIT(KCodesegFmt, "File name: %S\nSize %S\nRef count %i\nDependancy count %i\nRun address 0x%08x");
			aTemp->Copy(info.iFileName);
			inf.AppendFormat(KCodesegFmt, aTemp, &size, info.iAccessCount, info.iDepCount, info.iRunAddress);
			break;
			}
		case EListMimeTypes:
			{
			SDataType& info = *reinterpret_cast<SDataType*>(iInfo);
			_LIT(KInfo, "Mime type info");
			aTitle.Copy(KInfo);

			inf.AppendFormat(_L("%S\n\n%S\nApp UID: 0x%x\n%S"), &info.iMime, &info.iRecog, info.iAppUid, &info.iApp);
			break;
			}
		default:
			break;
		}
	}

void CKernListBoxModel::SummaryInfoL(TDes& aBuf)
	{
	switch (iCurrentList)
		{
		case EListProcess:
			{
			_LIT(KFmt, "%i processes");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListThread:
			{
			_LIT(KFmt, "%i threads");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListChunk:
			{
			_LIT(KFmt, "%i chunks, free %S/%S");
			TInt freeRam = -1, totalRam = -1;
			TBuf<16> free, total;
			HAL::Get(HAL::EMemoryRAMFree, freeRam);
			HAL::Get(HAL::EMemoryRAM, totalRam);
			HR(free, freeRam);
			HR(total, totalRam);
			aBuf.Format(KFmt, Count(), &free, &total);
			break;
			}
		case EListCodeSeg:
			{
			_LIT(KFmt, "%i code segments");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListHal:
			{
			_LIT(KFmt, "%i HAL attributes");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListWindowGroups:
			{
			_LIT(KFmt, "%i window groups");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListMimeTypes:
			{
			_LIT(KFmt, "%i mime types");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListOpenFiles:
			{
			_LIT(KFmt, "%i open files");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListFeatureReg:
			{
			_LIT(KFmt, "%i features in registry");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListServer:
			{
			_LIT(KFmt, "%i servers");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListMessageQueue:
			{
			_LIT(KFmt, "%i message queues");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListMutex:
			{
			_LIT(KFmt, "%i mutexes");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListSemaphore:
			{
			_LIT(KFmt, "%i semaphores");
			aBuf.Format(KFmt, Count());
			break;
			}
		case EListTimer:
			{
			_LIT(KFmt, "%i timers");
			aBuf.Format(KFmt, Count());
			break;
			}
		default:
			break;
		}
	}

TBool CKernListBoxData::SupportsCommand(TInt aCommand)
	{
	switch (aCommand)
		{
		case ECmdPoll:
		case ECmdHandleInfo:
			{
			if (iType == EListProcess || iType == EListChunk) return ETrue;
			break;
			}
		case ECmdKill:
			{
			if (iType == EListProcess) return ETrue;
			break;
			}
		default:
			break;
		}
	return EFalse;
	}

void CKernListBoxData::KillL()
	{
	TUint8* addr = iInfo->iAddressOfKernelObject;
	RMemoryAccess& mem = iModel->MemAccess();
	User::LeaveIfError(mem.ObjectDie(iType == EListThread ? EThread : EProcess, 0, addr, EExitKill, 666999, KNullDesC));
	// The undertaker should cause an update via ThreadDiedL
	}

TAny* CKernListBoxData::GetHandleL()
	{
	return iInfo->iAddressOfKernelObject;
	}

TInt CKernListBoxData::ExeNameForSid(TUint aSid, RBuf& aName)
	{
	// First try our list
	TInt lserr = KErrNotFound;
	if (aName.Create(256) != KErrNone) lserr = KErrNoMemory;
	if (iModel && aName.MaxLength())
		{
		for (TInt i = 0; i < iModel->iItemData.Count(); i++)
			{
			CKernListBoxData* data = static_cast<CKernListBoxData*>(iModel->iItemData[i]);
			TProcessKernelInfo* pinfo = static_cast<TProcessKernelInfo*>(data->iInfo);
			if (data->iType == EListProcess && pinfo->iProcessSecurityInfo.iSecureId == aSid)
				{
				lserr = KErrNone;
				aName.Copy(pinfo->iName);
				PrettyName(EListProcess, aName);
				break;
				}
			}
		}
#ifdef FSHELL_APPARC_SUPPORT
	if (lserr == KErrNotFound)
		{
		// Didn't find the process in our process list. Must have already closed. Try
		// apparc as a fallback - will only work for fully-fledged applications
		RApaLsSession ls;
		TInt lserr = ls.Connect();
		TApaAppInfo* appinfo = new TApaAppInfo;
		if (!appinfo ) lserr = KErrNoMemory;
		if (!lserr)
			{
			lserr = ls.GetAppInfo(*appinfo , TUid::Uid(aSid));
			}
		if (!lserr)
			{
			TInt slash = appinfo->iFullName.LocateReverse('\\');
			aName.Copy(appinfo->iFullName.Mid(slash+1));
			}
		delete appinfo;
		ls.Close();
		}
#endif

	if (aName.Length() == 0 && aSid == 0x10205c44)
		{
		// Special case this because sysstart isn't running & isn't an app, and appears
		// enough as parent to be worth special-casing
		aName.Copy(_L("SysStart"));
		lserr = KErrNone;
		}
	return lserr;
	}

void CKernListBoxModel::ThreadDiedL(TInt aHandle)
	{
	RThread thread;
	thread.SetHandle(aHandle);
	TThreadId id = thread.Id();
	iThreadHandleArray.Append(aHandle);

	if (iCurrentList == EListThread)
		{
		TInt idx = ItemIdIndex(id);
		if (idx != KErrNotFound)
			{
			ModelBeginUpdateLC();
			CKernListBoxData* data = static_cast<CKernListBoxData*>(iItemData[idx]);
			data->FormatL(data->iInfo, iTempBuf1, iTempBuf2); // Thread death info isn't stored in iInfo, so no need to update it
			DataUpdatedL(idx);
			ModelEndUpdateL();

			if (iInfoChangedCallback.iFunction != NULL && data == iCallbackData)
				{
				iInfoChangedCallback.CallBack();
				}

			}
		else
			{
			// We don't have an entry for this thread id. Oh well, just storing it in the tid list will guarantee it will still be there when the user hits refresh
			}
		}
	else if (iCurrentList == EListProcess)
		{
		RProcess p;
		if (thread.Process(p) == KErrNone)
			{
			TProcessId id = p.Id();
			TInt idx = ItemIdIndex(id);
			p.Close();
			if (idx != KErrNotFound)
				{
				ModelBeginUpdateLC();
				CKernListBoxData* data = static_cast<CKernListBoxData*>(iItemData[idx]);
				data->FormatL(data->iInfo, iTempBuf1, iTempBuf2); // Process death info isn't stored in iInfo, so no need to update it
				DataUpdatedL(idx);
				ModelEndUpdateL();
				if (iInfoChangedCallback.iFunction != NULL && data == iCallbackData)
					{
					iInfoChangedCallback.CallBack();
					}

				}
			}
		}
	}

void CKernListBoxModel::CloseAllThreadHandles()
	{
	for (TInt i = 0; i < iThreadHandleArray.Count(); i++)
		{
		RThread t;
		t.SetHandle(iThreadHandleArray[i]);
		t.Close();
		}
	iThreadHandleArray.Reset();
	}

CKernListBoxModel::~CKernListBoxModel()
	{
	CloseAllThreadHandles();
	delete iUnd;
#ifdef FSHELL_WSERV_SUPPORT
	if (iWsSession)
		{
		iWsSession->Close();
		delete iWsSession;
		}
#endif
	iTempBuf1.Close();
	iTempBuf2.Close();

	for (TInt i = iItemData.Count()-1; i >= 0; i--)
		{
		iItemData[i]->Close();
		}
	iItemData.Close();
	}

CUndertaker::CUndertaker(CKernListBoxModel& aModel)
	: CActive(EPriorityStandard), iModel(aModel)
	{
	CActiveScheduler::Add(this);
	}

CUndertaker::~CUndertaker()
	{
	Cancel();
	iUnd.Close();
	}

void CUndertaker::Register()
	{
	TInt err = KErrNone;
	if (!iUnd.Handle())
		{
		err = iUnd.Create();
		}
	if (!err)
		{
		iUnd.Logon(iStatus, iHandle);
		SetActive();
		}
	}

void CUndertaker::RunL()
	{
	if (iStatus == KErrDied)
		{
		iModel.ThreadDiedL(iHandle);
		Register();
		}
	}

void CUndertaker::DoCancel()
	{
	iUnd.LogonCancel();
	}

void CKernListBoxModel::RefreshDataL(TInt aIndex)
	{
	if (iCurrentList == EListWindowGroups && !iWsSession)
		{
		User::Leave(KErrNotSupported);
		}

	// Change some names so the code is the same
	RMemoryAccess& mem(iMemAccess);
	CKernListBoxModel& model(*this);

	if (iCurrentList == EListCodeSeg)
		{
		// Code segs don't support random access
		aIndex = -1;
		}

	if (aIndex == -1)
		{
		model.RemoveAllDataL();
		}
	model.ModelBeginUpdateLC();
	
	TObjectType type = EProcess; // Just set it to something to shut up compiler
	switch (iCurrentList) {
	case EListProcess: type = EProcess; break;
	case EListThread: type = EThread; break;
	case EListChunk: type = EChunk; break;
	case EListServer: type = EServer; break;
	case EListMessageQueue: type = EMsgQueue; break;
	case EListMutex: type = EMutex; break;
	case EListSemaphore: type = ESemaphore; break;
	case EListTimer: type = ETimer; break;
	default: break;
		}

	TPtr8 buf(0,0);
	TObjectKernelInfo* info = NULL;
	CKernListBoxData* data = NULL;
	if (aIndex != -1)
		{
		data = static_cast<CKernListBoxData*>(model.RetrieveDataL(aIndex));
		CleanupClosePushL(*data);
		}

	if (iCurrentList == EListCodeSeg)
		{
		mem.AcquireCodeSegMutex();
		TCodeSegKernelInfo* inf = new(ELeave) TCodeSegKernelInfo;
		info = reinterpret_cast<TObjectKernelInfo*>(inf); // These aren't actually related classes, just convenient
		buf.Set(TPckg<TCodeSegKernelInfo>(*inf));
		while (mem.GetNextCodeSegInfo(buf) == KErrNone)
			{
			TRAPD(err, model.NewKernDataL(iCurrentList, info));
			if (err)
				{
				break;
				}
			inf = NULL;
			inf = new(ELeave) TCodeSegKernelInfo;
			buf.Set(TPckg<TCodeSegKernelInfo>(*inf));
			info = reinterpret_cast<TObjectKernelInfo*>(inf); // These aren't actually related classes, just convenient
			}
		delete inf;
		mem.ReleaseCodeSegMutex();
		}
	else if (iCurrentList == EListHal)
		{
		// HAL isn't actually a kernel container type but from the PoV of this app it is treated similarly
		//HAL::SEntry* entry = new(ELeave) HAL::SEntry;
		if (aIndex == -1)
			{
			HAL::SEntry* ents = NULL;
			TInt numEntries = 0;
			User::LeaveIfError(HAL::GetAll(numEntries, ents));
			CleanupDeletePushL(ents);
			for (TInt i = 0; i < numEntries; i++)
				{
				SHalInfo* entry = new(ELeave) SHalInfo;
				entry->iProperties = ents[i].iProperties;
				entry->iValue = ents[i].iValue;
				entry->iAttribute = i;
				CleanupDeletePushL(entry);
				model.NewKernDataL(iCurrentList, reinterpret_cast<TObjectKernelInfo*>(entry));
				CleanupStack::Pop(entry);
				}
			CleanupStack::PopAndDestroy(ents);
			}
		else
			{
			SHalInfo* entry = reinterpret_cast<SHalInfo*>(data->iInfo);
			/*TInt err =*/ HAL::Get((HAL::TAttribute)entry->iAttribute, entry->iValue); // Nothing needs to be done if this fails
			}
		}
	else if (iCurrentList == EListWindowGroups)
		{
#if defined(FSHELL_WSERV_SUPPORT) && defined(FSHELL_APPARC_SUPPORT)
		if (aIndex == -1)
			{
			CArrayFixFlat<TInt>* wgIds = new(ELeave) CArrayFixFlat<TInt>(16);
			CleanupStack::PushL(wgIds);
			User::LeaveIfError(iWsSession->WindowGroupList(0, wgIds));
			TInt numEntries = wgIds->Count();
			for (TInt i = 0; i < numEntries; i++)
				{
				TInt handle = (*wgIds)[i];
				CApaWindowGroupName* name = CApaWindowGroupName::NewLC(*iWsSession, handle);
				SWgInfo* info = new(ELeave) SWgInfo;
				CleanupStack::PushL(info);
				info->iHandle = handle;
				info->iName = name;
				info->iSession = iWsSession;
				model.NewKernDataL(iCurrentList, reinterpret_cast<TObjectKernelInfo*>(info));
				CleanupStack::Pop(2, name); // info, name
				}
			CleanupStack::PopAndDestroy(wgIds);
			}
		else
			{
			SWgInfo* info = reinterpret_cast<SWgInfo*>(data->iInfo);
			info->iName->ConstructFromWgIdL(info->iHandle);
			}
#endif
		}
	else if (iCurrentList == EListMimeTypes)
		{
		if (aIndex == -1)
			{
#ifdef FSHELL_APPARC_SUPPORT
			RApaLsSession apparc;
			User::LeaveIfError(apparc.Connect());
			CleanupClosePushL(apparc);
			RImplInfoPtrArray array;
			CleanupResetAndDestroyPushL(array);
			REComSession::ListImplementationsL(TUid::Uid(0x101F7D87), array); // Recognisers ECOM interface UID
			TFileName* dllNamePtr = new(ELeave)TFileName;
			CleanupStack::PushL(dllNamePtr);
			TFileName& dllName = *dllNamePtr;
			RArray<TDataType> mimeTypes;
			CleanupClosePushL(mimeTypes);
			gPlugin->DisablePanicCheckPushL();
			for (TInt i = 0; i < array.Count(); i++)
				{
				dllName = _L("UnknownDll");
				
				TRAP_IGNORE(Sandbox::GetDllNameFromEcomUidL(mem, array[i]->ImplementationUid(), dllName));
				mimeTypes.Reset();
				Sandbox::GetRecogniserInfoL(array[i]->ImplementationUid(), mimeTypes);

				TInt count = mimeTypes.Count();
				for (TInt j = 0; j < count; j++)
					{
					SDataType* info = new(ELeave) SDataType;
					CleanupStack::PushL(info);
					info->iMime.Copy(mimeTypes[j].Des8());
					info->iRecog = dllName;
					
					TInt err = apparc.AppForDataType(mimeTypes[j], info->iAppUid);
					if (!err)
						{
						TApaAppInfo appInfo;
						err = apparc.GetAppInfo(appInfo, info->iAppUid);
						if (!err)
							{
							info->iApp = appInfo.iFullName;
							}
						}
					model.NewKernDataL(iCurrentList, reinterpret_cast<TObjectKernelInfo*>(info));
					CleanupStack::Pop(info);
					}
				}
			CleanupStack::PopAndDestroy(); // reenables panic checks
			CleanupStack::PopAndDestroy(4, &apparc); // mimeTypes, dllNamePtr, array, apparc
#else
			User::Leave(KErrNotSupported);
#endif
			}
		else
			{
			// No refreshing necessary for mime types
			}
		}
	else if (iCurrentList == EListOpenFiles)
		{
		if (aIndex == -1)
			{
			RPtrHashMap<TDesC, SOpenFile> fileMap;
			CleanupClosePushL(fileMap);
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			TOpenFileScan scan(fs);
			CFileList* fileList = NULL;
			FOREVER
				{
				scan.NextL(fileList);
				if (fileList == NULL)
					{
					break;
					}
				CleanupStack::PushL(fileList);
				const TInt numOpenFiles = fileList->Count();
				for (TInt i = 0; i < numOpenFiles; i++)
					{
					const TEntry& entry = (*fileList)[i];
					// See if we already have an SOpenFile for this filename
					SOpenFile* info = fileMap.Find(entry.iName);
					if (info)
						{
						if (info->iNumThreads < info->iThreadIds.Count())
							{
							info->iThreadIds[info->iNumThreads] = scan.ThreadId();
							}
						info->iNumThreads++; // Increment iNumThreads regardless - so we can see how many threads there actually are, rather than how many we can store
						CKernListBoxData* data = static_cast<CKernListBoxData*>(model.RetrieveDataLC(model.ItemIdIndex((TInt)info)));
						data->FormatL(reinterpret_cast<TObjectKernelInfo*>(info), iTempBuf1, iTempBuf2); // now we've updated the number of threads, that will have changed the Format view
						CleanupStack::PopAndDestroy(data);
						}
					else
						{
						SOpenFile* info = new(ELeave) SOpenFile;
						CleanupStack::PushL(info);
						info->iName = entry.iName;
						info->iNumThreads = 1;
						info->iThreadIds[0] = scan.ThreadId();
						model.NewKernDataL(iCurrentList, reinterpret_cast<TObjectKernelInfo*>(info));
						CleanupStack::Pop(info);
						fileMap.InsertL(&info->iName, info);
						}
					}
				CleanupStack::PopAndDestroy(fileList);
				fileList = NULL;
				}
			CleanupStack::PopAndDestroy(2, &fileMap); // fileMap, fs
			}
		else
			{
			// No refreshing possible for open files
			}
		}
	else if (iCurrentList == EListFeatureReg)
		{
		// Can't link directly against featreg.dll because it doesn't exist on generic 9.1
		RLibrary featLib;
		TInt err = featLib.Load(_L("featreg.dll"));
		if (err) return; // No feature registry, no features!

		#ifdef __WINS__
			#define KQueryOrdinal 9
		#else
			#define KQueryOrdinal 4
		#endif

		typedef TInt (*QueryFn)(TUid, TUint32&);
		QueryFn Query = (QueryFn)featLib.Lookup(KQueryOrdinal);

		if (aIndex == -1)
			{
			RArray<TUid> features;
			CleanupClosePushL(features);
			gPlugin->GetFeatureUidsL(features);

			for (TInt i = 0; i < features.Count(); i++)
				{
				SFeature* info = new(ELeave) SFeature;
				CleanupStack::PushL(info);
				info->iFeature = features[i].iUid;
				info->iErr = Query(features[i], info->iInfo);
				model.NewKernDataL(iCurrentList, reinterpret_cast<TObjectKernelInfo*>(info));
				CleanupStack::Pop(info);
				}
			CleanupStack::PopAndDestroy(&features);
			}
		else
			{
			SFeature* info = reinterpret_cast<SFeature*>(data->iInfo);
			info->iErr = Query(TUid::Uid(info->iFeature), info->iInfo);
			}
		featLib.Close();
		}
	else
		{
		// Generic code that works for all first-class Kernel objects (ie processes, threads, chunks. Despite appearances, code segs are not first-class kernel objects).

		TInt sizeofObjectBuf = 0;
		switch(iCurrentList)
			{
			case EListProcess:
				sizeofObjectBuf = sizeof(TProcessKernelInfo); break;
			case EListThread:
				sizeofObjectBuf = sizeof(TThreadKernelInfo); break;
			case EListChunk:
				sizeofObjectBuf = sizeof(TChunkKernelInfo); break;
			case EListServer:
				sizeofObjectBuf = sizeof(TServerKernelInfo); break;
			case EListMessageQueue:
				sizeofObjectBuf = sizeof(TMsgQueueKernelInfo); break;
			case EListMutex:
				sizeofObjectBuf = sizeof(TMutexKernelInfo); break;
			case EListSemaphore:
				sizeofObjectBuf = sizeof(TSemaphoreKernelInfo); break;
			case EListTimer:
				sizeofObjectBuf = sizeof(TTimerKernelInfo); break;
			default:
				User::Panic(_L("QR3 Missing type"), iCurrentList);
			}

		if (aIndex == -1)
			{
			// Be paranoid about releasing the mutexes, to avoid the risk of deadlocking. Only likely if we were looking through the chunk container AND needed to grow our chunk in NewKernDataL, but err on the side of caution
			//mem.AcquireContainerMutex(type);
			TUint count;
			mem.GetContainerCount(type, count);
			//mem.ReleaseContainerMutex(type);
			for (TUint i = 0; i < count; i++)
				{
				info = (TObjectKernelInfo*)User::AllocZL(sizeofObjectBuf);
				buf.Set((TUint8*)info, sizeofObjectBuf, sizeofObjectBuf);

				CleanupStack::PushL(info);
				// Don't need to grab the lock any more
				//mem.AcquireContainerMutex(type);
				TInt err = mem.GetObjectInfo(type, i, buf);
				//mem.ReleaseContainerMutex(type);
				if (err == KErrNone)
					{
					model.NewKernDataL(iCurrentList, info);
					CleanupStack::Pop(info);
					}
				else
					{
					CleanupStack::PopAndDestroy(info);
					}
				}
			}
		else
			{
			buf.Set((TUint8*)data->iInfo, sizeofObjectBuf, sizeofObjectBuf);
			mem.GetObjectInfo(type, data->iInfo->iAddressOfKernelObject, buf); // Nothing required on error
			}
		}
	
	if (aIndex != -1)
		{
		data->FormatL(data->iInfo, iTempBuf1, iTempBuf2);
		model.DataUpdatedL(aIndex);
		CleanupStack::PopAndDestroy(data);
		}
	model.Sort(); // We can't do this in NewKernDataL because the things we sort on aren't setup until FormatL
	model.ModelEndUpdateL();

	if (iInfoChangedCallback.iFunction != NULL && (aIndex == -1 || data == iCallbackData))
		{
		iInfoChangedCallback.CallBack();
		}
	}

void CKernListBoxModel::Sort(TLinearOrder<CKernListBoxData> aOrder)
	{
	iSort = aOrder;
	Sort();
	}

void CKernListBoxModel::Sort()
	{
	if (iSort)
		{
		User::QuickSort(Count(), *this, *this);
		}
	}

void CKernListBoxModel::SetInfoChangedCallback(TInt aIndex, const TCallBack& aCallback)
	{
	iInfoChangedCallback = aCallback;
	if (aCallback.iFunction)
		{
		iCallbackData = iItemData[aIndex];
		iCallbackData->Open();
		}
	else
		{
		iCallbackData->Close();
		iCallbackData = NULL;
		}
	}

void CKernListBoxData::Open()
	{
	iRefCount++;
	}

void CKernListBoxData::Close()
	{
	iRefCount--;
	if (iRefCount == 0)
		delete this;
	}

void CKernListBoxData::SetDelegate(MKernListBoxDataDelegate* aDelegate)
	{
	iDelegate = aDelegate;
	}

MKernListBoxDataDelegate* CKernListBoxData::Delegate()
	{
	return iDelegate;
	}

const MKernListBoxDataDelegate* CKernListBoxData::Delegate() const
	{
	return iDelegate;
	}

TInt CKernListBoxData::ItemId() const
	{
	return iItemId;
	}

void CKernListBoxData::SetItemId(TInt aItemId)
	{
	iItemId = aItemId;
	}

void CKernListBoxData::ConstructL()
	{
	}

CKernListBoxModel& CKernListBoxData::Model()
	{
	return *iModel;
	}

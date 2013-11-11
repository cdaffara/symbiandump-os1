// wglistboxdata.cpp
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
#include <fshell/clogger.h>
#include <fshell/memoryaccess.h>
#include <fshell/common.mmh>


#ifdef FSHELL_APPARC_SUPPORT
#include <apgcli.h>
#include <APGWGNAM.H>
#include <APMREC.H>
#endif
#ifndef FSHELL_WSERV_SUPPPORT
#include <W32STD.H>
#endif

CWindowGroupListBoxData::CWindowGroupListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CWindowGroupListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& /*itemId*/)
	{
	SWgInfo& info = *reinterpret_cast<SWgInfo*>(aInfo);
	CApaWindowGroupName* wgName = info.iName;
	name.Copy(wgName->Caption());
	_LIT(KUnnamed, "<Untitled window group>");
	if (name.Length() == 0) name.Copy(KUnnamed);

	TThreadId tid;
	TInt res = info.iSession->GetWindowGroupClientThreadId(info.iHandle, tid);
	if (res == KErrNone)
		{
		res = tid;
		}
	more.Format(_L("Busy=%i System=%i Tid=%i"), wgName->IsBusy(), wgName->IsSystem(), res);
	}

void CWindowGroupListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	_LIT(KWgDesc,"WindowGroup;RawName;OwnerThreadId;System;Busy;Hidden");
	_LIT(KWgFmt,"WindowGroup;%S;%i;%i;%i;%i");

	if (i == 0) clogger.Log(KWgDesc);
	SWgInfo& info = *reinterpret_cast<SWgInfo*>(iInfo);
	CApaWindowGroupName* wg = info.iName;
	TPtrC name = wg->WindowGroupName();
	TBuf<256> temp = name;
	PrettyName(EListWindowGroups, temp);
	RBuf val;
	val.CreateL(256);
	TThreadId tid;
	TInt res = info.iSession->GetWindowGroupClientThreadId(info.iHandle, tid);
	if (res == KErrNone)
		{
		res = tid;
		}

	clogger.Log(KWgFmt, &temp, res, wg->IsSystem(), wg->IsBusy(), wg->Hidden());
	val.Close();
	}

void CWindowGroupListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* name)
	{
	SWgInfo& info = *reinterpret_cast<SWgInfo*>(iInfo);
	_LIT(KInfo, "Window group info");
	aTitle.Copy(KInfo);
	CApaWindowGroupName* wg = info.iName;
	*name = wg->WindowGroupName();
	PrettyName(EListWindowGroups, *name);

	inf.Append(*name);
	TThreadId tid;
	TInt res = info.iSession->GetWindowGroupClientThreadId(info.iHandle, tid);
	inf.AppendFormat(_L("\n\nOwner thread: %i"), res == KErrNone ? (TInt)tid : res);
	RThread thread;
	if (res == KErrNone)
		{
		res = thread.Open(tid);
		}
	if (res == KErrNone)
		{
		*name = thread.FullName();
		PrettyName(EListThread, *name);
		inf.AppendFormat(_L(" (%S)"), name);
		}
	inf.AppendFormat(_L("\nBusy=%i System=%i Hidden=%i"), wg->IsBusy(), wg->IsSystem(), wg->Hidden());
	}

CWindowGroupListBoxData::~CWindowGroupListBoxData()
	{
	if (iInfo)
		{
		delete reinterpret_cast<SWgInfo*>(iInfo)->iName;
		}
	}

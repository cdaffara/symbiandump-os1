// openfilesListboxdata.cpp
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

COpenFilesListBoxData::COpenFilesListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void COpenFilesListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& /*itemId*/)
	{
	SOpenFile& info = *reinterpret_cast<SOpenFile*>(aInfo);

	if (info.iNumThreads)
		{
		if (info.iNumThreads > 1)
			{
			more.Format(_L("(%i) "), info.iNumThreads);
			}
		
		RThread t;
		TInt err = Model().MemAccess().RThreadForceOpen(t, info.iThreadIds[0]);
		if (err)
			{
			more.AppendFormat(_L("Thread Id: %i err: %i"), TUint(info.iThreadIds[0]), err);
			}
		else
			{
			name.Copy(t.FullName()); // Reuse 'name' here, saves allocating another buffer
			t.Close();
			PrettyName(EListThread, name);
			more.Append(name);
			}
		}
	name.Copy(info.iName);
	}

void COpenFilesListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	_LIT(KFileDesc,"OpenFile;Name;ThreadId;ThreadName");
	_LIT(KFileFmt, "OpenFile;%S;%i;%S");

	if (i == 0) clogger.Log(KFileDesc);
	SOpenFile& info = *reinterpret_cast<SOpenFile*>(iInfo);
	RBuf val;
	val.CreateL(256);
	for (TInt j = 0; j < Min(info.iNumThreads, info.iThreadIds.Count()); j++)
		{
		val.Zero();
		RThread t;
		TInt err = t.Open(info.iThreadIds[j]);
		if (err)
			{
			val.Format(_L("Err: %i"), err);
			}
		else
			{
			val.Copy(t.FullName());
			}
		t.Close();
		clogger.Log(KFileFmt, &info.iName, TUint(info.iThreadIds[j]), &val);
		}
	val.Close();
	}

void COpenFilesListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* name)
	{
	inf.ReAllocL(1024); // Maybe need a bit more room for file list
	SOpenFile& info = *reinterpret_cast<SOpenFile*>(iInfo);
	_LIT(KInfo, "%i thread(s) using this file");
	aTitle.Format(KInfo, info.iNumThreads);

	inf.Append(info.iName);
	inf.Append('\n');
	inf.Append('\n');
	for (TInt i = 0; i < Min(info.iNumThreads, info.iThreadIds.Count()); i++)
		{
		RThread t;
		TInt err = t.Open(info.iThreadIds[i]);
		if (err)
			{
			name->Format(_L("Couldn't open thread: %i"), err);
			}
		else
			{
			name->Copy(t.FullName());
			}
		t.Close();
		inf.AppendFormat(_L("Thread ID: %i\n%S\n\n"), TUint(info.iThreadIds[i]), name);
		}
	TInt numTruncated = info.iNumThreads - info.iThreadIds.Count();
	if (numTruncated > 0)
		{
		inf.AppendFormat(_L("And %i other thread(s)...\n\n"), numTruncated);
		}
	}

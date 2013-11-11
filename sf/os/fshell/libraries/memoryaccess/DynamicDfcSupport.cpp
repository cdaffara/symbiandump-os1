// DynamicDfcSupport.cpp
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
#include "DynamicDfcSupport.h"

#ifdef FSHELL_DYNAMICDFC_SUPPORT

TInt TDynDfcQueWrapper::Create(TDynDfcQueWrapper*& aDfcQ, TInt aPriority, const TDesC& aBaseName)
	{
	TDynDfcQueWrapper* wrapper = new TDynDfcQueWrapper;
	if (!wrapper) return KErrNoMemory;
	TInt err = Kern::DynamicDfcQCreate(wrapper->iQueue, aPriority, aBaseName);
	if (err)
		{
		delete wrapper;
		}
	else
		{
		aDfcQ = wrapper;
		}
	return err;
	}

void TDynDfcQueWrapper::Destroy()
	{
	delete this;
	}

TDynDfcQueWrapper::TDynDfcQueWrapper()
	: iQueue(NULL)
	{}

TDynDfcQueWrapper::~TDynDfcQueWrapper()
	{
	iQueue->Destroy();
	}

#else

TDynDfcQueWrapper::TDynDfcQueWrapper()
	: iQueue(NULL), iKillDfc(ExitDfcThread, this, 7)
	{}

TInt TDynDfcQueWrapper::Create(TDynDfcQueWrapper*& aDfcQ, TInt aPriority, const TDesC& aBaseName)
	{
	TDynDfcQueWrapper* wrapper = new TDynDfcQueWrapper;
	TInt nonce = 0;
	TInt ret;
	do
		{
		TBuf<64> buf;
		buf.Copy(aBaseName);
		buf.Append('-');
		buf.AppendNum(nonce);
		nonce++;
		ret = Kern::DfcQCreate(wrapper->iQueue, aPriority, &buf);
		} while (ret == KErrAlreadyExists);

	if (ret == KErrNone)
		{
		wrapper->iKillDfc.SetDfcQ(wrapper->iQueue);
		aDfcQ = wrapper;
		}

	return ret;
	}

void TDynDfcQueWrapper::Destroy()
	{
	iKillDfc.Enque(); 
	}

TDynDfcQueWrapper::~TDynDfcQueWrapper()
	{
	delete iQueue;
	}

void TDynDfcQueWrapper::ExitDfcThread(TAny* aSelf)
	{
	delete (TDynDfcQueWrapper*)aSelf;
	Kern::Exit(KErrNone);
	}

#endif

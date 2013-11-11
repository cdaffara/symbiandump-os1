// DynamicDfcSupport.h
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
#ifndef DYNAMICDFCSUPPORT_H
#define DYNAMICDFCSUPPORT_H

#include <kernel.h>
#include <fshell/common.mmh>

#ifdef FSHELL_DYNAMICDFC_SUPPORT

class TDynDfcQueWrapper
	{
public:
	static TInt Create(TDynDfcQueWrapper*& aDfcQ, TInt aPriority, const TDesC& aBaseName);
	void Destroy();
	TDfcQue* Que() const { return iQueue; }

private:
	TDynDfcQueWrapper();
	~TDynDfcQueWrapper();

private:
	TDynamicDfcQue* iQueue;
	};

#else

class TDynDfcQueWrapper
	{
public:
	static TInt Create(TDynDfcQueWrapper*& aDfcQ, TInt aPriority, const TDesC& aBaseName);
	void Destroy();
	TDfcQue* Que() const { return iQueue; }

private:
	TDynDfcQueWrapper();
	~TDynDfcQueWrapper();
	static void ExitDfcThread(TAny* aPtr);

private:
	TDfcQue* iQueue;
	TDfc iKillDfc;
	};

#endif // FSHELL_DYNAMICDFC_SUPPORT

#endif // DYNAMICDFCSUPPORT_H

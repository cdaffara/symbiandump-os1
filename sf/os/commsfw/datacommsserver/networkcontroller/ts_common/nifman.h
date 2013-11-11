// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__NIFMAN_H__)
#define __NIFMAN_H__

#ifndef __ES_PROT_H__
#include <es_prot.h>
#endif

#ifndef __NIFVAR_H__
#include <nifvar.h>
#endif

#ifndef __CDBOVER_H__
#include <cdbover.h>
#endif

// Manager classes
class CNifMan;
class CNifFactory : public CObject
	{
friend class CNifMan;
public:
	IMPORT_C CNifFactory();
	IMPORT_C virtual TInt Open();
	IMPORT_C virtual void Close();
	IMPORT_C static void Cleanup(TAny* aObject);
    IMPORT_C static TInt ControlledDelete(TAny* aFactory);
	IMPORT_C void InitL(RLibrary& aLib, CObjectCon& aCon);

protected:
	IMPORT_C ~CNifFactory();
	virtual void InstallL()=0;
	RLibrary iLib;
	CAsyncCallBack* iAsyncDtor;
	};

class CNifAgentBase;
class Nif
	{
public:
	static CNifAgentBase* CreateAgentL(const TDesC& aAgentName, const TBool aNewInstance = EFalse);
	};

enum TAgentConnectType 
	{
	EAgentStartDialOut,
	EAgentReconnect,
	EAgentStartCallBack,
	EAgentNone,
	EAgentStartDialIn
	};

#endif // __NIFMAN_H__


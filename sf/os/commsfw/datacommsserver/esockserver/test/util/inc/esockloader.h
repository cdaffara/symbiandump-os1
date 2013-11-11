// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __ESOCKLOADER_H__)
#define __ESOCKLOADER_H__
#include <cfshared.h>
#include <rsshared.h>

class MHarnessLogAdaptor
	{
public:
	virtual void Log( TRefByValue<const TDesC16> format, ... ) = 0;
	virtual void Log( TRefByValue<const TDesC8> format, ... ) = 0;
	};

using namespace CommsFW;

class RRootServ;

class ESockLoader
	{
public:
	enum {KModuleLeaked = - 7315};
public:
	IMPORT_C static TInt LoadESock(MHarnessLogAdaptor* aHarnessLogger);
	IMPORT_C static TInt UnloadESock(MHarnessLogAdaptor* aHarnessLogger);
	IMPORT_C static TInt UnloadESock(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger);
private:
	class TESockSvrUnloadInfo
		{
	public:
		TCFModuleName iName;
		RootServer::TRSModuleState iState;
		TRequestStatus iStatus;
		};
	typedef RPointerArray<TESockSvrUnloadInfo> RUnloadInfoArray;

	static void BuildESockCPMListL(RRootServ& aRootServer, RUnloadInfoArray& aInfo, TDes8& aDispList);

#ifdef __FLOG_ACTIVE
	static TBool TestC32ExeStarted(RFileLogger& __logger__);
	static TBool KillC32Exe(RFileLogger& __logger__);
	static void DoUnloadESockL(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger, RFileLogger& __logger__);
#else
	static TBool TestC32ExeStarted();
	static TBool KillC32Exe();
	static void DoUnloadESockL(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger);
#endif
	};

#endif //__ESOCKLOADER_H__


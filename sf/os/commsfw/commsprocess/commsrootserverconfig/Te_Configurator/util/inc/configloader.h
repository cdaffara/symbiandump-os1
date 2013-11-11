// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __CONFIGLOADER_H__)
#define __CONFIGLOADER_H__
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

class ConfigLoader
	{
public:
	IMPORT_C static TInt LoadConfig(MHarnessLogAdaptor* aHarnessLogger);
	IMPORT_C static TInt UnLoadConfig(MHarnessLogAdaptor* aHarnessLogger);
	IMPORT_C static TInt UnLoadConfig(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger);
private:
	class TConfigSvrUnloadInfo
		{
	public:
		TCFModuleName iName;
		RootServer::TRSModuleState iState;
		TRequestStatus iStatus;
		};
	typedef RPointerArray<TConfigSvrUnloadInfo> RUnloadInfoArray;
	
	static void BuildConfigCPMListL(RRootServ& aRootServer, RUnloadInfoArray& aInfo, TDes8& aDispList);

#ifdef __FLOG_ACTIVE
	static TBool TestC32ExeStarted(RFileLogger& __logger__);
	static TBool KillC32Exe(RFileLogger& __logger__);
	static void DoUnLoadConfigL(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger, RFileLogger& __logger__);
#else
	static TBool TestC32ExeStarted();
	static TBool KillC32Exe();
	static void DoUnLoadConfigL(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger);
#endif
	};

#endif //__CONFIGLOADER_H__


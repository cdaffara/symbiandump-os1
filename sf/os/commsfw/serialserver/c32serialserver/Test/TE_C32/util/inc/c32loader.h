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

#if (!defined C32LOADER_H)
#define C32LOADER_H
#include <cfshared.h>

class MHarnessLogAdaptor
	{
public:
	virtual void Log( TRefByValue<const TDesC16> format, ... ) = 0;
	virtual void Log( TRefByValue<const TDesC8> format, ... ) = 0;
	};


class RRootServ;

class C32Loader
	{
public:
	enum {KModuleLeaked = - 7315};
	
	IMPORT_C static TInt LoadC32(MHarnessLogAdaptor* aHarnessLogger);
	IMPORT_C static TInt UnloadC32(MHarnessLogAdaptor* aHarnessLogger);
	IMPORT_C static TInt UnloadC32(CommsFW::TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger);
private:
	class TC32SvrUnloadInfo
		{
	public:
		CommsFW::TCFModuleName iName;
		TRequestStatus iStatus;
		};
	typedef RPointerArray<TC32SvrUnloadInfo> RUnloadInfoArray;

	static void BuildC32CPMListL(RRootServ& aRootServer, RUnloadInfoArray& aInfo, TDes8& aDispList);

#ifdef __FLOG_ACTIVE
	static void DoUnloadC32L(CommsFW::TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger, RFileLogger& __logger__);
#else
	static void DoUnloadC32L(CommsFW::TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger);
#endif
	};

#endif //C32LOADER_H


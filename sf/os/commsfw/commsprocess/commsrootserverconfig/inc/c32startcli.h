// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__C32STARTCLI_H__)
#define __C32STARTCLI_H__

/**
@file 

@internalTechnology

*/

#include <e32std.h>
#include <rsshared.h>
#include <cfshared.h>
#include "c32startshared.h"

using namespace RootServer; // Allowing the usage of definitions from the RS
using namespace CommsFW; // Allowing the usage of definitions from the Comms Framework

class RRsConfigurator : public RSessionBase
	/** The client interface to the Configurator server. Allows e.g. the Comms Provider Modules 
	to Load the Comms Process with desired modules and bindings on demand.
	@internalTechnology 
	*/
	{
	public:
	IMPORT_C RRsConfigurator();
	IMPORT_C TInt Connect();
	IMPORT_C void LoadCpm(const CommsFW::TCFModuleName& aName,TRequestStatus& aStatus);
	IMPORT_C void UnloadCpm(const CommsFW::TCFModuleName& aName,RootServer::TRSUnLoadType aType,TRequestStatus& aStatus);
	IMPORT_C TInt EnumerateModules(const TCFGroupName& aGroupName, RootServer::TRSIter &aPosition, CommsFW::TCFModuleName& aModuleName);
	IMPORT_C TInt GetModuleIniData(const CommsFW::TCFModuleName& aModuleName,TDes8& aIniData, TInt& aRequiredIniDataSize);
	IMPORT_C void CancelLoadCpm(const CommsFW::TCFModuleName& aName);
	IMPORT_C void CancelUnloadCpm(const CommsFW::TCFModuleName& aName);
	};


#endif


// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMSUSCLI_H__
#define __SSMSUSCLI_H__

#include <e32std.h>
#include <ssm/ssmsupinfo.h>


/**
 Client interface to the Utility Server. Currently, the only supported client
 process is sysstatemgr.exe. It is expected that Utility Plugins are loaded from
 command resource files using the struct SSM_START_SSM_UTILITY_PLUGIN.

 @internalTechnology
 @released
 */
NONSHARABLE_CLASS(RSsmSusCli) : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C TInt Connect(TInt aAsyncMessageSlotCount);
	IMPORT_C void Close();
	IMPORT_C TInt RequestLoadSup(const TSsmSupInfo& aUtilityPluginDetails);
	IMPORT_C void RequestLoadSup(const TDesC8& aTSsmSupInfoPckgC, TRequestStatus& aStatus);
	IMPORT_C void RequestLoadSupCancel();
	TInt RequestUnLoadSup(const TSsmSupInfo& aUtilityPluginDetails);

protected:
	TVersion Version() const;
	TInt DoConnect(const TDesC& aServerName, TVersion aVersion, TInt aAsyncMessageSlots);
	};

#endif	// __SSMSUSCLI_H__

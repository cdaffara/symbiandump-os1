// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_SUPL_CHANNEL_H__
#define __T_SUPL_CHANNEL_H__

#include <lbs/lbshostsettingsclasstypes.h> 

const TInt KSuplTestApiTestUid 	   	= 0x11002038;
const TInt KSuplTestApiChannelUid	= 0x03192383;


struct TSuplStubApiCmd
	{
	enum TChannelCmds
		{
		EResetHostSettings = 0,
		ESetDefaultSettings,
		} iCmd;
	
	TSuplStubApiCmd(TChannelCmds aCmd) : iCmd(aCmd) { ; }
	};
	
	
struct TSuplApiChannelReset : public TSuplStubApiCmd
	{
	TSuplApiChannelReset() : TSuplStubApiCmd(TSuplStubApiCmd::EResetHostSettings) { ; }
	};

struct TSuplApiSetDefaultSettings  : public TSuplStubApiCmd
	{
	TLbsHostSettingsSupl iSettings;
	
	TSuplApiSetDefaultSettings(const TLbsHostSettingsSupl& aSet) 
		: TSuplStubApiCmd(TSuplStubApiCmd::ESetDefaultSettings), iSettings(aSet) { ; }
	TSuplApiSetDefaultSettings() 
		: TSuplStubApiCmd(TSuplStubApiCmd::ESetDefaultSettings) { ; }
	};

#endif //  __T_SUPL_CHANNEL_H__


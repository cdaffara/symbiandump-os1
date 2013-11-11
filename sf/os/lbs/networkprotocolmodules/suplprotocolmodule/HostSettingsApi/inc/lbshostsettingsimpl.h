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
// This file defines the internal interface to the library that manages the list
// of remote hosts used by the LBS subsystem.
// 
//

/**
 @file
 @internalAll
 @deprecated 
*/

#ifndef LBSHOSTSETTINGSIMPL_H_
#define LBSHOSTSETTINGSIMPL_H_

#include <e32std.h>
#include <lbs/lbshostsettingsclasstypes.h>
#include <e32cmn.h>

class MLbsHostSettingsStoreObserver;

class MLbsHostSettingsStoreImpl
	{
public:
	virtual TInt CreateHostSettings(const TLbsHostSettingsBase& aSettings, TLbsHostCreatorId aCreatorId, TLbsHostSettingsId& aSettingsId) = 0;
	virtual TInt UpdateHostSettings(TLbsHostSettingsId aSettingsId, const TLbsHostSettingsBase& aSettings) =0;
	virtual TInt GetHostSettings(TLbsHostSettingsId aSettingsId, TLbsHostSettingsBase& aSettings) const =0;
	virtual TInt RewindHostSettings() =0;
	virtual TInt GetNextHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId) =0;
	virtual TInt GetNextHostSettingsByCreator(TLbsHostCreatorId aCreator, TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId) =0;
	virtual TInt SetDefaultHostSettings(TLbsHostSettingsId aSettingsId) =0; 
	virtual TInt GetDefaultHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId) const =0;
	virtual TInt DeleteHostSettings(TLbsHostSettingsId aSettingsId) =0;
	};


#endif // LBSHOSTSETTINGSIMPL_H_



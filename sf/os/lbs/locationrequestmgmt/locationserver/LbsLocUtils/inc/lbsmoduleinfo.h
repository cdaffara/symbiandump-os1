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

#ifndef LBS_PARTNER_MODULE_INFO_H
#define LBS_PARTNER_MODULE_INFO_H

// System
#include <e32base.h>

// Lbs
#include <lbscommon.h>


//-------------------------------------------------------------------------------
/**  The class is a wrapper around Lbs internal APIs and provides an access to 
the static module info for components that should not be dependent on the 
Location Server (e.g. AGPS integration module).

@see RPositionServer
@publishedPartner
@released
*/
class RLbsModuleInfo
	{
public:
	IMPORT_C RLbsModuleInfo();
	IMPORT_C void OpenL();
	IMPORT_C void Close();

	IMPORT_C static TInt GetNumModules(TUint& aNumModules);
	IMPORT_C static TInt GetModuleInfoByIndex(TInt aModuleIndex, TPositionModuleInfoBase& aModuleInfo);
	IMPORT_C static TInt GetModuleInfoById(TPositionModuleId aModuleId, TPositionModuleInfoBase& aModuleInfo);
    	IMPORT_C static TInt GetDataSourceId(TPositionModuleId aModuleId, TUid& aDataSourceId);

private:
	RLbsModuleInfo& operator=(const RLbsModuleInfo&);

private:
	TAny* iReserved;
	};
	
#endif //LBS_PARTNER_MODULE_INFO_H

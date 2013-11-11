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

/**
 @file
*/

#ifndef __AMASTARTCLI_H__
#define __AMASTARTCLI_H__

#include <e32std.h>

const TInt KAmaStartSrvMsgSlots = 1;
const TInt KAmaStartCliVersionMajor = 0; // 0-127
const TInt KAmaStartCliVersionMinor = 0; // 0-99
const TInt KAmaStartCliVersionBuild = 0; // 0-32767


/**
 * @internalTechnology
 * @released
 */
NONSHARABLE_CLASS( RAmaStartSession ) : public RSessionBase
	{
public: 
	IMPORT_C RAmaStartSession();
	IMPORT_C TInt Connect();
	IMPORT_C void Close();
	IMPORT_C void StartDsc( const TUid& aDscId, TRequestStatus& aStatus );
	IMPORT_C void StartDscCancel();
	
private:
	TBool CheckDscExistsL(const TUid& aDscId);

private:
	TVersion iVersion;
	TRequestStatus* iClientStatus; // Not owning
	TInt iSpare[ 4 ];
	};
	

#endif // __AMASTARTCLI_H__

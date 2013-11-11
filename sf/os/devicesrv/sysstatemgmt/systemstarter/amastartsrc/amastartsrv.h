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

#ifndef __AMASTARTSRV_H__
#define __AMASTARTSRV_H__

#include <e32std.h>
#include <f32file.h>
#include <s32strm.h>
#include <s32mem.h>

#include "amastartsess.h"
#include "amastartconst.h"
#include "amastartasync.h"
#include "ssmdebug.h"


const TInt KAmaStartSrvVersionMajor = 0; // 0-127
const TInt KAmaStartSrvVersionMinor = 0; // 0-99
const TInt KAmaStartSrvVersionBuild = 0; // 0-32767
const TInt KAmaStartSrvDesArg = 0;

_LIT( KAmaStartSrvName, "!AmaStartSrv" );

class CAmaStartSession;

/**
 * @internalTechnology
 * @released
 */
struct TAmaStartSessionInfo
	{
	CAmaStartAsync* iStarter;
	RMessagePtr2 iMessagePtr;
	TBool iInUse;
	};

	
/**
 * @internalTechnology
 * @released
 */
NONSHARABLE_CLASS( CAmaStartServer ) : public CServer2
	{

public:
	IMPORT_C static CAmaStartServer* NewLC();
	IMPORT_C static TInt StartAmaStartSrv();
	~CAmaStartServer();
	
	void StartDscL( const TUid& aDscId, const RMessage2& aMessage, const TInt aSessionIndex );
	void StartDscCancel( const TInt aSessionIndex );
	void RegisterSessionL( TInt& aSessionIndex );	
	void DeregisterSession( const TInt& aSessionIndex );
	// From CServer2
	IMPORT_C CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
	
private:
	CAmaStartServer();
	void ConstructL();	
	static TInt AmaStartSrvThreadFn( TAny* aAny );
	static void AmaStartSrvThreadRunL();
	
private:
	TVersion iVersion;	
	RArray<TAmaStartSessionInfo> iSessionInfoArray;
	TInt iSessionCount;
	};


#endif // __AMASTARTSRV_H__

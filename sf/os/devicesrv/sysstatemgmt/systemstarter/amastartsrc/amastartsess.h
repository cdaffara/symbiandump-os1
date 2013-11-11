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

#ifndef __AMASTARTSESS_H__
#define __AMASTARTSESS_H__

#include <e32std.h>
#include <e32base.h>
#include <e32capability.h>

#include "amastartsrv.h"
#include "amastartconst.h"



class CAmaStartServer;

/**
 * @internalTechnology
 * @released
 */
NONSHARABLE_CLASS( CAmaStartSession ) : public CSession2
	{
	
public:
	static CAmaStartSession* NewL( CAmaStartServer& aAmaStartServer );
	~CAmaStartSession();	
	// From CSession2
	void ServiceL( const RMessage2& aMessage );
	
private:
	CAmaStartSession( CAmaStartServer& aAmaStartServer );
	void ConstructL();
	
	void StartDscL( const RMessage2& aMessage );
	void StartDscCancel();
	
private:
	CAmaStartServer& iAmaStartSvr;
	TInt iSessionIndex;
	};



#endif // __AMASTARTSESS_H__

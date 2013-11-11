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

#ifndef __AMASTARTASYNC_H__
#define __AMASTARTASYNC_H__

#include <e32base.h>
#include <e32debug.h>
#include <dscstore.h>
#include <dscitem.h>

#include "amastartconst.h"
#include "startsafe.h"
#include "ssmdebug.h"

/**
 * @internalTechnology
 * @released
 */
NONSHARABLE_CLASS( CAmaStartAsync ) : public CActive
	{
public:
	static CAmaStartAsync* NewL( RMessage2 aMessage );
	static CAmaStartAsync* NewLC( RMessage2 aMessage );
	~CAmaStartAsync();
		
	void StartL( const TUid& aDscId );
		
protected:
	//From CActive
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();
		
private:
	CAmaStartAsync( RMessage2 aMessage );
	void ConstructL();
	void StartDscItemL(const CDscItem& aDscItem);
		
private:
	RDscStore iDscStore;
	CStartSafe* iStartSafe;
	RSysMonSession iSysMon;
	TBool iRunning;
	RMessage2 iMessage;
	};



#endif // __AMASTARTASYNC_H__

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

#ifndef __CLECLI_H__
#define __CLECLI_H__

#include <e32std.h>

#include "cleconst.h"
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "ssmdebug.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ssm/ssmcmd.hrh>

const TInt KCleSrvMsgSlots = 1; // This value was fixed during design. Beware adjustments.
const TInt KCleCliVersionMajor = 0; // 0-127
const TInt KCleCliVersionMinor = 0; // 0-99
const TInt KCleCliVersionBuild = 0; // 0-32767

class CSsmCommandList;
/**
 The CLE server client session.
 
 @internalComponent
 @released
 */
NONSHARABLE_CLASS( RCleSession ) : public RSessionBase
	{

public: 
	RCleSession();
	void ConnectL();
	void ConnectL(const TDesC& aServerName);
	void Close();
	void ExecuteCommandList( const CSsmCommandList& aCmdList, TRequestStatus& aStatus ,TCmdErrorSeverity& aSeverity);
	void ExecuteCommandListCancel();

private:
	TVersion iVersion;
	TInt iSpare[ 4 ];
	};
	

#endif // __CLECLI_H__

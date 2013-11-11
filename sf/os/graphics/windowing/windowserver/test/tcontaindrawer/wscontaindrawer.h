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
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __WSCONTAINDRAWER_H__
#define __WSCONTAINDRAWER_H__

#include <w32std.h>

const TUid KContainDrawerInterfaceId = {0x10283522};
const TUid KContainDrawerImplId = {0x10283521};
/** Client representation of a window-server-side drawing owned by this Client
@publishedAll
@released
*/
NONSHARABLE_CLASS(CWsContainGraphicBitmap): public CWsGraphic
	{
public:
	IMPORT_C static CWsContainGraphicBitmap* NewL();
	IMPORT_C static CWsContainGraphicBitmap* NewL(TUid aUid);
	IMPORT_C static CWsContainGraphicBitmap* NewL(const TWsGraphicId& aReplace);
	IMPORT_C ~CWsContainGraphicBitmap();
public: 
	IMPORT_C TInt ShareGlobally();
	IMPORT_C TInt UnShareGlobally();
	IMPORT_C TInt Share(TSecureId aClientId);
	IMPORT_C TInt UnShare(TSecureId aClientId);
	
	IMPORT_C TInt UpdateColor(TRgb aColor);
public:
	CWsContainGraphicBitmap();
	void HandleMessage(const TDesC8& aData);
	void OnReplace();
private:
	TBool iIsReady;
};

#endif

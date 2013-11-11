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

#ifndef __WSSIMPLEDRAWER_H__
#define __WSSIMPLEDRAWER_H__


#include <w32std.h>
const TUid KSimpleDrawerInterfaceId = {0x1028351B};
const TUid KSimpleDrawerImplId = {0x1028351C};

/** Client representation of a window-server-side drawing owned by this Client
@publishedAll
@released
*/
NONSHARABLE_CLASS(CWsSimpleGraphicBitmap): public CWsGraphic
	{
public:
	IMPORT_C static CWsSimpleGraphicBitmap* NewL();
	IMPORT_C static CWsSimpleGraphicBitmap* NewL(TUid aUid);
	IMPORT_C static CWsSimpleGraphicBitmap* NewL(const TWsGraphicId& aReplace);

	IMPORT_C ~CWsSimpleGraphicBitmap();
public: // protected virtuals from CWsGraphic promoted to public
	IMPORT_C TInt ShareGlobally();
	IMPORT_C TInt UnShareGlobally();
	IMPORT_C TInt Share(TSecureId aClientId);
	IMPORT_C TInt UnShare(TSecureId aClientId);
	
	IMPORT_C TInt UpdateColor(TRgb aColor);

public:
	CWsSimpleGraphicBitmap();
	void HandleMessage(const TDesC8& aData);
	void OnReplace();
public:
	TBool iIsReady;
};


const TUid KInvisibleDrawerInterfaceId1 = {0x1028353A};
const TUid KInvisibleDrawerImplId1 = {0x10283539};

/** Client representation of a window-server-side drawing owned by this Client
@publishedAll
@released
*/
NONSHARABLE_CLASS(CWsInvisibleGraphicBitmap1): public CWsSimpleGraphicBitmap
	{
public:
	IMPORT_C static CWsInvisibleGraphicBitmap1* NewL();
	IMPORT_C static CWsInvisibleGraphicBitmap1* NewL(TUid aUid);
	IMPORT_C static CWsInvisibleGraphicBitmap1* NewL(const TWsGraphicId& aReplace);
};


const TUid KInvisibleDrawerInterfaceId2 = {0x1028353C};
const TUid KInvisibleDrawerImplId2 = {0x1028353B};

/** Client representation of a window-server-side drawing owned by this Client
@publishedAll
@released
*/
NONSHARABLE_CLASS(CWsInvisibleGraphicBitmap2): public CWsSimpleGraphicBitmap
	{
public:
	IMPORT_C static CWsInvisibleGraphicBitmap2* NewL();
	IMPORT_C static CWsInvisibleGraphicBitmap2* NewL(TUid aUid);
	IMPORT_C static CWsInvisibleGraphicBitmap2* NewL(const TWsGraphicId& aReplace);
};



#endif

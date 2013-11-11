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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef WSBUFFERDRAWER_H_
#define WSBUFFERDRAWER_H_


#include <w32std.h>
const TUid KBufferDrawerInterfaceId = {0x10285D53};
const TUid KBufferDrawerImplId = {0x10285D54};

/** Client representation of a window-server-side drawing owned by this Client
@publishedAll
@released
*/
NONSHARABLE_CLASS(CWsBufferGraphic): public CWsGraphic
	{
public:
	IMPORT_C static CWsBufferGraphic* NewL();
	IMPORT_C static CWsBufferGraphic* NewL(TUid aUid);
	IMPORT_C static CWsBufferGraphic* NewL(const TWsGraphicId& aReplace);
	IMPORT_C ~CWsBufferGraphic();
	IMPORT_C TInt UpdateWhiteLinePos(TInt aWhiteLinePos);
	
public: // protected virtuals from CWsGraphic promoted to public
	IMPORT_C TInt ShareGlobally();
	IMPORT_C TInt UnShareGlobally();
	IMPORT_C TInt Share(TSecureId aClientId);
	IMPORT_C TInt UnShare(TSecureId aClientId);
	
public:
	CWsBufferGraphic();
	void HandleMessage(const TDesC8& aData);
	void OnReplace();
};

#endif /*WSBUFFERDRAWER_H_*/

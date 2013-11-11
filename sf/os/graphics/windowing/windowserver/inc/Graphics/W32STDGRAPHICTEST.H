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
// Test CWsGraphic plugins
// 
//

#ifndef __W32STDGRAPHICTEST_H__
#define __W32STDGRAPHICTEST_H__

#include <w32std.h>

class TWsGraphicFrameRate: public TWsGraphicMsgFixedBase
/** data attached to a CWindowGc::DrawWsGraphic to encourage the artwork to display
	an approximate framerate as it animates, if available
	@publishedPartner
	@released
*/	{
public:
	enum
		{
		ETypeId = 0x10281AAF
		};
	IMPORT_C TWsGraphicFrameRate();
	};

NONSHARABLE_CLASS(CWsGraphicTestFrameRate): public CWsGraphic
/** Client representation of a test anim that displays its frame-rate

@publishedPartner
@released
*/
	{
public:
	IMPORT_C static CWsGraphicTestFrameRate* NewL();
	IMPORT_C static CWsGraphicTestFrameRate* NewL(TUid aUid);
	IMPORT_C static CWsGraphicTestFrameRate* NewL(const TWsGraphicId& aReplace);
	IMPORT_C ~CWsGraphicTestFrameRate();
public: // protected virtuals from CWsGraphic promoted to public
	using CWsGraphic::ShareGlobally;
	using CWsGraphic::UnShareGlobally;
	using CWsGraphic::Share;
	using CWsGraphic::UnShare;
private:
	CWsGraphicTestFrameRate();
	void HandleMessage(const TDesC8& aData);
	void OnReplace();
};

#endif //__W32STDGRAPHICTEST_H__

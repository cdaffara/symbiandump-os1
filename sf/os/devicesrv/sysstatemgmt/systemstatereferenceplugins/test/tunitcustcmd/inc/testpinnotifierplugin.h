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

#if !defined(__TESTPINNOTIFIERPLUGIN_H__)
#define __TESTPINNOTIFIERPLUGIN_H__

#include "securitynotification.h"
#include <e32std.h>
#include <e32base.h>
#include <eiknotapi.h>
#include <eikdialg.h>
#include <e32cmn.h>


IMPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray();

_LIT8(KHiddenKeyRequired, "HIDDENKEY");

NONSHARABLE_CLASS (CSecurityPinNotifier) :  public CEikDialog, public MEikSrvNotifierBase2
	{
public:
	static CSecurityPinNotifier* NewLC();
	~CSecurityPinNotifier();

private:
	CSecurityPinNotifier();
	void ConstructL();
	TInt DisplayNotifier();

	//CEikDialog
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);

	//MEikSrvNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	void UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);

private:
	RMessagePtr2 iMessage;
	TNotifierInfo iInfo;
	TInt iReplySlot;
	};

#endif //__TESTPINNOTIFIERPLUGIN_H__

// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TBTNotifiers app
// 
//

#ifndef __AFX_TBTNOTIFIERS_H__BF0B56A0_9E93_11D4_B3DC_0050DA9A115C_
#define __AFX_TBTNOTIFIERS_H__BF0B56A0_9E93_11D4_B3DC_0050DA9A115C_

#include <coecntrl.h>
#include <coeccntx.h>

#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <bttypes.h>
#include <btmanclient.h>

const TUid KUidTBTNotifiersApp={ 0x10008B07 };


//
// class CTBTNotifiersContainer
//
class CTBTNotifiersContainer : public CCoeControl
	{
public:
	void ConstructL(const TRect& aRect);
	~CTBTNotifiersContainer();

	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;

private:
	void Draw(const TRect& aRect) const;

private:
	};



//
// class CTBTNotifiersEikDocument
//
class CTBTNotifiersEikDocument : public CEikDocument
	{
public:
	void ConstructL();
	CTBTNotifiersEikDocument(CEikApplication& aApp): CEikDocument(aApp) { }
	~CTBTNotifiersEikDocument() {}
	// Stop doc file being created
	CFileStore* OpenFileL(TBool /*aDoOpen*/,const TDesC& /*aFilename*/,RFs& /*aFs*/) {return NULL;}
private:
	// from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


//
// CTBTNotifiersAppUi
//

class CTBTNotifiersAppUi : public CEikAppUi
	{
public:
	void ConstructL();
	~CTBTNotifiersAppUi();
private: // from CEikAppUi
	void HandleCommandL(TInt aCommand);
	void TestPinL();
	void TestAuthorisationL();
	void TestDeviceSearchL();
private:
	CTBTNotifiersContainer* iAppView;
	};

//
// CTBTNotifiersApplication
//

class CTBTNotifiersApplication : public CEikApplication
	{
private:
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	CApaDocument* CreateDocumentL(CApaProcess* a) { return CEikApplication::CreateDocumentL(a); }
	TUid AppDllUid() const;
	};


#endif

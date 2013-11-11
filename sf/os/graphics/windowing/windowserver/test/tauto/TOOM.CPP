// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Out of memory tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "TOOM.H"

 
typedef COomFailBase *(*COmmCreate)(CTOom *aTest);

COomFailBase *CreateOomConnect(CTOom *aTest)
	{return(new(ELeave) COomConnect(aTest));}

COomFailBase *CreateOomWindow(CTOom *aTest)
	{return(new(ELeave) COomWindow(aTest));}

COomFailBase *CreateOomWindowGroup(CTOom *aTest)
	{return(new(ELeave) COomWindowGroup(aTest));}

COomFailBase *CreateOomBackedUpWindow(CTOom *aTest)
	{return(new(ELeave) COomBackedUpWindow(aTest));}

COomFailBase *CreateOomBackupResize(CTOom *aTest)
	{return(new(ELeave) COomBackupResize(aTest));}

COomFailBase *CreateOomBlankWindow(CTOom *aTest)
	{return(new(ELeave) COomBlankWindow(aTest));}

COomFailBase *CreateOomGc(CTOom *aTest)
	{return(new(ELeave) COomGc(aTest));}

COomFailBase *CreateOomScreenDevice(CTOom *aTest)
	{return(new(ELeave) COomScreenDevice(aTest));}

COomFailBase *CreateOomPointerBuffer(CTOom *aTest)
	{return(new(ELeave) COomPointerBuffer(aTest));}

COomFailBase *CreateOomPolygon(CTOom *aTest)
	{return(new(ELeave) COomPolygon(aTest));}

COomFailBase *CreateOomPriorityKey(CTOom *aTest)
	{return(new(ELeave) COomPriorityKey(aTest));}

COomFailBase *CreateOomCaptureKey(CTOom *aTest)
	{return(new(ELeave) COomCaptureKey(aTest));}

COomFailBase *CreateOomCaptureKeyUpDown(CTOom *aTest)
	{return(new(ELeave) COomCaptureKeyUpDown(aTest));}

COomFailBase *CreateOomLongKeyCapture(CTOom *aTest)
	{return(new(ELeave) COomLongKeyCapture(aTest));}

COomFailBase *CreateOomHotKey(CTOom *aTest)
	{return(new(ELeave) COomHotKey(aTest));}

COomFailBase *CreateOomGroupName(CTOom *aTest)
	{return(new(ELeave) COomGroupName(aTest));}

COomFailBase *CreateOomMessageSend(CTOom *aTest)
	{return(new(ELeave) COomMessageSend(aTest));}

COomFailBase *CreateOomMessageFetch(CTOom *aTest)
	{return(new(ELeave) COomMessageFetch(aTest));}

COomFailBase *CreateOomSprite(CTOom *aTest)
	{return(new(ELeave) COomSprite(aTest));}

COomFailBase *CreateOomPointerCursor(CTOom *aTest)
	{return(new(ELeave) COomPointerCursor(aTest));}

COomFailBase *CreateOomCopyScreen(CTOom *aTest)
	{return(new(ELeave) COomCopyScreen(aTest));}

COomFailBase *CreateOomRequestEvents(CTOom *aTest)
	{return(new(ELeave) COomRequestEvents(aTest));}

COomFailBase *CreateOomCustomTextCursor(CTOom *aTest)
	{return(new(ELeave) COomCustomTextCursor(aTest));}

COomFailBase *CreateOomTranspWindow(CTOom *aTest)
	{return(new(ELeave) COomTranspWindow(aTest));}

COomFailBase *CreateOomObscuredWindow(CTOom *aTest)
	{return(new(ELeave) COomObscuredWindow(aTest));}

COmmCreate CreateOomFailTest[]={
	CreateOomConnect,
	CreateOomWindow,
	CreateOomWindowGroup,
	CreateOomBackedUpWindow,
	CreateOomBackupResize,
	CreateOomBlankWindow,
	CreateOomGc,
	CreateOomScreenDevice,
	CreateOomPointerBuffer,
	CreateOomPolygon,
	CreateOomPriorityKey,
	CreateOomCaptureKey,
	CreateOomCaptureKeyUpDown,
	CreateOomLongKeyCapture,
	CreateOomHotKey,
	CreateOomGroupName,
	CreateOomSprite,
	CreateOomPointerCursor,
	CreateOomCopyScreen,
	CreateOomRequestEvents,
	CreateOomMessageSend,
	CreateOomMessageFetch,
	CreateOomCustomTextCursor,
	CreateOomTranspWindow,
	CreateOomObscuredWindow,
	};

//
// Individual out of memory test classes //
//

COomFailBase::COomFailBase(CTOom *aTest) : iTest(aTest)
	{}

void COomFailBase::ConstructL()
	{
	iTest->INFO_PRINTF1(TestName());
	}

void COomFailBase::PreFail()
	{}

void COomFailBase::ClearUpL()
	{}

void COomFailBase::Flush()
	{
	iWs.Flush();
	}

COomConnect::COomConnect(CTOom *aTest) : COomFailBase(aTest)
	{}

TOomTestName COomConnect::TestName()
	{
	return(_L("Connect"));
	}

/** Creates a wserv session, connects and creates CWsScreenDevice object
*/
TInt COomConnect::Fail()
	{
	TInt err = iWs.Connect();
	if (err!=KErrNone)
		return err;
	
	TRAP(err, iDummyScreen = new (ELeave) CWsScreenDevice(iWs));
	if (err!=KErrNone)
		{
		iWs.Close();
		return err;
		}
		
	if ((err=iDummyScreen->Construct(iTest->ScreenNumber()))!=KErrNone)
		{
		delete iDummyScreen;
		iDummyScreen = NULL;
		iWs.Close();
		return err;
		}
		
	return err;
	}

void COomConnect::ClearUpL()
	{
	if (iDummyScreen)
		{
		delete iDummyScreen;
		iDummyScreen = NULL;
		}
	iWs.Close();
	}

//

COomSetup::COomSetup(CTOom *aTest) : COomFailBase(aTest)
	{}

void COomSetup::ConstructL()
	{
	COomFailBase::ConstructL();
	User::LeaveIfError(iWs.Connect());
	iDummyScreen = new (ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iDummyScreen->Construct(iTest->ScreenNumber()));
	
	iWinGroup=RWindowGroup(iWs);
	iWinGroup.Construct(556);
	iWinGroup.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	}

COomSetup::~COomSetup()
	{
	iWinGroup.Close();
	delete iDummyScreen;
	iDummyScreen = NULL;
	
	iTest->TEST(iWs.ResourceCount()==0);
	if (iWs.ResourceCount()!=0)
		iTest->INFO_PRINTF3(_L("iWs.ResourceCount() return value - Expected: %d, Actual: %d"), 0, iWs.ResourceCount());			

	iWs.Close();
	}

//

COomWindowGroup::COomWindowGroup(CTOom *aTest) : COomSetup(aTest)
	{}

TOomTestName COomWindowGroup::TestName()
	{
	return(_L("Window Group"));
	}

/** Creates a RWindowGroup object
*/
TInt COomWindowGroup::Fail()
	{
	iFailWinGroup=RWindowGroup(iWs);
	return(iFailWinGroup.Construct(987));
	}

void COomWindowGroup::ClearUpL()
	{
	iFailWinGroup.Close();
	}

//

COomWindow::COomWindow(CTOom *aTest) : COomSetup(aTest)
	{}

TOomTestName COomWindow::TestName()
	{
	return(_L("Window"));
	}

/** Creates a RWindow object
*/
TInt COomWindow::Fail()
	{
	iFailWin=RWindow(iWs);
	return(iFailWin.Construct(iWinGroup,11));
	}

void COomWindow::ClearUpL()
	{
	iFailWin.Close();
	}

//

COomBackedUpWindow::COomBackedUpWindow(CTOom *aTest) : COomSetup(aTest)
	{}

TOomTestName COomBackedUpWindow::TestName()
	{
	return(_L("Backed up window"));
	}

/** Creates a RBackedUpWindow object
*/
TInt COomBackedUpWindow::Fail()
	{
	iFailWin=RBackedUpWindow(iWs);
	return(iFailWin.Construct(iWinGroup,EGray16,22));
	}

void COomBackedUpWindow::ClearUpL()
	{
	iFailWin.Close();
	}

//

COomBackupResize::COomBackupResize(CTOom *aTest) : COomSetup(aTest)
	{}

COomBackupResize::~COomBackupResize()
	{
	iFailWin.Close();
	}

TOomTestName COomBackupResize::TestName()
	{
	return(_L("Resize backed up window"));
	}

void COomBackupResize::ConstructL()
	{
	COomSetup::ConstructL();
	iFailWin=RBackedUpWindow(iWs);
	User::LeaveIfError(iFailWin.Construct(iWinGroup,EGray16,33));
	User::LeaveIfError(iFailWin.SetSizeErr(TSize(10,10)));
	iFailWin.Activate();
	}
	
/** Sets the size of previously created backed-up window
*/
TInt COomBackupResize::Fail()
	{
	return(iFailWin.SetSizeErr(TSize(100,100)));
	}

void COomBackupResize::ClearUpL()
//Virtual fn declared in CoomFailBase. This is the only one that can leave, I think, but one is enough
	{
	User::LeaveIfError(iFailWin.SetSizeErr(TSize(10,10)));
	}

//

COomBlankWindow::COomBlankWindow(CTOom *aTest) : COomSetup(aTest)
	{}

TOomTestName COomBlankWindow::TestName()
	{
	return(_L("Blank window"));
	}

/** Creates a RBlankWindow object
*/
TInt COomBlankWindow::Fail()
	{
	iFailWin=RBlankWindow(iWs);
	return(iFailWin.Construct(iWinGroup,44));
	}

void COomBlankWindow::ClearUpL()
	{
	iFailWin.Close();
	}

//

COomPointerBuffer::COomPointerBuffer(CTOom *aTest) : COomSetup(aTest)
	{}

COomPointerBuffer::~COomPointerBuffer()
	{
	iFailWin.Close();
	}

TOomTestName COomPointerBuffer::TestName()
	{
	return(_L("Pointer buffer"));
	}

void COomPointerBuffer::ConstructL()
	{
	COomSetup::ConstructL();
	iFailWin=RBackedUpWindow(iWs);
	User::LeaveIfError(iFailWin.Construct(iWinGroup,EGray16,55));
	User::LeaveIfError(iFailWin.SetSizeErr(TSize(10,10)));
	iFailWin.Activate();
	}

/** Allocates a buffer for storing pointer movements for previously created backed-up window
*/
TInt COomPointerBuffer::Fail()
	{
	return(iFailWin.AllocPointerMoveBuffer(10,0));
	}

void COomPointerBuffer::ClearUpL()
	{
	iFailWin.FreePointerMoveBuffer();
	}

//

COomPriorityKey::COomPriorityKey(CTOom *aTest) : COomSetup(aTest)
	{}

COomPriorityKey::~COomPriorityKey()
	{
	}

TOomTestName COomPriorityKey::TestName()
	{
	return(_L("Priority key"));
	}

void COomPriorityKey::ConstructL()
	{
	COomSetup::ConstructL();
	}

/** Adds a priority key for a previously created window group
*/
TInt COomPriorityKey::Fail()
	{
	return(iWinGroup.AddPriorityKey(1,0,0));
	}

void COomPriorityKey::ClearUpL()
	{
	iWinGroup.RemovePriorityKey(1,0,0);
	}

//

COomCaptureKeyBase::COomCaptureKeyBase(CTOom *aTest) : COomSetup(aTest)
	{}

COomCaptureKeyBase::~COomCaptureKeyBase()
	{}

void COomCaptureKeyBase::ConstructL()
	{
	COomSetup::ConstructL();
	}

COomCaptureKey::COomCaptureKey(CTOom *aTest) : COomCaptureKeyBase(aTest)
	{}

COomCaptureKey::~COomCaptureKey()
	{
	}

TOomTestName COomCaptureKey::TestName()
	{
	return(_L("Capture key"));
	}

/** Requests a capture keys for a previously created window group
*/
TInt COomCaptureKey::Fail()
	{
	for (iIndex=0;iIndex<(TInt)(sizeof(iCapKey)/sizeof(iCapKey[0]));iIndex++)
		{
		TInt ret=iWinGroup.CaptureKey('a',0,0);
		if (ret<0)
			return(ret);
		iCapKey[iIndex]=ret;
		}
	return(KErrNone);
	}

void COomCaptureKey::ClearUpL()
	{
	for (TInt index=0;index<iIndex;index++)
		iWinGroup.CancelCaptureKey(iCapKey[index]);
	}

//

COomCaptureKeyUpDown::COomCaptureKeyUpDown(CTOom *aTest) : COomCaptureKeyBase(aTest)
	{}

COomCaptureKeyUpDown::~COomCaptureKeyUpDown()
	{
	}

TOomTestName COomCaptureKeyUpDown::TestName()
	{
	return(_L("Capture up/down keys"));
	}

/** Requests the capture of key-up and key-down events for a previously created window group
*/
TInt COomCaptureKeyUpDown::Fail()
	{
	for (iIndex=0;iIndex<(TInt)(sizeof(iCapKey)/sizeof(iCapKey[0]));iIndex++)
		{
		TInt ret=iWinGroup.CaptureKeyUpAndDowns('a',0,0);
		if (ret<0)
			return(ret);
		iCapKey[iIndex]=ret;
		}
	return(KErrNone);
	}

void COomCaptureKeyUpDown::ClearUpL()
	{
	for (TInt index=0;index<iIndex;index++)
		iWinGroup.CancelCaptureKeyUpAndDowns(iCapKey[index]);
	}

//

COomLongKeyCapture::COomLongKeyCapture(CTOom *aTest) : COomCaptureKeyBase(aTest)
	{}

COomLongKeyCapture::~COomLongKeyCapture()
	{
	}

TOomTestName COomLongKeyCapture::TestName()
	{
	_LIT(KLongKeyCapTestName, "Long key capture");
	return KLongKeyCapTestName();
	}

/** 
Requests capture of long key events for a previously created window group
*/
TInt COomLongKeyCapture::Fail()
	{
	for (iIndex=0; iIndex < KNumCapKeyRequest; iIndex++)
		{
		TInt ret=iWinGroup.CaptureLongKey(' ','a',0,0,2,ELongCaptureNormal);
		if (ret<0)
			return(ret);
		iCapKey[iIndex]=ret;
		}
	return(KErrNone);
	}

void COomLongKeyCapture::ClearUpL()
	{
	for (TInt index=0;index<iIndex;index++)
		iWinGroup.CancelCaptureLongKey(iCapKey[index]);
	}

//

COomHotKey::COomHotKey(CTOom *aTest) : COomSetup(aTest)
	{}

COomHotKey::~COomHotKey()
	{
	}

TOomTestName COomHotKey::TestName()
	{
	return(_L("hot keys"));
	}

void COomHotKey::ConstructL()
	{
	COomSetup::ConstructL();
	}

/** Sets hot key for the session.
*/
TInt COomHotKey::Fail()
	{
	return(iWs.SetHotKey(EHotKeyEnableLogging,'a',0,0));
	}

void COomHotKey::ClearUpL()
	{
	iWs.RestoreDefaultHotKey(EHotKeyEnableLogging);
	}

//

COomGroupName::COomGroupName(CTOom *aTest) : COomSetup(aTest)
	{}

COomGroupName::~COomGroupName()
	{
	}

TOomTestName COomGroupName::TestName()
	{
	return(_L("Group name"));
	}

void COomGroupName::ConstructL()
	{
	COomSetup::ConstructL();
	}

/** Sets the window group's name.
*/
TInt COomGroupName::Fail()
	{
	return(iWinGroup.SetName(_L("A Name")));
	}

void COomGroupName::ClearUpL()
	{
	iWinGroup.SetName(_L(""));
	}

//

COomMessageSend::COomMessageSend(CTOom *aTest) : COomSetup(aTest)
	{}

COomMessageSend::~COomMessageSend()
	{
	}

TOomTestName COomMessageSend::TestName()
	{
	return(_L("MessageSend"));
	}

void COomMessageSend::ConstructL()
	{
	COomSetup::ConstructL();
	}

void COomMessageSend::PreFail()
	{
	iWinGroup2=RWindowGroup(iWs);
	iWinGroup2.Construct(557);
	iWinGroup2.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	}

/** Sends a message to another window group
*/
TInt COomMessageSend::Fail()
	{
	TBuf8<0x20> params(_L8("SomeParams"));
	return(iWs.SendMessageToWindowGroup(iWinGroup2.Identifier(),TUid::Uid(123),params));
	}

void COomMessageSend::ClearUpL()
	{
	iWinGroup2.Close();
	}

//

COomMessageFetch::COomMessageFetch(CTOom *aTest) : COomSetup(aTest)
	{}

COomMessageFetch::~COomMessageFetch()
	{
	}

TOomTestName COomMessageFetch::TestName()
	{
	return(_L("MessageFetch"));
	}

void COomMessageFetch::ConstructL()
	{
	COomSetup::ConstructL();
	}

#define CLIENT_HANDLE 555
#define UID_VALUE 123
#define MESSAGE_LEN 0x20
void COomMessageFetch::PreFail()
	{
	iWinGroup2=RWindowGroup(iWs);
	iWinGroup2.Construct(CLIENT_HANDLE);
	iWinGroup2.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	TBuf8<MESSAGE_LEN> params(_L8("SomeParams"));
	TBool retVal = iWs.SendMessageToWindowGroup(iWinGroup2.Identifier(),TUid::Uid(UID_VALUE),params);
	iTest->TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		iTest->INFO_PRINTF3(_L("iWs.SendMessageToWindowGroup() return value - Expected: %d, Actual: %d"), KErrNone, retVal);			
	}

/** Fetches a message
*/
TInt COomMessageFetch::Fail()
	{
	TWsEvent event;
	TPtr8 ptr(NULL,0);
	TUid uid;
	event.SetType(EEventMessageReady);
	event.SetHandle(CLIENT_HANDLE);
	event.SetTimeNow();
	SEventMessageReady& eventMessageReady=*(SEventMessageReady*)event.EventData();
	eventMessageReady.iWindowGroupIdentifier=iWinGroup2.Identifier();
	eventMessageReady.iMessageUid=TUid::Uid(UID_VALUE);
	eventMessageReady.iMessageParametersSize=MESSAGE_LEN;
	TInt ret=TheClient->iWs.FetchMessage(uid,ptr,event);
	if (ret==KErrNone)
		User::Free((TAny *)ptr.Ptr());
	return(ret);
	}

void COomMessageFetch::ClearUpL()
	{
	iWinGroup2.Close();
	}

//

COomRequestEvents::COomRequestEvents(CTOom *aTest) : COomSetup(aTest)
	{}

COomRequestEvents::~COomRequestEvents()
	{
	}

TOomTestName COomRequestEvents::TestName()
	{
	return(_L("Request events"));
	}

void COomRequestEvents::ConstructL()
	{
	COomSetup::ConstructL();
	}

/** Enables window group change events and modifier change events
*/
TInt COomRequestEvents::Fail()
	{
	TInt err;
	if ((err=iWinGroup.EnableOnEvents())==KErrNone)
		if ((err=iWinGroup.EnableGroupChangeEvents())==KErrNone)
			err=iWinGroup.EnableModifierChangedEvents(EModifierCapsLock,EEventControlAlways);
	return(err);
	}

void COomRequestEvents::ClearUpL()
	{
	iWinGroup.DisableOnEvents();
	iWinGroup.DisableGroupChangeEvents();
	iWinGroup.DisableModifierChangedEvents();
	}

//

COomCopyScreen::COomCopyScreen(CTOom *aTest) : COomSetup(aTest)
	{}

TOomTestName COomCopyScreen::TestName()
	{
	return(_L("Copy screen"));
	}

COomCopyScreen::~COomCopyScreen()
	{
	delete iScrDev;
	delete iBitmap;
	}

void COomCopyScreen::ConstructL()
	{
	COomSetup::ConstructL();
	iScrDev=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScrDev->Construct(iTest->ScreenNumber()));
	iBitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmap->Create(TSize(100,100),EGray4));
	}

/** Copies screen to bitmap
*/
TInt COomCopyScreen::Fail()
	{
	TInt ret=iScrDev->CopyScreenToBitmap(iBitmap);
	if (ret==KErrNone)
		ret=iScrDev->CopyScreenToBitmap(iBitmap,TRect(10,10,50,50));
	return(ret);
	}

void COomCopyScreen::ClearUpL()
	{
	}

//

COomSpriteBase::COomSpriteBase(CTOom *aTest) : COomSetup(aTest)
	{}

COomSpriteBase::~COomSpriteBase()
	{
	iWin.Close();
	delete iBitmap;
	delete iMask;
	delete iBitmap2;
	delete iMask2;
	}

void COomSpriteBase::ConstructL()
	{
	COomSetup::ConstructL();
	iWin=RWindow(iWs);
	User::LeaveIfError(iWin.Construct(iWinGroup,33));
	User::LeaveIfError(iWin.SetSizeErr(TSize(100,100)));
	iWin.Activate();
	iBitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmap->Create(TSize(10,10),EGray4));
	iBitmap2=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmap2->Create(TSize(40,40),EGray4));
	iMask=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iMask->Create(TSize(10,10),EGray4));
	iMask2=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iMask2->Create(TSize(40,40),EGray4));
	}

//

COomSprite::COomSprite(CTOom *aTest) : COomSpriteBase(aTest)
	{}

TOomTestName COomSprite::TestName()
	{
	return(_L("Sprites"));
	}

/** Creates a sprite.
*/
TInt COomSprite::Fail()
	{
	iSprite=RWsSprite(iWs);
	TInt err=iSprite.Construct(iWin,TPoint(10,10),0);
	if (err==KErrNone)
		{
		TSpriteMember sprite;
		sprite.iBitmap=iBitmap;
		sprite.iMaskBitmap=iMask;
		sprite.iInvertMask=EFalse;
		sprite.iDrawMode=CGraphicsContext::EDrawModePEN;
		sprite.iOffset=TPoint(0,0);
		sprite.iInterval=TTimeIntervalMicroSeconds32(100000);
		if ((err=iSprite.AppendMember(sprite))==KErrNone)
			if ((err=iSprite.Activate())==KErrNone)
				{
				sprite.iBitmap=iBitmap2;		// Bigger bitmap to force resize of bitmap
				sprite.iMaskBitmap=iMask2;
				if ((err=iSprite.AppendMember(sprite))==KErrNone)
					err=iSprite.UpdateMember(1,sprite);
				}
		}
	return(err);
	}

void COomSprite::ClearUpL()
	{
	iSprite.Close();
	}

//

COomPointerCursor::COomPointerCursor(CTOom *aTest) : COomSpriteBase(aTest)
	{}

TOomTestName COomPointerCursor::TestName()
	{
	return(_L("Pointer cursor"));
	}

/** Creates a pointer cursor.
*/
TInt COomPointerCursor::Fail()
	{
	iPointerCursor=RWsPointerCursor(iWs);
	TInt err=iPointerCursor.Construct(0);
	if (err==KErrNone)
		{
		TSpriteMember sprite;
		sprite.iBitmap=iBitmap;
		sprite.iMaskBitmap=iMask;
		sprite.iInvertMask=EFalse;
		sprite.iDrawMode=CGraphicsContext::EDrawModePEN;
		sprite.iOffset=TPoint(0,0);
		sprite.iInterval=TTimeIntervalMicroSeconds32(100000);
		if ((err=iPointerCursor.AppendMember(sprite))==KErrNone)
			if ((err=iPointerCursor.Activate())==KErrNone)
				{
				sprite.iBitmap=iBitmap2;		// Bigger bitmap to force resize of bitmap
				sprite.iMaskBitmap=iMask2;
				if ((err=iPointerCursor.AppendMember(sprite))==KErrNone)
					err=iPointerCursor.UpdateMember(1,sprite);
				}
		}
	return(err);
	}

void COomPointerCursor::ClearUpL()
	{
	iPointerCursor.Close();
	}

//

COomGc::COomGc(CTOom *aTest) : COomSetup(aTest)
	{}

COomGc::~COomGc()
	{
	delete iScrDev;
	}

TOomTestName COomGc::TestName()
	{
	return(_L("Graphic Context"));
	}

void COomGc::ConstructL()
	{
	COomSetup::ConstructL();
	iScrDev=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScrDev->Construct(iTest->ScreenNumber()));
	}

/** Creates a graphical context for a previously created screen device.
*/
TInt COomGc::Fail()
	{
	TRAPD(err,iFailGc=new(ELeave) CWindowGc(iScrDev));
	if (err!=KErrNone)
		return(err);
	return(iFailGc->Construct());
	}

void COomGc::ClearUpL()
	{
	delete iFailGc;
	iFailGc=NULL;
	}

//

COomPolygon::COomPolygon(CTOom *aTest) : COomSetup(aTest), iPnts(5)
	{}

COomPolygon::~COomPolygon()
	{
	delete iGc;
	delete iScrDev;
	iWin.Close();
	}

TOomTestName COomPolygon::TestName()
	{
	return(_L("Polygon"));
	}

void COomPolygon::ConstructL()
	{
	COomSetup::ConstructL();
	iScrDev=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScrDev->Construct(iTest->ScreenNumber()));
	User::LeaveIfError(iScrDev->CreateContext(iGc));
	iWin=RBackedUpWindow(iWs);
	User::LeaveIfError(iWin.Construct(iWinGroup,EGray16,33));
	User::LeaveIfError(iWin.SetSizeErr(TSize(40,40)));
	iWin.Activate();
	TInt index;
	for(index=0;index<20;index+=2)
		iPnts.AppendL(TPoint(20+index,index));
	for(index=0;index<20;index+=2)
		iPnts.AppendL(TPoint(40-index,20+index));
	for(index=0;index<20;index+=2)
		iPnts.AppendL(TPoint(20-index,40-index));
	for(index=0;index<20;index+=2)
		iPnts.AppendL(TPoint(index,20-index));
	}

/** Draws a polygon.
*/
TInt COomPolygon::Fail()
	{
	iGc->Activate(iWin);
	iGc->Clear();
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(TRgb::Gray4(2));
	iGc->SetPenColor(TRgb::Gray4(0));
	TInt err=iGc->DrawPolygon(&iPnts,CGraphicsContext::EAlternate);
	if (err!=KErrNone)
		return(err);
	err=iGc->DrawPolygon(&iPnts,CGraphicsContext::EWinding);
	if (err!=KErrNone)
		return(err);
	return(KErrNone);
	}

void COomPolygon::ClearUpL()
	{
	iGc->Deactivate();
	}

//

COomScreenDevice::COomScreenDevice(CTOom *aTest) : COomSetup(aTest)
	{}

TOomTestName COomScreenDevice::TestName()
	{
	return(_L("Screen device"));
	}

/** Creates a screen device handle.
*/
TInt COomScreenDevice::Fail()
	{
	TRAPD(err,iScrDev=new(ELeave) CWsScreenDevice(iWs));
	if (err!=KErrNone)
		return(err);
	return(iScrDev->Construct(iTest->ScreenNumber()));
	}

void COomScreenDevice::ClearUpL()
	{
	delete iScrDev;
	iScrDev=NULL;
	}

//

COomCustomTextCursor::COomCustomTextCursor(CTOom *aTest) : COomSetup(aTest)
	{}

COomCustomTextCursor::~COomCustomTextCursor()
	{
	delete iMemberArray;
	delete iBitmap;
	}

TOomTestName COomCustomTextCursor::TestName()
	{
	return(_L("Custom Text Cursor"));
	}

void COomCustomTextCursor::ConstructL()
	{
	COomSetup::ConstructL();

	iBitmap = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Load(TEST_BITMAP_NAME, EMbmWsautotestBmp1));

	TSpriteMember member;
	member.iMaskBitmap=NULL;
	member.iInvertMask=EFalse;
	member.iDrawMode=CGraphicsContext::EDrawModePEN;
	member.iOffset=TPoint();
	member.iInterval=TTimeIntervalMicroSeconds32(0);
	member.iBitmap = iBitmap;

	iMemberArray = new(ELeave) CArrayFixFlat<TSpriteMember>(1);
	iMemberArray->AppendL(member);
	}

/** Adds a custom text cursor to the server's list of cursors.
*/
TInt COomCustomTextCursor::Fail()
	{
	TInt err = TheClient->iWs.SetCustomTextCursor(0x98765432, iMemberArray->Array(), 0, RWsSession::ECustomTextCursorAlignTop);
	if (err == KErrAlreadyExists)
		return KErrNone;
	else
		return err;
	}

void COomCustomTextCursor::ClearUpL()
	{
	}

COomTranspWindow::COomTranspWindow(CTOom *aTest) : COomSetup(aTest)
	{}

COomTranspWindow::~COomTranspWindow()
	{
	delete iBackgroundWin;
	delete iFirst;
	delete iSecond;
	
	delete iFbsBitmap;
	delete iWsBitmap;
	delete iFbsBitmapDevice; 
	delete iFbsBitmapGc;
	delete iWsBitmapDevice; 
	delete iWsBitmapGc;				
	}

TOomTestName COomTranspWindow::TestName()
	{
	return(_L("Transparent Window"));
	}

void COomTranspWindow::ConstructL()
	{
	COomSetup::ConstructL();
	
	iTransparencyEnabled = (TransparencySupportedL() == KErrNone);
	if(!iTransparencyEnabled)
		return;

	iFbsBitmap = new (ELeave) CFbsBitmap;

	// Do the creation 
	const TSize KSizeForBitmap(10,10);
	iFbsBitmap->Create(KSizeForBitmap,EGray256);
	
	// Fill the bitmap with a colour		
	iFbsBitmapDevice=CFbsBitmapDevice::NewL(iFbsBitmap);
		
	iFbsBitmapDevice->CreateContext(iFbsBitmapGc);
	
	iFbsBitmapGc->SetPenStyle(CGraphicsContext::ENullPen);
	iFbsBitmapGc->SetBrushStyle(CGraphicsContext::ESolidBrush);	
	iFbsBitmapGc->SetBrushColor(128);
	iFbsBitmapGc->DrawRect(TRect(KSizeForBitmap));
	
	// Create a CWsBitmap, and fill it with a colour	
	iWsBitmap = new (ELeave) CWsBitmap(TheClient->iWs);
	
	// Do the creation 
	iWsBitmap->Create(KSizeForBitmap,EGray256);	
		
	// Fill the bitmap with a colour		
	iWsBitmapDevice=CFbsBitmapDevice::NewL(iWsBitmap);	
			
	iWsBitmapDevice->CreateContext(iWsBitmapGc);		
	iWsBitmapGc->SetPenStyle(CGraphicsContext::ENullPen);
	iWsBitmapGc->SetBrushStyle(CGraphicsContext::ESolidBrush);	
	iWsBitmapGc->SetBrushColor(128);
	iWsBitmapGc->DrawRect(TRect(KSizeForBitmap));				
	}
	

/** Does nothing if transparency is not enabled. Creates a blank window with 
* two transparent child-windows. Tests setting the transparency via a factor, CFbsBitmap
* or a CWsBitmap, and changing from one to the other.
* Changes child-windows' sizes and toggles their visibility.
*/
TInt COomTranspWindow::Fail()
	{
	if(!iTransparencyEnabled)
		return KErrNone;
	TSize scrSize(TheClient->iScreen->SizeInPixels());
	TRAPD(res, iBackgroundWin = new (ELeave) CBlankWindow(TRgb(0,0,238)));
	if(res != KErrNone)
		{
		return res;	
		}

	TDisplayMode mode=EColor256;
	TRAP(res, iBackgroundWin->SetUpL(TPoint(50,50),scrSize-TSize(100,100),TheClient->iGroup,*TheClient->iGc,&mode));
	if(res != KErrNone)
		{
		return res;
		}

	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	iBackgroundWin->Win()->SetBackgroundColor(TRgb(51,204,51));
	iBackgroundWin->Invalidate();

	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	TRect	thePos1(20, 20, 60, 60);
	TRect	thePos2(100, 40, 160, 80);
	TRAP(res, iFirst = CTransWindow::NewL(iBackgroundWin, TRgb(255,0,0,128),&mode));
	if(res != KErrNone)
		{
		return res;
		}

	TRAP(res, iSecond = CTransWindow::NewL(iBackgroundWin, TRgb(255,0,0,128),&mode));
	if(res != KErrNone)
		{
		return res;
		}

	TRAP(res, iFirst->SetExtL(thePos1.iTl,thePos1.Size()));
	if(res != KErrNone)
		{
		return res;
		}

	TRAP(res, iSecond->SetExtL(thePos2.iTl,thePos2.Size()));
	if(res != KErrNone)
		{
		return res;
		}

	res=SetTransparencyTesting();
	if(res != KErrNone)
		{
		return res;
		}

	TheClient->Flush();
	//TheClient->WaitForRedrawsToFinish();

	iFirst->Activate();
	iFirst->AssignGC(*TheClient->iGc);

	iSecond->Activate();
	iSecond->AssignGC(*TheClient->iGc);

	iFirst->Win()->SetBackgroundColor(TRgb(0, 0, 255));
	iSecond->Win()->SetBackgroundColor(TRgb(0, 0, 255));
	iFirst->Invalidate();
	iSecond->Invalidate();
	
	iFirst->AdjustSize(150,10, 0);
	iSecond->AdjustSize(150,10, 0);

	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	iFirst->ToggleVisibility();
	iSecond->ToggleVisibility();

	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	
	iFirst->AdjustSize(200,10, 0);
	iSecond->AdjustSize(200,10, 0);

	iFirst->ToggleVisibility();
	iSecond->ToggleVisibility();

	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	return KErrNone;
	}


// This function tests setting the transparency via a factor, CFbsBitmap
// or a CWsBitmap, and changing from one to the other.
TInt COomTranspWindow::SetTransparencyTesting()
	{
	TInt res;	
	TLogMessageText buf;
		
	// Need all nine transitions, from the 3 ways to the same 3 ways.
	// The ways are Factor, CWsBitmap, CFbsBitmap
	const TRgb KTransparencyFactor(128,128,128);
	res=iFirst->Win()->SetTransparencyFactor(KTransparencyFactor);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyFactor -1");
		TheClient->LogMessage(buf);
		return res;
		}
			
	res=iFirst->Win()->SetTransparencyBitmap(*iFbsBitmap);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyBitmap - fbs -2");
		TheClient->LogMessage(buf);
		return res;
		}			
		
	res=iFirst->Win()->SetTransparencyWsBitmap(*iWsBitmap);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyBitmap - Ws -3");
		TheClient->LogMessage(buf);
		return res;
		}
				
	res=iFirst->Win()->SetTransparencyFactor(KTransparencyFactor);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyBitmap - fbs -4");
		TheClient->LogMessage(buf);
		return res;
		}	
	res=iFirst->Win()->SetTransparencyWsBitmap(*iWsBitmap);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyBitmap - Ws -5");
		TheClient->LogMessage(buf);
		return res;
		}
	res=iFirst->Win()->SetTransparencyWsBitmap(*iWsBitmap);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyBitmap - Ws -6");
		TheClient->LogMessage(buf);
		return res;
		}			
		
	res=iFirst->Win()->SetTransparencyBitmap(*iFbsBitmap);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyBitmap - fbs -7");
		TheClient->LogMessage(buf);
		return res;
		}	
		
	res=iFirst->Win()->SetTransparencyBitmap(*iFbsBitmap);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyBitmap - fbs -8");
		TheClient->LogMessage(buf);
		return res;
		}
		
	res=iFirst->Win()->SetTransparencyFactor(KTransparencyFactor);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyFactor -9");
		TheClient->LogMessage(buf);
		return res;
		}				
		
	res=iFirst->Win()->SetTransparencyFactor(KTransparencyFactor);
	if (res!=KErrNone)
		{
		buf=_L("test failure!! with SetTransparencyFactor -10");
		TheClient->LogMessage(buf);
		return res;
		}
					
	return res;
	}	

void COomTranspWindow::ClearUpL()
	{
	delete iFirst;
	iFirst = NULL;
	delete iSecond;
	iSecond = NULL;
	delete iBackgroundWin;
	iBackgroundWin = NULL;
	}

//

CWindowWithOneLine* CWindowWithOneLine::NewL(CTWinBase& aParent, const TRect& aExtent)
	{
	CWindowWithOneLine* self = new(ELeave) CWindowWithOneLine;
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	const TSize screenSize(TheClient->iScreen->SizeInPixels());	
	self->SetExtL(aExtent.iTl, aExtent.Size());
	self->AssignGC(*TheClient->iGc);
	self->Activate();
	self->DrawNow();
	CleanupStack::Pop(self);
	return self;
	}

void CWindowWithOneLine::Draw()
	{
	iGc->DrawLine(TPoint(0,0), TPoint(Size().iWidth, Size().iHeight));
	}

//

COomObscuredWindow::COomObscuredWindow(CTOom *aTest) : COomSetup(aTest)
	{}

TOomTestName COomObscuredWindow::TestName()
	{
	return(_L("Obscured window"));
	}

TInt COomObscuredWindow::Fail()
	{
	TRAPD(err, DoFailL());
	return err;
	}

	
void COomObscuredWindow::DoFailL()
	{
#ifdef __WINS__
	RDebug::Print(_L("COomObscuredWindow::DoFailL - enter"));
#endif
	const TRect extent(20, 20, 100, 100);
	CBlankWindow* backgroundWindow = new(ELeave) CBlankWindow(TRgb(12, 23, 34));
	CleanupStack::PushL(backgroundWindow);
	TDisplayMode mode = EColor16MAP;
	backgroundWindow->SetUpL(extent.iTl, extent.Size(), TheClient->iGroup, *TheClient->iGc, &mode);
	
	CWindowWithOneLine* window = CWindowWithOneLine::NewL(*backgroundWindow, extent);
	CleanupStack::PushL(window);
#ifdef __WINS__
	RDebug::Print(_L("COomObscuredWindow - WaitForRedrawsToFinish - 1"));
#endif
	TheClient->WaitForRedrawsToFinish();
	
	CBlankWindow* obscuringWindow = new(ELeave) CBlankWindow(TRgb(255, 0, 0));
	CleanupStack::PushL(obscuringWindow);
	obscuringWindow->SetUpL(extent.iTl, extent.Size(), backgroundWindow, *TheClient->iGc, &mode);

	TheClient->Flush();
#ifdef __WINS__
    RDebug::Print(_L("COomObscuredWindow - WaitForRedrawsToFinish - 2"));
#endif
	TheClient->WaitForRedrawsToFinish();

	window->Invalidate();

	for(TInt i = 0; i < 50; i++)
		{
		RWindow anotherWin(iWs);
		User::LeaveIfError(anotherWin.Construct(iWinGroup, 11));
		anotherWin.Close();
		
		if(i % 4)
			window->Invalidate();

		TheClient->Flush();
#ifdef __WINS__
	    RDebug::Print(_L("COomObscuredWindow - WaitForRedrawsToFinish - 3 - %d"), i);
#endif
	    TheClient->WaitForRedrawsToFinish();
		}

	TheClient->Flush();
#ifdef __WINS__
    RDebug::Print(_L("COomObscuredWindow - WaitForRedrawsToFinish - 4"));
#endif
	TheClient->WaitForRedrawsToFinish();

	CleanupStack::PopAndDestroy(obscuringWindow);
	CleanupStack::PopAndDestroy(window);
	CleanupStack::PopAndDestroy(backgroundWindow);
#ifdef __WINS__
    RDebug::Print(_L("COomObscuredWindow::DoFailL - exit"));
#endif
	}

void COomObscuredWindow::ClearUpL()
	{
	}	

//

CTOom::CTOom(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	iState = 0;
	}
	
const TDesC& CTOom::TestName() const
	{
	_LIT(KTestName,"CTOom");
	return(KTestName());
	}
	
void CTOom::DoOomTestL(COomFailBase *aOomTest)
	{
	//TLogMessageText buf;
	TEST(aOomTest!=NULL);
	if (aOomTest==NULL)
		INFO_PRINTF1(_L("aOomTest - Expected: Not Null, Actual: NULL"));			

	TRAPD(ret1,aOomTest->ConstructL());
	TEST(ret1==KErrNone);
	if (ret1!=KErrNone)
		INFO_PRINTF3(_L("aOomTest->ConstructL() return value - Expected: %d, Actual: %d"), KErrNone, ret1);			

	aOomTest->Flush();
	__UHEAP_MARK;
//	TInt oldCount=TheClient->iWs.HeapCount();
	for(TInt mode=0;mode<3;mode++)
		{
		/*buf.Format(_L("OOMTest[%d] Mode=%d"),iState,mode),
		TheClient->LogMessage(buf);*/
		TInt successCount=0;
		for(TInt count=1;;count++)
			{
			TInt ret;
			aOomTest->PreFail();
			if (mode==0)
				TheClient->iWs.HeapSetFail(RHeap::EDeterministic,count);
			else if (mode==1)
				TheClient->iWs.HeapSetBurstFail(RHeap::EBurstFailNext, count, KMaxTUint16);
			else if (mode==3)
				{
				__UHEAP_SETFAIL(RHeap::EDeterministic,count);	//Leavescan will complain about EFailNext, although it isn't a leaving function
				}
			ret=aOomTest->Fail();
			if (mode==0)
				TheClient->iWs.HeapSetFail(RHeap::ENone,0);
			else if (mode==1)
				TheClient->iWs.HeapSetBurstFail(RHeap::ENone, 0, 0);
			else if (mode==2)
				{
				__UHEAP_RESET;
				}
			aOomTest->ClearUpL();  //In just 1 case this could leave...
			aOomTest->Flush();
			if (ret==KErrNone)
				{
				if (successCount==10)
					break;
				successCount++;
				}
			else
				{
				/*if (successCount>0)
					{
					buf.Format(_L("[%d,%d] Count=%d, MaxSuccess=%d"),iState,mode,count,successCount);
					TheClient->LogMessage(buf);
					}*/
				successCount=0;
				/*if (ret!=KErrNoMemory)
					{
					buf.Format(_L("[%d,%d] Fail, Count=%d, Error=%d"),iState,mode,count,ret);
					TheClient->LogMessage(buf);
					}*/
				TEST(ret==KErrNoMemory);
				if (ret!=KErrNoMemory)
					INFO_PRINTF3(_L("aOomTest->Fail() return value - Expected: %d, Actual: %d"), KErrNoMemory, ret);			

				}
			}
		/*buf.Format(_L("[%d,%d] LastCount=%d"),iState,mode,count),
		TheClient->LogMessage(buf);*/
		}
//	TEST(oldCount>=TheClient->iWs.HeapCount());
	__UHEAP_MARKEND;
	delete aOomTest;
	/*buf.Format(_L("OOMTest[%d] Finished"),iState);
	TheClient->LogMessage(buf);*/
	}

void CTOom::ConstructL()
	{
	iShieldWin=RBlankWindow(TheClient->iWs);
	User::LeaveIfError(iShieldWin.Construct(*(TheClient->iGroup->GroupWin()),1));
//	iShieldWin.SetOrdinalPosition(0,-1);
	iShieldWin.Activate();
	}

CTOom::~CTOom()
	{
	iShieldWin.Close();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0124

@SYMDEF             DEF081259

@SYMTestCaseDesc    Out of memory tests

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Uses deterministic heap failure mode to test WSERV classes against out-of-memory errors.
					Sets and unsets deterministic mode using both RWsSession.HeapSetFail() methods and
					__UHEAP_SETFAIL/__UHEAP_RESET macros (doing this both client and WSERV threads are tested). 
					On each step the test creates a failure object and repeatedly asks it to do some work (calling Fail() method).
					A failure object is an object which encapsulates some allocation functionality inside it's Fail() method.
					The test is performed for 23 different failure objects that do the following:
						1. Creates a wserv session, connects and creates CWsScreenDevice object.
						2. Creates a RWindowGroup object.
						3. Creates a RWindow object.
						4. Creates a RBackedUpWindow object.
						5. Sets the size of previously created backed-up window.
						6. Creates a RBlankWindow object.
						7. Allocates a buffer for storing pointer movements for a previously created backed-up window.
						8. Adds a priority key for a previously created window group.
						9. Requests a capture keys for a previously created window group.
						10. Requests the capture of key-up and key-down events for a previously created window group.
						11. Sets hot key for the session.
						12. Sets the window group's name.
						13. Sends a message to another window group.
						14. Fetches a message.
						15. Enables window group change events and modifier change events.
						16. Copies screen to bitmap.
						17. Creates a sprite.
						18. Creates a pointer cursor.
						19. Creates a graphical context for a previously created screen device.
						20. Draws a polygon.
						21. Creates a screen device handle.
						22. Adds a custom text cursor to the server's list of cursors.
						23. Does nothing if transparency is not enabled. Creates a blank window with 
							two transparent child-windows. Tests setting the transparency via a factor, CFbsBitmap
							or a CWsBitmap, and changing from one to the other.
							Changes child-windows' sizes and toggles their visibility.
					

@SYMTestExpectedResults The test checks that the creation failure objects doesn't fail and their's work either causes no errors or causes KErrNoMemory error.
*/
void CTOom::RunTestCaseL(TInt /*aCurTestCase*/) 
	{
	((CTOomStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0124"));
	if (iState==0)
		{
		// commented by Anton Golovko, 01.03.2006 while converting to TEF
		// the code below prevented the test to do anything.
//		if (iTest->IsFullRomL()) 
//			{
//			TestComplete();
//			return;
//			}
		iOldCount=TheClient->iWs.HeapCount();
		}
	else if (iState==sizeof(CreateOomFailTest)/sizeof(CreateOomFailTest[0]))
		{
		iTest->CloseAllPanicWindows();
		TInt heapCount=TheClient->iWs.HeapCount();
		if (heapCount>iOldCount)
			{
			TEST(iOldCount>=heapCount-184);		//For some uninvestigated reason 184 object get allocated on the server side, partly because of INFO_PRINTF1
			if (iOldCount<heapCount-184)
					INFO_PRINTF3(_L("iOldCount>=heapCount-174 - Expected: %d, Actual: %d"), heapCount-184, iOldCount);			

			}
		((CTOomStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		return;
		}
	DoOomTestL(CreateOomFailTest[iState++](this));
	((CTOomStep*)iStep)->RecordTestResultL();
	}
	
	
	
	
__WS_CONSTRUCT_STEP__(Oom)
	

// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// used for timing graphics
// 
//

#include "TTIME.H"

#define TEST_ROM_BITMAP_NAME _L("Z:\\WSTEST\\TROM.MBM")

enum TFuncType
	{
	ESimpleFlush,
	ESpriteSetting,
	EBitmapDevice,
	ETrivialFunctions,
	ELoadBitmap,
	};

class TWsTest : public CBase
	{
public:
	void DoTestL(TInt aOwningGroup, TFuncType aFunc, TInt aParam1, TAny *aParam2);
	void SimpleFlush(TInt aParam1, TAny *aParam2);
	void SpriteSettingL(TInt aParam1, TAny *aParam2);
	void BitmapDeviceL(TInt aParam, TAny *);
	void TrivialFunctions(TInt aParam1, TAny *aParam2);
	void LoadBitmapL(TInt aParam1, TAny *aParam2);
private:
	void createSpriteBitmapL(CFbsBitmap *aBitmap, CFbsBitmapDevice *&aBitmapDevice, const TSize &aSize, TBool aDoMask);
private:
	RWsSession iWs;
	RWindowGroup iGroup;
	RWindow iWindow;
	CWsScreenDevice *iDevice;
	CWindowGc *iGc;
	};
	
TInt CreateWsTest(TInt aOwningGroup, TFuncType aFunc, TInt aParam1, TAny *aParam2)
	{
	TWsTest *iTest=NULL;
	TRAPD(err,iTest=new(ELeave) TWsTest());
	if (err==KErrNone)
		{
		TRAP(err,iTest->DoTestL(aOwningGroup, aFunc, aParam1, aParam2));
		delete iTest;
		}
	return(err);
	}

void TWsTest::DoTestL(TInt aOwningGroup, TFuncType aFunc, TInt aParam1, TAny *aParam2)
	{
	iWs.Connect();
	iDevice=new(ELeave) CWsScreenDevice(iWs);
	iDevice->Construct();
	iGroup=RWindowGroup(iWs);
	iGroup.Construct(ENullWsHandle);
	iGroup.SetOwningWindowGroup(aOwningGroup);
//
	iWindow=RWindow(iWs);
	iWindow.Construct(iGroup,ENullWsHandle);
	iWindow.SetExtent(TPoint(), iDevice->SizeInPixels());
	iWindow.Activate();
//
	iDevice->CreateContext(iGc);
	iWindow.BeginRedraw();
	iGc->Activate(iWindow);
	iGc->Clear();
	iWindow.EndRedraw();
//
	switch(aFunc)
		{
		case ESimpleFlush:
			SimpleFlush(aParam1, aParam2);
			break;
		case ESpriteSetting:
			SpriteSettingL(aParam1, aParam2);
			break;
		case EBitmapDevice:
			BitmapDeviceL(aParam1, aParam2);
			break;
		case ETrivialFunctions:
			TrivialFunctions(aParam1, aParam2);
			break;
		case ELoadBitmap:
			LoadBitmapL(aParam1, aParam2);
			break;
		default:;
		}
	delete iGc;
	iWindow.Close();
	iGroup.Close();
	delete iDevice;
	iWs.Close();
	}

// Flush //

void TWsTest::SimpleFlush(TInt aParam, TAny *)
	{
	TTimeIntervalMicroSeconds32 interval(100000);
	TInt distance=10;
	if (aParam==0)
		{
		TTimeIntervalMicroSeconds32 origInterval;
		TInt origDistance;
		iWs.GetDoubleClickSettings(origInterval,origDistance);
		for(TInt nTimes=0;nTimes<5000-1;nTimes++)
			{
			iWs.SetDoubleClick(interval,distance);
			iWs.Flush();
			}
		iWs.SetDoubleClick(origInterval,origDistance);
		}
	else
		{
		for(TInt nTimes=0;nTimes<5000;nTimes++)
			iWs.GetDoubleClickSettings(interval,distance);
		}
	}

TInt SimpleFlushTestFunc(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ESimpleFlush, 0, NULL));
	}

TInt SimpleFlushTestFunc2(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ESimpleFlush, 1, NULL));
	}

GLDEF_D TTimeTestHeader SimpleFlushTest={_S("Simple Flush[1] x5000"),SimpleFlushTestFunc};
GLDEF_D TTimeTestHeader SimpleFlushTest2={_S("Simple Flush[2] x5000"),SimpleFlushTestFunc2};

// IP Read/Write //

#if 0
TInt ReadWriteThreadFunc(TAny *aParams)
	{
	TWinCommand command;
	RProcess().CommandLine(command);
	TDesC8 *cmd=&command;
	RThread thread;
	thread.Open(_L("TimeTest"));
	TAny *remotePtr=*(TAny **)aCmd->Ptr();
	TBuf<0x10> buf;
	for(TInt count=0;count<100000;count++)
		thread.ReadL(remotePtr,buf,0);
	}

void TWsTest::IPReadWriteL(TInt aParam, TAny *)
	{
	TBuf<0x10> srcData;
	srcData.Append(_L("1234567890ABCDEF"));
	RProcess process;
	TWinCommand command;
	*((TAny **)command.Ptr())=&srcData;
	User::LeaveIfError(process.Create(_L("TimeThread1"),);
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	process.Close();
	}

TInt SimpleFlushTestFunc(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ESimpleFlush, 0, NULL));
	}

TInt SimpleFlushTestFunc2(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ESimpleFlush, 1, NULL));
	}

GLDEF_D TTimeTestHeader SimpleFlushTest={_S("Simple Flush[1] x5000"),SimpleFlushTestFunc};
GLDEF_D TTimeTestHeader SimpleFlushTest2={_S("Simple Flush[2] x5000"),SimpleFlushTestFunc2};
#endif
// Bitmap device //

void TWsTest::BitmapDeviceL(TInt aParam, TAny *)
	{
	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(bitmap->Create(TSize(10,10),EGray4));
	CFbsBitmapDevice *bitmapDevicePerm=NULL;
	if (aParam==1)
		bitmapDevicePerm=CFbsBitmapDevice::NewL(bitmap);
	for(TInt nTimes=0;nTimes<100;nTimes++)
		{
		CFbsBitmapDevice *bitmapDevice=CFbsBitmapDevice::NewL(bitmap);
		delete bitmapDevice;
		}
	delete bitmapDevicePerm;
	delete bitmap;
	}

TInt BitmapDeviceTestFunc1(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, EBitmapDevice, 0, NULL));
	}

TInt BitmapDeviceTestFunc2(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, EBitmapDevice, 1, NULL));
	}

GLDEF_D TTimeTestHeader BitmapDeviceTest1={_S("Bitmap Device (reload)"),BitmapDeviceTestFunc1};
GLDEF_D TTimeTestHeader BitmapDeviceTest2={_S("Bitmap Device "),BitmapDeviceTestFunc2};

// Sprite Setting //

void TWsTest::createSpriteBitmapL(CFbsBitmap *aBitmap, CFbsBitmapDevice *&aBitmapDevice, const TSize &aSize, TBool aDoMask)
	{
	User::LeaveIfError(aBitmap->Create(aSize,EGray4));
	aBitmapDevice=CFbsBitmapDevice::NewL(aBitmap);
	CFbsBitGc *gc=CFbsBitGc::NewL();
	gc->Activate(aBitmapDevice);
	gc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 2));
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetPenStyle(CGraphicsContext::ENullPen);
	gc->DrawRect(TRect(aSize));
	gc->SetPenStyle(CGraphicsContext::ESolidPen);
	gc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : 0));
	gc->SetBrushColor(TRgb::Gray4(aDoMask ? 3 : 1));
	gc->DrawEllipse(TRect(aSize));
	delete gc;
	}

void TWsTest::SpriteSettingL(TInt , TAny *)
	{
	RWsSprite sprite;
	TSize size(32,32);
	sprite=RWsSprite(iWs);
	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap();
	CFbsBitmap *mask=new(ELeave) CFbsBitmap();
	CFbsBitmapDevice *bitmapDevice=NULL;		//To stop warning
	TRAPD(err,createSpriteBitmapL(bitmap,bitmapDevice,size,EFalse));
	delete bitmapDevice;
	TRAP(err,createSpriteBitmapL(mask,bitmapDevice,size,ETrue));
	delete bitmapDevice;
	TSpriteMember spriteData;
	spriteData.iBitmap=bitmap;
	spriteData.iMaskBitmap=mask;
	spriteData.iInvertMask=EFalse;
	spriteData.iInterval=TTimeIntervalMicroSeconds32(0);
	User::LeaveIfError(sprite.Construct(iWindow,TPoint(0,0),0));
	User::LeaveIfError(sprite.AppendMember(spriteData));
	User::LeaveIfError(sprite.Activate());
	for(TInt i=0;i<500;i++)
		sprite.SetPosition(TPoint(i&0x7f,i&0x7f));
	sprite.Close();
	delete mask;
	delete bitmap;
	}

TInt SpriteSettingTestFunc(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ESpriteSetting, 0, NULL));
	}

GLDEF_D TTimeTestHeader SpriteTest={_S("Sprite Setting"),SpriteSettingTestFunc};

void TWsTest::TrivialFunctions(TInt , TAny *)
	{
	for(TInt i=0;i<100000;i++)
		iWs.FreeSystemPointerCursorList();
	}

TInt TrivialFunctionsTestFunc(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ETrivialFunctions, 0, NULL));
	}

GLDEF_D TTimeTestHeader TrivialFunctionsTest={_S("TrivialFunctions"),TrivialFunctionsTestFunc};

void TWsTest::LoadBitmapL(TInt aMode, TAny *)
	{
	if (aMode<2)
		{
		for(TInt count=0;count<10;count++)
			{
			if (aMode==0)
				{
				CFbsBitmap *bit=new(ELeave) CFbsBitmap();
				User::LeaveIfError(bit->Load(TEST_ROM_BITMAP_NAME,0));
				delete bit;
				}
			else
				{
				CWsBitmap *bit=new(ELeave) CWsBitmap(iWs);
				User::LeaveIfError(bit->Load(TEST_ROM_BITMAP_NAME,0));
				delete bit;
				}
			}
		}
	else for(TInt count=0;count<100;count++)
		{
	//__PROFILE_START(1)
		RFs fs;
		User::LeaveIfError(fs.Connect());
		fs.SetNotifyUser(EFalse);
	//__PROFILE_END(1)
	//__PROFILE_START(2)
		TParse parse;
		User::LeaveIfError(fs.Parse(TEST_ROM_BITMAP_NAME,parse));
	//__PROFILE_END(2)
	//__PROFILE_START(3)
		TInt drive;
		User::LeaveIfError(RFs::CharToDrive(parse.Drive()[0],drive));
		TDriveInfo driveinfo;
		User::LeaveIfError(fs.Drive(driveinfo,drive));
	//__PROFILE_END(3)
	//__PROFILE_START(4)
		RFile tempfile;
		User::LeaveIfError(tempfile.Open(fs,TEST_ROM_BITMAP_NAME,EFileShareAny));
		TInt aAddress;
		tempfile.Seek(ESeekAddress,aAddress);
	//__PROFILE_END(4)
	//__PROFILE_START(5)
		tempfile.Close();
		fs.Close();
	//__PROFILE_END(5)
		}
	}

TInt LoadBitmapTestFunc(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ELoadBitmap, 0, NULL));
	}
TInt LoadWsBitmapTestFunc(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ELoadBitmap, 1, NULL));
	}
TInt LoadRomFileTestFunc(TInt aOwningGroup)
	{
	return(CreateWsTest(aOwningGroup, ELoadBitmap, 3, NULL));
	}

GLDEF_D TTimeTestHeader BitmapLoadTest={_S("Load Bitmap"),LoadBitmapTestFunc};
GLDEF_D TTimeTestHeader WsBitmapLoadTest={_S("Load WsBitmap"),LoadWsBitmapTestFunc};
GLDEF_D TTimeTestHeader RomFileTest={_S("Rom File"),LoadRomFileTestFunc};

// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32hal.h>
#include <e32std.h>
#include <e32test.h>
#include <f32file.h>
#include <bitstd.h>
#include <bitdev.h>
#include <hal.h>
#include "tbmp.h"
#include "TClip.h"
#include <graphics/fbsdefs.h>

//TClip test app runs only uncompressed bitmaps tests - part of original TClip test app.
//The change was made because the original TClip test app took too much time (over 0.5 hour)
//and usually was terminated with a TIMEOUT on LUBBOCK device.
// main function
TInt E32Main()
	{
	FbsStartup();
	__UHEAP_MARK;
	RFbsSession::Connect();
	CTrapCleanup* tc=CTrapCleanup::New();

	CFbsScreenDevice* dev=NULL;
	CFbsBitGc* con=NULL;
	CFbsFont* font=NULL;
	CFbsFont* largefont=NULL;
	::CreateTestEnvironment(dev, con, font, largefont);

	RTest test(_L("TCLIP"));
	test.Title();
	test.Start(_L("Uncompressed"));

	// Uncompressed
	CFbsBitmap* bmp=new CFbsBitmap;
	if(bmp==NULL)
		User::Panic(_L("Bitmap not created"),KErrGeneral);
	TInt ret=bmp->Load(_L("z:\\system\\data\\tbmp.mbm"),EMbmTbmpTbmp,EFalse);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not loaded"),ret);
	
	TestClip* clip=new TestClip(0, &test, dev, con, bmp, font, largefont);
	clip->Construct();
	clip->TestRgn();
	delete bmp;
	delete clip;
	
	test.End();
	test.Close();

	::DestroyTestEnvironment(dev, con, font, largefont);

	delete tc;
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	return 0;
	}


// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32msgqueue.h>
#include <sgresource/sgimage.h>
#include "tgraphicsresourcemultiprocessthread.h"

// The second process waits for image IDs sent by the main test process
// and opens and closes RSgImage handles to the corresponding images.
// Image IDs are received through a message queue and synchronisation
// with the main process is achieved by means of rendezvous objects.
// A null ID indicates the end of the test.
void MainL()
	{
	RSgDriver driver;
	User::LeaveIfError(driver.Open());
	CleanupClosePushL(driver);
	RMsgQueue<TSgDrawableId> msgQ;
	User::LeaveIfError(msgQ.OpenGlobal(KSgTestMultiprocessMsgQ));
	CleanupClosePushL(msgQ);
	for (;;)
		{
		TSgDrawableId id;
		msgQ.ReceiveBlocking(id);
		if (id == KSgNullDrawableId)
			{
			break;
			}
		RSgImage image;
		TInt err = image.Open(id);
		image.Close();
		RProcess::Rendezvous(err);
		}
	CleanupStack::PopAndDestroy(2);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(ret, MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return ret;
	}

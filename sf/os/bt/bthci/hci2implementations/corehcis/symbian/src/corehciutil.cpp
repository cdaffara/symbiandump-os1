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
//

#include <e32base.h>
#include <bluetooth/hci/hciipc.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCISERVER);
#endif

void PanicClient(const RMessage2& aMessage,TInt aPanic)
/**
   Panic the client and complete the message.
   RMessage2::Panic() also completes the message. This is:
   (a) important for efficient cleanup within the kernel
   (b) a problem if the message is completed a second time
**/
	{
	LOG1(_L("\tPanicClient: Reason = %d\n"), aPanic);
	aMessage.Panic(KHCIServerClientPanicCat, aPanic);
	}

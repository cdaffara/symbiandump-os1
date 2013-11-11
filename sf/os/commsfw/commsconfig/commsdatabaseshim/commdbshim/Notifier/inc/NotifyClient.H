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

/**
 @file
 @internalComponent
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef NOTIFYCL_H
#define NOTIFYCL_H

#include <e32std.h>


_LIT(KCommsdatNotifierExecutable,"ShimNotifierSvr");
/** Shim Notifier 
* 
* @internalComponent */
class RCommsdatNotifier : public RSessionBase   
	{
public:
	IMPORT_C RCommsdatNotifier();
	IMPORT_C TVersion Version() const;
	
	IMPORT_C TInt Connect();
	IMPORT_C void RequestNotification(TRequestStatus& aStatus);
	IMPORT_C void CancelNotification();
	IMPORT_C void Close();
private:	
	TInt StartNotifierProcess();   	
	};

#endif // __NOTIFYCL_H__


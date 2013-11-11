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

#ifndef __TDIRECTGDIEGLCONTENT_CLIENT_H__
#define __TDIRECTGDIEGLCONTENT_CLIENT_H__

#include <e32std.h>

class TSgDrawableId;

/**
This class contains client side of egl content server session.
*/
class REglContentSession : public RSessionBase
	{
public:
	TInt Connect();
	void Close();
	TInt GetSyncImage(TSgDrawableId& aImageId);
	TInt GetAsyncImage(TSgDrawableId& aImageId, TInt& aFrameNumber);
	TInt GetAsyncImageDebug(TSgDrawableId& aImageId, TInt& aFrameNumber);
private:
	TInt StartServer();
private:
	RProcess iServer;
	};

#endif

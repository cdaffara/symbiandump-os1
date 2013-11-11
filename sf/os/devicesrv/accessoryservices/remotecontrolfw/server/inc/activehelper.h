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
*/

#ifndef REMCON_ACTIVEHELPER_H
#define REMCON_ACTIVEHELPER_H

#include <e32def.h>
#include <e32base.h>

class CRemConControllerSession;

/**
Active Helper.
Helps with the session class's pending async connect/disconnect requests.
*/
NONSHARABLE_CLASS(CActiveHelper) : public CActive
	{
public:
	CActiveHelper(CRemConControllerSession &aSession);
	~CActiveHelper();
	void Complete();
private:
//from CActive
	void RunL();
	void DoCancel();
private:
	CRemConControllerSession& iSession;
	};


#endif //REMCON_ACTIVEHELPER_H

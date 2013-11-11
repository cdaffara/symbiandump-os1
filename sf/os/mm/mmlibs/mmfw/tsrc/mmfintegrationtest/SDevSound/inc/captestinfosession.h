
// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file captestinfosession.h
*/

#ifndef CAPTESTINFOSESSION_H__
#define CAPTESTINFOSESSION_H__

#include <e32base.h>

/**
 * Connection to Cap Test Info server
 *
 * @class RCapTestInfoSession
 *
 */

class RCapTestInfoSession : public RSessionBase
	{
public:
	RCapTestInfoSession();
	TInt Connect();
	void Close();
	
	TThreadId InfoServerTidL();
	};


#endif
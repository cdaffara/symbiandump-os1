// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file atunsolicitedobserver.h
// 

#ifndef ATUNSOLICITEDOBSERVER_H_
#define ATUNSOLICITEDOBSERVER_H_

//system include
#include <e32std.h>
//user include

class MLtsyUnsolicitedCommandObserver
	{
public:
	/**
	 * 
	 * 
	 */
	virtual void UnsolicitedCommandArriving(TInt aEvent, const TDesC8& aWindPkg) = 0;
	};

#endif /*ATUNSOLICITEDOBSERVER_H_*/

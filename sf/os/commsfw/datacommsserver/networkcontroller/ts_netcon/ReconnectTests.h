// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __RECONNECTTESTS_H__
#define __RECONNECTTESTS_H__

#include "TestBases.h"

//
//  Test case NC-3-1
//
class CNetConTest0301 : public CNetConReconnectRequestTestBase
{
public:
	static CNetConTest0301* NewLC();

	const TDesC& Name() const;

private:
	CNetConTest0301();
};


//
//  Test case NC-3-2
//
class CNetConTest0302 : public CNetConReconnectRequestTestBase
{
public:
	static CNetConTest0302* NewLC();

	const TDesC& Name() const;

private:
	CNetConTest0302();
};



//
//  Test case NC-3-3
//
class CNetConTest0303 : public CNetConReconnectRequestTestBase
{
public:
	static CNetConTest0303* NewLC();

	const TDesC& Name() const;

private:
	CNetConTest0303();
};


//
//  Test case NC-3-4
//
class CNetConTest0304 : public CNetConReconnectRequestTestBase
{
public:
	static CNetConTest0304* NewLC();

	const TDesC& Name() const;

private:
	CNetConTest0304();
};


#endif // __RECONNECTTESTS_H__


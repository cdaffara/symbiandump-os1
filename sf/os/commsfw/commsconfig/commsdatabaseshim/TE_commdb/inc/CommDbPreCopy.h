/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/*
@file
@internalComponent
This Copies cdbv3.dat to one temp dat file
*/

#ifndef __CCOMMDB_PRECOPY_H
#define __CCOMMDB_PRECOPY_H

_LIT(KTempDb3, "c:\\system\\data\\default3.dat");

///////////////////////////////////////////////
// Test Copy Test
NONSHARABLE_CLASS (CCommDbPreCopy) : public CTestStepCommDb
{
public:
	CCommDbPreCopy();
	virtual ~CCommDbPreCopy();

	virtual enum TVerdict doTestStepL( void );

private:
};

#endif // __CCOMMDB_PRECOPY_H

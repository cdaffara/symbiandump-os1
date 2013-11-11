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
// ctsyphonebookonfunegative.h
//

/**
 @file
 @internalAll 
*/

#ifndef CTSYPHONEBOOKONFUNEGATIVE_H_
#define CTSYPHONEBOOKONFUNEGATIVE_H_

#include "cctsyphonebookfu.h"

class CCTsyPhonebookOnFUNegative : public CCTsyPhonebookFU
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	void TestGetInfoIpcL();
	void TestWriteIpcL();
	void TestReadIpcL();
	void TestDeleteEntryIpcL();
	void TestWriteEntryIpcL();
	void TestDeleteAllIpcL();
	void TestReadEntryIpcL();
	void TestReadSizeIpcL();
	void TestWriteSizeIpcL();
    virtual void DoCleanup();
	
private:
	void OpenAndPushEtelAndPhoneONStoreL(RMobileONStore& aONStore);
	
private:
	RMobilePhoneBookStore iAdnPhoneBookStore;
	}; // class CCTsyPhonebookOnFUNegative


#endif /* CTSYPHONEBOOKONFUNEGATIVE_H_ */

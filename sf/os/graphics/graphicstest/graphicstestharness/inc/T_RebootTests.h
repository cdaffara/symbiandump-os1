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
//
#ifndef T_REBOOTTESTS_H_
#define T_REBOOTTESTS_H_

/**
@test
@internalComponent
This class provides all the helper functions required in order to use
the rebooting test harness. This harness is utilised on hardware builds
to perform a full reboot after each test step whilst keeping track of 
which tests should be run at this boot.
*/
class CTRebootTestHelper : public CBase
	{
public:
	static CTRebootTestHelper* NewLC();
	void BuildCommandScriptL(const TDesC& aFileIn, const TDesC& aFileOut, TInt aTestCase);
	TInt8 FindLastTestL(const TDesC& aFile);
	void WriteTestOutL(const TDesC& aFile, TInt8 aValue);
	void WriteDummyLogFileL(const TDesC& aScriptName, const TDesC& aLogFile);
private:
	CTRebootTestHelper();
	~CTRebootTestHelper();
	void ConstructL();

public:
	RFs iFs;
	};

#endif /*T_REBOOTTESTS_H_*/

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

#ifndef CONTROLLERINITIALISATIONCMNDPARSER_H
#define CONTROLLERINITIALISATIONCMNDPARSER_H

#include <f32file.h>

class CVendorDebugCommand;

NONSHARABLE_CLASS(CCommandParser) : public CBase
	{
public:
	static CCommandParser* NewL(const TDesC& aCommandFile);
	TInt GetNextVendorCommand(CVendorDebugCommand& aVendorCommand);

	~CCommandParser();

private:
	CCommandParser();
	void ConstructL(const TDesC& aCommandFile);
	
private:
	static const TUint8 KScriptElementHeaderLength = 4;
	static const TUint8 KScriptLengthOffset = 2;
	static const TUint8 KScriptHeaderLength = 32;

	static const TUint8 KScriptHCIHeaderLength = 4;
	static const TUint8 KScriptHCIHeaderOpcodeOffset = 1;
	static const TUint8 KScriptHCICmdHdrLengthOffset = 3;

	static const TUint8 KSendCommand = 1;

	RFile iFile;
	RFs iFs;
	};

#endif // CONTROLLERINITIALISATIONCMNDPARSER_H

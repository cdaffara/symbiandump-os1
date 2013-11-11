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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <e32base.h>
#include <f32file.h>
#include <apgcli.h>
#include <ecom/ecom.h>

#define KConfCertain		CApaDataRecognizerType::ECertain
#define KConfProbable		CApaDataRecognizerType::EProbable
#define KConfPossible		CApaDataRecognizerType::EPossible
#define KConfNotRecognised	CApaDataRecognizerType::ENotRecognized


/*
**
*/
typedef struct
	{
	TInt iTestNumber;					// The test case number as specified in testspec.
	const TText* iFileName;				// The file to recognise.
	const TText8* iExpectedMimeType;	// The expected MIME-type.
	}
TTestSpec;


/*
** This utility class allows connection to the AppArc framework.
*/
class RAppArc
	{
public:
	TInt Connect();
	void Close();
	TInt Recognise(RFile& aFile, TDataType& aDataType);
	TInt Recognise(RFile& aFile, TDataRecognitionResult& aResult);
	RFs& FileServer();
		
protected:
	RApaLsSession iAppArc;
	RFs iFs;
	};


#endif

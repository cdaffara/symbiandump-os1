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

#include "definitions.h"

/*
** Connect to the framework.
*/
TInt RAppArc::Connect()
	{
	TInt err;
	
	do
		{
		err = iAppArc.Connect();
		if (err != KErrNone)
			{
			break;
			}

		err = iFs.Connect();
		if (err != KErrNone)
			{
			break;
			}

		err = iFs.ShareProtected();
		if (err != KErrNone)
			{
			break;
			}
			
		return KErrNone;
		}
	while (EFalse);
		
	// If we get here then an error has occurred.
	Close();
	return err;
	}

/*
**
*/
void RAppArc::Close()
	{
	iFs.Close();
	iAppArc.Close();
	}

/*
**
*/
TInt RAppArc::Recognise(RFile& aFile, TDataType& aDataType)
	{
	TUid uid;

	uid.iUid = 0;
	return iAppArc.AppForDocument(aFile, uid, aDataType);
	}

/*
**
*/
TInt RAppArc::Recognise(RFile& aFile, TDataRecognitionResult& aResult)
	{
	aResult.Reset();
	return iAppArc.RecognizeData(aFile, aResult);
	}

/*
**
*/
RFs& RAppArc::FileServer()
	{
	return iFs;
	}



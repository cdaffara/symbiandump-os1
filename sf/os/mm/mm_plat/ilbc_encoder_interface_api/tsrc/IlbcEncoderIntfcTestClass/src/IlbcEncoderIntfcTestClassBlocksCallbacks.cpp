/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: ilbc encoder test
*
*/



// INCLUDE FILES
#include "IlbcEncoderIntfcTestClass.h"
#include "debug.h"





/************************************************************************************************************/

//Devsound initialization is completed.Configure devsound capabilities.
void CIlbcEncoderIntfcTestClass::InitializeComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		FTRACE(FPrint(_L("InitializeComplete")));
		iLog->Log(_L("InitializeComplete, return code =%d!!!"),aError);
		}
	else
		{
		FTRACE(FPrint(_L("InitializeComplete FAILED %d"),aError));
		iLog->Log(_L("InitializeComplete Error!, return code =%d!!!"),aError);
		}
    ProcessEvent(EInitializeComplete, aError);
	}


void CIlbcEncoderIntfcTestClass::ToneFinished(TInt /*aError*/)
   	{
 //	Panic(KErrNotSupported);
 	}

//This is called when an empty audio buffer is available.Fill the buffer with audio data.
void CIlbcEncoderIntfcTestClass::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{

	}

//This is called when an audio play completion is successfully played or otherwise
void CIlbcEncoderIntfcTestClass::PlayError(TInt /* aError*/)
 	{
//	SendErrorToClient(aError);
	}


//Will panic if called.Should not be called during playing
void CIlbcEncoderIntfcTestClass::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CIlbcEncoderIntfcTestClass::RecordError(TInt /*aError*/)
	 {
  //   Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CIlbcEncoderIntfcTestClass::ConvertError(TInt /*aError*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CIlbcEncoderIntfcTestClass::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	 {
//	 Panic(KErrNotSupported);
	 }

// Audio Input Stream
void CIlbcEncoderIntfcTestClass::MaiscOpenComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
	    FTRACE(FPrint(_L("MaiscOpenComplete !!!")));
	    iLog->Log(_L("MaiscOpenComplete, return code =%d!!!"));
		}
	else
		{
	    FTRACE(FPrint(_L("MaiscOpenComplete FAIL =%d!!!"),aError));
	    iLog->Log(_L("EMaiscOpenComplete, return code =%d!!!"),aError);
		}
    ProcessEvent(EMaiscOpenComplete, aError);
	}

void CIlbcEncoderIntfcTestClass::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{

	}

void CIlbcEncoderIntfcTestClass::MaiscRecordComplete(TInt aError)
	{

	}

//End of File


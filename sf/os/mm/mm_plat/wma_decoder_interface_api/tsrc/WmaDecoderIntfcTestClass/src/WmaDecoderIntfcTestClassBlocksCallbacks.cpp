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
* Description: WmaDecoderIntfcTest
*
*/



// INCLUDE FILES
#include "WmaDecoderIntfcTestClass.h"
#include "debug.h"





/************************************************************************************************************/

//Devsound initialization is completed.Configure devsound capabilities.
void CWmaDecoderIntfcTestClass::InitializeComplete(TInt aError)
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


void CWmaDecoderIntfcTestClass::ToneFinished(TInt /*aError*/)
   	{
 //	Panic(KErrNotSupported);
 	}

//This is called when an empty audio buffer is available.Fill the buffer with audio data.
void CWmaDecoderIntfcTestClass::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{

	}

//This is called when an audio play completion is successfully played or otherwise
void CWmaDecoderIntfcTestClass::PlayError(TInt /* aError*/)
 	{
//	SendErrorToClient(aError);
	}


//Will panic if called.Should not be called during playing
void CWmaDecoderIntfcTestClass::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CWmaDecoderIntfcTestClass::RecordError(TInt /*aError*/)
	 {
  //   Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CWmaDecoderIntfcTestClass::ConvertError(TInt /*aError*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CWmaDecoderIntfcTestClass::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	 {
//	 Panic(KErrNotSupported);
	 }

void CWmaDecoderIntfcTestClass::MaiscOpenComplete(TInt aError)
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

void CWmaDecoderIntfcTestClass::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{

	}

void CWmaDecoderIntfcTestClass::MaiscRecordComplete(TInt /*aError*/)
	{

	}

// End of File

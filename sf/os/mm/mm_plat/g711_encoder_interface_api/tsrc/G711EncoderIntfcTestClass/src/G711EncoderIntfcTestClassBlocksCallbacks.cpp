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
* Description: G711EncoderIntfc Test
*
*/



// INCLUDE FILES
#include "G711EncoderIntfcTestClass.h"
#include "debug.h"





/************************************************************************************************************/

//Devsound initialization is completed.Configure devsound capabilities.
void CG711EncoderIntfcTestClass::InitializeComplete(TInt aError)
	{
    FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::InitializeComplete")));
    iLog->Log(_L("InitializeComplete, return code =%d!!!"),aError);
    ProcessEvent(EInitializeComplete, aError);

	}


void CG711EncoderIntfcTestClass::ToneFinished(TInt /*aError*/)
   	{
 //	Panic(KErrNotSupported);
 	}

//This is called when an empty audio buffer is available.Fill the buffer with audio data.
void CG711EncoderIntfcTestClass::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{

	}

//This is called when an audio play completion is successfully played or otherwise
void CG711EncoderIntfcTestClass::PlayError(TInt /* aError*/)
 	{
//	SendErrorToClient(aError);
	}


//Will panic if called.Should not be called during playing
void CG711EncoderIntfcTestClass::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CG711EncoderIntfcTestClass::RecordError(TInt /*aError*/)
	 {
  //   Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CG711EncoderIntfcTestClass::ConvertError(TInt /*aError*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CG711EncoderIntfcTestClass::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	 {
//	 Panic(KErrNotSupported);
	 }

void CG711EncoderIntfcTestClass::MaiscOpenComplete(TInt aError)
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

void CG711EncoderIntfcTestClass::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
    FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::MaiscBufferCopied")));
    iLog->Log(_L("MaiscBufferCopied, return code =%d!!!"),aError);
    ProcessEvent(EInitializeComplete, aError);
	}

void CG711EncoderIntfcTestClass::MaiscRecordComplete(TInt aError)
	{
    FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::MaiscRecordComplete")));
    iLog->Log(_L("MaiscRecordComplete, return code =%d!!!"),aError);
    ProcessEvent(EInitializeComplete, aError);
	}

//End of File

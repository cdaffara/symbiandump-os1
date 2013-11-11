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
*
* Description:  ErrConcealmentIntfc test
*
*/



// INCLUDE FILES
#include "ErrorConcealmentIntfcTestClass.h"
#include "debug.h"





/************************************************************************************************************/

//Devsound initialization is completed.Configure devsound capabilities.
void CErrorConcealmentIntfcTestClass::InitializeComplete(TInt aError)
	{
    FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::InitializeComplete")));

    iLog->Log(_L("InitializeComplete, return code =%d!!!"),aError);
    ProcessEvent(EInitializeComplete, aError);

	}


void CErrorConcealmentIntfcTestClass::ToneFinished(TInt /*aError*/)
   	{
 //	Panic(KErrNotSupported);
 	}

//This is called when an empty audio buffer is available.Fill the buffer with audio data.
void CErrorConcealmentIntfcTestClass::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{

	}

//This is called when an audio play completion is successfully played or otherwise
void CErrorConcealmentIntfcTestClass::PlayError(TInt /* aError*/)
 	{
//	SendErrorToClient(aError);
	}


//Will panic if called.Should not be called during playing
void CErrorConcealmentIntfcTestClass::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CErrorConcealmentIntfcTestClass::RecordError(TInt /*aError*/)
	 {
  //   Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CErrorConcealmentIntfcTestClass::ConvertError(TInt /*aError*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CErrorConcealmentIntfcTestClass::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	 {
//	 Panic(KErrNotSupported);
	 }

// Audio Output Stream
void CErrorConcealmentIntfcTestClass::MaoscOpenComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::MaoscOpenComplete")));
		iLog->Log(_L("MaoscOpenComplete, return code =%d!!!"),aError);
		}
	else
		{
		FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::MaoscOpenComplete FAILED %d"),aError));
		iLog->Log(_L("MaoscOpenComplete Error!, return code =%d!!!"),aError);
		}
	ProcessEvent(EMaoscOpenComplete, aError);
	}

void CErrorConcealmentIntfcTestClass::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{

	}

void CErrorConcealmentIntfcTestClass::MaoscPlayComplete(TInt /*aError*/)
	{

	}

// End of File

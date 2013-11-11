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
* Description:  G711DecoderIntfcTestClass test module.
*
*/



// INCLUDE FILES
#include "G711DecoderIntfcTestClass.h"
#include "debug.h"





/************************************************************************************************************/

//Devsound initialization is completed.Configure devsound capabilities.
void CG711DecoderIntfcTestClass::InitializeComplete(TInt aError)
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


void CG711DecoderIntfcTestClass::ToneFinished(TInt /*aError*/)
   	{
 //	Panic(KErrNotSupported);
 	}

//This is called when an empty audio buffer is available.Fill the buffer with audio data.
void CG711DecoderIntfcTestClass::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{

	}

//This is called when an audio play completion is successfully played or otherwise
void CG711DecoderIntfcTestClass::PlayError(TInt /* aError*/)
 	{
//	SendErrorToClient(aError);
	}


//Will panic if called.Should not be called during playing
void CG711DecoderIntfcTestClass::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CG711DecoderIntfcTestClass::RecordError(TInt /*aError*/)
	 {
  //   Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CG711DecoderIntfcTestClass::ConvertError(TInt /*aError*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CG711DecoderIntfcTestClass::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	 {
//	 Panic(KErrNotSupported);
	 }

// Audio Output Stream
void CG711DecoderIntfcTestClass::MaoscOpenComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		FTRACE(FPrint(_L("MaoscOpenComplete")));
		iLog->Log(_L("MaoscOpenComplete, return code =%d!!!"),aError);
		}
	else
		{
		FTRACE(FPrint(_L("MaoscOpenComplete FAILED %d"),aError));
		iLog->Log(_L("MaoscOpenComplete Error!, return code =%d!!!"),aError);
		}
	ProcessEvent(EMaoscOpenComplete, aError);
	}

void CG711DecoderIntfcTestClass::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
    iLog->Log(_L("MaoscBufferCopied, return code =%d!!!"),aError);
	}

void CG711DecoderIntfcTestClass::MaoscPlayComplete(TInt aError)
	{
    iLog->Log(_L("MaoscPlayComplete, return code =%d!!!"),aError);
	}

//End of File


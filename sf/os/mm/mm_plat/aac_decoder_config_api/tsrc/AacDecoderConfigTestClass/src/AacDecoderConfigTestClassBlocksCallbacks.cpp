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
* Description:  AacDecoderConfigTestClass test module.
*
*/



// INCLUDE FILES
#include "AacDecoderConfigTestClass.h"
#include "debug.h"





/************************************************************************************************************/

//Devsound initialization is completed.Configure devsound capabilities.
void CAacDecoderConfigTestClass::InitializeComplete(TInt aError)
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


void CAacDecoderConfigTestClass::ToneFinished(TInt /*aError*/)
   	{
 //	Panic(KErrNotSupported);
 	}

//This is called when an empty audio buffer is available.Fill the buffer with audio data.
void CAacDecoderConfigTestClass::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{

	}

//This is called when an audio play completion is successfully played or otherwise
void CAacDecoderConfigTestClass::PlayError(TInt /* aError*/)
 	{
//	SendErrorToClient(aError);
	}


//Will panic if called.Should not be called during playing
void CAacDecoderConfigTestClass::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CAacDecoderConfigTestClass::RecordError(TInt /*aError*/)
	 {
  //   Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CAacDecoderConfigTestClass::ConvertError(TInt /*aError*/)
	 {
//	 Panic(KErrNotSupported);
	 }


//Will panic if called.Should not be called during playing
void CAacDecoderConfigTestClass::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	 {
//	 Panic(KErrNotSupported);
	 }

// Audio Output Stream
void CAacDecoderConfigTestClass::MaoscOpenComplete(TInt aError)
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

void CAacDecoderConfigTestClass::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{

	}

void CAacDecoderConfigTestClass::MaoscPlayComplete(TInt /*aError*/)
	{

	}

// End of File

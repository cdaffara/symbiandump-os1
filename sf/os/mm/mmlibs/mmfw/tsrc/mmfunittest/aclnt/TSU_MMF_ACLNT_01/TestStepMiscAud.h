
// TestStepMiscAud.h

// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TESTSTEPMISCAUD_H
#define TESTSTEPMISCAUD_H

#include <testframework.h>
#include <mdaaudioinputstream.h>
#include <mdaaudiooutputstream.h>
#include <mdaaudiotoneplayer.h>

class RTestStepAudInStreamCustIntf : public RTestStep, public MMdaAudioInputStreamCallback
	{
public:
	static RTestStepAudInStreamCustIntf* NewL();

	// from RTestStep
	TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback - should never be called
	void MaiscOpenComplete(TInt aError);
	void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	void MaiscRecordComplete(TInt aError);

protected:
	RTestStepAudInStreamCustIntf();
	};

class RTestStepAudOutStreamCustIntf : public RTestStep, public MMdaAudioOutputStreamCallback
	{
public:
	static RTestStepAudOutStreamCustIntf* NewL();

	// from RTestStep
	TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	void MaoscOpenComplete(TInt aError);
	void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	void MaoscPlayComplete(TInt aError);

protected:
	RTestStepAudOutStreamCustIntf();
	};

class RTestStepTonePlayCustIntf : public RTestStep, public MMdaAudioToneObserver
	{
public:
	static RTestStepTonePlayCustIntf* NewL();

	// from RTestStep
	TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	void MatoPrepareComplete(TInt aError);
	void MatoPlayComplete(TInt aError);

protected:
	RTestStepTonePlayCustIntf();
	};

#endif // TESTSTEPMISCAUD_H

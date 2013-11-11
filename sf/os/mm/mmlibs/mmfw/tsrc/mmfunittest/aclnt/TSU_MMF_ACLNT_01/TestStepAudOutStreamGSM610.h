// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the output stream formats tests
// for GSM610 wav forms.  These require a buffer size
// of modulo 65 bytes.  Hence we have put this in a 
// separate class.
// 
//

#ifndef __TEST_STEP_AUD_OUT_STREAM_GSM610__
#define __TEST_STEP_AUD_OUT_STREAM_GSM610__

#include <mmf/server/mmffile.h>
#include "TestStepUnitMMFAudClient.h"
#include <mdaaudiooutputstream.h>


#if defined __WINS__
const TInt KGSM610BufferSize = 4160;	// use 4K GSM buffer on EKA2 due to difference in Sound Driver
#else
// require a larger buffer size on Lubbock 
const TInt KGSM610BufferSize = 6500;	// use ~6K GSM buffer to prevent an underrun
#endif // __WINS__
 
const TInt KGSM610NumBuffer = 6; // buffer to store data read from file

// parameter structure for conversion tests
class TGSM610ConvParameters
	{	
public:
	const TText* iTestName;
	TUint32 iEncoding;
	TUint	iSampleRate;
	TUint	iChannels;
	const TText* iFromFilename;
	TInt	iExpectedResult;
	};


// base class for the audio Output stream tests
class CTestStepAudOutStreamGSM610 : public CTestStepUnitMMFAudClient,
							  public MMdaAudioOutputStreamCallback
{													           
public:
	static CTestStepAudOutStreamGSM610* NewL();
	~CTestStepAudOutStreamGSM610();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	//virtual TInt HandleKeyEvent(/*TKeyCode aKeystroke*/);
	//virtual void StartL();
	//virtual const TDesC& Instructions();
	//void passParent(CTestStepUnitMMFAudClient& aParent);
	void ProcessDataL(TBuf8<KGSM610BufferSize>& aData);

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError); 

protected:
	CTestStepAudOutStreamGSM610();
	void ConstructL();
	void ResetBufferList();

protected:
	CMMFMdaAudioOutputStream* iAudOutStream;
	TBuf8<KGSM610BufferSize>	iBufferList[KGSM610NumBuffer];
	TInt					iStartBuf;
	TInt					iEndBuf;
	TInt					iError;
	TBool					iIsAllDataRead;
	TBool					iIsAllDataWritten;

protected:
	class CReadBufferActive : public CActive
		{
		friend class CTestStepAudOutStreamGSM610;
	public:
		CReadBufferActive();
		~CReadBufferActive();
		virtual void RunL();
		virtual void DoCancel();
		void Read();
		void Read(TInt aPos);
	public:
		TBuf8<KGSM610BufferSize>		iBuffer;
		CTestStepAudOutStreamGSM610*	iParent;
		};

protected:
	CReadBufferActive*		iReadBufferActive;

friend class CReadBufferActive;
	};



/**
 *
 * Test Output stream on GSM610 formats
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamGSM610Formats :	public	CTestStepAudOutStreamGSM610
	{
public:
	static CTestStepAudOutStreamGSM610Formats* NewL(TUint aTestIndex);
	CTestStepAudOutStreamGSM610Formats(TUint aTestIndex);
	~CTestStepAudOutStreamGSM610Formats();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	TFileName SetFileNameL( const TDesC& aPathNameAndExtn );
	inline TMMFFileConfig& GetFileName() { return iFileName; }
	// from MMdaAudioOutputStreamCallback
private:
	const TGSM610ConvParameters* iTestParameters;
	TMMFFileConfig	iFileName;// audio output file 
	};


#endif //(__TEST_STEP_AUD_OUT_STREAM_GSM610__)

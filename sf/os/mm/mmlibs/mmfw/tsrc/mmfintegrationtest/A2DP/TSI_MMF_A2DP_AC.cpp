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

#include <e32base.h>

#include "TSI_MMF_A2DP_AC.h"

#include <mmfHwDeviceImplementationUids.hrh>

const TInt KSizeBuf = 256;
const TInt KToneFrequency = 880;
const TInt KOneSecond = 1000000;
const TInt KTwoSeconds = 2000000;


/**
 *
 * RTestStepA2dpAclPlyNew
 *
 */
RTestStepA2dpAclPlyNew::RTestStepA2dpAclPlyNew(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclPlyNew::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntPlayer NewFilePlayerL with audio routing to A2DP"));

	TVerdict res;
	res = EFail;

	TBuf<KSizeBuf>	filename;
	filename.Append(KAclntFileNamePCM16M);

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this, EMdaPriorityNormal, prioritySettings.iPref);

	// Wait for initialisation callback
	CActiveScheduler::Start();

	if (iError == KErrNone)
		{
		res = EPass;
		}
	else
		{
		INFO_PRINTF1( _L("Fail to Initialise CMdaAudioPlayerUtility"));
		res = EFail;
		}
	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	res;
	}


/**
 *
 * RTestStepA2dpAclPlyOpenCloseOpen
 *
 */
RTestStepA2dpAclPlyOpenCloseOpen::RTestStepA2dpAclPlyOpenCloseOpen(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclPlyOpenCloseOpen::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntPlayer Open->Close->Open with audio routing to A2DP"));

	TVerdict res;
	res = EFail;

	TBuf<KSizeBuf>	filename;
	filename.Append(KAclntFileNamePCM16M);

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this, EMdaPriorityNormal, prioritySettings.iPref);

	// Wait for initialisation callback
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to initialise CMdaAudioPlayerUtility"));
		res = EInconclusive;
		delete player;
		return res;
		}

	//To close the audio player
	player->Close(); 

	// Open the file again
	TRAPD(err, player->OpenFileL(filename));
	if (err != KErrNone)
		{
		INFO_PRINTF1( _L("OpenFileL fails"));
		res = EFail;
		delete player;
		return res;
		}
	else
		{
		// wait for open to complete
		INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility again"));
		CActiveScheduler::Start();
		}

	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to initialise CMdaAudioPlayerUtility"));
		res = EFail;
		delete player;
		return res;
		}
	else
		{
		res = EPass;
		}

	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	res;
	}


/**
 *
 * RTestStepA2dpAclPlyReInitDisconn
 *
 */
RTestStepA2dpAclPlyReInitDisconn::RTestStepA2dpAclPlyReInitDisconn(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclPlyReInitDisconn::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntPlayer: reinitialize a disconnected headset "));

	TVerdict res;
	res = EFail;

	TBuf<KSizeBuf>	filename;
	filename.Append(KAclntFileNamePCM16M);

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this, EMdaPriorityNormal, prioritySettings.iPref);

	// Wait for initialisation callback
	CActiveScheduler::Start();
	
	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to initialise CMdaAudioPlayerUtility"));
		res = EInconclusive;
		delete player;
		return res;
		}

	//Disconnect BT link
	INFO_PRINTF1(_L("Disconnecting the BT link for the next test"));
	iBtPhysicalLink->Disconnect(*iBTheadsetAddress);
	iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeToRegisterLinkLost));
	CActiveScheduler::Current()->Start();

	if (!iBTLinkDisconnected) 
		{
		INFO_PRINTF1(_L("Error: BT link failed to disconnect" ));
		delete player;
		return EFail;
		}

	// Open the file again
	TRAPD(err, player->OpenFileL(filename));
	if (err != KErrNone)
		{
		INFO_PRINTF1( _L("OpenFileL fails"));
		res = EFail;
		delete player;
		return res;
		}
	else
		{
		// wait for open to complete
		INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility again"));
		CActiveScheduler::Start();
		}

	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to initialise CMdaAudioPlayerUtility"));
		res = EFail;
		delete player;
		return res;
		}
	else
		{
		res = EPass;
		}

	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	res;
	}


/**
 *
 * RTestStepA2dpAclPlyReInit
 *
 */
RTestStepA2dpAclPlyReInit::RTestStepA2dpAclPlyReInit(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclPlyReInit::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntPlayer: Reinitialize the headset "));

	TVerdict res;
	res = EFail;

	TBuf<KSizeBuf>	filename;
	filename.Append(KAclntFileNamePCM16M);

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this, EMdaPriorityNormal, prioritySettings.iPref);

	// Wait for initialisation callback
	CActiveScheduler::Start();
	
	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to initialise CMdaAudioPlayerUtility"));
		delete player;
		res = EInconclusive;
		return res;
		}

	// Open the file again
	TRAPD(err, player->OpenFileL(filename));
	if (err != KErrNone)
		{
		INFO_PRINTF1( _L("OpenFileL fails"));
		delete player;
		res = EFail;
		return res;
		}
	else
		{
		// wait for open to complete
		INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility again"));
		CActiveScheduler::Start();
		}

	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to initialise CMdaAudioPlayerUtility"));
		delete player;
		res = EFail;
		return res;
		}
	else
		{
		res = EPass;
		}

	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	res;
	}


/**
 *
 * RTestStepA2dpAclRecNew
 *
 */
RTestStepA2dpAclRecNew::RTestStepA2dpAclRecNew(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclRecNew::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntRecorder: New recorder test "));

	TVerdict res;
	res = EFail;

	TBuf<KSizeBuf>	filename;
	filename.Append(KAclntFileNamePCM16M);

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	//Initialize the recorder
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CMdaAudioRecorderUtility* recorder = NULL;
	recorder = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityNormal, prioritySettings.iPref);
	CleanupStack::PushL(recorder);
	
	//Recorder to open the file
	INFO_PRINTF1( _L("Openning the file"));
	TRAPD(err, recorder->OpenFileL(filename));	
	if (err != KErrNone)
		{
		INFO_PRINTF1( _L("OpenFileL fails"));
		CleanupStack::PopAndDestroy(recorder);
		res = EFail;
		return res;
		}
	else
		{
		// wait for open to complete
		INFO_PRINTF1( _L("OpenFileL starts"));
		CActiveScheduler::Start();
		}

	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to open the file"));
		CleanupStack::PopAndDestroy(recorder);
		res = EFail;
		return res;
		}
	else
		{
		res = EPass;
		}

	CleanupStack::PopAndDestroy(recorder);
	User::After(KTwoSeconds);
	return res;
	}


/**
 *
 * RTestStepA2dpAclRecOpenCloseOpen
 *
 */
RTestStepA2dpAclRecOpenCloseOpen::RTestStepA2dpAclRecOpenCloseOpen(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclRecOpenCloseOpen::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntRecorder: Open->Close->Open test "));

	TVerdict res;
	res = EFail;

	TBuf<KSizeBuf>	filename;
	filename.Append(KAclntFileNamePCM16M);

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	//Initialize the recorder
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CMdaAudioRecorderUtility* recorder = NULL;
	recorder = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityNormal, prioritySettings.iPref);
	CleanupStack::PushL(recorder);
	
	//Recorder: Open the file first time
	INFO_PRINTF1( _L("Openning the file"));
	TRAPD(err, recorder->OpenFileL(filename));	
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to open the file first time"));
		CleanupStack::PopAndDestroy(recorder);
		res = EInconclusive;
		return res;
		}
	
	//Close the file
	recorder->Close();

	//Recorder: to open the file second time
	INFO_PRINTF1( _L("Openning the file"));
	TRAP(err, recorder->OpenFileL(filename));	
	if (err != KErrNone)
		{
		INFO_PRINTF1( _L("OpenFileL fails"));
		CleanupStack::PopAndDestroy(recorder);
		res = EFail;
		return res;
		}
	else
		{
		// wait for open to complete
		INFO_PRINTF1( _L("OpenFileL starts"));
		CActiveScheduler::Start();
		}

	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("Fail to open the file"));
		CleanupStack::PopAndDestroy(recorder);
		res = EFail;
		return res;
		}
	else
		{
		res = EPass;
		}

	CleanupStack::PopAndDestroy(recorder);
	User::After(KTwoSeconds);
	return res;
	}


/**
 *
 * RTestStepA2dpAclToneNew
 *
 */
RTestStepA2dpAclToneNew::RTestStepA2dpAclToneNew(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclToneNew::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntTone: New Tone Utility test "));

	TVerdict res;
	res = EFail;

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	//New ToneUtility
	INFO_PRINTF1( _L("New CMdaAudioToneUtility"));
	CMdaAudioToneUtility* toneUtil = NULL;
	toneUtil = CMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityNormal, prioritySettings.iPref);
	CleanupStack::PushL(toneUtil);

	//Initialize ToneUtility
	INFO_PRINTF1( _L("Prepare CMdaAudioToneUtility"));
	TInt freq = KToneFrequency;
	TTimeIntervalMicroSeconds dur;
	dur = TTimeIntervalMicroSeconds(KOneSecond);
	toneUtil->PrepareToPlayTone(freq, dur);

	// Wait for prepare
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		res = EPass;
		}
	else
		{
		ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );
		CleanupStack::PopAndDestroy(toneUtil);
		res = EFail;
		return res;
		}

	CleanupStack::PopAndDestroy(toneUtil);
	return	res;
	}


/**
 *
 * RTestStepA2dpAclStrmNew
 *
 */
RTestStepA2dpAclStrmNew::RTestStepA2dpAclStrmNew(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclStrmNew::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntStrm: New Output Stream test "));

	TVerdict res;
	res = EFail;

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	//New OutputStream
	INFO_PRINTF1( _L("New CMdaAudioOutputStream"));
	CMdaAudioOutputStream* outputStream = NULL;
	outputStream = CMdaAudioOutputStream::NewL(*this, EMdaPriorityNormal, prioritySettings.iPref);
	CleanupStack::PushL(outputStream);

	//Open OutputStream
	INFO_PRINTF1( _L("Open CMdaAudioOutputStream"));
	outputStream->Open(NULL);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening output stream for playback = %d"), iError);
		CleanupStack::PopAndDestroy(outputStream);
		return EFail;
		}
	else
		{
		res = EPass;
		}
	CleanupStack::PopAndDestroy(outputStream);
	return	res;
	}


/**
 *
 * RTestStepA2dpAclStrmWrite
 *
 */
RTestStepA2dpAclStrmWrite::RTestStepA2dpAclStrmWrite(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclStrmWrite::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntStrm: OutputStream write test "));

	TVerdict res;
	res = EFail;

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	//New OutputStream
	INFO_PRINTF1( _L("New CMdaAudioOutputStream"));
	CMdaAudioOutputStream* outputStream = NULL;
	outputStream = CMdaAudioOutputStream::NewL(*this, EMdaPriorityNormal, prioritySettings.iPref);
	CleanupStack::PushL(outputStream);
	
	//Open OutputStream
	INFO_PRINTF1( _L("Open CMdaAudioOutputStream"));
	outputStream->Open(NULL);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening output stream for playback = %d"), iError);
		CleanupStack::PopAndDestroy(outputStream);
		return EInconclusive;
		}
		
	// Connect file session	
	RFs fs;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting to file session = %d"), err);
		return EInconclusive;
		}
	CleanupClosePushL(fs);

	TFileName filename(KInputTestFileNamePCM16);
	
	// Open file
	RFile file;
	err = file.Open(fs, filename, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file = %d"), err);
		CleanupStack::PopAndDestroy(2, outputStream);	// fs, outputStream
		return EInconclusive;
		}
	CleanupClosePushL(file);

	// Get size of file
	TInt fileSize = 0;
	err = file.Size(fileSize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of file = %d"), err);
		CleanupStack::PopAndDestroy(3, outputStream);	// file, fs, outputStream
		return EInconclusive;
		}
	
 	INFO_PRINTF1(_L("Set the Audio Properties to 32Khz and Mono"));
	TRAP(err, outputStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate32000Hz,TMdaAudioDataSettings::EChannelsMono));
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error setting the audio properties %d"), err);
		CleanupStack::PopAndDestroy(3, outputStream);	//file, fs, outputStream
		return EInconclusive;
		}
	
	// Read file
	HBufC8* buf = HBufC8::NewLC(fileSize);
	TPtr8 ptr(buf->Des());
	err = file.Read(ptr);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading file %d"), err);
		CleanupStack::PopAndDestroy(4, outputStream);	// buf, file, fs, outputStream
		return EInconclusive;
		}
	else
		{
		INFO_PRINTF1( _L("Outputstream Writing"));
		outputStream->WriteL(ptr);
		CActiveScheduler::Start();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Error writing file on audio output stream %d"), iError);
			CleanupStack::PopAndDestroy(4, outputStream);	// buf, file, fs, outputStream
			return EFail;
			}
		res = EPass;
		}
	CleanupStack::PopAndDestroy(4, outputStream);	// buf, file, fs, outputStream
	return	res;
	}


/**
 *
 * RTestStepA2dpAclStrmWriteStop
 *
 */
RTestStepA2dpAclStrmWriteStop::RTestStepA2dpAclStrmWriteStop(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclStrmWriteStop::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test AclntStrm: OutputStream write and stop test "));

	TVerdict res;
	res = EFail;

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	//New OutputStream
	INFO_PRINTF1( _L("New CMdaAudioOutputStream"));
	CMdaAudioOutputStream* outputStream = NULL;
	outputStream = CMdaAudioOutputStream::NewL(*this, EMdaPriorityNormal, prioritySettings.iPref);
	CleanupStack::PushL(outputStream);
	
	//Open OutputStream
	INFO_PRINTF1( _L("Open CMdaAudioOutputStream"));
	outputStream->Open(NULL);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening output stream for playback = %d"), iError);
		CleanupStack::PopAndDestroy(outputStream);
		return EInconclusive;
		}
	
	// Connect file session	
	RFs fs;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting to file session = %d"), err);
		return EInconclusive;
		}
	CleanupClosePushL(fs);

	TFileName filename(KInputTestFileNamePCM16);
	
	// Open file
	RFile file;
	err = file.Open(fs, filename, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file = %d"), err);
		CleanupStack::PopAndDestroy(2, outputStream);	// fs, outputStream
		return EInconclusive;
		}
	CleanupClosePushL(file);

	// Get size of file
	TInt fileSize = 0;
	err = file.Size(fileSize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of file = %d"), err);
		CleanupStack::PopAndDestroy(3, outputStream);	// file, fs, outputStream
		return EInconclusive;
		}
			
	INFO_PRINTF1(_L("Set the Audio Properties to 32Khz and Mono"));
	TRAP(err, outputStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate32000Hz,TMdaAudioDataSettings::EChannelsMono));
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error setting the audio properties %d"), err);
		CleanupStack::PopAndDestroy(3, outputStream);	//file, fs, outputStream
		return EInconclusive;
		}
		
	// Read file
	HBufC8* buf = HBufC8::NewLC(fileSize);
	TPtr8 ptr(buf->Des());
	err = file.Read(ptr);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading file %d"), err);
		CleanupStack::PopAndDestroy(4, outputStream);	// buf, file, fs, outputStream
		return EInconclusive;
		}
	
	INFO_PRINTF1( _L("Outputstream Writing"));
	outputStream->WriteL(ptr);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error writing file on audio output stream %d"), iError);
		CleanupStack::PopAndDestroy(4, outputStream);	// buf, file, fs, outputStream
		return EFail;
		}
	
	INFO_PRINTF1( _L("Outputstream stop"));
	outputStream->Stop();
	
	INFO_PRINTF1( _L("Outputstream write again"));
	outputStream->WriteL(ptr);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error writing file on audio output stream %d"), iError);
		CleanupStack::PopAndDestroy(4, outputStream);	// buf, file, fs, outputStream
		return EFail;
		}

	CleanupStack::PopAndDestroy(4, outputStream);	// buf, file, fs, outputStream
	return	res;
	}


/**
 *
 * RTestStepA2dpAclStrmNewNeg
 *
 */
RTestStepA2dpAclStrmNewNeg::RTestStepA2dpAclStrmNewNeg(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}	

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dpAclStrmNewNeg::DoTestStepL()
	{
	INFO_PRINTF1(_L("Negative Test AclntStrm: New Output Stream test when the BT headset is not available "));

	TVerdict res;
	res = EFail;

	TMMFPrioritySettings	prioritySettings;
	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	TInt pref = static_cast<TInt>(prioritySettings.iPref);
	pref |= static_cast<TInt>(EA2DPBluetoothHeadset);	
	TMdaPriorityPreference mdapref = static_cast<TMdaPriorityPreference>(pref);
	prioritySettings.iPref = mdapref;

	//New OutputStream
	INFO_PRINTF1( _L("New CMdaAudioOutputStream"));
	CMdaAudioOutputStream* outputStream = NULL;
	outputStream = CMdaAudioOutputStream::NewL(*this, EMdaPriorityNormal, prioritySettings.iPref);
	CleanupStack::PushL(outputStream);

	//Open OutputStream
	INFO_PRINTF1( _L("Open CMdaAudioOutputStream"));
	outputStream->Open(NULL);
	CActiveScheduler::Start();
	if (iError != KErrCouldNotConnect)
		{
		INFO_PRINTF2(_L("-34 is expected. But it returns %d"), iError);
		CleanupStack::PopAndDestroy(outputStream);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("It does return %d"), iError);
		res = EPass;
		}
	CleanupStack::PopAndDestroy(outputStream);
	return	res;
	}


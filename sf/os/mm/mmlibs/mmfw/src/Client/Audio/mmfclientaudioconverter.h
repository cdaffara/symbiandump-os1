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
//

#ifndef __MMFCLIENTAUDIOCONVERTER_H
#define __MMFCLIENTAUDIOCONVERTER_H

#include <e32std.h>
#include <e32base.h>
#include <mdaaudiosampleeditor.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmfurl.h>
#include <mmf/common/mmfcontroller.h>
#include "mmfclientutility.h"

/**
Concrete implementation of the CMdaAudioConvertUtility API.
@see CMdaAudioConvertUtility
*/
class CMMFMdaAudioConvertUtility;
NONSHARABLE_CLASS( CMMFMdaAudioConvertUtility ): public CBase,
								   				 public MMMFControllerEventMonitorObserver,
								   				 public MMMFFindAndOpenControllerObserver
	{
friend class CMdaAudioConvertUtility;
// for testing only
friend class CTestStepUnitMMFAudClient;

public:
	~CMMFMdaAudioConvertUtility();

	void OpenL(const TDesC& aPrimaryFile, const TDesC& aSecondaryFile);
	// Open existing file and new/existing secondary
	void OpenL(const TDesC& aPrimaryFile,
			   TMdaClipLocation* aLocation,	// Normally file or descriptor
			   TMdaClipFormat* aFormat,		// Data format
			   TMdaPackage* aArg1 = NULL,	// Normally codec to use
			   TMdaPackage* aArg2 = NULL);	// Normally audio settings

	void OpenL(TMdaClipLocation* aPriLocation, 
			   TMdaClipLocation* aSecLocation, 
			   TMdaClipFormat* aPriFormat, 
			   TMdaClipFormat* aSecFormat, 
			   TMdaPackage* aPriArg1 = NULL, // Normally source codec to use
			   TMdaPackage* aPriArg2 = NULL, // Normally source audio settings
			   TMdaPackage* aSecArg1 = NULL, // Normally sink codec to use
			   TMdaPackage* aSecArg2 = NULL);// Normally sink audio settings

	// new controller based open
	void OpenL(TMdaClipLocation* aPriLocation, 
			   TMdaClipLocation* aSecLocation, 
			   TUid aControllerUid, // the controller to use
			   TUid aDestFormatUid,
			   TFourCC aDestDataType);	
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);

	// from MMMFFindAndOpenControllerObserver 
	virtual void MfaocComplete(
		TInt& aError, 
		RMMFController* aController, 
		TUid aControllerUid, 
		TMMFMessageDestination* aSourceHandle, 
		TMMFMessageDestination* aSinkHandle);
	
	void UseSharedHeap();

protected:
	CMMFMdaAudioConvertUtility(CMdaAudioConvertUtility* aParent, MMdaObjectStateChangeObserver& aCallback);
	void ConstructL(TInt aPriority, TInt aPref);
private:

	CMdaAudioClipUtility::TState State();
	void Close();
	void PlayL();		// N.B. ConvertL should be used in preference to PlayL
	void RecordL();		// N.B. ConvertL should be used in preference to RecordL
	void ConvertL();
	void Stop();
	void CropL(TBool aCropToEnd = ETrue);
	void SetPosition(const TTimeIntervalMicroSeconds& aPosition);
	const TTimeIntervalMicroSeconds& Position();
	const TTimeIntervalMicroSeconds& RecordTimeAvailable();
	const TTimeIntervalMicroSeconds& Duration();
	void SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);
	void ClearPlayWindow();
	void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	void SetMaxWriteLength(TInt aMaxWriteLength /*= KMdaClipLocationMaxWriteLengthNone*/);
	void SetPriority(TInt aPriority, TInt aPref);  
	void CropL(const TTimeIntervalMicroSeconds & aCropBegin, 
			   const TTimeIntervalMicroSeconds & aCropEnd);

	// Codec support
	void GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDestinationDataTypes);
	void SetSourceDataTypeL(TFourCC aDataType)	;
	void SetDestinationDataTypeL(TFourCC aCodec);
	TFourCC DestinationDataTypeL();
	TFourCC SourceDataTypeL();

	// Bit rate support	
	void SetDestinationBitRateL(TUint aBitRate);
	TUint DestinationBitRateL();
	void GetSupportedConversionBitRatesL(RArray<TUint>& aSupportedBitRates);
	TUint SourceBitRateL();

	// Sample rate support
	void SetSourceSampleRateL(TUint aSampleRate);
	void SetDestinationSampleRateL(TUint aSampleRate);
	TUint DestinationSampleRateL();
	void GetSupportedConversionSampleRatesL(RArray<TUint>& aSupportedSampleRates);
	TUint SourceSampleRateL();
	
	// Format support
	void SetSourceFormatL(TUid aRecordFormat);
	void SetDestinationFormatL(TUid aFormatUid);
	TUid DestinationFormatL();
	TUid SourceFormatL();

	// Number of channels
	void SetSourceNumberOfChannelsL(TUint aNumberOfChannels);
	void SetDestinationNumberOfChannelsL(TUint aNumberOfChannels);
	TUint DestinationNumberOfChannelsL();
	void GetSupportedConversionNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels);
	TUint SourceNumberOfChannelsL();
	
	const CMMFControllerImplementationInformation& ControllerImplementationInformationL();
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);

	CMdaAudioConvertUtility* Parent() const;

	void Reset();
	TInt ControllerOpen();
	void ConfigureSourceSinkL(TMdaClipLocation* aPriLocation, TMdaClipLocation* aSecLocation);

	TInt SetThreadPriority(const TThreadPriority& aThreadPriority) const;
private:
	MMdaObjectStateChangeObserver& iCallback;
	CMMFMdaObjectStateChangeObserverCallback* iAsyncCallback;

	RMMFController iController;
	CMMFControllerEventMonitor* iControllerEventMonitor;

	// Source and sink handle info
	TMMFMessageDestination iSourceHandle;
	TMMFMessageDestination iSinkHandle;
	// Custom commands
	RMMFAudioControllerCustomCommands iAudioControllerCustomCommands;
	RMMFAudioPlayControllerCustomCommands iAudioPlayControllerCustomCommands;
	RMMFAudioRecordControllerCustomCommands iAudioRecordControllerCustomCommands;

	TMMFPrioritySettings iPrioritySettings;

	TTimeIntervalMicroSeconds iPosition;
	TTimeIntervalMicroSeconds iPositionTemp;
	TTimeIntervalMicroSeconds iDuration;
	TTimeIntervalMicroSeconds iRecordTimeAvailable;

	CMdaAudioClipUtility::TState iState;

	// Convert window start and end times and whether it has been set
	TTimeIntervalMicroSeconds iConvertStart;
	TTimeIntervalMicroSeconds iConvertEnd;
	TBool iConvertWindowSet;

	// parent class for callback
	CMdaAudioConvertUtility* iParent;
	RArray<TUid> iMediaIds;

	// keeping track of cropped clip duration
	TBool iHasCropped;
	TTimeIntervalMicroSeconds iCroppedDuration;
	CMMFControllerImplementationInformation* iControllerImplementationInformation;
	TUid iControllerUid;

	// utility class to find and open a suitable controller asynchronously
	CMMFFindAndOpenController* iFindAndOpenController;

	TUid iSourceFormatUid;		// for SetSourceFormat()
	TUid iSinkFormatUid;		// for SetSinkFormat()

	TFourCC iSourceDataType;	// for SetSourceDataType()
	TFourCC iSinkDataType;		// for SetSinkDataType()

	TInt iSourceSampleRate;		// for SetSourceSampleRateL()
	TInt iSinkSampleRate;		// for SetDestinationSampleRateL()

	TInt iSourceChannels;		// for SetSourceNumberOfChannelsL()
	TInt iSinkChannels;			// for SetDestinationNumberOfChannelsL()
	};

#endif

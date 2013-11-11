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


#ifndef OGGRECORDCONTROLLER_H
#define OGGRECORDCONTROLLER_H

#include <e32base.h>
#include <ecom/implementationproxy.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfaudioinput.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <stdlib.h>
#include "OggRecordControllerUIDs.hrh"
#include "OggEncode.h"

#define KMMFFourCCCodeVORB TFourCC('V', 'O', 'R', 'B')
const TInt KNumSampleRates = 11;

struct TSampleRateTable
    	{
		TInt iRate;
		TMMFSampleRate iRateEnum;
		};

/*
 TMmfOggRecordControllerPanics is an enumeration with the panic codes internal to OggRecordController.
 EBadCall indicates that the particular call is not supposed to be made in the OggRecordController.
*/
enum TMmfOggRecordControllerPanics
	{
	EBadCall,
	};

/**
@internalTechnology

The public API of COggRecordController. It mainly implements the public APIs in CMMFController. It also implements RecordCustomInterfaces 
to support extended functionality during recording. It is reponsible for controlling the data transfer from DevSound to Sink.
*/
class COggRecordController : public CMMFController,
                             public MMMFAudioRecordDeviceCustomCommandImplementor,
                             public MMMFAudioRecordControllerCustomCommandImplementor,
                             public MMMFAudioControllerCustomCommandImplementor,
                             public MDevSoundObserver,
                             public MOggEncodeObserver
    {
public:
    static COggRecordController* NewL();
    virtual ~COggRecordController();
    
    //from CMMFController
    void AddDataSourceL(MDataSource& aSource);
    void AddDataSinkL(MDataSink& aSink);
    void RemoveDataSourceL(MDataSource& aSource);
    void RemoveDataSinkL(MDataSink& aSink);
    void ResetL();
    void PrimeL(TMMFMessage& aMessage);
    void PrimeL();
    void PlayL();
    void PauseL();
    void StopL(TMMFMessage& aMessage);
    void StopL();
    TTimeIntervalMicroSeconds PositionL() const;
    void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
    TTimeIntervalMicroSeconds DurationL() const;
    void SetPrioritySettings(const TMMFPrioritySettings& aSettings);
    void GetNumberOfMetaDataEntriesL(TInt& aNumber);
    CMMFMetaDataEntry* GetMetaDataEntryL(TInt aIndex);
    
    //from MMMFAudioRecordDeviceCustomCommandImplementor
	void MardSetGainL(TInt aGain);
	void MardGetMaxGainL(TInt& aMaxGain);
	void MardGetGainL(TInt& aGain);
	void MardSetBalanceL(TInt aBalance);
	void MardGetBalanceL(TInt& aBalance);
	
    //from MMMFAudioRecordControllerCustomCommandImplementor
	void MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime);
	void MarcSetMaxDurationL(const TTimeIntervalMicroSeconds& aMaxDuration);
	void MarcSetMaxFileSizeL(TInt aMaxSize);
	void MarcCropL(TBool aToEnd);
	void MarcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);
	void MarcRemoveMetaDataEntryL(TInt aIndex);
	void MarcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry);
    
    //from MMMFAudioControllerCustomCommandImplementor
	void MacSetSourceSampleRateL(TUint aSampleRate);
	void MacSetSourceNumChannelsL(TUint aNumChannels);
	void MacSetSourceFormatL(TUid aFormatUid);
	void MacSetSinkSampleRateL(TUint aSampleRate);
	void MacSetSinkNumChannelsL(TUint aNumChannels);
	void MacSetSinkFormatL(TUid aFormatUid);
	void MacSetCodecL(TFourCC aSourceDataType, TFourCC aSinkDataType);
	void MacSetSourceBitRateL(TUint aBitRate);
	void MacSetSourceDataTypeL(TFourCC aDataType);
	void MacSetSinkBitRateL(TUint aRate);
	void MacSetSinkDataTypeL(TFourCC aDataType);
	void MacGetSourceSampleRateL(TUint& aRate);
	void MacGetSourceBitRateL(TUint& aRate);
	void MacGetSourceNumChannelsL(TUint& aNumChannels);
	void MacGetSourceFormatL(TUid& aFormat);
	void MacGetSourceDataTypeL(TFourCC& aDataType);
	void MacGetSinkSampleRateL(TUint& aRate);
	void MacGetSinkBitRateL(TUint& aRate);
	void MacGetSinkNumChannelsL(TUint& aNumChannels);
	void MacGetSinkFormatL(TUid& aFormat);
	void MacGetSinkDataTypeL(TFourCC& aDataType);
	void MacGetSupportedSourceSampleRatesL(RArray<TUint>& aSupportedRates);
	void MacGetSupportedSourceBitRatesL(RArray<TUint>& aSupportedRates);
	void MacGetSupportedSourceNumChannelsL(RArray<TUint>& aSupportedChannels);
	void MacGetSupportedSourceDataTypesL(RArray<TFourCC>& aSupportedDataTypes);
	void MacGetSupportedSinkSampleRatesL(RArray<TUint>& aSupportedRates);
	void MacGetSupportedSinkBitRatesL(RArray<TUint>& aSupportedRates);
	void MacGetSupportedSinkNumChannelsL(RArray<TUint>& aSupportedChannels);
	void MacGetSupportedSinkDataTypesL(RArray<TFourCC>& aSupportedDataTypes);
	
	//from MDevSoundObserver 
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError); 
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError); 
	void BufferToBeEmptied(CMMFBuffer* aBuffer); 
	void RecordError(TInt aError); 
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	
	//from MOggEncodeObserver
	void BufferEmptied();
	 
	//Internal to oggrecordcontroller
	static void LeaveIfNullL(TAny* aPtr);
	void SendErrorToClient(TInt aError);
	void StartPrimeL();
private:
    COggRecordController();
    void ConstructL();
    
private:
    /**
	@internalTechnology
	
	This class provides the mechanism to store the client message during the asynchronous operation
	in the controller sothat it can be completed after the operation is completed. In specific, during PrimeL(), StopL().
	*/
	class CMMFMessageHolder : public CBase
		{
	public:
		static CMMFMessageHolder* NewL(TMMFMessage& aMessage);
		void Complete(TInt aError);
	private:
		CMMFMessageHolder(TMMFMessage& aMessage);
		TMMFMessage iMessage;
		};
    /**
	@internalTechnology
	
	This class is used to receive events from the source and sink on behalf of the controller.
	
	The reason behind adding an extra class for receiving events is that COggRecordcontroller cannot derive
	from both MAsyncEventHandler and MDevSoundObserver as there is a clash in the API they contain.
	*/
    class CAsyncEventHandler : public MAsyncEventHandler
		{
		public:
			CAsyncEventHandler(MDevSoundObserver& aParent);
			virtual ~CAsyncEventHandler();
		public:
			virtual TInt SendEventToClient(const TMMFEvent& aEvent);
		private:
			MDevSoundObserver& iParent;
		};
	/**
 	@internalTechnology
 	
 	The enumeration provides a set of controller states:
 	
 	EStopped is the initial state the controller resides in after it has been loaded
 	EPriming is the state the controller is in when it is waiting for the devsound to be initialized and configured the codec.
 	EPrimed is the state the controller is in when it has initialized devsound and configured the codec.
 	ERecording is the state the controller is in when it is streaming data from devsound to sink
	*/
	enum TControllerState
        {
        EStopped,
        EPriming,
        EPrimed,
        ERecording
        };
	TControllerState iState;
    
    //Extracted from MDataSource passed by the MMF. Not owned by the controller
    CMMFDevSound* iMMFDevSound;
    
    //Sink of the data which is not owned by the controller
    CMMFClip* iClip;
    MDataSource* iSource;
    COggEncode* iOggSink;
    CAsyncEventHandler* iEventHandler;
    CMMFMessageHolder* iMessage;
    TMMFPrioritySettings iPrioritySettings;
    };


#endif // OGGRECORDCONTROLLER_H

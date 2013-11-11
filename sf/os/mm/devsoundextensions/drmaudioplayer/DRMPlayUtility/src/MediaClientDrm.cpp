/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   DRM PlayUtility
*
*/




// INCLUDE FILES
#include <DrmAudioSamplePlayer.h>
#include "DRMPlayClientServer.h"
#include "mmfclientdrmaudioplayer.h"
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include "drmaudioplayeradaptation.h"

// This is the UID defined by custom interface builder
const TUid KUidCustomInterfaceBuilderImpl = {0x10207A8E};

#ifdef _DEBUG
#define DEP_PRN0(str)   RDebug::Print(str)
#define DEP_PRN1(str, v1)   RDebug::Print(str, v1)
#define DEP_PRN2(str, v1, v2)   RDebug::Print(str, v1, v2)
#else
#define DEP_PRN0(str)
#define DEP_PRN1(str, v1)
#define DEP_PRN2(str, v1, v2)
#endif // _DEBUG


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::NewL
// Constructs and initialises a new instance of the audio player utility.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDrmPlayerUtility* CDrmPlayerUtility::NewL(
    MDrmAudioPlayerCallback& aCallback,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    CDrmPlayerUtility* self = new(ELeave) CDrmPlayerUtility();
    CleanupStack::PushL(self);
    self->iProperties = CDrmAudioPlayerAdaptation::NewL( aCallback, aPriority, aPref );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::NewFilePlayerL
// Constructs and initialises a new instance of the audio player utility for
// playing sampled audio data from a file. The audio data must be in a supported
// format (e.g. WAV and AU).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDrmPlayerUtility* CDrmPlayerUtility::NewFilePlayerL(
    const TDesC& aFileName,
    MDrmAudioPlayerCallback& aCallback,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    CDrmPlayerUtility* self = new(ELeave) CDrmPlayerUtility();
    CleanupStack::PushL(self);
    self->iProperties = CDrmAudioPlayerAdaptation::NewFilePlayerL(aFileName, aCallback, aPriority, aPref);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::NewDesPlayerL
// Constructs and initialises a new instance of the audio player utility for
// playing sampled audio data from a descriptor.  The audio data must be in a
// supported format (e.g. WAV and AU).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDrmPlayerUtility* CDrmPlayerUtility::NewDesPlayerL(
    const TDesC8& aData,
    MDrmAudioPlayerCallback& aCallback,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    CDrmPlayerUtility* self = new(ELeave) CDrmPlayerUtility();
    CleanupStack::PushL(self);
    self->iProperties = CDrmAudioPlayerAdaptation::NewDesPlayerL( aData, aCallback, aPriority, aPref );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::NewDesPlayerReadOnlyL
// Constructs and initialises a new instance of the audio player utility for
// playing sampled audio data from a read only descriptor.  The audio data must
// be in a supported format (e.g. WAV and AU).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDrmPlayerUtility* CDrmPlayerUtility::NewDesPlayerReadOnlyL(
    const TDesC8& aData,
    MDrmAudioPlayerCallback& aCallback,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    CDrmPlayerUtility* self = new(ELeave) CDrmPlayerUtility();
    CleanupStack::PushL(self);
    self->iProperties = CDrmAudioPlayerAdaptation::NewDesPlayerReadOnlyL(aData, aCallback, aPriority, aPref);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
EXPORT_C CDrmPlayerUtility::~CDrmPlayerUtility()
    {
    delete iProperties;
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::Play
// Begins playback of the initialised audio sample at the current volume and
// priority levels.  When playing of the audio sample is complete, successfully
// or otherwise, the callback function MMdaAudioPlayerCallback::MapcPlayComplete()
// is called.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::Play()
    {
    iProperties->Play();
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::Stop
// Stops playback of the audio sample as soon as possible.  If the audio sample
// is playing, playback is stopped as soon as possible. If playback is already
// complete, nothing further happens as a result of calling this function. The
// callback function MMdaAudioPlayerCallback::MapcPlayComplete() is not called.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::Stop()
    {
    iProperties->Stop();
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::Pause
// Pauses the playback of the audio clip.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::Pause()
    {
    return iProperties->Pause();
    }


// -----------------------------------------------------------------------------
// CDrmPlayerUtility::SetVolume
// Changes the current playback volume to a specified value.  The volume can be
// changed before or during playback and is effective immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::SetVolume(
    TInt aVolume)
    {
    iProperties->SetVolume(aVolume);
    }


// -----------------------------------------------------------------------------
// CDrmPlayerUtility::SetRepeats
// Sets the number of times the audio sample is to be repeated during the playback
// operation.  A period of silence can follow each playing of the sample. The
// audio sample can be repeated indefinitely.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::SetRepeats(
    TInt aRepeatNumberOfTimes,
    const TTimeIntervalMicroSeconds& aTrailingSilence)
    {
    iProperties->SetRepeats(aRepeatNumberOfTimes,aTrailingSilence);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::SetVolumeRamp
// Defines the period over which the volume level is to rise smoothly from nothing
// to the normal volume level.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::SetVolumeRamp(
    const TTimeIntervalMicroSeconds& aRampDuration)
    {
    iProperties->SetVolumeRamp(aRampDuration);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::Duration
// Returns the duration of the audio sample.  The function raises a
// CMdaAudioPlayerUtility 1 panic if the audio player utility is not initialised.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TTimeIntervalMicroSeconds& CDrmPlayerUtility::Duration()
    {
    return iProperties->Duration();
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::MaxVolume
// Returns an integer representing the maximum volume.  This is the maximum value
// which can be passed to CMdaAudioPlayerUtility::SetVolume().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::MaxVolume()
    {
    return iProperties->MaxVolume();
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::OpenFileL
// Opens an audio clip from a file.  The audio data must be in a supported
// format (for example, WAV or AU).  This function leaves with KErrInUse if there
// is a previous open statement awaiting notification of completion.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::OpenFileL(
    const TDesC &aFileName)
    {
    iProperties->OpenFileL(aFileName);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::OpenFileL
// Opens an audio clip from a file.  The audio data must be in a supported
// format (for example, WAV or AU).  This function leaves with KErrInUse if there
// is a previous open statement awaiting notification of completion.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::OpenFileL(
    const RFile& aFile)
    {
    iProperties->OpenFileL(aFile);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::OpenFileL
// Opens an audio clip from a file.  The audio data must be in a supported
// format (for example, WAV or AU).  This function leaves with KErrInUse if there
// is a previous open statement awaiting notification of completion.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::OpenFileL(
#ifdef SYMBIAN_CAF_V2
    const TMMSource& aSource
#else
    const TMMSource& /*aSource*/
#endif
    )
    {
#ifdef SYMBIAN_CAF_V2
    iProperties->OpenFileL(aSource);
#else
    User::Leave(KErrNotSupported);
#endif
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::OpenDesL
// Opens an audio clip from a descriptor.  The audio data must be in a supported
// format (for example, WAV or AU).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::OpenDesL(
    const TDesC8& aDescriptor)
    {
    iProperties->OpenDesL(aDescriptor);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::OpenUrlL
// Opens an audio clip from a URL.  The audio data must be in a supported format
// (for example, WAV or AU).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::OpenUrlL(
    const TDesC& aUrl,
    TInt aIapId,
    const TDesC8& aMimeType)
    {
    iProperties->OpenUrlL(aUrl,aIapId,aMimeType);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::Close
// Closes the current audio clip (allowing another clip to be opened).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::Close()
   {
   iProperties->Close();
   }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::GetPosition
// Returns the current playback position in microseconds from the start
// of the clip.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::GetPosition(
    TTimeIntervalMicroSeconds& aPosition)
    {
    return iProperties->GetPosition(aPosition);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::SetPosition
// Sets the current playback position in microseconds from the start of the clip.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::SetPosition(
    const TTimeIntervalMicroSeconds& aPosition)
    {
    iProperties->SetPosition(aPosition);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::SetPriority
// Sets the priority for playback. This is used to arbitrate between multiple
// objects trying to access a single sound device.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::SetPriority(
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    return iProperties->SetPriority(aPriority,aPref);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::GetVolume
// Returns the current playback volume.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::GetVolume(
    TInt& aVolume)
    {
    return iProperties->GetVolume(aVolume);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::GetNumberOfMetaDataEntries
// Returns the number of meta data entries in the current audio clip.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::GetNumberOfMetaDataEntries(
    TInt& aNumEntries)
    {
    return iProperties->GetNumberOfMetaDataEntries(aNumEntries);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::GetMetaDataEntryL
// Returns the requested meta data entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CMMFMetaDataEntry* CDrmPlayerUtility::GetMetaDataEntryL(
    TInt aMetaDataIndex)
    {
    return iProperties->GetMetaDataEntryL(aMetaDataIndex);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::SetPlayWindow
// Sets the current playback window.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::SetPlayWindow(
    const TTimeIntervalMicroSeconds& aStart,
    const TTimeIntervalMicroSeconds& aEnd)
    {
    return  iProperties->SetPlayWindow(aStart,aEnd);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::ClearPlayWindow
// Clears the current playback window.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::ClearPlayWindow()
    {
    return iProperties->ClearPlayWindow();
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::SetBalance
// Sets the current playback balance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::SetBalance(
    TInt aBalance)
    {
    return iProperties->SetBalance(aBalance);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::GetBalance
// Returns the current playback balance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::GetBalance(
    TInt& aBalance)
    {
    return iProperties->GetBalance(aBalance);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::GetBitRate
// Returns the bit rate of the audio clip
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::GetBitRate(
    TUint& aBitRate)
    {
    return iProperties->GetBitRate(aBitRate);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::RegisterForAudioLoadingNotification
// Registers callback object to receive notifications of audio loading/rebuffering.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::RegisterForAudioLoadingNotification(
    MAudioLoadingObserver& aCallback)
    {
    iProperties->RegisterForAudioLoadingNotification(aCallback);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::GetAudioLoadingProgressL
// Returns the current progress of audio loading
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::GetAudioLoadingProgressL(
    TInt& aPercentageProgress)
    {
    iProperties->GetAudioLoadingProgressL(aPercentageProgress);
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::ControllerImplementationInformationL
// Returns the controller implementation information associated with the current controller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const CMMFControllerImplementationInformation& CDrmPlayerUtility::ControllerImplementationInformationL()
    {
    return iProperties->ControllerImplementationInformationL();
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::CustomCommandSync
// Sends a synchronous custom command to the controller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::CustomCommandSync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TDes8& aDataFrom)
    {
      TInt status( KErrAccessDenied );
      status = iProperties->CustomCommandSync(aDestination,aFunction,aDataTo1,aDataTo2,aDataFrom);

    return status;
    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::CustomCommandSync
// Sends a synchronous custom command to the controller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDrmPlayerUtility::CustomCommandSync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2)
    {

      TInt status( KErrAccessDenied );
      status = iProperties->CustomCommandSync(aDestination,aFunction,aDataTo1,aDataTo2);
    return status;

    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::CustomCommandAsync
// Sends an asynchronous custom command to the controller
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::CustomCommandAsync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TDes8& aDataFrom,
    TRequestStatus& aStatus)
    {

        iProperties->CustomCommandAsync(aDestination,aFunction,aDataTo1,aDataTo2,aDataFrom,aStatus);

    }

// -----------------------------------------------------------------------------
// CDrmPlayerUtility::CustomCommandAsync
// Sends an asynchronous custom command to the controller
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDrmPlayerUtility::CustomCommandAsync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TRequestStatus& aStatus)
    {

        iProperties->CustomCommandAsync(aDestination,aFunction,aDataTo1,aDataTo2,aStatus);

    }
//=========================================================================================================================




// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::NewL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMMFDrmAudioPlayerUtility* CMMFDrmAudioPlayerUtility::NewL(
    MDrmAudioPlayerCallback& aCallback,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    CMMFDrmAudioPlayerUtility* self = new(ELeave) CMMFDrmAudioPlayerUtility(aCallback,EPriorityStandard);
    CleanupStack::PushL(self);
    self->ConstructL(aPriority, aPref);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::NewFilePlayerL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMMFDrmAudioPlayerUtility* CMMFDrmAudioPlayerUtility::NewFilePlayerL(
    const TDesC& aFileName,
    MDrmAudioPlayerCallback& aCallback,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    CMMFDrmAudioPlayerUtility* self = new(ELeave) CMMFDrmAudioPlayerUtility(aCallback,EPriorityStandard);
    CleanupStack::PushL(self);
    self->ConstructL(aFileName,aPriority,aPref);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::NewDesPlayerL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMMFDrmAudioPlayerUtility* CMMFDrmAudioPlayerUtility::NewDesPlayerL(
    const TDesC8& aData,
    MDrmAudioPlayerCallback& aCallback,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    CMMFDrmAudioPlayerUtility* self = new(ELeave) CMMFDrmAudioPlayerUtility(aCallback,EPriorityStandard);
    CleanupStack::PushL(self);
    self->ConstructL(aData,aPriority,aPref);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::NewDesPlayerReadOnlyL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMMFDrmAudioPlayerUtility* CMMFDrmAudioPlayerUtility::NewDesPlayerReadOnlyL(
    const TDesC8& aData,
    MDrmAudioPlayerCallback& aCallback,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    CMMFDrmAudioPlayerUtility* self = new(ELeave) CMMFDrmAudioPlayerUtility(aCallback,EPriorityStandard);
    CleanupStack::PushL(self);
    self->ConstructReadOnlyL(aData,aPriority,aPref);
    CleanupStack::Pop(self);
    return self;
    }


// Destructor
CMMFDrmAudioPlayerUtility::~CMMFDrmAudioPlayerUtility()
    {
    Stop();
    Cancel();
    delete iDrm;
    delete iMetaDataBuffer;
    if(iDrmSession)
      {
      iDrmSession->Disconnect();
      iDrmSession->Close();
      delete iDrmSession;
      iDrmSession = NULL;
      }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::CMMFDrmAudioPlayerUtility
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMMFDrmAudioPlayerUtility::CMMFDrmAudioPlayerUtility(
    MDrmAudioPlayerCallback& aCallback,
    TInt aActivePriority)
    : CActive(aActivePriority),
    iCallback(aCallback)
    {
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::ConstructL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::ConstructL(
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    ConnectL();
    TDataStruct theStruct;

    theStruct.iPriority = aPriority;
    theStruct.iPref = aPref;

    TDataStructPckgBuf pckg(theStruct);
    iState = EInitializing;
    iDrmSession->Send(EDRMPlayServNewPlayerL,TIpcArgs(&iAsyncCallback,&pckg,&iErrorDurationStruct));
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::ConstructL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::ConstructL(
    const TDesC& aFileName,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    ConnectL();

    iDrmSession->Send(EDRMPlayServSetPriorityPreference,TIpcArgs(aPriority,aPref));
    TDataStructPckgBuf pckgBuf;
    TDataStruct &theStruct = pckgBuf();



    theStruct.iPriority = aPriority;
    theStruct.iPref = aPref;
    iFileName = aFileName;

    iState = EInitializing;
    if (!IsActive())
        {
        iDrmSession->Send(EDRMPlayServNewFilePlayerL,TIpcArgs(&iAsyncCallback,&pckgBuf,&iErrorDurationStruct,&iFileName),iStatus);
        SetActive();
        }
    else
        {
        User::Leave(KErrNotReady);
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::ConstructL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::ConstructL(
    const TDesC8& aData,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    ConnectL();
    TDataStruct theStruct;

    theStruct.iData = aData;
    iDrmSession->Send(EDRMPlayServSetPriorityPreference,TIpcArgs(aPriority,aPref));

    TDataStructPckgBuf pckg(theStruct);

    iState = EInitializing;
    if (!IsActive())
        {
        iDrmSession->Send(EDRMPlayServNewDesPlayerL,TIpcArgs(&iAsyncCallback,&pckg,&iErrorDurationStruct),iStatus);
        SetActive();
        }
    else
        {
        User::Leave(KErrNotReady);
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::ConstructReadOnlyL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::ConstructReadOnlyL(
    const TDesC8& aData,
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    ConnectL();
    TDataStruct theStruct;

    theStruct.iData = aData;
    iDrmSession->Send(EDRMPlayServSetPriorityPreference,TIpcArgs(aPriority,aPref));

    TDataStructPckgBuf pckg(theStruct);

    iState = EInitializing;
    if (!IsActive())
        {
        iDrmSession->Send(EDRMPlayServNewDesPlayerReadOnlyL,TIpcArgs(&iAsyncCallback,&pckg,&iErrorDurationStruct),iStatus);
        SetActive();
        }
    else
        {
        User::Leave(KErrNotReady);
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::ConnectL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::ConnectL()
    {
    CActiveScheduler::Add(this);
    iDrmSession = new(ELeave) RDrmSession();
    User::LeaveIfError( iDrmSession->Connect() );
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::OpenFileL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::OpenFileL(
    const TDesC& aFileName)
    {
    iState = EInitializing;
    if (!IsActive())
        {
        iDrmSession->Send(EDRMPlayServOpenFile,TIpcArgs(&iAsyncCallback,&aFileName,&iErrorDurationStruct),iStatus);
        SetActive();
        }
    else
        {
        User::Leave(KErrNotReady);
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::OpenFileL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::OpenFileL(
    const RFile& aFile)
    {
    TIpcArgs ipcArgs;
    aFile.TransferToServer(ipcArgs, 1, 3);
    ipcArgs.Set(0,&iAsyncCallback);
    ipcArgs.Set(2,&iErrorDurationStruct);
    iState = EInitializing;
    if (!IsActive())
        {
        iDrmSession->Send(EDRMPlayServOpenFileByHandle,ipcArgs,iStatus);
        SetActive();
        }
    else
        {
        User::Leave(KErrNotReady);
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::OpenFileL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
#ifdef SYMBIAN_CAF_V2
void CMMFDrmAudioPlayerUtility::OpenFileL(const TMMSource& aSource)
    {
    if (aSource.SourceType()==KUidMMFileHandleSource)
        {
#ifdef SYMBIAN_SECURITY_CAF_RFILE_HANDLE
        RFile& fileHandle = static_cast<const TMMFileHandleSource&>(aSource).Handle();
        OpenFileL(fileHandle);
#else
        User::Leave(KErrNotSupported);
#endif
        }
    if (aSource.SourceType()==KUidMMFileSource)
        {
        const TDesC& fileName = static_cast<const TMMFileSource&>(aSource).Name();
        OpenFileL(fileName);
        }
    }
#endif


// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::OpenDesL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::OpenDesL(
    const TDesC8& aDescriptor)
    {
    iStatus = EInitializing;
    if (!IsActive())
        {
        iDrmSession->Send(EDRMPlayServOpenDes,TIpcArgs(&iAsyncCallback,&aDescriptor,&iErrorDurationStruct),iStatus);
        SetActive();
        }
    else
        {
        User::Leave(KErrNotReady);
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::OpenUrlL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::OpenUrlL(
    const TDesC& aUrl,
    const TInt aIapId,
    const TDesC8& aMimeType)
    {
    TUrlStruct theStruct;

    theStruct.iUrl = aUrl;
    theStruct.iIapId = aIapId;
    theStruct.iMimeType = aMimeType;

    TUrlStructPckgBuf pckg(theStruct);
    iStatus = EInitializing;
    if (!IsActive())
        {
        iDrmSession->Send(EDRMPlayServOpenUrl,TIpcArgs(&iAsyncCallback,&pckg,&iErrorDurationStruct),iStatus);
        SetActive();
        }
    else
        {
        User::Leave(KErrNotReady);
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::Play
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::Play()
    {
    if (iState != EPlaying)
        {
        iState = EPlaying;
        if (!IsActive())
            {
            SetActive();
            iDrmSession->Send(EDRMPlayServPlay,TIpcArgs(&iAsyncCallback,0,&iErrorDurationStruct),iStatus);
            }
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::Stop
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::Stop()
    {
    iDrmSession->Send(EDRMPlayServStop);
    if (IsActive())
            {
            Cancel();
            }

    iState = EIdle;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::Pause
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::Pause()
    {
    iDrmSession->Send(EDRMPlayServPause);
    iState = EPaused;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::Close
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::Close()
    {
    if (IsActive())
            {
                Cancel();
            }
    iDrmSession->Send(EDRMPlayServClose);
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::SetVolume
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::SetVolume(
    TInt aVolume)
    {
    iDrmSession->Send(EDRMPlayServSetVolume,TIpcArgs(aVolume));
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::SetRepeats
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::SetRepeats(
    TInt aRepeatNumberOfTimes,
    const TTimeIntervalMicroSeconds& aTrailingSilence)
    {

    TPckgTTimeIntervalMicroSeconds trailingSilencePckg(aTrailingSilence);
    iDrmSession->Send(EDRMPlayServSetRepeats,TIpcArgs(aRepeatNumberOfTimes,&trailingSilencePckg));
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::SetVolumeRamp
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::SetVolumeRamp(
    const TTimeIntervalMicroSeconds& aRampDuration)
    {

    TPckgTTimeIntervalMicroSeconds volumeRampPckg(aRampDuration);
    iDrmSession->Send(EDRMPlayServSetVolumeRamp,TIpcArgs(&volumeRampPckg));
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::SetPriority
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::SetPriority(
    TInt aPriority,
    TMdaPriorityPreference aPref)
    {
    return iDrmSession->Send(EDRMPlayServSetPriority,TIpcArgs(aPriority,aPref));
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::Duration
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TTimeIntervalMicroSeconds& CMMFDrmAudioPlayerUtility::Duration()
    {
    TPckgBufTTimeIntervalMicroSeconds pckg;
    iDrmSession->Send(EDRMPlayServDuration,TIpcArgs(&pckg));
    return pckg();
    }


// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::MaxVolume
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::MaxVolume()
    {
    return iDrmSession->Send(EDRMPlayServMaxVolume);
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::GetPosition
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::GetPosition(
    TTimeIntervalMicroSeconds& aPosition)
    {
    TPckgTTimeIntervalMicroSeconds thePositionPckg(aPosition);
    TInt ret = iDrmSession->Send(EDRMPlayServGetPosition,TIpcArgs(&thePositionPckg));
    aPosition = thePositionPckg();
    return ret;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::SetPosition
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::SetPosition(
    const TTimeIntervalMicroSeconds& aPosition)
    {
    TSetPositionStruct sendPckg;
    sendPckg.iPosition = aPosition;

    TSetPositionStructBuf pckg(sendPckg);
    iDrmSession->Send(EDRMPlayServSetPosition,TIpcArgs(&pckg));
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::GetVolume
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::GetVolume(TInt& aVolume)
    {
    TInt theReturnValue;
    TPckgBufTInt theVolume;
    theReturnValue = iDrmSession->Send(EDRMPlayServGetVolume,TIpcArgs(&theVolume));
    aVolume = theVolume();
    return theReturnValue;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::GetNumberOfMetaDataEntries
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::GetNumberOfMetaDataEntries(
    TInt& aNumEntries)
    {
    TPckgBufTInt theNumEntries;
    TInt retValue = iDrmSession->Send(EDRMPlayServGetNumberOfMetaDataEntries,TIpcArgs(&theNumEntries));
    aNumEntries = theNumEntries();
    return retValue;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::GetMetaDataEntryL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMMFMetaDataEntry* CMMFDrmAudioPlayerUtility::GetMetaDataEntryL(
    TInt aMetaDataIndex)
    {
    TBuf8<100> data;
    iDrmSession->Send(EDRMPlayServGetMetaDataEntry,TIpcArgs(aMetaDataIndex,&data));
    delete iMetaDataBuffer;
    iMetaDataBuffer = NULL;
    iMetaDataBuffer = CBufFlat::NewL(32);
    HBufC8* theData = data.Alloc();
    iMetaDataBuffer->InsertL(0,theData->Des());
    RBufReadStream s;
    s.Open(*iMetaDataBuffer);
    CleanupClosePushL(s);
    CMMFMetaDataEntry* theMetaDataEntry  = CMMFMetaDataEntry::NewL();
    theMetaDataEntry->InternalizeL(s);
    CleanupStack::PopAndDestroy();//s
    return theMetaDataEntry;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::SetPlayWindow
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::SetPlayWindow(
    const TTimeIntervalMicroSeconds& aPlayStart,
    const TTimeIntervalMicroSeconds& aPlayEnd)
    {
    TPlayWindowStruct sendPckg;
    sendPckg.iPlayStart = aPlayStart;
    sendPckg.iPlayEnd = aPlayEnd;

    TPlayWindowStructBuf pckg(sendPckg);

    return iDrmSession->Send(EDRMPlayServSetPlayWindow,TIpcArgs(&pckg));
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::ClearPlayWindow
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::ClearPlayWindow()
    {
    return iDrmSession->Send(EDRMPlayServClearPlayWindow);
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::SetBalance
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::SetBalance(
    TInt aBalance)
    {
    return iDrmSession->Send(EDRMPlayServSetBalance,TIpcArgs(aBalance));
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::GetBitRate
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::GetBitRate(
    TUint& aBitRate)
    {
    TPckgBufTUint pckg;
    TInt retValue = iDrmSession->Send(EDRMPlayServGetBitRate,TIpcArgs(&pckg));
    aBitRate = pckg();
    return retValue;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::ControllerImplementationInformationL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CMMFControllerImplementationInformation& CMMFDrmAudioPlayerUtility::ControllerImplementationInformationL()
    {
    TUid theUid;
    iDrmSession->Send(EDRMPlayServControllerImplementationInformation,TIpcArgs(&theUid));
    iControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(theUid);
    return *iControllerImplementationInformation;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::GetAudioLoadingProgressL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::GetAudioLoadingProgressL(
    TInt& aPercentageProgress)
    {
    TPckgBufTInt pckg;
    iDrmSession->Send(EDRMPlayServGetAudioLoadingProgress,TIpcArgs(&pckg));
    aPercentageProgress = pckg();
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::CustomCommandSync
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::CustomCommandSync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TDes8& aDataFrom)
    {
    TInt theReturnValue(KErrPermissionDenied);

    TPckgCustomCommand thePckg;
    thePckg().iDestination = aDestination;
    thePckg().iFunction = aFunction;
/*
    RDebug::Print(_L("CMMFDrmAudioPlayerUtility::CustomCommandSync:IFId[%x]DestHndl[%x]Fn[%d]RetDesLen[%d]"),
                                thePckg().iDestination().InterfaceId(),
                                thePckg().iDestination().DestinationHandle(),
                                thePckg().iFunction,
                                aDataFrom.Length());
*/
     // Only Pass 'Allowed CustomCommand
     if ( IsValidCustomCommandDestination(aDestination().InterfaceId(), aDataTo1) )
        {
        theReturnValue = iDrmSession->Send( EDRMPlayServCustomCommandSyncWithReturn,
                                            TIpcArgs(&thePckg, &aDataTo1, &aDataTo2, &aDataFrom));
        }
    return theReturnValue;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::CustomCommandSync
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::CustomCommandSync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2)
    {
    TInt theReturnValue(KErrPermissionDenied);

    TPckgCustomCommand thePckg;
    thePckg().iDestination = aDestination;
    thePckg().iFunction = aFunction;
/*
    RDebug::Print(_L("CMMFDrmAudioPlayerUtility::CustomCommandSync:IFId[%x]DestHndl[%x]Fn[%d]"),
                                thePckg().iDestination().InterfaceId(),
                                thePckg().iDestination().DestinationHandle(),
                                thePckg().iFunction);
*/
    // Only Pass 'Allowed CustomCommand
    if ( IsValidCustomCommandDestination(aDestination().InterfaceId(), aDataTo1) )
        {
        theReturnValue = iDrmSession->Send( EDRMPlayServCustomCommandSyncWithoutReturn,
                                            TIpcArgs(&thePckg, &aDataTo1, &aDataTo2));
        }
    return theReturnValue;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::CustomCommandAsync
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::CustomCommandAsync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TDes8& aDataFrom,
    TRequestStatus& aStatus )
    {
    TPckgCustomCommand thePckg;
    thePckg().iDestination = aDestination;
    thePckg().iFunction = aFunction;
    TInt status(KErrPermissionDenied);
/*
    RDebug::Print(_L("CMMFDrmAudioPlayerUtility::CustomCommandAsync:IFId[%x]DestHndl[%x]Fn[%d]Data1[Len:%d]Data2Len[Len:%d]RetDesLen[Len:%d:MaxLen:%d]"),
                                thePckg().iDestination().InterfaceId(),
                                thePckg().iDestination().DestinationHandle(),
                                thePckg().iFunction,
                                aDataTo1.Length(),
                                aDataTo2.Length(),
                                aDataFrom.Length(),
                                aDataFrom.MaxLength() );
*/
    // Only Pass 'Allowed CustomCommand
    if ( IsValidCustomCommandDestination(aDestination().InterfaceId(), aDataTo1) )
        {
        // Send in two steps, data in first step and async request in second step
        // othewise thePckg will become invalid when tried to read from server
        status = iDrmSession->Send( EDRMPlayServCustomCommandAsyncWithReturnStep1,
                                    TIpcArgs(&thePckg, &aDataTo1, &aDataTo2) );
        if ( status == KErrNone )
            {
            // Pass descriptor place holder to get data back in the async call
            iDrmSession->Send( EDRMPlayServCustomCommandAsyncWithReturnStep2,
                               TIpcArgs(&aDataFrom),
                               aStatus );
            }
#ifdef _DEBUG
        else
            {
            RDebug::Print(_L("CMMFDrmAudioPlayerUtility::CustomCommandAsync:IFId[%x]DestHndl[%x]Fn[%d]Data1[Len:%d]Data2Len[Len:%d]RetDesLen[Len:%d:MaxLen:%d]DRM Proxy CustCmdAsyncWRet Step1[%d]"),
                                        thePckg().iDestination().InterfaceId(),
                                        thePckg().iDestination().DestinationHandle(),
                                        thePckg().iFunction,
                                        aDataTo1.Length(),
                                        aDataTo2.Length(),
                                        aDataFrom.Length(),
                                        aDataFrom.MaxLength(),
                                        status );
            }
#endif // _DEBUG
        }
     if ( status != KErrNone )
        {
        TRequestStatus* s = &aStatus;
        User::RequestComplete( s , status );
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::CustomCommandAsync
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::CustomCommandAsync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TRequestStatus& aStatus)
    {
    TPckgCustomCommand thePckg;
    thePckg().iDestination = aDestination;
    thePckg().iFunction = aFunction;

    TInt status(KErrPermissionDenied);
/*
    RDebug::Print(_L("CMMFDrmAudioPlayerUtility::CustomCommandAsync:IFId[%x]DestHndl[%x]Fn[%d]Data1[Len:%d]Data2Len[Len:%d]"),
                                thePckg().iDestination().InterfaceId(),
                                thePckg().iDestination().DestinationHandle(),
                                thePckg().iFunction,
                                aDataTo1.Length(),
                                aDataTo2.Length() );
*/
     // Only Pass 'Allowed CustomCommand
    if ( IsValidCustomCommandDestination(aDestination().InterfaceId(), aDataTo1) )
        {
        // Send in two steps, data in first step and async request in second step
        // othewise thePckg will become invalid when tried to read from server
        status = iDrmSession->Send( EDRMPlayServCustomCommandAsyncWithoutReturnStep1,
                                    TIpcArgs(&thePckg, &aDataTo1, &aDataTo2) );
        if ( status == KErrNone )
            {
            iDrmSession->Send( EDRMPlayServCustomCommandAsyncWithoutReturnStep2,
                               TIpcArgs(),
                               aStatus );
            }
#ifdef _DEBUG
        else
            {
            RDebug::Print(_L("CMMFDrmAudioPlayerUtility::CustomCommandAsync:IFId[%x]DestHndl[%x]Fn[%d]Data1[Len:%d]Data2Len[Len:%d]DRM Proxy CustCmdAsyncWoRet Step1[%d]"),
                                        thePckg().iDestination().InterfaceId(),
                                        thePckg().iDestination().DestinationHandle(),
                                        thePckg().iFunction,
                                        aDataTo1.Length(),
                                        aDataTo2.Length(),
                                        status );
            }
#endif // _DEBUG
        }
     if ( status != KErrNone )
        {
        TRequestStatus* s = &aStatus;
        User::RequestComplete( s , status );
        }
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::GetBalance
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDrmAudioPlayerUtility::GetBalance(
    TInt& aBalance)
    {
    TPckgBufTInt theBalance;

    TInt retValue = iDrmSession->Send(EDRMPlayServGetBalance,TIpcArgs(&theBalance));
    aBalance = theBalance();
    return retValue;
    }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::RegisterForAudioLoadingNotification
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::RegisterForAudioLoadingNotification(
    MAudioLoadingObserver& aLoadingObserver)
    {
    iAsyncCallback() = ELoadingStarted;
    iLoadingObserver = &aLoadingObserver;
    iDrmSession->Send(EDRMPlayServRegisterForAudioLoadingNotification,TIpcArgs(&iAsyncCallback));
    }


// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::DoCancel
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::DoCancel()
   {
   }

// -----------------------------------------------------------------------------
// CMMFDrmAudioPlayerUtility::RunL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDrmAudioPlayerUtility::RunL()
   {
    DEP_PRN1(_L("CMMFDrmAudioPlayerUtility::RunL:iStatus[%d]"), iStatus.Int());

    if (iState == EPaused)
        {
        return;
        }
    if ( iStatus.Int() == KErrCancel )
        return;
    switch (iAsyncCallback())
        {
        case EInitComplete:
            if (iState != EIdle)
                {
                DEP_PRN0(_L("CMMFDrmAudioPlayerUtility::RunL - InitComplete"));
                iState = EIdle;
                iCallback.MdapcInitComplete(iErrorDurationStruct().iError, iErrorDurationStruct().iDuration);
                }
        break;

        case EPlayComplete:
            DEP_PRN0(_L("CMMFDrmAudioPlayerUtility::RunL - PlayComplete"));
            iState = EIdle;
            iCallback.MdapcPlayComplete(iErrorDurationStruct().iError);
        break;

        case ELoadingStarted:
            DEP_PRN0(_L("CMMFDrmAudioPlayerUtility::RunL - MaloLoadingStarted"));

            iDrmSession->Send(EDRMPlayServRegisterForAudioLoadingNotification,TIpcArgs(&iAsyncCallback),iStatus);
            SetActive();
            iLoadingObserver->MaloLoadingStarted();
        break;

        case ELoadingComplete:
            DEP_PRN0(_L("CMMFDrmAudioPlayerUtility::RunL - MaloLoadingComplete"));

            iAsyncCallback() = ELoadingStarted;
            iLoadingObserver->MaloLoadingComplete();
        break;

        default:
        break;
        }
   }

TBool CMMFDrmAudioPlayerUtility::IsValidCustomCommandDestination(TUid aDestinationUid, const TDesC8& aParam)
    {
    TBool retValue(ETrue);
    if (aDestinationUid == KUidInterfaceMMFDRMControl)
        {
        retValue = EFalse;
        }
    else if ( aDestinationUid == KUidCustomInterfaceBuilderImpl )
        {
             TRAPD(err, retValue = CheckCustomInterfaceBuilderImplL(aParam));
             if((err != KErrNone) || (retValue == EFalse))
                retValue = EFalse;
        }
    return retValue;
    }

TBool CMMFDrmAudioPlayerUtility::CheckCustomInterfaceBuilderImplL(const TDesC8& aParam)
{
    TBool retValue(ETrue);
    RDesReadStream stream(aParam);
    CleanupClosePushL(stream);
    TUid paramUid;
    paramUid.iUid = stream.ReadInt32L();
    CleanupStack::PopAndDestroy(&stream);
    if ( paramUid == KUidAudioOutput )
         {
         retValue = EFalse;
         }
   return retValue;
}

// End of file

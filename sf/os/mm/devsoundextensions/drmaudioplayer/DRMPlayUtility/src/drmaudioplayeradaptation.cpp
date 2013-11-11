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

#include <e32std.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <mmfaudio.h> // MAudioLoadingObserver
#include <AudioOutputMessageTypes.h>
#include "drmaudioplayeradaptation.h"

// This is the UID defined by custom interface builder
const TUid KUidCustomInterfaceBuilderImpl = {0x10207A8E};

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CDrmAudioPlayerAdaptation* CDrmAudioPlayerAdaptation::NewL( MDrmAudioPlayerCallback& aCallback,
                                                            TInt aPriority,
                                                            TMdaPriorityPreference aPref )
    {
    CDrmAudioPlayerAdaptation* self = new(ELeave) CDrmAudioPlayerAdaptation( aCallback );
    CleanupStack::PushL(self);
    self->ConstructL( aPriority, aPref );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CDrmAudioPlayerAdaptation::CDrmAudioPlayerAdaptation(MDrmAudioPlayerCallback& aCallback)
    :iCallback(&aCallback)
    {}


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::ConstructL( TInt aPriority, TMdaPriorityPreference aPref )
    {

    if( iMdaPlayUtil )
        {
        delete iMdaPlayUtil;
        iMdaPlayUtil = NULL;
        }

    iMdaPlayUtil = CMdaAudioPlayerUtility::NewL( *this,
                                                 aPriority,
                                                 aPref);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CDrmAudioPlayerAdaptation::~CDrmAudioPlayerAdaptation()
    {

    // Delete DMA play utility
    if( iMdaPlayUtil )
        iMdaPlayUtil->Close();
    delete iMdaPlayUtil;
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CDrmAudioPlayerAdaptation* CDrmAudioPlayerAdaptation::NewFilePlayerL( const TDesC& aFileName,
                                                MDrmAudioPlayerCallback& aCallback,
                                                TInt aPriority,
                                                TMdaPriorityPreference aPref)
    {

    CDrmAudioPlayerAdaptation* self = new(ELeave) CDrmAudioPlayerAdaptation( aCallback );
    CleanupStack::PushL(self);
    self->ConstructNewFilePlayerL( aFileName, aPriority, aPref );
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::ConstructNewFilePlayerL( const TDesC& aFileName,
                                                            TInt aPriority,
                                                            TMdaPriorityPreference aPref )
    {

    if( iMdaPlayUtil )
        {
        delete iMdaPlayUtil;
        iMdaPlayUtil = NULL;
        }
    iMdaPlayUtil = CMdaAudioPlayerUtility::NewFilePlayerL( aFileName,
                                                                   *this,
                                                                   aPriority,
                                                                   aPref);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CDrmAudioPlayerAdaptation* CDrmAudioPlayerAdaptation::NewDesPlayerL( const TDesC8& aData,
                                                MDrmAudioPlayerCallback& aCallback,
                                                TInt aPriority,
                                                TMdaPriorityPreference aPref)
    {

    CDrmAudioPlayerAdaptation* self = new(ELeave) CDrmAudioPlayerAdaptation( aCallback );
    CleanupStack::PushL(self);
    self->ConstructNewDesPlayerL( aData, aPriority, aPref );
    CleanupStack::Pop(self);

    return self;
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::ConstructNewDesPlayerL( const TDesC8& aData,
                                                        TInt aPriority,
                                                        TMdaPriorityPreference aPref )
    {

    if( iMdaPlayUtil )
        {
        delete iMdaPlayUtil;
        iMdaPlayUtil = NULL;
        }
    iMdaPlayUtil = CMdaAudioPlayerUtility::NewDesPlayerL( aData,
                                                          *this,
                                                          aPriority,
                                                          aPref);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CDrmAudioPlayerAdaptation* CDrmAudioPlayerAdaptation::NewDesPlayerReadOnlyL( const TDesC8& aData,
                                                        MDrmAudioPlayerCallback& aCallback,
                                                        TInt aPriority,
                                                        TMdaPriorityPreference aPref)
    {

    CDrmAudioPlayerAdaptation* self = new(ELeave) CDrmAudioPlayerAdaptation( aCallback );
    CleanupStack::PushL(self);
    self->ConstructNewDesPlayerReadOnlyL( aData, aPriority, aPref );
    CleanupStack::Pop(self);

    return self;
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::ConstructNewDesPlayerReadOnlyL( const TDesC8& aData,
                                                        TInt aPriority,
                                                        TMdaPriorityPreference aPref )
    {

    delete this->iMdaPlayUtil;
    this->iMdaPlayUtil = NULL;
    this->iMdaPlayUtil = CMdaAudioPlayerUtility::NewDesPlayerReadOnlyL( aData,
                                                                  *this,
                                                                  aPriority,
                                                                  aPref);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::OpenFileL(const TDesC &aFileName)
    {
    iMdaPlayUtil->OpenFileL( aFileName );
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::OpenFileL(const RFile& aFile)
    {
    iMdaPlayUtil->OpenFileL( aFile );
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::OpenFileL(const TMMSource& aSource)
    {
    iMdaPlayUtil->OpenFileL( aSource );
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::OpenDesL(const TDesC8& aDes)
    {
    iMdaPlayUtil->OpenDesL(aDes);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::OpenUrlL( const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType )
    {
    iMdaPlayUtil->OpenUrlL( aUrl, aIapId, aMimeType);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::Play()
    {
    iMdaPlayUtil->Play();
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::Stop()
    {
    iMdaPlayUtil->Stop();
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::SetVolume( const TInt aVolume )
    {
    iMdaPlayUtil->SetVolume( aVolume );
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::SetRepeats( TInt aRepeatNumberOfTimes,
                                            const TTimeIntervalMicroSeconds& aTrailingSilence )
    {
    iMdaPlayUtil->SetRepeats( aRepeatNumberOfTimes, aTrailingSilence );
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::SetVolumeRamp( const TTimeIntervalMicroSeconds& aRampDuration )
    {
    iMdaPlayUtil->SetVolumeRamp( aRampDuration );
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
const TTimeIntervalMicroSeconds& CDrmAudioPlayerAdaptation::Duration()
    {
    return iMdaPlayUtil->Duration();
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::MaxVolume()
    {
    return iMdaPlayUtil->MaxVolume();
    }


// API Additions since version 7.0

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::Pause()
    {
    return iMdaPlayUtil->Pause();
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::Close()
    {
    iMdaPlayUtil->Close();
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::GetPosition(TTimeIntervalMicroSeconds& aPosition)
    {
    return iMdaPlayUtil->GetPosition(aPosition);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::SetPosition( const TTimeIntervalMicroSeconds& aPosition )
    {
    iMdaPlayUtil->SetPosition(aPosition);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::SetPriority( TInt aPriority, TMdaPriorityPreference aPref )
    {
    return iMdaPlayUtil->SetPriority( aPriority, aPref );
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::GetVolume(TInt& aVolume)
    {
    return iMdaPlayUtil->GetVolume(aVolume);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::GetNumberOfMetaDataEntries( TInt& aNumEntries )
    {
    return iMdaPlayUtil->GetNumberOfMetaDataEntries( aNumEntries);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CMMFMetaDataEntry* CDrmAudioPlayerAdaptation::GetMetaDataEntryL( const TInt aMetaDataIndex )
    {
    return iMdaPlayUtil->GetMetaDataEntryL(aMetaDataIndex);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
                                                const TTimeIntervalMicroSeconds& aEnd)
    {
    return iMdaPlayUtil->SetPlayWindow(aStart,aEnd);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::ClearPlayWindow()
    {
    return iMdaPlayUtil->ClearPlayWindow();
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::SetBalance( const TInt aBalance )
    {
    return iMdaPlayUtil->SetBalance(aBalance);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::GetBalance(TInt& aBalance)
    {
    return iMdaPlayUtil->GetBalance(aBalance);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::GetBitRate(TUint& aBitRate)
    {
    return iMdaPlayUtil->GetBitRate(aBitRate);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback)
    {
    iMdaPlayUtil->RegisterForAudioLoadingNotification(aCallback);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::GetAudioLoadingProgressL( TInt& aPercentageProgress )
    {
    iMdaPlayUtil->GetAudioLoadingProgressL(aPercentageProgress);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
const CMMFControllerImplementationInformation& CDrmAudioPlayerAdaptation::ControllerImplementationInformationL()
    {
    return iMdaPlayUtil->ControllerImplementationInformationL();
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination,
                        TInt aFunction,
                        const TDesC8& aDataTo1,
                        const TDesC8& aDataTo2,
                        TDes8& aDataFrom)
    {
    return iMdaPlayUtil->CustomCommandSync( aDestination,
                                            aFunction,
                                            aDataTo1,
                                            aDataTo2,
                                            aDataFrom );

    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CDrmAudioPlayerAdaptation::CustomCommandSync(  const TMMFMessageDestinationPckg& aDestination,
                                                    TInt aFunction,
                                                    const TDesC8& aDataTo1,
                                                    const TDesC8& aDataTo2 )
    {
    return iMdaPlayUtil->CustomCommandSync( aDestination,
                                            aFunction,
                                            aDataTo1,
                                            aDataTo2 );

    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
                                                   TInt aFunction,
                                                   const TDesC8& aDataTo1,
                                                   const TDesC8& aDataTo2,
                                                   TDes8& aDataFrom,
                                                   TRequestStatus& aStatus)
    {
    iMdaPlayUtil->CustomCommandAsync( aDestination,
                                        aFunction,
                                        aDataTo1,
                                        aDataTo2,
                                        aDataFrom,
                                        aStatus);

    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
                                                   TInt aFunction,
                                                   const TDesC8& aDataTo1,
                                                   const TDesC8& aDataTo2,
                                                   TRequestStatus& aStatus)
    {
    iMdaPlayUtil->CustomCommandAsync( aDestination,
                                        aFunction,
                                        aDataTo1,
                                        aDataTo2,
                                        aStatus);
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TBool CDrmAudioPlayerAdaptation::IsValidCustomCommandDestination(TUid aDestinationUid, TDesC8& aParam)
    {
    TBool retValue(ETrue);

    if( aDestinationUid == KUidInterfaceMMFDRMControl)
        {
        retValue = EFalse;
        }
    else if( aDestinationUid == KUidCustomInterfaceBuilderImpl )
        {
        TRAPD(err, retValue = CheckCustomInterfaceBuilderImplL(aParam));
        if(err != KErrNone)
            {
            retValue = EFalse;
            }
        }
    return retValue;
    }
TBool CDrmAudioPlayerAdaptation::CheckCustomInterfaceBuilderImplL(const TDesC8& aParam)
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


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds& aDuration )
    {
    iCallback->MdapcInitComplete( aError, aDuration );
    }


// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CDrmAudioPlayerAdaptation::MapcPlayComplete( TInt aError )
    {
    iCallback->MdapcPlayComplete( aError );
    }

// End of File


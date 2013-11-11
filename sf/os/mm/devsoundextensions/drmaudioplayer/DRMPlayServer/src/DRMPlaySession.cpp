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
* Description:   DRM Play Session
*
*/


#include "DRMPlaySession.h"
#include <e32test.h>
#include <e32std.h>
#include <e32svr.h>
#include <mmf/common/mmfcontrollerframework.h>
#ifdef RD_TSP_CLIENT_MAPPER
#include <tspclientmapper.h>
#endif // #ifdef RD_TSP_CLIENT_MAPPER

#ifdef _DEBUG
#define DEP_PRN0(str)   RDebug::Print(str)
#define DEP_PRN1(str, v1)   RDebug::Print(str, v1)
#define DEP_PRN2(str, v1, v2)   RDebug::Print(str, v1, v2)
#else
#define DEP_PRN0(str)
#define DEP_PRN1(str, v1)
#define DEP_PRN2(str, v1, v2)
#endif // _DEBUG

// This is the UID defined by custom interface builder
const TUid KUidCustomInterfaceBuilderImpl = {0x10207A8E};

// panic the client
void CDRMPlayServerSession::PanicClient(const RMessage2 &aMessage, TInt aPanic) const
    {
    _LIT(KTxtServer,"DRM Play server");
    aMessage.Panic(KTxtServer,aPanic);
    }

CDRMPlayServerSession::CDRMPlayServerSession(CDRMPlayServer* aServer)
:iMessage2(NULL)
    {
    iServer=aServer;
    }

CDRMPlayServerSession* CDRMPlayServerSession::NewL(CDRMPlayServer* aServer, const RMessage2& aMessage )
    {
    DEP_PRN0(_L("CDRMPlayServerSession::NewL"));

    CDRMPlayServerSession* self = new(ELeave) CDRMPlayServerSession(aServer);
    CleanupStack::PushL(self);
    self->ConstructL( aMessage );
    CleanupStack::Pop(self);
    return self;
    }

#ifdef RD_TSP_CLIENT_MAPPER
void CDRMPlayServerSession::ConstructL( const RMessage2& aMessage )
#else
void CDRMPlayServerSession::ConstructL( const RMessage2& /*aMessage*/ )
#endif //RD_TSP_CLIENT_MAPPER
    {
#ifdef RD_TSP_CLIENT_MAPPER
    iTSPClientMapper = CTspClientMapper::NewL();
    // Get client process ID
    RThread clientThread;
    RProcess clientProcess;
    // Open client thread and process handles
    User::LeaveIfError( aMessage.Client(clientThread) );
    User::LeaveIfError(  clientThread.Process(clientProcess) );
    // Get process id
    iClientProcessId = clientProcess.Id();
    // Close client thread and client process handles
    clientThread.Close();
    clientProcess.Close();

    iTSPClientMapper->SetTspTargetClient(
                            CTspClientMapper::ERegisteredClients,
                            iClientProcessId );
#endif // #ifdef RD_TSP_CLIENT_MAPPER

    // Increment number of sessions in server
    iServer->iClients++;

    DEP_PRN1(_L("CDRMPlayServerSession::iClients = %d"), iServer->iClients);
    }

CDRMPlayServerSession::~CDRMPlayServerSession()
    {
    if(iMetaDataBuffer)
    {
     delete iMetaDataBuffer;
     iMetaDataBuffer = NULL;
    }
    // Delete any pending message objects
    // This will never happen, only happens when
    // Client closes the session in the middle of playback
    // but we cannot complete the message as RSession Handle is invalid
/*    if (iMessage2)
        {
        iMessage2->Complete(KErrCancel);
        }
*/
    if(iMessage2)
    {	
     delete iMessage2;
     iMessage2 = NULL;
    }

    // Delete play utility
    if (iPlayUtil)
     {	
     	delete iPlayUtil;
     	iPlayUtil = NULL;
     }
    //delete iAsyncCallback;
    //iAsyncCallback = NULL;

#ifdef RD_TSP_CLIENT_MAPPER
    if (iTSPClientMapper)
    	{
    
    TInt status = iTSPClientMapper->RemoveTspTargetClient(
                                        CTspClientMapper::ERegisteredClients,
                                        iClientProcessId );
    DEP_PRN2(_L("CDRMPlayServerSession::~CDRMPlayServerSession:Unreg PID[%x]Status[%d]"), iClientProcessId.Id(), status );
    delete iTSPClientMapper;
    	}
#endif // #ifdef RD_TSP_CLIENT_MAPPER

    // Cancel any pending async messages.
    TInt index(0);
    CDRMCustomCommandAsyncAO* tempAO(NULL);
    for (;index < iActiveAsyncCustomCommands.Count(); index++)
        {
        tempAO = iActiveAsyncCustomCommands[0];
        if ( iCDRMCustomCommandAsyncAO2Phase == tempAO )
            {
            iCDRMCustomCommandAsyncAO2Phase = NULL;
            }
        delete tempAO;
        iActiveAsyncCustomCommands.Remove(0);
        }
    iActiveAsyncCustomCommands.Reset();
    iActiveAsyncCustomCommands.Close();

    delete iCDRMCustomCommandAsyncAO2Phase;

    iServer->iClients--;

    if(!iServer->iClients)
        {
        CActiveScheduler::Stop();
        }
    }

void CDRMPlayServerSession::ServiceL(const RMessage2& aMessage)
    {
    TInt ret = KErrNone;
    TRAPD(err,ret = DispatchMessageL(aMessage));
    if (KErrNone != err)
        {
        ret = err;
        }
    if(iAsyncComplete == EFalse)
        {
        DEP_PRN1(_L("CDRMPlayServerSession::ServiceL - ret = %d"),ret);
        aMessage.Complete(ret);
        }
    }



// service a client request; test the opcode and then do appropriate servicing
TInt CDRMPlayServerSession::DispatchMessageL(const RMessage2 &aMessage)
    {
    TInt ret=KErrNone;
    iAsyncComplete= EFalse;
    DEP_PRN1(_L("CDRMPlayServerSession::DispatchMessageL:FnId[%d]"),aMessage.Function());
    switch (aMessage.Function())
        {
        case EDRMPlayServCreate:
             break;
        case EDRMPlayServNewFilePlayerL:
            iMessage = aMessage;
            NewFilePlayerL(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServNewDesPlayerL:
            iMessage = aMessage;
            NewDesPlayerL(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServNewDesPlayerReadOnlyL:
            iMessage = aMessage;
            NewDesPlayerReadOnlyL(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServNewPlayerL:
            NewPlayerL(aMessage);
            break;
        case EDRMPlayServOpenFile:
            iMessage = aMessage;
            OpenFileL(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServOpenFileByHandle:
            iMessage = aMessage;
            OpenFileByHandleL(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServOpenFileBySource:
            iMessage = aMessage;
            OpenFileBySourceL(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServOpenDes:
            iMessage = aMessage;
            OpenDesL(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServOpenUrl:
            iMessage = aMessage;
            OpenUrlL(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServPlay:
            iMessage = aMessage;
            if(iMessage2)
                {
                iMessage2->Complete(KErrCancel);
                delete iMessage2;
                iMessage2 = NULL;
                }
            iMessage2 = new (ELeave) RMessage2(aMessage);
            Play();
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServStop:
            if(iState == EStopped)
                break;
            if(iMessage2)
                {
                iMessage2->Complete(KErrCancel);
                delete iMessage2;
                iMessage2 = NULL;
                }
            Stop();
            break;
        case EDRMPlayServSetVolume:
            SetVolume(aMessage);
            break;
        case EDRMPlayServSetRepeats:
            SetRepeats(aMessage);
            break;
        case EDRMPlayServSetVolumeRamp:
            SetVolumeRamp(aMessage);
            break;
        case EDRMPlayServDuration:
            Duration(aMessage);
            break;
        case EDRMPlayServMaxVolume:
            ret = MaxVolume();
            break;
        case EDRMPlayServPause:
            ret = Pause();
            if(iMessage2)
                {
                iMessage2->Complete(KErrCancel);
                delete iMessage2;
                iMessage2 = NULL;
                }
            iAsyncComplete = EFalse;
            break;
        case EDRMPlayServClose:
            Close();
            break;
        case EDRMPlayServGetPosition:
            ret = GetPosition(aMessage);
            break;
        case EDRMPlayServSetPosition:
            SetPosition(aMessage);
            break;
        case EDRMPlayServSetPriority:
            ret = SetPriority(aMessage);
            break;
        case EDRMPlayServGetVolume:
            ret = GetVolume(aMessage);
            break;
        case EDRMPlayServGetNumberOfMetaDataEntries:
            ret = GetNumberOfMetaDataEntries(aMessage);
            break;
        case EDRMPlayServGetMetaDataEntry:
            GetMetaDataEntryL(aMessage);
            break;
        case EDRMPlayServClearPlayWindow:
            ret = ClearPlayWindow();
            break;
        case EDRMPlayServSetBalance:
            ret = SetBalance(aMessage);
            break;
        case EDRMPlayServGetBalance:
            ret = GetBalance(aMessage);
            break;
        case EDRMPlayServGetBitRate:
            ret = GetBitRate(aMessage);
            break;
        case EDRMPlayServRegisterForAudioLoadingNotification:
            iMessage = aMessage;
            RegisterForAudioLoadingNotification(aMessage);
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServGetAudioLoadingProgress:
            GetAudioLoadingProgressL(aMessage);
            break;
        case EDRMPlayServSetPlayWindow:
            SetPlayWindow(aMessage);
            break;
        case EDRMPlayServControllerImplementationInformation:
            ControllerImplementationInformationL(aMessage);
            break;
        case EDRMPlayServCustomCommandSyncWithReturn:
        case EDRMPlayServCustomCommandSyncWithoutReturn:
                CustomCommandSyncL(aMessage);
                break;
        case EDRMPlayServCustomCommandAsyncWithReturnStep1:
        case EDRMPlayServCustomCommandAsyncWithoutReturnStep1:
            CustomCommandAsyncStep1L(aMessage);
            // Setting this flag does not complete message in ServiceL,
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServCustomCommandAsyncWithReturnStep2:
        case EDRMPlayServCustomCommandAsyncWithoutReturnStep2:
            CustomCommandAsyncStep2L(aMessage);
            // Setting this flag does not complete message in ServiceL,
            iAsyncComplete = ETrue;
            break;
        case EDRMPlayServSetPriorityPreference:
            SetPriorityPreference(aMessage);
            break;

       default:
         PanicClient(aMessage, EBadRequest);
         break;
        }
    return ret;
    }

void CDRMPlayServerSession::NewFilePlayerL(const RMessage2 aMessage)
    {
    TDataStructPckgBuf params;

    aMessage.Read(0,iCallbackState);
    aMessage.Read(1,params);
    aMessage.Read(2,iErrDurPckg);
    HBufC16* inputBuf = HBufC16::NewL(aMessage.GetDesLengthL(3));
    TPtr16 ptr = inputBuf->Des();
    aMessage.Read(3,ptr);
    const TDataStruct &theStruct = params();
    delete iPlayUtil;
    iPlayUtil = NULL;
    iPlayUtil = CMdaAudioPlayerUtility::NewFilePlayerL( inputBuf->Des(),
                                                       *this,
                                                       iPriority,
                                                       iPref);
    delete inputBuf;
    }

void CDRMPlayServerSession::NewDesPlayerL(const RMessage2 aMessage)
    {
    TDataStructPckgBuf thePckg;
    aMessage.Read(0,iCallbackState);
    aMessage.Read(1,thePckg);
    aMessage.Read(2,iErrDurPckg);

    delete iPlayUtil;
    iPlayUtil = NULL;
    iPlayUtil = CMdaAudioPlayerUtility::NewDesPlayerL(thePckg().iData,
                                                      *this,
                                                      iPriority,
                                                      iPref);

    }

void CDRMPlayServerSession::NewDesPlayerReadOnlyL(const RMessage2 aMessage)
    {
    TDataStructPckgBuf thePckg;
    aMessage.Read(0,iCallbackState);
    aMessage.Read(1,thePckg);
    aMessage.Read(2,iErrDurPckg);

    delete iPlayUtil;
    iPlayUtil = NULL;
    iPlayUtil = CMdaAudioPlayerUtility::NewDesPlayerReadOnlyL(thePckg().iData,
                                                              *this,
                                                              iPriority,
                                                              iPref);
    }

void CDRMPlayServerSession::NewPlayerL(const RMessage2 aMessage)
    {

    TDataStructPckgBuf thePckg;
    aMessage.Read(0,iCallbackState);
    aMessage.Read(1,thePckg);
    aMessage.Read(2,iErrDurPckg);

    delete iPlayUtil;
    iPlayUtil = NULL;
    iPlayUtil = CMdaAudioPlayerUtility::NewL(*this,
                                             iPriority,
                                             iPref);
    }

void CDRMPlayServerSession::OpenFileL(const RMessage2 &aMessage)
    {
    //call mmfclientaudioplayer open file
    iFileHandleUsed = EFalse;

    aMessage.Read(0,iCallbackState);
    // Get the file length
    TInt len = aMessage.GetDesLengthL(1);
    HBufC* fileName = HBufC::NewLC(len);
    TPtr fileNamePtr = fileName->Des();
    aMessage.Read(1,fileNamePtr);
    aMessage.Read(2,iErrDurPckg);
    if(iPlayUtil)
        {
        iPlayUtil->OpenFileL(fileNamePtr);
        }
    CleanupStack::PopAndDestroy(fileName);
    }

void CDRMPlayServerSession::OpenFileByHandleL(const RMessage2 &aMessage)
    {
    iFileHandleUsed = ETrue;
    TInt err = iFileHandle.AdoptFromClient(aMessage,1,3);

    aMessage.Read(0,iCallbackState);
    aMessage.Read(2,iErrDurPckg);

    if (err == KErrNone)
        {
        if(iPlayUtil)
            {
            iPlayUtil->OpenFileL(iFileHandle);
            }
        }
    else
        {
        User::Leave(err);
        }
    }

void CDRMPlayServerSession::OpenFileBySourceL(const RMessage2 &/*aMessage*/)
    {

    }

void CDRMPlayServerSession::OpenDesL(const RMessage2 &aMessage)
    {
    //call mmfclientaudioplayer OpenDesL
    TBuf8<50> aDes;

    aMessage.Read(0,iCallbackState);
    aMessage.Read(1,aDes);
    aMessage.Read(2,iErrDurPckg);

    if(iPlayUtil)
        {
        iPlayUtil->OpenDesL(aDes);
        }
    }

void CDRMPlayServerSession::OpenUrlL(const RMessage2 &aMessage)
    {
    //call mmfclientaudioplayer OpenUrlL
    TUrlStructPckgBuf thePckg;

    aMessage.Read(0,iCallbackState);
    aMessage.Read(1,thePckg);
    aMessage.Read(2,iErrDurPckg);

    if(iPlayUtil)
        {
        iPlayUtil->OpenUrlL(thePckg().iUrl,
                            thePckg().iIapId,
                            thePckg().iMimeType);
        }
    }

void CDRMPlayServerSession::Play()
    {
    //call mmfclientaudioplayer Play
    iState = EPlaying;
    if(iPlayUtil)
        {
        iPlayUtil->Play();
#ifdef RD_TSP_CLIENT_MAPPER
        TInt status = iTSPClientMapper->SetTspTargetClientToOtherType(
                                            CTspClientMapper::EPlayingClients,
                                            iClientProcessId );
        DEP_PRN2(_L("CDRMPlayServerSession::Play:Change PID[%x]Status[%d]"), iClientProcessId.Id(), status );
#endif // #ifdef RD_TSP_CLIENT_MAPPER
        }
    }

void CDRMPlayServerSession::Stop()
    {
    //call mmfclientaudioplayer Stop
    if(iPlayUtil)
        {
        iPlayUtil->Stop();
#ifdef RD_TSP_CLIENT_MAPPER
        TInt status = iTSPClientMapper->SetTspTargetClientToOtherType(
                                            CTspClientMapper::EStoppedClients,
                                            iClientProcessId );
        DEP_PRN2(_L("CDRMPlayServerSession::Stop:Change PID[%x]Status[%d]"), iClientProcessId.Id(), status );
#endif // #ifdef RD_TSP_CLIENT_MAPPER
        }
    iState = EStopped;
    }

void CDRMPlayServerSession::SetVolume(const RMessage2 & aMessage)
    {
    //call mmfclientaudioplayer SetVolume
    TInt theVolume = aMessage.Int0();

    if(iPlayUtil)
        {
        iPlayUtil->SetVolume(theVolume);
        }
    }

void CDRMPlayServerSession::SetRepeats(const RMessage2 &aMessage)
    {
    //call mmfclientaudioplayer SetRepeats
    TInt theRepeatNumberOfTimes = aMessage.Int0();
    TTimeIntervalMicroSeconds theTrailingSilence;

    TPckgTTimeIntervalMicroSeconds theTrailingSilencePckg(theTrailingSilence);
    aMessage.Read(1,theTrailingSilencePckg);

    iPlayUtil->SetRepeats(theRepeatNumberOfTimes,theTrailingSilence);
    }

void CDRMPlayServerSession::SetVolumeRamp(const RMessage2 &aMessage)
    {
    //call mmfclientaudioplayer SetVolumeRamp
    TTimeIntervalMicroSeconds theRampDuration;
    TPckgTTimeIntervalMicroSeconds theRampDurationPckg(theRampDuration);
    aMessage.Read(0,theRampDurationPckg);

    iPlayUtil->SetVolumeRamp(theRampDuration);
    }

void CDRMPlayServerSession::Duration(const RMessage2& aMessage)
    {
    TTimeIntervalMicroSeconds theDuration;
    theDuration = iPlayUtil->Duration();

    TPckgBufTTimeIntervalMicroSeconds theDurationPckg(theDuration);
    aMessage.Write(0,theDurationPckg);
    }

TInt CDRMPlayServerSession::MaxVolume()
    {
    TInt theMaxVolume;
    theMaxVolume = iPlayUtil->MaxVolume();

    return theMaxVolume;
    }

// API Additions since version 7.0
TInt CDRMPlayServerSession::Pause()
    {
    TInt ret = KErrNone;
    if(iPlayUtil)
        {
        ret = iPlayUtil->Pause();
        }
    
    // This message should not be completed here. It should be completed in ServiceL()
/*    if(iState == EPlaying)
        {
        iMessage.Complete(KErrNone);
        }*/
    iState = EStopped;
    return ret;
    }

void CDRMPlayServerSession::Close()
    {

    if(iFileHandleUsed)
     iFileHandle.Close();

    iPlayUtil->Close();
    }

TInt CDRMPlayServerSession::GetPosition(const RMessage2 &aMessage)
    {
    TInt ret;
    TTimeIntervalMicroSeconds thePosition;

    ret = iPlayUtil->GetPosition(thePosition);


    TPckgTTimeIntervalMicroSeconds thePositionPckg(thePosition);
    aMessage.Write(0,thePositionPckg);

    return ret;
    }

void CDRMPlayServerSession::SetPosition(const RMessage2 &aMessage)
    {
    TTimeIntervalMicroSeconds thePosition;
    TPckgTTimeIntervalMicroSeconds thePositionPckg(thePosition);
    aMessage.Read(0,thePositionPckg);
    iPlayUtil->SetPosition(thePosition);
    }

TInt CDRMPlayServerSession::SetPriority(const RMessage2 &aMessage)
    {
    TInt thePriority = aMessage.Int0();
    TMdaPriorityPreference thePref = (TMdaPriorityPreference)aMessage.Int1();

    TInt ret = iPlayUtil->SetPriority(thePriority,thePref);

    return ret;
    }

TInt CDRMPlayServerSession::GetVolume(const RMessage2 &aMessage)
    {
    TInt theVolume;
    TInt ret = iPlayUtil->GetVolume(theVolume);
    TPckgBufTInt pckg(theVolume);
    aMessage.Write(0,pckg);
    return ret;
    }

TInt CDRMPlayServerSession::GetNumberOfMetaDataEntries(const RMessage2 &aMessage)
    {
    TInt ret;
    TInt theNumOfMetaDataEntries;

    ret = iPlayUtil->GetNumberOfMetaDataEntries(theNumOfMetaDataEntries);

    TPckgBufTInt pckg(theNumOfMetaDataEntries);
    aMessage.Write(0,pckg);

    return ret;
    }


void CDRMPlayServerSession::GetMetaDataEntryL(const RMessage2 &aMessage)
    {
    //Get the index of the required entry
    TInt theIndex = aMessage.Int0();
    CMMFMetaDataEntry* entry = iPlayUtil->GetMetaDataEntryL(theIndex);
    CleanupStack::PushL(entry);
    // Delete any existing buffer
    if (iMetaDataBuffer)
    	{
    	delete iMetaDataBuffer;
    	iMetaDataBuffer = NULL;
    	}
    
    // Create a buffer to hold the externalised entry
    iMetaDataBuffer = CBufFlat::NewL(32);
    RBufWriteStream s;
    s.Open(*iMetaDataBuffer);
    CleanupClosePushL(s);
    entry->ExternalizeL(s);
    CleanupStack::PopAndDestroy(2);//s, entry

    //Write the externalised data back to the client
    aMessage.Write(1,iMetaDataBuffer->Ptr(0));
    }


TInt CDRMPlayServerSession::SetPlayWindow(const RMessage2 &aMessage)
    {
    TPlayWindowStructBuf buf;
    aMessage.Read(0,buf);

    TTimeIntervalMicroSeconds playStart = buf().iPlayStart;
    TTimeIntervalMicroSeconds playEnd = buf().iPlayEnd;

    TInt ret = iPlayUtil->SetPlayWindow(playStart,playEnd);

    return ret;
    }

TInt CDRMPlayServerSession::ClearPlayWindow()
    {
    return iPlayUtil->ClearPlayWindow();
    }

TInt CDRMPlayServerSession::SetBalance(const RMessage2 &aMessage)
    {
    TInt theBalance = aMessage.Int0();
    return iPlayUtil->SetBalance(theBalance);
    }

TInt CDRMPlayServerSession::GetBalance(const RMessage2 &aMessage)
    {
    TInt theBalance;
    TInt ret;

    ret = iPlayUtil->GetBalance(theBalance);

    TPckgBufTInt pckg(theBalance);
    aMessage.Write(0,pckg);

    return ret;
    }

TInt CDRMPlayServerSession::GetBitRate(const RMessage2 &aMessage)
    {
    TUint theBitRate;
    TInt ret;

    ret = iPlayUtil->GetBitRate(theBitRate);

    TPckgBufTUint pckg(theBitRate);
    aMessage.Write(0,pckg);

    return ret;
    }

void CDRMPlayServerSession::RegisterForAudioLoadingNotification(const RMessage2 &aMessage)
    {
    aMessage.Read(0,iCallbackState);

    if(iCallbackState()==ELoadingStarted)
        {
        iPlayUtil->RegisterForAudioLoadingNotification(*this);
        }
    }

void CDRMPlayServerSession::GetAudioLoadingProgressL(const RMessage2 &aMessage)
    {
    TInt thePercentProgress;

    iPlayUtil->GetAudioLoadingProgressL(thePercentProgress);

    TPckgBufTInt pckg(thePercentProgress);

    aMessage.Write(0,pckg);
    }

void CDRMPlayServerSession::ControllerImplementationInformationL(const RMessage2 &aMessage)
    {
    const CMMFControllerImplementationInformation& x= iPlayUtil->ControllerImplementationInformationL();

    //for now just write Uid by to client
    typedef TPckg<TUid> TPckgTUid;

    TPckgTUid thePckg(x.Uid());
    aMessage.Write(0,thePckg);
    }


void CDRMPlayServerSession::CustomCommandSyncL(const RMessage2 &aMessage)
    {
    DEP_PRN0(_L("CDRMPlayServerSession::CustomCommandSyncL:Enter"));

    TPckgCustomCommand thePckgCustomCommand;
    User::LeaveIfError(aMessage.Read(0,thePckgCustomCommand));

    HBufC8* dataTo1Buf(NULL);
    HBufC8* dataTo2Buf(NULL);
    HBufC8* dataFromBuf(NULL);
    TPtr8 ptr1(NULL, NULL);
    TPtr8 ptr2(NULL, NULL);
    TPtr8 ptr3(NULL, NULL);

    // Read data1
    dataTo1Buf = HBufC8::NewL(aMessage.GetDesLengthL(1));
    CleanupStack::PushL(dataTo1Buf);
    ptr1.Set(dataTo1Buf->Des());
    User::LeaveIfError(aMessage.Read(1,ptr1));

   // Check and filter the messages
   // Dont Allow Select Custom Commands
   if( !IsValidCustomCommandDestination(thePckgCustomCommand().iDestination().InterfaceId(), ptr1) )
            User::Leave(KErrAccessDenied);

    // Read data2
    dataTo2Buf = HBufC8::NewL(aMessage.GetDesLengthL(2));
    CleanupStack::PushL(dataTo2Buf);
    ptr2.Set(dataTo2Buf->Des());
    User::LeaveIfError(aMessage.Read(2,ptr2));

    if ( aMessage.Function() == EDRMPlayServCustomCommandSyncWithReturn )
        {
        // Read data3
        dataFromBuf = HBufC8::NewL(aMessage.GetDesLengthL(3));
        CleanupStack::PushL(dataFromBuf);
        ptr3.Set(dataFromBuf->Des());
        User::LeaveIfError(aMessage.Read(3,ptr3));
/*
        RDebug::Print(_L("CDRMPlayServerSession::CustomCommandSync:IFId[%x]DestHndl[%x]Fn[%d]RetDesLen[%d]"),
                                    thePckgCustomCommand().iDestination().InterfaceId(),
                                    thePckgCustomCommand().iDestination().DestinationHandle(),
                                    thePckgCustomCommand().iFunction,
                                    ptr3.MaxLength());
*/
        // Call CustomCommandSync on play util
        User::LeaveIfError(iPlayUtil->CustomCommandSync(
                                            thePckgCustomCommand().iDestination,
                                            thePckgCustomCommand().iFunction,
                                            ptr1,
                                            ptr2,
                                            ptr3));
        User::LeaveIfError(aMessage.Write(3,ptr3));

        // Free heap allocated for data3
        CleanupStack::PopAndDestroy(dataFromBuf);
        }
    else
        {
/*
        RDebug::Print(_L("CDRMPlayServerSession::CustomCommandSync:IFId[%x]DestHndl[%x]Fn[%d]"),
                                    thePckgCustomCommand().iDestination().InterfaceId(),
                                    thePckgCustomCommand().iDestination().DestinationHandle(),
                                    thePckgCustomCommand().iFunction );
*/
        // Call CustomCommandSync on play util
        User::LeaveIfError(iPlayUtil->CustomCommandSync(
                                            thePckgCustomCommand().iDestination,
                                            thePckgCustomCommand().iFunction,
                                            ptr1,
                                            ptr2));
        }

    // Free heap allocated for data2
    CleanupStack::PopAndDestroy(dataTo2Buf);
    // Free heap allocated for data1
    CleanupStack::PopAndDestroy(dataTo1Buf);

    DEP_PRN0(_L("CDRMPlayServerSession::CustomCommandSyncL:Exit"));
    }

void CDRMPlayServerSession::CustomCommandAsyncStep1L( const RMessage2& aMessage )
    {
    DEP_PRN0(_L("CDRMPlayServerSession::CustomCommandAsyncStep1L:Enter"));

    // If there is a pending phase1, then there is something wrong.
    if ( iCDRMCustomCommandAsyncAO2Phase )
        {
        User::Leave(KErrAlreadyExists);
        }

    if ( aMessage.Function() == EDRMPlayServCustomCommandAsyncWithReturnStep1 )
        {
        iCDRMCustomCommandAsyncAO2Phase = CDRMCustomCommandAsyncAO::NewL(
                                                    aMessage,
                                                    CDRMCustomCommandAsyncAO::ECustomCommandWithResult,
                                                    *this );
        }
    else /*if ( aMessage.Function() == EDRMPlayServCustomCommandAsyncWithoutReturnStep1 )*/
        {
        iCDRMCustomCommandAsyncAO2Phase = CDRMCustomCommandAsyncAO::NewL(
                                                    aMessage,
                                                    CDRMCustomCommandAsyncAO::ECustomCommandWithoutResult,
                                                    *this );
        }
    // Get reference to data1 descriptor
    TPtr8 data1Ptr(iCDRMCustomCommandAsyncAO2Phase->GetData1FromClient()->Des());

    // Check and filter the messages
    // Dont Allow Select Custom Commands
    if( !IsValidCustomCommandDestination((iCDRMCustomCommandAsyncAO2Phase->GetMMFMessageDestinationPckg())().InterfaceId(), data1Ptr) )
        {
        User::Leave( KErrAccessDenied );
        }

    aMessage.Complete( KErrNone );
    DEP_PRN0(_L("CDRMPlayServerSession::CustomCommandAsyncStep1L:Exit"));
    }

void CDRMPlayServerSession::CustomCommandAsyncStep2L( const RMessage2& aMessage )
    {
    DEP_PRN0(_L("CDRMPlayServerSession::CustomCommandAsyncStep2L:Enter"));

    // Read info from client session
    TPtr8 data1Ptr(NULL,NULL);
    TPtr8 data2Ptr(NULL,NULL);
    TPtr8 dataToClientPtr(NULL,NULL);

    // If there is no pending phase1, then there is something wrong.
    if ( !iCDRMCustomCommandAsyncAO2Phase )
        {
        User::Leave(KErrNotReady);
        }

    // Add custCmdAsyncAO object to array of async custom command objects
    iActiveAsyncCustomCommands.AppendL( iCDRMCustomCommandAsyncAO2Phase );

    // aMessage ownership is transferred to custCmdAsyncAO.
    // After succesful transfer, aMessage should not be completed
    // anywhere outside of this object.
    iCDRMCustomCommandAsyncAO2Phase->TransferOwnershipL(aMessage);

    CDRMCustomCommandAsyncAO* drmCCAsyncAO = iCDRMCustomCommandAsyncAO2Phase;
    iCDRMCustomCommandAsyncAO2Phase = NULL;

    // Get reference to descriptors
    data1Ptr.Set(drmCCAsyncAO->GetData1FromClient()->Des());
    data2Ptr.Set(drmCCAsyncAO->GetData2FromClient()->Des());

    // Make call on controller
    drmCCAsyncAO->SetActive();
    if ( aMessage.Function() == EDRMPlayServCustomCommandAsyncWithReturnStep2 )
        {
        // If date needs to be returned, get reference to the descriptor
        dataToClientPtr.Set(drmCCAsyncAO->GetDataToClient()->Des());
/*
        RDebug::Print(_L("CDRMPlayServerSession::CustomCommandAsyncStep2L:IFId[%x]DestHndl[%x]Fn[%d]Data1[Len:%d:MaxLen:%d]Data2Len[Len:%d:MaxLen:%d]RetDesLen[Len:%d:MaxLen:%d]"),
                                    (drmCCAsyncAO->GetMMFMessageDestinationPckg())().InterfaceId(),
                                    (drmCCAsyncAO->GetMMFMessageDestinationPckg())().DestinationHandle(),
                                    drmCCAsyncAO->GetMMFMessageFunction(),
                                    data1Ptr.Length(),
                                    data1Ptr.MaxLength(),
                                    data2Ptr.Length(),
                                    data2Ptr.MaxLength(),
                                    dataToClientPtr.Length(),
                                    dataToClientPtr.MaxLength() );
*/
        // Call CustomCommandAsync on play util
        iPlayUtil->CustomCommandAsync(
                        drmCCAsyncAO->GetMMFMessageDestinationPckg(),
                        drmCCAsyncAO->GetMMFMessageFunction(),
                        data1Ptr,
                        data2Ptr,
                        dataToClientPtr,
                        drmCCAsyncAO->iStatus );
        }
    else /*if ( aMessage.Function() == EDRMPlayServCustomCommandAsyncWithoutReturnStep2 )*/
        {
/*
        RDebug::Print(_L("CDRMPlayServerSession::CustomCommandAsyncStep2L:IFId[%x]DestHndl[%x]Fn[%d]Data1[Len:%d:MaxLen:%d]Data2Len[Len:%d:MaxLen:%d]"),
                                    (drmCCAsyncAO->GetMMFMessageDestinationPckg())().InterfaceId(),
                                    (drmCCAsyncAO->GetMMFMessageDestinationPckg())().DestinationHandle(),
                                    drmCCAsyncAO->GetMMFMessageFunction(),
                                    data1Ptr.Length(),
                                    data1Ptr.MaxLength(),
                                    data2Ptr.Length(),
                                    data2Ptr.MaxLength() );
*/
        // Call CustomCommandAsync on play util
        iPlayUtil->CustomCommandAsync(
                        drmCCAsyncAO->GetMMFMessageDestinationPckg(),
                        drmCCAsyncAO->GetMMFMessageFunction(),
                        data1Ptr,
                        data2Ptr,
                        drmCCAsyncAO->iStatus );
        }

    DEP_PRN0(_L("CDRMPlayServerSession::CustomCommandAsyncStep2L:Exit"));
    }

void CDRMPlayServerSession::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
    {

    DEP_PRN2(_L("CDRMPlayServerSession::MapcInitComplete :-> Status[%d] Duration[%d]"), aError, I64INT(aDuration.Int64()));
     //iMessage.Write aError & aDuration to client
    iCallbackState() = EInitComplete;

    iMessage.Write(0,iCallbackState);

    iErrDurPckg().iError = aError;
    iErrDurPckg().iDuration = aDuration;

    iMessage.Write(2,iErrDurPckg);
    iMessage.Complete(aError);
    iAsyncComplete= EFalse;
    }

void CDRMPlayServerSession::MapcPlayComplete(TInt aError)
    {
    DEP_PRN1(_L("CDRMPlayServerSession::MapcPlayComplete :-> Error[%d]"), aError);

#ifdef RD_TSP_CLIENT_MAPPER
    TInt status = iTSPClientMapper->SetTspTargetClientToOtherType(
                                        CTspClientMapper::EStoppedClients,
                                        iClientProcessId );
    DEP_PRN2(_L("CDRMPlayServerSession::MapcPlayComplete:Change PID[%x]Status[%d]"), iClientProcessId.Id(), status );
#endif // #ifdef RD_TSP_CLIENT_MAPPER


    iState = EStopped;

    iCallbackState()= EPlayComplete;
/*
    iMessage.Write(0,iCallbackState);
    iErrDurPckg().iError = aError;
    iErrDurPckg().iDuration = 0;
    iMessage.Write(2,iErrDurPckg);
    iMessage.Complete(aError);
*/
    if ( iMessage2 )
        {
         //iMessage2. Write aError & aDuration to client
        iCallbackState() = EPlayComplete;
    
        iMessage2->Write(0,iCallbackState);
    
        iErrDurPckg().iError = aError;
        iErrDurPckg().iDuration = 0;    
        iMessage2->Write(2,iErrDurPckg);
        iMessage2->Complete(aError);
        delete iMessage2;
        iMessage2 = NULL;
        }

    iAsyncComplete= EFalse;
    }


void CDRMPlayServerSession::MaloLoadingStarted()
    {
    //send event to client
    iCallbackState() = ELoadingStarted;
    iMessage.Write(0,iCallbackState);
    iMessage.Complete(KErrNone);
    iAsyncComplete= EFalse;
    }
void CDRMPlayServerSession::MaloLoadingComplete()
    {
    //send event to client
    iCallbackState() = ELoadingComplete;
    iMessage.Write(0,iCallbackState);
    iMessage.Complete(KErrNone);
    iAsyncComplete= EFalse;
    }

void CDRMPlayServerSession::AsyncCustomCommandCompleted( CDRMCustomCommandAsyncAO* aObject)
    {
    TInt index = iActiveAsyncCustomCommands.Find( aObject );
    // This should never happen. Else there is something wrong....
    if ( index != KErrNotFound )
        {
        iActiveAsyncCustomCommands.Remove(index);
        }
    delete aObject;
    }

void CDRMPlayServerSession::SetPriorityPreference(const RMessage2 &aMessage)
    {
    iPriority = aMessage.Int0();
    iPref = (TMdaPriorityPreference)aMessage.Int1();
    }

TBool CDRMPlayServerSession::IsValidCustomCommandDestination(TUid aDestinationUid, TDesC8& aParam)
    {
    TBool retValue(ETrue);
    if (aDestinationUid == KUidInterfaceMMFDRMControl)
        {
        retValue = EFalse;
        }
    else if ( aDestinationUid == KUidCustomInterfaceBuilderImpl )
        {
        RDesReadStream stream(aParam);
        CleanupClosePushL(stream);
        TUid paramUid;
        TRAPD(status, paramUid.iUid = stream.ReadInt32L());
        CleanupStack::PopAndDestroy(&stream);
        if ( (status == KErrNone ) && (paramUid == KUidAudioOutput) )
            {
            retValue = EFalse;
            }
        }
    return retValue;
    }

// End of File


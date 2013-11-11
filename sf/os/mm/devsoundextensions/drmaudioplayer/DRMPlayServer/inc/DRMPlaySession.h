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


#ifndef __CDRMPLAYSESSION_H
#define __CDRMPLAYSESSION_H


#include <e32base.h>
#include <e32cons.h>
#include <f32file.h>
#include "DRMPlayServer.h"
#include "DRMCustomCommandAsyncAO.h"
#include <mdaaudiosampleplayer.h>
#include <s32mem.h>
#include <mmfcontrollerframeworkbase.h>


class CDRMCustomCommandAsyncAO;
class CDRMPlayServer;
#ifdef RD_TSP_CLIENT_MAPPER
class CTspClientMapper;
#endif // #ifdef RD_TSP_CLIENT_MAPPER

class CDRMPlayServerSession : public CSession2, 
                              public MMdaAudioPlayerCallback,
							  public MAudioLoadingObserver
	{
	friend class CDRMPlayServer;
public:
	// construct/destruct
	CDRMPlayServerSession(CDRMPlayServer* aServer);
    static CDRMPlayServerSession* NewL(CDRMPlayServer* aServer, const RMessage2& aMessage );
	~CDRMPlayServerSession();
	//service request
	virtual void ServiceL(const RMessage2 &aMessage);
	TInt DispatchMessageL(const RMessage2 &aMessage);

	//services available 
	void NewFilePlayerL(const RMessage2 aMessage);
	void NewDesPlayerL(const RMessage2 aMessage);
	void NewDesPlayerReadOnlyL(const RMessage2 aMessage);
	void NewPlayerL(const RMessage2 aMessage);	
	void OpenFileL(const RMessage2 &aMessage);
	void OpenFileByHandleL(const RMessage2 &aMessage);
	void OpenFileBySourceL(const RMessage2 &aMessage);
	void OpenDesL(const RMessage2 &aMessage);
	void OpenUrlL(const RMessage2 &aMessage);
	void Play();
	void Stop();
	void SetVolume(const RMessage2 &aMessage);
	void SetRepeats(const RMessage2 &aMessage);
	void SetVolumeRamp(const RMessage2 &aMessage);
	void Duration(const RMessage2&aMessage); 
	TInt MaxVolume();

	// API Additions since version 7.0	
	TInt Pause();
	void Close();
	TInt GetPosition(const RMessage2 &aMessage);
	void SetPosition(const RMessage2 &aMessage);

	TInt SetPriority(const RMessage2 &aMessage);
	TInt GetVolume(const RMessage2 &aMessage);
	TInt GetNumberOfMetaDataEntries(const RMessage2 &aMessage);
	void GetMetaDataEntryL(const RMessage2 &aMessage);
	TInt SetPlayWindow(const RMessage2 &aMessage);
	TInt ClearPlayWindow();
	TInt SetBalance(const RMessage2 &aMessage);
	TInt GetBalance(const RMessage2 &aMessage);
	TInt GetBitRate(const RMessage2 &aMessage);
	
	void RegisterForAudioLoadingNotification(const RMessage2 &aMessage);
	void GetAudioLoadingProgressL(const RMessage2 &aMessage);
	void ControllerImplementationInformationL(const RMessage2 &aMessage);

	void CustomCommandSyncL(const RMessage2 &aMessage);
    void CustomCommandAsyncStep1L(const RMessage2 &aMessage);
    void CustomCommandAsyncStep2L(const RMessage2 &aMessage);
    
    void SetPriorityPreference(const RMessage2 &aMessage);
    void AsyncCustomCommandCompleted( CDRMCustomCommandAsyncAO* aObject );



protected:
	// panic the client
	void PanicClient(const RMessage2 &aMessage, TInt aPanic) const;

private:
    void ConstructL( const RMessage2& aMessage );

	//MMdaAudioPlayerCallback
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);

    //MAudioLoadingObserver
	void MaloLoadingStarted();
	void MaloLoadingComplete();

    //Returns False if Destination CustomCommand needs to be blocked
	TBool IsValidCustomCommandDestination(TUid aDestinationUid, TDesC8& aParam);
	TBool CheckCustomInterfaceBuilderImplL(TDesC8& aParam);

private:
	enum{EIdle,EPlaying,EStopped};

	CDRMPlayServer *iServer;
	CMdaAudioPlayerUtility* iPlayUtil;
	RMessage2 iMessage;
	RMessage2* iMessage2;
	CBufFlat* iMetaDataBuffer;
	//CAsyncCallback* iAsyncCallback;
	TErrorDurationStructPckgBuf iErrDurPckg;
	TAsyncCallbackStatePckgBuf  iCallbackState;
    TInt iAsyncComplete;
	TInt iCount;
	TInt iState;
	TInt iPriority;
	TMdaPriorityPreference iPref;
	TBool iFileHandleUsed;
	RFile iFileHandle;
#ifdef RD_TSP_CLIENT_MAPPER
    // Handle to TSP Client Mapper for Rem Con Framework
    CTspClientMapper*   iTSPClientMapper;
    // Process id of client to DRM Session
    TProcessId          iClientProcessId;
#endif // RD_TSP_CLIENT_MAPPER
	RPointerArray<CDRMCustomCommandAsyncAO> iActiveAsyncCustomCommands;
	CDRMCustomCommandAsyncAO* iCDRMCustomCommandAsyncAO2Phase;
	};

#endif

// End of file

// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedAll
 @released
*/

#ifndef REMCONCOREAPITARGET_H
#define REMCONCOREAPITARGET_H

#include <e32base.h>
#include <remcon/remconinterfacebase.h>
#include <remconcoreapi.h>
#include <remcon/remconinterfaceif.h>

class MRemConCoreApiTargetObserver;
class CRemConInterfaceSelector;

/**
Client-instantiable type supporting sending Core API responses.
*/
NONSHARABLE_CLASS(CRemConCoreApiTarget) : public CRemConInterfaceBase, 
								          public MRemConInterfaceIf
	{
public:
	/**
	Factory function.
	@param aInterfaceSelector The interface selector. The client must have 
	created one of these first.
	@param aObserver The observer of this interface.
	@return A new CRemConCoreApiTarget, owned by the interface selector.
	*/
	IMPORT_C static CRemConCoreApiTarget* NewL(CRemConInterfaceSelector& aInterfaceSelector, 
		MRemConCoreApiTargetObserver& aObserver);
	/**
	Factory function.
	@param aInterfaceSelector The interface selector. The client must have 
	created one of these first.
	@param aObserver The observer of this interface.
	@param aFeatureSupported The Array is used to stote features supportd by the client 
	
	@return A new CRemConCoreApiTarget, owned by the interface selector.
	*/
	IMPORT_C static CRemConCoreApiTarget* NewL(CRemConInterfaceSelector& aInterfaceSelector, 
			MRemConCoreApiTargetObserver& aObserver,const RArray<TRemConCoreApiOperationId>& aFeatureSupported);
	
	/** Destructor */
	IMPORT_C ~CRemConCoreApiTarget();

public:
	/** Send a 'select' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void SelectResponse(TRequestStatus& aStatus, TInt aError);

	/** Send an 'up' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void UpResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'down' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void DownResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'left' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void LeftResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'right' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void RightResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'right up' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void RightUpResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'right down' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void RightDownResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'left up' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void LeftUpResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'left down' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void LeftDownResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'root menu' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void RootMenuResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'setup menu' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void SetupMenuResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'contents menu' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void ContentsMenuResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'favorite menu' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void FavoriteMenuResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send an 'exit' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void ExitResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a '0' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _0Response(TRequestStatus& aStatus, TInt aError);
	
	/** Send a '1' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _1Response(TRequestStatus& aStatus, TInt aError);
	
	/** Send a '2' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _2Response(TRequestStatus& aStatus, TInt aError);
	
	/** Send a '3' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _3Response(TRequestStatus& aStatus, TInt aError);
	
	/** Send a '4'response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _4Response(TRequestStatus& aStatus, TInt aError);
	
	/** Send a '5' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _5Response(TRequestStatus& aStatus, TInt aError);

	/** Send a '6' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _6Response(TRequestStatus& aStatus, TInt aError);
	
	/** Send a '7' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _7Response(TRequestStatus& aStatus, TInt aError);
	
	/** Send a '8' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _8Response(TRequestStatus& aStatus, TInt aError);

	/** Send a '9' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void _9Response(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'dot' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void DotResponse(TRequestStatus& aStatus, TInt aError);

	/** Send an 'enter' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void EnterResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'clear' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void ClearResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'channel up' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void ChannelUpResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'channel down' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void ChannelDownResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'previous channel' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void PreviousChannelResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'sound select' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void SoundSelectResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'input select' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void InputSelectResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'display information' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void DisplayInformationResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'help' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void HelpResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'page up' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void PageUpResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'page down' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void PageDownResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'power' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void PowerResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'volume up' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void VolumeUpResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'volume down' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void VolumeDownResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'mute' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void MuteResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a play response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the request.
	@param aError The response error.
	*/
	IMPORT_C void PlayResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a stop response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the request.
	@param aError The response error.
	*/
	IMPORT_C void StopResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'pause' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void PauseResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'record' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void RecordResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'rewind' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void RewindResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'fast forward' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void FastForwardResponse(TRequestStatus& aStatus, TInt aError);

	/** Send an 'eject' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void EjectResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'forward' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void ForwardResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'backward' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void BackwardResponse(TRequestStatus& aStatus, TInt aError);

	/** Send an 'angle' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void AngleResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'subpicture' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void SubpictureResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'pause play function' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void PausePlayFunctionResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'restore volume function' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void RestoreVolumeFunctionResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'tune function' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void TuneFunctionResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'select disk function' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void SelectDiskFunctionResponse(TRequestStatus& aStatus, TInt aError);
	
	/** Send a 'select AV input function' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void SelectAvInputFunctionResponse(TRequestStatus& aStatus, TInt aError);

	/** Send a 'select audio input function' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void SelectAudioInputFunctionResponse(TRequestStatus& aStatus, TInt aError);

	/** Send an 'F1' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void F1Response(TRequestStatus& aStatus, TInt aError);

	/** Send an 'F2' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void F2Response(TRequestStatus& aStatus, TInt aError);

	/** Send an 'F3' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void F3Response(TRequestStatus& aStatus, TInt aError);

	/** Send an 'F4' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void F4Response(TRequestStatus& aStatus, TInt aError);

	/** Send an 'F5' response.
	Only one response per target session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aError The response error.
	*/
	IMPORT_C void F5Response(TRequestStatus& aStatus, TInt aError);

	/** Send a response. 
	Only one response per target session can be outstanding at any one time.
	This function may be used to send a response when it is undesirable to use the 
	'specific' response-sending methods (PlayResponse, StopResponse etc). 
	@param aStatus Used by RemCon to indicate completion of the send request. 
	@param aOperationId The operation ID of the command we are sending a response to. 
	@param aError The response error. 
	*/ 
	IMPORT_C void SendResponse(TRequestStatus& aStatus, TRemConCoreApiOperationId aOperationId, TInt aError);
	
private:
	/** 
	Constructor.
	@param aInterfaceSelector The interface selector.
	@param aObserver The observer of this interface.
	*/
	CRemConCoreApiTarget(CRemConInterfaceSelector& aInterfaceSelector, 
		MRemConCoreApiTargetObserver& aObserver);
    void ConstructL(const RArray<TRemConCoreApiOperationId>& aFeaturesSupported);

private: // utility
	/**
	Utility to send a response.
	@param aStatus TRequestStatus for the send operation.
	@param aOperationId The operation ID.
	*/
	void SendGenericResponse(TRequestStatus& aStatus, 
		TRemConCoreApiOperationId aOperationId,
		TInt aError);

private: // from CRemConInterfaceBase
	TAny* GetInterfaceIf(TUid aUid);

private: // from MRemConInterfaceIf
	void MrcibNewMessage(TUint aOperationId, const TDesC8& aData);

private: // utility
	void HandlePlay(const TDesC8& aData, TRemConCoreApiButtonAction aButton);
	void HandleTuneFunction(const TDesC8& aData, TRemConCoreApiButtonAction aButton);
	void HandleSelectDiskFunction(const TDesC8& aData, TRemConCoreApiButtonAction aButton);
	void HandleSelectAvInputFunction(const TDesC8& aData, TRemConCoreApiButtonAction aButton);
	void HandleSelectAudioInputFunction(const TDesC8& aData, TRemConCoreApiButtonAction aButton);
private :
	void RemConCoreSetFeatures(TBool* aFeatureSupported);

private: // unowned
	MRemConCoreApiTargetObserver& iObserver;

private: // owned
	TBuf8<KRemConCoreApiMaxOperationSpecificDataSize> iRspData;
	TUint iNumRemotes;
	TFixedArray<TBool,ELargestOperationId> iOutstandingOperations;	
	};

#endif // REMCONCOREAPITARGET_H

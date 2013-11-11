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

#ifndef REMCONCOREAPICONTROLLER_H
#define REMCONCOREAPICONTROLLER_H

#include <e32base.h>
#include <remcon/remconinterfacebase.h>
#include <remconcoreapi.h>
#include <remcon/remconinterfaceif.h>

class MRemConCoreApiControllerObserver;
class CRemConInterfaceSelector;

/**
Client-instantiable type supporting sending Core API commands.
*/
NONSHARABLE_CLASS(CRemConCoreApiController) : public CRemConInterfaceBase, 
									          public MRemConInterfaceIf
	{
public:
	/**
	Factory function.
	@param aInterfaceSelector The interface selector. The client must have 
	created one of these first.
	@param aObserver The observer of this interface.
	@return A new CRemConCoreApiController, owned by the interface selector.
	*/
	IMPORT_C static CRemConCoreApiController* NewL(CRemConInterfaceSelector& aInterfaceSelector, 
		MRemConCoreApiControllerObserver& aObserver);
	
	/** Destructor */
	IMPORT_C ~CRemConCoreApiController();

public:
	/** Send a 'select' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Select(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'up' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Up(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'down' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Down(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'left' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Left(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'right' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Right(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'right up' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void RightUp(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'right down' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void RightDown(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'left up' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void LeftUp(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'left down' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void LeftDown(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'root menu' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void RootMenu(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'setup menu' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void SetupMenu(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'contents menu' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void ContentsMenu(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'favorite menu' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void FavoriteMenu(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send an 'exit' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Exit(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a '0' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _0(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a '1' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _1(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a '2' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _2(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a '3' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _3(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a '4'command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _4(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a '5' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _5(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a '6' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _6(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a '7' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _7(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a '8' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _8(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a '9' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void _9(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'dot' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Dot(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'enter' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Enter(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'clear' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Clear(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'channel up' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void ChannelUp(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'channel down' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void ChannelDown(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'previous channel' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void PreviousChannel(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'sound select' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void SoundSelect(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'input select' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void InputSelect(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'display information' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void DisplayInformation(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'help' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Help(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'page up' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void PageUp(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'page down' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void PageDown(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'power' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Power(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'volume up' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void VolumeUp(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'volume down' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void VolumeDown(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'mute' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Mute(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'play' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	@param aSpeed The play speed.
	*/
	IMPORT_C void Play(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct,
		TRemConCoreApiPlaybackSpeed aSpeed = ERemConCoreApiPlaybackSpeedX1);

	/** Send a 'stop' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Stop(TRequestStatus& aStatus, 
		TUint& aNumRemotes,
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'pause' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Pause(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'record' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Record(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'rewind' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Rewind(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'fast forward' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void FastForward(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'eject' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Eject(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'forward' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Forward(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'backward' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Backward(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'angle' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Angle(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'subpicture' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void Subpicture(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'pause play function' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void PausePlayFunction(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'restore volume function' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void RestoreVolumeFunction(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'tune function' command.
	Only one command per controller session can be outstanding at any one time.
	@param aTwoPart If EFalse, only aMajorChannel is being used to perform the 
	tuning (aMinorChannel is ignored). Otherwise, both aMajorChannel and 
	aMinorChannel are used.
	@param aMajorChannel The major channel number.
	@param aMinorChannel The minor channel number.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void TuneFunction(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TBool aTwoPart, 
		TUint aMajorChannel, 
		TUint aMinorChannel,
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'select disk function' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	@param aDisk The disk to select.
	*/
	IMPORT_C void SelectDiskFunction(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TUint aDisk,
		TRemConCoreApiButtonAction aButtonAct);

	/** Send a 'select AV input function' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	@param aAvInputSignalNumber AV input signal.
	*/
	IMPORT_C void SelectAvInputFunction(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TUint8 aAvInputSignalNumber,
		TRemConCoreApiButtonAction aButtonAct);
	
	/** Send a 'select audio input function' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	@param aAudioInputSignalNumber Audio input signal.
	*/
	IMPORT_C void SelectAudioInputFunction(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TUint8 aAudioInputSignalNumber,
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'F1' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void F1(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'F2' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void F2(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'F3' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void F3(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'F4' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void F4(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);

	/** Send an 'F5' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@param aButtonAct The button action to send.
	*/
	IMPORT_C void F5(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct);
	
private:
	/** 
	Constructor.
	@param aInterfaceSelector The interface selector.
	@param aObserver The observer of this interface.
	*/
	CRemConCoreApiController(CRemConInterfaceSelector& aInterfaceSelector, 
		MRemConCoreApiControllerObserver& aObserver);

private: // utility
	/**
	Utility to send a command with only the button action as operation-specific data.
	@param aStatus TRequestStatus for the send operation.
	@param aNumRemotes The number of remotes the command was sent to.
	@param aButtonAct The button action.
	@param aOperationId The operation ID.
	*/
	void SendGenericCommand(TRequestStatus& aStatus, 
		TUint& aNumRemotes, 
		TRemConCoreApiButtonAction aButtonAct, 
		TRemConCoreApiOperationId aOperationId);

private: // from CRemConInterfaceBase
	TAny* GetInterfaceIf(TUid aUid);

private: // from MRemConInterfaceIf
	void MrcibNewMessage(TUint aOperationId, const TDesC8& aData);

private: // unowned
	MRemConCoreApiControllerObserver& iObserver;

private: // owned
	TBuf8<KRemConCoreApiMaxOperationSpecificDataSize> iCmdData;
	};

#endif // REMCONCOREAPICONTROLLER_H

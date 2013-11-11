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

#ifndef REMCONCOREAPITARGETOBSERVER_H
#define REMCONCOREAPITARGETOBSERVER_H

#include <e32base.h>
#include <remconcoreapi.h>

/**
Clients must implement this interface in order to instantiate objects of type 
CRemConCoreApiTarget. This interface passes incoming commands from RemCon to 
the client. 
In order to minimise the number of virtual functions the client has to 
implement, (a) they aren't pure, and (b) most commands are presented via 
'Command' with an operation ID. 
Commands which are associated with command-specific data come instead through 
specific methods, e.g. 'Play', which comes with a play speed.
Incoming commands are given to client via the MRemConCoreApiTargetObserver mixin. 
Having received a command, the client is obliged to send a response.
A client can only have one send outstanding at any time. 
If commands come in quickly, the client is responsible for queueing its responses to them. 
Note that each command delivered to a target involves memory allocated in the server’s heap, 
which is only released on client closure or when a response is sent. 
*/
class MRemConCoreApiTargetObserver
	{
public:
	/** 
	A command has been received. 
	@param aOperationId The operation ID of the command.
	@param aButtonAct The button action associated with the command.
	*/
	IMPORT_C virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
		TRemConCoreApiButtonAction aButtonAct);
	
	/** 
	A 'play' command has been received. 
	@param aSpeed The playback speed.
	@param aButtonAct The button action associated with the command.
	*/
	IMPORT_C virtual void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, 
		TRemConCoreApiButtonAction aButtonAct);

	/**
	A 'tune function' command has been received.
	@param aTwoPart If EFalse, only aMajorChannel is to be used. Otherwise, 
	both aMajorChannel and aMinorChannel are to be used.
	@param aMajorChannel The major channel number.
	@param aMinorChannel The minor channel number.
	@param aButtonAct The button action associated with the command.
	*/
	IMPORT_C virtual void MrccatoTuneFunction(TBool aTwoPart, 
		TUint aMajorChannel, 
		TUint aMinorChannel,
		TRemConCoreApiButtonAction aButtonAct);

	/** 
	A 'select disk function' has been received.
	@param aDisk The disk.
	@param aButtonAct The button action associated with the command.
	*/
	IMPORT_C virtual void MrccatoSelectDiskFunction(TUint aDisk,
		TRemConCoreApiButtonAction aButtonAct);
	
	/** 
	A 'select AV input function' has been received.
	@param aAvInputSignalNumber The AV input.
	@param aButtonAct The button action associated with the command.
	*/
	IMPORT_C virtual void MrccatoSelectAvInputFunction(TUint8 aAvInputSignalNumber,
		TRemConCoreApiButtonAction aButtonAct);

	/** 
	A 'select audio input function' has been received.
	@param aAudioInputSignalNumber The audio input.
	@param aButtonAct The button action associated with the command.
	*/
	IMPORT_C virtual void MrccatoSelectAudioInputFunction(TUint8 aAudioInputSignalNumber,
		TRemConCoreApiButtonAction aButtonAct);
	};

#endif // REMCONCOREAPITARGETOBSERVER_H

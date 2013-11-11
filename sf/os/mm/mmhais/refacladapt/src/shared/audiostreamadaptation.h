// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef AUDIOSTREAMADAPTATION_H
#define AUDIOSTREAMADAPTATION_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <badesca.h>
#include <a3f/a3fbase.h>

/**
@publishedAll
@released

Implementation of the Audio Component Version

*/

class TAudioComponentInfo
	{
	TInt iVersion;
	};

/**
@publishedAll
@released

Implementation of the Stream Adaptation

*/

class MAudioStreamAdaptationObserver;
class CFourCCConvertor;
class MLogicalChain;
class MAudioContext;

const TUid KAudioComponentManagerUid = {0x10283462};

// Messages flags
const TInt KServerStopping = 0x01;
const TInt KPreemptionRequest = 0x02;

/**
@publishedAll
@released

This is the interface to AudioStreamAdaptation.
This is represent the stream loaded when the MMRC server 
selects the adaptation.

This functionality includes audio playback and recording using different formats
CAudioStreamAdaptation provides the access point for clients to Adaptation Layer.
It is responsible for initializing connections to various subsystems
in audio adaptation. These include setting up and managing connection 
to the low level audio adaptation API implementations. The connections are then made
available .
*/
class CAudioStreamAdaptation : public CBase
	{

public:

	/**
	Constructs, and returns a pointer to, a new CAudioComponentManager object.

	@return A pointer to the CAudioComponentManager instance.
	*/
	static CAudioStreamAdaptation* NewL(TUid aType);

	/**
	Destructor.

	Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CAudioStreamAdaptation();


	/**
	Returns if a message was passed through the adaptation.
	
	@param MLogicalChain
		Logical Current Chain
	@param MLogicalChain
		Desired Chain after message
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
		another of the system-wide error codes.
	*/

	virtual TInt Message(MLogicalChain& aCurrentChain, MLogicalChain& aDesiredChain, MAudioContext& aContext, TInt aFlags) = 0;

	/**
	Returns if a Audio Stream Observer was Registered.
	
	@param MAudioStreamAdaptationObserver
		Observer object witch will be registered
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
		another of the system-wide error codes.
	*/

	virtual TInt RegisterAudioStreamObserver(MAudioStreamAdaptationObserver& aObserver) = 0 ;

	/**
	Returns if a Audio Stream Observer was UnRegistered.
	
	@param MAudioStreamAdaptationObserver
		Observer object witch will be unregistered
	*/

	virtual void UnregisterAudioStreamObserver(MAudioStreamAdaptationObserver& aObserver) = 0;

	virtual void UnregisterAllAudioStreamObserver() = 0;
	
	virtual void SetFourCC(const CFourCCConvertor& aFourCCConvertor) = 0;
	
	
private:

	/**	iDtor_ID_Key Instance identifier key. When instance	of an
	*	implementation is created by ECOM framework, the
	*	framework will assign UID for it. The UID is used in
	*	destructor to notify framework that	this instance is
	*	being destroyed	and	resources can be released.
	*/
	TUid iDtor_ID_Key;
	};

#include "audiostreamadaptation.inl"

#endif // C_AUDIOCOMPONENTMANAGER_H

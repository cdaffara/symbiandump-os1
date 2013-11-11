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



/**
 @file
 @publishedPartner 
 @released
*/


#ifndef A3FBASE_H
#define A3FBASE_H

#include <e32base.h>

/**
* Provides the process id of the original application.
*/ 
class TClientContextSettings
	{
public:
	/**
	* Application Process Id.
	*/
	TProcessId iProcessId; 
	};


/**
* States of a stream.
*/
enum TAudioState
	{
	/**Uninitialized state */    
	EUninitialized = 0,
	/**Initialized state */    
	EInitialized,
	/**Idle state */    
	EIdle,
	/**Primed state */    
	EPrimed,
	/**Active state */    
	EActive,
	/**Dead state */    
	EDead
	};

/**
* Audio type of a stream.
*/
class TAudioTypeSettings
	{
public:
	/**
	* Priority of client.
	*/
	TInt iPriority;

	/**
	* The priority preference of client.
	*/
	TInt iPref;
	};


/**
* Mode setting available.
*
* "Mode" in this context refers to the number of channels (i.e. mono or stereo).
* The modes are given as UIDs. 
*
*/

/** Mono data */ 
const TInt  KA3FModeMonoValue					= 0x1028348D;
const TUid	KA3FModeMono						= {KA3FModeMonoValue};
/** Stereo data, non-interleaved */ 
const TInt	KA3FModeStereoNonInterleavedValue	= 0x1028348E;
const TUid	KA3FModeStereoNonInterleaved		= {KA3FModeStereoNonInterleavedValue};
/** Interleaved Stereo data */ 
const TInt	KA3FModeStereoInterleavedValue		= 0x1028348F;
const TUid	KA3FModeStereoInterleaved			= {KA3FModeStereoInterleavedValue};
/** 5.1 encoded data */ 
const TInt	KA3FMode51Value						= 0x10283490;
const TUid	KA3FMode51							= {KA3FMode51Value};


/**
* Type class for audio channel.
*/
class TAudioChannelGain
	{
public:
	/**
	* Location of the channel in degrees on a circle located on the same plane as the listener,
	* with listener in the center of the circle.
	* Negative values are used for channels the location of which is not important.
	*/
	TInt iLocation;

	/**
	* Absolute gain value of channel.
	* Allowed values are 0 - MaxGain().
	*/
	TInt iGain;

	/**
	* Mapping of common channels to location degrees.
	*/    
	enum TStandardChannels
		{
		/** Low Frequency Effects Channels */  
		ELowFrequencyEffects = -1,
		/** Center Channels*/  
		ECenter = 0,
		/** Right Channels */  
		ERight = 45,
		/** Right Surround Channels */  
		ERightSurround = 90,
		/** Right Rear Channels */  
		ERightRear = 135,
		/** Rear Surround Channels */  
		ERearSurround = 180,
		/** Left Rear Channels */  
		ELeftRear = 225,
		/** Left Surround Channels */  
		ELeftSurround = 270,
		/** Left Channels */  
		ELeft = 315
		};
	};


/**
* KGainSawTooth – an explicit 0->aGain sweep, rising linearly. 
* If the channel is active, the effective gain drops to 0 ASAP after the request. 
* If used when the stream is not in EActive state, this acts as fade-in during the subsequent Activate() transistion.
*/
const TInt	KUidGainSawToothValue				= 0x10285CDC;
const TUid	KUidGainSawTooth					= {KUidGainSawToothValue};

/**
* KGainFadeOut – drop from current to 0, 
* dropping linearly over aRampDuration period.
*/
const TInt	KUidGainFadeOutValue				= 0x10285CDD;
const TUid	KUidGainFadeOut						= {KUidGainFadeOutValue};

/**
* KGainRamped – (gradual) change from current gain values to those given in aGain over aRampDuration period
*/
const TInt	KUidGainRampedValue					= 0x10285CDE;
const TUid	KUidGainRamped						= {KUidGainRampedValue};

/**
* KGainContinue – if a previous ramped operation is in progress, 
* this continues but the concept of “target gain” is updated 
* using the new values. aRampDuration is ignored 
* – the previous ramp duration is reused, minus the time already spent ramping.
* The smoothness of this operation will depend on the implementation. 
* If no ramped operation is in progress, this is the same as KNullUid. 
* This is intended as a way of changing gain values without stopping an ongoing ramp operation.
*/
const TInt	KUidGainContinueValue				= 0x10285CDF;
const TUid	KUidGainContinue					= {KUidGainContinueValue};


const TInt KErrA3fUnsupportedRamp = (-12250);

/**
 * Represents an unique Id for a specific context at runtime.
 */
class TAudioContextId : public TObjectId 
	{
public:
	/**
	 *Default constructor.
	 */
	TAudioContextId() {}

	/**
	 *Unique Id for a specific context at runtime.
	 *@param aId Unique id to be set.
	 */
	inline TAudioContextId(TUint64 aId) : TObjectId(aId) {}
	};
	
/**
 * Represents an unique Id for a specific audio processing unit at runtime.
 */
class TAudioComponentId : public TObjectId 
	{
	public:
	/**
	 * Default constructor.
	 */
	TAudioComponentId() {}

	/**
	 *Unique Id for a specific audio processing unit at runtime.
	 *@param aId Unique id to be set.
	 */
	inline TAudioComponentId(TUint64 aId) : TObjectId(aId) {}
	}; 

/**
 * Represents an unique Id for a specific audio stream unit at runtime.
 */
class TAudioStreamId : public TObjectId 
	{
	public:
	/**
	 * Default constructor.
	 */
	TAudioStreamId() {}

	/**
	 *Unique Id for a specific stream at runtime.
	 *@param aId Unique id to be set.
	 */
	inline TAudioStreamId(TUint64 aId) : TObjectId(aId) {}
	}; 


#endif // A3FBASE_H

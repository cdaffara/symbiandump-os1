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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MDFPULOADER_H
#define MDFPULOADER_H

#include <e32base.h>
class MMdfInputPort;
class MMdfOutputPort;
class CMdfProcessingUnit;
class MMdfProcessingUnitObserver;
class TFourCC;

/**
Utility class used by the HwDevice to load Processing Units.
*/
class CMdfPuLoader : public CBase
	{
public:
	/**
	Constructs an instance of the ECom plugin.
	@param	aImplementationUid
			ECom implementation UID.
	@return	A pointer to the newly constructed object
	*/
	inline static CMdfPuLoader* NewL(TUid aImplementationUid);
	
	/**
	Loads a Processing Unit using a UID synchronously.
	@param	aProcessingUnitObserver
			The observer for the newly loaded Processing Unit.
	@param	aImplementationUid
			ECom implementation UID.
	@return  A pointer to the newly loaded Processing Unit.
	*/
	virtual CMdfProcessingUnit* LoadProcessingUnitL(const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		TUid aImplementationUid) = 0;

	/**
	Loads an audio Processing Unit synchronously.  The Processing Unit is
	resolved according to the source and destination data types provided.
	@param	aProcessingUnitObserver
			The observer for the newly loaded Processing Unit.
	@param	aSrcDataType
			The source data type.
	@param	aDestDataType
			The destination data type.
	@return	A pointer to the newly loaded Processing Unit.	   
	*/
	virtual CMdfProcessingUnit* LoadProcessingUnitL( const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		TFourCC aSrcDataType, TFourCC aDestDataType) = 0;
	

	/**
	Loads a video Processing Unit synchronously.  The Processing Unit is
	resolved according to the source and destination data types provided.
	@param	aProcessingUnitObserver
			The observer for the newly loaded Processing Unit.
	@param	aSrcDataType
			The source data type.
	@param	aDestDataType
			The destination data type.
	@param	aImplementationType
			The implementation type. Can be audio, video encoding or video decoding.
	@return	A pointer to the newly loaded Processing Unit.	   
	*/
	virtual CMdfProcessingUnit* LoadProcessingUnitL( const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		const TDesC8& aSrcDataType, const TDesC8& aDestDataType, const TUid& aImplementationType) = 0;

	
	/**
	Attempts to set-up a tunnel from a given output port to an input port.  
	The data flow is from the output port to the input port.  This method is synchronous.
	@param	aOutputPort
			The output port of the tunnel.
	@param	aInputPort
			The input port of the tunnel.
	@return	An error code indicating if the function call was successful. KErrNone on success, otherwise
			another of the system-wide error codes.	  
	*/
	virtual TInt TunnelSetup(MMdfOutputPort& aOutputPort, MMdfInputPort& aInputPort) = 0;
	
	/**
	Unloads a Processing Unit synchronously.
	@param	aPu
			The pointer to the Processing Unit to be unloaded.
	*/
	virtual void UnloadProcessingUnit(CMdfProcessingUnit*& aPu) = 0;
	
	/**
	Standard destructor.
	*/
	virtual inline ~CMdfPuLoader();
private:
	TUid iDtor_ID_Key;
	};

#include <mdf/mdfpuloader.inl>
#endif // MDFPULOADER_H

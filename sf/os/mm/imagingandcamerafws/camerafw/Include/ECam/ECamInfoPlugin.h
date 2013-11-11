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
 @publishedPartner
 @released
*/

#ifndef ECAMINFOPLUGIN_H
#define ECAMINFOPLUGIN_H

#include <ecam.h>

class CCameraPlugin;

/**
Plugin used to supply static data for CCamera.
*/
class CCameraInfoPlugin : public CBase
	{
	friend class CCameraPlugin;
	
public:
	
	/**
	Default destructor for this class.
	*/
	IMPORT_C ~CCameraInfoPlugin();
	
	/** 
	Determines the number of cameras on the device.

    @return Count of cameras present on the device.
	*/
	virtual TInt CamerasAvailable()=0;
	
protected:

	/**
	Default constructor for this class.
	*/
	IMPORT_C CCameraInfoPlugin();
	
private:	
	/**
	Loads the plugin and returns a pointer to it.
	
	@return Pointer to the plugin.
	*/
	static CCameraInfoPlugin* NewL();

private:
	TUid iDestructorKey;
	
	};
	
/**
@publishedPartner
@prototype

'Property' in the Publish/Subscribe API.
Used by the ECAM implementation as property sub-keys while defining the property within the category given by 
current process's secureID.
*/
static const TUint KUidECamPropertyCameraIndex0ReservedStatus = 0x102831F1;

/**
@publishedPartner
@prototype

'Property' in the Publish/Subscribe API.
Used by the ECAM implementation as property sub-keys while defining the property within the category given by 
current process's secureID.
*/
static const TUint KUidECamPropertyCameraIndex1ReservedStatus = 0x102831FA;

/**
@publishedPartner
@prototype

'Property' in the Publish/Subscribe API.
Used by the ECAM implementation as property sub-keys while defining the property within the category given by 
current process's secureID.
*/
static const TUint KUidECamPropertyCameraIndex2ReservedStatus = 0x102831FB;

/**
@publishedPartner
@prototype

'Property' in the Publish/Subscribe API.
Used by the ECAM implementation as property sub-keys while defining the property within the category given by 
current process's secureID.
*/
static const TUint KUidECamPropertyCameraIndex3ReservedStatus = 0x102831FC;

/**
@publishedPartner
@prototype

'Property' in the Publish/Subscribe API.
Used by the ECAM implementation as property sub-keys while defining the property within the category given by 
current process's secureID.
*/
static const TUint KUidECamPropertyCameraIndex4ReservedStatus = 0x102831FD;

/**
@publishedPartner
@prototype

'Property' in the Publish/Subscribe API.
Used by the ECAM implementation as property sub-keys while defining the property within the category given by 
current process's secureID.
*/
static const TUint KUidECamPropertyCameraIndex5ReservedStatus = 0x102831FE;

/**
@publishedPartner
@prototype

'Property' in the Publish/Subscribe API.
Used by the ECAM implementation as property sub-keys while defining the property within the category given by 
current process's secureID.
*/
static const TUint KUidECamPropertyCameraIndex6ReservedStatus = 0x102831FF;

/**
@publishedPartner
@prototype

'Property' in the Publish/Subscribe API.
Used by the ECAM implementation as property sub-keys while defining the property within the category given by 
current process's secureID.
*/
static const TUint KUidECamPropertyCameraIndex7ReservedStatus = 0x10283200;

/**
Plugin used to retrieve the secure ID of the process where serialized part of the ECam implementation runs.

Meant for implementation by the ECam implementers.

Implementation has to define and publish the properties as given by the keys KUidECamPropertyCameraIndex0ReservedStatus 
to KUidECamPropertyCameraIndex7ReservedStatus. Client's responsibility is to subscribe to these properties using 
TReservedInfo::SubscribeReserveInfoL and hence implement the interface MReserveObserver.

@publishedPartner
@prototype
*/	
class MSecureIdPlugin
	{
public:
	virtual void Release() =0;
	/**
	Used to retrieve the Secure ID of the process where serialized part of the ECam implementation runs. Secure ID will be
	internally used to attach to the Properties for which the Reserve notification will be subscribed for.
	
	@param aSecureId
		   The secure ID of the process where serialized part of the ECam implementation runs.
		   
	@leave May leave with any error code.
	
	@note Serialized part of the ECam implementation is supposed to define and publish those Properties.
	*/
	virtual void GetSecureIdL(TInt& aSecureId) const =0;
	};

#endif // ECAMINFOPLUGIN_H

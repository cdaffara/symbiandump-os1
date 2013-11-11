/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef RESOURCEDATA_H
#define RESOURCEDATA_H

// INCLUDES
#include <e32base.h>
#include <barsread.h>
#include <mmf/server/mmfhwdevice.h>
#include "audiocodec.h"

_LIT16(KDrive,"z:");
_LIT16(KFileNameFormat,"HwDev0x%x.rsc");
const TInt8 KResFileNameLength = 20;
const TInt8 KResFileNameAndPathLength = 50;
// CLASS DECLARATION

/////
// Class is used to read capabilities and initilaization data from codec
// resource data files.
///
NONSHARABLE_CLASS (CResourceData) : public CBase
	{
public: // Constructors and destructor

	/////
	 // Two-phased constructor.
	 //
	 // @param aResourceUid       UID of the resource whose data to read.
	 // @return CResourceData*  Pointer to a newly constructed object.
	 ///
	static CResourceData* NewL(TUid aResourceUid);

	/////
	 // Destructor.
     ///
	~CResourceData();

public:

	/////
	 // Reads the capabilities data.
	 //
	 // @param aMode      Mode of the resource.
	 // @param aConf      On return from a succesful call, contains the capabilities data.
 	 // @return           error code
	 ///
	TInt GetSSampleRates(TMode aMode, RArray<TInt>& aSampleRate);
	TInt GetSModes(TMode aMode, RArray<TUid>& aModeConfig);

private:

	
	//Constructor.
	
	CResourceData();

	
	//2nd phase constructor is private.
	//The param is the UID of the resource whose data to read.
	
	void ConstructL(TUid aResourceUid);

	
	//Utility method for seeking resource to the correct mode. Leaves with
	//KErrNotFound if mode is not found in the opened resource.
	
	//aMode is the codec mode.
	
	TInt SeekToMode(TMode aMode);

private: // Data

	// Resource UID.
	TUid iResourceUid;

	// Resource file data.
	HBufC8* iResourceData;

	// Resource file reader.
	TResourceReader iResourceReader;

	// Resource data end pointer (for safety checks).
	const TUint8* iResourceDataEnd;
	};

#endif // RESOURCEDATA_H

// End of File

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
* Description:   Provides the interface between SUPL Framework and POS
*				 Message plugin. 
*
*/



// INCLUDE FILES
#include "epos_tomasuplposprotocolversion.h"

// ============================ MEMBER FUNCTIONS OF TOMASuplPosProtocolVersion ===============================

// -----------------------------------------------------------------------------
// TOMASuplPosProtocolVersion::TOMASuplPosProtocolVersion,constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TOMASuplPosProtocolVersion::TOMASuplPosProtocolVersion()
	{       
	iMajorVersion = 0;
	iTechVersion = 0;
	iMinorVersion = 0;
	}
// -----------------------------------------------------------------------------
// TOMASuplPosProtocolVersion::GetPosProtocolVersion()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TOMASuplPosProtocolVersion::GetPosProtocolVersion(TUint8& aMajorVersion, 
									  TUint8& aTechVersion,
									  TUint8& aMinorVersion)const 
	{
	
		if (iMajorVersion == 0 && iTechVersion == 0 && iMinorVersion == 0) 
			return KErrNotFound;
		else
		{
				aMajorVersion = iMajorVersion;
				aTechVersion = iTechVersion;
				aMinorVersion = iMinorVersion;
				
				return KErrNone;
		}
	}
// -----------------------------------------------------------------------------
// TOMASuplPosProtocolVersion::SetPosProtocolVersion()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TOMASuplPosProtocolVersion::SetPosProtocolVersion(TUint8 aMajorVersion, 
									   TUint8 aTechVersion,
									   TUint8 aMinorVersion)
	{
		// version numbers can be between 0 and 255 only
		// unsigned integers - no need to check >=0 (causes compiler warning)
		if ( ( aMajorVersion <= 255 ) && ( aTechVersion <= 255 ) && ( aMinorVersion <= 255 ) )
		{
	 	 	 iMajorVersion = aMajorVersion;
			 iTechVersion = aTechVersion;
			 iMinorVersion = aMinorVersion;
			 
			 return KErrNone;
		}
		else 
		return KErrArgument;
	}


//  End of File  

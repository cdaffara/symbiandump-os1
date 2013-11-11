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
* Description:  Header file for the different data types that
*                can be exchanged between POS Message plugin and
*			     SUPL FW
*
*/


#ifndef C_TOMASUPLPOSPROTOCOLVERSION_H
#define C_TOMASUPLPOSPROTOCOLVERSION_H

#include <e32def.h>
#include <e32std.h>

/**
* @Description TOMASuplPosProtocolVersion determines the underlying control plane
* positioning protocol version. The different possible values for the positioning 
* protocol version are 
* - Major Version (First element of the version number for POS Protocol, range: (0..255)) 
* - Technical Version (Second element of the version number, range: (0..255)) 
* - Minor Version (Third element of the version number for RRC, range: (0..255)) 
*
*  @since Series 60 5.2
**/
class TOMASuplPosProtocolVersion
{

public: // Construction

     IMPORT_C TOMASuplPosProtocolVersion();
     

public:	//Getter Methods

	   /**
	    * @Description GetPosProtocolVersion retrieves the underlying control plane positioning
	    * protocol Version. SUPL Framework invokes this method.
	    * @param aMajorVersion A integer reference Parameter for POS 
        * Protocol Major version 
	    * @param aTechVersion A integer reference Parameter for POS 
        * Protocol Technical version 
	    * @param aMinorVersion A integer reference Parameter for POS 
        * Protocol Minor version 
        * @return KErrNone if protocol version is present. Else KErrNotFound is returned.
	    */  
	    IMPORT_C TInt GetPosProtocolVersion(TUint8& aMajorVersion, 
									  TUint8& aTechVersion,
									  TUint8& aMinorVersion) const;
	
public: // Setter Methods

	   /**
	    * @Description SetPosProtocolVersion is invoked by POS Message plug-in to set the
	    * underlying control plane positioning protocol version.
	    * @param aMajorVersion A integer Parameter for POS Protocol Major version, range: (0..255) 
	    * @param aTechVersion A integer Parameter for POS Protocol Technical version, range: (0..255) 
	    * @param aMinorVersion A integer Parameter for POS Protocol Editorial version, range: (0..255) 
        * @return KErrArgument If any of the parameter is not in range 
	    */  
        
	  IMPORT_C TInt SetPosProtocolVersion(TUint8 aMajorVersion, 
									   TUint8 aTechVersion,
									   TUint8 aMinorVersion);
	
private :	//Data members , Positioning protocols
	TUint8 iMajorVersion; 
	TUint8 iTechVersion;
	TUint8 iMinorVersion;
};
// End of TOMASuplPosProtocolVersion


#endif //  C_TOMASUPLPOSPROTOCOLVERSION_H

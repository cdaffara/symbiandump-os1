// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Location Based Services PSK API
// Note this file is not exported and remains in prototype status.
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/
#ifndef LBS_PSK_BASE_H
#define LBS_PSK_BASE_H


//-------------------------------------------------------------------------------
#include <e32base.h>

//-------------------------------------------------------------------------------
class TLbsPskParams;


//-------------------------------------------------------------------------------
/**
The CLbsPskBase is the abstract interface to a licensee specific PSK plug-in responsible for
deciding which PSK identity and value should be used to secure a connection. 

It is normally used by the LBS SUPL Protocol Module to secure connections to SUPL Servers.

An actual implementation of a PSK plug-in will derive from CLbsPskBase to provide the specific 
implementation of the interface methods.

Not yet supported/implemented.
*/
class CLbsPskBase : public CBase
	{
public:
#pragma message("NOTE: Location of the UID TBD. Possibly in the SUPL PM Central Rep file")
#pragma message("NOTE: OR there should be only one PSK plug-in? TBD")

	/** Creates an instance of the CLbsPskBase implementation with the given Uid.

	@param aImplementationUid [In] Identifies the plug-in to load.
	@param aParams            [In] The parameters passed to the plug-in when it is instantiated.
								   Currently not used.
	
	*/
	IMPORT_C static CLbsPskBase* NewL(TUid aImplementationUid, TLbsPskParams& aParams);

	/** Called during the TLS PSK handshake to get the PSK identity and value to be used to secure the connection.

	@param aHostNameAddress [In]  A pointer to a string containing an address of a server. 
								  The pointer may be NULL if the address is unknown. Data ownership is not passed.
	@param aPskIdentityHint [In]  A pointer to a string containing a "PSK identity hint".
								  The pointer may be NULL if the hint is unknown. Data ownership is not passed.
	@param aPskIdentity     [Out] NULL passed in, must be set to an HBufC8 containing the PSK Identity to be used.
								  Data ownership is passed back to the caller.
	@param aPskKey          [Out] NULL passed in, must be set to an HBufC8 containing the PSK key value to be used.
								  Data ownership is passed back to the caller.

	Note that the meaning of the PSK identity hint is NOT defined by the TLS standard, therefore any application
	using PSK must previously agree the source of the PSK to be used and the interpretation of the (optional) PSK identity 
	hint.*/
	virtual void GetPskL(const TDesC8* aHostNameAddress, const TDesC8* aPskIdentityHint, HBufC8*& aPskIdentity, HBufC8*& aPskKey) = 0;
	

	/**
	Standard destructor that handles the ECom destruction.
	The LBS subsystem invokes the destructor when it wishes to 
	unload the module. */
	IMPORT_C ~CLbsPskBase();

	/** Currently not implemented */
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	
private:
	//** Prohibit copy constructor */
	CLbsPskBase(const CLbsPskBase&);
	//** Prohibit assigment operator */
	CLbsPskBase& operator= (const CLbsPskBase&);

private:
	/** Reserved for future use */
	TAny* iReserved;
	
	/** Instance identifier key */
	TUid iDtor_ID_Key;
	};


//-------------------------------------------------------------------------------
/** Reserved for future use */
class TLbsPskParams
	{
	/** Default constructor */
	IMPORT_C TLbsPskParams();
	
private:
	TUint8 iReserved[16];
	};

#endif //LBS_PSK_BASE_H

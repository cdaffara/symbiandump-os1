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
// @file imsextn.h
// Header file for IMS API
// @internalTechnology
// @prototype
//

#ifndef __IMSEXTN_H__
#define __IMSEXTN_H__

#include <e32std.h>
#include <in_sock.h>
#include <es_mbuf.h>
#include <networking/qos_extension.h>

/** 
This class contain the IMS Signalling Indicator flag for 3GPP R5 dedicated 
signalling PDP contexts

Connection level APIs, such as RSubConnection, shall be extended to support 
dedicated PDP context for special charging and enhanced QoS.

@internalTechnology
@prototype
*/
class TImsParameter
{
public:
    /**	
    Constructor
	*/
    IMPORT_C explicit TImsParameter();
    
    /** 
    Gets the IMS Signalling Indicator flag
    
	@return IMS Signalling Indicator flag
	*/
    IMPORT_C TBool GetIMSSigallingIndicator() const;
    
    /**	
    Sets the IMS Signalling Indicator flag
	
	@param aIMSSignallingIndicator Value to which to set the IMS Signalling Indicator flag
	*/
    IMPORT_C void SetIMSSigallingIndicator(const TBool aIMSSignallingIndicator);
    
protected:
    TBool iIMSSignallingIndicator;	// IMS Signalling Indicator flag
};

/** 
This is the IMS policy extension. The IMS policy can be extended by adding the 
CSubConIMSExtensionParamSet object at the client side

@internalTechnology
@prototype
*/
class CImsPolicy : public CExtensionBase
{

public:
	/** 
	Two phase constructor. Creates a CImsPolicy object.
	
	@return A pointer to CImsPolicy object.
	@exception Leaves if no memory is available.
	*/
	IMPORT_C static CImsPolicy* NewL();
	
	/**
	Destructor.
	*/
	IMPORT_C ~CImsPolicy();
	
	/**
	Returns IMS policy extension in a descriptor. This is used by ipscpr.
	
	@return Descriptor that contains ims policy.
	*/
	IMPORT_C TDesC8& Data();
	
	/**
	Parses a IMS policy extension given in a descriptor. This is used by ipscpr.
	
	@return KErrNone if aData contains a valid IMS policy extension, otherwise KErrGeneral.
	*/
	IMPORT_C TInt ParseMessage(const TDesC8& aData);
	
	/**
	Creates a CImsPolicy object. This is used by ipscpr.
	
	@return A pointer to CImsPolicy object.
	@exception Leaves if no memory is available.
	*/
	IMPORT_C CExtensionBase* CreateL();
	
	/**
	Copies the parameters from aExtension object to this object. aExtension must be a 
	CImsPolicy object. If some other extension is given as a parameter, KErrArgument is returned.
	
	@param aExtension A CImsPolicy object that is copied into this object.
	@return KErrNone, aExtension is a CImsPolicy object, otherwise KErrArgument.
	*/
	IMPORT_C TInt Copy(const CExtensionBase& aExtension);
	
	/**
	Sets the IMS QoS parameter set.
	
	@param aIms contains the Ims QoS parameter.
	*/
	IMPORT_C void SetImsParameter(const TImsParameter& aIms);
	
	/**
	Gets the IMS QoS parameter set.
	
	@param aIms IMS QoS parameter set will be copied to aIms.
	*/
	IMPORT_C void GetImsParameter(TImsParameter& aIms) const;

protected:
	CImsPolicy();
	void ConstructL();

private:
	TImsParameter iIms;	// IMS QoS Extn parameter set
};

#endif // __IMSEXTN_H__

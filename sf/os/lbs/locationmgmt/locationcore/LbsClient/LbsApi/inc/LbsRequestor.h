// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LBSREQUESTOR_H__
#define __LBSREQUESTOR_H__

#include <e32base.h>

// Forward declarations
class RReadStream;
class RWriteStream;


class CRequestorBase : public CBase
/**
Each instance of a CRequestor class is used to hold the identity of one
of the parties involved requesting the location. The class contains three
data fields that indicate:
(1) If the requesting party is a "service" or an actual person ("contact").
(2) A descriptor that identifiers the requestor and
(3) a field that indicates which format the information is in
    - for example, a telephone number, URL or email address.

@publishedAll
@released
 */
	{
public:
	/** defined type for TRequestorType */
	typedef TInt TRequestorType;

	/** 
	TRequestorType
	 */
	enum _TRequestorType
		{
		/** Unknown Requestor */
		ERequestorUnknown,
		/** Requestor of type Service */
		ERequestorService,
		/** Requestor of type Contact */
		ERequestorContact
		};

	/** defined type for TRequestorFormat */
	typedef TInt TRequestorFormat;

	/** 
	TRequestorFormat
	 */
	enum _TRequestorFormat
		{
		/** Requestor format unknown */
		EFormatUnknown,
		/** EFormatApplication */
		EFormatApplication,
		/** EFormatTelephone */
		EFormatTelephone,
		/** EFormatUrl */
		EFormatUrl,
		/** EFormatMail */
		EFormatMail
		};

	IMPORT_C ~CRequestorBase();

	IMPORT_C void SetRequestorL(TRequestorType aType,
	                            TRequestorFormat aFormat,
	                            const TDesC& aData);
	IMPORT_C void GetRequestor(TRequestorType& aType,
	                           TRequestorFormat& aFormat,
	                           TPtrC& aData) const;

	IMPORT_C TRequestorType RequestorType() const;
	IMPORT_C TRequestorFormat RequestorFormat() const;
	IMPORT_C TDesC& RequestorData() const;

	IMPORT_C virtual void InternalizeL(RReadStream& aStream);
	IMPORT_C virtual void ExternalizeL(RWriteStream& aStream) const;

protected:
	IMPORT_C CRequestorBase();

	IMPORT_C void ConstructL(TRequestorType aType,
							 TRequestorFormat aFormat,
							 const TDesC& aData);
	// Reserved for future expansion - derived classes should see documentation
	// on how this is to be used.
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);

protected:
	/** Requestor type */
	TRequestorType   iRequestorType;
	/** Requestor format */
	TRequestorFormat iFormat;
private:
	/** This is owned by the CRequestor */
	HBufC* iData;
	/** Reserved data for future extension */
	TAny* iBaseReservedPtr;
	};


class CRequestor : public CRequestorBase
/**
CRequestor class for LBS clients
@see CRequestorBase 
@publishedAll
@released
 */
	{
public:
	IMPORT_C static CRequestor* New(TRequestorType aType,
	                                TRequestorFormat aFormat,
	                                const TDesC& aData);
	IMPORT_C static CRequestor* NewL(TRequestorType aType,
	                                 TRequestorFormat aFormat,
	                                 const TDesC& aData);
	IMPORT_C static CRequestor* NewLC(TRequestorType aType,
	                                  TRequestorFormat aFormat,
	                                  const TDesC& aData);
	IMPORT_C static CRequestor* NewL(RReadStream& aStream);

	/**
	Destructor for CRequestor.
	*/
	~CRequestor();

private:
	CRequestor();
	};

class RRequestorStack : public RPointerArray<CRequestor>
/**
Most standard applications will not use the RRequestorStack. Instead,
they will call the simpler RPositioner::SetRequestor() method to
identify themselves.

RRequestorStack will typically only be used if the application needs to
identify a chain of requestors. For example, a remote party is requesting
the location and this is routed through a local application. In this
situation, the application should identify both itself and the remote party.

@see RPointerArray
@publishedAll
@released
 */
	{
public:
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private:
	static void CleanupResetAndDestroy(TAny* aArray);

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[8];
	};

#endif //__LBSREQUESTOR_H__

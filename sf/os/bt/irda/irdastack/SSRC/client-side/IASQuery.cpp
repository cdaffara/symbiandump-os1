// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll
*/

#include <ir_sock.h>
#include "client_util.H"

EXPORT_C TIASQuery::TIASQuery()
/** Constructs an empty IAS query object.

The class name, the attribute and the remote device name must be set before 
this query object can be used. */
	{
	}

EXPORT_C TIASQuery::TIASQuery(const TDesC8& aClass,const TDesC8& aAttribute,TUint aRemoteDevAddr)
/** Constructs a new IAS query taking the class name, the attribute and the remote 
device address.

@param aClass The IAS query class name. 
@param aAttribute The IAS query attribute. 
@param aRemoteDevAddr The 32 bit address of the remote device to be queried. 
This is needed because the underlying IrLAP connection may not yet have been 
established. */
	{
	Set(aClass,aAttribute,aRemoteDevAddr);
	}

//
// Construct a descriptor with an IAS query inside it.
//
EXPORT_C void TIASQuery::Set(const TDesC8& aClass,const TDesC8& aAttribute,TUint aRemoteDevAddr)
/** Changes the class name, the attribute and the remote device address of the 
IAS query.

@param aClass The IAS query class name. 
@param aAttribute The IAS query attribute. 
@param aRemoteDevAddr The 32 bit address of the remote device to be queried. */
	{
	Zero();
	*WPtr()=(TUint8)aClass.Length();
	SetLength(1);

	//handle class name and attribute
	Append(aClass);
	SetLength(Length()+1);
	operator[](Length()-1)=(TUint8)aAttribute.Length();
	Append(aAttribute);

	//handle device address
	__ASSERT_ALWAYS(Length()+4<=MaxLength(),IrdaUtil::Panic(EIASQueryDes8Overflow));
	BigEndian::Put32(WPtr()+Size(),aRemoteDevAddr);
	SetLength(Length()+4);
	}

//
// Dole out our contents
//
EXPORT_C void TIASQuery::Get(TDes8& aClass,TDes8& aAttribute,TUint& aRemoteDevAddr)
/** Retrieves the detail of this IAS query.

@param aClass On return, contains the IAS query class name. 
@param aAttribute On return, contains the IAS query attribute. 
@param aRemoteDevAddr On return, contains the 32 bit address of the remote 
device to be queried. */
	{
	aClass.Copy(Ptr()+1,operator[](0));
	aAttribute.Copy(Ptr()+1+operator[](0),operator[](operator[](0)+1));
   	aRemoteDevAddr=BigEndian::Get32(Ptr()+Length()-4);
	}

// EOF

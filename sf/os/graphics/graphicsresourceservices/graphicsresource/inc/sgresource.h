// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @prototype
*/

#ifndef SGRESOURCE_H
#define SGRESOURCE_H

#include <e32cmn.h>
#include <graphics/sgconst.h>


/**
@publishedPartner
@prototype
@deprecated

Set of static functions in the Graphics Resource API.
*/
class SgDriver
	{
public:
	IMPORT_C static TInt Open();
	IMPORT_C static void Close();
	IMPORT_C static TInt ResourceCount();
	IMPORT_C static void AllocMarkStart();
	IMPORT_C static void AllocMarkEnd(TInt aCount);
	IMPORT_C static void SetAllocFail(RAllocator::TAllocFail aType, TInt aRate);
	};


/**
@publishedPartner
@prototype
@deprecated

A user-defined graphics resource attribute. The Graphics Resource driver can attach
attributes defined by the user to a resource at creation time but does not interpret
them. The attributes cannot be changed after the resource has been created.
*/
struct TSgUserAttribute
	{
	/**
	The globally unique identifier of the attribute.
	*/
	TUid iUid;
	/**
	The value of the attribute.
	*/
	TInt iValue;
	};


/**
@publishedPartner
@prototype
@deprecated

This class is used to uniquely identify drawable resources. It represents an opaque
192-bit identifier that is unique across the system but not persistent.
*/
NONSHARABLE_CLASS(TSgDrawableId)
	{
public:
	inline TBool operator ==(const TSgDrawableId& aId) const;
	inline TBool operator !=(const TSgDrawableId& aId) const;
public:
	/**
	The identifier stored as an array of six 32-bit integers. The contents have
	no meaning to the user, except that all zeros represents a null identifier.
	*/
	TUint32 iId[6];
	};


/**
@publishedPartner
@prototype
@deprecated

This constant represents the null drawable resource identifier, defined as 192 zero bits.
*/
const TSgDrawableId KSgNullDrawableId = {0, 0, 0, 0, 0, 0};


/**
@publishedPartner
@prototype
@deprecated

This globally unique identifier represents the handle type for instances of RSgDrawable.
*/
const TUid KSgDrawableTypeUid = {0x102858EB};


class MSgDrawableAdapter;

/**
@publishedPartner
@prototype
@deprecated

A handle to a reference-counted graphics resource that can be drawn in different
rendering pipelines, for example DirectGDI, OpenVG etc. Instances of RSgDrawable
allow access to drawable resources created using other APIs without knowing the
concrete type of the resource. Since drawable resources are reference-counted they
are guaranteed to exist while there are open handles referencing them.

A new RSgDrawable handle does not refer to a drawable resource until a successful
call to Open(). Before that point, the handle is said to be a null handle. Instances
of RSgDrawable can be shared among threads in the same process.

An RSgDrawable handle is said to be invalid if it is not null but it does not
reference an existing drawable resource. Copying an instance of RSgDrawable must
be done with extreme care, since it does not increment the reference count of the
referenced drawable resource and may therefore allow some RSgDrawable handle to
become invalid when the drawable resource is destroyed.
*/
NONSHARABLE_CLASS(RSgDrawable)
	{
public:
	IMPORT_C RSgDrawable();
	IMPORT_C TInt Open(const TSgDrawableId& aId);
	IMPORT_C TInt Open(const TSgDrawableId& aId, TUint32 aMode);
	IMPORT_C void Close();
	IMPORT_C const TSgDrawableId& Id() const;
	IMPORT_C TBool IsNull() const;
	IMPORT_C TUid DrawableType() const;
	inline TUid HandleType() const;
	template<class M> inline TInt GetInterface(M*& aInterfacePtr);
	template<class M> inline TInt GetInterface(const M*& aInterfacePtr) const;
private:
	IMPORT_C TInt GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr) const;
protected:
	TUid iHandleType;
	MSgDrawableAdapter* iImpl;
	};


#include <graphics/sgresource.inl>

#endif // SGRESOURCE_H

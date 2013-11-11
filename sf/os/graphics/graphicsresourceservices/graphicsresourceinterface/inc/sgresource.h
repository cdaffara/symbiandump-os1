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
// Graphics Resource - general functionality
//

#ifndef SGRESOURCE_H
#define SGRESOURCE_H

#include <e32std.h>
#include <sgresource/sgconst.h>

/**
Panic codes defined for use by the Graphics Resource API.
*/
enum TSgResourcePanicReason
	{
	ESgPanicNoDriver = 1,
	ESgPanicUnclosedResources = 2,
	ESgPanicBadDrawableHandle = 3,
	ESgPanicBadAttributeArrayIndex = 4
	};

/**
Panics the calling thread with category “SGRES” and the given panic code.
@param aReason The panic code.
*/
inline void Panic(TSgResourcePanicReason aReason);

/**
A handle to the implementation of the Graphics Resource API 
initialised in the context of a process. It groups miscellaneous functions that 
do not apply to individual drawable resources.

At least one thread in the process must call RSgDriver::Open() before the 
Graphics Resource API can be used in the context of the process. 
When the Graphics Resource API is no longer needed RSgDriver::Close() should be 
called to release unused resources.  Note that it is possible to call RSgDriver::Open() 
and RSgDriver::Close() from different threads.
*/
NONSHARABLE_CLASS(RSgDriver)
	{
public:
	/**
	Default constructor.
	@post The instance of RSgDriver is a null handle.
	*/
	inline RSgDriver();

	/**
	Initialises the implementation of the Graphics Resource API in the context 
	of a process if it is not initialised already, updates the implementation 
	opening count in the process and returns KErrNone if successful.
	Each successful call to RSgDriver::Open() in a process should be balanced 
	by a corresponding call to RSgDriver::Close() in the same process, but not 
	necessarily from the same thread.

	Note that an RSgDriver handle should never be bitwise-copied into another 
	but this function should be used instead in all cases, since it is the only 
	way to ensure that the implementation opening count is kept up to date.
	Failure to keep the implementation opening count up to date may result in 
	the implementation of the Graphics Resource API becoming unexpectedly 
	unavailable and, as a consequence, in an abnormal termination of the process.

	@pre The instance of RSgDriver is a null handle.
	@post The instance of RSgDriver references the implementation of the Graphics 
		Resource API initialised in the context of the process.
	@return KErrNone if successful;
		KErrInUse if the instance of RSgDriver is not a null handle;
		KErrNoMemory if there is not enough system memory to initialise the 
			implementation of the Graphics Resource API;
		KErrNoGraphicsMemory if there is not enough specialised graphics memory
			to initialise the implementation of the Graphics Resource API.
	*/
	IMPORT_C TInt Open();

	/**
	Decrements the implementation opening count in the process.
	When the count reaches zero, checks whether there are still open handles to
	drawable resources in the process and carries out termination tasks to
	release the internal resources used by the implementation of the Graphics
	Resource API.

	An attempt to carry out these termination tasks will panic with category
	“SGRES” and code 2 in debug builds if there still are any open handles to 
	drawable resources in the process. In release builds all the open handles 
	to drawable resources in the process are silently closed at this point.
	Calling this function on a null handle has no effect.

	@post The instance of RSgDriver is a null handle.
	@panic SGRES 2 Debug builds only.  There are open handles to drawable resources 
		in the process during termination.
	*/
	IMPORT_C void Close();

	/**
	Makes the extension interface specified by the type of the parameter 
	aInterfacePtr available in the context of the process.
	This function stores a pointer to the extension interface in the variable 
	referenced by the parameter aInterfacePtr and returns KErrNone if successful.

	@pre An RSgDriver has been opened in the context of the process.
	@param[out] aInterfacePtr Reference to a pointer to the interface class that on return points 
		to the extension interface or, in case of error, is set to NULL.
	@return KErrNone if successful;
		KErrBadHandle if the instance of RSgDriver is a null handle;
		KErrExtensionNotSupported if the requested extension interface is not supported.
	*/
	template<class M> inline TInt GetInterface(M*& aInterfacePtr) const;

	/**
	Returns the version of the implementation of the Graphics Resource API.
	The version comprises 3 numbers: major version number, minor version number 
	and build number.

	Major revisions mean incompatible changes in the API. 
	Minor revisions mean forward-compatible changes in the API. 
	Build numbers are unrelated to API changes.

	@return The version of the implementation of the Graphics Resource API.
	*/
	IMPORT_C static TVersion Version();
private:
	/**
	Copy constructor.
	Implementation not provided. Declared private to prevent bit-wise copying 
	of handles.
	*/
	RSgDriver(const RSgDriver&);

	/**
	Assignment operator.
	Implementation not provided. Declared private to prevent bit-wise copying 
	of handles.
	*/
	const RSgDriver& operator =(const RSgDriver&);

	/**
	Makes the extension interface with the given UID available in the context 
	of the process.
	This function stores a pointer to the extension interface in the variable 
	referenced by the parameter aInterfacePtr and returns KErrNone if successful.

	@pre An RSgDriver has been opened in the context of the process.
	@param[in] aInterfaceUid The UID of the extension interface.
	@param[out] aInterfacePtr Reference to an untyped pointer that on return points 
		to the extension interface or, in case of error, is set to NULL.
	@return KErrNone if successful;
		KErrBadHandle if the instance of RSgDriver is a null handle;
		KErrArgument if aInterfaceUid is the null UID;
		KErrExtensionNotSupported if the requested extension interface is not supported.
	*/
	IMPORT_C TInt GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr) const;
private:
	/** A pointer to an implementation-defined object initially set to NULL by the constructor.*/
	TAny* iImpl;
	};

/**
An extension attribute.
*/
NONSHARABLE_CLASS(TSgAttribute)
	{
public:
	/**
	Default constructor.
	Data members remain uninitialised.
	*/
	inline TSgAttribute();

	/**
	Constructor which initialises data members to the given values.
	*/
	inline TSgAttribute(TUid aUid, TInt aValue);
public:
	/** A UID that identifies the type of extension attribute.*/
	TUid iUid;
	/** The value of the extension attribute.*/
	TInt iValue;
	};

/**
Base class of all attribute array classes.
It defines functions for bounds-checked access to extension attributes.
*/
NONSHARABLE_CLASS(TSgAttributeArrayBase)
	{
public:
	/**
	Returns the number of extension attributes in the array.
	@return The number of extension attributes in the array.
	*/
	inline TInt Count() const;

	/**
	Returns a reference to the extension attribute located in the array at the 
	given position.

	@pre aIndex is equal to or greater than zero and less than the number of 
		extension attributes in the array.
	@param aIndex The position of the extension attribute in the array.
	@return A reference to the extension attribute located in the array at the 
		given position.
	@panic SGRES 4 Debug builds only. aIndex is out of bounds.
	*/
	inline TSgAttribute& operator [](TInt aIndex);

	/**
	Returns a const-qualified reference to the extension attribute located in 
	the array at the given position.

	@pre aIndex is equal to or greater than zero and less than the number of 
		extension attributes in the array.
	@param aIndex The position of the extension attribute in the array.
	@return A reference to the extension attribute located in the array at the 
		given position.
	@panic SGRES 4 Debug builds only. aIndex is out of bounds.
	*/
	inline const TSgAttribute& operator [](TInt aIndex) const;
protected:
	/**
	Constructor used by constructors of derived classes to initialise the 
	attribute count.
	@param aCount The number of extension attributes.
	*/
	inline TSgAttributeArrayBase(TInt aCount);

	/**
	Assignment operator.
	No operation is performed. Allows the compiler to generate assignment 
	operators for derived classes automatically.
	*/
	inline void operator =(const TSgAttributeArrayBase&);
private:
	const TInt iCount;
	};

/**
A fixed-size array of extension attributes.
It is a thin wrapper with bounds checking for arrays of TSgAttribute.
*/
template<TInt S>
NONSHARABLE_CLASS(TSgAttributeArray): public TSgAttributeArrayBase
	{
public:
	/**
	Default constructor.
	Data members remain uninitialised.
	*/
	inline TSgAttributeArray();
private:
	TSgAttribute iAttributes[S];
	};

/**
A system-wide unique 64-bit identifier that can be used to share a drawable 
resource between processes.

The actual value of a drawable resource identifier has no meaning to the user 
of the Graphics Resource API, except that 64 zero bits represent the null 
drawable resource identifier, which explicitly identifies no drawable resource.
As a convenience, the constant KSgNullDrawableId is defined as the null drawable 
resource identifier.
*/
NONSHARABLE_CLASS(TSgDrawableId)
	{
public:
	/**
	Equality operator.
	Tests whether the given drawable resource identifier is the same as this.
	
	@param aId The drawable resource identifier to compare with this.
	@return ETrue if the 64-bit identifiers are the same, EFalse if not.
	*/
	inline TBool operator ==(TSgDrawableId aId) const;

	/**
	Inequality operator.
	Tests whether the given drawable resource identifier is different from this.
	
	@param aId The drawable resource identifier to compare with this.
	@return ETrue if the 64-bit identifiers are different, EFalse if they are the same.
	*/
	inline TBool operator !=(TSgDrawableId aId) const;
public:
	/** The 64-bit identifier.*/
	TUint64 iId;
	};

/**
The null drawable resource identifier.
*/
const TSgDrawableId KSgNullDrawableId = {0};

/**
The UID identifying a RSgDrawable handle.
This must never be changed.
*/
const TUid KSgDrawableTypeUid = {0x102858EB};

/**
A drawable resource handle. 
A null handle is a handle that explicitly does not reference any drawable resource. 
An open handle is a handle that references an existing drawable resource. 
An invalid handle is a handle that is not null but does not reference any 
existing drawable resource. 
Drawable resources are reference-counted so that a drawable resource is not 
destroyed while there still are open handles to it in any process in the system.

RSgDrawable is not an abstract class, that is, it can be instantiated, 
but it can also be derived from to define subclasses representing types of 
handles that refer only to concrete types of drawable resources, for example, 
image handles. RSgImage is the only subclass of RSgDrawable defined in version 
1.1 of the Graphics Resource API. 
Instances of RSgDrawable are useful when drawable resources created in a 
component have to be passed to another component through a third component that 
cannot depend on the concrete type of the drawable resources being passed.
@see RSgImage
*/
NONSHARABLE_CLASS(RSgDrawable)
	{
public:
	/**
	Default constructor.
	@post The instance of RSgDrawable is a null handle.
	*/
	inline RSgDrawable();

	/**
	Opens a new handle to the drawable resource with the given drawable resource 
	identifier.
	The drawable resource may have been created in the same process or in a 
	different one. This function does not allocate any specialised graphics memory.

	Note that an RSgDrawable handle should never be bitwise-copied into another 
	but this function should be used instead in all cases, since it is the only 
	way to ensure that the reference count of the drawable resource is kept up 
	to date. Failure to keep the reference count of the resource up to date may 
	result in invalid handles.

	@pre An RSgDriver handle has been opened in the context of the process.
	@pre The instance of RSgDrawable is a null handle.
	@post The specified drawable resource has its reference count incremented by one.
	@param aId The identifier of the drawable resource.
	@param aAttributes A pointer to an array of extension attributes, 
		if allowed by any extension of the Graphics Resource API, or NULL otherwise.
	@return KErrNone if successful;
		KErrInUse if the instance of RSgDrawable is an open handle;
		KErrArgument if the parameter aId is the null drawable resource identifier;
		KErrNotSupported if the parameter aAttributes is not NULL and one or more 
			of the extension attributes in the array is not defined by any extension 
			of the Graphics Resource API;
		KErrNotFound if the parameter aId cannot be found to refer to an existing 
			drawable resource;
		KErrNoMemory if it fails due to lack of system memory.
	@panic SGRES 1 No RSgDriver handle has been opened in the context of the process.
	*/
	IMPORT_C TInt Open(TSgDrawableId aId, const TSgAttributeArrayBase* aAttributes = NULL);

	/**
	Closes a handle to a drawable resource. 
	It decrements the reference count of the drawable resource and, if it reaches
	zero, destroys the drawable resource. 
	Calling this function on a null handle has no effect.

	@pre If the instance of RSgDrawable is not a null handle then an RSgDriver 
		handle has been opened in the context of the process.
	@post The instance of RSgDrawable is a null handle.
	@panic SGRES 1 The instance of RSgDrawable is not a null handle and no 
		RSgDriver handle has been opened in the context of the process.
	@panic SGRES 3 The instance of RSgDrawable is an invalid handle.
	*/
	IMPORT_C void Close();

	/**
	Returns the identifier of the drawable resource.
	This identifier can be used to open another handle to the drawable resource 
	in the same process or in a different process. 
	Calling this function on a null handle returns the null drawable resource 
	identifier.

	@pre If the instance of RSgDrawable is not a null handle then an RSgDriver 
		handle has been opened in the context of the process.
	@return The identifier of the drawable resource.
	@panic SGRES 1 The instance of RSgDrawable is not a null handle and no 
		RSgDriver handle has been opened in the context of the process.
	@panic SGRES 3 The instance of RSgDrawable is an invalid handle.
	*/
	IMPORT_C TSgDrawableId Id() const;

	/**
	Tests whether a handle is null.
	@return ETrue if the instance of RSgDrawable is a null handle; EFalse otherwise.
	*/
	inline TBool IsNull() const;

	/**
	Returns the UID corresponding to the concrete type of the drawable resource 
	referenced by a handle at run-time. 
	Calling this function on a null handle returns the null UID. 
	In version 1.1 of the Graphics Resource API every drawable resource is an image, 
	so this function always returns KSgImageTypeUid when called on an open handle.

	@pre If the instance of RSgDrawable is not a null handle then an RSgDriver 
		handle has been opened in the context of the process.
	@return The UID corresponding to the concrete type of the drawable resource.
	@panic SGRES 1 The instance of RSgDrawable is not a null handle and no 
		RSgDriver handle has been opened in the context of the process.
	@panic SGRES 3 The instance of RSgDrawable is an invalid handle.
	*/
	IMPORT_C TUid DrawableType() const;

	/**
	Makes the extension interface specified by the type of the parameter 
	aInterfacePtr available on a drawable resource.
	This function stores a pointer to the extension interface in the variable 
	referenced by the parameter aInterfacePtr and returns KErrNone if successful.
	The returned extension interface is tied to the drawable resource, that is, 
	calls to functions in the returned extension interface operate on the 
	drawable resource.

	@pre An RSgDriver handle has been opened in the context of the process.
	@pre The instance of RSgDrawable is an open handle.
	@param[out] aInterfacePtr Reference to a pointer to the interface class. that on return points 
		to the extension interface or, in case of error, is set to NULL.
	@return KErrNone if successful;
		KErrBadHandle if the instance of RSgDrawable is a null handle;
		KErrExtensionNotSupported if the requested extension interface is not supported.
	@panic SGRES 1 No RSgDriver handle has been opened in the context of the process.
	@panic SGRES 3 The instance of RSgDrawable is an invalid handle.
	*/
	template<class M> inline TInt GetInterface(M*& aInterfacePtr) const;
protected:
	/**
	Handle base constructor.
	Used by constructors of derived classes to initialise the handle type.
	@post The instance of RSgDrawable is a null handle.
	@param aHandleType The UID identifying the handle type.
	*/
	inline RSgDrawable(TUid aHandleType);
private:
	/**
	Copy constructor.
	Implementation not provided. Declared private to prevent bit-wise copying 
	of handles.
	*/
	RSgDrawable(const RSgDrawable&);

	/**
	Assignment operator.
	Implementation not provided. Declared private to prevent bit-wise copying 
	of handles.
	*/
	const RSgDrawable& operator =(const RSgDrawable&);

	/**
	Makes the extension interface with the given UID available on a drawable 
	resource.
	This function stores a pointer to the extension interface in the variable 
	referenced by the parameter aInterfacePtr and returns KErrNone if successful.
	The returned extension interface is tied to the drawable resource, that is, 
	calls to functions in the returned extension interface operate on the 
	drawable resource.

	@pre An RSgDriver handle has been opened in the context of the process.
	@pre The instance of RSgDrawable is an open handle.
	@param[in] aInterfaceUid The UID of the extension interface.
	@param[out] aInterfacePtr Reference to an untyped pointer that on return points 
		to the extension interface or, in case of error, is set to NULL.
	@return KErrNone if successful;
		KErrBadHandle if the instance of RSgDrawable is a null handle;
		KErrArgument if aInterfaceUid is the null UID;
		KErrExtensionNotSupported if the requested extension interface is not supported.
	@panic SGRES 1 No RSgDriver handle has been opened in the context of the process.
	@panic SGRES 3 The instance of RSgDrawable is an invalid handle.
	*/
	IMPORT_C TInt GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr) const;
protected:
	/**
	A UID permanently set by the constructor to the constant 
	KSgDrawableTypeUid, except for subclasses of RSgDrawable, in which case it 
	is permanently set by the constructor to the UID corresponding to the 
	concrete type of drawable resource referenced by instances of the subclass.
	*/
	const TUid iHandleType;

	/** A pointer to an implementation-defined object initially set to NULL by the constructor.*/
	TAny* iImpl;
	};

#include <sgresource/sgresource.inl>

#endif // SGRESOURCE_H

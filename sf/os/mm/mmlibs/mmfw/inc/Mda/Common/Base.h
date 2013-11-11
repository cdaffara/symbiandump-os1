// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Common\Base.h
// largely EMPTY - maintained for backwards compatibility
// 
//

#ifndef __MDA_COMMON_BASE_H__
#define __MDA_COMMON_BASE_H__

/*
Notes:
Header file for information shared between client and server side
Contains UID definitions, function numbers
*/

// Standard EPOC32 includes
#include <e32base.h>
// Public Media Server includes
#include <mda/common/base.hrh>

#ifdef ASSERT
#ifdef _DEBUG
#undef ASSERT

// Redefine assert to be a little more useful to us, i.e. to include file & line number
#define __ASSERT_FILE__(s) _LIT(KPanicFileName,s)
#define __ASSERT_PANIC__(l) User::Panic(KPanicFileName().Right(12),l)
#define ASSERT(x) { __ASSERT_FILE__(__FILE__); __ASSERT_DEBUG(x, __ASSERT_PANIC__(__LINE__) ); }
#endif
#endif

/**
 * @publishedAll
 *
 * Macro for producing different variants of Uids
 */
#define MDA_UID(uid_name)	const TUint uid_name##Value = uid_name##Define; \
							const TUid uid_name = {uid_name##Value};

// Uids
#if defined(UNICODE)

/**
 * @publishedAll
 * @deprecated 
 *
 * Not used in MMF
 */
const TInt KUidMediaServerLibraryValue = KUidMediaServerLibraryUnicodeDefine;
#else

/**
 * @publishedAll
 * @deprecated 
 *
 * Not used in MMF
 */
const TInt KUidMediaServerLibraryValue = KUidMediaServerLibraryDefine;
#endif

/**
 * @publishedAll
 * @deprecated
 *
 * Not used in MMF
 */
const TUid KUidMediaServerLibrary = {KUidMediaServerLibraryValue};

/** 
 * @publishedAll
 * @deprecated 
 *
 * Unknown value
 */
const TInt KMdaUnknown = -1;

// Package classes

/**
@publishedAll
@deprecated

Abstract base class for all media server package types.

This class has no user accessible functions.
*/
class TMdaRawPackage
	{
public:
	inline TPtr8& Package();
	inline const TPtr8& Package() const;
protected:
	TMdaRawPackage(TInt aDerivedSize);
	inline void SetSize(TInt aDerivedSize);
protected:
	TPtr8 iThis;
	};

/**
@publishedAll
@deprecated

A data structure used to package messages (that include the package type) sent between the media server 
and its clients.

This class is abstract. It defines the attributes common to packages where the derived class's type needs 
to be sent as part of the package. The attributes are a UID that identifies the package's concrete class 
(so that the server can construct objects of the correct type), and a UID that identifies the package type 
(the category of classes to which the package belongs). The package type is little used in practice. These 
attributes and the derived class's size are set during construction of the derived class.
*/
class TMdaPackage : public TMdaRawPackage
	{
public:
	inline TUid Type() const;
	inline TUid Uid() const;
	inline void SetUid(TUid aUid);
	inline TBool operator==(const TMdaPackage& aPackage); // Type&Uid comparison
protected:
	inline TMdaPackage(TUid aType, TUid aUid, TInt aDerivedSize);
private:
	TUid iType;
	TUid iUid;
	};

/** 
 *  
 * @publishedAll
 * @deprecated
 *
 * Media server event identification - Not used in MMF
 */
class TMdaEvent
	{
public:
	TInt32 iId;
	TInt32 iArg[3];
	};


/** 
 *  
 * @publishedAll
 * @deprecated
 *
 * Not used in MMF
 */
class TMdaObjectEvent
	{
public:
	TInt iHandle;
	TMdaEvent iEvent;
	};

/** 
 *  
 * @publishedAll
 * @deprecated
 *
 * Not used in MMF
 */
class TMdaEventPackage : public TMdaRawPackage
	{
public:
	inline TMdaEventPackage(); // For single events
	//
	inline TInt EventCount() const;
	inline TInt MaxEvents() const;
	inline const TMdaEvent& Event(); // First event only
	inline const TMdaEvent& Event(TInt aIndex);
	inline TInt EventFrom(); // first event only
	inline TInt EventFrom(TInt aIndex);
protected:
	inline TMdaEventPackage(TInt aMaxEvents);
protected:
	TInt iMaxEvents;
	TInt iValidEvents;
	TMdaObjectEvent iFirstEvent;
	};

#include <mda/common/base.inl>

#include <mmf/common/mmfbase.h>

#endif

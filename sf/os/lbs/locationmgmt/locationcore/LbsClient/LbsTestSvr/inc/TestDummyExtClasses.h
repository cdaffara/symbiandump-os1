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

#ifndef __TESTDUMMYEXTCLASSES_H__
#define __TESTDUMMYEXTCLASSES_H__

#include <lbspositioninfo.h>
#include <lbscommon.h>
/**
 * @file TestDummyExtClasses.h
 *
 * This file includes classes that create alternative versions of the API
 * by inheriting from the various base classes defined in the API dll,
 * such as TPositionInfoBase and TPositionModuleStatusBase.
 *
 * For simplicity (to avoid having to build another DLL to run the dummy
 * server tests), the classes have public data and all methods are inline.
 */

#define DUMMY_NOTIFY_POS_UPDATE_EXT_GALAXY      (TInt64(0x0102080701987654))
#define DUMMY_NOTIFY_POS_UPDATE_EXT_SOLARSYS    (TInt64(0x034D0058043C8DE1))
#define DUMMY_NOTIFY_POS_UPDATE_EXT_STARDATE    (_L("9521.5"))  // 2292-12-19

// This macro asserts that the class aClassType has size equal to aSize.
#define __ASSERT_CLASS_SIZE(aClassType, aSize) \
		__ASSERT_ALWAYS(sizeof(aClassType)==(aSize), \
		User::Panic(KPosClientFault, EPositionClassSizeMismatch))

// This macro should be used by all classes that derive from TPositionModuleStatusEventBase.
// This should be the first thing called in the derived class' DoReset method which must
// be implemented. 
#define __TPOSITION_MODULE_STATUS_EVENT_RESET_BASE(aThisClass, aExpectedSize, aFirstBaseClass) \
		__ASSERT_CLASS_SIZE(aThisClass, (aExpectedSize)); \
		aFirstBaseClass::DoReset(sizeof(aFirstBaseClass))

enum _TPositionInfoClassTypeExtended
	{
	EPositionClassTestExtension = 0x10
	};

class TExtPosInfo : public TPositionInfoBase
	{
public:
	inline TExtPosInfo();
public:
	TInt64 iGalaxy;
	TInt64 iSolarSystem;
	TBuf<10> iStarDate;
	};


//
// TExtPosInfo
//
inline TExtPosInfo::TExtPosInfo()
	{
	iPosClassType |= EPositionClassTestExtension;
	iPosClassSize = sizeof(TExtPosInfo);
	}

//

_LIT(KDummyString, "DummyString");

enum _TModuleStatusEventClassTypeExtended
	{
	EModuleStatusEventTestExtension = 0x10
	};

class TExtPosModuleStatusEvent : public TPositionModuleStatusEvent
	{
public:
	inline TExtPosModuleStatusEvent();
protected:
	inline void DoReset(TInt aSizeOfClass);
public:
	TBufC<11> iDummyString;
	};

inline TExtPosModuleStatusEvent::TExtPosModuleStatusEvent()
: iDummyString(KDummyString)
	{
	iPosClassType |= EModuleStatusEventTestExtension;
	iPosClassSize = sizeof(TExtPosModuleStatusEvent);
	}

inline void TExtPosModuleStatusEvent::DoReset(TInt aSizeOfClass)
	{
	__TPOSITION_MODULE_STATUS_EVENT_RESET_BASE(TExtPosModuleStatusEvent,
											   aSizeOfClass,
											   TPositionModuleStatusEvent);
	//Nothing to do here.
	}

#endif // __TESTDUMMYEXTCLASSES_H__

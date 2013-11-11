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
//

/**
 @file
 @internalTechnology
*/

#if !defined(__CINIFILE_H__)
#define __CINIFILE_H__

#include <e32base.h>
#include "dbdef.h"

/**
Macro for build independent literal ADD_SECTION
*/
#define BEGIN_ADD			_L("ADD_SECTION")

/**
Macro for build independent literal ADD_SECTION
*/
#define BEGIN_TEMPLATE		_L("ADD_TEMPLATE")

/**
Macro for build independent literal ADD_SECTION
*/
#define BEGIN_SET			_L("SET_SECTION")

/**
Macro for build independent literal END_ADD
*/
#define END_ADD				_L("END_ADD")

/**
Macro for build independent literal END_TEMPLATE
*/
#define END_TEMPLATE		_L("END_TEMPLATE")

/**
Macro for build independent literal END_SET
*/
#define END_SET				_L("END_SET")

/**
Macro for build independent literal SET_CONDITION
*/
#define CONDITION			_L("SET_CONDITION")

/**
Macro for build independent literal FIELD_COUNT
*/
#define FIELD_COUNT			_L("FIELD_COUNT")

#define E_UNKNOWN			0

#define E_ADD				1

#define E_SET				2

#define E_TEMPLATE			3

class LinkByTagResolver;

NONSHARABLE_CLASS(CIniData) : public CBase 
/**
@internalComponent
*/
	{
	friend class LinkByTagResolver;
public:
	// Constructor, pass in name of CFG file to open, relative to RAM root
	static CIniData* NewL(const TDesC& aName);
	virtual ~CIniData();
	
	// methods for enumerating through ADD or SET sections
	// within a particular section
	TInt OpenSetBlock(const TDesC &aSection);
	TInt OpenTemplateBlock(const TDesC &aSection);
	TInt OpenAddBlock(const TDesC &aSection);
	TInt StepToNextBlock();
	TInt GetSetting(const TDesC &aValue, TPtrC & aSetting);
	
protected:
	CIniData();
	
private:
	HBufC* iName;
	HBufC* iToken;
	TPtr iPtr;
	TPtr section;
	TPtr block;
	TInt BlockState;
	TInt blockStart;
	TInt blockEnd;
	TInt scanStart;
	
	TInt OpenBlock(const TDesC &aSection);
	void ConstructL(const TDesC& aName);
	};

#endif

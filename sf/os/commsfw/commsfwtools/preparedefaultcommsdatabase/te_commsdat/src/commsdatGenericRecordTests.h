/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* CCommsdat80Step
* CCommsdat81Step
* CCommsdat82Step
* CCommsdat83Step
* CCommsdat84Step
* CCommsdat85Step
* CCommsdat86Step
* CCommsdat87Step
* CCommsdat88Step
* CCommsdat89Step
* 
*
*/



/**
 @file commsdatGenericRecordTests.h
*/

#if (!defined __COMMSDAT_GENERIC_RECORD_TESTS_H__)
#define __COMMSDAT_GENERIC_RECORD_TESTS_H__
#include "Te_commsdatSuiteStepBase.h"
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>

using namespace CommsDat;
using namespace Meta;

// field names
#define     KCDTypeNameIntegerField    _S("IntegerField")
#define     KCDTypeNameStringField     _S("StringField")
#define     KCDTypeNameMediumStringField _S("A field name which is 64 characters; that is the limit defined")

// record type id
const TMDBElementId KCDTIdTestRecord         = 0x5F800000;
// field type ids
const TMDBElementId KCDTIdTestIntegerField   = 0x00120000;
const TMDBElementId KCDTIdTestStringField    = 0x00130000;
const TMDBElementId KCDTIdTestMediumStringField    = 0x00140000;


// 'CMDBGenericRecord' field schema information.

// relate names, id and value type together.  
static const SGenericRecordTypeInfo TestGenRecordInfo[] =
    {
	MDB_GENERIC_TYPE_INFO(KCDTIdRecordTag,	     	EUint32,    ENoAttrs,   KCDTypeNameRecordTag),
    MDB_GENERIC_TYPE_INFO(KCDTIdRecordName,	     	EText,	    ENotNull,   KCDTypeNameRecordName),
	MDB_GENERIC_TYPE_INFO(KCDTIdTestIntegerField,	EUint32,  	ENoAttrs,   KCDTypeNameIntegerField),
	MDB_GENERIC_TYPE_INFO(KCDTIdTestStringField,	EText,      ENoAttrs,   KCDTypeNameStringField),
	MDB_GENERIC_TYPE_INFO(KCDTIdTestMediumStringField,	EText,      ENoAttrs,   KCDTypeNameMediumStringField),
    MDB_GENERIC_TYPE_INFO(0, 0, ENoAttrs, KCDNull) 
    };

//-----
#define	KCDTypeNameTestIAPLinkField		      		_S("TestIAPLinkField")
#define	KCDTypeNameTestOtherGenRecField				_S("TestOtherGenRecField")

const TMDBElementId KCDTIdLinkTestRecord         	= 0x60800000;
const TMDBElementId KCDTIdTestIAPLinkField    		= 0x00150000;
const TMDBElementId KCDTIdTestOtherGenRecLinkField	= 0x00160000;
   
   
static const SGenericRecordTypeInfo TestGenLinkRecordInfo[] =
    {
	MDB_GENERIC_TYPE_INFO(KCDTIdRecordTag,	     	EUint32,    ENoAttrs,   KCDTypeNameRecordTag),
    MDB_GENERIC_TYPE_INFO(KCDTIdRecordName,	     	EText,	    ENotNull,   KCDTypeNameRecordName),
	MDB_GENERIC_TYPE_INFO(KCDTIdTestIAPLinkField,	ELink,  	ENoAttrs,   KCDTypeNameTestIAPLinkField),
	MDB_GENERIC_TYPE_INFO(KCDTIdTestOtherGenRecLinkField,	ELink,      ENoAttrs,   KCDTypeNameTestOtherGenRecField),
    MDB_GENERIC_TYPE_INFO(0, 0, ENoAttrs, KCDNull)
    };

//-----
#define	KCDTypeNameTestSomeField		      		_S("TestSomeField")

const TMDBElementId KCDTIdLinkedTestRecord         	= 0x60000000;
const TMDBElementId KCDTIdTestSomeField    		= 0x00170000;

static const SGenericRecordTypeInfo TestGenLinkedkRecordInfo[] =
    {
	MDB_GENERIC_TYPE_INFO(KCDTIdRecordTag,	     	EUint32,    ENoAttrs,   KCDTypeNameRecordTag),
    MDB_GENERIC_TYPE_INFO(KCDTIdRecordName,	     	EText,	    ENotNull,   KCDTypeNameRecordName),
	MDB_GENERIC_TYPE_INFO(KCDTIdTestSomeField,	EInt,  	ENoAttrs,   KCDTypeNameTestSomeField),
    MDB_GENERIC_TYPE_INFO(0, 0, ENoAttrs, KCDNull)
    };

class CGenericRecordTestBase:public CTe_commsdatSuiteStepBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};
	
// Test Step declerations
//------------------------------------------------------------------------------------------
_LIT(KCommsdat80Step,"commsdat80Step");
class CCommsdat80Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat80Step();
	~CCommsdat80Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat81Step,"commsdat81Step");
class CCommsdat81Step : public CGenericRecordTestBase
	{
public:
	CCommsdat81Step();
	~CCommsdat81Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat82Step,"commsdat82Step");
class CCommsdat82Step : public CGenericRecordTestBase
	{
public:
	CCommsdat82Step();
	~CCommsdat82Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat83Step,"commsdat83Step");
class CCommsdat83Step : public CGenericRecordTestBase
	{
public:
	CCommsdat83Step();
	~CCommsdat83Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat84Step,"commsdat84Step");
class CCommsdat84Step : public CGenericRecordTestBase
	{
public:
	CCommsdat84Step();
	~CCommsdat84Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat85Step,"commsdat85Step");
class CCommsdat85Step : public CGenericRecordTestBase
	{
public:
	CCommsdat85Step();
	~CCommsdat85Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat86Step,"commsdat86Step");
class CCommsdat86Step : public CGenericRecordTestBase
	{
public:
	CCommsdat86Step();
	~CCommsdat86Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat87Step,"commsdat87Step");
class CCommsdat87Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat87Step();
	~CCommsdat87Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat88Step,"commsdat88Step");
class CCommsdat88Step : public CGenericRecordTestBase
	{
public:
	CCommsdat88Step();
	~CCommsdat88Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat89Step,"commsdat89Step");
class CCommsdat89Step : public CGenericRecordTestBase
	{
public:
	CCommsdat89Step();
	~CCommsdat89Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat90Step,"commsdat90Step");
class CCommsdat90Step : public CGenericRecordTestBase
	{
public:
	CCommsdat90Step();
	~CCommsdat90Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat91Step,"commsdat91Step");
class CCommsdat91Step : public CGenericRecordTestBase
	{
public:
	CCommsdat91Step();
	~CCommsdat91Step();
	virtual TVerdict doTestStepL();
private:
	void IAPCreateL();
	void GenericLinkedCreateL();
	void GenericLinkCreateL();
	
	CMDBSession* iDBSession;
	TInt iIAPRecId;
	TMDBElementId iLinkedGenericId;
	};

_LIT(KCommsdat92Step,"commsdat92Step");
class CCommsdat92Step : public CGenericRecordTestBase
	{
public:
	CCommsdat92Step();
	~CCommsdat92Step();
	virtual TVerdict doTestStepL();
private:	
	CMDBSession* iDBSession;
	TInt iIAPRecId;
	TMDBElementId iLinkedGenericId;
	};

#endif //__COMMSDAT_GENERIC_RECORD_TESTS_H__

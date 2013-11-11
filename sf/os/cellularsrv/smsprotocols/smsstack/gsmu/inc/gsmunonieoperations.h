// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the container class CSmsNonIEOperation and its specialisations.
// The classes provide interfaces that allow the client to operate on a number
// of features which are not associated with Information Elements.
// 
//

/**
 @file
*/

#ifndef __GSMUNONIEOPERATIONS_
#define __GSMUNONIEOPERATIONS_


#include <e32base.h>
#include <gsmumsg.h>
#include <gsmuelem.h>

/**
 *  SMS Stack clients use specialisations of this class to configure a SMS Message with parameters which are not
 *  a type of Information Element.
 *
 *  Each parameter that uses this interface is supported by a class derived from CSmsNonIEOperation.
 *  The derived class operates on the CSmsMessage, allowing the client to add, remove and access these parameters
 *  inside the CSmsMessage.
 *
 *  The client gets access to an operations class using the following interface:
 *
 *  CSmsNonIEOperation& CSmsMessage::GetOperationsForNonIEL(TSmsNonIEIdentifier aId);
 *
 *  When the CSmsMessage is deleted, all its associated CSmsNonIEOperations classes are also deleted and references to
 *  them become stale. Each instance of the CSmsNonIEOperation Class is an attribute of the CSmsMessage itself.
 *
 *  @publishedAll
 *  @released
 */
class CSmsNonIEOperation : public CBase
{
public: 
	static CSmsNonIEOperation* NewL(TSmsNonIEIdentifier aId, CSmsMessage& aMessage);
	IMPORT_C TSmsNonIEIdentifier Id() const;
	virtual ~CSmsNonIEOperation(){};
protected:
	virtual TBool MessageTypeSupported() const;
	virtual void ValidateOperationL() const {};
private: 
    void ConstructL();   
private:    
    TSmsNonIEIdentifier iId;
protected:
    CSmsMessage& iMessage;
    CSmsNonIEOperation(TSmsNonIEIdentifier aId, CSmsMessage& aMessage);
    void operator=(const CSmsNonIEOperation&);
    TBool operator==(const CSmsNonIEOperation&);    
};

class CSmsCtrlNonIEOperation : public CSmsNonIEOperation
{
protected:
    CSmsCtrlNonIEOperation(TSmsNonIEIdentifier aId, CSmsMessage& aMessage);
    ~CSmsCtrlNonIEOperation(){};
    void operator=(const CSmsCtrlNonIEOperation&);
    TBool operator==(const CSmsCtrlNonIEOperation&);
};

class CSmsTPSRROperations : public CSmsCtrlNonIEOperation
	{
public:
	CSmsTPSRROperations(TSmsNonIEIdentifier aId, CSmsMessage& aMessage);
    ~CSmsTPSRROperations() {};
    IMPORT_C TInt GetStatusReport(TUint aSegmentSequenceNum) const;
    IMPORT_C void SetLastSegmentStatusReportL(TBool aEnable);
    IMPORT_C TInt SetSegmentStatusReportL(TUint aSegmentSequenceNum, TBool aEnable);
    IMPORT_C void SetSchemeL();
    IMPORT_C TSmsStatusReportScheme GetScheme() const;
   	IMPORT_C void ResetSchemeL();
   	IMPORT_C void SetDefaultL(TBool aEnable);
protected:
    void ValidateOperationL() const;
    void operator=(const CSmsTPSRROperations&);
    TBool operator==(const CSmsTPSRROperations&);
	};

/**
 *  This class will store the incomplete class 0 message information.
 *  
 *  
 *  The object of this class will be stored inside CSmsMessageAdditionalAttributes class.
 *  This class will provide 2 exported functions which will allow the caller of this class 
 *  to get/set incomplete class 0 message information.
 *  @publishedAll
 *  @released 
 */
class CIncompleteClass0MessageInfo : public CSmsNonIEOperation
	{
public:
	CIncompleteClass0MessageInfo(TSmsNonIEIdentifier aId, CSmsMessage& aMessage);
	IMPORT_C void GetIncompleteMessageInfoL(TInt& aStartPos, TInt& aEndPos, TBool& aIsLastIncompleteMessage);
	IMPORT_C void SetIncompleteMessageInfoL(TInt aStartPos, TInt aEndPos, TBool aIsLastIncompleteMessage);
	TInt Version();
	void SetVersion(TInt aVersion);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;

protected:
	TBool MessageTypeSupported() const;
	void ValidateOperationL() const;
	void operator=(const CIncompleteClass0MessageInfo&);
	TBool operator==(const CIncompleteClass0MessageInfo&);

public:
/**
 *  Incomplete Class 0 Message versions
 */
	enum TSmsIncompleteClass0MessageVersion
		{
		/** The default value. */
		ESmsIncompleteClass0MessageV0,
		// for all other versions the corresponding enum values would be added i.e. ESmsIncompleteClass0MessageV1, etc.
		EMaxSmsIncompleteClass0MessageV
		};

private:
/**
 *  iVersion is the version of CIncompleteClass0MessageInfo. This parameter will be
 *  used to identify what iAdditionalInfo points to for that particular
 *  version of CIncompleteClass0MessageInfo
 */
	TInt iVersion;

	TInt iStartPos;
	TInt iEndPos;
	TBool iIsLastIncompleteMessage;
	};

#endif

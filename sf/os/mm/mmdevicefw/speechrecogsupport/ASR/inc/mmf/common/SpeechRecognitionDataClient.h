// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SPEECHRECOGNITIONDATACLIENT_H__
#define __SPEECHRECOGNITIONDATACLIENT_H__

#include <e32base.h>
#include <s32strm.h>

//  INCLUDES
#include <mmf/common/mmfcontrollerframework.h>
#include <mmf/common/speechrecognitiondatacommon.h>


// FORWARD DECLARATION
class TDiagnostic;


// CLASS DECLARATION

/**
@publishedAll
@released

Class to encapsulate a client recognition result.
The result is formulated for client consumption.

@since  8.0
*/
class CSDClientResult : public CBase
	{
public:	 // Constructors and destructor
	/**
	Destructor.
	*/
	IMPORT_C ~CSDClientResult();

	/**
	Two-phased constructor. Creates an uninitialised recognition result.

	Can leave with one of the system-wide error codes
	
	@return CSDClientResult object
	*/
	IMPORT_C static CSDClientResult* NewL();

	/**
	Two-phased constructor. Creates an uninitialised recognition result. A copy of the newly created class
	is pushed onto the cleanup stack.

	Can leave with one of the system-wide error codes.
	
	@return CSDClientResult object
	*/
	IMPORT_C static CSDClientResult* NewLC();

	/**
	Two-phased constructor. Creates a result with a grammar and rule ID.

	Can leave with one of the system-wide error codes.

	@param  aGrammarID
	        The grammar ID of the result.
	@param  aRuleID
	        The rule ID of the result.
	        
	@return CSDClientResult object.
	*/
	IMPORT_C static CSDClientResult* NewL(TGrammarID aGrammarID, TRuleID aRuleID);

	/**
	Two-phased constructor. Creates a result with a grammar and rule ID. A copy of the newly created class
	is pushed onto the cleanup stack.

	Can leave with one of the system-wide error codes.

	@param  aGrammarID
	        The grammar ID of the result
	@param  aRuleID
	        The rule ID of the result
	        
	@return CSDClientResult object.
	*/
	IMPORT_C static CSDClientResult* NewLC(TGrammarID aGrammarID, TRuleID aRuleID);

	/**
	Sets the grammar ID on the result.

	@param  aGrammarID
	        The grammar identifier.

	@since 8.0
	*/
	IMPORT_C void SetGrammarID(TGrammarID aGrammarID);

	/**
	Retrieves the grammar ID of the result.

	@return Grammar identifier.
	@since  8.0
	*/
	IMPORT_C TGrammarID GrammarID() const;

	/**
	Sets the rule ID of the result.

	@param  aRuleID
	        The rule identifier.

	@since  8.0
	*/
	IMPORT_C void SetRuleID(TRuleID aRuleID);

	/**
	Returns the rule ID of the result.

	@return The rule identifier.
	@since  8.0
	*/
	IMPORT_C TRuleID RuleID() const;

	/**
	@internalTechnology
	
	Writes the class to a stream.

	@param  aStream
	        A RWriteStream to write the class to.

	@since  8.0
	*/
	void ExternalizeL(RWriteStream& aStream) const;
	/**
	@internalTechnology
	
	Reads the class from a stream.

	@param  aStream
	        A RReadStream to read from.

	@since  8.0
	*/
	void InternalizeL(RReadStream& aStream);

private:
	void ConstructL();
	CSDClientResult();
	CSDClientResult(TGrammarID aGrammarID, TRuleID aRuleID);


private:
	TGrammarID iGrammarID;
	TRuleID iRuleID;
	};


/**
@publishedAll
@released

Class to encapsulate a client recogntion result set
This is a lightweight result set formulated for client consumption

@since  8.0
*/
class CSDClientResultSet : public CBase
	{
public:	// Constructors and destructor
	/**
	Destructor.
	*/
	IMPORT_C ~CSDClientResultSet();

	/**
	Two-phased constructor.
	
	@return CSDClientResultSet object.
	*/
	IMPORT_C static CSDClientResultSet* NewL();

	/**
	Two-phased constructor. Pushes the newly created object onto the cleanup stack
	
	@return CSDClientResultSet object.
	*/
	IMPORT_C static CSDClientResultSet* NewLC();


	/**
	Sets the maximum number of best matches (N-best) expected in the recognition result after 
	recognition.

	@param  aMaxResults
	        The maximum number of matches expected in the recognition result.

	@since  8.0
	*/
	IMPORT_C void SetMaxResultsL(TInt aMaxResults);

	/**
	Returns the current value of the max number of expected after recognition.

	@return The maximum number of best matches (N-best) expected in the recognition result.

	@since  8.0
	*/
	IMPORT_C TInt MaxResults() const;

	/**
	Returns a result at a given index within the result set.

	@param  aIndex 
	        The index of result to return.

	@return A pointer to CSDClientResult object.

	@since 8.0
	*/
	IMPORT_C const CSDClientResult& At(TInt aIndex) const;

	/**
	Sets the number of results actually returned after recogntion.

	@param  aResultCount 
	        The number of results returned by the recognizer.

	@since 8.0
	*/
	IMPORT_C void SetResultCount(TInt aResultCount);

	/**
	Returns the number of results actually returned after recogntion.

	@return The number of results actually returned after recogntion.

	@since 8.0
	*/
	IMPORT_C TInt ResultCount() const;

	/**
	Sets the diagnostic of this result set.

	@param  aDiagnostic 
	        The reference to the diagostic object.

	@since 8.0
	*/
	IMPORT_C void SetDiagnostic(TDiagnostic& aDiagnostic);

	/**
	Gets the diagnostic of this result set.

	@return A reference to TDiagnostic object.

	@since 8.0
	*/
	IMPORT_C const TDiagnostic& Diagnostic() const;


	/**
	Writes the state of the class to a stream.

	@param  aStream
	        The stream to write to.

	@since  8.0
	*/
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	/**
	Restores the state of the object from a stream.

	@param  aStream
	        The stream to read from.

	@since  8.0
	*/
	IMPORT_C void InternalizeL(RReadStream& aStream);

private:
	void ConstructL();
	CSDClientResultSet();

private:
	TInt iMaxResults;	// maximum number of results
	TInt iResultCount; // number of recognition results
	RPointerArray<CSDClientResult> iResultArray; // array of recognition results
	TDiagnostic* iDiagnostic;	// diagnostic information
	};
	
#endif  // __SPEECHRECOGNITIONDATACLIENT_H__


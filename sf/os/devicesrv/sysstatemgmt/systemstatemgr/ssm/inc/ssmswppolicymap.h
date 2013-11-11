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

#ifndef _SSMSWPPOLICYMAP_H_
#define _SSMSWPPOLICYMAP_H_

#include <e32base.h>
#include <ssm/ssmswp.h>

class CSsmSwpPolicyMapping;

/**
 The class used for storing the available SWPs and the mapping of SWPs to policy files.
 Entries are sorted on addition and the RPointerArray binary search facilities are used for speedy access.
 Note: This object is not thread safe
 
 @internalComponent
 @released
 */
class CSsmSwpPolicyMap : public CBase
	{
public:
	static CSsmSwpPolicyMap* NewLC();
	static CSsmSwpPolicyMap* NewL();
	~CSsmSwpPolicyMap();
	
	// API	
	void AddL(TUint aSwpKey, const TDesC& aFilename);
//	used for testing
#ifdef _DEBUG
 	void DeleteSwpMapL(TUint aSwpKey);
#endif
	const TDesC& FilenameL(TUint aSwpKey) const;
	
protected:
	CSsmSwpPolicyMap();
	const CSsmSwpPolicyMapping* MappingL(const TUint aSwpKey) const;

protected:
	RPointerArray<CSsmSwpPolicyMapping> iItems;
	};

	
/**
This contains the mapping between the SWP and its policy file name.
It is used solely as a container for the CSsmSwpPolicyMap collection and should 
only be used internally in CSsmSwpPolicyMap and CSsmSwpPolicyMapTestVersion

@internalComponent
@released
*/
class CSsmSwpPolicyMapping : public CBase
	{
public:
	static CSsmSwpPolicyMapping* NewLC(TUint aSwpKey, const TDesC& aFile);
	static TInt CompareBySwp(const CSsmSwpPolicyMapping& aFirst, const CSsmSwpPolicyMapping& aSecond);
	~CSsmSwpPolicyMapping();
	TUint SwpKey() const;
	const TDesC& Filename() const;
	
private: // construction
	CSsmSwpPolicyMapping(TUint aSwpKey);
	void ConstructL(const TDesC& aFilename);

private:
	/** our swp key */
	TUint	iSwpKey;
	/** our associated file name */
	HBufC* iFilename;
	};	
	
#endif // #define _SSMSWPPOLICYMAP_H_

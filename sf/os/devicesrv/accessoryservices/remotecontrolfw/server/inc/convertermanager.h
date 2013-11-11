// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef CONVERTERMANAGER_H
#define CONVERTERMANAGER_H

#include <e32base.h>

class CRemConConverterPlugin;
class MRemConConverterInterface;

/**
Converter manager.
*/
NONSHARABLE_CLASS(CConverterManager) : public CBase
	{
public:
	/**
	Converter manager construction.
	@return Ownership of a new CConverterManager object
	*/
	static CConverterManager* NewL();

	/** Destructor. */
	~CConverterManager();

public:
	/** Returns the converter interface which supports the given outer-layer 
	API and bearer UIDs, or NULL if it does not exist. */
	MRemConConverterInterface* Converter(TUid aInterfaceUid, TUid aBearerUid) const;
	
	/** Returns the converter interface which supports the given outer-layer 
	API and bearer UIDs, or NULL if it does not exist. */
	MRemConConverterInterface* Converter(const TDesC8& aInterfaceData, TUid aBearerUid) const;

private:
	/** Constructor. */
	CConverterManager();
	
	/** 2nd-phase construction. */
	void ConstructL();

private: // utility
	/** Called during (2-phase) construction to instantiate all the converter 
	plugins, from ROM only. */
	void LoadAllConvertersL();
	
#ifdef __FLOG_ACTIVE
	void LogConverters() const;
#endif // __FLOG_ACTIVE

private: // owned
	RPointerArray<CRemConConverterPlugin> iConverters;

	// Struct wrapping a pointer to a converter interface object with the UID 
	// identifying that interface and the UID identifying the converter 
	// supplying the interface.
	// In the current implementation iIfUid will always be the same as we've 
	// only (so far) released one version of the converter API.
	struct TInterfaceInfo
		{
		MRemConConverterInterface* iIf;
		TUid iIfUid;
		TUid iConverterUid;
		};
	RArray<TInterfaceInfo> iConverterIfs;
	};

#endif // CONVERTERMANAGER_H

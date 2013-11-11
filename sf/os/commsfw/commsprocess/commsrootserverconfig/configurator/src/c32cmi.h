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
// Defines rootserver startup configuration classes
// Defines the reference Comms Configurator
//
//

/**
 @file
 @publishedPartner
 @released
*/

#if !defined(__C32CMI_H__)
#define __C32CMI_H__

#include <comms-infras/commsdebugutility.h>
#include <c32root.h>
#include <e32base.h>
#include <f32file.h>

class CBindingRef;

class C32ParseIniFile : public CBase
/** Provides basic read access to a .CMI/.INI - format file
@internalComponent */
	{
public:
	enum { KTokenSize = 32 };

	static C32ParseIniFile* NewL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer);
	virtual ~C32ParseIniFile();

	TBool FindVar(const TDesC8 &aSection, const TDesC8 &aVarName, TPtrC8 &aResult);
	TBool FindVar(const TDesC8 &aSection, const TDesC8 &aVarName, TInt &aResult);
	TBool FindVar(const TDesC8 &aSection, const TDesC8 &aVarName, TPtrC8 &aResult, TInt aEnumerator);

	TPtrC8 RetrieveSectionStart(const TDesC8& aSectionName);
private:
	void ConstructL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer);

	void ReadFileL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer);

private:
	/** Temporary buffer used by the lexer. */
	HBufC8* iToken;
	/** Buffer for the .CMI file data. */
	HBufC8* iCmiData;

	// Debug build logging
	__FLOG_DECLARATION_MEMBER;
	};


class C32CmiData : public CBase
/** Used to "parse" the contents of a .CMI file for the Comms Configurator.
@internalComponent */
	{
public:

	enum OptionBits
		/** Bits to set/unset - specifying which options was present in the CMI data.
		@internalComponent */

		{
		AttrName			= 1,
		AttrFileName		= 1<<1,
		AttrIniDataFile		= 1<<2,
		AttrIsServer		= 1<<3,
		AttrPriority		= 1<<4,
		AttrStackSize		= 1<<5,
		AttrHeapOption		= 1<<6,
		AttrStartSequence	= 1<<7,
		AttrStartBindings	= 1<<8,
		AttrIniDataAttached	= 1<<9,
		AttrMinHeapSize		= 1<<10,
		AttrMaxHeapSize		= 1<<11,
		AttrSharedHeapName  = 1<<12,
		AttrIniData			= 1<<13,
		AttrThreadFuncOrdinal = 1<<14,
		AttrIsSticky		= 1<<15,
		AttrSystemCritical  = 1<<16,
		AttrControlFlags	= 1<<17,
		AttrSystemCriticalAfterInit = 1<<18,
		AttrScaledStartupState = 1<<19,
		AttrOnDemand 		= 1<<20,
		AttrGroupName 		= 1<<21

		};

	static C32CmiData* NewL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer);
	virtual ~C32CmiData();
	TBool FindVar(const TDesC8 &aSection, const TDesC8 &aVarName, TPtrC8 &aResult);
	TBool FindVar(const TDesC8 &aSection, const TDesC8 &aVarName, TInt &aResult);

	TPtrC8& Name() {return iAttrName;}
	TPtrC8& FileName() {return iAttrFileName;}
	TPtrC8& IniDataFile() {return iAttrIniDataFile;}
	TInt IsServer() const {return iAttrIsServer;}
	TInt IsSticky() const {return iAttrIsSticky;}
	TInt SystemCritical() const {return iAttrSystemCritical;}
	TInt SystemCriticalAfterInit() const {return iAttrSystemCriticalAfterInit;}
	TUint32 ControlFlags() const {return iAttrControlFlags;}
	TThreadPriority Priority() const {return iAttrPriority;}
	TInt StackSize() const {return iAttrStackSize;}
	TRSHeapType HeapOption() const {return iAttrHeapOption;}
	TInt MinHeapSize() const {return iAttrMinHeapSize;}
	TInt MaxHeapSize() const {return iAttrMaxHeapSize;}
	TUint32 ScaledStartupState() const { return iAttrScaledStartupState; }
	TInt ThreadFuncOrdinal() const {return iAttrThreadFuncOrdinal;}
	TPtrC8& SharedHeapName() {return iAttrSharedHeapName;}
	HBufC8* IniDataL();
	TBool NextBindingL(TRSBindingInfo& aBinding, TBool aReset=EFalse);

	//Methods added for on demand loading
	TPtrC8& GroupName() {return iAttrGroupName;}
	inline TBool IsOnDemand();

protected:
	void ConstructL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer);
	C32CmiData();
private:
	void RetrieveIniDataSection();
	void RetrieveAttributesL();
	void RetrieveAttribute(TInt aAttr, const TDesC8 &aVarName, TPtrC8 &aResult);
	void RetrieveAttribute(TInt aAttr, const TDesC8 &aVarName, TInt &aResult);
	void RetrievePriorityL();
	void RetrieveHeapTypeL();
	TBool CompareHeapTypes(const TDesC8 &aType1, const TDesC8 &aType2, TRSHeapType aType3);
	void SetAttr(TUint aAttribute) { iAttributes|=aAttribute; }
	TBool AttrIsSet(TUint aAttribute) const { return ((iAttributes&aAttribute)==aAttribute)?ETrue:EFalse; }
	void SplitBindingL(TPtrC8 aSource, TPtrC8& aAddr1, TPtrC8& aAddr2,
                            TPtrC8& aType, TInt& aForwardQLength, TInt& aReverseQLength);
	void MakeAddressL(const TPtrC8& aAddress, TCFSubModuleAddress& aSubModuleAddress);
	TRSBindType MakeBindingTypeL(const TDesC8& aTxtType);
private:
	/** CMI file parser */
	C32ParseIniFile* iCmiFile;
	/** Options set in the CMI file, this is a bitmask containing the OptionBits enums. */
	TUint iAttributes;

	/** Required options - as a bitmask. Depends on configuration.
	E.g. if the HeapOption was "EShareHeap" then it is suddenly
	also required that SharedHeapName is defined in the file and
	this mask should have AttrSharedHeapName added to it.
	When finished parsing the file iAttributes should as a minimum
	match this mask, or there was a missing option... */
	TUint iAttrRequired;

	/** Number of next binding to return (Binding0, Binding1, etc..) */
	TInt			iNextBinding;		//

	// Below members will be initialised as we parse the file
	TPtrC8			iAttrName;
	TPtrC8			iAttrFileName;
	TPtrC8			iAttrIniDataFile;
	TInt			iAttrIsServer;
	TThreadPriority	iAttrPriority;
	TInt			iAttrStackSize;
	TRSHeapType		iAttrHeapOption;
	TInt			iAttrMinHeapSize;
	TInt			iAttrMaxHeapSize;
	TUint32			iAttrScaledStartupState;
	TPtrC8			iAttrSharedHeapName;
	TPtrC8			iAttrIniData;
	TInt			iAttrThreadFuncOrdinal;
	TInt			iAttrIsSticky;
	TInt			iAttrSystemCritical;
	TInt			iAttrSystemCriticalAfterInit;
	TUint32			iAttrControlFlags;
	//members added for on demand loading
	TInt			iAttrOnDemand;
	TPtrC8			iAttrGroupName;
	// Debug build logging
	__FLOG_DECLARATION_MEMBER;
	};
#include "c32cmi.inl"

#endif // __C32CMI_H__


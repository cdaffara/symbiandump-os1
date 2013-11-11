// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// rscommon.inl
//


#if !defined(__RSSHARED_INL__)
#define __RSCOMMON_INL__

/**
    Default constructor for TRSStartModuleParamContainer
*/
inline TRSStartModuleParamContainer::TRSStartModuleParamContainer()
    {
    iPriority = EPriorityNull;
    iStackSize = -1;
    iHeapType = EDefaultHeap;
    iMinHeapSize = -1;
    iMaxHeapSize = -1;
    iThreadFunctionOrdinal = -1;
    iIsSticky = EFalse;
    iIsServer = EFalse;
    iControlFlags = 0;
    }



inline TRSStartModuleParams::TRSStartModuleParams(const CommsFW::TCFModuleName& aName, const TFileName& aFileName,
												  const TThreadPriority aPriority, TInt aStackSize,
												  const TRSHeapType aHeapType, TInt aMinHeapSize,
												  TInt aMaxHeapSize, const CommsFW::TCFModuleName& aShareHeapWith,
												  TInt aThreadFunctionOrdinal, TBool aIsSticky, TUint32 aControlFlags):
TPckg<TRSStartModuleParamContainer>(iParams)
	/**	Constructor; copies the parameters.
    @param aName Name of the module and its thread.
	@param aFileName Filename of the Comms Provider Module DLL.
	@param aPriority Priority of the CPM thread.
	@param aStackSize Initial size of the stack.
	@param aHeapType Type of the heap for the module.
	@param aMinHeapSize Minimum size of the heap for the module.
	@param aMaxHeapSize Maximum size of the heap for the module.
	@param aShareHeapWith If the module are to share the heap with an existing module this must be the name	of the other module.
	@param aThreadFunctionOrdinal Ordinal of the CPM main thread function, or entrypoint, in the DLL.
	@param aIsSticky If ETrue specifies that this module can not be unloaded.
	@param aControlFlags modle thread control flags
	
	@see TCFModuleName
	@see TFileName
	@see TThreadPriority
	@see TRSHeapType
	@see TRSBindType
	*/	
	{
	iParams.iName = aName;
	iParams.iFilename = aFileName;
	iParams.iPriority = aPriority;
	iParams.iStackSize = aStackSize;
	iParams.iHeapType = aHeapType;
	iParams.iMinHeapSize = aMinHeapSize;
	iParams.iMaxHeapSize = aMaxHeapSize;
	iParams.iShareHeapWith = aShareHeapWith;
	iParams.iThreadFunctionOrdinal = aThreadFunctionOrdinal;
	iParams.iIsSticky = aIsSticky;
	iParams.iControlFlags = aControlFlags;
	}

inline TRSStartModuleParams::TRSStartModuleParams(const CommsFW::TCFModuleName& aName, const TFileName& aFileName,
												  const TThreadPriority aPriority, TInt aStackSize,
												  const TRSHeapType aHeapType, TInt aMinHeapSize,
												  TInt aMaxHeapSize, TInt aThreadFunctionOrdinal,
												  TBool aIsSticky, TUint32 aControlFlags):
TPckg<TRSStartModuleParamContainer>(iParams)
	/**	Constructor; copies the parameters.
    @param aName Name of the module and its thread.
	@param aFileName Filename of the Comms Provider Module DLL.
	@param aPriority Priority of the CPM thread.
	@param aStackSize Initial size of the stack.
	@param aHeapType Type of the heap for the module.
	@param aMinHeapSize Minimum size of the heap for the module.
	@param aMaxHeapSize Maximum size of the heap for the module.
	@param aThreadFunctionOrdinal Ordinal of the CPM main thread function, or entrypoint, in the DLL.
	@param aIsSticky If ETrue specifies that this module can ot be unloaded.
	@param aControlFlags modle thread control flags
	
	@see TCFModuleName
	@see TFileName
	@see TThreadPriority
	@see TRSHeapType
	@see TRSBindType
	*/	
	{
	iParams.iName = aName;
	iParams.iFilename = aFileName;
	iParams.iPriority = aPriority;
	iParams.iStackSize = aStackSize;
	iParams.iHeapType = aHeapType;
	iParams.iMinHeapSize = aMinHeapSize;
	iParams.iMaxHeapSize = aMaxHeapSize;
	iParams.iThreadFunctionOrdinal = aThreadFunctionOrdinal;
	iParams.iIsSticky = aIsSticky;
	iParams.iControlFlags = aControlFlags;
	}

inline TRSStartModuleParams::TRSStartModuleParams(const CommsFW::TCFModuleName& aName, const TFileName& aFileName,
												  const TThreadPriority aPriority, TInt aStackSize,
												  const TRSHeapType aHeapType, TInt aThreadFunctionOrdinal, 
												  TBool aIsSticky, TUint32 aControlFlags):
TPckg<TRSStartModuleParamContainer>(iParams)
	/**	Constructor; copies the parameters.
    @param aName Name of the module and its thread.
	@param aFileName Filename of the Comms Provider Module DLL.
	@param aPriority Priority of the CPM thread.
	@param aStackSize Initial size of the stack.
	@param aHeapType Type of the heap for the module.
	@param aThreadFunctionOrdinal Ordinal of the CPM main thread function, or entrypoint, in the DLL.
	@param aIsSticky If ETrue specifies that this module can ot be unloaded.
	@param aControlFlags modle thread control flags
	
	@see TCFModuleName
	@see TFileName
	@see TThreadPriority
	@see TRSHeapType
	@see TRSBindType
	*/
	{
	iParams.iName = aName;
	iParams.iFilename = aFileName;
	iParams.iPriority = aPriority;
	iParams.iStackSize = aStackSize;
	iParams.iHeapType = aHeapType;
	iParams.iMinHeapSize = 0;
	iParams.iMaxHeapSize = 0;
	iParams.iThreadFunctionOrdinal = aThreadFunctionOrdinal;
	iParams.iIsSticky = aIsSticky;
	iParams.iControlFlags = aControlFlags;
	}

inline TRSModuleInfo::TRSModuleInfo(const CommsFW::TCFModuleName aName, const TFileName& aDll,
									TRSModuleState aState, TInt aNumSubModules):
TPckg<TRSModuleInfoContainer>(iParams)
	/** Constructor; Copies the parameters.
	@param aName Name of the module.
	@param aDll Filename of the CPM DLL.
	@param aState State of the CPM.
	@param aNumSubModules Number of submodules this module has reported. 
	*/
	{
	iParams.iName = aName;
	iParams.iDll = aDll;
	iParams.iState = aState;
	iParams.iNumSubModules = aNumSubModules;
	}

inline TRSSubModuleAddress::TRSSubModuleAddress(const CommsFW::TCFSubModuleAddress& aAddress):
TPckg<CommsFW::TCFSubModuleAddress>(iAddress)  
	/** Constructor; copies the parameter.
	@param aAddress The fully qualified address of the submodule.
	@see TCFSubModuleAddress
	*/	
	{
	iAddress = aAddress;
	};

inline TRSSubModuleAddress::TRSSubModuleAddress(const CommsFW::TCFModuleName& aModule, const CommsFW::TCFSubModuleName& aSubModule):
TPckg<CommsFW::TCFSubModuleAddress>(iAddress) 
	/** Constructor; copies the parameters.
	@param aModule The name of the module containing the submodule.
	@param aSubModule The name of the submodule within aModule.
	*/	
	{
	iAddress.SetModule(aModule);
	iAddress.SetSubModule(aSubModule);
	};

inline TRSBindingInfo::TRSBindingInfo(const TRSBindType& aType, const CommsFW::TCFSubModuleAddress& aAddress1,
									  const CommsFW::TCFSubModuleAddress& aAddress2,
						  			  TInt aForwardQLength, TInt aReverseQLength):
TPckg<TRSBindingInfoContainer>(iParams)
	/** Constructor; copies the parameters.
	@param aType Type of binding.
	@param aAddress1 Fully qualified address of the first submodule.
	@param aAddress2 Fully qualified address of the second submodule.
	@param aForwardQLength The number of slots in the forward queue.
	@param aReverseQLength The number of slots in the reverse direction.
	@see TCFSubModuleAddress
	@see TRSBindType
	 */
	{
	iParams.iType = aType;
	iParams.iAddress1 = aAddress1;
	iParams.iAddress2 = aAddress2;
	iParams.iState1 = KErrNone;
	iParams.iState2 = KErrNone;
	iParams.iForwardQLength = aForwardQLength;
	iParams.iReverseQLength = aReverseQLength;
	}

/** A copy constructor for the TRSBindingInfo class
*/
inline TRSBindingInfo::TRSBindingInfo(const TRSBindingInfo &aRef)
	:TPckg<TRSBindingInfoContainer>(aRef)
    {
    iParams = aRef.iParams;    
    iPtr = REINTERPRET_CAST(TUint8*, &iParams);
    }

inline TRSBindingInfo& TRSBindingInfo::operator=(const TRSBindingInfo& aInfo)
	/** Assignment operator, copies the content of the right side to the
		instance on the left side.
		@param aInfo The TRSBindingInfo instance to copy from.
	*/
	{
	iParams=aInfo.iParams;
	iPtr = REINTERPRET_CAST(TUint8*, &iParams);
	return *this;
	}

inline TRSBindingInfoContainer& TRSBindingInfoContainer::operator=(const TRSBindingInfoContainer& aInfo)
	/** Assignment operator, copies the content of the right side to the
		instance on the left side.
		@param aInfo The TRSBindingInfoContainer instance to copy from.
	*/
	{
	iType=aInfo.iType;
	iAddress1=aInfo.iAddress1;
	iAddress2=aInfo.iAddress2;
	iState1=aInfo.iState1;
	iState2=aInfo.iState2;
	iForwardQLength=aInfo.iForwardQLength;
	iReverseQLength=aInfo.iReverseQLength;
	return *this;
	}

inline TRSUnBindingInfo::TRSUnBindingInfo(const CommsFW::TCFSubModuleAddress aAddress1, const CommsFW::TCFSubModuleAddress aAddress2):
TPckg<TRSUnBindingInfoContainer>(iParams)
	/** 
	@param aAddress1 Fully qualified address of the first submodule.
	@param aAddress2 Fully qualified address of the second submodule.
	@see TCFSubModuleAddress
	 */
	{
	iParams.iAddress1 = aAddress1;
	iParams.iAddress2 = aAddress2;
	iParams.iState1 = KErrNone;
	iParams.iState2 = KErrNone;
	}

inline TRSIter::TRSIter()
: iPosition(0)
	/** Constructor; initializes the iterator to first position.
	*/
	{ }

inline const TInt& TRSIter::operator()() const
	/** Returns the current position of the iteration. Note that does not 
	relate directly to the position of a specific module.
	*/
	{
	return iPosition;
	}

inline TInt& TRSIter::operator()()
	/** Returns a reference to the iteration count. Note that does not 
	relate directly to the position of a specific module.
	*/
	{
	return iPosition;
	}

inline void TRSIter::Reset()
	/** Resets the iteration to start from the beginning.
	*/
	{
	iPosition = 0;
	}

#endif // __RSCOMMON_INL__


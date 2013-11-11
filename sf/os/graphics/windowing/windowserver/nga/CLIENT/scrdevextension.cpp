// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "scrdevextension.h"
#include "../SERVER/w32cmd.h"
#include <graphics/displaycontrolbase.h>
#include "CLIENT.H"
#include "w32comm.h"

CWsScreenDevice::CScrDevExtension::CScrDevExtension(RWsBuffer *aBuffer,TInt32 aWsHandle):	MWsClientClass(aBuffer)
	{
	iWsHandle=aWsHandle;
	__ASSERT_DEBUG(aBuffer,Panic(EW32PanicBadClientInterface));
	__ASSERT_DEBUG(aWsHandle,Panic(EW32PanicBadClientInterface));
	}

CWsScreenDevice::CScrDevExtension::~CScrDevExtension()
	{
	//typeface store is not owned by this class, and is created/destroyed in CWsScreenDevice
	}
/** Interface Extension capability
 * 	Use of this interface going forward will allow the published client interface to be dynamically extended.
 * 	Note that the pointer returned is only good for the lifetime of the called CBase derived object.
 * 	@pre	caller has already checked that the implementation is initialised using RepeatableConstruct
 *	@param  aInterfaceId	uniqueid or well known id of interface
 * 	@return	pointer to interface object matching this ID or NULL if no match.
 **/
void* CWsScreenDevice::CScrDevExtension::GetInterface(TUint aInterfaceId)
	{
	if (RepeatableConstruct()<KErrNone)
		{
		return NULL;
		}
	__ASSERT_DEBUG(this!=NULL,Panic(EW32PanicBadClientInterface));
	__ASSERT_DEBUG(iBuffer,Panic(EW32PanicBadClientInterface));
	__ASSERT_DEBUG(iWsHandle!=NULL,Panic(EW32PanicBadClientInterface));
	
	switch (aInterfaceId)
		{
		case MDisplayControlBase::ETypeId:
			if (iSupportedExtensionsBits&TWsSdXDisplayControl)
				{
				return static_cast<MDisplayControlBase*>(this);
				}
			break;
		case MDisplayControl::ETypeId:
			if (iSupportedExtensionsBits&TWsSdXDisplayControl)
				{
				return static_cast<MDisplayControl*>(this);
				}
			break;
		case MDisplayMappingBase::ETypeId:
			if(iSupportedExtensionsBits&TWsSdXDisplayMapping)
				{
				return static_cast<MDisplayMappingBase*>(this);
				}
			break;
		case MDisplayMapping::ETypeId:
			if(iSupportedExtensionsBits&TWsSdXDisplayMapping)
				{
				return static_cast<MDisplayMapping*>(this);
				}
			break;
		case MTestScreenCapture::ETypeId:
			{
			TInt requiredIf = TWsSdXDebugComposition | TWsSdXDisplayMapping;
			if ((iSupportedExtensionsBits & requiredIf) == requiredIf)
				{
				return static_cast<MTestScreenCapture*>(this);
				}
			break;
			}
		default:
			break;
		}
	return NULL;
}
//Accessor to typeface store instance
CFbsTypefaceStore* CWsScreenDevice::CScrDevExtension::TypefaceStore()
	{
	return iTypefaceStore;
	}
//Accessor to typeface store instance
void  CWsScreenDevice::CScrDevExtension::SetTypefaceStore(CFbsTypefaceStore* aTypeFaceStore)
	{
	iTypefaceStore=aTypeFaceStore;
	}

/**
 * Constructs the extension interface implementation, or returns error if interface is not available.
 * After success, the interface is then always available and is not re-constructed if method called again, 
 * but after failure the construct attempt will be repeated (and fail again) if the method is called again.
 * Clients would be expected to make then keep a pointer to this interface, not get new ones repeatedly. 
 * Note that if the extension was not allocated, then "this" could be NULL
 * @param aWsHandle	server-side object handle to use in messages if first time
 * @return KErrNone if initialised correctly, or a standard error code.
 **/
TInt CWsScreenDevice::CScrDevExtension::RepeatableConstruct()
	{
	if (this==NULL)
		{
		return KErrNoMemory;	//The extension was not allocated. Making this call is bad!
		}
	__ASSERT_DEBUG(iBuffer,Panic(EW32PanicBadClientInterface));
	__ASSERT_DEBUG(iWsHandle,Panic(EW32PanicBadClientInterface));
	if (iSupportedExtensionsBits==0)
		{	//need to initialise
		TInt	ExtensionsOrError=	WriteReply(EWsSdOpExtensionsSupported);
		if (ExtensionsOrError&TWsSdXReservedErrorFlag)
			{
			//Server is allowed to report that lower level drivers did not support dynamic screen res.
			//Any other error is unexpected
			__ASSERT_DEBUG(ExtensionsOrError==KErrExtensionNotSupported,Panic(EW32PanicBadClientInterface));
			ExtensionsOrError=TWsSdXReservedErrorFlag;
			return ExtensionsOrError;
			}
		else
			{
			iSupportedExtensionsBits=ExtensionsOrError;
			}
		}
	return KErrNone;
	}

TInt CWsScreenDevice::CScrDevExtension::NumberOfResolutions() const
	{
	__ASSERT_DEBUG(iSupportedExtensionsBits&TWsSdXDisplayControl,Panic(EW32PanicBadClientInterface));
	TInt	numResOrError=WriteReply(EWsSdOpXDcGetNumberOfResolutions);
	return numResOrError;
	}

TInt CWsScreenDevice::CScrDevExtension::FillResolutionArray(TInt aNumOfRes, RArray<TResolution>& aResolutions) const
	{
	TInt arrayMaxSize=aResolutions.Count();
	if (arrayMaxSize < aNumOfRes)
		{
		//Array is too small or not initialized
		TResolution emptyRes(TSize(0,0),TSize(0,0));	//ARM BUILD needs this form of constructor!
		for (TInt index = 0;index < aNumOfRes-arrayMaxSize;index++)
			aResolutions.Append(emptyRes);
	
		//reset arrayMaxSize
		arrayMaxSize = aNumOfRes;
		}
	//Else array is large enough.

	TPtr8 pArr((TUint8*)&aResolutions[0],arrayMaxSize*sizeof(TResolution),arrayMaxSize*sizeof(TResolution));
	return WriteReplyP(NULL,0,TWriteDescriptorType(&pArr),EWsSdOpXDcGetResolutionsList);
	}
TInt CWsScreenDevice::CScrDevExtension::GetResolutions(RArray<TResolution>& aResolutions) const
	{
	TInt result;
	__ASSERT_DEBUG(iSupportedExtensionsBits&TWsSdXDisplayControl,Panic(EW32PanicBadClientInterface));
	
	TInt resCount=WriteReply(EWsSdOpXDcGetNumberOfResolutions);
	if(resCount < KErrNone)
		{
		return resCount;
		}
	
	result=aResolutions.Reserve(resCount);
	if (result<KErrNone)
		{
		return result;	//Failed - probably KErrNoMemory
		}
	
	if(resCount > 0)
		{
		result = FillResolutionArray(resCount, aResolutions);
		if (result<KErrNone)
			{
			return result;	//Content of the array is "undefined"
			}
		__ASSERT_DEBUG((result%sizeof(TResolution))==0,Panic(EW32PanicSizeNotExpected));
		result=result/sizeof(TResolution);
		
		if(result > aResolutions.Count())
			{
			//The resolution list at server side is larger and can't fit in client buffer we supplied
			//The Content of the array is undefined at this point. 
			//Give it one more try
			result = FillResolutionArray(result, aResolutions);
			if(result < KErrNone)
				{
				return result;
				}
			__ASSERT_DEBUG((result%sizeof(TResolution))==0,Panic(EW32PanicSizeNotExpected));
			result=result/sizeof(TResolution);
			
			if(result > aResolutions.Count()) 
				{//give up
				return KErrCorrupt; //which means resolution list is changing during the process of getting it 
				}
			}
		
		TInt arrayMaxSize = aResolutions.Count();
		while(result<arrayMaxSize)
			{
			aResolutions.Remove(--arrayMaxSize);
			}
		}
	else
		{
		aResolutions.Reset(); //There's not available resolutions
		}
	return KErrNone;	//Content of array is good.
	}

void CWsScreenDevice::CScrDevExtension::GetConfiguration(TDisplayConfiguration& aConfig) const
	{
	__ASSERT_DEBUG(iSupportedExtensionsBits&TWsSdXDisplayControl,Panic(EW32PanicBadClientInterface));
	TInt currentVersion = aConfig.Version();
	if (sizeof(TDisplayConfiguration)<currentVersion)
		currentVersion = sizeof(TDisplayConfiguration);
	TPtr8 displayPtr((TUint8 *)&aConfig, currentVersion);
	WriteReplyP(&aConfig, sizeof(TDisplayConfiguration), &displayPtr, EWsSdOpXDcGetConfiguration);
	}
TInt CWsScreenDevice::CScrDevExtension::SetConfiguration(const TDisplayConfiguration& aConfig)
	{
	__ASSERT_DEBUG(iSupportedExtensionsBits&TWsSdXDisplayControl,Panic(EW32PanicBadClientInterface));
	TInt currentVersion = aConfig.Version();
	if (TDisplayConfiguration().Version()<currentVersion)
		currentVersion = TDisplayConfiguration().Version();
	TInt reply=WriteReply(&aConfig, sizeof(TDisplayConfiguration), EWsSdOpXDcSetConfiguration);
	return reply;
	}
TInt CWsScreenDevice::CScrDevExtension::PreferredDisplayVersion() const
	{
	__ASSERT_DEBUG(iSupportedExtensionsBits&TWsSdXDisplayControl,Panic(EW32PanicBadClientInterface));
	TInt reply=WriteReply(EWsSdOpXDcGetPreferredDisplayVersion);
	return reply;
	}

TBool CWsScreenDevice::CScrDevExtension::DisplayChangeEventsEnabled() const
	{
	__ASSERT_DEBUG(iSupportedExtensionsBits&TWsSdXDisplayControl,Panic(EW32PanicBadClientInterface));
	if(WriteReply(EWsSdOpXDcDisplayChangeEventEnabled))
		return ETrue;
	return EFalse;
	}
void CWsScreenDevice::CScrDevExtension::EnableDisplayChangeEvents(TBool aEnable)
	{
	__ASSERT_DEBUG(iSupportedExtensionsBits&TWsSdXDisplayControl,Panic(EW32PanicBadClientInterface));
	if(aEnable)
		Write(EWsSdOpXDcNotifyOnDisplayChange);
	else
		Write(EWsSdOpXDcNotifyOnDisplayChangeCancel);
	}
TInt CWsScreenDevice::CScrDevExtension::MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const
	{
	TWsSdCmdMapCoordinates cmdMapCoord(aSource, aSourceSpace, aTargetSpace);
	TPtr8 targetRectPtr((TUint8 *)&aTarget, sizeof(TRect));
	TInt reply = WriteReplyP(&cmdMapCoord, sizeof(TWsSdCmdMapCoordinates), &targetRectPtr, EWsSdOpXDmMapExtent);
	return reply;
	}

void CWsScreenDevice::CScrDevExtension::GetMaximumWindowExtent(TRect& aExtent) const	
	{
	TDisplayConfiguration config;
	GetConfiguration(config);
	aExtent.SetRect(0,0,0,0);
	if(config.IsDefined(TDisplayConfigurationBase::EResolution))
		{
		TSize fullUiSize;
		config.GetResolution(fullUiSize);
		TRect fullUiRect(TPoint(0,0), fullUiSize);
		aExtent=fullUiRect;
#ifdef _DEBUG
		TInt err = 
#endif
			MapCoordinates(EFullScreenSpace, fullUiRect, EApplicationSpace, aExtent);
		//This should NOT fail even if display is detached/disabled.
		__ASSERT_DEBUG(err>=KErrNone, Panic(EW32PanicBadClientInterface));
		} 
	}
void CWsScreenDevice::CScrDevExtension::GetMaximumSurfaceSize(TSize& aPixels, TSize& aTwips) const
	{
	TDisplayConfiguration config;
	GetConfiguration(config);
	TInt err = KErrGeneral;
	TSize fullUiSize;
	aPixels=fullUiSize;
	aTwips=fullUiSize;
	if(config.IsDefined(TDisplayConfigurationBase::EResolution))
		{
		config.GetResolution(fullUiSize);
		TRect fullUiRect(TPoint(0,0), fullUiSize);
		TRect compositionRect;
		err = MapCoordinates(EFullScreenSpace, fullUiRect, ECompositionSpace, compositionRect);
		//This WILL fail if display is detached/disabled.
		if(err == KErrNone)
			{
			aPixels = compositionRect.Size();
			}
		}
	if(config.IsDefined(TDisplayConfigurationBase::EResolutionTwips))
		{
		config.GetResolutionTwips(aTwips);
		}
	//Why can't this function return an error?
	//In case of error the return values will be empty. 
	}

void CWsScreenDevice::CScrDevExtension::GetDisplayExtentOfWindow(const RWindowBase& aWindow, TRect& aExtent) const
	{
	TRect winRect(aWindow.AbsPosition(), aWindow.Size());
	aExtent.SetRect(0,0,0,0);
	TInt err = MapCoordinates(EApplicationSpace, winRect, ECompositionSpace, aExtent);
	//This WILL fail if display is detached/disabled.
	//Why can't this function return an error?
	//In case of error the return values will be empty. 
	}

TInt CWsScreenDevice::CScrDevExtension::TranslateExtent(const TRect& aInitial, TRect& aTarget) const
	{
	TPckgBuf<TRect> rectRes;
	TWsScsComposeScreenCommand translateExtentCmd(EWsScsTranslateExtent, aInitial);
	TInt ret = WriteReplyP(&translateExtentCmd, sizeof(translateExtentCmd), &rectRes, EWsSdOpXTestScreenCapture);
	if (ret == KErrNone)
		{
		aTarget=rectRes();
		}
	return ret;
	}

TInt CWsScreenDevice::CScrDevExtension::GetCompositedSize(TSize& aSize) const
	{
	TPckgBuf<TSize> sizePkg;
	WriteReplyP(&sizePkg,EWsSdOpXTestScreenCaptureSize);
	aSize = sizePkg();
	return KErrNone;
	}

TInt CWsScreenDevice::CScrDevExtension::ComposeScreen(const CFbsBitmap& aBitmap) const
	{
	TInt bitmapHandle = aBitmap.Handle();
	AddToBitmapArray(bitmapHandle);
	TWsScsComposeScreenCommand composeScreenCmd(EWsScsScreenCompose, bitmapHandle);
	return(WriteReply(&composeScreenCmd,sizeof(composeScreenCmd), EWsSdOpXTestScreenCapture));
	}

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

#ifndef __DEVVIDEOINTERNAL_H__
#define __DEVVIDEOINTERNAL_H__

#include <e32base.h>
#include <ecom/ecom.h>

/**
Static utilities class.
@internalComponent
*/
class DevVideoUtilities
	{
public:
	/** 
	Convert a text number into a real number using lexical analysis
	*/
	static TInt ConvertTextToTUint32(const TDesC8& aData, TUint32& aNumber);

	/**
	Inspect the plugin implementation information objects to see if the required pre/post proc type is supported. 
	If so, the UID of the plugin is appended to aMatchingPlugins
	*/
	static void MatchPrePostProcessorCapabilitiesL(const RImplInfoPtrArray& aPlugins, TUint32 aReqPrePostProcType, RArray<TUid>& aMatchingPlugins);

	/**
	Select a plug-in that supports the mime type. If a plug-in does not support the mime type it is removed
	from the array and deleted. Therefore after a call to this method "aImplInfoArray" contains only plug-in
	that support the supplied mime type or is null if none was found.
	*/
	static void SelectPluginBasedOnMatchType(const TDesC8& aMatchType, RImplInfoPtrArray& aImplInfoArray);

#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	static void FindVideoDecoderPluginsL(const TDesC8& aMimeType, RImplInfoPtrArray& aImplInfoArray);
	static void FindVideoEncoderPluginsL(const TDesC8& aMimeType, RImplInfoPtrArray& aImplInfoArray);
	static void CreatePuListL(RImplInfoPtrArray& aImplInfoArray);
	static const CImplementationInformation* FindPu(const RImplInfoPtrArray& aImplInfoArray, TUid aPuUid);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	};

/** 
 * Template class CleanupResetAndDestroy to clean up the array
 * of implementation information from the cleanup stack.
 */

template <class T>
class CleanupResetAndDestroy
	{
public:
	inline static void PushL(T& aRef);
private:
	static void ResetAndDestroy(TAny *aPtr);
	};
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);
template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
	{CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));}
template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
	{(static_cast<T*>(aPtr))->ResetAndDestroy();}
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
	{CleanupResetAndDestroy<T>::PushL(aRef);}

#endif

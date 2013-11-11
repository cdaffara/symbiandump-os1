
// MmfGlblAudioEffectPlugin.h

// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFGLBLAUDIOEFFECTPLUGIN_H
#define MMFGLBLAUDIOEFFECTPLUGIN_H

#include <mmf/common/mmfglblaudioeffect.h>

/**
@publishedPartner
@released
@file
*/

/**
Implementation of CMmfGlobalAudioEffect.
This is the base interface for plugins that provide for global effects. It is intended
to be exclusively called from CMmfGlobalAudioEffect itself.
 */
 
class MMmfGlobalAudioImpl
	{
	friend class CMmfGlobalAudioEffect; 
	
public:
	/**
	Release object.
	Equivalent of destructor - called to request data to be deleted.
	*/
	virtual void Release()=0;

protected:
	/*
	Provide implementation of CMmfGlobalAudioEffect::Capability()
	@param aCurrentOnly
	       If true, capabilities are for current situation only - see text
	@return Capability settings, as given in TCapabilityFlags
	@see CMmfGlobalAudioEffect::Capability()
	*/	
	virtual TUint Capability(TBool aCurrentOnly)=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::RequestNotificationL()
	@param aEventUid
	       Uid specifying event for which notification is requested
	       
	@leave KErrNotSupported
	       If Observer passed during construction was NULL.
	       The Uid is not recognised.
	       The feature is simply not supported in this implementation.
	@see CMmfGlobalAudioEffect::RequestNotificationL()
	*/
	virtual void RequestNotificationL(TUid aEventUid)=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::IsEnabled()
	@return True if SetEnabledL(ETrue), or similar, has been called on this effect
	@see CMmfGlobalAudioEffect::IsEnabled()
	*/
	virtual TBool IsEnabled() const=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::IsActive()
	@return True if the effect is in use
	@see CMmfGlobalAudioEffect::IsActive()
	*/
	virtual TBool IsActive() const=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::SetEnabledL()
	@param aValue
	       If true, enables this specific effect. If false, disables it. 
	@see CMmfGlobalAudioEffect::SetEnabledL()
	*/
	virtual void SetEnabledL(TBool aValue)=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::SettingsByUidL()
	@return The Uid used by SetSettingsByUidL()
	@leave KErrNotSupported
	       The settings cannot be expressed as a Uid (usually means SetSettingsByUidL() was not the last
	       thing to change them).
	@see CMmfGlobalAudioEffect::SettingsByUidL()
	*/
	virtual TUid SettingsByUidL() const=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::SetSettingsByUidL()
	@param aPresetUid
	       Uid representing the preset in question
	@leave KErrNotSupported
	       This implementation does not support presets for this effect
	@leave KErrUnknown
	       The value of aUid does not correspond to a known preset
	@see CMmfGlobalAudioEffect::SetSettingsByUidL()
	*/
	virtual void SetSettingsByUidL(TUid aPresetUid)=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::SettingsByDesL()
	@return HBufC8 containing current settings
	@leave KErrNotSupported
	       This implementation does not support expressing settings in descriptor form
	@see CMmfGlobalAudioEffect::SettingsByDesL()
	*/
	virtual HBufC8* SettingsByDesL() const=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::SetSettingsByDesL()
	@param aParam
	       Descriptor value to use
	@leave KErrNotSupported
	       This implementation does not support expressing settings in descriptor form
	@leave KErrCorrupt
	       Value in descriptor does not correspond to known format
	@see CMmfGlobalAudioEffect::SetSettingsByDesL()	  
	*/
	virtual void SetSettingsByDesL(const TDesC8& aParam)=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::KnownPresetsL()
	@see CMmfGlobalAudioEffect::KnownPresetsL()
	@leave KErrNotSupported
	       This feature may not be supported in some circumstances
	*/
	virtual MMmfGlobalAudioPresetList* KnownPresetsL()=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::ExtractValuesL()
	Extract the settings into a struct
	Note that the implementation should check the size of the supplied package buffer, to check
	it is as expected.
	@param aPackageBuf
	       This should be a package buffer wrapping the appropriate class/struct, and will be 
	       specific to a particular CMmfGlobalAudioEffect derivitive.
	@leave KErrNotSupported
	       This will only be supported by some child classes, and even then will not
	       be supported by all implementations.
	@leave KErrArgument
		   Passed package buffer is not the expected size.
	@see CMmfGlobalAudioEffect::ExtractValuesL()
	*/
	virtual void ExtractValuesL(TDes8& aPackageBuf)=0;
	
	/**
	Provide implementation of CMmfGlobalAudioEffect::SetByValuesL().
	Note that the implementation should check the size of the supplied package buffer, to check
	it is as expected.
	@param aPackageBuf
	       This should be a package buffer wrapping the appropriate class/struct, and will be 
	       specific to a particular CMmfGlobalAudioEffect derivitive.
	@leave KErrNotSupported
	       This will only be supported by some child classes, and even then will not
	       be supported by all implementations.
	@leave KErrArgument
		   Passed package buffer is not the expected size, or individual values are out of range
	@see CMmfGlobalAudioEffect::SetByValuesL()
	*/
	virtual void SetByValuesL(const TDesC8& aPackageBuf)=0;
	
	/**
	Request extension feature.
	This is intended to provide additional features, should a particular global effect
	need it. In typical use, the global effect will make a call to this interface on
	construction. Repeatedly calling this interface will have no additional effect -
	if the interface has already been setup internally, then no further activity will
	take place.
	@param aInterfaceUid
	       Used to indicate which interface is required. 
	@return Standard error code. KErrNotSupported is used to indicate that the particular
	        plugin is used.
	*/
	virtual TInt CreateCustomInterface(TUid aInterfaceUid)=0;
	
	
	/**
	Return previously created extension.
	This returns a custom interface, used to provide additional features for a certain
	global effect. This should only be used if CreateCustomInterface() has already
	been called for the same UID value. This means that any construction for that interface
	has already been called, and thus this call cannot fail. Typically the returned class 
	will be another Mixin.No transfer of ownership is implied. 

	@param aInterfaceUid
	       Used to indicate which interface is required. 
	@return The requested interface, or NULL if not known.
	@see CreateCustomInterface()
	*/
	virtual TAny* CustomInterface(TUid aInterfaceUid)=0;
	
	
	/**
	Pass destructor key.
	Called on construction so plugin takes responsibility to call REComSession::DestroyedImplementation()
	@param aDestructorKey
	       The Uid returned by REComSession::CreateImplementationL() or similar
	*/
	virtual void PassDestructorKey(TUid aDestructorKey)=0;
	
	/**
	Complete construction.
	Pass additional values from the construction phase, used subsequently by the plugin.
	@param aParent
	       The CMmfGlobalAudioEffect that created this object
	@param aObserver
	       Observer as passed to CMmfGlobalAudioEffect::BaseConstructL()
	@see CMmfGlobalAudioEffect::BaseConstructL()
	*/
	virtual void CompleteConstructL(CMmfGlobalAudioEffect* aParent, MMmfGlobalAudioEffectObserver* aObserver)=0;
	
private:
	static MMmfGlobalAudioImpl* NewL(TUid aImplementationUid, 
									 CMmfGlobalAudioEffect* aParent,
	                                 MMmfGlobalAudioEffectObserver* aObserver);
	
	};


#endif // MMFGLBLAUDIOEFFECTPLUGIN_

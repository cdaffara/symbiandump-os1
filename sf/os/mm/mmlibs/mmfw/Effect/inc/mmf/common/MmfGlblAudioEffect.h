
// MmfGlblAudioEffect.h

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

#ifndef MMFGLBLAUDIOEFFECT_H
#define MMFGLBLAUDIOEFFECT_H

#include <e32std.h>
#include <e32base.h>
#include <bamdesca.h>
#include <mmf/common/mmfbase.hrh>

/**
@publishedPartner
@released
@file
*/

//forward decs
class MMmfGlobalAudioImpl;
class CMmfGlobalAudioEffect;
class MMmfGlobalAudioPresetList;

/**
Notify changes as requested
*/
class MMmfGlobalAudioEffectObserver
	{
public:
    /**
    Callback that event is occuring, as requested.
    This pairs with CMmfGlobalAudioEffect::RequestNotificationL()
    @param aEffect 
           The effect object from where the notification is being signaled
    @param aEventUid
           The uid passed to CMmfGlobalAudioEffect::RequestNotificationL()
    @param aParam
           Parameter data, exact use will depend on aEventUid
    @see CMmfGlobalAudioEffect::RequestNotificationL()
    */
	virtual void GAEEventNotificationL(CMmfGlobalAudioEffect* aEffect, TUid aEventUid, const TDesC8& aParam)=0;
	};
	
/**
EnableChanged event Uid
@see CMmfGlobalAudioEffect::RequestNotificationL()
*/
const TUid KUidMmfGlblAudioEnableChanged = {KUidMmfGlblAudioEnableChangedDefine};

/**
ActiveChanged event Uid
@see CMmfGlobalAudioEffect::RequestNotificationL()
*/
const TUid KUidMmfGlblAudioActiveChanged = {KUidMmfGlblAudioActiveChangedDefine};   

/**
PresetsChanged event Uid
@see CMmfGlobalAudioEffect::RequestNotificationL()
*/
const TUid KUidMmfGlblAudioPresetsChanged = {KUidMmfGlblAudioPresetsChangedDefine};

/**
The value associated with the callback has been changed
@see CMmfGlobalAudioEffect::RequestNotificationL()
*/
const TUid KUidMmfGlblAudioValueChanged = {KUidMmfGlblAudioValueChangedDefine};


/**
Parent class for global audio effects
All global effect class derive from this class. It should be seen as an abstract class providing
common facilities, that they all share. Users of these classes should be aware that there could
be multiple instances, at any one go - e.g. in separate processes. All instances should use
event notification (see RequestNotificationL()) to observe changes in values, in case another
instance has changed things.
If there is more than one output device on a phone, then the behaviour is system dependent,
but typical behaviour would be for the settings etc only to apply to the current device - 
switching output device would lead to a direct change in the settings reported.
*/
class CMmfGlobalAudioEffect : public CBase
	{
public:
	/**
	Destructor. 
	*/
	IMPORT_C ~CMmfGlobalAudioEffect();

	/**
	Flags denoting capability of specific global effect.
	These flags allow the client app to equire about the capabilities of the given effect.
	It should be noted that these refer to the specific implementation, and not 
	necessarily the effect class
	*/
	enum TCapabilityFlags
		{
		ECapabilitySupportsInvidividualValues	=0x0001, 	//*< Supports ExtractValuesL() and SetByValuesL()
		ECapabilitySupportsSettingByUid			=0x0002, 	//*< Supports current setting to be requested by Uid
		ECapabilitySupportsSetSettingByUid		=0x0004,	//*< Supports current value to be changed by Uid
		ECapabilitySupportsSettingByDes			=0x0008,	//*< Supports current setting to be required by Descriptor
		ECapabilitySupportsSetSettingByDes		=0x0010,	//*< Supports current value to be changed by Descriptor
		ECapabilitySupportsActiveChangedCallback=0x0020,	//*< Supports KUidMmfGlblAudioActiveChanged callbacks
		ECapabilitySupportsEnableChangedCallback=0x0040,	//*< Supports KUidMmfGlblAudioEnableChanged callbacks
		ECapabilitySupportsPresetChangedCallback=0x0080,	//*< Supports KUidMmfGlblAudioPresetsChanged callbacks
		};
		
	/**
	Indication of how to interpret volume settings.
	*/
	enum TVolumeValue
		{
		EVolValueAbsolute, 	//*< Interpret volume and similar values as some form of absolute scale
		EVolValuedB			//*< Interpret volume and similar values as dB
		};
		
	/**
	Request capabilities of this object.
	Return the capabilities of the particular GlobalAudioEffect. If aCurrentOnly is false, then
	it will return all the possible features of the implementation, whereas if it is true only
	the features available at that moment are indicated. For example, an implementation may
	support returning the value by UID, but if it was set by UID. In such a circumstance, the
	bit will always be true if aCurrentOnly is false, but if aCurrentOnly is true and the
	value had been (say) set by descriptor, then that bit will be false.
	@param aCurrentOnly
	       If true, capabilities are for current situation only - see text
	@return Capability settings, as given in TCapabilityFlags
	*/	
	IMPORT_C TUint Capability(TBool aCurrentOnly);
	
	/**
	Request to be told about particular events occuring
	This is a generic facility for providing callbacks as particular events occur. All callbacks
	are sent to the Observer that was passed on object creation, invoking GAEEventNotificationL(). 
	The following Uids are defined:
	
	KUidMmfGlblAudioEnableChanged - Indicates the value of IsEnabled() has changed.
	KUidMmfGlblAudioActiveChanged - Indicates the value of IsActive() has changed.
	KUidMmfGlblAudioPresetsChanged - Indicates the list of known presets etc has been modified.
	KUidMmfGlblAudioValueChanged - Indicates the settings have been changed
	
	With all these settings, the value of aParam returned to GAEEventNotificationL() will be empty.
	
	@param aEventUid
	       Uid specifying event for which notification is requested
	
	@leave KErrNotSupported
	       If Observer passed during construction was NULL.
	       The Uid is not recognised.
	       The feature is simply not supported in this implementation.
	@see MMmfGlobalAudioEffectObserver::GAEEventNotificationL()
	*/
	IMPORT_C void RequestNotificationL(TUid aEventUid);
	
	/**
	The particular effect is enabled
	@return True if SetEnabledL(ETrue), or similar, has been called on this effect
	@see SetEnabledL()
	*/
	IMPORT_C TBool IsEnabled() const;
	
	/**
	The particular effect is active
	Active is defined as being that the effect is in use - that is audio data is being played
	through it
	@return True if the effect is in use
	*/
	IMPORT_C TBool IsActive() const;
	
	/**
	Enable (or disable) this effect
	If not enabled, the Effect will not change the data being passed through. However, the
	values associated with the effect. Where audio data is being played, the effect will
	take place immediately. 
	@param aValue
	       If true, enables this specific effect. If false, disables it. 
	*/
	IMPORT_C void SetEnabledL(TBool aValue);
	
	/**
	Obtain uid-based current settings
	If the value was set using SetSettingsByUidL() then this returns that setting. Note: can use 
	Capability(ETrue) to avoid unnecessarily leave.
	@return The Uid used by SetSettingsByUidL()
	@leave KErrNotSupported
	       The settings cannot be expressed as a Uid (usually means SetSettingsByUidL() was not the last
	       thing to change them).
	*/
	IMPORT_C TUid SettingsByUidL() const;
	
	/**
	Change settings by passing Uid
	Change the settings using a uid, that represents a preset. The uid could be known, but
	should preferably be obtained by using RequestPresets()
	@param aPresetUid
	       Uid representing the preset in question
	@leave KErrNotSupported
	       This implementation does not support presets for this effect
	@leave KErrUnknown
	       The value of aUid does not correspond to a known preset
	@see RequestPresets()
	*/
	IMPORT_C void SetSettingsByUidL(TUid aPresetUid);
	
	/**
	Obtain current settings by descriptor
	Return an HBufC8* representing the current settings. The format of this descriptor
	is not specified, but in any given implementation it must be usable by SetSettingsByDesL().
	Ownership of the descriptor is passed back to the calling code.
	@return HBufC8 containing current settings
	@leave KErrNotSupported
	       This implementation does not support expressing settings in descriptor form
	@see SetSettingsByDesL()
	*/
	IMPORT_C HBufC8* SettingsByDesL() const;
	
	/**
	Change settings by descriptor
	Update the current settings from values stored in a descriptor. The value in the 
	descriptor will typically have been created using SettingsByDesL().
	@param aParam
	       Descriptor value to use
	@leave KErrNotSupported
	       This implementation does not support expressing settings in descriptor form
	@leave KErrCorrupt
	       Value in descriptor does not correspond to known format	  
	*/
	IMPORT_C void SetSettingsByDesL(const TDesC8& aParam);
	
	/**
	Return info about known presets by Uid
	This is used in connection with SetSettingsByUidL(). It allows the user to select a preset by
	name but this to be fed back as a Uid. It should also allow a Uid to be translated back to
	a name, if required. Ownership of the list itself remains within the effect implementation.
	The list may be changed on each call to KnownPresetsL() - users should be careful about 
	caching the returned pointer.
	@return List of known preset names and uids
	@see SetSettingsByUidL()
	@leave KErrNotSupported
	       This feature may not be supported in some circumstances
	*/
	IMPORT_C MMmfGlobalAudioPresetList* KnownPresetsL();
	
	/**
	Extract the settings into a struct
	This extracts the current settings, and other data such as min/max values supported. 
	This is intended to support direct selecting of values via graphical controls.
	Unlike SettingsByDesL(), the parameter is expected to be a package buffer wrapping a
	known class/struct, which will be defined by the particular derivation of this class.
	@param aPackageBuf
	       This should be a package buffer wrapping the appropriate class/struct, and will be 
	       specific to a particular CMmfGlobalAudioEffect derivitive.
	@leave KErrNotSupported
	       This will only be supported by some child classes, and even then will not
	       be supported by all implementations.
	*/
	IMPORT_C void ExtractValuesL(TDes8& aPackageBuf);
	
	/**
	Sets current settings from appropriate struct.
	This takes the same struct as used for ExtractValuesL() and changes the current settings.
	Typically it will be used at the end of a dialog to update the data.
	@param aPackageBuf
	       This should be a package buffer wrapping the appropriate class/struct, and will be 
	       specific to a particular CMmfGlobalAudioEffect derivitive.
	@leave KErrNotSupported
	       This will only be supported by some child classes, and even then will not
	       be supported by all implementations.
	@leave KErrArgument
		   Passed package buffer is not the expected size, or individual values are out of range
	*/
	IMPORT_C void SetByValuesL(const TDesC8& aPackageBuf);
	
protected:
	IMPORT_C CMmfGlobalAudioEffect();
	
	/**
	Should be called in the ConstructL() of any derived type
	@param aImplementationUid
	       Fixed for a given derived class, it is used to find the correct plugin or similar
	       that implements that class.
	@param aObserver
	       Observer class. Could be NULL, in which case RequestNotificationL() would not be supported
	@leave KErrNotSupported
	       Cannot find implementation for given derived class
	*/
	IMPORT_C void BaseConstructL(TUid aImplementationUid, MMmfGlobalAudioEffectObserver* aObserver);
	
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
	IMPORT_C TInt CreateCustomInterface(TUid aInterfaceUid);
	
	
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
	IMPORT_C TAny* CustomInterface(TUid aInterfaceUid);
	
private:
	MMmfGlobalAudioImpl* iBaseImplementation; //*< Implementation Object
	};
	

/**
List of preset names against their Uid
Array of preset name and uid pairs. Returned by CMmfGlobalAudioEffect::KnownPresetsL()

@see CMmfGlobalAudioEffect::KnownPresetsL()
*/
class MMmfGlobalAudioPresetList : public MDesCArray
	{
public:
	/**
	Return Uid corresponding to the name of the same index - the name being returned by
	McaPoint()
	@param aIndex
	       Select element from list
	@return Uid of that element
	*/
	virtual TUid GAPUidPoint(TInt aIndex) const=0;
	};



#endif // MMFGLBLAUDIOEFFECT_H

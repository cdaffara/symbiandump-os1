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

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef _DISPLAYCONFIGURATION__INCLUDED_
#define _DISPLAYCONFIGURATION__INCLUDED_

#include <babitflags.h> 
#include <gdi.h>


/**
 * Base class for display configuration hierarchy. Defines common facilities for
 * all configurations (size, mask, accessibility).
 */
class TDisplayConfigurationBase
	{
public:
	/**
	 * Enumeration of all of the display configuration attributes. Used in combination
	 * when calling ClearSettings() and SettingsDefined(). If attributes are added,
	 * corresponding enumeration bits must also be added.
	 */
	enum TAttribute
		{
		EResolution = 0,
		ERotation,
		EResolutionTwips,
		
		EAttributeMax	//this should safely work with value 32.
		};
	/**
	 * 	Enumeration of the panics that may be raised when filling the display configuration
	 * 	These are all contractual input checks and should be avoidable by correctly written callers.
	 * 	
	 **/
	enum	TPanics
		{
			EPanicNegResolution=1,		//<Input resolution size has negative ordinate
			EPanicSemiZeroResolution,	//<Input resolution size has only 1 zero ordinate
			EPanicNonOpaqueBackground,	//<Input background color is not opaque
			EPanicIllegalRotation,		//<Input rotation value is out of range
			EPanicNegTwips,				//<Input twips size has negative ordinate 
			EPanicSemiZeroTwips,		//<Input twips size has only 1 zero ordinate 
			EPanicConfigInvalid			//<Input config version is invalid
		};
public:
	/**
	 * Reads the version field.
	 * 
	 * The version is set automatically in the constructor, or by the GetDisplayConfiguration() 
	 * method to indicate the sender's supported structure version. The version reported after 
	 * GetDisplayConfiguration() will match the version of one of the hierarchy of TDisplayConfiguration 
	 * classes documented in the header, and will never exceed the constructed version.
	 * 
	 * @return The defined version of this display configuration instance, in bytes.
	 */
	inline TInt Version() const;
	/**
	 * Marks the given attribute as undefined.
	 * 
	 * @param aAttribute The attribute to set as undefined.
	 */
	inline void Clear(TAttribute aAttribute);
	/**
	 * Marks all attributes as undefined.
	 */
	inline void ClearAll();
	/**
	 * Check if a particular member is defined.
	 * 
	 * @param aAttribute The attribute to check.
	 * @return ETrue if the attribute is defined, EFalse otherwise.
	 */
	inline TBool IsDefined(TAttribute aAttribute) const;
protected:
	/**
	 * Constructs a TDisplayConfigurationBase, setting the version to aVersion and all 
	 * attributes undefined.
	 * 
	 * Generally used internally in the constructor chain.
	 * 
	 * @param aVersion The defined size of this object.
	 */
	inline TDisplayConfigurationBase(TInt aVersion);
	/** 
	 * Internal function for determining whether a given field name is accessible for an 
	 * object of the current defined size. This must be used before trying to access any field.
	 * 
	 * @param aMember The name of the field to check.
	 * @return ETrue if the field is accessible, EFalse if not.
	 */
	template <class TMember> TBool MemberAccessible(const TMember& aMember) const
		{
		return iVersion>=sizeof(TMember)+TInt(&aMember)-TInt(this);
		}
	inline void Panic(TInt aPanicNo);	//Should be a TPanic
	/**
	 * Compares two TDisplayConfigurationBase instances.
	 * 
	 * Generally used internally in the comparison chain. The two objects are equivalent if 
	 * they have the same version and the same set of attributes defined.
	 * 
	 * @param aRhs The object to compare with this object
	 * @return ETrue if the objects are equivalent, EFalse if not.
	 */
	inline TBool operator == (const TDisplayConfigurationBase& aRhs) const;
private:
	//Intentionally blocked functionality
	inline TDisplayConfigurationBase();
	inline TDisplayConfigurationBase(const TDisplayConfigurationBase& aDisplayConfigurationBase);
	inline TDisplayConfigurationBase operator=(const TDisplayConfigurationBase& aRhs) const;

protected:
	/**
	 * Size to treat this object in bytes. Used to provide backward and forward
	 * compatibility.
	 */
	TInt iVersion;
	/**
	 * Indicates which fields in the configuration have defined values. If a field
	 * contains an undefined value, it must be ignored.
	 * See TAttribute for possible bit values.
	 */
	TBitFlags32 iDefined;
	};

/**
 * First collection of display configuration settings.
 * 
 */
class TDisplayConfiguration1 : public TDisplayConfigurationBase
	{
public:
	/** Defines possible rotation values. */
	enum TRotation
		{
		/** Normal orientation is supported. */
		ERotationNormal,
		/** A 90 degree rotation is supported. */
		ERotation90CW,
		/** A 180 degree rotation is supported. */
		ERotation180,
		/** A 270 degree rotation is supported. */
		ERotation270CW,
		/** Illegal rotation value. */
		ERotationIllegal
		};
public:
	/**
	 * Construct a TDisplayConfiguration1 version object with all attributes undefined.
	 * 
	 * This can be used to create a specific version of configuration:
	 * 	TDisplayConfiguration config(TDisplayConfiguration1.Version());
	 */
	inline TDisplayConfiguration1();
	/**
	 * Defines the resolution for the display, in pixels.
	 * 
	 * This always corresponds to a rotation of ERotationNormal. If a display can be 
	 * disabled, (0,0) can be used to do so. A display can be disabled if the 
	 * resolution list includes (0,0).
	 * 
	 * @param aSize The resolution in pixels.
	 */
	inline void SetResolution(const TSize& aSize);
	/**
	 * Retrieves the resolution, if defined.
	 * 
	 * If the resolution is undefined, the parameter is left unchanged. A resolution 
	 * of (0,0) means the display is disabled.
	 * 
	 * @see IsDefined
	 * @see TAttribute::EResolution
	 * 
	 * @param aSize Receives the resolution.
	 */
	inline TBool GetResolution(TSize& aSize) const;
	/**
	 * Defines the rotation for the display.
	 * 
	 * The values that can be passed in here correspond directly to UI rotation values that can be 
	 * passed in to CWsScreenDevice::SetScreenSizeAndRotation(), CWsScreenDevice::SetCurrentRotations() 
	 * and so on, although the type is defined locally to avoid undesirable interdependencies in 
	 * the interface. Variables of the two types may be freely cast to the other type.
	 * 
	 * @see CFbsBitGc::TGraphicsOrientation
	 * @see CWsScreenDevice
	 * 
	 * @param aRotation The display rotation.
	 */
	inline void SetRotation(TRotation);
	/**
	 * Retrieves the rotation, if defined.
	 * 
	 * If the rotation is undefined, the parameter is left unchanged.
	 * 
	 * @see IsDefined
	 * @see TAttribute::ERotation
	 * 
	 * @param aSize Receives the rotation
	 * @return ETrue if the rotation is defined, EFalse otherwise.
	 */
	inline TBool GetRotation(TRotation&)const;
	/**
	 * Defines the physical size of the display resolution area, in twips.
	 * 
	 * @param aSizeInTwips Provides the size in twips.
	 */
	inline void SetResolutionTwips(const TSize& aSize);
	/**
	 * Retrieves the physical size of the display resolution area, in twips, if defined.
	 * 
	 * For displays that have a fixed size, or can report their size, the physical 
	 * dimensions corresponding to the current resolution shall be defined. Where the display's 
	 * physical size cannot be determined (such as with composite video output, or a projector) 
	 * an arbitrary size shall be defined that reflects the pixel aspect ratio.
	 * 
	 * If the display is not connected or output is disabled, the physical size shall not 
	 * be defined. If the physical size is undefined, the parameter is left unchanged.
	 * 
	 * @see IsDefined
	 * @see TAttribute::EResolutionTwips
	 * 
	 * @param aSizeInTwips Receives size in twips.
	 * @return ETrue if the size is defined, EFalse if not.
	 */
	inline TBool GetResolutionTwips(TSize& aSize) const;
protected:
	/**
	 * Constructor for passing through a version already supplied.
	 * 
	 * Generally used internally in the constructor chain.
	 * 
	 * @param aVersion - Version of the class already calculated and being passed up 
	 * through the classes.
	 */
	inline TDisplayConfiguration1(TInt aVersion);
	/**
	 * Compares two TDisplayConfiguration1 instances.
	 * 
	 * Generally used internally in the comparison chain. The two objects are equivalent if they have 
	 * the same version, the same set of attributes defined and all the defined attributes are the 
	 * same. Undefined attributes are not compared.
	 * 
	 * @param aRhs The object to compare with this object.
	 * @return ETrue if the objects are equivalent, EFalse if not.
	 */
	inline TBool operator == (const TDisplayConfiguration1& aRhs)const;
private:
	//Intentionally blocked functionality
	inline TDisplayConfiguration1(const TDisplayConfiguration1& aDisplayConfiguration1);
	inline TDisplayConfiguration1 operator=(const TDisplayConfiguration1& aRhs) const;

private:
	//Streamable data members
	TSize 	iResolution;
	TInt	iRotation;	//0,1,2,3
	TSize	iTwipsSize;
	};

/**
 * The display configuration class for general use.
 * 
 * May be extended by adding a chain of classes between this and TDisplayConfiguration1.
 * If so, also update the typedef TDisplayConfigurationTop.
 * 
 */
class TDisplayConfiguration : public TDisplayConfiguration1
	{
private:	
	typedef TDisplayConfiguration1	TDisplayConfigurationTop;
public:
	/**
	 * Construct a configuration of the default version with all attributes undefined.
	 */
	inline TDisplayConfiguration():	TDisplayConfigurationTop()
		{}
	/**
	 * Copy constructor. This constructor will read the version field of the source and target 
	 * objects and only copy the common set of fields. Any remaining fields in the target 
	 * object will be set as undefined.
	 * 
	 * @param aDisplayConfiguration Configuration to be copied.
	 */
	inline TDisplayConfiguration(const TDisplayConfiguration& aDisplayConfiguration);
	/**
	 * Constructs a display configuration for a particular version.
	 * 
	 * This constructor is designed to be used with the value returned by 
	 * MDisplayControlBase::PreferredDisplayConfigurationVersion(), and initializes the members 
	 * to represent an object compatible with that.
	 * 
	 * The version used is the earlier of aVersion and the compile time version 
	 * of TDisplayConfiguration.
	 * 
	 * @param aVersion Caller-defined maximum version of configuration.
	 */
	inline TDisplayConfiguration(TInt aVersion);
	/**
	 * Compares two TDisplayConfiguration instances.
	 * 
	 * The two objects are equivalent if they have the same version, the same set of attributes defined
	 * and all the defined attributes are the same. Undefined attributes are not compared.
	 * 
	 * @param aRhs The object to compare with this object.
	 * @return ETrue if the objects are equivalent, EFalse if not.
	 */
	inline TBool operator == (const TDisplayConfiguration& aRhs)const
		{
		return TDisplayConfiguration1::operator==(aRhs);
		}
private:
	//Intentionally blocked functionality
	inline TDisplayConfiguration operator=(const TDisplayConfiguration& aRhs) const;
	};

#include <graphics/displayconfiguration.inl>
#endif // _DISPLAYCONFIGURATION__INCLUDED_

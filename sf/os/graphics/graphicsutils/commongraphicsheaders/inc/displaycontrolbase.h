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

#ifndef DISPLAYCONTROLBASE__INCLUDED_
#define DISPLAYCONTROLBASE__INCLUDED_
#include <graphics/displayconfiguration.h>
/** Common base interface for display control facilities.

*/
class MDisplayControlBase
	{
public:
	enum	
		{
		KUidDisplayControlBase =  0x10286496,
		ETypeId=	 KUidDisplayControlBase 
		};
	
	class TResolution
		{	//Kernel mode makes use of agregate constructors
	public:
		enum
			{
			ERotationNormalSupported = 0,
			ERotation90Supported = 1,
			ERotation180Supported = 2,
			ERotation270Supported = 3,
			EIsVirtual = 4,
			};
		inline TResolution(TSize aPixelSize, TSize aTwipsSize, TBitFlags32 aFlags = ERotationNormalSupported);
//		inline TResolution(const TResolution& aSrc);

	public:
		/** The physical display size in pixels with no rotation (i.e. ERotationNormal).
		    See SetConfiguration().
		*/
		TSize iPixelSize;
		/** The physical display size in twips with no rotation (i.e. ERotationNormal).
		    See SetConfiguration().
		*/
		TSize iTwipsSize;
		/** A bitwise combination of TDisplayConfiguration1::TGraphicsOrientation values.
		*/
		TBitFlags32 iFlags;

	private:
		/** Reserved for extension and alignment.
		*/
		TUint32 iReservedAlignmentSpare_0;
		};
public:
	/** Returns the number of discrete resolutions that can be retrieved using
		GetResolutions().
		
		If the display is disconnected, the return value shall be KErrDisconnected. Implicitly, 
		there are no resolutions available. When initially connected, the number of resolutions 
		may be limited, possibly zero, only increasing when the display has reported 
		which resolutions it supports.
		
		@return Number of resolutions available, or KErrDisconnected.
	*/
	virtual TInt NumberOfResolutions()const =0;
	/** Retrieves the set of available display resolutions and supported rotations per resolution.
		
		If a display can be disabled, the list shall include a resolution with the size (0,0). If the 
		display is connected but disabled, the list may also include other supported resolutions. 
		If a display cannot be disabled, the list shall not include (0,0).

		The flags field in each entry shall contain the set of rotations supported for the given 
		resolution. The size fields shall always give the resolution with no rotation applied (ERotationNormal), 
		whether or not that rotation is supported.

		If the EIsVirtual flags is set, the resolution will be simulated by scaling the output. Scaling 
		quality varies between platfoms, and in general the best quality will be achieved using a 
		physical resolution, where the EIsVirtual flag is not set.

		The size in twips may be an estimate for some displays, but can be used to determine the pixel aspect 
		ratio in combination with the size in pixels.

		The array shall be reset and the contents replaced by the available resolutions. If an error is returned, 
		the contents of the array shall be undefined.

		If no display is connected, the return value shall be KErrDisconnected. When initially connected, the 
		set of resolutions available may be limited, possibly zero, increasing when the display has reported 
		which resolutions it supports.

		If the array is not large enough to retrieve the set of resolutions initially, it will be expanded once, 
		or fail with KErrNoMemory. If the number of resolutions increases again after this expansion, the 
		function will fail with KErrOverflow. The caller may re-try the operation.

		@see TResolutionFlags
		
		@param aResolutions Buffer to receive resolutions.
		
		@return KErrNone on success, KErrDisconnected if no display is connected, KErrNoMemory if a memory allocation 
		failure occurs or KErrOverflow if the number of elements increased.
	*/	
	virtual TInt GetResolutions(RArray<TResolution>& aResolutions)const =0;
	/** Gets the current display configuration.

		Note that some entries in the configuration may be undefined if they are unknown, so their validity should 
		either be checked using the accessor return code, or using the IsDefined() function before reading them.

		If the display is not connected, the resolution shall be set as undefined. If the display is connected, but 
		disabled, the resolution shall be (0,0). Other attributes may or may not be defined in these states.

		@see TDisplayConfiguration::IsDefined
		@see SetConfiguration

		@param aConfig Receives the display configuration.
	*/	
	virtual void GetConfiguration(TDisplayConfiguration& aConfig)const =0;
	/** Sets the new display configuration.

		It is valid to leave settings undefined to let the implementer determine the optimal combination. If a defined 
		setting cannot be achieved, the function shall fail with KErrArgument.
	
		Note that the new configuration will be in use when the function returns, but the change may not appear on screen 
		immediately. It may be deferred due to rendering, composition and display synchronization taking place, but will 
		generally become apparent in the next frame update.
	
		Whether a given configuration is valid or not shall be platform dependent, and may depend on the current screen 
		size mode. Some platforms may restrict resolutions to those listed in the resolution list, while others may be 
		more flexible. Where the resolution does not match a physical resolution from the list, display quality may be
		reduced.
	
		Setting a resolution of (0,0), if allowed, shall disable display output. Otherwise the pixel resolution shall 
		define the maximum number of pixels that can be rendered using CWindowGc, and this area shall fill the display. 
		Alternative means of rendering may be displayed at the physical resolution, irrespective of the current 
		configuration.
	
		@see CWsScreenDevice::SetScreenMode
		@see CWindowGc

		@param aConfig Display configuration to set, and receive the complete configuration that will be used.

		@capability WriteDeviceData

		@return A system-wide error code.
	*/	
	virtual TInt SetConfiguration(const TDisplayConfiguration& aConfig) =0;
	/** Returns the preferred version of the TDisplayConfiguration object.

		GetConfiguration() and SetConfiguration() may be passed any version of the TDisplayConfiguration class, as returned 
		by TDisplayConfiguration::Version(). If an earlier version is presented, the implementation will treat later 
		attributes as undefined. If a later structure is presented, then the additional attributes will be ignored.

		This method may be used to fine-tune use of the interface, by avoiding the generation of attribute data that will not 
		be used, or by reducing the options presented to users.

		@return The optimum version of TDisplayConfiguration to pass to SetConfiguration() and GetConfiguration().
	*/	
	virtual TInt PreferredDisplayVersion()const =0;
	};

inline MDisplayControlBase::TResolution::TResolution(TSize aPixelSize, TSize aTwipsSize, TBitFlags32 aFlags)
	:	iPixelSize(aPixelSize),iTwipsSize(aTwipsSize),iFlags(aFlags),iReservedAlignmentSpare_0(0)
	{	}
//inline MDisplayControlBase::TResolution::TResolution(const TResolution& aSrc)
//:	iSize(aSrc.iSize),iRotations(aSrc.iRotations),iReservedAlignmentSpare_0(0)
//	{	}

#endif // DISPLAYCONTROLBASE__INCLUDED_

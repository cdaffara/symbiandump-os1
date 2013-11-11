// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
// Display Update Abstract Interface
// 
//

#ifndef OPENWFC_RI_DISPLAY_UPDATE_H
#define OPENWFC_RI_DISPLAY_UPDATE_H

#include <e32base.h>
#include <graphics/symbianstream.h>

/**
 * \brief   Abstract interface used by OPENWF-C SI in order to display
 * the composed content
 *
 */

class MOpenWFC_RI_Display_Update
    {
public:
    /**
     * Retrieves one of the display attributes
     *
     * @param   aAttributeId   The attribute ID to be retrieved.
     * @param   aAttribute   Pointer to the attribute to be retrieved
     * @param   aAttributeSize   Size in bytes of attribute to be retrieved
     *
     * @return  KErrNone or a system wide error
     */
    virtual TInt GetAttribute(TInt aAttributeId, TAny* aAttribute, TInt aAttributeSize) = 0;
    
    /**
     * Sets one of the display attributes
     *
     * @param   aAttributeId   The attribute ID to be retrieved.
     * @param   aAttribute   Pointer to the attribute to be retrieved
     * @param   aAttributeSize   Size in bytes of attribute to be retrieved
     *
     * @return  KErrNone or a system wide error
     */
    virtual TInt SetAttribute(TInt aAttributeId, TAny* aAttribute, TInt aAttributeSize) = 0;
    
    /**
     * Commits the display attribute.
     *
     * @return  KErrNone or a system wide error
     */
    virtual TInt CommitAttribute() = 0;
    
    /**
     * Associates a stream with the topmost display layer (overlay).
     * Must have been called successfully before calling UpdateDisplay.
     * Also used to specify the composed output stream as the display layer.
     * Note that currently this method is called each composition, 
     * but it is only necessary to call it when the top stream changes.
     * 
     * This method may fail if any attributes of the stream cannot be handled by the display device.
     * The aNonTrivialAttributes is a zero-terminated list of attribute keys and values 
     * that do not match the trivial cases for direct composition, but that some devices may support,
     * representing eg mirror, scale, rotation, offset, or oversize cases. 
     * The implementation of this method will stop processing the list and fail if it does not recogise
     * any of the keys or cannot accept the value associated with that key for direct display.
     * All platforms should accept an empty or NULL list, but some may simply fail if the list is non-empty, 
     * so the caller should keep the list brief.
     *  
     * If the method fails, then the caller must compose the content into a simpler format, 
     * and call SetTopLayerSurface again before calling UpdateDisplay.
     * It is expected that there will be at least 1 pre-determined simpler format or stream handle that will always succeed.  
     * 
     * @param   aStream   The stream Id to be associated with the topmost layer
     * @param   aNonTrivialAttributes   Attribute integer pair list of any non-trivial attribute values. 
     *
     * @return  KErrNone or a system wide error
     */
    virtual TInt SetTopLayerSurface(SymbianStreamType aStream,TInt* aNonTrivialAttributes=NULL) = 0;   

    /**
     * Associates a stream with a specific layer.
     * Must have been called successfully before calling UpdateDisplay.
     * Note that currently this method is called each composition, 
     * but it is only necessary to call it when the stream stack changes,
     * when all layers should be re-specified in turn before the next UpdateDisplay call.
     * 
     * This method may fail if any attributes of the stream cannot be handled by the display device.
     * The aNonTrivialAttributes is a zero-terminated list of attribute keys and values 
     * that do not match the trivial cases for direct composition, but that some devices may support,
     * representing eg mirror, scale, rotation, offset, undersize or oversize cases,
     * and could be used to represent multiple layers. 
     * The implementation of this method will fail if it does not recogise any of the keys 
     * or cannot accept the value associated with that key for direct display.
     * All platforms should accept an empty or NULL list, but some may simply fail if the list is non-empty, 
     * so the caller should keep the list brief.
     * 
     * If the method fails, then the caller must compose the content into a simpler format,
     * and call SetTopLayerSurface (or SetLayerSurface) again before calling UpdateDisplay.
     * 
     * @param   aLayer   The layer id. The layer number "0" is the topmost layer.
     * @param   aStream  The stream Id to be associated with the specified layer, or SYMBIAN_INVALID_HANDLE.
     * @param   aNonTrivialAttributes   Attribute integer pair list of any non-trivial attribute values. 
     *
     * @return  KErrNone or a system wide error
     */
    virtual TInt SetLayerSurface(TInt aLayer, SymbianStreamType aStream,TInt* aNonTrivialAttributes=NULL) = 0;
    
    /**
     * Update (flush/post) the screen
     * 
     * @return  KErrNone or a system wide error
     */
    virtual TInt UpdateDisplay() = 0;
    };
#endif // OPENWFC_RI_DISPLAY_UPDATE_H

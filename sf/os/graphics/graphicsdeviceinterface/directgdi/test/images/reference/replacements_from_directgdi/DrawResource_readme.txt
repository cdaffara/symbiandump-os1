------------------------------------------------------------------------------------------

Notes about the origin of the replacement reference images used for the DrawResource Tests

31/03/2008

------------------------------------------------------------------------------------------


The reference images for the some of the BitGdi versions of the DrawResource tests contain 
2 known defects so they cannot be used as reference images. Instead the DirectGdi SW 
versions of the images have been copied into this folder so they can be compared against 
the DirectGdi HW versions. 

The defects occur when we generate BitGdi versions of the DrawResource tests by creating
a bitmap, then rotating this bitmap 90, 180 and 270 degrees in order to be able to draw
a rotated version of the bitmap using BitGdi. The CBitmapRotator class is used to do the
bitmap rotation, but 2 errors have been seen with the rotated versions:
- when rotating a bitmap that is in EColor16MAP format, the alpha information is lost. (See
defect DEF119752).
- when rotating a bitmap 180 degrees, a banding effect can be seen appearing in the center
of the rotated bitmap. (See defect DEF120342).

There are 85 DrawResource tests, all marked "TestDrawResource". For 51 of these the 
reference images have been copied from SW version, these are listed below. When the above 
defects are fixed, these replacement reference images should be deleted, and the 
reference images created with BitGdi should be be used again. When this happens the 
correct BitGdi reference images should also be added to perforce in the images/reference 
folder.

Replacements from SW DirectGdi:
BitGdi_DrawResource_DestRectClip_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectSrcRectClip_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectSrcRectWriteAlpha_TGT_ARGB_8888_PRE_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRectSrcRectWriteAlpha_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectSrcRectWriteAlpha_TGT_RGB_565_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRectSrcRectWriteAlpha_TGT_RGB_565_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectSrcRectWriteAlpha_TGT_XRGB_8888_PRE_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRectSrcRectWriteAlpha_TGT_XRGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_ARGB_8888_PRE_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_ARGB_8888_PRE_SRC_RGB_565.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_ARGB_8888_PRE_SRC_XRGB_8888.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_RGB_565_SRC_RGB_565.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_RGB_565_SRC_XRGB_8888.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_XRGB_8888_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_XRGB_8888_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_XRGB_8888_SRC_RGB_565.mbm
BitGdi_DrawResource_DestRectSrcRect_TGT_XRGB_8888_SRC_XRGB_8888.mbm
BitGdi_DrawResource_DestRectWriteAlpha_TGT_ARGB_8888_PRE_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRectWriteAlpha_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectWriteAlpha_TGT_RGB_565_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRectWriteAlpha_TGT_RGB_565_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRectWriteAlpha_TGT_XRGB_8888_PRE_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRectWriteAlpha_TGT_XRGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRect_TGT_ARGB_8888_PRE_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRect_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRect_TGT_ARGB_8888_PRE_SRC_RGB_565.mbm
BitGdi_DrawResource_DestRect_TGT_ARGB_8888_PRE_SRC_XRGB_8888.mbm
BitGdi_DrawResource_DestRect_TGT_RGB_565_SRC_RGB_565.mbm
BitGdi_DrawResource_DestRect_TGT_RGB_565_SRC_XRGB_8888.mbm
BitGdi_DrawResource_DestRect_TGT_XRGB_8888_SRC_ARGB_8888.mbm
BitGdi_DrawResource_DestRect_TGT_XRGB_8888_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_DestRect_TGT_XRGB_8888_SRC_RGB_565.mbm
BitGdi_DrawResource_DestRect_TGT_XRGB_8888_SRC_XRGB_8888.mbm
BitGdi_DrawResource_NonZeroOrigin0_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_NonZeroOrigin1_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_NonZeroOrigin2_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_NonZeroOrigin3_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_NonZeroOrigin4_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_NonZeroOrigin5_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_PosClip_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_PosWriteAlpha_TGT_ARGB_8888_PRE_SRC_ARGB_8888.mbm
BitGdi_DrawResource_PosWriteAlpha_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_PosWriteAlpha_TGT_RGB_565_SRC_ARGB_8888.mbm
BitGdi_DrawResource_PosWriteAlpha_TGT_RGB_565_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_PosWriteAlpha_TGT_XRGB_8888_SRC_ARGB_8888.mbm
BitGdi_DrawResource_PosWriteAlpha_TGT_XRGB_8888_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_Pos_TGT_ARGB_8888_PRE_SRC_ARGB_8888.mbm
BitGdi_DrawResource_Pos_TGT_ARGB_8888_PRE_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_Pos_TGT_ARGB_8888_PRE_SRC_RGB_565.mbm
BitGdi_DrawResource_Pos_TGT_ARGB_8888_PRE_SRC_XRGB_8888.mbm
BitGdi_DrawResource_Pos_TGT_RGB_565_SRC_RGB_565.mbm
BitGdi_DrawResource_Pos_TGT_RGB_565_SRC_XRGB_8888.mbm
BitGdi_DrawResource_Pos_TGT_XRGB_8888_SRC_ARGB_8888.mbm
BitGdi_DrawResource_Pos_TGT_XRGB_8888_SRC_ARGB_8888_PRE.mbm
BitGdi_DrawResource_Pos_TGT_XRGB_8888_SRC_RGB_565.mbm
BitGdi_DrawResource_Pos_TGT_XRGB_8888_SRC_XRGB_8888.mbm

# S60 build level configuration makefile 
 
MAKEFILE = /sf/os/deviceplatformrelease/sf_build/sf_prebuild/makefile
          
# Exporting makefile template
include /sf/os/devicesrv/commonservices/commonengine/makefile
include /sf/os/devicesrv/devicesrv_pub/common_platform_security_definitions_api/makefile
include /sf/os/devicesrv/devicesrv_pub/platform_path_definitions_api/makefile


ifeq (S60,$(BUILD))
include /sf/mw/uiresources/uigraphics/icons/makefile
include /sf/mw/uiresources/uigraphics/bitmaps2/makefile
include /sf/mw/platformtools/gfxconversion/mifconv/makefile_templates/makefile
include /sf/mw/qttranslations/cmaker/makefile
endif
 
# Place the first target as the default target which is executed from this level 
s60_prebuild  :: commonengine_all platform_path_all common_platform_security_all export_template_files icons_all bitmaps_all 
os_prebuild  :: commonengine_all platform_path_all common_platform_security_all  

# include include_template.mk 
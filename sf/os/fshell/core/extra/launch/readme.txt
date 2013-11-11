To update the icon files:

1) Edit the .sgv file useing an editor such as Inkscape. Note, things like text
   must be converted using "Path | Object to path" if they are to survive the next
   step.

2) Convert the .sgv file to a Tiny SGV file using Nokia's SGV2SGVT tool (available in
   the S60 SDK).

3) Convert the Tiny SGV file to a .mif file using "mifconv" as follows:

	\epoc32\tools\mifconv fshell_local.mif /x /c32 fshell_local.tiny.svg

   Note, it is important that the .mif file is not encoded in binary XML (the "/x"
   option arranges for this) because not all S60 handsets can handle the more compact
   format.

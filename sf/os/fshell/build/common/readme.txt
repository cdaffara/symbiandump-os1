A few words about how and why the tool-kit's UIDs are defined here.

The motivation for centrally defining all the tool-kit's UIDs in one place
was to allow them all to be changed into the non-protected range, thereby
allowing the tool-kit to be installed using a self-signed SIS file. Note,
self-signed SIS files can only be used with UIDs in the non-protected range.

Each platform that the tool-kit supports gets to decide which range of UIDs
it will use. The platforms (i.e. sub-directories of \fshell/platforms)
decide which range to use by whether they define FSHELL_PROTECTED_UIDS in their
platform.mmh. This macro is picked up by common.mmh (which is #included by
all .mmp files) and any .cpp or .rss (or similar) files that need it.
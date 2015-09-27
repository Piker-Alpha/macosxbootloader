Introduction
------------

This Github project is a new and updated branch of the [Yosemite tree](https://github.com/Piker-Alpha/macosxbootloader/tree/master)


Goals
-----

This project aims to add support for El Capitan (OS X 10.11) for unsupported Mac hardware with a 32-bit only EFI implementation. Like for example the first and second generation Mac Pro models. Though it should work well with any other model with a 32-bit EFI implementation.


Development
-----------

This project is maintained by [Pike R. Alpha](https://pikeralpha.wordpress.com)


Current State
-------------

The current state of this project is: Phase 1 and 2 Completed (OS X 10.11 El Capitan is now supported).

Note:

A second development phase began on Saturday 26 September 2015 and was completed on Sunday 27 September 2015. A third development phase is also planned, to finish it up. See also our [TODO list](https://github.com/Piker-Alpha/macosxbootloader/blob/El-Capitan/TODO.md).


Download
--------

First. All downloads should be done from the following links. All other sources are unsupported. 

Download your copy of the prebuild (32-bit) version of boot.efi for El Capitan (compiled on Microsoft Windows 10 with Microsoft Visual Studio 2015) with a [black background and white Apple logo](https://raw.githubusercontent.com/Piker-Alpha/macosxbootloader/El-Capitan/Prebuilt/boot.efi) now, and verify the download by entering (either one) in a terminal window:

```
openssl md5  boot.efi = 4db3cf800b8888337a70448c9ed370a4
openssl sha1 boot.efi = 91456d827b1e656ea72ec0d78fea39273ab48dd8
```

Or download the one for legacy hardware, with a [grey background and logo] (https://raw.githubusercontent.com/Piker-Alpha/macosxbootloader/El-Capitan/Prebuilt/boot_grey.efi) for your Mac Pro and verify the download by entering (either one) in a terminal window:

```
openssl md5  boot.efi = 1218d85076b8e2bb9d711f5aaf617a6d
openssl sha1 boot.efi = 4853bf4bed4a9727ab572ebccf1baf0d7eb6c048
```

Note: If the output is different, then your copy of boot.efi should not be used!


Compilation
-----------

Don't want a prebuilt copy of boot.efi then compile the source code yourself, with either Microsoft Visual Studio 2013 or 2015. The latter requires you to update the solution files, but that should be done automatically in the MS Visual Studio IDE.

Note: See also [Compiling-Instructions](https://github.com/Piker-Alpha/macosxbootloader/wiki/Compiling-Instructions)


Thanks To
---------

This project would not have been possible without the help of Peter Holbrook and Mike Boss. Peter did all compiling and Mike ran dozens of test builds that Peter pushed out over at forums.macrumors.com Awesome work guys. Job well done!


Donations
---------

If you like to support my work then you may want to consider to [make a donation](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JXFBXK7J97RAE)


Help
----

If you need help with the setup then please visit [this macrumor thread](http://forums.macrumors.com/threads/2006-2007-mac-pro-1-1-2-1-and-os-x-el-capitan.1890435/)

Note: Sorry. I can't help you. I don't have unsupported hardware with 32-bit EFI.


Bugs
----

Bugs can be reported [here](https://github.com/Piker-Alpha/macosxbootloader/issues)

Note: Please provide a clear step by step way to reproduce the bug.


License
-------

My work is licensed under [the Creative Commons Attribution-NonCommercial 3.0 Unported License](http://creativecommons.org/licenses/by-nc/3.0/) and as such you must add a link to this license. Even if you only use a download link. Also note the term "NonCommercial" because I don't want to see my work end up anywhere else but here.

Note: "Tiamo" released his work under a [BSD-3-Clause license](http://opensource.org/licenses/BSD-3-Clause)


Disclaimer
----------

Copyright (c) 2014-2015, by Pike R. Alpha – All right reserved.

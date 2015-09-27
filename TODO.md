
OS X 10.11 El Capitan TODO list:
--------------------------------

Phase 1: Status - Completed (in 2015)!

```
1.) Load the prelinkedkernel for safe boot mode i.e. when -x is used (status: Done).

2.) Fix IODeviceTree:/chosen/boot-file property value. Examples: (status: Done).
    - "\System\Library\Kernels\kernel" for booting in safe mode.
    - "\com.apple.recovery.boot\prelinkedkernel" for booting from the Recovery HD.
    - "\System\Library\Prelinkedkernels\prelinkedkernel" for regular bootups.

3.) Implement SIP support (status: Done).
    - Previously csrutil disable/enable failed from the Recovery HD.

4.) Fix Beach Ball of Death (BBOD) issue after clearing NVRAM (status: Done).
    - Booting from the Recovery HD is fine.
    - This issue was fixed by setting IODeviceTREE:efi/platform/system-id to the SMBIOS UUID.

5.) Add missing booter info properties, like: "booter-name", "booter-version" and "booter-build-time" (status: Done).
```

Phase 2: Status - Work In Progress

```
1.) Back port Clang changes (status: Done).

2.) Add OS X support for (pre)Mavericks, Yosemite and El Capitan (status: Done).

3.) Mask csr-active-config so that csrutil never reports 0x80.

4.) Include image data for mavericks and older versions of OS X (status: work in progress).
   
5.) Disable debug output.

6.) Select normal boot (no more verbose boots).
```

Phase 3: Status - Work In Progress

```
1.) Fix flush cache i.e. when -f is used.
```

OS X 10.10 Yosemite TODO list:
------------------------------

Phase 1: Status - Completed (in 2014)!

```
1.) None. All issues related to full Yosemite compatibility have been completed.
```

Phase 2: Status - Completed (in 2014)!

```
1.) Fix Recovery HD boot support for Yosemite (status: Done).

2.) Add missing Panic Dialog data for black mode (status: Done).

3.) Convert all Panic Dialog data from LZSS to LZVN (status: Done).
```

Phase 3: Status - TBD

```
1.) Add Xcode compiler support.
    - It compiles without errors/warnings but you cannot boot with it.
```

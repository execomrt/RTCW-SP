# Return to Castle Wolfenstein - Single Player HD Patch

This patch brings enhancements to the **Return to Castle Wolfenstein** single-player experience on modern Windows systems:

## Features

- ✅ Recompiled using **Visual Studio 2022**
- 🖥️ Native support for **Windows 10 / 11 and 4K displays (High DPI aware)**  
  - Fixes rendering issues where the game appears zoomed in on high-DPI monitors
- 🔧 Removed legacy mini OpenGL driver; now uses modern OpenGL with **GLEW**
- 🖼️ Support for **BC7 texture compression** as a modern alternative to S3TC
- 🔍 Enabled **multisampling (anti-aliasing)** via:  
  `seta r_ati_fsaa_samples "8"`
- 🌿 **Alpha-to-coverage** transparency support
- 🔄 Support for **FreeSync / Adaptive VSync** via:  
  `r_swapInterval "0"` (requires `WGL_EXT_swap_control_tear`)

> 🗂️ Use the built executable from  
> `src\win32\Release\WolfSP.exe`  
> in your Steam install at:  
> `steamapps\common\Return to Castle Wolfenstein`

---

## License

This project is released under the terms of the **GNU General Public License (GPL)**.  
See [`COPYING.txt`](COPYING.txt) for full details.

### Additional Terms

The Return to Castle Wolfenstein single-player GPL source code is subject to additional licensing terms. You should have received a copy of these terms immediately following the GPL in the original RTCW source code release.

If not, you may request a copy in writing from:

> id Software LLC  
> c/o ZeniMax Media Inc.  
> Suite 120  
> Rockville, Maryland 20850 USA

---

## Excluded Code & Third-party Licenses

Some components included in this patch are **excluded from the GPL** and fall under their own licenses:

---

### 🔹 Zip File I/O – zlib

**Files:**
- `src/qcommon/unzip.c` (4301 lines)  
- Uses parts of **zlib**

**Authors:**
- Gilles Vollant (unzip.c)
- Jean-loup Gailly & Mark Adler (zlib)

**License:**  
Free for any use, including commercial. See original comments in the source.

---

### 🔹 MD4 Message Digest

**File:**
- `src/qcommon/md4.c` (289 lines)

**Author:**  
- RSA Data Security, Inc.

**License:**  
Freely usable with attribution and acknowledgment in materials. No warranty.

---

### 🔹 JPEG Library

**Directory:**
- `src/jpeg-6`

**Author:**  
- Thomas G. Lane

**License Highlights:**
- Free to use, modify, and redistribute
- Executables must acknowledge:  
  “This software is based in part on the work of the Independent JPEG Group.”
- No liability is accepted by the authors

> ℹ️ Note: The original README from this version was lost. A copy from release 6b is included. The library contains minimal glue code for integration.

---

## Contributions & Feedback

Pull requests, bug reports, and suggestions are welcome!

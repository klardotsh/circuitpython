/*
 * This file is part of the CircuitPython project
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Josh Klar (https://github.com/klardotsh)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma GCC diagnostic ignored "-Wcast-align"

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "py/binary.h"
#include "extmod/modgzip.h"
#include "extmod/moduzlib.h"

#define MAGIC 0x1F8B
#define DEFLATED 0x08
#define FTEXT 0x01
#define FMULTI 0x02
#define FEXTRA 0x04
#define FNAME 0x08
#define FCOMMENT 0x10

#define DEBUG_ENABLE 0

#if DEBUG_ENABLE // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

mp_obj_t mod_gzip_decompress(size_t n_args, const mp_obj_t *args) {
    mp_obj_t data = args[0];
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    // Read, and then promptly discard, the GZIP header. This is mostly a
    // sanity-checking step, consistent with CPython's lack of exposing this
    // information anywhere (which feels like an odd oversight, and some third
    // parties seem to agree: the pure-Python gzip implementation "gzippy"
    // explicitly has a Header class which can be read from)

    byte *p = bufinfo.buf;

    {
        uint magic = *p++ << 8;
        magic |= *p++;

        if (magic != MAGIC) {
            mp_raise_ValueError(translate("not a gzip file"));
        }

        if (*p++ != DEFLATED) {
            mp_raise_ValueError(translate("unknown compression method (only DEFLATE supported)"));
        }

        bufinfo.len -= 3;
    }

    uint flags = *p++;
    // we do nothing with FTEXT and frankly I have no idea what we even would do with it
    // suppress the unused-variable error by never bothering to define it
    #if DEBUG_ENABLE
    bool has_text = flags & FTEXT;
    #endif
    bool has_multi = flags & FMULTI;
    bool has_extra = flags & FEXTRA;
    bool has_name = flags & FNAME;
    bool has_comment = flags & FCOMMENT;

    DEBUG_printf(
        "flags: " UINT_FMT " has_text=" UINT_FMT " has_multi=" UINT_FMT " has_extra=" UINT_FMT " has_name=" UINT_FMT " has_comment=" UINT_FMT "\n",
        flags, has_text, has_multi, has_extra, has_name, has_comment
    );

    // skip a bunch of other stuff we're never going to use:
    // mtime (4 bytes)
    // flags for "extra" (1 byte)
    // os flags (1 byte)

    p += 6;
    bufinfo.len -= 7;

    if (has_multi) {
        mp_raise_ValueError(translate("multipart gzip files not supported"));
    }

    if (has_extra) {
        // we explicitly do not currently do anything with "extra" data, just skip it and move on
        p += 2;
        bufinfo.len -= 2;
    }

    if (has_name) {
        // don't actually parse a string for the filename to save RAM since this info is never used
        // just read until we see a null byte and move on
        while (*p++) {
            bufinfo.len--;
        }
    }

    if (has_comment) {
        // don't actually parse a string for the comment to save RAM since this info is never used
        // just read until we see a null byte and move on
        while (*p++) {
            bufinfo.len--;
        }
    }

    // sync state of buffer reads back to buffer
    bufinfo.buf = p;

    return mod_uzlib_decompress_internal(&bufinfo, false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_gzip_decompress_obj, 1, 2, mod_gzip_decompress);

#if MICROPY_PY_GZIP

STATIC const mp_rom_map_elem_t mp_module_gzip_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_gzip) },
    { MP_ROM_QSTR(MP_QSTR_decompress), MP_ROM_PTR(&mod_gzip_decompress_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_gzip_globals, mp_module_gzip_globals_table);

const mp_obj_module_t mp_module_gzip = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_gzip_globals,
};

#endif //MICROPY_PY_GZIP

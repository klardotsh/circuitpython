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

    return mod_uzlib_decompress_internal(&bufinfo, UZLIB_HEADER_GZIP);
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

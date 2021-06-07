// *************************************************************************
// *                                                                       *
// *    MicroPython '_ttyacm' module                                       *
// *                                                                       *
// *************************************************************************
// *                                                                       *
// *    This is the lowest level handler for multiple virtual serial       *
// *    ports over USB. This "_ttyacm" module is imported by the "ttyacm"  *
// *    module which can be found in './modules/ttyacm.py'. That module    *
// *    provides the object-priented interface users should be using.      *
// *                                                                       *
// *    This module provides the basic interface to multiple virtual       *
// *    serial ports but not in a very elegant manner; it uses interface   *
// *    numbers and does not present an object-oriented interface.         *
// *                                                                       *
// *************************************************************************
// *                                                                       *
// *    .len()                  Number of virtual serial ports avaiolable  *
// *                                                                       *
// *    .any(itf)               Number of bytes received                   *
// *    .rxbyte(itf)            Receive a single byte, -1 if none          *
// *    .txbyte(itd, txb)       Transmits a single byte                    *
// *    .rxbreak(itf)           Determine if break received, 0 if not      *
// *                                                                       *
// *************************************************************************

#define PRODUCT "Low-Level VSP Interface"
#define PACKAGE "ttyacm"
#define PROGRAM "modttyacm.c"
#define VERSION "0.01"
#define CHANGES "0000"
#define TOUCHED "0000-00-00 00:00:00"
#define LICENSE "MIT Licensed"
#define DETAILS "https://opensource.org/licenses/MIT"

// .--------------------------------------------------------------------------.
// |    MIT Licence                                                           |
// `--------------------------------------------------------------------------'

// Copyright 2021, "Hippy"

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// .-----------------------------------------------------------------------.
// |    MicroPython, Pico C SDK and TinyUSB includes                       |
// `-----------------------------------------------------------------------'

#include "py/runtime.h"
#include "py/objstr.h"

#include "pico/stdlib.h"
#include <tusb.h>

// *************************************************************************
// *                                                                       *
// *    Hooks into TinyUSB                                                 *
// *                                                                       *
// *************************************************************************

uint16_t breakDuration[CFG_TUD_CDC];

void tud_cdc_send_break_cb(uint8_t itf, uint16_t duration_ms)
{
  if (itf < CFG_TUD_CDC) {
    breakDuration[itf] = duration_ms;
  }
}

// *************************************************************************
// *                                                                       *
// *    C functionality                                                    *
// *                                                                       *
// *************************************************************************

int32_t ttyacm_len(void) {
  return CFG_TUD_CDC;
}

int32_t ttyacm_any(int32_t itf) {
  if (itf >= 0 && itf < CFG_TUD_CDC) {
    return tud_cdc_n_available(itf);
  }
  return 0;
}

int32_t ttyacm_rxbyte(int32_t itf) {
  // Calling 'ttyacm_any(itf)' checks 'itf' is in range so we don't
  // need to explicitly check it here.
  if (ttyacm_any(itf)) {
    uint8_t c;
    tud_cdc_n_read(itf, &c, 1);
    return c;
  }
  return -1;
}

void ttyacm_txbyte(int32_t itf, int32_t txb) {
  if (itf >= 0 && itf < CFG_TUD_CDC) {
    uint8_t c = txb & 0xFF;
    tud_cdc_n_write(itf, &c, 1);
    tud_cdc_n_write_flush(itf);
  }
}

uint16_t ttyacm_rxbreak(int32_t itf) {
  if (itf >= 0 && itf < CFG_TUD_CDC) {
    uint16_t duration = breakDuration[itf];
    breakDuration[itf] = 0;
    return duration;
  }
  return 0;
}

// *************************************************************************
// *                                                                       *
// *    MicroPython interface                                              *
// *                                                                       *
// *************************************************************************

STATIC const MP_DEFINE_STR_OBJ(PRODUCT_string_obj, PRODUCT);
STATIC const MP_DEFINE_STR_OBJ(PACKAGE_string_obj, PACKAGE);
STATIC const MP_DEFINE_STR_OBJ(PROGRAM_string_obj, PROGRAM);
STATIC const MP_DEFINE_STR_OBJ(VERSION_string_obj, VERSION);
STATIC const MP_DEFINE_STR_OBJ(CHANGES_string_obj, CHANGES);
STATIC const MP_DEFINE_STR_OBJ(TOUCHED_string_obj, TOUCHED);
STATIC const MP_DEFINE_STR_OBJ(LICENSE_string_obj, LICENSE);
STATIC const MP_DEFINE_STR_OBJ(DETAILS_string_obj, DETAILS);

// .-----------------------------------------------------------------------.
// |    Information on interfaces available                                |
// `-----------------------------------------------------------------------'

STATIC mp_obj_t ttyacm_len_extension(void) {
  return MP_OBJ_NEW_SMALL_INT(ttyacm_len());
}
MP_DEFINE_CONST_FUN_OBJ_0(ttyacm_len_obj,
                          ttyacm_len_extension);

// .-----------------------------------------------------------------------.
// |    Determine if receive data is available                             |
// `-----------------------------------------------------------------------'

STATIC mp_obj_t ttyacm_any_extension(mp_obj_t itf_obj) {
  int32_t itf = mp_obj_get_int(itf_obj);
  int32_t res = ttyacm_any(itf);
  return MP_OBJ_NEW_SMALL_INT(res);
}
MP_DEFINE_CONST_FUN_OBJ_1(ttyacm_any_obj,
                          ttyacm_any_extension);

// .-----------------------------------------------------------------------.
// |    Receive byte routines                                              |
// `-----------------------------------------------------------------------'

STATIC mp_obj_t ttyacm_rxbyte_extension(mp_obj_t itf_obj) {
  int32_t itf = mp_obj_get_int(itf_obj);
  int32_t res = ttyacm_rxbyte(itf);
  return MP_OBJ_NEW_SMALL_INT(res);
}
MP_DEFINE_CONST_FUN_OBJ_1(ttyacm_rxbyte_obj,
                          ttyacm_rxbyte_extension);

// .-----------------------------------------------------------------------.
// |    Transmit byte routines                                             |
// `-----------------------------------------------------------------------'

STATIC mp_obj_t ttyacm_txbyte_extension(mp_obj_t itf_obj, mp_obj_t txb_obj) {
  int32_t itf = mp_obj_get_int(itf_obj);
  int32_t txb = mp_obj_get_int(txb_obj);
  ttyacm_txbyte(itf, txb);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(ttyacm_txbyte_obj,
                          ttyacm_txbyte_extension);

// .-----------------------------------------------------------------------.
// |    Break detect routines                                              |
// `-----------------------------------------------------------------------'

STATIC mp_obj_t ttyacm_rxbreak_extension(mp_obj_t itf_obj) {
  int32_t itf = mp_obj_get_int(itf_obj);
  int32_t res = ttyacm_rxbreak(itf);
  return MP_OBJ_NEW_SMALL_INT(res);
}
MP_DEFINE_CONST_FUN_OBJ_1(ttyacm_rxbreak_obj,
                          ttyacm_rxbreak_extension);

// *************************************************************************
// *                                                                       *
// *    MicroPython module definition                                      *
// *                                                                       *
// *************************************************************************

STATIC const mp_rom_map_elem_t ttyacm_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__),     MP_ROM_QSTR(MP_QSTR__ttyacm)        },

  { MP_ROM_QSTR(MP_QSTR_PRODUCT),      MP_ROM_PTR(&PRODUCT_string_obj)     },
  { MP_ROM_QSTR(MP_QSTR_PACKAGE),      MP_ROM_PTR(&PACKAGE_string_obj)     },
  { MP_ROM_QSTR(MP_QSTR_PROGRAM),      MP_ROM_PTR(&PROGRAM_string_obj)     },
  { MP_ROM_QSTR(MP_QSTR_VERSION),      MP_ROM_PTR(&VERSION_string_obj)     },
  { MP_ROM_QSTR(MP_QSTR_CHANGES),      MP_ROM_PTR(&CHANGES_string_obj)     },
  { MP_ROM_QSTR(MP_QSTR_TOUCHED),      MP_ROM_PTR(&TOUCHED_string_obj)     },
  { MP_ROM_QSTR(MP_QSTR_LICENSE),      MP_ROM_PTR(&LICENSE_string_obj)     },
  { MP_ROM_QSTR(MP_QSTR_DETAILS),      MP_ROM_PTR(&DETAILS_string_obj)     },

  { MP_ROM_QSTR(MP_QSTR_len),          MP_ROM_PTR(&ttyacm_len_obj)         },
  { MP_ROM_QSTR(MP_QSTR_any),          MP_ROM_PTR(&ttyacm_any_obj)         },
  { MP_ROM_QSTR(MP_QSTR_rxbyte),       MP_ROM_PTR(&ttyacm_rxbyte_obj)      },
  { MP_ROM_QSTR(MP_QSTR_txbyte),       MP_ROM_PTR(&ttyacm_txbyte_obj)      },
  { MP_ROM_QSTR(MP_QSTR_rxbreak),      MP_ROM_PTR(&ttyacm_rxbreak_obj)     },

};
STATIC MP_DEFINE_CONST_DICT(ttyacm_module_globals, ttyacm_module_globals_table);

// .-----------------------------------------------------------------------.
// |    MicroPython integration                                            |
// `-----------------------------------------------------------------------'

const mp_obj_module_t mp_module_ttyacm = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t *)&ttyacm_module_globals,
};

// *************************************************************************
// *                                                                       *
// *    End of '_ttyacm' module                                            *
// *                                                                       *
// ************************************************************************* 

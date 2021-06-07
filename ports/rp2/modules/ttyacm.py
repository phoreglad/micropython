# **************************************************************************
# *                                                                        *
# *    MicroPython 'ttyacm' module                                         *
# *                                                                        *
# **************************************************************************
# *                                                                        *
# *     This is the high level handler for multiple virtual serial ports   *
# *     over USB. This module uses the lower level "_ttyacm" module to     *
# *     deliver its functionality.                                         *
# *                                                                        *
# **************************************************************************
# *                                                                        *
# *     ttyacm.version()        Version information for module             *
# *     ttyacm.len()            Number of virtual serial ports             *
# *     ttyacm.max()            Number of last virtual serial port         *
# *                                                                        *
# *     this = ttyacm.open(N)   Open a virtual serial port link            *
# *     n = this.any()          Number of bytes received                   *
# *     n = this.rx()           Receive a single byte, -1 is none          *
# *     this.tx(*args)          Transmits bytes or strings                 *
# *     n = this.rxbreak()      Check if break received, 0 is no           *
# *     this.close()            Fish using the virtual serial port         *
# *                                                                        *
# **************************************************************************

PRODUCT = "Virtual Serial Port Interface"
PACKAGE = "ttyacm"
PROGRAM = "ttyacm.py
VERSION = "0.01"
CHANGES = "0000"
TOUCHED = "0000-00-00 00:00:00"
LICENSE = "MIT Licensed"
DETAILS = "https://opensource.org/licenses/MIT"

# .------------------------------------------------------------------------.
# |    MIT Licence                                                         |
# `------------------------------------------------------------------------'

# Copyright 2021, "Hippy"

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

import _ttyacm

def version():
  return PROGRAM + " " + VERSION + " (" + CHANGES + ") "+ TOUCHED

def len():
  return _ttyacm.len()

def max():
  return _ttyacm.len() - 1

class open():
  def __init__(self, itf):
    if itf < 0 or itf >= _ttyacm.len():
      pass
    self.itf = itf
  def close(self):
    pass
  def any(self):
    return _ttyacm.any(self.itf)
  def rx(self):
    return _ttyacm.rxbyte(self.itf)
  def tx(self, arg, *args):
    def tx_this(self, arg):
      if isinstance(arg, str):
        for c in arg:
          if c == "\n":
            _ttyacm.txbyte(self.itf, 0x0D)
            _ttyacm.txbyte(self.itf, 0x0A)
          else:
            _ttyacm.txbyte(self.itf, ord(c))
      else:
        _ttyacm.txbyte(self.itf, arg)
    tx_this(self, arg)
    for arg in args:
      tx_this(self, arg)
  def rxbreak(self):
    return _ttyacm_rxbreak(self.itf)

def loopback(this=2, that=1):
  ttyThis = open(this)
  ttyThat = open(that)
  try:
    while True:
      # this -> that
      rx = ttyThis.rx()
      if rx >= 0:
        ttyThat.tx(chr(rx))
      # that -> this
      rx = ttyThat.rx()
      if rx >= 0:
        ttyThis.tx(chr(rx))
  except KeyboardInterrupt:
    pass

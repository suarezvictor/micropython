#!/usr/bin/env python3

#
# This file is part of LiteX-Boards.
#
# Copyright (c) 2020 Michael Welling <mwelling@ieee.org>
# Copyright (c) 2020 Sean Cross <sean@xobs.io>
# Copyright (c) 2020 Drew Fustini <drew@pdp7.com>
# Copyright (c) 2020 Florent Kermarrec <florent@enjoy-digital.fr>
# SPDX-License-Identifier: BSD-2-Clause

from migen import *
from migen.genlib.resetsync import AsyncResetSynchronizer

from litex.gen import LiteXModule

from litex_boards.platforms import hackaday_hadbadge

from litex.soc.cores.clock import *
from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *

from spi_ram_dual import SpiRamDualQuad
from litex.soc.cores.led import LedChaser

# CRG ----------------------------------------------------------------------------------------------

class _CRG(LiteXModule):
    def __init__(self, platform, sys_clk_freq, with_usb_pll=True):
        self.rst    = Signal()
        self.cd_sys = ClockDomain()

        # # #

        # Clk / Rst
        clk8 = platform.request("clk8")


        # PLL
        self.pll = pll = ECP5PLL()
        pll.pfd_freq_range = (8e6, 400e6) # Lower Min from 10MHz to 8MHz.
        self.comb += pll.reset.eq(self.rst)
        pll.register_clkin(clk8, 8e6)
        pll.create_clkout(self.cd_sys, sys_clk_freq)

        # USB PLL
        if with_usb_pll:
            self.cd_usb_12 = ClockDomain()
            self.cd_usb_48 = ClockDomain()
            usb_pll = pll
            #self.submodules += usb_pll
            #self.comb += usb_pll.reset.eq(~por_done)
            usb_pll.register_clkin(clk8, 8e6)
            usb_pll.create_clkout(self.cd_usb_48, 48e6)
            usb_pll.create_clkout(self.cd_usb_12, 12e6)

        """
        # FPGA Reset (press usr_btn for 1 second to fallback to bootloader)
        reset_timer = WaitTimer(int(8e6))
        reset_timer = ClockDomainsRenamer("por")(reset_timer)
        self.submodules += reset_timer
        self.comb += reset_timer.wait.eq(~rst_n)
        self.comb += platform.request("rst_n").eq(~reset_timer.done)
        """

# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
    def __init__(self, toolchain="trellis", sys_clk_freq=int(48e6),
        with_led_chaser = True, **kwargs):
        platform = hackaday_hadbadge.Platform(toolchain=toolchain)

        # CRG --------------------------------------------------------------------------------------
        self.crg = _CRG(platform, sys_clk_freq)

        # SoCCore ---------------------------------------------------------------------------------
        SoCCore.__init__(self, platform, sys_clk_freq, ident="LiteX SoC on Hackaday Badge", **kwargs)

        # SPI RAM ----------------------------------------------------------------------------------
        if not self.integrated_main_ram_size:
            # # Add the 16 MB SPI RAM at address 0x40000000 # Value at 40010000: afbfcfef
            #
            # see https://github.com/xobs/haddecks/blob/master/haddecks.py
            # Copyright 2018 Sean Cross, Apache license 2.0 http://www.apache.org/licenses/LICENSE-2.0
            #
            reset_cycles = 2**14-1
            ram = SpiRamDualQuad(platform.request("spiram4x", 0), platform.request("spiram4x", 1), dummy=5, reset_cycles=reset_cycles, qpi=True)
            self.submodules.ram = ram
            self.register_mem("main_ram", self.mem_map["main_ram"], self.ram.bus, size=16 * 1024 * 1024)

        # Leds -------------------------------------------------------------------------------------
        if with_led_chaser:
            self.leds = LedChaser(
                pads         = platform.request("led", 0),
                sys_clk_freq = sys_clk_freq)

# Build --------------------------------------------------------------------------------------------

def main():
    from litex.build.parser import LiteXArgumentParser
    parser = LiteXArgumentParser(platform=hackaday_hadbadge.Platform, description="LiteX SoC on Hackaday Badge")
    parser.add_target_argument("--sys-clk-freq", default=48e6,        help="System clock frequency.")
    args = parser.parse_args()

    soc = BaseSoC(
        toolchain    = args.toolchain,
        sys_clk_freq = int(float(args.sys_clk_freq)),
        **parser.soc_argdict)
    builder = Builder(soc, **parser.builder_argdict)
    if args.build:
        builder.build(**parser.toolchain_argdict)

if __name__ == "__main__":
    main()

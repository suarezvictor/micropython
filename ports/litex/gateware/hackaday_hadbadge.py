#!/usr/bin/env python3

#
# This file is part of LiteX-Boards.
#
# Copyright (c) 2020 Michael Welling <mwelling@ieee.org>
# Copyright (c) 2020 Sean Cross <sean@xobs.io>
# Copyright (c) 2020 Drew Fustini <drew@pdp7.com>
# Copyright (c) 2020 Florent Kermarrec <florent@enjoy-digital.fr>
# Copyright (c) 2023 Victor Suarez Rovere <suarezvictor@gmail.com>
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

# LCD ----------------------------------------------------------------------------------------------

from litex.soc.interconnect import stream
from litex.soc.cores.video import video_data_layout

class LCD_PHY(Module):
    def __init__(self, pads, clock_domain="sys", ref_freq=25e6):
        self.sink = sink = stream.Endpoint(video_data_layout)

        # # #
        from lcd import LCD # AUO H320QN01
        l = LCD(pads, ref_freq=ref_freq, OFFX=56-3, OFFY=56-3) #seems like blanking-sync_offset-3
        l = ClockDomainsRenamer(clock_domain)(l)
        self.submodules.lcd = l

        # Always ack Sink, no backpressure.
        self.comb += sink.ready.eq(1)

        # Drive Controls.
        self.comb += l.en.eq(sink.de)
        self.comb += l.hsync.eq(~sink.hsync)
        self.comb += l.vsync.eq(~sink.vsync)

        # Drive Datas.
        self.comb += l.r.eq(sink.r)
        self.comb += l.g.eq(sink.g)
        self.comb += l.b.eq(sink.b)

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


# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
    def __init__(self, toolchain="trellis", sys_clk_freq=int(48e6),
        with_led_chaser = True, with_video_terminal = True, **kwargs):
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


        # Video Terminal ---------------------------------------------------------------------------
        if with_video_terminal:
            from lcd import LCD
            lcd_clk = "usb_12" #use a lower clock for the display
            self.submodules.videophy = LCD_PHY(platform.request("lcd"), clock_domain=lcd_clk, ref_freq=12e6)
            timings = ("480x320@60Hz", {
            	"pix_clk"       : 12e6, #TODO: check datasheet
            	"h_active"      : 480,
	            "h_blanking"    : 64,
        	    "h_sync_offset" : 8,
        	    "h_sync_width"  : 24,
        	    "v_active"      : 320,
        	    "v_blanking"    : 64,
        	    "v_sync_offset" : 8,
        	    "v_sync_width"  : 24,
    	    })
            self.add_video_terminal(phy=self.videophy, timings=timings, clock_domain=lcd_clk)
            #self.add_video_colorbars(phy=self.videophy, timings=timings, clock_domain=lcd_clk)
            
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

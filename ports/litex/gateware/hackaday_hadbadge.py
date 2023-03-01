#!/usr/bin/env python3

#
# This file is part of LiteX-Boards.
#
# Copyright (c) 2020 Michael Welling <mwelling@ieee.org>
# Copyright (c) 2020 Sean Cross <sean@xobs.io> some portions Apache License 2.0
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

from litex.build.generic_platform import Subsignal, Pins, IOStandard, Misc

# LCD ----------------------------------------------------------------------------------------------

from litex.soc.interconnect import stream
from litex.soc.cores.video import video_data_layout

lcd_timings = ("480x320@60Hz", {
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

dvi_pads = ("dvi_out", 0, #DRIVE=4 should limit current to 4mA => +/-200mV (exactly what CML expects) 
#    Subsignal("data0_p",     Pins("N19"), IOStandard("LVCMOS33"), Misc("SLEWRATE=FAST"), Misc("DRIVE=4")),
#    Subsignal("data0_n",     Pins("N20"), IOStandard("LVCMOS33"), Misc("SLEWRATE=FAST"), Misc("DRIVE=4")),
#    Subsignal("data1_p",     Pins("L20"), IOStandard("LVCMOS33"), Misc("SLEWRATE=FAST"), Misc("DRIVE=4")),
#    Subsignal("data1_n",     Pins("M20"), IOStandard("LVCMOS33"), Misc("SLEWRATE=FAST"), Misc("DRIVE=4")),
#    Subsignal("data2_p",     Pins("L16"), IOStandard("LVCMOS33"), Misc("SLEWRATE=FAST"), Misc("DRIVE=4")),
#    Subsignal("data2_n",     Pins("L17"), IOStandard("LVCMOS33"), Misc("SLEWRATE=FAST"), Misc("DRIVE=4")),
#    Subsignal("clk_p",       Pins("P20"), IOStandard("LVCMOS33"), Misc("SLEWRATE=FAST"), Misc("DRIVE=4")),
#    Subsignal("clk_n",       Pins("R20"), IOStandard("LVCMOS33"), Misc("SLEWRATE=FAST"), Misc("DRIVE=4")),
    Subsignal("data0_p",     Pins("N19"), IOStandard("LVDS")), #good up to 550mbps (500 specified for ODDRX1)
    Subsignal("data1_p",     Pins("L20"), IOStandard("LVDS")),
    Subsignal("data2_p",     Pins("L16"), IOStandard("LVDS")),
    Subsignal("clk_p",       Pins("P20"), IOStandard("LVDS")),
)

# HAD: LFE5U-45F-8BG381C (8=max speed, C=commercial) - 400MHz max with LVDS+GDDRX2 (150MHz with LVCMOS33)
# High speed differential pins
# A2 A4 A5 B1 B4 C1 C4 C18 C20 D1 D5 D17 D18 D19 E3 E4 E17 E20 F1 F3 F4 F17 F19 G2 G3 G5 G18 G19 H4 H17 H18 H20 J1
# J4 J5 J19 K2 K4 K5 K19 L16* L17* L20* M1 M3 M4 M5 M17 M20* N2 N3 N5 N16 N17 N19* N20* P1 P2 P16 P20* R18 R20* T16 T19 U17 U18
# *DVI connector (AC coupled)

# RADIONA: DVI AC coupled

class LCD_PHY(Module):
    def __init__(self, pads, clock_domain="sys", ref_freq=25e6):
        self.sink = sink = stream.Endpoint(video_data_layout)

        # # #
        from lcd import LCD # AUO H320QN01
        l = LCD(pads, ref_freq=ref_freq, OFFX=56-3, OFFY=56) #OFFX seems like blanking-sync_offset-3
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
    def __init__(self, platform, sys_clk_freq, with_usb_pll=True, with_video_pll=True):
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
            pll.create_clkout(self.cd_usb_48, 48e6)
            pll.create_clkout(self.cd_usb_12, 12e6)

        # Video PLL
        if with_video_pll:
            self.submodules.video_pll = video_pll = ECP5PLL()
            video_pll.pfd_freq_range = pll.pfd_freq_range
            video_pll.register_clkin(clk8, 8e6)
            self.cd_dvi = ClockDomain()
            self.cd_dvi5x = ClockDomain()
            video_clock = 55e6
            video_pll.create_clkout(self.cd_dvi, video_clock)
            video_pll.create_clkout(self.cd_dvi5x, 5*video_clock)
            #self.cd_dvi25x = ClockDomain()
            #video_pll.create_clkout(self.cd_dvi25x, 2.5*video_clock) #this is for ODDRX2

# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
    def __init__(self, toolchain="trellis", sys_clk_freq=int(48e6),
        with_led_chaser = True,
        with_video_terminal = False,
        with_video_framebuffer = True,
        **kwargs):
        platform = hackaday_hadbadge.Platform(toolchain=toolchain)

        # CRG --------------------------------------------------------------------------------------
        self.crg = _CRG(platform, sys_clk_freq)

        # SoCCore ---------------------------------------------------------------------------------
        SoCCore.__init__(self, platform, sys_clk_freq, ident="LiteX SoC on Hackaday Badge", **kwargs)

        # SPI RAM ----------------------------------------------------------------------------------
        if not self.integrated_main_ram_size:
            # Add the 16 MB SPI RAM at address 0x40000000 # Value at 40010000: afbfcfef
            # see https://github.com/xobs/haddecks/blob/master/haddecks.py
            reset_cycles = 2**14-1
            self.submodules.ram = ram = SpiRamDualQuad(
            	platform.request("spiram4x", 0),
            	platform.request("spiram4x", 1),
            	dummy=5,
            	reset_cycles=reset_cycles,
            	qpi=True)
            self.register_mem("main_ram", self.mem_map["main_ram"], self.ram.bus, size=16 * 1024 * 1024)


        # Video Terminal ---------------------------------------------------------------------------
        if with_video_terminal:
            from lcd import LCD
            lcd_clk = "usb_12" #use a lower clock for the display
            self.submodules.videophy = LCD_PHY(platform.request("lcd"), clock_domain=lcd_clk, ref_freq=12e6)
            self.add_video_terminal(phy=self.videophy, timings=lcd_timings, clock_domain=lcd_clk)

        # Video Framebuffer ------------------------------------------------------------------------
        if with_video_framebuffer:
            #see VideoFrameBuffer https://github.com/enjoy-digital/litex/blob/master/litex/soc/cores/video.py#L625
            from litex.soc.cores.video import VideoHDMIPHY
            platform.add_extension([dvi_pads])
            self.submodules.videophy = VideoHDMIPHY(platform.request("dvi_out"), clock_domain="dvi")
            #timings = "640x480@60Hz" #works with LVCMOS33/12
            #timings = "800x600@60Hz" #works with LVCMOS33/12 & LVDS
            timings = "1024x768@60Hz" #works with LVDS @ 55MHz (bat  at 60MHz, not at all at 65MHz)
            #timings = "1280x720@60Hz" #doesn't work
            self.add_video_colorbars(phy=self.videophy, timings=timings, clock_domain="dvi")
            
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

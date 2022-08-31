# Dear ImGui example from Micropython
# based on https://github.com/pyimgui/pyimgui/blob/master/doc/source/guide/first-steps.rst
#
# Copyright (C) 2016 Michał Jaworski & pyimgui developers
# Copyright (C) 2022 Victor Suarez Rovere <suarezvictor@gmail.com>
# BSD-3-Clause license

import dpg_lite as imgui
imgui.create_context()

def example_ui():
	imgui.begin("My window", True)
	imgui.text("Hello world!")
	imgui.end()

while True:
	imgui.new_frame()
	example_ui()
	imgui.render()
	imgui.end_frame()


/*
  * Copyright (c) 2019 Heng Yuan
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
 #include <agg_pixfmt_rgba.h>
 #include <agg_rasterizer_scanline_aa.h>
 #include <agg_renderer_base.h>
 #include <agg_renderer_scanline.h>
 #include <agg_rounded_rect.h>
 #include <agg_scanline_p.h>
 #include <agg_scanline_u.h>
 
 #include <stdio.h>
 #include <stdint.h>

extern uintptr_t framebuffer_address;

#if 0
extern "C" int agg_demo(void)
{

         const int imageWidth = /*200*/640;
         const int imageHeight = /*200*/480;
 
         typedef agg::pixfmt_rgba32  PixelFormat;
         typedef agg::renderer_base<PixelFormat> RendererBaseType;
 
         const int pixelSize = PixelFormat::pix_width;
 
         agg::rendering_buffer   renderBuffer;
         PixelFormat             pixFmt;
         RendererBaseType        rBase;
 
         unsigned char *imageBuffer = (unsigned char *) framebuffer_address; //new unsigned char[imageWidth * imageHeight * pixelSize];
 

         // Hook everything up
         renderBuffer.attach (imageBuffer, imageWidth, imageHeight, imageWidth * pixelSize);
         pixFmt.attach(renderBuffer);
         rBase.attach(pixFmt);
 
         const agg::rgba8 bgColor (0x00, 0x00, 0x00, 0x00);
         const agg::rgba8 greenColor (0, 0xff, 0, 0xff);
         const agg::rgba8 blueColor (0, 0, 0xff, 0xff);
 
         // clear the buffer
         rBase.clear(bgColor);

         agg::rasterizer_scanline_aa<> ras;
         agg::scanline_p8 scanline; //try packed or unpacked
         //agg::scanline_u8 scanline; 
 
 
         {
             agg::rounded_rect rect1 (0, 0, 195, 195, 0);
             ras.reset();
             ras.add_path(rect1); //hangs here!
             agg::render_scanlines_aa_solid(ras, scanline, rBase, blueColor);

 
             agg::rounded_rect rect2 (20, 60, 170, 110, 10);
             ras.reset();
             ras.add_path(rect2);
             agg::render_scanlines_aa_solid(ras, scanline, rBase, greenColor);
         }

 
     return 0;
}

#include <../src/agg_rounded_rect.cpp>
#include <../src/agg_arc.cpp>
#else

/*
#python example

import celiagg as agg
import numpy as np
from PIL import Image

state = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawStroke,
                          line_width=10.0)
transform = agg.Transform()
red_paint = agg.SolidPaint(1.0, 0.0, 0.0)
orange_paint = agg.SolidPaint(0.99, 0.66, 0.0)

canvas = agg.CanvasRGB24(np.zeros((400, 400, 3), dtype=np.uint8))

path = agg.Path()
path.ellipse(200, 200, 190, 190)
canvas.clear(1.0, 1.0, 1.0)
canvas.draw_shape(path, transform, state, stroke=red_paint)

font = agg.Font(agg.example_font(), 96.0)
transform.translate(30.0, 220.0)
canvas.draw_text("celiagg", font, transform, state, fill=orange_paint)

image = Image.fromarray(canvas.array, "RGB")
image.save("example.png")

*/

#include <../../../celiagg/canvas_impl.cpp>
#include <../../../celiagg/image.cpp>
#include <../../../celiagg/font.cpp>
#include <../../../celiagg/font_cache.cpp>
#include <../../../celiagg/paint.cpp>
#include <../../../celiagg/vertex_source.cpp>

extern "C" int agg_demo(void)
{
  static FontCache cache;
  unsigned char *buf = (unsigned char *) framebuffer_address;
  unsigned width=640-1 /*leave a black pixel at right*/, height=480;
  int stride=640*4;
  bool bottom_up=false;
  memset(buf, 0, height*stride);
  
  
  auto state = GraphicsState();
  state.drawing_mode(GraphicsState::DrawStroke);
  state.line_width(10.0);
  
  auto transform = agg::trans_affine();
  auto orange_paint = Paint(0.99, 0.66, 0.0, 1.0);
  auto red_paint = Paint(1.0, 0.0, 0.0, 1.0);
  
  PathSource *path = new PathSource();
  path->ellipse(200, 200, 190, 190);

  canvas_base *canvas = new canvas_rgba32_t(buf, width, height, stride, 4, cache, bottom_up);
  canvas->clear(0.5, 0.25, 1.0, 1.0);
  canvas->draw_shape(*path, transform, orange_paint, red_paint, state);

  delete canvas;
  delete path;
  return 0;
}

// celiaag dependencies:
#include <../src/agg_arc.cpp>
#include <../src/agg_arrowhead.cpp>
#include <../src/agg_bezier_arc.cpp>
#include <../src/agg_bspline.cpp>
#include <../src/agg_curves.cpp>
#include <../src/agg_vcgen_contour.cpp>
#include <../src/agg_vcgen_dash.cpp>
#include <../src/agg_vcgen_markers_term.cpp>
#include <../src/agg_vcgen_smooth_poly1.cpp>
#include <../src/agg_vcgen_stroke.cpp>
#include <../src/agg_vcgen_bspline.cpp>
#include <../src/agg_gsv_text.cpp>
#include <../src/agg_image_filters.cpp>
#include <../src/agg_line_aa_basics.cpp>
#include <../src/agg_line_profile_aa.cpp>
#include <../src/agg_rounded_rect.cpp>
#include <../src/agg_sqrt_tables.cpp>
#include <../src/agg_embedded_raster_fonts.cpp>
#include <../src/agg_trans_affine.cpp>
#include <../src/agg_trans_warp_magnifier.cpp>
#include <../src/agg_trans_single_path.cpp>
#include <../src/agg_trans_double_path.cpp>
#include <../src/agg_vpgen_clip_polygon.cpp>
#include <../src/agg_vpgen_clip_polyline.cpp>
#include <../src/agg_vpgen_segmentator.cpp>
#include <../src/ctrl/agg_cbox_ctrl.cpp>
#include <../src/ctrl/agg_gamma_ctrl.cpp>
#include <../src/ctrl/agg_gamma_spline.cpp>
#include <../src/ctrl/agg_rbox_ctrl.cpp>
#include <../src/ctrl/agg_slider_ctrl.cpp>
#include <../src/ctrl/agg_spline_ctrl.cpp>
#include <../src/ctrl/agg_scale_ctrl.cpp>
#include <../src/ctrl/agg_polygon_ctrl.cpp>
#include <../src/ctrl/agg_bezier_ctrl.cpp>
//optional gpc/gpc.c


#endif

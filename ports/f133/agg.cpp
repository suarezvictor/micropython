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

/*
 #include "path.h"
 #include "writepng.h"
 */

extern uintptr_t framebuffer_address;
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

// Copyright (C) 2021-2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause
#define USE_CIMGUI

#include <cstdio>

extern "C" {
#include "lite_fb.h"
#include "py/gc.h"
}

#include "imgui.h"
#include "imgui_sw.h"
/*
static void *custom_malloc(size_t size, void* user_data)
{
  IM_UNUSED(user_data);
  if (size == 0)
    ++size; //always alloc somethig in case malloc returns NULL for 0 bytes  
  return malloc(size);
}

static void custom_free(void* ptr, void* user_data)
{
  IM_UNUSED(user_data);
  free(ptr);
}
*/

static size_t alloc_total = 0;
static void *custom_malloc(size_t size, void* user_data)
{
  IM_UNUSED(user_data);
  size_t *ptr = (size_t *) gc_alloc(size+sizeof(size_t), false);
  if(!ptr) return ptr;
  *ptr++ = size;
  alloc_total += size;
  //printf("alloc: 0x%p, size %d, total %d\n", ptr, size, alloc_total);
  return ptr;
}

static void custom_free(void* ptr, void* user_data)
{
  IM_UNUSED(user_data);
  if(!ptr) return;
  size_t *ptra = (size_t*)ptr;
  size_t size = *(--ptra);
  alloc_total -= size;
  //printf("free: 0x%p, size %d, total %d\n", ptr, size, alloc_total);
  gc_free(ptra);
}


extern "C" void dpg_lite_init(void)
{
  fb_init();
  fb_set_dual_buffering(1);

    printf("Initializing ImGui at %dx%d...\n", VIDEO_FRAMEBUFFER_HRES, VIDEO_FRAMEBUFFER_VRES);
    IMGUI_CHECKVERSION();
    ImGui::SetAllocatorFunctions(custom_malloc, custom_free, nullptr);
    ImGui::CreateContext();
/*
    ImGuiIO& io = ImGui::GetIO();
    //io.MouseDrawCursor = true; //makes things hang up
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   // We can honor GetMouseCursor() values (optional)
    for(uint8_t key = 0; key < HID_KEY_MAX; ++key)
    {
      ImGuiKey imkey = scan2imguikey(key);
      if(imkey != IMGUIKEY_NONE)
      {
        //printf("key 0x%02x -> imkey %d\n", key, imkey);
        io.KeyMap[imkey] = key; //setup keymap
      }
    }
*/
    imgui_sw::bind_imgui_painting();
    imgui_sw::make_style_fast();

    printf("Starting ImGui...\n");
}
/*
extern "C" void dpg_demo(void)
{
  static int color_r = 0x80, color_g = 0, color_b = 0;
  ImGui::SetNextWindowPos(ImVec2(100, 100));        
  ImGui::SetNextWindowSize(ImVec2(180, 100));
  ImGui::Begin("Color");
  ImGui::SliderInt("R", &color_r, 0, 255);
  ImGui::SliderInt("G", &color_g, 0, 255);
  ImGui::SliderInt("B", &color_b, 0, 255);
  ImGui::End();

  if(color_r++>=255) color_r=0; //automatic movement
  //return IM_COL32(color_r, color_g, color_b, 0);
}
*/
extern "C" void dpg_create_context(void)
{
    dpg_lite_init();
}

extern "C" void dpg_new_frame(float dt)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(VIDEO_FRAMEBUFFER_HRES, VIDEO_FRAMEBUFFER_VRES);
    

    io.DeltaTime = dt;

    ImGui::NewFrame();
}

extern "C" void dpg_end_frame(void)
{
    ImGui::EndFrame();
}

extern "C" void dpg_render(void)
{
    //ImGuiIO& io = ImGui::GetIO();
    //TODO: move button & wheel logic
    //ImVec2 p = io.MousePos;
    //int mousex = (int)p.x, mousey = (int)p.y;

    ImGui::Render();

    imgui_sw::paint_imgui((uint32_t*)fb_base,VIDEO_FRAMEBUFFER_HRES,VIDEO_FRAMEBUFFER_VRES);
/*
    //draw the mouse pointer as a cross
    fb_set_cliprect(0, 0, VIDEO_FRAMEBUFFER_HRES-1, VIDEO_FRAMEBUFFER_VRES-1);
    fb_line(mousex-5, mousey, mousex+6, mousey, IM_COL32(0, 255, 0, 0));
    fb_line(mousex, mousey-5, mousex, mousey+6, IM_COL32(0, 255, 0, 0));
*/
    fb_swap_buffers();
    //fb_fillrect(0, 0, VIDEO_FRAMEBUFFER_HRES-1, VIDEO_FRAMEBUFFER_VRES-1, bgcolor);
    fb_clear();
}

#ifdef USE_CIMGUI
#include "cimgui.h"
#include "cimgui.cpp"
#endif


// This file is Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

#ifndef __LITESDK_DMA_H_
#define __LITESDK_DMA_H_

#include "litesdk_csrdefs.h" //should be autogenerated

///////////////////////////////
// DMA Writer
///////////////////////////////
#ifdef LITEDMA_WRITER_ENABLED

//getters projectors
static inline const void *litedma_writer_get_baseaddress(const litedma_writer_t *dma)
{
  return (void*) (uintptr_t) LITEDMAWRITER_READ(dma, BASE);
}

static inline litedma_writer_length_t litedma_writer_get_length(const litedma_writer_t *dma)
{
  return LITEDMAWRITER_READ(dma, LENGTH);
}

static inline bool litedma_writer_is_started(const litedma_writer_t *dma)
{
  return dma->ENABLE;
}

static inline bool litedma_writer_is_done(const litedma_writer_t *dma)
{
  return dma->DONE;
}

static inline bool litedma_writer_has_loop(const litedma_writer_t *dma)
{
  return dma->LOOP;
}

static inline litedma_writer_offset_t litedma_writer_get_offset(const litedma_writer_t *dma)
{
  return LITEDMAWRITER_READ(dma, OFFSET);
}

//setter projectors
static inline void litedma_writer_set_baseaddress(litedma_writer_t *dma, void *value)
{
  LITEDMAWRITER_WRITE(dma, BASE, (uintptr_t) value);
}

static inline void litedma_writer_set_length(litedma_writer_t *dma, litedma_writer_length_t value)
{
  LITEDMAWRITER_WRITE(dma, LENGTH, value);
}

static inline void litedma_writer_start(litedma_writer_t *dma)
{
  dma->ENABLE = 1;
}

static inline void litedma_writer_stop(litedma_writer_t *dma)
{
  dma->ENABLE = 0;
}

static inline void litedma_writer_set_loop(litedma_writer_t *dma, bool value)
{
  dma->LOOP = value;
}

//helpers
static void litedma_writer_set_buffer(litedma_writer_t *dma, void *buf, litedma_writer_length_t len)
{
    litedma_writer_stop(dma);
    litedma_writer_set_baseaddress(dma, buf);
    litedma_writer_set_length(dma, len);
}

//lookups base address from a pheripheral id (0, 1, 2...)
static inline litedma_writer_t *litedma_writer_instance(litepheripheral_id id)
{
  if(id != 0) //only supports single DMA
    return NULL; 
  return (litedma_writer_t *) CSR_DMA_WRITER_BASE;
}

static inline void litedma_writer_init(litedma_writer_t *dma)
{
  litedma_writer_stop(dma);
}

static inline void litedma_writer_deinit(litedma_writer_t *dma)
{
  litedma_writer_stop(dma);
}
#endif //LITEDMA_WRITER_ENABLED


///////////////////////////////
// DMA Reader
///////////////////////////////
#ifdef LITEDMA_READER_ENABLED

//getter projectors
static inline void *litedma_reader_get_baseaddress(const litedma_reader_t *dma)
{
  return (void*) (uintptr_t) LITEDMAREADER_READ(dma, BASE); //possibly 64-bit (needs swapping)
}

static inline litedma_reader_length_t litedma_reader_get_length(const litedma_reader_t *dma)
{
  return LITEDMAREADER_READ(dma, LENGTH); //possibly 64-bit (needs swapping)
}

static inline bool litedma_reader_is_started(const litedma_reader_t *dma)
{
 return dma->ENABLE;
}

static inline bool litedma_reader_is_done(const litedma_reader_t *dma)
{
  return dma->DONE;
}

static inline bool litedma_reader_has_loop(const litedma_reader_t *dma)
{
 return dma->LOOP;
}

static inline litedma_reader_offset_t litedma_reader_get_offset(const litedma_reader_t *dma)
{
  return LITEDMAREADER_READ(dma, OFFSET);
}

//setter projectors
static inline void litedma_reader_set_baseaddress(litedma_reader_t *dma, const void *value)
{ 
  LITEDMAREADER_WRITE(dma, BASE, (uintptr_t) value); //possibly 64-bit
}

static inline void litedma_reader_set_length(litedma_reader_t *dma, litedma_reader_length_t value)
{
  LITEDMAREADER_WRITE(dma, LENGTH, value); //possibly 64-bit
}
static inline void litedma_reader_start(litedma_reader_t *dma)
{
  dma->ENABLE = 1;
}

static inline void litedma_reader_stop(litedma_reader_t *dma)
{
  dma->ENABLE = 0;
}

static inline void litedma_reader_set_loop(litedma_reader_t *dma, bool value)
{
  dma->LOOP = value;
}

//helpers
static void litedma_reader_set_buffer(litedma_reader_t *dma, const void *buf, litedma_reader_length_t len)
{
    litedma_reader_stop(dma);
    litedma_reader_set_baseaddress(dma, buf);
    litedma_reader_set_length(dma, len);
}

//lookups base address from a pheripheral id (0, 1, 2...)
static inline litedma_reader_t *litedma_reader_instance(litepheripheral_id id)
{
  if(id != 0) //only supports single DMA
    return NULL; 
  return (litedma_reader_t *) CSR_DMA_READER_BASE;
}

static inline void litedma_reader_init(litedma_reader_t *dma)
{
  litedma_reader_stop(dma);
}

static inline void litedma_reader_deinit(litedma_reader_t *dma)
{
  litedma_reader_stop(dma);
}
#endif //LITEDMA_READER_ENABLED


#endif //__LITESDK_DMA_H_

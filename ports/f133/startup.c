//TODO: use ((constructor)) attribute to manage things in C and not in the startup assembly

typedef void (*func_ptr) (void);
extern func_ptr _init_array_start[], _init_array_end[];
extern func_ptr _fini_array_start[], _fini_array_end[];

void _init_init(void)
{
  for ( func_ptr* func = _init_array_start; func != _init_array_end; ++func )
    (*func)();
}
 
void _fini_fini(void)
{
  for ( func_ptr* func = _fini_array_start; func != _fini_array_end; ++func )
    (*func)();
}




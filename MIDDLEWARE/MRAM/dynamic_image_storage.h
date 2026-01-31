#ifndef DYNAMIC_IMAGE_STORAGE
#define DYNAMIC_IMAGE_STORAGE

#include "file_type.h"
#include "MRAM.h"

void write(Image_Storage*, Image*);
void write_positive(Image_Storage*, Image*);
void write_negative(Image_Storage*, Image*);

void header_to_bitfield(Image*);
void bitfield_to_header(Image*);

void read_positive(Image_Storage*, Image*);
void read_negative(Image_Storage*, Image*);

uint32_t get_positive_pointer(Image_Storage*);
uint32_t get_negative_pointer(Image_Storage*);

void set_positive_pointer(Image_Storage*, uint32_t);
void set_negative_pointer(Image_Storage*, uint32_t);

void reset_pointers(Image_Storage*);

#endif
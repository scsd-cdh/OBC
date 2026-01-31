#include "dynamic_image_storage.h"

//#define _DEBUG

/*
Automatically decides whether to write to the positive or negative storage based on the is_positive flag in the Image struct
Converts the header information into a compact bit field for storage
*/
void write(Image_Storage* p_image_storage, Image* p_image){
	// Converting the header information into a bits for future use. The information is stored in the image struct
	header_to_bitfield(p_image);
		
	// Determine if the image is positive or negative
	if (p_image->is_positive)
	{
		write_positive(p_image_storage, p_image);
	} 
	else
	{
		write_negative(p_image_storage, p_image);
	}
}

/*
Positive images are the images that are stored towards the end of the partition.
Starting from the very end of the partition and moving towards the beginning.
Positive pointers points to the start of the header of the newest positive image (the image that is the closest to the beginning).
*/
void write_positive(Image_Storage* p_image_storage, Image* p_image) 
{
	int node_size = p_image->node_size;								// The size of everything including the header, the image data itself and the footer (bytes) 
	uint32_t negative_pointer = get_negative_pointer();				// A pointer that points to where the last negative image has been written to. (Points at the address after the CRC)
	uint32_t positive_pointer = get_positive_pointer();				// A pointer that points to where the last positive image has been written to. (Points at the address before the header)
	uint32_t next_positive_address = positive_pointer - node_size;	// The address where the next positive image should start.
	uint32_t negative_bounds = p_image_storage->negative_boundary;	// Address at which the positive write can't cross, At init have at least 1 image worth of space.
	uint32_t image_size = p_image->image_size;						// Size of the data of the image.

	// Check if there is space to write the positive image
	if (next_positive_address <= negative_pointer || next_positive_address <= negative_bounds)
	{
		printf("Overlapping the negative write zone, Aborting..."); //idk if this works. If it does'nt, need to find out how to do a serial output on the board.
	}
	
	// Set the address to write the image
	uint32_t write_address = next_positive_address;
	
	// Writing the header into the MRAM
	MRAM_write_cmd_addr_data(Write_Mem_Array, write_address, p_image->header, sizeof(p_image->header) / sizeof(p_image->header[0]));
	write_address += sizeof(p_image->header) / sizeof(p_image->header[0]);
	
	// Writing the image data into MRAM
	MRAM_write_cmd_addr_data(Write_Mem_Array, write_address, p_image->content, sizeof(p_image->content) / sizeof(p_image->content[0]));
	write_address += sizeof(p_image->content) / sizeof(p_image->content[0]);
	
	// Writing the CRC into MRAM
	MRAM_write_cmd_addr_data(Write_Mem_Array, write_address, p_image->CRC, sizeof(p_image->CRC) / sizeof(p_image->CRC[0]));
	write_address += sizeof(p_image->CRC) / sizeof(p_image->CRC[0]);
	
	// Update the positive image pointer
	set_positive_pointer(p_image_storage, next_positive_address);
}
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
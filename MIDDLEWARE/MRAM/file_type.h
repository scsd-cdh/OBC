#ifndef FILE_TYPE_H
#define FILE_TYPE_H

const uint32_t HEADER_SIZE = 3; // This is a temp variable (delete once the full implementation is done)

typedef struct MRAM_Block 
{
	uint32_t data_start_address;	// The address where the block starts
	uint32_t data_end_address;		// The address where the block ends
} MRAM_Block;

typedef struct Telemetry_Storage 
{
	int size;
	MRAM_Block partition;
	
	uint32_t write_pointer_address;	// The address where the write pointer is stored in the MRAM.
	uint32_t read_pointer_address;	// The address where the read pointer is stored in the MRAM.
	uint32_t roll_over_address;		// The address where the roll over pointer is.
} Telemetry_Storage;

typedef struct Image_Storage 
{
	MRAM_Block partition;
	
	uint32_t positive_pointer_address;	// A pointer that indicates the address of the last positive flag image.
	uint32_t negative_pointer_address;	// A pointer that indicates the address of the last negative flag image.
	
	uint32_t positive_boundary;			// Address at which the negative write can't cross, At init have at least 1 image worth of space. This one should be towards the end of the MRAM
	uint32_t negative_boundary;			// Address at which the positive write can't cross, At init have at least 1 image worth of space.
};

typedef struct Image 
{
	bool is_positive;	// Whether the image has a positive flag from the Zetan camera.
	int node_size;		// The size of everything including the header, the image data itself and the footer (bytes)
	int image_size;		// Size of the data part of the image (bytes)
	uint8_t* content;	// The content of the image
	uint8_t* CRC;		// CRC is 4 bytes
	uint8_t header[3];	// The header of the image. It contains information about the positive flag and the data size of the image.
} Image;


#endif
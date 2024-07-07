

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Incredible resource -> http://www.multiweb.cz/twoinches/mp3inside.htm
typedef struct TagV2 {
	char identifier[3];
	char TAG_VERSION[2];
	char flags;
	char tag_size[4];
} TagV2;

typedef struct MP3FrameHeader{
	unsigned char bytes[4]; 
	// Not very descriptive but you'd understand if you read the documentation
	// The header is essentially written in byte terms.
} MP3FrameHeader;

int traverse_TAGV2(FILE *ptr){
	TagV2 *tag = (TagV2 *)malloc(sizeof(TagV2));	
	size_t bytesRead = fread(tag, sizeof(char), sizeof(TagV2), ptr);

	int bytes_to_traverse;
	bytes_to_traverse = 10; //Account for the header
	bytes_to_traverse += (tag->tag_size[0] << 21) | (tag->tag_size[1] << 14) | (tag->tag_size[2] << 7) | tag->tag_size[3];	
	free(tag);
	if (fseek(ptr, bytes_to_traverse, SEEK_SET) != 0) {
        	printf("Error seeking to frames\n");
        	fclose(ptr);
        	return 0;
	}
	return 1;

}
int verify_synchronisation(MP3FrameHeader *header) {
    int mask = 0xE0;
    for (int i = 0; i < 4; i++) {
        printf("Byte %d: %02X\n", i, header->bytes[i]);
    }

    if ((header->bytes[0] == 0xFF) && ((header->bytes[1] & mask) == mask)) {
        return 1;   
    }

    return 0;
}

int main(int argc, const char *argv[]) {
	const char *filename;
    	if (argc > 1) {
        	// Access the first argument
        	filename = argv[1];
        	printf("The first argument is: %s\n", filename);
    	} else {
        	printf("No arguments provided.\n");
		return 0;
    	}
    	FILE *ptr;
	ptr = fopen(filename, "rb");
    	if (ptr == NULL) {
    		printf("Error opening file %s\n", filename);
    		return 0;
	}

	if (traverse_TAGV2(ptr) == 0){
		printf("Error");
		return 0;
	}
	
	MP3FrameHeader *frameHeader =  (MP3FrameHeader *) malloc(sizeof(MP3FrameHeader));
	size_t bytesRead = fread(frameHeader, sizeof(char), sizeof(MP3FrameHeader), ptr);
	printf("read %d bytes\n", bytesRead);
	int is_synchronised = verify_synchronisation(frameHeader);
	printf("%d", is_synchronised);
	long int currentPosition = ftell(ptr);
	printf("Current position in the file: %ld bytes\n", currentPosition);	
		

	return 1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ULONG unsigned long

ULONG file_size(FILE *input_file)
{
	// Returns size of file
	ULONG origin = ftell(input_file);
	fseek(input_file, 0, SEEK_END);
	printf("fseek END errors: %s\n", strerror(errno));
	ULONG end = ftell(input_file);
	fseek(input_file, origin, SEEK_SET);
	printf("fseek ORIGIN errors: %s\n", strerror(errno));
	return end;

}

long write_data(FILE *input_file, FILE *output_file, long start_at)
{
	/*
	This function writes the data of input_file to output_file starting at start_at.
	Returns -1 on error.
	Returns the end byte of where the data ends at.
	*/
	fseek(output_file, start_at, SEEK_SET);
	ULONG input_size = file_size(input_file);
	ULONG output_size = file_size(output_file);
	long bytes_required = output_size - start_at;
	printf("Input size: %li\n", input_size);
	printf("outputSize - start_at = %li\n", bytes_required);
	if (input_size > bytes_required)
	{
		printf("ERROR! Input file cannot fit inside output file.\nPlease select a smaller start at or choose a bigger file.\n");
		fclose(input_file);
		fclose(output_file);
		exit(1);
	}
	void* write_buffer = malloc(2048); // Allocate for writing stuff
	long written = 0;
	while (input_size > written)
	{
		printf("write iteration\n");
		ULONG bytes_read = fread(write_buffer, 1, 2048, input_file);
		ULONG bytes_written = fwrite(write_buffer, 1, bytes_read, output_file);
		written = written + bytes_written;
	}

	free(write_buffer);
	printf("I am returning %li", written);
	return written;
}

long read_data(FILE *input_file, char* store_to, ULONG start_at, ULONG end_at)
{
	/*
		Stores data of input_file to store_to starting at start_at and ending at end_at
		Returns NULL on error.
		returns the amount of bytes it has written.
	 */
	ULONG output_filesize = end_at - start_at;
	printf("Output fileSize: %li\nStart_at: %li\nEnd_at: %li\n", output_filesize, start_at, end_at);
	printf("Attempting to open STORING file.\n");
	FILE *storing_file = fopen(store_to, "w");
	
	printf("Creating buffer for reading\n");
	void* buffer = malloc(2048);
	ULONG stored = 0;
	ULONG to_read = output_filesize;
	ULONG read_blockSize;
	if (to_read > 2048)
	{
		read_blockSize = 2048;
	} else {
		read_blockSize = to_read;
	}
	
	fseek(input_file, start_at, SEEK_SET);
	printf("Starting while loop.\n");
	while (output_filesize > stored)
	{
		printf("READ iteration.\n");
		if ((output_filesize - stored) < 2048)
		{
			read_blockSize = output_filesize - stored;
		}
		ULONG read = fread(buffer, 1, read_blockSize, input_file);
		printf("fread errors: %s\n", strerror(errno));
		ULONG written = fwrite(buffer, 1, read, storing_file);
		printf("fwrite errors: %s\n", strerror(errno));
		printf("READ iteration end.\n");
		if ( written == read )
		{
			stored = stored + written;
		} else {
			printf("Warning! I read %li but only stored %li\n", read, written);
			free(buffer);
			fclose(storing_file);
			return 0;
		}
	}
		free(buffer);
		fclose(storing_file);
		return stored;
	
	 
}

void ask_confirmation()
{
	int confirmed = 0;
	scanf("%i", &confirmed);
	if (confirmed != 1)
	{
		printf("Canceled.");
		exit(1);
	}
}

int main(int argc, char** argv)
{
	printf("WARNING! CLEARVISION CANNOT SAVE DATA FROM CORRUPTION, USE AT YOUR OWN RISK!\nEnter 1 to continue. ");
	ask_confirmation();
	
	if (strcmp(argv[1], "write") == 0)
	{
		
		FILE *input_file = fopen(argv[2], "r");
		printf("Input file errors: %s\n", strerror(errno));
		FILE *output_file = fopen(argv[3], "r+");
		printf("Output file errors: %s\n", strerror(errno));
		ULONG start_at = atol(argv[4]);
		ULONG bytes_written = write_data(input_file, output_file, start_at);
		if (bytes_written == 0)
		{
			printf("An error occurred while writing file.\n");
			exit(1);
		}
		printf("File successfully written.\nLater on, recover this file with command:\nclearvision recover %s %s %li %li\n", argv[3], argv[2], start_at, (start_at + bytes_written));
		fclose(input_file);
		fclose(output_file);
		return 0;
	}
	if (strcmp(argv[1], "recover") == 0)
	{
		FILE *input_file = fopen(argv[2], "r");
		long start_at = atol(argv[4]);
		long end_at = atol(argv[5]);
		ULONG stored = read_data(input_file, argv[3], start_at, end_at);
		if (stored != (atol(argv[5]) - atol(argv[4])))
		{
			printf("Warning! I wasnt able to read the amount required!\n");
		}
		fclose(input_file);
		
	}
}
// usage
/*
 * clearvision recover inputdata.img outputfile start_at end_at
 * 
 * 
 * */


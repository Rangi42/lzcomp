#include "proto.h"

int main (int argc, char ** argv) {
  struct options options = get_options(argc, argv);
  unsigned short size;
  unsigned char * file_buffer = read_file_into_buffer(options.input, &size);
  struct command * compressed = compress(file_buffer, &size);
  if (options.mode)
    write_commands_to_textfile(options.output, compressed, size, file_buffer);
  else
    write_commands_to_file(options.output, compressed, size, file_buffer);
  free(file_buffer);
  free(compressed);
  return 0;
}

struct command * compress (const unsigned char * data, unsigned short * size) {
  unsigned char * bitflipped = malloc(*size);
  bit_flip(data, *size, bitflipped);
  struct command * compressed_sequences[COMPRESSION_METHODS];
  unsigned short lengths[COMPRESSION_METHODS];
  const struct compressor * compressor = compressors;
  unsigned current, flags = compressor -> methods;
  for (current = 0; current < COMPRESSION_METHODS; current ++) {
    lengths[current] = *size;
    if (!flags) flags = (++ compressor) -> methods;
    compressed_sequences[current] = compressor -> function(data, bitflipped, lengths + current, -- flags);
  }
  free(bitflipped);
  struct command * result = select_command_sequence(compressed_sequences, lengths, COMPRESSION_METHODS, size);
  for (current = 0; current < COMPRESSION_METHODS; current ++) free(compressed_sequences[current]);
  return result;
}

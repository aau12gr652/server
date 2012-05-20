#include <iostream>
#include <fstream>
#include <kodo/rlnc/full_vector_codes.h>
#include <postoffice/Postoffice.h>
#include <node/kodo_decoder.h>
#include <server/kodo_encoder.h>
#include "video_getter.hpp"

void the_new_make_layers(uint32_t nb_layers, uint32_t gsize, uint32_t symb_size, kodo_encoder* m_kodo_encoder, uint32_t i_size);
uint32_t calculate_generation_size_from_gop_size(uint32_t gop_size);
uint32_t calculate_symbol_size_from_generation_size(uint32_t generation_size);
uint32_t calculate_layer_size(uint32_t layer_size);

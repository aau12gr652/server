#include <iostream>
#include <kodo/rlnc/full_vector_codes.h>
#include "encoder.h"

encoder::encoder(void)
{
    symbols_max = 1500;
    symbol_size_max = 1400;
    layers = 2;
    layer_size.resize(layers);
    layer_size[0] = 500;
    layer_size[1] = 1000;
    layer_gamma.resize(layers);
    layer_gamma[0] = 50;
    layer_gamma[1] = 100;

    Generation_Size = symbols_max;
    symbol_size = symbol_size_max;

    payload_stamp.Generation_ID = 0;
    payload_stamp.Field_Size = 1;
    payload_stamp.Number_Of_Layers = layers;
    payload_stamp.Generation_Size = Generation_Size;
    payload_stamp.Symbol_Size = symbol_size;

    encoder_factory = new rlnc_encoder::factory(symbols_max, symbol_size);
}

void encoder::new_generation(char* data)
{
	assert(data);
    payload_stamp.Generation_ID++;
    for (int n = 0; n < layers; n++)
    {
        encoders[n] = encoder_factory->build(layer_size[n], symbol_size);
        data_in_buffers[n].resize(layer_size[n]);
        kodo::set_symbols(kodo::storage(data_in_buffers[n]), encoders[n]);
        payload_buffers[n].resize(encoders[n]->payload_size());
    }
    for (int n = 0; n < layers; n++)
        copy(data, data+layer_size[n], data_in_buffers[n].begin());
}

void encoder::set_symbol_size(uint32_t S)
{
	assert(S <= symbol_size_max);
	symbol_size = S;
	payload_stamp.Symbol_Size = S;
}

void encoder::set_generation_size(uint32_t G)
{
	assert(G <= symbols_max);
    Generation_Size = G;
    payload_stamp.Generation_Size = Generation_Size;
}

void encoder::set_layers(uint32_t L)
{
	layer_size.resize(L);
    layers = L;
    payload_stamp.Number_Of_Layers = layers;
}

void encoder::set_layer_size(uint32_t L, uint32_t S)
{
	assert(L < layers);
	assert(L + 1 != layers || S == Generation_Size);
    layer_size[L] = S;
}

void encoder::set_layer_gamma(uint32_t L, uint32_t G)
{
	assert(L < layers);
	assert(L + 1 != layers || G == 100);
    layer_gamma[L] = G;
}

serial_data encoder::get_packet()
{
    int layer_choice = 1+*devRandom()%100;
    int n;
    for (n = 0; n < layers; n++)
        if (layer_choice <= layer_gamma[n])
            break;
    encoders[n]->encode( &payload_buffers[n][0] );
    payload_stamp.Layer_ID = n+1; // +1 for at gøre Benjamin glad
    payload_stamp.Layer_Size = layer_size[n];
    serial_data return_value = {encoders[n]->payload_size(), (void*)&payload_buffers[n][0]};
    return return_value;
}

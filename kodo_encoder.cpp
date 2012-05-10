#include "kodo_encoder.h"

kodo_encoder::kodo_encoder(void)
{
    symbols_max = 1500;
    symbol_size_max = 1500;
    layers = 2;
    layer_size.resize(layers);
    layer_size[0] = 450;
    layer_size[1] = 1400;
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

    encoder_factory = new rlnc_encoder::factory(symbols_max, symbol_size_max);
    encoders.resize(layers);
    data_in_buffers.resize(layers);
    payload_buffers.resize(layers);
}

void kodo_encoder::new_generation(char* data)
{
	assert(data);
    payload_stamp.Generation_ID++;
    for (uint32_t n = 0; n < layers; n++)
    {
        encoders[n] = encoder_factory->build(layer_size[n], symbol_size);
        data_in_buffers[n].resize(layer_size[n]*symbol_size);
 	    memcpy(&data_in_buffers[n][0],data,layer_size[n]*symbol_size);
        kodo::set_symbols(kodo::storage(data_in_buffers[n]), encoders[n]);
        encoders[n]->systematic_off();
        payload_buffers[n].resize(encoders[n]->payload_size());
    }
}

void kodo_encoder::set_symbol_size(uint32_t S)
{
	assert(S <= symbol_size_max);
	symbol_size = S;
	payload_stamp.Symbol_Size = S;
}

void kodo_encoder::set_generation_size(uint32_t G)
{
	assert(G <= symbols_max);
    Generation_Size = G;
    payload_stamp.Generation_Size = Generation_Size;
}

void kodo_encoder::set_layers(uint32_t L)
{
	assert(L);
	layer_size.resize(L);
    layer_gamma.resize(L);
    encoders.resize(L);
    data_in_buffers.resize(L);
    payload_buffers.resize(L);
    layers = L;
    payload_stamp.Number_Of_Layers = layers;
}

void kodo_encoder::set_layer_size(uint32_t L, uint32_t S)
{
	L--; // -1 for at gøre Benjamin glad
	assert(L < layers);
    layer_size[L] = S;
}

void kodo_encoder::set_layer_gamma(uint32_t L, uint32_t G)
{
	L--; // -1 for at gøre Benjamin glad
	assert(L < layers);
    layer_gamma[L] = G;
}

serial_data kodo_encoder::get_packet()
{
	uint8_t* RandomTal = devRandom();
//    uint32_t layer_choice = 1+*RandomTal%100;
    uint32_t layer_choice = 1+(*RandomTal*99)/255;
    free(RandomTal);
    uint32_t n;
    for (n = 0; n < layers; n++)
        if (layer_choice <= layer_gamma[n])
            break;
    return get_packet(n + 1); // +1 for at gøre Benjamin glad
}

serial_data kodo_encoder::get_packet(uint32_t n)
{
	n--; // -1 for at gøre Benjamin glad
    encoders[n]->encode( &payload_buffers[n][0] );
    payload_stamp.Layer_ID = n + 1; // +1 for at gøre Benjamin glad
    payload_stamp.Layer_Size = layer_size[n];
    serial_data return_value = {encoders[n]->payload_size(), (void*)&payload_buffers[n][0]};
    return return_value;
}

int kodo_encoder::send_packet(postoffice &po)
{
	int return_value = po.send(get_packet(), &payload_stamp);
	if (return_value)
		std::cout << std::endl << "ERROR: " << return_value*1 << std::endl;
	assert(!return_value);
	return return_value;

}

int kodo_encoder::send_packet(postoffice &po, uint32_t L)
{
	int return_value = po.send(get_packet(L), &payload_stamp);
	if (return_value)
		std::cout << std::endl << "ERROR: " << return_value*1 << std::endl;
	assert(!return_value);
	return return_value;
}

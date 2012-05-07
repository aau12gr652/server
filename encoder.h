#ifndef _encoder_h
#define _encoder_h
#include "../postoffice/Postoffice.h"

typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;

class encoder
{
    uint32_t symbols_max;
    uint32_t symbol_size;
    uint32_t layers;
    uint32_t layer_size[10];
    uint32_t layer_gamma[10];
    uint16_t Generation_Size;
    rlnc_encoder::pointer encoders[10];
    std::vector<uint8_t> payload_buffers[10];
    stamp payload_stamp;

public:
    std::vector<uint8_t> data_in_buffers[10];
    encoder(void);
    void new_generation(char*);
    void set_generation_size(uint32_t);
    void set_layers(uint32_t);
    void set_layer_size(uint32_t layer, uint32_t size);
    void set_layer_gamma(uint32_t layer, uint32_t size);
    serial_data get_packet(stamp*);
};

#endif
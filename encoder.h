#ifndef _encoder_h
#define _encoder_h
#include "../postoffice/Postoffice.h"

typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;

class encoder
{
    uint32_t symbols_max;
    uint32_t symbol_size;
    uint32_t symbol_size_max;
    uint32_t layers;
    std::vector<uint32_t> layer_size;
    std::vector<uint32_t> layer_gamma;
    uint16_t Generation_Size;
    std::vector<rlnc_encoder::pointer> encoders;
    std::vector<std::vector<uint8_t> > data_in_buffers;
    std::vector<std::vector<uint8_t> > payload_buffers;
    rlnc_encoder::factory* encoder_factory;

public:
    encoder(void);
    void new_generation(char*);
    void set_generation_size(uint32_t);
    void set_layers(uint32_t);
    void set_layer_size(uint32_t layer, uint32_t size);
    void set_layer_gamma(uint32_t layer, uint32_t size);
    void set_symbol_size(uint32_t S);
    serial_data get_packet();
    serial_data get_packet(uint32_t);
    stamp payload_stamp;
};

#endif

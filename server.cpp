#include <kodo/rlnc/full_vector_codes.h>

#include "../postoffice/Postoffice.h"

int main(){
    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes 
    uint32_t symbols_max = 13;
    uint32_t symbol_size = 1;
    
    
    uint32_t symbols_1 = 5; //The size of HELLO
    uint32_t symbols_2 = 8; // The size of HELLO WO
    uint32_t symbols_3 = symbols_max; //The size of HELLO WORLD!
    
    
    // Typdefs for the encoder/decoder type we wish to use
    typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;
    typedef kodo::full_rlnc_decoder<fifi::binary8> rlnc_decoder;
    
    //Making the factories
    rlnc_encoder::factory encoder_factory(symbols_max, symbol_size);    
    
    //Making the encoders with different sizes
    //encoders
    rlnc_encoder::pointer encoder_1 = encoder_factory.build(symbols_1, symbol_size);
    rlnc_encoder::pointer encoder_2 = encoder_factory.build(symbols_2, symbol_size);
    rlnc_encoder::pointer encoder_3 = encoder_factory.build(symbols_3, symbol_size);
    
    // Allocate some data to encode. In this case we make a buffer with the
    // same size as the encoder's block size (the max. amount a single encoder
    // can encode)
    std::vector<uint8_t> data_in_1(symbols_1);
    std::vector<uint8_t> data_in_2(symbols_2);
    std::vector<uint8_t> data_in_3(13);
    
    const char* streng = "HELLO WORLD!";
    
    copy(streng, streng+symbols_1, data_in_1.begin());
    copy(streng, streng+symbols_2, data_in_2.begin());
    copy(streng, streng+symbols_3, data_in_3.begin());
      
    
    
    // Assign the data buffer to the encoder so that we may start
    // to produce encoded symbols from it
    kodo::set_symbols(kodo::storage(data_in_1), encoder_1);
    kodo::set_symbols(kodo::storage(data_in_2), encoder_2);
    kodo::set_symbols(kodo::storage(data_in_3), encoder_3);
    
    
    
    // Allocate some storage for a "payload" the payload is what we would
    // eventually send over a network
    std::vector<uint8_t> payload_1(encoder_3->payload_size());
    std::vector<uint8_t> payload_2(encoder_3->payload_size());
    std::vector<uint8_t> payload_3(encoder_3->payload_size());
    
    
    
    
    
    //Generate postoffice
    postoffice po("4000", "255.255.255.255");
    
    for (int i = 0; i<symbols_max+3; i++){
    	serial_data letter;
    //int coder_choice = 1+rand()%3;
        
        int coder_choice = 3;
        
        if (coder_choice==1) {
            encoder_1->encode( &payload_1[0] );
            
            stamp p1_stamp = {1, 3, 1, 1, 1, symbols_max, symbols_1};
        	serial_data p1_serial = {encoder_1->payload_size(), (void*)&payload_1[0]};
        	letter = frank(&p1_stamp, p1_serial);
            
        }
        else if (coder_choice==2) {
            encoder_2->encode( &payload_2[0] );
            
            stamp p2_stamp = {1, 3, 2, 1, 1, symbols_max, symbols_2};
        	serial_data p2_serial = {encoder_2->payload_size(), (void*)&payload_2[0]};
        	letter = frank(&p2_stamp, p2_serial);
            
        }
        else {
            encoder_3->encode( &payload_3[0] );
            
            stamp p3_stamp = {1, 3, 3, 1, 1, symbols_max, symbols_3};
        	serial_data p3_serial = {encoder_3->payload_size(), (void*)&payload_3[0]};
        	letter = frank(&p3_stamp, p3_serial);
            
        }
        po.send(letter.data, letter.size);
    }
    
    po.closeConnection();
}

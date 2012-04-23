#include <kodo/rlnc/full_vector_codes.h>

#include "../postoffice/Postoffice.h"

int main(){
    
    std::cout << "SERVER WITH RLNC\n\n" << std::endl;
    
    uint32_t symbols = 42; //Generation size
    uint32_t symbol_size = 100; //Size of every symbol (bytes)
    
    // Typdefs for the encoder/decoder type we wish to use
    typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;//Passing af binary finite field obj to the encoder obj
    typedef kodo::full_rlnc_decoder<fifi::binary8> rlnc_decoder;
    
    //Make encoding factory with the symbols and their size
    rlnc_encoder::factory encoder_factory(symbols, symbol_size);    
    rlnc_encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);

    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    rlnc_decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);
    
    //Allocate storage for the payload
    std::vector<uint8_t> payload(encoder->payload_size());

    std::cout << encoder->block_size();
    
    //Allocate storage for the ingoing data
    std::vector<uint8_t> data_in(encoder->block_size());

    
    // Just for fun - fill the data with random data
    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());
    
    //Set the first three characters (test)
    
    data_in[0] = 'd';
    data_in[1] = 'a';
    data_in[2] = 'w';
    
    // Assign the data buffer to the encoder so that we may start
    // to produce encoded symbols from it
    kodo::set_symbols(kodo::storage(data_in), encoder);
    
    
    const char* IP = "255.255.255.255";
    const char* PORT = "4000";
    
    bool yes = true;
    Postoffice::createTxSocket(yes, IP, PORT, encoder->block_size());
    
    int i=0;
    
    std::cout << "Ready to send packets!\n\n" << std::endl;
    
//    while( i!=symbols )
//    {
//        std::cout << i;
//        // Encode a packet into the payload buffer
//        encoder->encode( &payload[0] );
//        
//        //Send the encoded packet
//        Postoffice::send(&payload[0], sizeof(encoder->payload_size()));
//        i++;
//        
//        std::cout << "packet sent\n";
//    }
    
    
    
    while( 1 )
    {
        // Encode a packet into the payload buffer
        encoder->encode( &payload[0] );
        
        //Send the encoded packet
        Postoffice::send(&payload[0], sizeof(encoder->payload_size()));
        i++;
        
        
        std::cout << i << "packet sent: " << payload[0] << "\n" << std::endl;
         
        
        // Pass that packet to the decoder
        decoder->decode( &payload[0] );
        
        if (decoder->is_complete()) {
            break;
        }
        
    }
    

       
    
    Postoffice::closeConnection();
    
    
    // The decoder is complete, now copy the symbols from the decoder
    std::vector<uint8_t> data_out(decoder->block_size());
    kodo::copy_symbols(kodo::storage(data_out), decoder); 
    
    std::cout << "Recieved: " << data_out[0] << data_out[1] << data_out[2];
    
    // Check we properly decoded the data
    if (std::equal(data_out.begin(), data_out.end(), data_in.begin()))
    {
        std::cout << "Data decoded correctly" << std::endl;
    }
    else
    {
        std::cout << "Unexpected failure to decode "
        << "please file a bug report :)" << std::endl;
    }

    
    }

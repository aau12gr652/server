#include "test_video.hpp"

#define JONAS

int max_packet_size = 1450;

int main()
{
	std::ofstream statFileUEP;
	statFileUEP.open("uep.txt");
	statFileUEP << "%M, Generation, Generation size, Symbol size, Layers, Layer 1 size, Layer 2 size, Layer 3 size, Received packets from layer 1, Received packets from layer 2, Received packets from layer 3, Total frames, Layer 1 Frames, Layer 2 Frames, Layer 3 Frames" << std::endl;

	int Monte_Carlo = 100;
	for (int M = 0; M < Monte_Carlo; M++)
	{
		{
			std::cout << "M: " << M << std::endl;
			int GOPS = 2000;
			int Over_Head = 300;

			kodo_encoder foo=kodo_encoder();
			kodo_decoder poo=kodo_decoder();
			#ifdef JONAS
			video_getter hoo=video_getter("/Users/jonashansen/Desktop/Demo/ed_an_G20_2500k.avi");
			#else
			video_getter hoo=video_getter("/home/jeppe/Videos/ed_an_G20_3500k.avi");
			#endif
			std::cout << "Hurra, vi crashede ikke!" << std::endl;

		/*  // Example usage
			uint8_t* p = 0;
			uint32_t i_size = 0;
			i_size = hoo.get_gop(&p);

			uint32_t total_size = hoo.buffer_size;
			uint32_t total_buffers = hoo.serialized_buffer_table_out.size();

			std::cout << "i_size: " << i_size*1 << " totalsize: " << total_size << " " << total_buffers << std::endl;
			if(!p)
			{
				std::cout << "Error: p is 0" << std::endl;
				return 0;
			}
		*/
			serial_data Packet;


			for (int n = 0; n < GOPS; n++)
			{
				uint8_t* p = 0;
				uint32_t i_size = 0;

				i_size = hoo.get_gop(&p);
				if (i_size == 0)
					break;
				uint32_t total_size = hoo.buffer_size;
				uint32_t total_buffers = hoo.serialized_buffer_table_out.size();

				uint32_t gsize = calculate_generation_size_from_gop_size(total_size);
				uint32_t symbol_size = calculate_symbol_size_from_generation_size(gsize);

				the_new_make_layers(3, gsize, symbol_size, &foo, i_size);
				foo.new_generation((char*)p);

				int frames_1 = 0, frames_2 = 0, frames_3 = 0;
				for (int u = 0; u < total_buffers; u++)
					if (hoo.serialized_buffer_table_out[u] < foo.get_layer_size(1) * symbol_size)
						frames_1++;
					else if (hoo.serialized_buffer_table_out[u] < foo.get_layer_size(2) * symbol_size)
						frames_2++;
					else if (hoo.serialized_buffer_table_out[u] < foo.get_layer_size(3) * symbol_size)
						frames_3++;

				int Packet_Number, Packet_Number_1 = 0, Packet_Number_2 = 0, Packet_Number_3 = 0;
				for (Packet_Number = 0; Packet_Number < Over_Head * gsize; Packet_Number++)
				{
					Packet = foo.get_packet(); // Get a packet
					poo.decode(&foo.payload_stamp, Packet); // Decode the packet

					if (Packet_Number_1 == 0 && poo.is_layer_finish(1) == 1)
						Packet_Number_1 = Packet_Number+1;
					if (Packet_Number_2 == 0 && poo.is_layer_finish(2) == 1 && foo.get_layers() > 1)
						Packet_Number_2 = Packet_Number+1;
					if (Packet_Number_3 == 0 && poo.is_layer_finish(3) == 1 && foo.get_layers() > 2)
						Packet_Number_3 = Packet_Number+1;
					if (poo.has_finished_decoding())
						break; // No need to continue, since the data is all decoded
				}
				statFileUEP << M << ", " << n << ", " << gsize << ", " << symbol_size << ", " << foo.get_layers() << ", " << foo.get_layer_size(1) << ", " << foo.get_layer_size(2) << ", " << foo.get_layer_size(3) << ", " << Packet_Number_1 << ", " << Packet_Number_2 << ", " << Packet_Number_3 << ", " << total_buffers << ", " << frames_1 << ", " << frames_2 << ", " << frames_3 << std::endl;
			}
			statFileUEP.flush();
		}
	}
	statFileUEP.close();
    return 0;
}

uint32_t calculate_generation_size_from_gop_size(uint32_t gop_size)
{
    return std::ceil(4*max_packet_size-2*sqrt(4*(max_packet_size*max_packet_size)-2*gop_size) );
}

uint32_t calculate_symbol_size_from_generation_size(uint32_t generation_size)
{
    return max_packet_size-generation_size/8+1;
}

uint32_t calculate_layer_size(uint32_t layer_size)
{
    uint32_t addition = 0;
    if ( layer_size%8 ) addition = (8-layer_size%8);
    return layer_size + addition;
}

void the_new_make_layers(uint32_t nb_layers, uint32_t gsize, uint32_t symb_size, kodo_encoder* m_kodo_encoder, uint32_t i_size)
{
    m_kodo_encoder->set_generation_size(gsize);
    m_kodo_encoder->set_symbol_size(symb_size);

    uint32_t first_layer;
    first_layer = std::ceil(i_size/(float)symb_size);

    if (first_layer < 32) first_layer = 32; //puha
    else first_layer = calculate_layer_size(first_layer);

    if (first_layer >= gsize)
    {
        first_layer = gsize;
        nb_layers = 1;
    }

    m_kodo_encoder->set_layers(nb_layers);

    uint32_t first_gamma = std::ceil(first_layer*100/(float)gsize);
    m_kodo_encoder->set_layer_size(1,first_layer);
    m_kodo_encoder->set_layer_gamma(1,first_gamma);

    uint32_t p_layer_size;
	if (nb_layers > 1)
	    p_layer_size = std::ceil( (gsize-first_layer)/(float)(nb_layers-1) );
    for (uint32_t layer = 2; layer < nb_layers; layer++)
    {
        uint32_t this_layer_size = p_layer_size*(layer-1)+first_layer;
        this_layer_size = calculate_layer_size(this_layer_size);
        if (this_layer_size >= gsize)
        {
        	nb_layers = layer;
        	break;
        }
        m_kodo_encoder->set_layer_size(layer, this_layer_size);
        m_kodo_encoder->set_layer_gamma(layer, std::ceil(this_layer_size*100/(float)gsize) );
    }
    m_kodo_encoder->set_layers(nb_layers);
    m_kodo_encoder->set_layer_size(nb_layers,gsize);
    for (uint32_t layer = 1; layer <= nb_layers; layer++)
    {
    	int p = std::ceil(100/(float)nb_layers * layer);
	    m_kodo_encoder->set_layer_gamma(layer,p);
	}
	m_kodo_encoder->set_layer_gamma(nb_layers,100);
}

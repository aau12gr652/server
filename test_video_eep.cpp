#include "test_video.hpp"

#define JONAS

int max_packet_size = 1450;

int main()
{
	std::ofstream statFileEEP;
	statFileEEP.open("eep.txt");
	statFileEEP << "%M, Generation, Generation size, Symbol size, Packets, Frames" << std::endl;
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
			video_getter hoo=video_getter("/Users/jonashansen/Desktop/Demo/videos/ed_an_G20_2500k.avi");
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

				foo.set_layers(1);
				foo.set_layer_size(1, gsize);
				foo.set_layer_gamma(1, 100);
				foo.set_symbol_size(symbol_size);
				foo.new_generation((char*)p);

				int Packet_Number, Packet_Number_1 = 0, Packet_Number_2 = 0, Packet_Number_3 = 0;
				for (Packet_Number = 0; Packet_Number < Over_Head * gsize; Packet_Number++)
				{
					Packet = foo.get_packet(1); // Get a packet
					poo.decode(&foo.payload_stamp, Packet); // Decode the packet

					if (poo.has_finished_decoding())
						break; // No need to continue, since the data is all decoded
				}
				statFileEEP << M << ", " << n << ", " << gsize << ", " << symbol_size << ", " << Packet_Number+1 << ", " << total_buffers << std::endl;
			}
			statFileEEP.flush();
		}
	}
	statFileEEP.close();
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

#include <kodo/rlnc/full_vector_codes.h>

#include <postoffice/Postoffice.h>
#include "kodo_encoder.h"
#include <fstream>
int main(int argc, char *argv[])
{
    kodo_encoder foo=kodo_encoder();
    postoffice po("4000", "255.255.255.255");

    if (argc == 1)
	{
		char* msg[3];
		msg[0] = "ABCDE, world!1";
		msg[1] = "FGHIJ, world!2";
		msg[2] = "KLMNO, world!3";
		foo.set_layers(2);
		foo.set_generation_size(14);
		foo.set_layer_size(1, 5);
		foo.set_layer_size(2, 14);
		foo.set_layer_gamma(1, 50);
		foo.set_layer_gamma(2, 100);
		foo.set_symbol_size(1);

		for (int u = 0; u < 3; u++)
		{
			foo.new_generation(msg[u]);
	//		for (int n = 0; n < 20; n++)
	//			foo.get_packet(2);
			for (int n = 0; n < 9; n++)
			{
				foo.send_packet(po, 2);
				std::cout << "Send packet: " << n  << " from layer: " << foo.payload_stamp.Layer_ID*1 << " Generation: " << foo.payload_stamp.Generation_ID*1 << std::endl;
			}
			for (int n = 0; n < 5; n++)
			{
	//			serial_data packet = foo.get_packet(1);
	//			po.send(packet, &foo.payload_stamp);
				foo.send_packet(po, 1);
				std::cout << "Send packet: " << n  << " from layer: " << foo.payload_stamp.Layer_ID*1 << " Generation: " << foo.payload_stamp.Generation_ID*1 << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Jeg sender lige mega meget data" << std::endl;
		foo.set_layers(2);
		foo.set_generation_size(1400);
		foo.set_layer_size(1, 450);
		foo.set_layer_size(2, 1400);
		foo.set_layer_gamma(1, 50);
		foo.set_layer_gamma(2, 100);
		foo.set_symbol_size(1282);
		for (int u = 0; u < 3; u++)
		{
			char* data = new char[1400*1400];
			foo.new_generation(data);
			for (int n = 0; n < 1403; n++)
			{
				foo.send_packet(po);
				print_stamp(&foo.payload_stamp);
			}
			free(data);
		}
	}

    po.closeConnection();
    return 0;
}

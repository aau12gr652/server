#include <kodo/rlnc/full_vector_codes.h>

#include "../postoffice/Postoffice.h"
#include "encoder.h"

int main()
{
    encoder foo=encoder();
    postoffice po("4000", "255.255.255.255");

    char* msg[3];
    msg[0] = "Hello, world!1";
    msg[1] = "World, hello!2";
    msg[2] = "Hello, world!3";
    foo.set_layers(2);
    foo.set_generation_size(14);
    foo.set_layer_size(0, 5);
    foo.set_layer_size(1, 14);
    foo.set_layer_gamma(0, 0);
    foo.set_layer_gamma(1, 100);
    foo.set_symbol_size(1);

	for (int u = 0; u < 3; u++)
	{
		foo.new_generation(msg[u]);
		//std::cout << "Done preparing GENENERATION: " << foo.payload_stamp.Generation_ID*1 << std::endl;
		for (int n = 0; n < 20; n++)
		{
			serial_data packet = foo.get_packet();
			po.send(packet, &foo.payload_stamp);
			//std::cout << "Send packet from layer: " << foo.payload_stamp.Layer_ID*1 << std::endl;
			usleep(500);
		}
	}

    po.closeConnection();
    return 0;
}

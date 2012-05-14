#include "video_getter.hpp"

video_getter::video_getter(const char* filename)
{
	hwood_src = new hollywood_source;
	int errorcode = 0;
	errorcode = hwood_src->set_file(filename);
	if (errorcode) std::cout << "unable to set file in hollywood_source" << std::endl;
	
	buster = new blockbuster(false);
	hwood_src->signal_video_packet.connect(  )
	data_ptr = 0;
	buffer_ready = false;
}

video_getter::~video_getter()
{
	if(hwood_src) delete hwood_src;
	if(buster) delete buster;
	if(slizer) delete slizer;
	if(data_ptr) delete data_ptr;
}

void video_getter::prepare_avpacket_for_encoder(AVPacket* pkt)
{
	if(pkt->flags == AV_PKT_FLAG_KEY) // Packet contains keyframe:
	{
		if(slizer.size()) // If serializer contains data:
		{
			std::vector<uint8_t>& serialized_buffer = slizer->serialize(); // Because of the new keyframe, serializer contains one GOP. Flag this as ready.
			if(data_ptr) delete data_ptr;
			data_ptr = new uint8_t[serialized_buffer.size()]
			memcpy(data_ptr, &serialized_buffer[0], serialized_buffer.size);
			buffer_ready = true;
		}
	}
	serialize_avpacket(pkt);
}

void video_getter::serialize_avpacket(AVPacket* pkt)
{
	//Get index in serializer where start of packet is:
	uint32_t index = slizer->feed(pkt->data, pkt->size);
	serialized_buffer_table.push_back(index);
} 

uint32_t video_getter::get_gop(uint8_t **new_ptr)
{
	while (!buffer_ready)
	{
		hwood_src->demux();
	}
	*new_ptr = data_ptr;
	return serialized_buffer_table[1];
}





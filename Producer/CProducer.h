#pragma once

#include <librdkafka/rdkafkacpp.h>

namespace KafkaProducer {

	using namespace RdKafka;

	class CProducer
	{
		Conf* _conf;
		Producer* _producer;
		std::vector<Topic*> _topics;

	public:
		CProducer() {}
		CProducer(std::string brokerList, std::vector<std::string> topics);
		~CProducer();

		void Produce(std::string topic, std::string key, void* payload, size_t payload_len);
	};

}
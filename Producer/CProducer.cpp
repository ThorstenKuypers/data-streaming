#include "CProducer.h"

#include <iostream>

using namespace KafkaProducer;

CProducer::CProducer(std::string brokerList, std::vector<std::string> topics)
{
	std::string errstr;
	_topics = std::vector<Topic*>{};

	_conf = Conf::create(Conf::CONF_GLOBAL);
	if (_conf == nullptr) {
		auto ex = std::exception(errstr.c_str());
		std::cerr << "[ERROR]: config creation failed!" << std::endl;

		throw ex;
	}

	auto res = _conf->set("bootstrap.servers", brokerList, errstr);
	if (res != Conf::ConfResult::CONF_OK) {
		auto ex = std::exception(errstr.c_str());
		std::cerr << "[ERROR]: setting broker list failed!" << std::endl;

		delete _conf;
		_conf = nullptr;
		throw ex;
	}

	res = _conf->set("client.id", "Producer-ID", errstr);
	if (res != Conf::ConfResult::CONF_OK) {
		auto ex = std::exception(errstr.c_str());
		std::cerr << "[ERROR]: setting client id failed!" << std::endl;

		delete _conf;
		_conf = nullptr;
		throw ex;
	}

	_producer = Producer::create(_conf, errstr);
	if (_producer == nullptr) {
		std::cerr << "[ERROR]: Producer creation failed!" << std::endl;

		delete _conf;
		_conf = nullptr;

		throw std::exception(errstr.c_str());
	}

	auto tconf = Conf::create(Conf::ConfType::CONF_TOPIC);
	for (auto& s : topics) {
		auto topic = Topic::create(_producer, s, tconf, errstr);
		if (topic == nullptr) {
			std::cerr << "[ERROR]: creating topic \"" << s << "\" failed!" << std::endl;
			continue;
		}

		_topics.push_back(topic);
	}

	if (tconf != nullptr) {
		delete tconf;
		tconf = nullptr;
	}
}


CProducer::~CProducer()
{
	_producer->flush(1000);

	if (_topics.size() != 0) {
		for (auto& t : _topics) {
			if (t != nullptr) {
				delete t;
				t = nullptr;
			}
		}
	}

	if (_producer != nullptr) {
		_producer->poll(1000);

		delete _producer;
		_producer = nullptr;
	}

	if (_conf != nullptr) {
		delete _conf;
		_conf = nullptr;
	}

	wait_destroyed(5000);
}

void KafkaProducer::CProducer::Produce(std::string topic, std::string key, void * payload, size_t payload_len)
{
	Topic* top = nullptr;

	for (auto& t : _topics) {
		if (t->name() == topic) {
			top = t;
		}
	}

	if (top == nullptr) {
		throw std::exception("[ERROR]: unknown topic!");
	}

	auto res = _producer->produce(top, 0, Producer::RK_MSG_COPY, payload, payload_len, &key, NULL);
	if (res != ERR_NO_ERROR) {
		std::cerr << "[ERROR]: Producer::Produce()" << std::endl;

		throw std::exception(err2str(res).c_str());
	}
}


#define LIBRDKAFKACPP_EXPORTS 

#include <Windows.h>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

using namespace RdKafka;
using namespace std;

int main(void)
{
	string errstr;

	auto conf = Conf::create(Conf::CONF_GLOBAL);
	auto tconf = Conf::create(Conf::CONF_TOPIC);

	auto ret = conf->set(string("bootstrap.servers"), string("172.20.12.227:9092"), errstr);

	auto consumer = Consumer::create(conf, errstr);
	if (!consumer) {
		cout << "Consumer creation failed: " << errstr << endl;
		return -1;
	}
	Sleep(1000);

	auto topic = Topic::create(consumer, string("My-Topic"), tconf, errstr);
	auto res = consumer->start(topic, 0, 0);
	if (res != ERR_NO_ERROR) {
		cout << "Consumer::Start() failed!" << endl;
		delete topic;
		delete consumer;

		return -1;
	}
	while (true) {
		auto msg = consumer->consume(topic, 0, 1000);
		if (!msg->err()) {
			char* ptr = static_cast<char*>(msg->payload());
			if (ptr != nullptr)
			{
				string s{ ptr };
				cout << "Message received -- Key: " << msg->key() << " - Value: " << s << endl;
			}
		}

		delete msg;
	}
	system("PAUSE");

	delete topic;
	delete consumer;

	RdKafka::wait_destroyed(2000);

	return 0;
}
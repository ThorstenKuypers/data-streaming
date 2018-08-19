

#define LIBRDKAFKACPP_EXPORTS 

#include <Windows.h>
#include <iostream>
#include <rdkafkacpp.h>

using namespace RdKafka;
using namespace std;


class ErrorEventCb :public EventCb {

	virtual void event_cb(Event& event) {

		if (event.type() == Event::EVENT_ERROR) {
			if (event.err() != ERR_NO_ERROR) {
				cout << "Error: " << event.err() << endl;
			}
		}
	}
};

class DelReportCb :public DeliveryReportCb
{
	virtual void dr_cb(Message& msg)
	{
		int x = 0;
	}
};

int main(void)
{
	ErrorEventCb errCb{};
	DelReportCb dr_cb{};

	string errstr;
	auto conf = Conf::create(Conf::ConfType::CONF_GLOBAL);
	auto ret = conf->set(string("bootstrap.servers"), string("172.20.12.233:9092"), errstr);
	ret = conf->set("event_cb", &errCb, errstr);
	ret = conf->set("dr_cb", &dr_cb, errstr);

	auto prod = Producer::create(conf, errstr);
	if (prod == nullptr)
	{
		cout << "Producer failed: " << errstr << endl;
		return -1;
	}

	auto cfg = conf->dump();
	cout << "config:" << endl;
	for (auto& s : *cfg) {
		cout << s << endl;
	}

	auto res = prod->produce(string("Topic-2"), 0, Producer::RK_MSG_COPY, (void*)"Hello Kafka World!", strlen("Hello Kafka World!")+1, NULL, 0, 0, NULL);
	if (res != ERR_NO_ERROR) {
		cout << "Produce failed: " << err2str(res) << endl;

		return -1;
	}
	Sleep(1000);
	prod->flush(1000);	
	int n = prod->poll(0);
	system("PAUSE");
}
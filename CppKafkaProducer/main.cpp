
#define CPPKAFKA_STATIC

#include <cppkafka/cppkafka.h>

using namespace cppkafka;

int main(void)
{
	Configuration cfg = { {"metadata.broker.list", "172.20.12.227:9092"} };

	Producer prod{ cfg };
	std::string msg = "hey there!";
	prod.produce(MessageBuilder("my-topic").partition(0).payload(msg));

	Sleep(1000);

	prod.flush();

	system("PAUSE");

	return 0;
}
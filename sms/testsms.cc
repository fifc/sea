
// 
// Akago Inc.
// Steen Yi
// 2015-12
//

#include "send_sms.h"
#include <iostream>

int main(int argc, char *argv[]) {
	std::vector<sms::SmsMsg> msgs;
	std::string phone(argc > 1 ? argv[1] : "  86 186 0300 0506");
	msgs.emplace_back(phone, "您好，您的验证码是12345");
	msgs.emplace_back(phone, "您好，您的验证码是67890");
	if (sms::SendSM(msgs) == 0) {
		std::cout << "ok\n";
	} else {
		std::cout << "error\n";
	}
}

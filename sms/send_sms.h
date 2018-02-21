
// 
// Akago Inc.
// Steen Yi
// 2015-12
//

#include <string>
#include <vector>

namespace sms {
struct SmsMsg {
	std::string phone_;
	std::string msg_;
	SmsMsg(const std::string& phone, const std::string& msg): phone_(phone), msg_(msg) {}
};

int SendSM(const std::string& phone, const std::string& msg);
int SendSM(const std::vector<SmsMsg>& msgs);
} // namespace sms

#include <iostream>
#include <string>

void SendSms(const std::string& number, const std::string& message) {
    std::cout << "Send '" << message << "' to number " << number << std::endl;
}

void SendEmail(const std::string& email, const std::string& message) {
    std::cout << "Send '" << message << "' to e-mail "  << email << std::endl;
}

class NotifierBase {
    private:
        std::string addr;
    public:
        NotifierBase(std::string addr_): addr(addr_){};
        virtual ~NotifierBase() = default;
        virtual void Notify(std::string masage) const = 0;
        std::string GetName() const{
            return addr;
        }
};

class SmsNotifier: public NotifierBase {
    public:
        SmsNotifier(std::string addr_): NotifierBase(addr_) {};
        ~SmsNotifier() override = default;
        void Notify(std::string masage) const override;
};

void SmsNotifier::Notify(std::string masage) const {
    SendSms(this->GetName(), masage);
}

class EmailNotifier: public NotifierBase {
    public:
        EmailNotifier(std::string addr_): NotifierBase(addr_) {};
        ~EmailNotifier() = default;
        void Notify(std::string message) const override;
};

void EmailNotifier:: Notify(std::string message) const {
    SendEmail(this->GetName(), message);
}


void Notify(const NotifierBase& notifier, const std::string& message) {
    notifier.Notify(message);
}

int main() {
    SmsNotifier sms("+7-495-777-77-77");
    EmailNotifier email("na-derevnyu@dedushke.ru");

    Notify(sms, "Hello! How are you?");
    Notify(email, "Let's learn C++!");
    return 0;
}

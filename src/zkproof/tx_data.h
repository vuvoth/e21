#include <string>

using String = std::string;
namespace e21 {
class TxData {
private:
  String message;
public:
  TxData(String message) : message(message) {}
  
  String getMessage() const { return message; }
};
} // namespace e21

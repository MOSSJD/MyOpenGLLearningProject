#include <string>
#include "Util.h"
void ME::removeBOM(std::string& string) {
	if (string.size() >= 3 &&
		(unsigned char)string.at(0) == 0xEF &&
		(unsigned char)string.at(1) == 0xBB &&
		(unsigned char)string.at(2) == 0xBF) {
		string.erase(0, 3); // 移除BOM
	}
}
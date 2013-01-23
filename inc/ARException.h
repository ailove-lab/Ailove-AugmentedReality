#ifndef _AREXCEPTION_H
#define _AREXCEPTION_H

#include <string>

class ARException {
private:
    std::string message;
public:
    static ARException CAN_NOT_INIT_CAMERA;
    static ARException CAN_NOT_INIT_TEXTURE;

    ARException(const std::string& message);
    ~ARException(void);

    std::string getMessage() {
        return message;
    }
};

#endif


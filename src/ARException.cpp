#include "ARException.h"

ARException ARException::CAN_NOT_INIT_CAMERA("Не получается активировать камеру");
ARException ARException::CAN_NOT_INIT_TEXTURE("Ваша видеокарта не поддерживает необходимую функциональность");

//-------------------------------------------------------------------------------------

ARException::ARException(const std::string& message)
{
    this->message = message;
}

//-------------------------------------------------------------------------------------

ARException::~ARException(void)
{
}

//-------------------------------------------------------------------------------------

#include "ARException.h"

ARException ARException::CAN_NOT_INIT_CAMERA("Can not init camera!");
ARException ARException::CAN_NOT_INIT_TEXTURE("Can not init texture!");

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

#include "ARException.h"

ARException ARException::CAN_NOT_INIT_CAMERA("�� ���������� ������������ ������");
ARException ARException::CAN_NOT_INIT_TEXTURE("���� ���������� �� ������������ ����������� ����������������");

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

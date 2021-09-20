/*
	GT_521F.h v1.0 - Library for controlling the GT_521F Finger Print Scanner (FPS)
	Created by Nicholas Hosein and Christian Bararn, 2020
	Licensed for non-commercial use, must include this license message
	basically, Feel free to hack away at it, but just give me credit for my work =)
  Subscribe to my Youtube Channel
	www.youtube.com/c/justbarran
*/


#include "GT_521F.h"

/********************************** PUBLIC ***********************************/

GT_521F::GT_521F(HardwareSerial& serial) { _hwS = &serial; serialType = 0;}

GT_521F::GT_521F(SoftwareSerial& serial) { _SwS = &serial; serialType = 1;}

bool GT_521F::begin(uint32_t baud) {
  if(serialType == 0)
  {
    _s = _hwS;
    if(baud == 9600 || baud == 19200 || baud == 38400 ||
        baud == 57600 || baud == 115200 || baud == 230400) {
      // Auto-detect baud rate
      uint8_t baudDelay = 10;
      _hwS->begin(9600);
      delay(baudDelay);
      if(close() != 0) {
        _hwS->flush();
        _hwS->begin(19200);
        delay(baudDelay);
        if(close() != 0) {
          _hwS->flush();
          _hwS->begin(38400);
          delay(baudDelay);
          if(close() != 0) {
            _hwS->flush();
            _hwS->begin(57600);
            delay(baudDelay);
            if(close() != 0) {
              _hwS->flush();
              _hwS->begin(115200);
              delay(baudDelay);
              if(close() != 0) {
                _hwS->flush();
                _hwS->begin(230400);
                delay(baudDelay);
                if(close() != 0) {
                  return false;
                }
              }
            }
          }
        }
      }
      if(changeBaudRate(baud) == 0) {
        _hwS->flush();
        _hwS->begin(baud);
        return true;
      }
    } 
  }
  else
  {
    _s = _SwS;
    _SwS->begin(baud);
    return true;
  }
  return false;
}

uint16_t GT_521F::open(bool info) {
  // Send command packet
  if(!_sendCommand(Open, info)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() != Ack) return ACK_ERROR;
  // If extra data requested
  if(info) {
    // Read extra data into SRAM buffer
    resStatus = _readResponse(30);
    if(resStatus != NO_ERROR) return resStatus;
  }

  return NO_ERROR;
}

uint16_t GT_521F::close(void) {
  // Send command packet
  if(!_sendCommand(Close)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() != Ack) return ACK_ERROR;

  return NO_ERROR;
}

uint16_t GT_521F::usbInternalCheck(void) {
  // Send command packet
  if(!_sendCommand(UsbInternalCheck)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() != Ack) return ACK_ERROR;
  if(_getResParameter() != 0x55) return PARAMETER_ERROR;

  return NO_ERROR;
}

uint16_t GT_521F::changeBaudRate(uint32_t baud) {
  // Send command packet
  if(!_sendCommand(ChangeBaudRate, baud)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack) {
    return NO_ERROR;
  } else if(_getResponse() == Nack) {
    if(_getResParameter() == NACK_INVALID_PARAM) {
      return NACK_INVALID_PARAM;
    } else {
      return PARAMETER_ERROR;
    }
  } else {
    return ACK_ERROR;
  }
}

uint16_t GT_521F::cmosLed(bool ledOn) {
  // Send command packet
  if(!_sendCommand(CmosLed, ledOn)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() != Ack) return ACK_ERROR;

  return NO_ERROR;
}

uint16_t GT_521F::isPressFinger(void) {
  // Send command packet
  if(!_sendCommand(IsPressFinger)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() != Ack) return ACK_ERROR;
  if(_getResParameter() == 0) {
    return FINGER_IS_PRESSED;
  } else {
    return FINGER_IS_NOT_PRESSED;
  }
}

uint16_t GT_521F::getEnrollCount(void) {
  // Send command packet
  if(!_sendCommand(GetEnrollCount)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() != Ack) return ACK_ERROR;
  return _getResParameter();
}

uint16_t GT_521F::checkEnrolled(uint16_t id){
  // Send command packet
  if(!_sendCommand(CheckEnrolled,id)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack){
    return ID_IS_ENROLLED;
  }
  else{
    return _getResParameter();
  }
}

uint16_t GT_521F::enrollStart(uint16_t id){
  // Send command packet
  if(!_sendCommand(EnrollStart,id)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack){
    return NO_ERROR;
  }
  else {
    return _getResParameter();
  }
}

uint16_t GT_521F::enrollFinger(uint8_t num){
  // Send command packet
  if(num==1)
  {
    if(!_sendCommand(Enroll1)) return SEND_ERROR;
  }
  else if(num==2)
  {
    if(!_sendCommand(Enroll2)) return SEND_ERROR;

  }
  else if(num==3)
  {
    if(!_sendCommand(Enroll3)) return SEND_ERROR;
  }  
  else
  {
    return PARAMETER_ERROR;
  }
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack){
    return NO_ERROR;
  }
  else {
    return _getResParameter();
  }
}


uint16_t GT_521F:: captureFinger(bool best) {
  // Send command packet
  if(!_sendCommand(CaptureFinger, best)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack) {
    return NO_ERROR;
  } else if(_getResponse() == Nack) {
    if(_getResParameter() == NACK_FINGER_IS_NOT_PRESSED) {
      return NACK_FINGER_IS_NOT_PRESSED;
    } else {
      return PARAMETER_ERROR;
    }
  } else {
    return ACK_ERROR;
  }
}

uint16_t GT_521F::deleteID(uint16_t id){
  // Send command packet
  if(!_sendCommand(DeleteID,id)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack){
    return NO_ERROR;
  }
  else {
    return _getResParameter();
  }
}

uint16_t GT_521F::deleteAll(void){
  // Send command packet
  if(!_sendCommand(DeleteAll)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack){
    return NO_ERROR;
  }
  else {
    return _getResParameter();
  }
}

uint16_t GT_521F::verify(uint16_t id){
  // Send command packet
  if(!_sendCommand(Verify,id)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack){
    return NO_ERROR;
  }
  else {
    return _getResParameter();
  }
}

uint16_t GT_521F::identify(void){
  // Send command packet
  if(!_sendCommand(Identify)) return SEND_ERROR;
  // Read response
  uint16_t resStatus = _readResponse(12);
  if(resStatus != NO_ERROR) return resStatus;
  if(_getResponse() == Ack){
    return _getResParameter();
  }
  else if (_getResParameter() == NACK_INVALID_POS || _getResParameter() == NACK_IS_NOT_USED){
    return _getResParameter();
  }
  else{
    return PARAMETER_ERROR;
  }
}


/*****************************************************************************/
/********************************** PRIVATE **********************************/
/*****************************************************************************/

bool GT_521F::_sendCommand(uint16_t cmd, uint32_t par) {
  // Build packet
  _buildCmdPacket(cmd, par);
  // Flush receive buffer
  while(_s->available() > 0) uint8_t tmp = _s->read();
  // Send packet
  _s->flush();
  uint8_t bytesSent = _s->write(cmdPacket, 12);
  if(bytesSent != 12) {
    return false;
  }
  return true;
}

void GT_521F::_buildCmdPacket(uint16_t cmd, uint32_t par) {
  // Build packet
  uint16_t cmd_word = cmd;
  cmdPacket[0] = COMMAND_START_CODE_1;
  cmdPacket[1] = COMMAND_START_CODE_2;
  cmdPacket[2] = DEVICE_ID_1;
  cmdPacket[3] = DEVICE_ID_2;
  cmdPacket[4] = (par & 0x000000FF);
  cmdPacket[5] = (par & 0x0000FF00) >> 8;
  cmdPacket[6] = (par & 0x00FF0000) >> 16;
  cmdPacket[7] = (par & 0xFF000000) >> 24;
  cmdPacket[8] = (uint8_t)(cmd_word & 0x00FF);
  cmdPacket[9] = (uint8_t)(cmd_word >> 8) & 0x00FF;
  // Calculate checksum - byte addition
  uint16_t checksum_word = 0;
  for(uint8_t i=0; i<10; i++) {
    checksum_word += cmdPacket[i];
  }
  cmdPacket[10] = (uint8_t)(checksum_word & 0x00FF);
  cmdPacket[11] = (uint8_t)(checksum_word >> 8) & 0x00FF;
}

uint16_t GT_521F::_readResponse(uint16_t numExpectBytes) {
  uint8_t readByte;
  uint16_t numReadBytes = 0;
  uint16_t dataIndex = 0;
  uint16_t checksum_cal = 0;
  _PacType pacType = _PacType::INVALID;
  
  while(numReadBytes < numExpectBytes) {
    uint32_t startTime = millis();
    while(!_s->available()) {
      uint32_t currentTime = millis();
      if(currentTime - startTime >= READ_TIMEOUT_MS) return RESPONSE_TIMEOUT;
    }
    readByte = _s->read();
    numReadBytes++;

    if(numReadBytes != 1 && numReadBytes != 2 &&
        numReadBytes != 3 && numReadBytes != 4 &&
        numReadBytes < (numExpectBytes-1)) {
      checksum_cal += readByte;
      dataBuf[dataIndex] = readByte;
      dataIndex++;
    } else if(numReadBytes == 1) {
      checksum_cal += readByte;
      if(readByte == RESPONSE_START_CODE_1) {
        pacType = _PacType::RESPONSE;
      } else if(readByte == DATA_START_CODE_1) {
        pacType = _PacType::DATA;
      } else {
        return RESPONSE_START_INVALID;
      }
    } else if(numReadBytes == 2) {
      checksum_cal += readByte;
      if(readByte == RESPONSE_START_CODE_2) {
        if(pacType != _PacType::RESPONSE) {
          return RESPONSE_START_INVALID;
        }
      } else if(readByte == DATA_START_CODE_2) {
        if(pacType != _PacType::DATA) {
          return RESPONSE_START_INVALID;
        }
      } else {
        return RESPONSE_START_INVALID;
      }
    } else if(numReadBytes == 3) {
      checksum_cal += readByte;
      if(readByte != DEVICE_ID_1) {
        return RESPONSE_DEVICE_ID_INVALID;
      }
    } else if(numReadBytes == 4) {
      checksum_cal += readByte;
      if(readByte != DEVICE_ID_2) {
        return RESPONSE_DEVICE_ID_INVALID;
      }
    } else if(numReadBytes == numExpectBytes-1) {
      if(readByte != (uint8_t)(checksum_cal & 0x00FF)) {
        return RESPONSE_CHECKSUM_INVALID;
      }
    } else if(numReadBytes == numExpectBytes) {
      if(readByte != (uint8_t)(checksum_cal >> 8) & 0x00FF) {
        return RESPONSE_CHECKSUM_INVALID;
      }
    } else {
      return RESPONSE_INVALID;
    }
  }
  
  switch(pacType) {
    case _PacType::RESPONSE :
      // Set parameter
      parameter = 0;
      parameter = (parameter << 8) + dataBuf[3];
      parameter = (parameter << 8) + dataBuf[2];
      parameter = (parameter << 8) + dataBuf[1];
      parameter = (parameter << 8) + dataBuf[0];
      // Set response
      response = 0;
      response = (response << 8) + dataBuf[5];
      response = (response << 8) + dataBuf[4];
      return NO_ERROR;
    case _PacType::DATA :
      return NO_ERROR;
    default :
      return RESPONSE_INVALID;
  }
}

uint32_t GT_521F::_getResParameter(void) {
  return parameter;
}

uint16_t GT_521F::_getResponse(void) {
  return response;
}

bool GT_521F::_sendData(const uint8_t data[], uint16_t numDataBytes) {
  // Build packet
  _buildDataPacket(data, numDataBytes);
  // Flush receive buffer
  while(_s->available() > 0) uint8_t tmp = _s->read();
  // Send packet
  _s->flush();
  uint16_t bytesSent = _s->write(dataBuf, numDataBytes);
  if(bytesSent != numDataBytes) {
    return false;
  }
  return true;
}

void GT_521F::_buildDataPacket(const uint8_t data[], uint16_t numDataBytes) {
  // Build packet
  dataBuf[0] = DATA_START_CODE_1;
  dataBuf[1] = DATA_START_CODE_2;
  dataBuf[2] = DEVICE_ID_1;
  dataBuf[3] = DEVICE_ID_2;
  for(uint16_t i=4; i<numDataBytes+4; i++) {
    dataBuf[i] = dataBuf[i-4];
  }
  // Calculate checksum - byte addition
  uint16_t checksum_word = 0;
  for(uint16_t i=0; i<numDataBytes+4; i++) {
    checksum_word += dataBuf[i];
  }
  dataBuf[numDataBytes+4] = (uint8_t)(checksum_word & 0x00FF);
  dataBuf[numDataBytes+5] = (uint8_t)(checksum_word >> 8) & 0x00FF;
}

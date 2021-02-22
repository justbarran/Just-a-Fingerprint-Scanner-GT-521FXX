/*
	GT_521F.h v1.0 - Library for controlling the GT_521F32/GT_521F52 Finger Print Scanner (FPS)
	Created by Nicholas Hosein and Christian Barran, Feb 1st 2021
	Licensed for non-commercial use, must include this license message
	basically, Feel free to hack away at it, but just give me credit for my work =)
  Subscribe to my Channel
  https://www.youtube.com/c/justbarran

*/



#ifndef GT_521F_h
#define GT_521F_h

#include <Arduino.h>
#include <HardwareSerial.h>

#define DATA_BUFFER_SIZE 32

#define READ_TIMEOUT_MS 1000

/** @brief  Status codes
 *  @note   0x0xxx valid codes
 *  @note   0x1xxx nack error codes
 *  @note   0x2xxx non-nack error codes
 * 
 * 
*/


enum StatusEnum {
  NO_ERROR                    = 0x0000,
  FINGER_IS_PRESSED           = 0x0001,
  FINGER_IS_NOT_PRESSED       = 0x0002,  
  ID_IS_ENROLLED              = 0x0003,
  ID_IS_NOT_ENROLLED          = 0x0004,

  NACK_TIMEOUT                = 0x1001,
  NACK_INVALID_BAUDRATE       = 0x1002,
  NACK_INVALID_POS            = 0x1003,
  NACK_IS_NOT_USED            = 0x1004,
  NACK_IS_ALREADY_USED        = 0x1005,
  NACK_COMM_ERR               = 0x1006,
  NACK_VERIFY_FAILED          = 0x1007,
  NACK_IDENTIFY_FAILED        = 0x1008,
  NACK_DB_IS_FULL             = 0x1009,
  NACK_DB_IS_EMPTY            = 0x100A,
  NACK_TURN_ERR               = 0x100B,
  NACK_BAD_FINGER             = 0x100C,
  NACK_ENROLL_FAILED          = 0x100D,
  NACK_IS_NOT_SUPPORTED       = 0x100E,
  NACK_DEV_ERR                = 0x100F,
  NACK_CAPTURE_CANCELED       = 0x1010,
  NACK_INVALID_PARAM          = 0x1011,
  NACK_FINGER_IS_NOT_PRESSED  = 0x1012,

  SEND_ERROR                  = 0x2001,
  RESPONSE_INVALID            = 0x2002,
  RESPONSE_TIMEOUT            = 0x2003,
  RESPONSE_START_INVALID      = 0x2004,
  RESPONSE_DEVICE_ID_INVALID  = 0x2005,
  RESPONSE_CHECKSUM_INVALID   = 0x2006,
  ACK_ERROR                   = 0x2007,
  PARAMETER_ERROR             = 0x2008
};

class GT_521F {
  public:
    /** @brief  Instanciate GT_521F object
     *  @param  serial Hardware serial port object
    */
    GT_521F(HardwareSerial& serial);
    
    /** @brief  Initialize serial communication
     *  @param  baud Serial baud rate. Default = 9600.
     *               Valid = 9600, 19200, 38400, 57600, 115200, 230400
     *  @return <i>bool</i> On success; true. On failure; false
    */
    bool begin(uint32_t baud = 9600);

    /** @brief  Initialize GT_521F fingerprint sensor
     *  @param  info Extra information request. Default = false
     *  @return <i>uint16_t</i> On success; 0 or extra info (if requested).
     *          On failure; status code
     *  @note   Extra info stored in SRAM buffer. Use getData to retrieve
     *  @note   Extra info structure:
     *          byte 0 - Firmware version byte 3
     *          byte 1 - Firmware version byte 2
     *          byte 2 - Firmware version byte 1
     *          byte 3 - Firmware version byte 0
     *          byte 4 - Iso area max size byte 3
     *          byte 5 - Iso area max size byte 2
     *          byte 6 - Iso area max size byte 1
     *          byte 7 - Iso area max size byte 0
     *          bytes 8-23 - Device serial number
     *  @note   Iso area max size should always be 0KB
     *  @note   If serial number is zero, no guarantee for stable operation
     */
    uint16_t open(bool info = false);

    /** @brief  Termination
     *  @return <i>uint16_t</i> On success; 0. On failure; status code
     *  @note   Does nothing
    */
    uint16_t close(void);

    /** @brief  Fast searching of the device
     *  @return <i>uint16_t</i> On success; 0. On failure; status code
     *  @note   The device operates as removable CD drive. If another
     *          removable CD drive exists in the system, connection time
     *          may be long. To prevent this, usbInternalCheck is used for
     *          fast searching of the device
    */
    uint16_t usbInternalCheck(void);

    /** @brief  Changes UART baud rate at run-time
     *  @param  baud Serial baud rate. Default = 9600
     *  @return <i>uint16_t</i> On success; 0. On failure; status code
     *  @note   The device initializes its baud to 9600 after power-up
    */
    uint16_t changeBaudRate(uint32_t baud = 9600);

    /** @brief  Control CMOS LED
     *  @param  ledOn LED on = true. LED off = false. Default = false
     *  @return <i>uint16_t</i> On success; 0. On failure; status code
     *  @note   Default state of LED is off but during boot it blinks once
    */
    uint16_t cmosLed(bool ledOn = false);

    /** @brief  Check finger pressing status 
     *  @return <i>uint16_t</i> On success/failure; status code
     *  @note   Used during enrollment, the host waits to take off the
     *          finger per enrollment stage
    */
    uint16_t isPressFinger(void);

    /** @brief  Capture fingerprint
     *  @param  best Slow/Best image = true. Fast = false. Default = 1
     *  @return <i>uint16_t</i> On success; 0. On failure; status code
     *  @note   Captures raw image from sensor and converts it to 256x256
     *          image for fingerprint algorithm
     *  @note   Use slow image for enrollment to get best enrollment data
     *  @note   Use fast image for verification to get fast sensibility
    */
    uint16_t captureFinger(bool best = false);


    /** @brief  Get Enroll Count
     *  @return <i>uint16_t</i> On success; The total number of enrolled fingerprints
     *           On failure; status code 
     *  
    */
    uint16_t getEnrollCount(void);


    /** @brief  Check Enrollment Status
     *  @param  Enrolled ID you want to check
     *  @return <i>uint16_t</i> On success; ID_IS_ENROLLED
     *           On failure; status code 
     *  
    */
    uint16_t checkEnrolled(uint16_t id);


    /** @brief  Start an enrollment
    *  @param  Enrolled ID you want to check
    *  @return <i>uint16_t</i> On success; 0
    *           On failure; status code 
    *  
    */
    uint16_t enrollStart(uint16_t id);


    /** @brief  Make template for an enrollment
    *  @param  Enroll template number
    *  @return <i>uint16_t</i> On success; 0
    *           On failure; status code 
    *  @note   param can only be 1,2,3 anything else would result in error
    */
    uint16_t enrollFinger(uint8_t num);


    /** @brief  Delete one fingerprint
    *  @param  Enrolled ID you want to delete
    *  @return <i>uint16_t</i> On success; 0
    *           On failure; status code 
    */
    uint16_t deleteID(uint16_t id);

    /** @brief  Delete All fingerprints
    *  @return <i>uint16_t</i> On success; 0
    *           On failure; status code 
    */
    uint16_t deleteAll(void);

    /** @brief  Checks the currently pressed finger against a specific ID
    *  @param  id number to be checked
    *  @return <i>uint16_t</i> On success; 0
    *           On failure; status code 
    * @note   0-2999, if using GT-521F52
    *         0-199, if using GT-521F32/GT-511C3
    */
    uint16_t verify(uint16_t id);

    /** @brief  Checks the currently pressed finger against all enrolled fingerprints
    *  @return <i>uint16_t</i> On success; id number
    *           On failure; status code  > 2999
    */
    uint16_t identify(void);


    //ALOT MORE TO DO STAY TUNED

    /** SRAM buffer */
    uint8_t dataBuf[DATA_BUFFER_SIZE];

  private:
    // Packet type enum
    enum class _PacType {
      INVALID  = 0x00,
      COMMAND  = 0x01,
      RESPONSE = 0x02,
      DATA     = 0x03,
    };
    // Command enum
    enum _CmdEnum {
      NotSet              = 0x00,
      Open                = 0x01,
      Close               = 0x02,
      UsbInternalCheck    = 0x03,
      ChangeBaudRate      = 0x04,
      CmosLed             = 0x12,
      GetEnrollCount      = 0x20,
      CheckEnrolled       = 0x21,
      EnrollStart         = 0x22,
      Enroll1             = 0x23,
      Enroll2             = 0x24,
      Enroll3             = 0x25,
      IsPressFinger       = 0x26,
      DeleteID            = 0x40,
      DeleteAll           = 0x41,
      Verify              = 0x50,
      Identify            = 0x51,
      VerifyTemplate      = 0x52,
      IdentifyTemplate    = 0x53,
      CaptureFinger       = 0x60,
      MakeTemplate        = 0x61,
      GetImage            = 0x62,
      GetRawImage         = 0x63,
      GetTemplate         = 0x70,
      SetTemplate         = 0x71,
      GetDatabaseStart    = 0x72,
      GetDatabaseEnd      = 0x73,
      SetSecurityLevel    = 0xF0,
      GetSecurityLevel    = 0xF1,
      Identify_Template2  = 0xF4,
      EnterStandbyMode    = 0xF9
    };
    // Response enum
    enum _ResEnum { 
      Ack  = 0x30,
      Nack = 0x31
    };

    Stream* _s;
    HardwareSerial* _hwS;
    
    static const uint8_t DEVICE_ID_1 = 0x01;
    static const uint8_t DEVICE_ID_2 = 0x00;
    // Command
    static const uint8_t COMMAND_START_CODE_1 = 0x55;
    static const uint8_t COMMAND_START_CODE_2 = 0xAA;
    uint8_t cmdPacket[12];
    bool _sendCommand(uint16_t cmd, uint32_t par = 0);
    void _buildCmdPacket(uint16_t cmd, uint32_t par);
    // Response
    static const uint8_t RESPONSE_START_CODE_1 = 0x55;
    static const uint8_t RESPONSE_START_CODE_2 = 0xAA;
    static const uint8_t RESPONSE_ACK = 0x30;
    static const uint8_t RESPONSE_NACK = 0x31;
    uint32_t parameter;
    uint16_t response;
    uint16_t _readResponse(uint16_t numExpectBytes);
    uint32_t _getResParameter(void);
    uint16_t _getResponse(void);
    // Data
    static const uint8_t DATA_START_CODE_1 = 0x5A;
    static const uint8_t DATA_START_CODE_2 = 0xA5;
    bool _sendData(const uint8_t data[], uint16_t numDataBytes);
    void _buildDataPacket(const uint8_t data[], uint16_t numDataBytes);
};

#endif

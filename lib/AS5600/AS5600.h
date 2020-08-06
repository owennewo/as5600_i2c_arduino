 
#include <Arduino.h>
#include <Wire.h>
 
#define AS5600_I2C_ADDR 0x36

// CONGIG REGISTERS
#define REG_ZMCO 0x00
#define REG_ZPOS 0x01
#define REG_MPOS 0x03
#define REG_MANG 0x05
#define REG_CONF 0x07
// OUTPUT REGISTERS
#define REG_RAW_ANGLE 0x0C
#define REG_ANGLE 0x0E
// STATUS REGISTERS
#define REG_STATUS 0x0B
#define REG_AGC 0x1A
#define REG_MAGNITUDE 0x1B
// BURN COMMANDS
#define REG_BURN 0xFF

#define BURN_ANGLE 0x80
#define BURN_SETTINGS 0x40 

struct AS5600_Status {
    bool magnetHigh; // AGC minimum gain overflow, magnet too strong
    bool magnetLow;  // AGC maximum gain overflow, magnet too weak
    bool magnetDetected; 
};

struct AS5600_Config {
    uint8_t powerMode; // 00 = NOM, 01 = LPM1, 10 = LPM2, 11 = LPM3
    uint8_t hysteresis; // 00 = OFF, 01 = 1 LSB, 10 = 2 LSBs, 11 = 3 LSBs
    uint8_t outputStage; // 00 = analog (full range from 0% to 100% between GND and VDD, 01 = analog(reduced range from 10% to 90% between GND and VDD, 10 = digital PWM
    uint8_t pwmFrequency; // 00 = 115 Hz; 01 = 230 Hz; 10 = 460 Hz; 11 = 920 Hz
    uint8_t slowFilter; // 00 = 16x; 01 = 8x; 10 = 4x; 11 = 2x
    uint8_t fastFilterThreshold; // 000 = slow filter only, 001 = 6 LSBs, 010 = 7 LSBs, 011 = 9 LSBs,100 = 18 LSBs, 101 = 21 LSBs, 110 = 24 LSBs, 111 = 10 LSBs
    bool watchdog;  // 0 = OFF, 1 = ON
};

class AS5600 {
    public:
        AS5600();
        AS5600(uint8_t i2cAddress);

        // if you need to change config you can either write to registers every time arduino starts up or burn the values.  
        // Burning the values can only be done 3 times.  This method give how many times config has been burnt 
        uint8_t getBurnCount();
        // I think angle == rawAngle if you don't play with start pos, end pos, max angle
        uint16_t getAngle();
        uint16_t getRawAngle();
        // default read == (unset?)
        uint16_t getStartPos();
        // defaults to 0 (unset?).  I think EndPos or MaxAngle can be set
        uint16_t getEndPos();
        // defaults to 0 (unset?).  I think EndPos or MaxAngle can be set
        uint16_t getMaxAngle();
        AS5600_Config getConfig();
        void setConfig(AS5600_Config config);
        AS5600_Status getStatus();
        // from 0-255.  ideally you would adjust your air gap distance so the gain ~128
        // my skinny 6mm x 1mm diametric magnet was nearly always 255. It dropped to 200 when touching.  
        // Should probably use suggested 6mm x 2.5 diametric magnet.
        uint8_t getAutomaticGainControl();
        // the strength of the magnetic field.  Could be used to detect if magnet is missing (value hovers around 5 when missing and should be 1000+ if present)
        uint16_t getMagnitude();
        void setStartPos(uint16_t value);
        void setEndPos(uint16_t value);
        void setMaxAngle(uint16_t value);

        /****** HERE BE DRAGONS!  Easy to permanently misconfigure your device and probably not essential (you can call setXXX in your setup() so why burn?) *****/
        // This burns previously set REG_ZPOS and REG_MPOS to non-volatile memory. You have 3 burns! 
        void burnAngle();
        // This burns REG_MANG and REG_CONFIG to non-volatile memory. You have 1 burn!
        void burnSettings();
        
    private:
        uint8_t i2cAddress;
        uint16_t readTwoBytes(uint8_t addr);
        uint8_t readByte(uint8_t addr);
        void writeByte(uint8_t addr, uint8_t value);
        void writeTwoBytes(uint8_t addr, uint16_t value);
    };
# RAK4631_AES

This is a demo for my extension to the [Adafruit_nRFCrypto](https://github.com/Kongduino/Adafruit_nRFCrypto), which adds AES encryption. It adds a class, nRFCrypto_AES:

```c
class nRFCrypto_AES {
  public:
    nRFCrypto_AES(void);
    bool begin(void);
    void end(void);
    int Process(char *msg, uint8_t msgLen, uint8_t *IV, uint8_t *pKey, uint8_t pKeyLen,
                   char *retBuf, SaSiAesEncryptMode_t modeFlag, SaSiAesOperationMode_t opMode);
    SaSiAesEncryptMode_t encryptFlag = (SaSiAesEncryptMode_t) 0; // SASI_AES_ENCRYPT
    SaSiAesEncryptMode_t decryptFlag = (SaSiAesEncryptMode_t) 1; // SASI_AES_DECRYPT
    SaSiAesOperationMode_t ecbMode = (SaSiAesOperationMode_t) 0; // SASI_AES_MODE_ECB
    SaSiAesOperationMode_t cbcMode = (SaSiAesOperationMode_t) 1; // SASI_AES_MODE_CBC
  private:
    bool _begun;
    SaSiAesPaddingType_t _paddingType = (SaSiAesPaddingType_t) 0; // SASI_AES_PADDING_NONE
    SaSiAesKeyType_t _userKey = (SaSiAesKeyType_t) 0; // SASI_AES_USER_KEY
};
```

The `Process` function does everything for you: ECB/CBC, and encrypt/decrypt.

## Installation

Remove (and possibly save somewhere) the original version of the Adafruit library, and install mine. Since the only thing it does is **add** things, without removing anything, it is safe to use as a replacement. After that you use the usual `#include "Adafruit_nRFCrypto.h"` statement, and create an `nRFCrypto_AES` object. The sample code here shows how to do all 4 operations.

```c
void testAES() {
  nRFCrypto.begin();
  nRFCrypto_AES aes;
  if (!aes.begin()) {
    Serial.println("Could not start AES!");
    return;
  }
  uint8_t pKey[16] = {0xC6, 0xBB, 0x2D, 0x94, 0x90, 0xB9, 0x65, 0x23, 0x98, 0xED, 0x83, 0x3F, 0x9B, 0x9D, 0x02, 0xCC};
  char pDataIn[65] = "Ca va chez vous les enfants...? Sinon il faut me le dire, hein ?";
  char pDataOut[65] = {0};
  char pDataRe[65] = {0};
  size_t dataOutBuffSize;
  Serial.println("AES test");
  int err = aes.Process(pDataIn, 64, myIV, pKey, 16, pDataOut, aes.encryptFlag, aes.ecbMode);
  // myIV is not needed since it's ECB, but we need the parameter.
  // Could pass just about anything
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataOut, 64);
  }
  err = aes.Process(pDataOut, 64, myIV, pKey, 16, pDataRe, aes.decryptFlag, aes.ecbMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataRe, 64);
  }
  uint8_t myIV[16] =  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  memset(pDataOut, 0, 65);
  err = aes.Process(pDataIn, 64, myIV, pKey, 16, pDataOut, aes.encryptFlag, aes.cbcMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataOut, 64);
  }
  err = aes.Process(pDataOut, 64, myIV, pKey, 16, pDataRe, aes.decryptFlag, aes.cbcMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataRe, 64);
  }

  nRFCrypto.end();
}
```
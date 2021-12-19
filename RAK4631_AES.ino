#include "Adafruit_nRFCrypto.h"
#include <Adafruit_TinyUSB.h> // for Serial

void hexDump(char *buf, int len) {
  char alphabet[17] = "0123456789abcdef";
  printf("%s\n", "   +------------------------------------------------+ +----------------+");
  printf("%s\n", "   |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f | |      ASCII     |");
  for (int i = 0; i < len; i += 16) {
    if (i % 128 == 0)
      printf("%s\n", "   +------------------------------------------------+ +----------------+");
    char s[] = "|                                                | |................|";
    char ix = 1, iy = 52;
    for (char j = 0; j < 16; j++) {
      if (i + j < len) {
        char c = buf[i + j];
        s[ix++] = alphabet[(c >> 4) & 0x0F];
        s[ix++] = alphabet[c & 0x0F];
        ix++;
        if ((int)c > 31 && (int)c < 128) s[iy++] = c;
        else s[iy++] = '.';
      }
    }
    char index = i / 16;
    if (i < 256) printf("%s", " ");
    printf("%x.", index);
    printf("%s\n", s);
  }
  printf("%s\n", "   +------------------------------------------------+ +----------------+");
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  while (!Serial) delay(10);
  delay(1000);
  testAES();
}

void loop() {
}

void testAES() {
  nRFCrypto.begin();
  nRFCrypto_AES aes;
  if (!aes.begin()) {
    Serial.println("Could not start AES!");
    return;
  }
  uint8_t myIV[16] =  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  uint8_t pKey[16] = {0xC6, 0xBB, 0x2D, 0x94, 0x90, 0xB9, 0x65, 0x23, 0x98, 0xED, 0x83, 0x3F, 0x9B, 0x9D, 0x02, 0xCC};
  char pDataIn[65] = "Ca va chez vous les enfants...? Sinon il faut me le dire, hein ?";
  char pDataOut[65] = {0};
  char pDataRe[65] = {0};
  size_t dataOutBuffSize;
  Serial.println("AES test");
  Serial.println("======================");
  Serial.println(" * ECB");
  Serial.println("======================");
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
  Serial.println("======================");
  Serial.println(" * CBC");
  Serial.println("======================");
  memset(pDataOut, 0, 65);
  memset(pDataRe, 0, 65);
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
  Serial.println("======================");
  Serial.println(" * CTR");
  Serial.println("======================");
  memset(pDataOut, 0, 65);
  memset(pDataRe, 0, 65);
  err = aes.Process(pDataIn, 64, myIV, pKey, 16, pDataOut, aes.encryptFlag, aes.ctrMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataOut, 64);
  }
  err = aes.Process(pDataOut, 64, myIV, pKey, 16, pDataRe, aes.decryptFlag, aes.ctrMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataRe, 64);
  }

  nRFCrypto.end();
}

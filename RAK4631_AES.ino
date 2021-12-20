#include <Adafruit_TinyUSB.h> // for Serial
#include "Adafruit_nRFCrypto.h"
//#include "ecc/nRFCrypto_ECC.h"
#include "nRF52840_DHKE.h"

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
  Serial.println("AES test");
  nRFCrypto.begin();
  nRFCrypto_AES aes;
  if (!aes.begin()) {
    Serial.println("Could not start AES!");
    return;
  }
  Serial.println("======================");
  Serial.println(" * Random");
  Serial.println("======================");
  uint8_t myIV[16];
  nRFCrypto.Random.generate(myIV, 16);
  Serial.println("myIV:");
  hexDump((char*)myIV, 16);
  uint8_t pKey[16];
  nRFCrypto.Random.generate(pKey, 16);
  Serial.println("pKey:");
  hexDump((char*)pKey, 16);
  char pDataIn[65] = "Ca va chez vous les enfants...? Sinon il faut me le dire, hein ?";
  char smallData[24] = {
    0xb8, 0x4d, 0x39, 0x1a, 0x03, 0x3a, 0x0f, 0xed, 0x82, 0x97, 0x0d, 0x05,
    0xb8, 0x4d, 0x39, 0x1a, 0x03, 0x3a, 0x0f, 0xed, 0x82, 0x97, 0x0d, 0x05
  };
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

  Serial.println(" * Small Blocks");
  Serial.println("----------------------");
  memset(pDataOut, 0, 65);
  memset(pDataRe, 0, 65);
  hexDump(smallData, 12);
  err = aes.Process(smallData, 12, myIV, pKey, 16, pDataOut, aes.encryptFlag, aes.ecbMode);
  // myIV is not needed since it's ECB, but we need the parameter.
  // Could pass just about anything
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataOut, 16);
  }
  err = aes.Process(pDataOut, 16, myIV, pKey, 16, pDataRe, aes.decryptFlag, aes.ecbMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataRe, 12);
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

  Serial.println(" * Small Blocks");
  Serial.println("----------------------");
  memset(pDataOut, 0, 65);
  memset(pDataRe, 0, 65);
  hexDump(pDataIn, 12);
  err = aes.Process(pDataIn, 12, myIV, pKey, 16, pDataOut, aes.encryptFlag, aes.cbcMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataOut, 16);
  }
  err = aes.Process(pDataOut, 16, myIV, pKey, 16, pDataRe, aes.decryptFlag, aes.cbcMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataRe, 12);
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

  Serial.println(" * Small Blocks");
  Serial.println("----------------------");
  memset(pDataOut, 0, 65);
  memset(pDataRe, 0, 65);
  hexDump(pDataIn, 17);
  err = aes.Process(pDataIn, 17, myIV, pKey, 16, pDataOut, aes.encryptFlag, aes.ctrMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataOut, 32);
  }
  err = aes.Process(pDataOut, 64, myIV, pKey, 16, pDataRe, aes.decryptFlag, aes.ctrMode);
  if (err < 0) {
    Serial.println("Error " + String(err));
  } else {
    hexDump(pDataRe, 17);
  }

  // This breaks the code hard...
  // nRFCrypto_ECC_PrivateKey AlicePrivate, BobPrivate;
  // nRFCrypto_ECC_PublicKey  AlicePublic,  BobPublic;
  // nRFCrypto_ECC ecc;
  // if (!ecc.begin()) {
  //   Serial.println("ECC init error.");
  // }
  // ^____ Even these 6 lines alone cause a hard fault...
  // ecc.genKeyPair(AlicePrivate, AlicePublic);
  // ecc.genKeyPair(BobPrivate, BobPublic);
  // uint8_t secret0[32];
  // uint8_t secret1[32];
  // ecc.SVDP_DH(AlicePrivate, BobPublic, (uint8_t*)&secret0, 32);
  // ecc.SVDP_DH(AlicePrivate, BobPublic, (uint8_t*)&secret1, 32);
  // bool same = true;
  // for (uint8_t ix = 0; ix < 32; ix++) {
  //   if (secret0[ix] != secret1[ix]) {
  //     same = false;
  //     break;
  //   }
  // }
  // if (same) Serial.println("SVDP_DH passed!");
  // else Serial.println("SVDP_DH failed!");

  Serial.println("======================");
  Serial.println(" * DHKE");
  Serial.println("======================");
  nrf52_DHKE_Set Alice;
  nrf52_DHKE_Set Bob;
  nrf52_DHKE_Set Carl;
  if (!Alice.begin()) {
    Serial.println("Error initing Alice!");
    return;
  }
  if (!Bob.begin()) {
    Serial.println("Error initing Bob!");
    return;
  }
  if (!Carl.begin()) {
    Serial.println("Error initing Bob!");
    return;
  }

  Serial.println(" . Alice:");
  Serial.println("     Public Key:");
  hexDump((char*)Alice.getPub().oneChunk, 32);
  Serial.println("     Private Key:");
  hexDump((char*)Alice.getPvt().oneChunk, 32);

  Serial.println(" . Bob:");
  Serial.println("     Public Key:");
  hexDump((char*)Bob.getPub().oneChunk, 32);
  Serial.println("     Private Key:");
  hexDump((char*)Bob.getPvt().oneChunk, 32);

  Serial.println(" . Carl:");
  Serial.println("     Public Key:");
  hexDump((char*)Carl.getPub().oneChunk, 32);
  Serial.println("     Private Key:");
  hexDump((char*)Carl.getPvt().oneChunk, 32);

  memset(pDataOut, 0, 65);
  memset(pDataRe, 0, 65);
  Serial.println(" . Alice:");
  Serial.println("----------------------");
  Serial.println("> Plaintext:");
  hexDump(pDataIn, 64);
  Serial.println("> Encryption:");
  err = Alice.encrypt(pDataIn, 64, pDataOut, Bob.getPub());
  if (err == 0) {
    hexDump(pDataOut, 64);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }
  Serial.println("> Decryption:");
  err = Alice.decrypt(pDataOut, 64, pDataRe, Bob.getPub());
  if (err == 0) {
    hexDump(pDataRe, 64);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }
  memset(pDataRe, 0, 65);
  Serial.println("> Carl Snooping In:");
  err = Carl.decrypt(pDataOut, 64, pDataRe, Bob.getPub());
  if (err == 0) {
    hexDump(pDataRe, 64);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }

  memset(pDataOut, 0, 65);
  memset(pDataRe, 0, 65);
  Serial.println(" . Bob:");
  Serial.println("----------------------");
  Serial.println("> Plaintext:");
  hexDump(pDataIn, 64);
  Serial.println("> Encryption:");
  err = Bob.encrypt(pDataIn, 64, pDataOut, Alice.getPub());
  if (err == 0) {
    hexDump(pDataOut, 64);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }
  Serial.println("> Decryption:");
  err = Bob.decrypt(pDataOut, 64, pDataRe, Alice.getPub());
  if (err == 0) {
    hexDump(pDataRe, 64);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }
  memset(pDataRe, 0, 65);
  Serial.println("> Carl Snooping In:");
  err = Carl.decrypt(pDataOut, 64, pDataRe, Bob.getPub());
  if (err == 0) {
    hexDump(pDataRe, 64);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }

  nRFCrypto.end();
}

//////////////////////////////
/////     GPS             ////
//////////////////////////////

uint32_t parsedecimal(char *str) {
  uint32_t d = 0;

  while (str[0] != 0) {
    if ((str[0] > '9') || (str[0] < '0'))
      return d;
    d *= 10;
    d += str[0] - '0';
    str++;
  }
  return d;
}

void readline(void) {
  char c;

  buffidx = 0; // start at begninning
  while (1) {
    c=mySerial.read();
    if (c == -1)
      continue;
    Serial.print(c);
    if (c == '\n')
      continue;
    if ((buffidx == BUFFSIZ-1) || (c == '\r')) {
      buffer[buffidx] = 0;
      return;
    }
    buffer[buffidx++]= c;
  }
}



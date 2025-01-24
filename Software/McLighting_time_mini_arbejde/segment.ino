// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t rainbow(byte WheelPos) {
  Serial.println(WheelPos);
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    main_color.red = 255 - WheelPos * 3;
    main_color.green = 0;
    main_color.blue = WheelPos * 3;
    return 1;
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    main_color.red = 0;
    main_color.green = WheelPos * 3;
    main_color.blue = 255 - WheelPos * 3;
    return 1;
  }
  WheelPos -= 170;
  main_color.red = WheelPos * 3;
  main_color.green = 255 - WheelPos * 3;
  main_color.blue = 0;
  return 1;
}

void select_number(int number, int segment[]){
  if (number == 1)
    number_1(segment);
    
  else if (number == 2)
    number_2(segment);
    
  else if (number == 3)
    number_3(segment);
    
  else if (number == 4)
    number_4(segment);
    
  else if (number == 5)
    number_5(segment);
    
  else if (number == 6)
    number_6(segment);

  else if (number == 7)
    number_7(segment);
    
  else if (number == 8)
    number_8(segment);
    
  else if (number == 9)
    number_9(segment);
    
  else if (number == 0)
    number_0(segment);
    
  }

void number_1(int segment[]) {
  segment_a(0,segment);
  segment_b(1,segment);
  segment_c(1,segment);
  segment_d(0,segment);
  segment_e(0,segment);
  segment_f(0,segment);
  segment_g(0,segment);
  //pixels.show();
  }

void number_2(int segment[]) {
  segment_a(1,segment);
  segment_b(1,segment);
  segment_c(0,segment);
  segment_d(1,segment);
  segment_e(1,segment);
  segment_f(0,segment);
  segment_g(1,segment);
  //pixels.show();
  }

void number_3(int segment[]) {
  segment_a(1,segment);
  segment_b(1,segment);
  segment_c(1,segment);
  segment_d(1,segment);
  segment_e(0,segment);
  segment_f(0,segment);
  segment_g(1,segment);
  //pixels.show();
  }

void number_4(int segment[]) {
  segment_a(0,segment);
  segment_b(1,segment);
  segment_c(1,segment);
  segment_d(0,segment);
  segment_e(0,segment);
  segment_f(1,segment);
  segment_g(1,segment);
  //pixels.show();
  }

void number_5(int segment[]) {
  segment_a(1,segment);
  segment_b(0,segment);
  segment_c(1,segment);
  segment_d(1,segment);
  segment_e(0,segment);
  segment_f(1,segment);
  segment_g(1,segment);
  //pixels.show();
  }

void number_6(int segment[]) {
  segment_a(1,segment);
  segment_b(0,segment);
  segment_c(1,segment);
  segment_d(1,segment);
  segment_e(1,segment);
  segment_f(1,segment);
  segment_g(1,segment);
  //pixels.show();
  }

void number_7(int segment[]) {
  segment_a(1,segment);
  segment_b(1,segment);
  segment_c(1,segment);
  segment_d(0,segment);
  segment_e(0,segment);
  segment_f(0,segment);
  segment_g(0,segment);
  //pixels.show();
  }

void number_8(int segment[]) {
  segment_a(1,segment);
  segment_b(1,segment);
  segment_c(1,segment);
  segment_d(1,segment);
  segment_e(1,segment);
  segment_f(1,segment);
  segment_g(1,segment);
  //pixels.show();
  }

void number_9(int segment[]) {
  segment_a(1,segment);
  segment_b(1,segment);
  segment_c(1,segment);
  segment_d(1,segment);
  segment_e(0,segment);
  segment_f(1,segment);
  segment_g(1,segment);
  //pixels.show();
  }

void number_0(int segment[]) {
  segment_a(1,segment);
  segment_b(1,segment);
  segment_c(1,segment);
  segment_d(1,segment);
  segment_e(1,segment);
  segment_f(1,segment);
  segment_g(0,segment);
  //pixels.show();
  }



void segment_a(bool state, int segment[]) {
  if (state){
    pixels.setPixelColor(segment[0], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[1], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[2], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
  }
  else {
    pixels.setPixelColor(segment[0], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[1], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[2], pixels.Color(0,0,0)); // Moderately bright green color.
  }
}

void segment_b(bool state, int segment[]) {
  if (state){
    pixels.setPixelColor(segment[3], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[4], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[5], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
  }
  else {
    pixels.setPixelColor(segment[3], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[4], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[5], pixels.Color(0,0,0)); // Moderately bright green color.
  }
}

void segment_c(bool state, int segment[]) {
  if (state){
    pixels.setPixelColor(segment[6], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[7], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[8], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
  }
  else {
    pixels.setPixelColor(segment[6], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[7], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[8], pixels.Color(0,0,0)); // Moderately bright green color.
  }
}

void segment_d(bool state, int segment[]) {
  if (state){
    pixels.setPixelColor(segment[9], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[10], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[11], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
  }
  else {
    pixels.setPixelColor(segment[9], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[10], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[11], pixels.Color(0,0,0)); // Moderately bright green color.
  }
}

void segment_e(bool state, int segment[]) {
  if (state){
    pixels.setPixelColor(segment[12], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[13], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[14], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
  }
  else {
    pixels.setPixelColor(segment[12], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[13], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[14], pixels.Color(0,0,0)); // Moderately bright green color.
  }
}

void segment_f(bool state, int segment[]) {
  if (state){
    pixels.setPixelColor(segment[15], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[16], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[17], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
  }
  else {
    pixels.setPixelColor(segment[15], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[16], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[17], pixels.Color(0,0,0)); // Moderately bright green color.
  }
}

void segment_g(bool state, int segment[]) {
  if (state){
    pixels.setPixelColor(segment[18], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[19], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
    pixels.setPixelColor(segment[20], pixels.Color(Red,Green,Blue)); // Moderately bright green color.
  }
  else {
    pixels.setPixelColor(segment[18], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[19], pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.setPixelColor(segment[20], pixels.Color(0,0,0)); // Moderately bright green color.
  }
}

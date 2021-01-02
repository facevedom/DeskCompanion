// this is a wrapper for display functions to declutter main .ino file

void Display::initialize() {
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  DEBUG_PRINTLN("Display initialized");
}

void Display::welcome() {
    display.drawString(0, 0, "Desk companion started!");
    display.display();
}

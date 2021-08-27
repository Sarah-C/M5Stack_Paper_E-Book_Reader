/*
 * Prototype E-Book reader.
 * 
 * It reads a single book on the root of the Micro SD card, called "Book.txt".
 * 
 * The jog wheel moves forward and backward between pages, and pressing it turns the M5 Paper on/off. (Hold for a few seconds to do either)
 * 
 * The page you're on is saved to the SD card, rather than the EEPROM, so as not to wear it out.
 * 
 */



#include <M5EPD.h>
#include "Free_Fonts.h"

M5EPD_Canvas canvas(&M5.EPD);
const char *DATA_FILE = "/pageNumber.txt";

int currentPage = 0;

int pageCount = 0;
int border = 10;
uint8_t *textFile;

rtc_time_t RTCtime;
rtc_date_t RTCDate;

struct aPage {
  uint32_t start;
  uint32_t end;
} pages[2000];

String textSubstring(uint8_t *textFile, int startPtr, int endPtr) {
  String s = "";
  for (int i = startPtr; i < endPtr; i++) {
    s += (char) textFile[i];
  }
  return s;
}

int textIndexOfSpaceCR(uint8_t *textFile, int startPtr, int textLength) {
  for (int ptr = startPtr; ptr < textLength; ptr++) {
    if ( (textFile[ptr] == 32) || (textFile[ptr] == 13) ) {
      return ptr;
    }
  }
  return -1;
}

bool reachedEndOfBook(int wordStart, int textLength) {
  return wordStart >= textLength;
}

void storePageSD(uint32_t c) {
  auto f = SD.open(DATA_FILE, "wb");
  uint8_t buf[4];
  buf[0] = c;
  buf[1] = c >> 8;
  buf[2] = c >> 16;
  buf[3] = c >> 24;
  auto bytes = f.write(&buf[0], 4);
  f.close();
}

uint32_t getPageSD() {
  uint32_t val;
  if (SD.exists(DATA_FILE)) {
    auto f = SD.open(DATA_FILE, "rb");
    val = f.read();
    f.close();
  }
  else {
    val = 0;
  }
  return val;
}

int getNextPage(uint8_t *textFile, int startPtr, int textLength) {
  int wordStart = 0;
  int wordEnd = 0;
  int xPos = 0;
  int yPos = 0;
  int xMax = 540 - (border << 1);
  int yMax = 960 - (border << 1);
  while ( !reachedEndOfBook(wordStart + startPtr, textLength - 500) ) {
    // Get the end of the current word.
    wordEnd = textIndexOfSpaceCR(textFile, startPtr + wordStart + 1, textLength) - startPtr;
    // Measure the text.
    String text = textSubstring(textFile, startPtr + wordStart, startPtr + wordEnd);
    int textPixelLength = canvas.textWidth(text);
    // If the line of text with the new word over-runs the side of the screen,
    if ((xPos + textPixelLength >= xMax) || (text.charAt(0) == 13) ) {
      xPos = 0;
      yPos += 22;  //canvas.fontHeight();
      wordStart++; // Miss out space as this is a new line.
      //if ( ( yPos + canvas.fontHeight() ) >= yMax ) {
      if ( ( yPos + 90 ) >= yMax ) {
        if (pageCount > 0) {
          Serial.print(" New PAGE: ");
        }
        return startPtr + wordStart;
      }
    }
    xPos += textPixelLength;
    wordStart = wordEnd;
  }
  return textLength;
}

void findPageStartStop(uint8_t *textFile, int textLength) {
  int startPtr = 0;
  int endPtr = 0;
  while ( (endPtr < textLength) && (textFile[endPtr] != 0) ) {
    endPtr = getNextPage(textFile, startPtr, textLength);
    if (startPtr >= textLength) break;
    pages[pageCount].start = startPtr;
    pages[pageCount].end = endPtr;
    pageCount++;
    startPtr = endPtr;
  }
}

void displayPage(uint8_t *textFile, aPage page) {
  canvas.setCursor(0, 0);
  String text = textSubstring(textFile, page.start, page.end);
  int wordStart = 0;
  int wordEnd = 0;
  while ( (text.indexOf(' ', wordStart) >= 0) && ( wordStart <= text.length())) {
    wordEnd = text.indexOf(' ', wordStart + 1);
    uint16_t len = canvas.textWidth(text.substring(wordStart, wordEnd));
    if (canvas.getCursorX() + len >= canvas.width() - (border << 1)) {
      canvas.println();
      wordStart++;
    }
    canvas.print(text.substring(wordStart, wordEnd));
    wordStart = wordEnd;
  }

  char timeStrbuff[64];
  M5.RTC.getTime(&RTCtime);
  M5.RTC.getDate(&RTCDate);

  sprintf(timeStrbuff, "%d/%02d/%02d...%02d:%02d:%02d",
          RTCDate.year, RTCDate.mon, RTCDate.day,
          RTCtime.hour, RTCtime.min, RTCtime.sec);

  canvas.drawRightString(timeStrbuff, 200, 955, 1);  
  char footer[256] = "...";
  sprintf(footer, "Page %d of %d  ", currentPage + 1, pageCount);
  canvas.drawRightString(footer, 500, 955, 1);
  canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
}

void setup() {
  M5.begin();
  M5.EPD.SetRotation(90);
  M5.EPD.Clear(true);
  canvas.createCanvas(540, 960);
  canvas.fillCanvas(0);
  canvas.setFreeFont(FSS9);
  //canvas.setFreeFont(FSS12);
  canvas.setTextSize(1);
  canvas.setTextColor(15);
  canvas.setTextWrap(true, true);
  canvas.setCursor(0, 0);
  canvas.setTextArea(border, border, 540 - (border << 1), 960 - (border << 1));

  textFile = (uint8_t*)ps_malloc(1048576); // 1MB
  auto f = SD.open("/book.txt", "rb");
  int fileLength = f.available();
  Serial.print("File length: ");
  Serial.println(fileLength);
  f.read(textFile, f.available());
  Serial.println("Loaded.");

  Serial.println("Splitting into pages.");
  findPageStartStop(textFile, fileLength);

  currentPage = getPageSD();
  displayPage(textFile, pages[currentPage]);
}

void loop() {
  delay(500);
  M5.update();
  if (M5.BtnL.wasPressed()) {
    if (--currentPage < 0) currentPage = 0;
    canvas.fillCanvas(0);
    canvas.setCursor(0, 0);
    displayPage(textFile, pages[currentPage]);
  }
  if (M5.BtnP.wasPressed()) {
    storePageSD(currentPage);
    canvas.drawRightString("Off", 250, 955, 1);
    canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
    delay(1000);
    M5.shutdown();
  }
  if (M5.BtnR.wasPressed()) {
    if (++currentPage == pageCount) currentPage = pageCount - 1;
    canvas.fillCanvas(0);
    canvas.setCursor(0, 0);
    displayPage(textFile, pages[currentPage]);
  }

}

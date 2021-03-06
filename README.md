# M5Stack Paper: E-Book reader
This is a very simple prototype E-Book reader for the M5Stack Paper.                       
https://shop.m5stack.com/products/m5paper-esp32-development-kit-v1-1-960x540-4-7-eink-display-235-ppi

It comes with a small sample of the start of Robopocalypse as a test. It's worth a read, it's a good book.       
If you like it, please support the author and buy the full book.                 
https://en.wikipedia.org/wiki/Robopocalypse                 
https://www.amazon.com/dp/B007YATMOE/                   
https://www.amazon.co.uk/Robopocalypse-Contemporaries-Daniel-H-Wilson-ebook                      

This E-Book/E-Reader a **prototype** so please note, it's very limited, for instance:           
1. Uses only 1 font.
2. Loads a single book "book.txt" from the root of your SD card.
3. Pages don't always exactly end in the same place at the bottom of the screen.

**Features_____**                   
1. Navigation by the jog wheel.                
Left: Previous page.           
Right: Next page.               
Push (hold for 4 seconds): On / Off                
(The Paper shows "Off" at the bottom center of the screen when it's off, but still displays the current page. No "Off" screen yet. =)  )
2. Remembers your page in the book for when you power back up.
3. Fast page drawing.
4. Date/Time and "page of pages" bar at the bottom.

I've also coded it to be accessible to all devs - I've not split it out into classes or used much in the way of dynamic allocation or pointer/reference useage, it's just a single familiar Arduino .ino file. It's quite short too. That means adding features like touch navigation, SD card browsing, font changing - can all be experimented with, while avoiding plunging headlong into polymorphism, encapsulation, multiple inheritence, abstract classes, pure virutal functions, advanced data abstraction, templates and so on. (yikes!)

**Features for the future:______**
1. A Micro SD card browser to select the book you want to read.
2. A font changing option. Just remember the startIndex of the page, then re-parse the book page start/stop positions in the new font size, make a note of the page that coincides where startIndex is, and display that page. 
3: A screen tap navigation option, to type the page number to go to.
4: WiFi book upload/download. Reading place synchronisation between M5 Paper.
5: Support for Mobi, EPub... etc....
6: Support for images, and page covers.

**Program issues/situation_____**
1. M5EPD.h doesn't expose TFT_eSPI's "fontHeight()" function, so it's hard coded for the current font. It could find this out by displaying two lines of text, and seeing what the canvas.getCursorY(); jumps to when the 2nd line's drawn.                 
2. The program parses the book, finding where the start/stop character index of each page is when it first loads by keeping track of an xPos/yPos variable and measuring lines of text (that AREN'T actually rendered, just measured). This doesn't QUITE map to the display function, so the end of each page doesn't quite stop at the same place at the bottom of the page. It needs improving!


<p align="center" width="100%">
    <img width="33%" src="https://user-images.githubusercontent.com/1586332/131074409-0bc8815d-7d46-448b-8ae6-1407bf49db1f.jpg">
</p>




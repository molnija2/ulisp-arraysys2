/*
 User Extensions
*/

// Definitions
object *fn_now (object *args, object *env) {
  (void) env;
  static unsigned long Offset;
  unsigned long now = millis()/1000;
  int nargs = listlength(args);

  // Set time
  if (nargs == 3) {
    Offset = (unsigned long)((checkinteger(first(args))*60 + checkinteger(second(args)))*60
      + checkinteger(third(args)) - now);
  } else if (nargs > 0) error2(PSTR("wrong number of arguments"));
  
  // Return time
  unsigned long secs = Offset + now;
  object *seconds = number(secs%60);
  object *minutes = number((secs/60)%60);
  object *hours = number((secs/3600)%24);
  return cons(hours, cons(minutes, cons(seconds, NULL)));
}




void initTouchscreen()
{
  #if defined(touchscreen_support)

  tft_touch.setResolution( HRES, VRES) ;
  tft_touch.setRotation(3) ;
  
  // TouchInitCalibration
  // Change it by calibrated values 
  /*tft_touch._xflip = 0 ;
  tft_touch._yflip = 0 ;
  tft_touch.setCal(0, 4095, 0, 4095, HRES, VRES, 0 );*/
  pfstring(PSTR("Touchscreen setup\n"), pserial);
  tft_touch._xflip = 1 ;
  tft_touch._yflip = 1 ;
  tft_touch.setCal(526, 3451, 678, 3443, HRES, VRES,  1 ) ;

  //tft_touch._xyswap = 1 ;


  #endif
}

object *fn_touch_press (object *args, object *env) {
  (void) env;
#if defined(touchscreen_support)

  return  tft_touch.Pressed() ? tee : nil;
#else
  return nil ;
#endif
}

object *fn_touch_x (object *args, object *env) {
  (void) env;
#if defined(touchscreen_support)
  return number(tft_touch.X());
#else
  return nil ;
#endif
}

object *fn_touch_y (object *args, object *env) {
  (void) env;
#if defined(touchscreen_support)
  return number(tft_touch.Y());
#else
  return nil ;
#endif
}

void PrintToucscreenParameters()
{
#if defined(touchscreen_support)
  pfstring(PSTR("\n(touch-setcal "), pserial);
  pint(tft_touch._hmin, pserial);  pserial(' ');
  pint(tft_touch._hmax, pserial);  pserial(' ');
  pint(tft_touch._vmin, pserial);  pserial(' ');
  pint(tft_touch._vmax, pserial);  pserial(' ');
  pint(tft_touch._hres, pserial);  pserial(' ');
  pint(tft_touch._vres, pserial);  pserial(' ');
  pint(tft_touch._xyswap, pserial);  pserial(' ');
  pint(tft_touch._xflip, pserial);  pserial(' ');
  pint(tft_touch._yflip, pserial);
  pfstring(PSTR(")\n"), pserial);
#endif
}

object *fn_touch_printcal(object *args, object *env) 
{
  (void) env;

  PrintToucscreenParameters() ;

  return tee ;
}

object *fn_touch_setcal(object *args, object *env) 
{
  (void) env;

#if defined(touchscreen_support)
  int hmin, hmax, vmin, vmax, hres, vres, xyswap, xflip, yflip ;
  object *obj = args ;

  if(obj) { hmin = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { hmax = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { vmin = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { vmax = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { hres = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { vres = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { xyswap = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { xflip = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj)
  { 
    yflip = checkinteger(car(obj)) ;

    tft_touch.setResolution( HRES, VRES) ;
    tft_touch.setRotation(3) ;

    tft_touch._xflip = xflip ;
    tft_touch._yflip = yflip ;

    tft_touch.setCal(hmin, hmax, vmin, vmax, hres, vres, xyswap);

    return tee ;
  }
#endif
  

return nil ;
}

void drawCalibrationPrompt()
{
  tft.setTextColor(TFT_RED, TFT_BLACK) ;
  tft.drawString("Touch calibration", 100, 40) ;
  tft.setTextColor(TFT_GREEN, TFT_BLACK) ;
  tft.drawString("Press this point", 100, 50) ;
}


void drawCalibrationTestPrompt()
{
  tft.setTextColor(TFT_BLUE, TFT_BLACK) ;
  tft.drawString("Test touch calibration", 100, 40) ;
  tft.setTextColor(TFT_GREEN, TFT_BLACK) ;
  tft.drawString("Press this point", 100, 50) ;
}


void drawCalibrationCross(uint16_t x, uint16_t y, uint16_t color)
{
  tft.drawLine(x-10, y, x+10, y, color) ;
  tft.drawLine(x, y-10, x, y+10, color) ;
  }


int TestTouchPoint(int x, int y, int x0, int y0)
{
  if((abs(x-x0)>5)||(abs(y-y0)>5)) return 0 ;

  return 1 ;
}

#define DEF_CROSS_SHIFT 30


object *fn_touch_calibrate(object *args, object *env) 
{
  (void) env;

#if defined(touchscreen_support)
  int X_Raw, Y_Raw ;
  int x1, y1;
  int x2, y2;
  int x3, y3;
  bool xyswap  = 0, xflip = 0, yflip = 0;


  // Reset the calibration values
  tft_touch._xflip = 0 ;
  tft_touch._yflip = 0 ;
  tft_touch._xyswap = 0 ;
  tft_touch.setCal(0, 4095, 0, 4095, HRES, VRES, 0);//, 0, 0);
  // Set TFT the screen to landscape orientation
  tft.setRotation(0);
    // Set Touch the screen to the same landscape orientation
  tft_touch.setRotation(3);
  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Show the screen prompt
  drawCalibrationPrompt();

  drawCalibrationCross(DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_RED);
  while (!tft_touch.Pressed());
  delay(200);
  
  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("First point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x1 = X_Raw;
  y1 = Y_Raw;

  drawCalibrationCross(HRES/2, VRES/2, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits for the centre area to be touched
  delay(200);
  
  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(HRES/2, VRES/2, TFT_BLACK);
  Serial.print("Second point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

// not used

  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits until the centre area is no longer pressed
  delay(200);           // Wait a little for touch bounces to stop after release
  
  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("Third point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x2 = X_Raw;
  y2 = Y_Raw;

 
  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_RED);


  while (!tft_touch.Pressed()); // wait press touch
  delay(200);

  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch
 
  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_BLACK);

  Serial.print("Fourth point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

// not used

  drawCalibrationCross(DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits until the centre area is no longer pressed
  delay(200);           // Wait a little for touch bounces to stop after release
  
  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("Fifth point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x3 = X_Raw;
  y3 = Y_Raw;

  int temp;
  if (abs(x1 - x3) > 1000) {
    xyswap = 1;
    temp = x1; x1 = y1; y1 = temp;
    temp = x2; x2 = y2; y2 = temp;
    temp = x3; x3 = y3; y3 = temp;
  }
  else xyswap = 0;
  


  if (x2 < x1) {
    temp = x2; x2 = x1; x1 = temp;
    xflip = 1;
  }
  
  if (y2 < y1) {
    temp = y2; y2 = y1; y1 = temp;
    yflip = 1;
  }

  int hmin = x1;// - (x2 - x1) * 3 / (HRES/10 - 6);
  int hmax = x2;// + (x2 - x1) * 3 / (HRES/10 - 6);

  int vmin = y1;// - (y2 - y1) * 3 / (VRES/10 - 6);
  int vmax = y2;// + (y2 - y1) * 3 / (VRES/10 - 6);

  Serial.print("Parameters : ");
  Serial.print(hmin);
  Serial.print(",");
  Serial.print(hmax);
  Serial.print(",");
  Serial.print(vmin);
  Serial.print(",");
  Serial.print(vmax);
  Serial.print(", xyswap=");
  Serial.print(xyswap);
  Serial.print(", xflip=");
  Serial.print(xflip);
  Serial.print(", yflip=");
  Serial.println(yflip);

  tft_touch.setCal(hmin,hmax,vmin,vmax, HRES, VRES, xyswap);
  
  tft_touch._xflip = xflip ;
  tft_touch._yflip = yflip ;

  
  pfstring(PSTR("\nUse command:\n"), pserial);
    Serial.print("(touch-setcal  ");
  Serial.print(hmin);
  Serial.print(" ");
  Serial.print(hmax);
  Serial.print(" ");
  Serial.print(vmin);
  Serial.print(" ");
  Serial.print(vmax);
  Serial.print(" ");
  Serial.print(320);
  Serial.print(" ");
  Serial.print(240);
  Serial.print(" ");
  Serial.print(xyswap);
  Serial.print(" ");
  Serial.print(xflip);
  Serial.print(" ");
  Serial.print(yflip);
  Serial.println(")");

  //PrintToucscreenParameters() ;

  // *********  Test calibration result  *************
  drawCalibrationTestPrompt() ;

  Serial.print("\n\nTest of calibration\n");


  drawCalibrationCross(DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_RED);
  while (!tft_touch.Pressed());
  delay(200);
  
  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_BLACK);

  Serial.print("First point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw,DEF_CROSS_SHIFT, DEF_CROSS_SHIFT)) return nil ;

  drawCalibrationCross(HRES/2, VRES/2, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits for the centre area to be touched
  delay(200);
  
  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(HRES/2, VRES/2, TFT_BLACK);
  Serial.print("Second point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw, HRES/2, VRES/2)) return nil ;

  // point not used

  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits until the centre area is no longer pressed
  delay(200);           // Wait a little for touch bounces to stop after release
  
  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("Third point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw, HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT)) return nil ;


  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_RED);

  while (!tft_touch.Pressed()); // wait press touch
  delay(200);

  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch
 
  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_BLACK);

  Serial.print("Fourth point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw, HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT)) return nil ;

  //  point not used

  drawCalibrationCross(DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits until the centre area is no longer pressed
  delay(200);           // Wait a little for touch bounces to stop after release
  
  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("Fifth point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw, DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT)) return nil ;


  return tee;
#else
  return nil ;
#endif
}



/*
(kbhit)  -  test whether any keyboard keys hits"
*/

object *fn_kbhit (object *args, object *env) {
  (void) env;

  if(LastChar>0) return tee ;
  
  delay(200);
  if(Serial.available()>0)
  {
    //delay(10);
    LastChar = Serial.read();
    return tee ;
  }

  return  nil;
}

const char string_kbhit[] = "kbhit" ;
const char doc_kbhit[] = "(kbhit) - test whether any keyboard keys hits.\n"
" Returns t if any char symbols are available"
"and otherwise returns nil.";



/* Insert '/' symbol into begin of filename if it is absent.
"name" => "/name",  "home/name" => "/home/name"
*/
void test_filename(char *name) {
  int len, i ;
  char *cPtr ;
  if (name[0] == '/' ) return ;
  len = strlen(name) ;
  cPtr = &name[len] ;
  *(cPtr+1) = 0 ;

  for (i=0;i<len;i++) {
      *cPtr= *(cPtr-1) ;
      cPtr -- ;  
  }

  name[0] = '/' ;
}



/*(probe-file pathspec)  tests whether a file exists.
Returns nil if there is no file (directory) named pathspec,
and otherwise returns the truename of pathspec.
(probe-file "pathname")  - tests whether a file "pathname" exists
(probe-file "pathname/")  -  tests whether a directory "pathname" exists
*/
object *fn_probefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  char pattern_string[256] ;
  int findDir = 0 ;
 
  if (stringp(car(args))) cstring(car(args), pattern_string, 256) ;
  else {
    error("argument must be string", car(args)); return nil; 
  }
 
  if (pattern_string[strlen(pattern_string)-1] == '/') {
    pattern_string[strlen(pattern_string)-1] = 0x0 ;
    findDir = 1 ;
  }

  test_filename(pattern_string) ;
  SDBegin();
  if (SD.exists(pattern_string)) {
    File entry = SD.open(pattern_string) ;
    if ( (entry.isDirectory()) && (findDir)) {
      entry.close();
      return car(args);
    }
    else if ( (!entry.isDirectory() )&& (!findDir)) {
      entry.close();
      return car(args);
    }
  }
  return nil;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}


/* (delete-file pathspec)   delete specified file.
Returns true if success and otherwise returns nil.
*/
object *fn_deletefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  char pattern_string[256] ;

  if (stringp(car(args))) cstring(car(args), pattern_string, 256) ;
  else {
    error("argument must be string", car(args)); return nil; 
  }

  test_filename(pattern_string) ;
  SDBegin();
  if (SD.exists(pattern_string)) {
    if (SD.remove(pattern_string)) return tee;
    else return nil;
  }
  return tee;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}


/* (delete-dir pathspec)   delete specified directory.
Returns true if success and otherwise returns nil.
*/
object *fn_deletedir (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  char pattern_string[256] ;

  if (stringp(car(args))) cstring(car(args), pattern_string, 256) ;
  else {
    error("argument must be string", car(args)); return nil; 
  }

  test_filename(pattern_string) ;
  SDBegin();
  if (SD.exists(pattern_string))
  {
     if (SD.rmdir(pattern_string)) return tee;
     else return nil;
  }
  return tee;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}


/* (rename-file pathspec newfile)  rename or moving specified file.
Returns true if success and otherwise returns nil.
*/
object *fn_renamefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  char filename_string[256] ;
  char newname_string[256] ;
  object *firstarg = car(args);

  if (stringp(car(args))) cstring(car(args), filename_string, 256) ;
  else  {  
    error("first argument must be string", car(args)); return nil; 
  }

  args = cdr(args);
  
  if (stringp(car(args)))
    cstring(car(args), newname_string, 256) ;
  else  {
    error("second argument must be string", car(args)); return nil; 
  }

  test_filename(filename_string) ;
  test_filename(newname_string) ;

  SDBegin();
  if (!SD.exists(filename_string)) {  error("File not exists", firstarg); return nil; }
  
  File fp_source = SD.open(filename_string, FILE_READ);
  if (fp_source.isDirectory()) { 
      fp_source.close() ;
      error("argument must be a file", firstarg); return nil; 
  }

  if (SD.exists(newname_string)) SD.remove(newname_string) ;
  File fp_dest = SD.open(newname_string, FILE_WRITE);
  if (!fp_dest) {
    error("cannot open destination file", car(args)); return nil; 
  }

  uint32_t i, sz ;
  sz = fp_source.size();
  for (i=0; i<sz;i++) fp_dest.write(fp_source.read()) ;

  fp_source.close();
  fp_dest.close();
  SD.remove(filename_string) ;

  return tee;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}

/* (copy-file pathspec newfile)  copy specified file.
Returns true if success and otherwise returns nil.
*/
object *fn_copyfile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  char filename_string[256] ;
  char newname_string[256] ;
  object *firstarg = car(args);

  if (stringp(car(args))) cstring(car(args), filename_string, 256) ;
  else  {  error("first argument must be string", car(args)); return nil; }

  args = cdr(args);

  if (stringp(car(args)))
    cstring(car(args), newname_string, 256) ;
  else  {
    error("second argument must be string", car(args)); return nil; 
  }

  test_filename(filename_string) ;
  test_filename(newname_string) ;

  SDBegin();
  if (!SD.exists(filename_string)) {
    error("File not exists", firstarg); return nil; 
  }

  File fp_source = SD.open(filename_string, FILE_READ);
  if (fp_source.isDirectory()) { 
      fp_source.close() ;
      error("argument must be a file", firstarg); return nil; 
  }

  if (SD.exists(newname_string)) SD.remove(newname_string) ;
  File fp_dest = SD.open(newname_string,FILE_WRITE);
  if (!fp_dest) {
    error("cannot open destination file", car(args)); return nil; 
  }

  uint16_t i, sz ;
  sz = fp_source.size();
  for (i=0; i<sz;i++) fp_dest.write(fp_source.read()) ;

  fp_source.close();
  fp_dest.close();

  return tee;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}

/* (ensure-directories-exist pathspec)   Tests whether the specified
directories actually exist, and attempts to create them if they do not.
Returns true if success and otherwise returns nil.
*/
object *fn_ensuredirectoriesexist(object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  char pattern_string[256] ;
 
  if (stringp(car(args))) cstring(car(args), pattern_string, 256) ;
  else  {
    error("argument must be string", car(args)); return nil; 
  }

  test_filename(pattern_string) ;

  SDBegin();
  if(!SD.exists(pattern_string)) {
    if(SD.mkdir(pattern_string)) return tee;
  }
  else return tee;

  return nil;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}


/*********************************************************************************************/

#if defined(DEF_ARRAY2)

enum { CHARACTER_ARRAY, SINGLEFLOAT_ARRAY, DOUBLEFLOAT_ARRAY, INTEGER_ARRAY };




const char string_makearray2[] = "make-array*";
const char string_aref2[] = "aref*";
const char string_delarray2[] = "del-array*";
const char string_array2p[] = "arrayp*";



const char string_integer[] = "integer";
const char string_char[] = "character";
const char string_singlefloat[] = "single-float";
const char string_doublefloat[] = "double-float";




const char doc_makearray2[] = "(make-array* size [:initial-element element] [:element-type 'bit])\n"
"If size is an integer it creates a one-dimensional array with elements from 0 to size-1.\n"
"If size is a list of n integers it creates an n-dimensional array with those dimensions.\n"
"If :element-type 'bit is specified the array is a bit array.";

const char doc_aref2[] = "(aref* array index [index*])\n"
"Returns an element from the specified array.";

const char doc_delarray2[] = "(del-array* array)\n"
"Delete array.";

const char doc_array2p[] = "(arrayp* item)\n"
"Returns t if its argument is an array.\n";



symbol_t array2_CHAR_name ;
symbol_t array2_SINGLEFLOAT_name ;
symbol_t array2_DOUBLEFLOAT_name ;
symbol_t array2_INTEGER_name ;

void InitArray2()
{
    if(array2_CHAR_name == 0)
    {
        array2_SINGLEFLOAT_name = sym(lookupbuiltin ((char*)"single-float")) ;
        array2_DOUBLEFLOAT_name = sym(lookupbuiltin ((char*)"double-float")) ;
        array2_INTEGER_name = sym(lookupbuiltin ((char*)"integer")) ;
        array2_CHAR_name = sym(lookupbuiltin ((char*)"character")) ;

        Aref2_name = sym(lookupbuiltin ((char*)"aref*")) ;
    }
}


void filldimensionsteps(array_desc_t *descriptor)
{
    int32_t sz = 1 ;
    int i ;
    for(i=descriptor->ndim-1;i>=0;i--)
    {
        descriptor->dimstep[i] = sz ;
        sz *= descriptor->dim[i] ;
    }
}


object *makearray2 (object *dims, object *def, bool bitp, int type) {
  int size = 1, i = 0 ;
  array_desc_t descriptor ;
  //object *dimensions = dims;
  while (dims != NULL) {
    int d = car(dims)->integer;
    if (d < 0) error2("dimension can't be negative");
    descriptor.dim[i] = d;
    size = size * d;
    dims = cdr(dims);
    i++;
  }
  descriptor.ndim = i;
  descriptor.size = size;
  filldimensionsteps(&descriptor);

  if(def)
      descriptor.type = def->type ;
  else
      descriptor.type = type ;

  // Bit array identified by making first dimension negative
  /*if (bitp) {
    size = (size + sizeof(int)*8 - 1)/(sizeof(int)*8);
    car(dimensions) = number(-(car(dimensions)->integer));
  }*/
  
  object *ptr = myalloc();
  ptr->type = ARRAY2;


    switch(descriptor.type)
    {
    case FLOAT:
       {
         descriptor.element_size = sizeof(sfloat_t) ;
#if defined(BOARD_HAS_PSRAM)
         ptr->pointer = (uintptr_t)ps_malloc(sizeof(sfloat_t)*size+sizeof(descriptor));
#else
         ptr->pointer = (uintptr_t)malloc(sizeof(sfloat_t)*size+sizeof(descriptor));
#endif
         uintptr_t array_pointer = (uintptr_t)(ptr->pointer + sizeof (array_desc_t)) ;
         sfloat_t *Ptr = (sfloat_t*)(array_pointer) ;
         if(def) for(i=0;i<size;i++)  *Ptr ++= def->single_float ;
         else for(i=0;i<size;i++)  *Ptr ++= 0.0 ;
       }
        break ;
    case CHAR:
       {
         descriptor.element_size = sizeof(char) ;
#if defined(BOARD_HAS_PSRAM)
         ptr->pointer = (uintptr_t)ps_malloc(sizeof(char)*size+sizeof(descriptor));
#else
         ptr->pointer = (uintptr_t)malloc(sizeof(char)*size+sizeof(descriptor));
#endif
         uintptr_t array_pointer = (uintptr_t)(ptr->pointer + sizeof (array_desc_t)) ;
         char *Ptr = (char*)(array_pointer) ;
         if(def) for(i=0;i<size;i++)  *Ptr ++= (def->chars)&0x0ff ;
          else for(i=0;i<size;i++)  *Ptr ++= 0 ;
       }
        break;
    case NUMBER:
    default : ;
       {
         descriptor.element_size = sizeof(long int) ;
#if defined(BOARD_HAS_PSRAM)
         ptr->pointer = (uintptr_t)ps_malloc(sizeof(long int)*size+sizeof(descriptor));
#else
         ptr->pointer = (uintptr_t)malloc(sizeof(long int)*size+sizeof(descriptor));
#endif
         uintptr_t array_pointer = (uintptr_t)(ptr->pointer + sizeof (array_desc_t)) ;
         long int *Ptr = (long int*)(array_pointer) ;
         if(def) for(i=0;i<size;i++)  *Ptr ++= def->integer ;
         else for(i=0;i<size;i++)  *Ptr ++= 0 ;
       }
    }

    *(array_desc_t*)(ptr->pointer) = descriptor;

  return ptr;
}






/*
  (make-array size [:initial-element element] [:element-type 'bit])
  If size is an integer it creates a one-dimensional array with elements from 0 to size-1.
  If size is a list of n integers it creates an n-dimensional array with those dimensions.
  If :element-type 'bit is specified the array is a bit array.
*/
object *fn_makearray2 (object *args, object *env) {
  (void) env;
  object *def = nil;
  bool bitp = false;
  int type = NUMBER ;
  object *dims = first(args);
  if (dims == NULL) error2("dimensions can't be nil");
  else if (atom(dims)) dims = cons(dims, NULL);
  args = cdr(args);
  while (args != NULL && cdr(args) != NULL) {
    object *var = first(args);
    if (isbuiltin(first(args), INITIALELEMENT)) def = second(args);
    else
      if (isbuiltin(first(args), ELEMENTTYPE))
       {
          object *typeobj = second(args) ;

          InitArray2() ;

          if( isbuiltin(typeobj, BIT)) { bitp = true; type = CHAR ; }
          else if( typeobj->name == array2_CHAR_name) type = CHAR ;
          else if( typeobj->name == array2_INTEGER_name) type = NUMBER ;
          else if( typeobj->name == array2_SINGLEFLOAT_name) type = FLOAT ;
          else if( typeobj->name == array2_DOUBLEFLOAT_name) type = FLOAT ;
      }
    else error("argument not recognised", var);
    args = cddr(args);
  }
  if (bitp) {
    if (def == nil) def = number(0);
    else def = number(-checkbitvalue(def)); // 1 becomes all ones
  }
  return makearray2(dims, def, bitp, type);
}

/*
  (aref* array index [index*])
  Returns an element from the specified array.
*/
object *fn_aref2 (object *args, object *env) {
  (void) env;
  int bit;
  object *array = first(args);
  if (!array2p(array))
      error("first argument is not an array", array);
  object *loc = myalloc(); //&((array_desc_t*)array->pointer)->bufferobj ;
  loc->type = ((array_desc_t*)array->pointer)->type ;
  switch(loc->type)
  {
  case FLOAT:
      loc->single_float = *(sfloat_t*)(*getarray2(array, cdr(args), 0, &bit));
      break ;
  case CHAR:
      loc->type = NUMBER ;
      loc->integer = *(char*)(*getarray2(array, cdr(args), 0, &bit)) & 0x0ff ;
      break ;
  default:
      loc->integer = *(long int*)(*getarray2(array, cdr(args), 0, &bit)) ;
      if(loc->integer==0x72)
      {
          bit = 0 ;
      }
  }

  return loc;
  //if (bit == -1) return loc;
  //else return number((loc->integer)>>bit & 1);
}



/*
  getarray - gets a pointer to an element in a multi-dimensional array, given a list of the subscripts subs
  If the first subscript is negative it's a bit array and bit is set to the bit number
*/
object **getarray2 (object *array, object *subs, object *env, int *bit) {
  int index = 0, size = 1, s;
  *bit = -1;

  array_desc_t *descriptor = (array_desc_t*)array->integer ;
  int *dimstep = (int*)descriptor->dimstep;
  int *dim = (int*)descriptor->dim;
  int n = descriptor->ndim ;
  while (n != 0 && subs != NULL) {
    int d = *dimstep;
    //if (d < 0) { d = -d; bitp = true; }
    if (env) s = checkinteger(eval(car(subs), env)); else s = checkinteger(car(subs));
    if (s < 0 || s >= *dim) error("subscript out of range", car(subs));
    size = d;
    index += s*d;
    n--;
    dimstep++;
    dim++;
    subs = cdr(subs);
  }
  if (n != 0) error2("too few subscripts");
  if (subs != NULL) error2("too many subscripts");
  /*if (bitp) {
    size = (size + sizeof(int)*8 - 1)/(sizeof(int)*8);
    *bit = index & (sizeof(int)==4 ? 0x1F : 0x0F);
    index = index>>(sizeof(int)==4 ? 5 : 4);
  }*/

  return arrayref2(array, index, size);
}




//extern volatile int isAref ;

/*
  arrayref - returns a pointer to the element specified by index in the array of size s
*/
object **arrayref2 (object *array, int index, int size) {

    size = 0 ;
/*  object **p = &car(cdr(array));
  while (mask) {
    if ((index & mask) == 0) p = &(car(*p)); else p = &(cdr(*p));
    mask = mask>>1;
  }*/
    //int i ;
    //object *ret ; //= myalloc(); //= car(cdr(array));
    array_desc_t *descriptor = (array_desc_t*)array->pointer ;
    object **p = &descriptor->obj_pointer ; //&car(cdr(array));

    //ret->type = descriptor->type ;
    uintptr_t array_pointer = (uintptr_t)(array->pointer) + sizeof(array_desc_t);
    char *Ptr = (char*)(array_pointer) ;

    //printf("array %u\n", Ptr); fflush(stdout);
    *p =(object *)&Ptr[index*descriptor->element_size] ;

  return p;
}


/*
  (arrayp item)
  Returns t if its argument is an array.
*/
object *fn_array2p (object *args, object *env) {
  (void) env;
  return array2p(first(args)) ? tee : nil;
}


int32_t array2_lenght(object *arg)
{
    array_desc_t *desc = (array_desc_t*)(arg->pointer) ;

    if(!arg->pointer) return -1 ;
    if(desc->ndim > 1) return -2 ;

    return desc->size ;
}

/*
  delarray
*/
void delarray2 (object *array) {
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
#if defined(BOARD_HAS_PSRAM)
  ps_free((char*)descriptor) ;
#else
  free((char*)descriptor) ;
#endif
  array->pointer = (uintptr_t)NULL ;
  array->type = SYMBOL ;
}


object *fn_delarray2 (object *args, object *env) {
  (void) env;
    object *arg = first(args) ;

    if(arg)
    {
        if(array2p(arg))
        {
            delarray2(arg) ;
            return tee ;
        }
    }
  return nil;
}



/*
  pslice2 - prints a slice of an array recursively
*/
void pslice2 (object *array, int size, int slice, int32_t *dim, int32_t *dimstep, int n, pfun_t pfun, bool bitp) {
  extern void myfree (object *obj) ;

  bool spaces = true;
  if (slice == -1) { spaces = false; slice = 0; }
  int d = *dim ;
  int ds = *dimstep ;
  if (d < 0) d = -d;
  for (int i = 0; i < d; i++) {
    if (i && spaces) pfun(' ');
    int index = slice + ds * i;
    if (n == 1)
    {
      /*if (bitp) pint(((*arrayref2(array, index>>(sizeof(int)==4 ? 5 : 4), size))->integer)>>
        (index & (sizeof(int)==4 ? 0x1F : 0x0F)) & 1, pfun);
      else*/
        array_desc_t *descriptor = (array_desc_t*)array->pointer ;
        object *obj = myalloc() ;
        obj->type = descriptor->type ;

        switch(obj->type)
        {
        case FLOAT:
            obj->single_float = *(sfloat_t*)(*arrayref2(array, index, size));
            break ;
        case CHAR:
            obj->type = NUMBER ;
            obj->integer = *(char*)(*arrayref2(array, index, size)) & 0x0ff ;
            break ;
        default:
            obj->integer = *(long int*)(*arrayref2(array, index, size)) ;
        }
        //*arrayref2(array, index, size)
        printobject(obj, pfun);
        myfree(obj);
    } else { pfun('(');  pslice2(array, size, index, &dim[1], &dimstep[1], n-1, pfun, bitp); pfun(')'); }
  }
}


/*
  printarray2 - prints an array2 in the appropriate Lisp format
*/
void printarray2 (object *array, pfun_t pfun) {
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
  bool bitp = false;
  int size =  descriptor->size, n = descriptor->ndim;

  //if (bitp) size = (size + sizeof(int)*8 - 1)/(sizeof(int)*8);
  pfun('#');
  /*if (n == 1 && bitp) { pfun('*'); pslice(array, size, -1, dimensions, pfun, bitp); }
  else */{
    if (n > 1) { pint(n, pfun); pfun('A'); }
    pfun('('); pslice2(array, size, 0, descriptor->dim, descriptor->dimstep, n, pfun, bitp); pfun(')');
  }
}


object *array2dimensions(object *array)
{
    array_desc_t *descriptor = (array_desc_t*)array->pointer ;
    int i, j;

    j = descriptor->dim[0] ;
    object *num = number(j);

    object *dimensions = cons(num, NULL);
    object *ptr = dimensions ;

    for(i=1;i<descriptor->ndim;i++){
        j = descriptor->dim[i] ;
        num = number(j);
        cdr(ptr) = cons(num, NULL);
        ptr = cdr(ptr);
    }
    return dimensions ;
}


void array2info (object *array) {
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
  printf("ndim   =%d \n", descriptor->ndim);
  if((descriptor->ndim>0)&&(descriptor->ndim<10))
    for(int i=0;i<descriptor->ndim;i++) printf(" %d", descriptor->dim[i]);
  printf("\nsize   =%d \n", (int)descriptor->size) ;
  printf("elsize =%d \n", descriptor->element_size);
  printf("type   =%d \n", (int)descriptor->type) ;
  fflush(stdout);
}
#endif


/*********************************************************************************************/



// Symbol names
const char stringnow[] PROGMEM  = "now";
const char stringtouch_press[] PROGMEM = "touch-press";
const char stringtouch_x[] PROGMEM = "touch-x";
const char stringtouch_y[] PROGMEM = "touch-y";
const char stringtouch_calibrate[] PROGMEM = "touch-calibrate";
const char stringtouch_setcal[] PROGMEM = "touch-setcal";
const char stringtouch_printcal[] PROGMEM = "touch-printcal";

const char string_probefile[] PROGMEM = "probe-file";
const char string_deletefile[] PROGMEM = "delete-file";
const char string_deletedir[] PROGMEM = "delete-dir";
const char string_renamefile[] PROGMEM = "rename-file";
const char string_copyfile[] PROGMEM = "copy-file";
const char string_ensuredirectoriesexist[] PROGMEM = "ensure-directories-exist";



// Documentation strings
const char docnow[] PROGMEM  = "(now [hh mm ss])\n"
"Sets the current time, or with no arguments returns the current time\n"
"as a list of three integers (hh mm ss).";

const char doctouch_press[] PROGMEM = "(touch-press)\n"
"Returns true if touchscreen is pressed and false otherwise.";
const char doctouch_x[] PROGMEM = "(touch-x)\n"
"Returns pressed touchscreen x-coordinate.";
const char doctouch_y[] PROGMEM = "(touch-y)\n"
"Returns pressed touchscreen y-coordinate.";
const char doctouch_calibrate[] PROGMEM = "(touch-calibrate)\n"
"Runs touchscreen calibration.";
const char doctouch_setcal[] PROGMEM = "(touch-setcal minx maxx miny maxy\n      hres vres axisswap xflip yflip)\n"
"Set touchscreen calibration parameters.";
const char doctouch_printcal[] PROGMEM = "(touch-printcal)\n"
"Print touchscreen calibration parameters.";
const char doc_readserial[] PROGMEM = "(read-serial)\n"
"Reads a byte from a serial port and returns it.";



const char doc_probefile[] PROGMEM = "(probe-file pathspec)\n"
"tests whether a file exists.\n"
" Returns nil if there is no file named pathspec,"
" and otherwise returns the truename of pathspec.";

const char doc_deletefile[] PROGMEM = "(delete-file pathspec)\n"
"delete specified file.\n"
" Returns true if success and otherwise returns nil.";

const char doc_deletedir[] PROGMEM = "(delete-dir pathspec)\n"
"delete specified directory.\n"
" Returns true if success and otherwise returns nil.";

const char doc_renamefile[] PROGMEM = "(rename-file filespec newfile)\n"
"rename or moving specified file.\n"
" Returns true if success and otherwise returns nil.";

const char doc_copyfile[] PROGMEM = "(copy-file filespec newfile)\n"
"copy specified file.\n"
" Returns true if success and otherwise returns nil.";

const char doc_ensuredirectoriesexist[] PROGMEM = "(ensure-directories-exist pathspec)\n"
"Tests whether the specified directories actually exist,"
" and attempts to create them if they do not.\n"
" Returns true if success and otherwise returns nil.\n";




// Symbol lookup table
const tbl_entry_t lookup_table2[] PROGMEM  = {
    { stringnow, fn_now, 0203, docnow },
    { stringtouch_press, fn_touch_press, 0200, doctouch_press },
    { stringtouch_x, fn_touch_x, 0200, doctouch_x },
    { stringtouch_y, fn_touch_y, 0200, doctouch_y },
    { stringtouch_calibrate, fn_touch_calibrate, 0200, doctouch_calibrate },
    { stringtouch_setcal, fn_touch_setcal, 0217, doctouch_setcal },
    { stringtouch_printcal, fn_touch_printcal, 0200, doctouch_printcal },

    { string_probefile, fn_probefile, 0211, doc_probefile },
    { string_renamefile, fn_renamefile, 0222, doc_renamefile },
    { string_copyfile, fn_copyfile, 0222, doc_copyfile },
    { string_deletefile, fn_deletefile, 0211, doc_deletefile },
    { string_deletedir, fn_deletedir, 0211, doc_deletedir },
    { string_ensuredirectoriesexist, fn_ensuredirectoriesexist, 0211, doc_ensuredirectoriesexist },

    { string_kbhit, fn_kbhit, 0200, doc_kbhit },


#if defined(DEF_ARRAY2)
    { string_makearray2, fn_makearray2, 0215, doc_makearray2 },
    { string_delarray2, fn_delarray2, 0215, doc_delarray2 },
    { string_array2p, fn_array2p, 0211, doc_array2p },

    { string_aref2, fn_aref2, 0227, doc_aref2 },

    { string_char, NULL, 0000, NULL },
    { string_singlefloat, NULL, 0000, NULL },
    { string_doublefloat, NULL, 0000, NULL },
    { string_integer, NULL, 0000, NULL },

#endif
};




// Table cross-reference functions

tbl_entry_t *tables[] = {lookup_table, lookup_table2};
const unsigned int tablesizes[] PROGMEM = { arraysize(lookup_table), arraysize(lookup_table2) };

const tbl_entry_t *table (int n) {
  return tables[n];
}

unsigned int tablesize (int n) {
  return tablesizes[n];
}

This is lisp interpretator, based on "ulisp" project which is developed by David Johnson-Davies (http://www.ulisp.com). 
This code can be used for the ESP32-2432S028R device ((ESP32-WROOM-32 board with TFT and touchscreen) or for other ESP32 modules.
In this code new system of arrays is added.
New array commands are named as "aref*", "arrayp*", "make-array*", and "del-array*" insteed of "aref", "arrayp", "make-array".
Examples of calls:

    (defvar b (make-array* '(4 4) :element-type 'single-float))

    (do ((i 0 (1+ i)))  ((= i 4))
     (do ((j 0 (1+ j)))  ((= j 4))
      (setf (aref* b i j) (+ (* i 4) j))
     )
    )

    (print b)

    5480> (print b)
    #2A((0 1.0 2.0 3.0) (4.0 5.0 6.0 7.0) (8.0 9.0 10.0 11.0) (12.0 13.0 14.0 15.0))
    #2A((0 1.0 2.0 3.0) (4.0 5.0 6.0 7.0) (8.0 9.0 10.0 11.0) (12.0 13.0 14.0 15.0))

    5480> (save-image)
    Write cell:94,  array size 64
    3570
    5480>

After reseting device:

    5483> (print b)
    Error: undefined: b

    5483> (load-image)
    Read cell:94,  array size 64
    3570

    5480> (print b)
    #2A((0 1.0 2.0 3.0) (4.0 5.0 6.0 7.0) (8.0 9.0 10.0 11.0) (12.0 13.0 14.0 15.0))
    #2A((0 1.0 2.0 3.0) (4.0 5.0 6.0 7.0) (8.0 9.0 10.0 11.0) (12.0 13.0 14.0 15.0))
    5480>

   5480> (array-dimensions b)
   (4 4)

   5480>

The sources are contain new arrays typewhich are on or off by

    #define DEF_ARRAY2

You can found new array implementation parts by search

    #if defined(DEF_ARRAY2)

in this sources.



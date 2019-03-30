open Ctypes
open Foreign

type image = unit ptr
type point = int * int
type area = point * point

let libIT = Dl.dlopen ~flags:[Dl.RTLD_LAZY] ~filename:("/home/pi/IT8951/IT8951")

let funer name params = foreign ~from:libIT ~release_runtime_lock:false name params
let vv = void @-> returning void

let fg = 0x00
let bg = 0xFF

let avg (x1,y1) (x2,y2) = (x1+x2)/2, (y1+y2)/2

(* Returns true on failure *)
let init () =
  let init () = funer "initGraphics" (void @-> returning bool) () in
  init ()

let free () = funer "freeGraphics" vv ()

let clear () = funer "clearColor" (int @-> returning void) bg

let width () = funer "width" (void @-> returning int) ()

let height () = funer "height" (void @-> returning int) ()

let load_image t () = funer "loadImage" (ptr void @-> returning void) t

let display (x1,y1) (x2,y2) () = funer "display" (int @-> int @-> int @-> int @-> returning void) x1 y1 x2 y2

let display_all () = funer "display" (int @-> int @-> int @-> int @-> returning void) 0 0 (width () - 1) (height () - 1)

let plot (x,y) () = funer "plot" (int @-> int @-> returning void) x y

let point_color (x,y) = funer "pointColor" (int @-> int @-> returning int) x y

let draw_line (x,y) (x2,y2) () = funer "drawLine" (int @-> int @-> int @-> int @-> returning void) x y x2 y2

let draw_text (x,y) str ?(fg=fg) ?(bg=bg) () = funer "putText" (int @-> int @-> string @-> int @-> int @-> returning void) x y str fg bg

let set_color c = funer "setColor" (int @-> returning void) c

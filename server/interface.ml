open Ctypes
open Foreign

type image = unit ptr
type point = int * int
type area = point * point

let libIT = Dl.dlopen ~flags:[Dl.RTLD_LAZY] ~filename:("/home/pi/IT8951/IT8951")

let funer name params = foreign ~from:libIT ~release_runtime_lock:false name params
let vv = void @-> returning void

let foreground = 0x00
let background = 0xFF

(* Returns true on failure *)
let init () =
  let init () = funer "initGraphics" (void @-> returning bool) () in
  init ()

let free () = funer "freeGraphics" vv ()

let clear () = funer "clearColor" (int @-> returning void) background

let load_image t = funer "loadImage" (ptr void @-> returning void) t

let display () = funer "display" vv ()

let plot (x,y) = funer "plot" (int @-> int @-> returning void) x y

let point_color (x,y) = funer "pointColor" (int @-> int @-> returning int) x y

let draw_line (x,y) (x2,y2) = funer "drawLine" (int @-> int @-> int @-> int @-> returning void) x y x2 y2

let set_color c = funer "setColor" (int @-> returning void) c

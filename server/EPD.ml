open Ctypes
open Foreign

let libIT = Dl.dlopen ~flags:[Dl.RTLD_LAZY] ~filename:("/home/pi/IT8951/IT8951")

let funer name params = foreign ~from:libIT ~release_runtime_lock:false name params
let vv = void @-> returning void

type point = int * int
type area = point * point
type image = int carray
type mode = White | Unknown | Slow | Medium | Fast
let mode_to_int = function White -> 0 | Unknown -> 1 | Slow -> 2 | Medium -> 3 | Fast -> 4

let fg = 0xFF
let bg = 0x00

let width () = funer "width" (void @-> returning int) ()
let height () = funer "height" (void @-> returning int) ()

(* Returns true on failure *)
let init () =
  let init () = funer "initGraphics" (void @-> returning bool) () in
  init ()

let free () = funer "freeGraphics" vv ()

let load_image t = funer "loadImage" (ptr void @-> returning void) t

let plot (x,y) = funer "plot" (int @-> int @-> returning void) x y

let point_color (x,y) = funer "pointColor" (int @-> int @-> returning int) x y

let set_color c = funer "setColor" (int @-> returning void) c

let clear () = funer "clearColor" (int @-> returning void) bg

let load_image img ((x1,y1),(x2,y2)) = funer "loadImage" (ptr void @-> int @-> int @-> int @-> int @-> returning void) img x1 y1 x2 y2

let display ((x1,y1),(x2,y2)) mode = funer "display" (int @-> int @-> int @-> int @-> int @-> returning void) x1 y1 x2 y2 mode

let display_buffer (x1,y1) (x2,y2) = funer "displayBuffer" (int @-> int @-> int @-> int @-> returning void) x1 y1 x2 y2

let display_all () = funer "display" (int @-> int @-> int @-> int @-> returning void) 0 0 (width () - 1) (height () - 1)

let draw_line (x1,y1) (x2,y2) = funer "drawLine" (int @-> int @-> int @-> int @-> returning void) x1 y1 x2 y2

let draw_text ?(fg=fg) ?(bg=bg) (x,y) str = funer "putText" (int @-> int @-> string @-> int @-> int @-> returning void) x y str fg bg

let draw_circle (x,y) r = funer "drawCircle" (int @-> int @-> int @-> returning void) x y r

let fill_circle (x,y) r = funer "fillCircle" (int @-> int @-> int @-> returning void) x y r

let draw_rect (x1,y1) (x2,y2) =
  draw_line (x1,y1) (x2,y1);
  draw_line (x2,y1) (x2,y2);
  draw_line (x2,y2) (x1,y2);
  draw_line (x1,y2) (x1,y1)

let rec fill_rect (x1,y1) (x2,y2) =
  draw_line (x1,y1) (x2,y1);
  if y1 < y2 then
    fill_rect (x1,y1+1) (x2,y2)

let rgb r g b = funer "rgb" (int @-> int @-> int @-> returning int) r g b



let random int = Random.int int

let point () = random 799, random 599

let char () = random 255 |> Char.chr

let rec init_aux i n f =
  if i >= n then []
  else
    let r = f i in
    r :: init_aux (i+1) n f

let linit len f =
  if len < 0 then invalid_arg "List.init" else
    init_aux 0 len f

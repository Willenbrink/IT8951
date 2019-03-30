open Ctypes
open Foreign
open Protocol

let libIT = Dl.dlopen ~flags:[Dl.RTLD_LAZY] ~filename:("/home/pi/IT8951/IT8951")

let funer name params = foreign ~from:libIT ~release_runtime_lock:false name params
let vv = void @-> returning void

let mode_to_int = function White -> 0 | Unknown -> 1 | Slow -> 2 | Medium -> 3 | Fast -> 4

let width () = funer "width" (void @-> returning int) ()
let height () = funer "height" (void @-> returning int) ()

let handle = function
  | Display (((x1, y1), (x2, y2)),mode) -> funer "display" (int @-> int @-> int @-> int @-> int @-> returning void) x1 y1 x2 y2 (mode_to_int mode); Ack
  | Image (img, ((x1,y1),(x2,y2))) ->
    funer "loadImg"
      (ptr void @-> int @-> int @-> int @-> int @-> returning void)
      (CArray.of_list int img |> CArray.start |> to_voidp) x1 y1 x2 y2; Ack
  | Size -> Dim (width (), height ())

(* Returns true on failure *)
let init () =
  let init () = funer "initGraphics" (void @-> returning bool) () in
  init ()

let free () = funer "freeGraphics" vv ()

open Unix
open Protocol
open Ctypes
open Foreign

let libIT = Dl.dlopen ~flags:[Dl.RTLD_LAZY] ~filename:("/home/pi/IT8951/IT8951")

let funer name params = foreign ~from:libIT ~release_runtime_lock:false name params

let init () =
  let init = funer "init" (void @-> returning bool) () in
  if init then Failure "init" |> raise

let cancel () = funer "cancel" (void @-> returning void) ()

let handle_input ic oc request =
  match request with
  | None -> ()
  | Some request -> match request with
    | Init -> init ()
    | _ -> send oc Fail

let handle ic oc =
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)

  let rec worker () =
    let request = recv ic in
    handle_input ic oc request;
    worker ()
  in
  worker ()

let start
    ?(f = handle)
    ?(addr = ADDR_INET (inet_addr_any, 1234))
    () =
  Unix.establish_server f addr

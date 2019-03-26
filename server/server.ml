open Unix
open Protocol
open Ctypes
open Foreign

exception LLError

let libIT = Dl.dlopen ~flags:[Dl.RTLD_LAZY] ~filename:("/home/pi/IT8951/IT8951")

let funer name params = foreign ~from:libIT ~release_runtime_lock:false name params

type state = {conn : connection; initialised : bool;}

let init () =
  let init () = funer "init" (void @-> returning bool) () in
  if init () then LLError |> raise

let cancel () = funer "cancel" (void @-> returning void) ()

let handle_input ({conn; initialised} as state) request =
  try match request with
    | Heartbeat -> Ack
    | Init -> if not initialised then init (); Success
    | _ -> Ack
  with _ -> Fail

let rec worker ({conn; _} as state) =
  let state = match react conn (handle_input state) with
    | Init, Success -> {conn; initialised = true}
    | Init, Fail -> {conn; initialised = false}
    | _ -> state
  in
  worker state


let handle ic oc =
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)
  worker { conn = {ic; oc}; initialised = false}

let start
    ?(f = handle)
    ?(addr = ADDR_INET (inet_addr_any, 1234))
    () =
  Unix.establish_server f addr

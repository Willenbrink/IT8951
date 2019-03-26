open Unix
open Protocol
open Ctypes
open Foreign

exception LLError

let libIT = Dl.dlopen ~flags:[Dl.RTLD_LAZY] ~filename:("/home/pi/IT8951/IT8951")

let funer name params = foreign ~from:libIT ~release_runtime_lock:false name params

type state = {initialised : bool; ic : in_channel; oc : out_channel}

let init initialised =
  let init () = funer "init" (void @-> returning bool) () in
  if initialised then () else
  if init () then LLError |> raise

let cancel () = funer "cancel" (void @-> returning void) ()

let handle_input ({oc; initialised; ic} as state) request =
  try match request with
    | Heartbeat -> send oc Ack; state
    | Init -> init initialised; {initialised = true; ic; oc}
    | _ -> send oc (Failure "handle"); state
  with Failure _ -> send oc Exc; state
     | exc -> send oc Exc; raise exc

let rec worker ({ic; oc; _} as state) =
  let request = recvb ic in
  handle_input state request
|> worker


let handle ic oc =
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)
  worker {initialised = false; ic; oc}

let start
    ?(f = handle)
    ?(addr = ADDR_INET (inet_addr_any, 1234))
    () =
  Unix.establish_server f addr

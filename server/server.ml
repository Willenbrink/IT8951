open Unix
open Protocol
open Ctypes
open Foreign

let libIT = Dl.dlopen ~flags:[Dl.RTLD_LAZY] ~filename:("/home/pi/IT8951/IT8951")

let funer name params = foreign ~from:libIT ~release_runtime_lock:false name params
let vv = void @-> returning void

type state = {conn : connection; initialised : bool;}

let ready = funer "LCDWaitForReady" (void @-> returning bool)

let init () =
  let init () = funer "init" (void @-> returning bool) () in
  if init () then Fail else Success

let exam1 () =
  let f = funer "example1" vv in
  f ()

let exam2 () =
  let f = funer "example2" vv in
  f ()

let exam3 () =
  let f = funer "example3" vv in
  f ()

let cancel () = funer "cancel" (void @-> returning void) ()

let handle_input ({conn; initialised} as state) request =
  try match request with
    | Heartbeat -> Ack
    | Init -> if not initialised then init () else Success
    | Exam1 -> exam1 (); Success
    | Exam2 -> exam2 (); Success
    | Exam3 -> exam3 (); Success
    | Cancel -> cancel (); Success
    | _ -> Ack
  with
  | Dl.DL_error str -> Exc str
  | _ -> Fail

let rec worker ({conn; _} as state) =
  let state = match react conn (handle_input state) with
    | Init, Success -> {conn; initialised = true}
    | Init, Fail -> {conn; initialised = false}
    | _ -> state
  in
  worker state


let handle ic oc =
  (*Sys.(set_signal sigsegv (Signal_handle(fun _ -> send oc (Exc "segfault"))));
  let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)
  worker { conn = {ic; oc}; initialised = false}

let rec waitpid_non_intr pid =
  try waitpid [] pid
with Unix_error (EINTR, _, _) -> waitpid_non_intr pid

let rec accept_non_intr s =
  try accept s
with Unix_error (EINTR, _, _) -> accept_non_intr s

let establish_server ?(do_fork=true) server_fun sockaddr =
  let sock = socket (domain_of_sockaddr sockaddr) SOCK_STREAM 0 in
  let call_fun s =
    let inchan = in_channel_of_descr s in
    let outchan = out_channel_of_descr s in
    server_fun inchan outchan;
    exit 0
  in

  setsockopt sock SO_REUSEADDR true;
  bind sock sockaddr;
  listen sock 5;
  while true do
    let (s, _caller) = accept_non_intr sock in
    (* The "double fork" trick, the process which calls server_fun will not
       leave a zombie process *)
    if do_fork then
    match fork () with
      | 0 -> if fork() <> 0 then exit 0; (* The son exits, the grandson works *)
        close sock;
        call_fun s
      | id -> close s; ignore(waitpid_non_intr id) (* Reclaim the son *)
    else
      call_fun s
  done

let start
    ?(f = handle)
    ?(addr = ADDR_INET (inet_addr_any, 1234))
    () =
  establish_server ~do_fork:false f addr

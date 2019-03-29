open Unix
open Protocol
open Interface

type state = {conn : connection; initialised : bool;}

let handle_input ({conn; initialised} as state) request =
  try match request with
    | Heartbeat -> Ack
    | Init -> if not initialised then init () else Fail
    | Clear -> clear (); Success
    | Plot (x,y) -> plot x y; Success
    | Display -> display (); Success
    | Color_point (x,y) -> Color (point_color x y)
    | Line (x1,y1,x2,y2) -> set_color foreground; plot x1 y1; plot x2 y2; Success
    | Color_set c -> set_color c; Success
    | Exam1 -> exam1 (); Success
    | Exam2 -> exam2 (); Success
    | Exam3 -> exam3 (); Success
    | Free -> free (); Success
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

let handle ic oc = worker { conn = {ic; oc}; initialised = false}

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

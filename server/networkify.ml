open Unix
open Protocol

let rec handle ic oc =
  react {ic; oc};
  handle ic oc

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
    ?(addr = ADDR_INET (inet_addr_any, 1234))
    () =
  establish_server ~do_fork:false handle addr

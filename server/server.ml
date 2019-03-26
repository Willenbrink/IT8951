open Unix
open Protocol

let handle_input ic oc request =
  match request with
  | None -> ()
  | Some request -> match request with
    | Init -> send oc Ack
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

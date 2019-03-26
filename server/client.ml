open Protocol

let init addr = Unix.open_connection addr

let start () =
  let ic,oc = init (Unix.ADDR_INET(Unix.inet_addr_any, 1234)) in
  let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in

  let rec worker () =
    send oc Init;
    Printf.fprintf stderr "Response:\n";
    let resp = recv ic in
    (match resp with
    | Some Ack -> Printf.fprintf stderr "Success\n"
    | Some Fail | None -> Printf.fprintf stderr "Failure\n");
    flush stderr;
    Unix.sleepf 0.1;
    worker ()
  in
  worker ()

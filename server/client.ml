open Protocol

let init addr = Unix.open_connection addr

type state = {ic : in_channel; oc: out_channel; conn : bool}

let rec worker ({ic; oc; conn} as state) =
  Unix.sleep 1;
  send oc Heartbeat;
  Printf.fprintf stderr "Response:\n";
  let resp = recvb ic in
  (match resp with
   | Ack -> Printf.fprintf stderr "Success\n"
   | Fail -> Printf.fprintf stderr "Failure\n");
  flush stderr;
  worker state


let start () =
  let ic,oc = init (Unix.ADDR_INET(Unix.inet_addr_any, 1234)) in
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)

  try
    worker {ic; oc; conn = false}
  with exc -> Printf.fprintf stderr "Exception!"; raise exc
